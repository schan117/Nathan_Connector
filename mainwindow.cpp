#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);;



    this->setWindowTitle(tr("Connector Inspection Terminal"));

        learn_front_color_on_next_capture = false;
        learn_back_color_on_next_capture = false;
        front_inspection_count = 0;
        front_cycle_count;
        actual_count = 0;
        ready_and_do_data_logging = false;
        front_consecutive_fail = 0;
        back_consecutive_fail = 0;
        front_prev_result = true;
        back_prev_result = true;
        product_count = 0;
        bypass_result = false;
        front_fail_alarmed = false;
        back_fail_alarmed  = false;

        qDebug() << "Load Product Settings returns:" << Load_Settings();

        Connect_Signals();

        if (use_labjack)
        {
            if (lj.Initialize_LabJack())
            {
                qDebug("Labjack opened!");

                if (lj.Load_Settings())
                {
                    qDebug("Labjack settings loaded properly");
                    lj_thread.Initialize(&lj);
                    labjack_ok = true;
                }
                else
                {
                    qDebug("Fail to load labjack settings!");
                    labjack_ok = false;
                    this->setEnabled(false);
                }

            }
            else
            {
                qDebug("Fail to open Lackjack!");
                this->setEnabled(false);
                labjack_ok = false;
            }
        }
        else
        {
            qDebug() << "Not using labjack!";
        }

        if (Initialize_Vimba_System())
        {
            // only start cameras if vimba system is initialized properly

            for (int i=0; i<vw.cameras_to_open; i++)
            {
                if (vw.found_list[i] == 1)
                {
                    vw.Start_Acquisition(i);
                }
            }

            camera_thread.Initialize(&vw);

            bool load_ok = false;

            for (int i=0; i<product_count; i++)
            {
                // load in front camera settings
                if (product_type[i].use_front)
                {
                    if (measurement.Load_Settings(2*i, product_type[i].vision_ini_front))
                    {
                        qDebug() << "Load in parameters successful for product:" <<  product_type[i].vision_ini_front;
                        load_ok = true;
                    }
                    else
                    {
                        qDebug() << "Load in parameters NOT successful for product:" << product_type[i].vision_ini_front;
                        load_ok = false;
                        break;
                    }
                }

                // load in back camera settings
                if (product_type[i].use_back)
                {
                    if (measurement.Load_Settings(2*i + 1, product_type[i].vision_ini_back))
                    {
                        qDebug() << "Load in parameters successful for product:" << product_type[i].vision_ini_back;
                        load_ok = true;
                    }
                    else
                    {
                        qDebug() << "Load in parameters NOT successful for product:" << product_type[i].vision_ini_back;
                        load_ok = false;
                        break;
                    }
                }
            }

            // load in default meaurement settings for front
            if (load_ok)
            {
                // check if both front and back camera are found
                if (vw.cameras_opened == 2)
                {


                }

                // fill in reference color informations
                Display_Settings(0);


            }
            else
            {
                this->setEnabled(false);
            }
        }
        else
        {
            this->setEnabled(false);
        }


        this->showMaximized();

        if (labjack_ok)
        {
            qDebug("Start labjack thread!");
            lj_thread.start();
        }


}

MainWindow::~MainWindow()
{

    qDebug("Request to destruct class MainWindow");

    // check if camera_thread is running, if so, stop thread properly
    if (camera_thread.isRunning())
    {
        camera_thread.keep_capturing = false;
    }

    while (camera_thread.isRunning())
    {
        QTest::qSleep(50);
    }

    if (lj_thread.isRunning())
    {
        lj_thread.keep_running = false;
    }

    while (lj_thread.isRunning())
    {
        QTest::qSleep(50);
    }


    for (int i=0; i<vw.cameras_to_open; i++)
    {
        if (vw.found_list[i] == 1)
        {
            vw.Stop_Acquisition(i);
        }
    }

    delete [] product_type;
    delete ui;
}

void MainWindow::Connect_Signals()
{
    connect(&camera_thread, SIGNAL(FrameReceived(int)), this, SLOT(On_Frame_Received(int)));
    connect(ui->Start, SIGNAL(clicked()), this, SLOT(On_Start()));
    connect(ui->Stop, SIGNAL(clicked()), this, SLOT(On_Stop()));
    connect(ui->Front_Learn, SIGNAL(clicked()), this, SLOT(On_Front_Learn()));
    connect(ui->Back_Learn, SIGNAL(clicked()), this, SLOT(On_Back_Learn()));
    connect(ui->actionDelay_Inspection, SIGNAL(triggered()), this, SLOT(On_Delay_State_Changed()));
    connect(ui->actionStart_Data_Logging, SIGNAL(toggled(bool)), this, SLOT(On_Action_Start_Data_Logging(bool)));

    connect(ui->Front_Threshold, SIGNAL(valueChanged(int)), this, SLOT(On_Front_Locator_Threshold_Changed(int)));
    connect(ui->Front_Threshold, SIGNAL(sliderReleased()), this, SLOT(On_Front_Locator_Threshold_Finished()));
    connect(ui->Back_Threshold, SIGNAL(valueChanged(int)), this, SLOT(On_Back_Locator_Threshold_Changed(int)));
    connect(ui->Back_Threshold, SIGNAL(sliderReleased()), this, SLOT(On_Back_Locator_Threshold_Finished()));

    connect(ui->Camera_Shutter_Front, SIGNAL(valueChanged(int)), this, SLOT(On_Front_Camera_Shutter_Changed(int)));
    connect(ui->Camera_Shutter_Back, SIGNAL(valueChanged(int)), this, SLOT(On_Back_Camera_Shutter_Changed(int)));

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(On_Action_Quit()));
    connect(ui->actionSet_Output_Folder, SIGNAL(triggered()), this, SLOT(On_Action_Select_Output_Folder()));

    connect(&lj_thread, SIGNAL(Counter0Read(long)), this, SLOT(On_Request_Counter_Update(long)));
    connect(&lj_thread, SIGNAL(GreenButtonTriggered()), this, SLOT(On_Green_Button_Triggered()));
    connect(&lj_thread, SIGNAL(RedButtonTriggered()), this, SLOT(On_Red_Button_Triggered()));

    connect(ui->actionActual_Counter_Reset, SIGNAL(triggered()), this, SLOT(On_Action_Counter_Reset()));

    connect(ui->Front_Min_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Front_Min_Distance_Changed(double)));
    connect(ui->Front_Max_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Front_Max_Distance_Changed(double)));
    connect(ui->Back_Min_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Back_Min_Distance_Changed(double)));
    connect(ui->Back_Max_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Back_Max_Distance_Changed(double)));

    connect(ui->actionBypass_Results, SIGNAL(triggered()), this, SLOT(On_Action_Bypass_Results()));
    connect(ui->actionClear_Alarm, SIGNAL(triggered()), this, SLOT(On_Action_Clear_Alarm()));

    connect(ui->Product_Type, SIGNAL(currentIndexChanged(int)), this, SLOT(On_Product_Type_Changed(int)));

}

bool MainWindow::Initialize_Vimba_System()
{
    if (!vw.Startup()) return false;

    vw.List_Cameras();

    if (!vw.Load_Settings()) return false;
    if (!vw.Open_Cameras()) return false;

    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int ret;

    if (camera_thread.isRunning())
    {
        ret = QMessageBox::warning(this, tr("Quit"), tr("Stop camera first before quitting."), QMessageBox::Ok, QMessageBox::Ok);
        event->ignore();
        return;
    }

    ret = QMessageBox::question(this, tr("Quit"), tr("Are you sure to quit?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

    if (ret == QMessageBox::Ok)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::On_Frame_Received(int camera_index)
{
    // check which camera triggered the slot

    QString sr;
    QString cr;

    QTime qtime;

    qtime.start();

    int process_time;

    QString current_product = ui->Product_Type->currentText();
    int product_index = ui->Product_Type->findText(current_product);

    // clear result window for every new inspection cycle
     ui->Results->clear();

    if (product_type[product_index].use_front)
    {
        measurement.Set_Image(vw.internal_image_rgb[FRONT_CAM]);
        qtime.restart();
        measurement.Perform_Extraction(product_index * 2);
        process_time = qtime.elapsed();
        Display_Image(ui->View_Front, measurement.result_image.data, measurement.result_image.cols, measurement.result_image.rows);

        if (measurement.result[0] == CAL_OK)
        {
            Display_Front_Inspection_Results(product_index);
            ui->Results->append(tr("Acquisition time: %1 secs, Processing time: %2 secs").arg(camera_thread.capture_time/1000.0).arg(process_time/1000.0));

            Display_Front_Production_Status(product_index);

            if (ready_and_do_data_logging)
            {
                Log_and_Process_Results(product_index * 2);
                ui->Results->append("Data are recorded");
            }
            else
            {
                ui->Results->append(tr("Data recording not performed."));
            }

            // if there was an request to learn color, do it
            if (learn_front_color_on_next_capture)
            {
                measurement.is[product_index * 2].lower_h = measurement.result[1];
                measurement.is[product_index * 2].lower_s = measurement.result[2];
                measurement.is[product_index * 2].upper_h = measurement.result[3];
                measurement.is[product_index * 2].upper_s = measurement.result[4];

                Display_Front_Settings(product_index);

                measurement.Save_Settings(2 * product_index, product_type[product_index].vision_ini_front);
                learn_front_color_on_next_capture = false;
            }
            ////////////////////////////////////////////////
        }
        else
        {
            Display_Front_NA_Inspection_Results();

            if (ready_and_do_data_logging)
            {
                Log_Extraction_Error(product_index * 2);
            }
        }
    }

    if (product_type[product_index].use_back)
    {
        measurement.Set_Image(vw.internal_image_rgb[BACK_CAM]);
        qtime.restart();
        measurement.Perform_Extraction(product_index * 2 + 1);
        process_time = qtime.elapsed();
        Display_Image(ui->View_Back, measurement.result_image.data, measurement.result_image.cols, measurement.result_image.rows);

        if (measurement.result[0] == CAL_OK)
        {
            Display_Back_Inspection_Results(product_index);
            ui->Results->append(tr("Acquisition time: %1 secs, Processing time: %2 secs").arg(camera_thread.capture_time/1000.0).arg(process_time/1000.0));

            Display_Back_Production_Status(product_index);

            if (ready_and_do_data_logging)
            {
                Log_and_Process_Results(product_index * 2 + 1);
                ui->Results->append("Data are recorded");
            }
            else
            {
                ui->Results->append(tr("Data recording not performed."));
            }

            // if there was an request to learn color, do it
            if (learn_back_color_on_next_capture)
            {
                measurement.is[product_index * 2 + 1].lower_h = measurement.result[1];
                measurement.is[product_index * 2 + 1].lower_s = measurement.result[2];
                measurement.is[product_index * 2 + 1].upper_h = measurement.result[3];
                measurement.is[product_index * 2 + 1].upper_s = measurement.result[4];

                Display_Back_Settings(product_index);

                measurement.Save_Settings(2 * product_index + 1, product_type[product_index].vision_ini_back);
                learn_back_color_on_next_capture = false;
            }
            ////////////////////////////////////////////////
        }
        else
        {
            Display_Back_NA_Inspection_Results();

            if (ready_and_do_data_logging)
            {
                Log_Extraction_Error(product_index * 2 + 1);
            }
        }
    }


    /// tell camera thread that processing has finished
    camera_thread.mutex.unlock();
}

void MainWindow::Display_Front_Inspection_Results(int index)
{
        QString ur;
        QString lr;
        QString dis;

    // clear result textbox;

        Display_Image(ui->H_Front, measurement.front_roi_h.data, measurement.front_roi_h.cols, measurement.front_roi_h.rows);
        Display_Image(ui->S_Front, measurement.front_roi_s.data, measurement.front_roi_s.cols, measurement.front_roi_s.rows);
        Display_Image(ui->Color_Front, measurement.front_roi_color.data, measurement.front_roi_color.cols, measurement.front_roi_color.rows);

        // display current readings
        lr = QString("%1, %2").arg(measurement.result[1]).arg(measurement.result[2]);
        ur = QString("%1, %2").arg(measurement.result[3]).arg(measurement.result[4]);

        if (measurement.result[5] > 0.35)
        {
            dis = QString(tr("%1 - Good")).arg(measurement.result[5]);
        } else if (measurement.result[5] > 0.2)
        {
            dis = QString(tr("%1 - OK")).arg(measurement.result[5]);
        } else
        {
            dis = QString(tr("%1 - Bad")).arg(measurement.result[5]);
        }

        ui->Upper_Reading_Front->setText(ur);
        ui->Lower_Reading_Front->setText(lr);
        ui->Current_Discrepancy_Front->setText(dis);

        // Display results

        QString result_string;

        result_string = QString(tr("Front Cycle: %1")).arg(front_cycle_count);
        ui->Results->append(result_string);
        result_string = QString(tr("Samples Inspected: %1")).arg(front_inspection_count);
        ui->Results->append(result_string);

        for (int i=0; i<measurement.result[6]; i++)
        {
            result_string = QString(tr("Sample %1: %2 mm")).arg(i).arg(measurement.result[i+7]);
            ui->Results->append(result_string);
        }

        double avg = measurement.result[7+(int) measurement.result[6]];

        result_string = QString(tr("--------------------\nAverage: %1 mm")).arg(avg);

        if (avg < measurement.is[index * 2].min_distance)
        {
            result_string = result_string + ", Min: " + QString("%1").arg(measurement.is[index * 2].min_distance) + " mm,  Max: " + QString("%1").arg(measurement.is[index * 2].max_distance) + ", FAIL";


        }
        else
        {
            result_string = result_string + ", Min: " + QString("%1").arg(measurement.is[index * 2].min_distance) + " mm,  Max: " + QString("%1").arg(measurement.is[index * 2].max_distance) + ", PASS";
        }

        ui->Results->append(result_string);

        front_inspection_count = front_inspection_count + measurement.result[6];
        front_cycle_count++;

    //////////////////////////////////////////////////////////////
}

void MainWindow::Display_Back_Inspection_Results(int index)
{
        QString ur;
        QString lr;
        QString dis;

    // clear result textbox;

        Display_Image(ui->H_Back, measurement.front_roi_h.data, measurement.front_roi_h.cols, measurement.front_roi_h.rows);
        Display_Image(ui->S_Back, measurement.front_roi_s.data, measurement.front_roi_s.cols, measurement.front_roi_s.rows);
        Display_Image(ui->Color_Back, measurement.front_roi_color.data, measurement.front_roi_color.cols, measurement.front_roi_color.rows);;

        // display current readings
        lr = QString("%1, %2").arg(measurement.result[1]).arg(measurement.result[2]);
        ur = QString("%1, %2").arg(measurement.result[3]).arg(measurement.result[4]);

        if (measurement.result[5] > 0.35)
        {
            dis = QString(tr("%1 - Good")).arg(measurement.result[5]);
        } else if (measurement.result[5] > 0.2)
        {
            dis = QString(tr("%1 - OK")).arg(measurement.result[5]);
        } else
        {
            dis = QString(tr("%1 - Bad")).arg(measurement.result[5]);
        }

        ui->Upper_Reading_Back->setText(ur);
        ui->Lower_Reading_Back->setText(lr);
        ui->Current_Discrepancy_Back->setText(dis);

        // Display results

        QString result_string;

        result_string = QString(tr("Back Cycle: %1")).arg(back_cycle_count);
        ui->Results->append(result_string);
        result_string = QString(tr("Samples Inspected: %1")).arg(back_inspection_count);
        ui->Results->append(result_string);

        for (int i=0; i<measurement.result[6]; i++)
        {
            result_string = QString(tr("Sample %1: %2 mm")).arg(i).arg(measurement.result[i+7]);
            ui->Results->append(result_string);
        }

        double avg = measurement.result[7+(int) measurement.result[6]];

        result_string = QString(tr("--------------------\nAverage: %1 mm")).arg(avg);

        if (avg < measurement.is[index * 2 + 1].min_distance || avg > measurement.is[index * 2 + 1].max_distance)
        {
            result_string = result_string + ", Min: " + QString("%1").arg(measurement.is[index * 2 + 1].min_distance) + " mm, Max: " + QString("%1").arg(measurement.is[index * 2 + 1].max_distance) + ", FAIL";
        }
        else
        {
            result_string = result_string + ", Min: " + QString("%1").arg(measurement.is[index * 2 + 1].min_distance) + " mm,  Max: " + QString("%1").arg(measurement.is[index * 2 + 1].max_distance) + ", PASS";
        }

        ui->Results->append(result_string);

        back_inspection_count = back_inspection_count + measurement.result[6];
        back_cycle_count++;

    //////////////////////////////////////////////////////////////
}

void MainWindow::Display_Front_NA_Inspection_Results()
{
    if (measurement.result[0] == NO_LOCATOR)
    {
        ui->Results->append(tr("Front Error #1: No locator found! Check locator sensitivity settings."));
    }
    else if (measurement.result[0] == LOCATOR_INVALID)
    {
        ui->Results->append(tr("Front Error #2: Locator all invalid! Check locator sensitivity settings."));
    }
    else if (measurement.result[0] == ROI_INVALID)
    {
        ui->Results->append(tr("Front Error #3: Roi's of of screen."));
    }

    ui->H_Front->setPixmap(QPixmap());
    ui->S_Front->setPixmap(QPixmap());
    ui->Color_Front->setPixmap(QPixmap());

    // display current readings
    QString cr = QString("H: %1, S: %2").arg("N/A").arg("N/A");
    QString sr = QString("H: %1, S: %2").arg("N/A").arg("N/A");
    QString dis = QString("%1").arg("N/A");

    ui->Upper_Reading_Front->setText(sr);
    ui->Lower_Reading_Front->setText(cr);
    ui->Current_Discrepancy_Front->setText(dis);
    //////////////////////////////////////////////////////////////

    ui->Front_Result->setStyleSheet("background: white");
    ui->Front_Result->setText("N/A");
}

void MainWindow::Display_Back_NA_Inspection_Results()
{
    if (measurement.result[0] == NO_LOCATOR)
    {
        ui->Results->append(tr("Back Error #1: No locator found! Check locator sensitivity settings."));
    }
    else if (measurement.result[0] == LOCATOR_INVALID)
    {
        ui->Results->append(tr("Back Error #2: Locator all invalid! Check locator sensitivity settings."));
    }
    else if (measurement.result[0] == ROI_INVALID)
    {
        ui->Results->append(tr("Back Error #3: Roi's of of screen."));
    }

    ui->H_Back->setPixmap(QPixmap());
    ui->S_Back->setPixmap(QPixmap());
    ui->Color_Back->setPixmap(QPixmap());

    // display current readings
    QString cr = QString("H: %1, S: %2").arg("N/A").arg("N/A");
    QString sr = QString("H: %1, S: %2").arg("N/A").arg("N/A");
    QString dis = QString("%1").arg("N/A");

    ui->Upper_Reading_Back->setText(sr);
    ui->Lower_Reading_Back->setText(cr);
    ui->Current_Discrepancy_Back->setText(dis);
    //////////////////////////////////////////////////////////////

    ui->Back_Result->setStyleSheet("background: white");
    ui->Back_Result->setText("N/A");
}

void MainWindow::Display_Image(QLabel* view, uchar* data, int width, int height)
{
    QImage qi(data, width, height, QImage::Format_RGB888);

    QPixmap qp = QPixmap::fromImage(qi).scaled(QSize(view->width(),view->height()), Qt::IgnoreAspectRatio);

    /*
    QPainter qpaint(&qp);
    qpaint.setBrush(Qt::SolidPattern);
    QPen qpen(QColor(255,0,0,100));
    qpen.setWidth(1);
    qpaint.setPen(qpen);

    qpaint.drawLine(0, View->height()/2, View->width(), View->height()/2);
    */



    view->setPixmap(qp);



}

void MainWindow::On_Product_Type_Changed(int index)
{    
    if (product_type[index].use_front)
    {
        measurement.Load_Settings(index * 2, product_type[index].vision_ini_front);
    }

    if (product_type[index].use_back)
    {
        measurement.Load_Settings(index * 2 + 1, product_type[index].vision_ini_back);
    }

    // disconnect some signals for a while to prvent problems
    disconnect(ui->Front_Threshold, SIGNAL(sliderReleased()), this, SLOT(On_Front_Locator_Threshold_Finished()));
    disconnect(ui->Front_Threshold, SIGNAL(valueChanged(int)), this, SLOT(On_Front_Locator_Threshold_Changed(int)));
    disconnect(ui->Front_Min_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Front_Min_Distance_Changed(double)));
    disconnect(ui->Front_Max_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Front_Max_Distance_Changed(double)));
    disconnect(ui->Camera_Shutter_Front, SIGNAL(valueChanged(int)), this, SLOT(On_Front_Camera_Shutter_Changed(int)));

    disconnect(ui->Back_Threshold, SIGNAL(sliderReleased()), this, SLOT(On_Back_Locator_Threshold_Finished()));
    disconnect(ui->Back_Threshold, SIGNAL(valueChanged(int)), this, SLOT(On_Back_Locator_Threshold_Changed(int)));
    disconnect(ui->Back_Min_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Back_Min_Distance_Changed(double)));
    disconnect(ui->Back_Max_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Back_Max_Distance_Changed(double)));
    disconnect(ui->Camera_Shutter_Back, SIGNAL(valueChanged(int)), this, SLOT(On_Back_Camera_Shutter_Changed(int)));

    Display_Settings(index);

    connect(ui->Front_Threshold, SIGNAL(sliderReleased()), this, SLOT(On_Front_Locator_Threshold_Finished()));
    connect(ui->Front_Threshold, SIGNAL(valueChanged(int)), this, SLOT(On_Front_Locator_Threshold_Changed(int)));
    connect(ui->Front_Min_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Front_Min_Distance_Changed(double)));
    connect(ui->Front_Max_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Front_Max_Distance_Changed(double)));
    connect(ui->Camera_Shutter_Front, SIGNAL(valueChanged(int)), this, SLOT(On_Front_Camera_Shutter_Changed(int)));

    connect(ui->Back_Threshold, SIGNAL(sliderReleased()), this, SLOT(On_Back_Locator_Threshold_Finished()));
    connect(ui->Back_Threshold, SIGNAL(valueChanged(int)), this, SLOT(On_Back_Locator_Threshold_Changed(int)));
    connect(ui->Back_Min_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Back_Min_Distance_Changed(double)));
    connect(ui->Back_Max_Distance, SIGNAL(valueChanged(double)), this, SLOT(On_Back_Max_Distance_Changed(double)));
    connect(ui->Camera_Shutter_Back, SIGNAL(valueChanged(int)), this, SLOT(On_Back_Camera_Shutter_Changed(int)));
}

bool MainWindow::Load_Settings()
{
    bool ok;

    QSettings set("settings/settings.ini", QSettings::IniFormat);

    product_count = set.value("Main/Product_Count").toInt(&ok);
    if (!ok) return false;

    use_labjack = set.value("Main/Use_Labjack").toInt(&ok);
    if (!ok) return false;

    product_type = new Product_Type[product_count];

    for (int i=0; i<product_count; i++)
    {\
        product_type[i].index = i;
        product_type[i].name = set.value(QString("Product%1/Name").arg(i)).toString();
        ui->Product_Type->addItem(product_type[i].name);

        product_type[i].use_front = set.value(QString("Product%1/Use_Front").arg(i)).toInt(&ok);
        if (!ok) return false;
        product_type[i].use_back = set.value(QString("Product%1/Use_Back").arg(i)).toInt(&ok);
        if (!ok) return false;

        product_type[i].vision_ini_front = set.value(QString("Product%1/Vision_Ini_Name_Front").arg(i)).toString();
        product_type[i].vision_ini_back = set.value(QString("Product%1/Vision_Ini_Name_Back").arg(i)).toString();
    }

    output_folder = set.value("Main/Output_Folder").toString();

    return true;
}

void MainWindow::On_Start()
{
    // Check if there are any cameras being enabled at all


    if (!camera_thread.isRunning())
    {
        qDebug("Start front capture!");

        Disable_Manual_Controls();

        // Load in settings everytime to make changes in ini file easier to reflect

        QString product_name = ui->Product_Type->currentText();
        int product_index = ui->Product_Type->findText(product_name);

        camera_thread.do_back_camera = false;
        camera_thread.do_front_camera = false;

        if (product_type[product_index].use_front)
        {
            measurement.Load_Settings(product_index * 2, product_type[product_index].vision_ini_front);
            camera_thread.do_front_camera = true;

            // update camera shutter to make sure images are taken wth correct shutter

            int shutter = measurement.is[product_index * 2].camera_shutter;
            int gain = measurement.is[product_index*2].camera_gain;
            qDebug() << "Set Camera Exposure:" << vw.Set_Exposure(FRONT_CAM, shutter);
            qDebug() << "Set Camera Gain:" << vw.Set_Gain(FRONT_CAM, gain);
            qDebug() << "Set Image Format:" << vw.Set_Image_Format(FRONT_CAM, "RGB8Packed");

        }

        if (product_type[product_index].use_back)
        {
            measurement.Load_Settings(product_index * 2 + 1, product_type[product_index].vision_ini_back);
            camera_thread.do_back_camera = true;

            int shutter = measurement.is[product_index * 2 + 1].camera_shutter;
            int gain = measurement.is[product_index*2 + 1].camera_gain;
            qDebug() << "Set Camera Exposure:" << vw.Set_Exposure(BACK_CAM, shutter);
            qDebug() << "Set Camera Gain:" <<vw.Set_Gain(BACK_CAM, gain);
            qDebug() << "Set Image Format:" <<vw.Set_Image_Format(BACK_CAM, "RGB8Packed");
        }


        // result number of inspections done so far
        front_inspection_count = 0;
        back_inspection_count = 0;
        front_cycle_count = 0;
        back_cycle_count = 0;

        camera_thread.start();

        Start_LJ();
    }
}

void MainWindow::Start_LJ()
{
    lj_thread.do_counter_read = true;
    lj_thread.do_green_button = true;
    lj_thread.do_red_button = true;
}

void MainWindow::Stop_LJ()
{
    lj_thread.do_counter_read = false;
    lj_thread.do_green_button = false;
    lj_thread.do_red_button = false;
}

void MainWindow::On_Stop()
{
    if (camera_thread.isRunning())
    {
        qDebug("Stop front capture!");
        camera_thread.keep_capturing = false;
        Enable_Manual_Controls();

        // reset fail accumulation on stop camera
        front_consecutive_fail = 0;
        front_prev_result = true;
        back_consecutive_fail = 0;
        back_prev_result = true;

        Stop_LJ();

        ui->actionStart_Data_Logging->setChecked(false);
    }

}

void MainWindow::Display_Front_Settings(int index)
{
    QString q_upper = QString("%1, %2").arg(measurement.is[2 * index].upper_h).arg(measurement.is[2 * index].upper_s);
    QString q_lower  = QString("%1, %2").arg(measurement.is[2 * index].lower_h).arg(measurement.is[2 * index].lower_s);

    ui->Upper_Reference_Front->setText(q_upper);
    ui->Lower_Reference_Front->setText(q_lower);

    ui->Front_Threshold->setValue(measurement.is[index * 2].locator_threshold);
    ui->Front_Min_Distance->setValue(measurement.is[index * 2].min_distance);
    ui->Front_Max_Distance->setValue(measurement.is[index * 2].max_distance);

    ui->Camera_Shutter_Front->setValue(measurement.is[index * 2].camera_shutter);

}

void MainWindow::Display_Back_Settings(int index)
{
    QString q_upper = QString("%1, %2").arg(measurement.is[2 * index + 1].upper_h).arg(measurement.is[2 * index + 1].upper_s);
    QString q_lower = QString("%1, %2").arg(measurement.is[2 * index + 1].lower_h).arg(measurement.is[2 * index + 1].lower_s);

    ui->Upper_Reference_Back->setText(q_upper);
    ui->Lower_Reference_Back->setText(q_lower);

    ui->Back_Threshold->setValue(measurement.is[2 * index + 1].locator_threshold);
    ui->Back_Min_Distance->setValue(measurement.is[2 * index + 1].min_distance);
    ui->Back_Max_Distance->setValue(measurement.is[2 * index + 1].max_distance);

    ui->Camera_Shutter_Back->setValue(measurement.is[2 * index + 1].camera_shutter);

}

void MainWindow::Display_Settings(int product_index)
{
    if (product_type[product_index].use_front)
    {
        Display_Front_Settings(product_index);
        Enable_Front_Camera_Controls();
    }
    else
    {
        Disable_Front_Camera_Controls();
    }

    if (product_type[product_index].use_back)
    {
        Display_Back_Settings(product_index);
        Enable_Back_Camera_Controls();
    }
    else
    {
        Disable_Back_Camera_Controls();
    }

}

void MainWindow::Disable_Manual_Controls()
{
    ui->Product_Type->setEnabled(false);
}

void MainWindow::Enable_Manual_Controls()
{
    ui->Product_Type->setEnabled(true);
}

void MainWindow::On_Front_Learn()
{
    if (camera_thread.isRunning())
    {
        int ret = QMessageBox::information(this, tr("Learn colors"), tr("Press OK to learn Hue and Saturation values for upper and lower colors for front camera."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

        if (ret == QMessageBox::Ok)
        {
            learn_front_color_on_next_capture = true;
        }
    }

}

void MainWindow::On_Back_Learn()
{
    if (camera_thread.isRunning())
    {
        int ret = QMessageBox::information(this, tr("Learn colors"), tr("Press OK to learn Hue and Saturation values for upper and lower colors for back camera."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

        if (ret == QMessageBox::Ok)
        {
            learn_back_color_on_next_capture = true;
        }
    }
}

void MainWindow::Disable_Back_Camera_Controls()
{
    ui->View_Back->setVisible(false);
    ui->Color_Back->setVisible(false);
    ui->H_Back->setVisible(false);
    ui->S_Back->setVisible(false);

    ui->Back_Controls->setVisible(false);
}

void MainWindow::Enable_Back_Camera_Controls()
{
    ui->View_Back->setVisible(true);
    ui->Color_Back->setVisible(true);
    ui->H_Back->setVisible(true);
    ui->S_Back->setVisible(true);

    ui->Back_Controls->setVisible(true);

}

void MainWindow::Disable_Front_Camera_Controls()
{
    ui->View_Front->setVisible(false);
    ui->Color_Front->setVisible(false);
    ui->H_Front->setVisible(false);
    ui->S_Front->setVisible(false);

    ui->Front_Controls->setVisible(false);
}

void MainWindow::Enable_Front_Camera_Controls()
{
    ui->View_Front->setVisible(true);
    ui->Color_Front->setVisible(true);
    ui->H_Front->setVisible(true);
    ui->S_Front->setVisible(true);

    ui->Front_Controls->setVisible(true);

}

void MainWindow::On_Delay_State_Changed()
{
    if (ui->actionDelay_Inspection->isChecked())
    {
        camera_thread.do_delay = true;
    }
    else
    {
        camera_thread.do_delay = false;
    }
}

void MainWindow::On_Front_Locator_Threshold_Changed(int value)
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    measurement.is[current_index * 2].locator_threshold = value;
}

void MainWindow::On_Front_Locator_Threshold_Finished()
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    measurement.Save_Settings(current_index * 2, product_type[current_index].vision_ini_front);
}

void MainWindow::On_Back_Locator_Threshold_Changed(int value)
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    measurement.is[current_index * 2 + 1].locator_threshold = value;
}

void MainWindow::On_Back_Locator_Threshold_Finished()
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    measurement.Save_Settings(current_index * 2 + 1, product_type[current_index].vision_ini_back);
}

void MainWindow::On_Action_Quit()
{
    this->close();
}

void MainWindow::On_Action_Select_Output_Folder()
{
    QString ret = QFileDialog::getExistingDirectory(this, tr("Select output folder"));

    output_folder = ret;

    QSettings set("settings/settings.ini", QSettings::IniFormat);

    set.setValue("Main/Output_Folder", output_folder);

}

bool MainWindow::Register_Lot_Number()
{

    lot_number = QDateTime::currentDateTime().toString("[dd.MM.yyyy, hh.mm.ss.zzz]");

    QDir qdir(output_folder);

    if (qdir.mkdir(lot_number) || qdir.exists(lot_number))
    {
        //QMessageBox::information(this, tr("Register"), QString(tr("Lot %1 registered at %2.")).arg(lot_number).arg(output_folder), QMessageBox::Ok, QMessageBox::Ok);
        return true;
    }
    else
    {
        return false;
    }


}

void MainWindow::Display_Front_Production_Status(int index)
{
    double avg = measurement.result[7+(int) measurement.result[6]];
    double min_distance = measurement.is[index * 2].min_distance;
    double max_distance = measurement.is[index * 2].max_distance;

    if (!bypass_result)
    {
        if (avg <= min_distance || avg >= max_distance)
        {
            if (front_prev_result == false)
            {
                // only increment fail counter if system is ready
                if (ready_and_do_data_logging)
                {
                    front_consecutive_fail++;
                }
            }

            if (ready_and_do_data_logging)
            {
                ui->Front_Result->setStyleSheet("background: red");
            }
            else
            {
                ui->Front_Result->setStyleSheet("background: white");
            }

            if (front_consecutive_fail > measurement.is[index * 2].fail_threshold)
            {
                ui->Front_Result->setText(QString(tr("Fail - %1/%2, Alarm!")).arg(front_consecutive_fail).arg(measurement.is[index * 2].fail_threshold));
                front_fail_alarmed = true;

                // Send signal out frrom labjack

                lj.Set_Front_Fail_Status(1);

            }
            else
            {
                ui->Front_Result->setText(QString(tr("Fail - %1/%2")).arg(front_consecutive_fail).arg(measurement.is[index * 2].fail_threshold));
            }

            front_prev_result = false;
        }
        else
        {

            // if there is a pass, reset consecutive fail
            // also need to check if this pass is immediately after a fail alarm if so, notify the user as well

            if (ready_and_do_data_logging)
            {
                if (front_fail_alarmed)
                {
                    ui->Front_Result->setStyleSheet("background: yellow");
                }
                else
                {
                    ui->Front_Result->setStyleSheet("background: green");
                }
            }
            else
            {
                ui->Front_Result->setStyleSheet("background: white");
            }


            if (front_fail_alarmed)
            {
                ui->Front_Result->setText(tr("Pass - Alarm Triggered"));
                front_prev_result = true;
                front_consecutive_fail = 0;
            }
            else
            {
                ui->Front_Result->setText(tr("Pass"));
                front_prev_result = true;
                front_consecutive_fail = 0;
            }
        }
    }
    else
    {
        // ignore inspection results
        ui->Front_Result->setText(QString(tr("Bypass")));
        ui->Front_Result->setStyleSheet("background: blue");
        front_consecutive_fail = 0;
        front_prev_result = true;
    }
}

void MainWindow::Display_Back_Production_Status(int index)
{
    double avg = measurement.result[7+(int) measurement.result[6]];
    double min_distance = measurement.is[index * 2 + 1].min_distance;
    double max_distance = measurement.is[index * 2 + 1].max_distance;

    if (!bypass_result)
    {
        if (avg <= min_distance || avg >= max_distance)
        {
            if (back_prev_result == false)
            {
                // only increment fail counter if system is ready
                if (ready_and_do_data_logging)
                {
                    back_consecutive_fail++;
                }
            }

            if (ready_and_do_data_logging)
            {
                ui->Back_Result->setStyleSheet("background: red");
            }
            else
            {
                ui->Back_Result->setStyleSheet("background: white");
            }

            if (back_consecutive_fail > measurement.is[index * 2 + 1].fail_threshold)
            {
                ui->Back_Result->setText(QString(tr("Fail - %1/%2, Alarm!")).arg(back_consecutive_fail).arg(measurement.is[index * 2 + 1].fail_threshold));
                back_fail_alarmed = true;

                // Send signal out frrom labjack
                lj.Set_Back_Fail_Status(1);

            }
            else
            {
                ui->Back_Result->setText(QString(tr("Fail - %1/%2")).arg(back_consecutive_fail).arg(measurement.is[index * 2 + 1].fail_threshold));
            }

            back_prev_result = false;
        }
        else
        {

            // if there is a pass, reset consecutive fail
            // also need to check if this pass is immediately after a fail alarm if so, notify the user as well

            if (ready_and_do_data_logging)
            {
                if (back_fail_alarmed)
                {
                    ui->Back_Result->setStyleSheet("background: yellow");
                }
                else
                {
                    ui->Back_Result->setStyleSheet("background: green");
                }
            }
            else
            {
                ui->Back_Result->setStyleSheet("background: white");
            }


            if (back_fail_alarmed)
            {
                ui->Back_Result->setText(tr("Pass - Alarm Triggered"));
                back_prev_result = true;
                back_consecutive_fail = 0;
            }
            else
            {
                ui->Back_Result->setText(tr("Pass"));
                back_prev_result = true;
                back_consecutive_fail = 0;
            }
        }
    }
    else
    {
        // ignore inspection results
        ui->Back_Result->setText(QString(tr("Bypass")));
        ui->Back_Result->setStyleSheet("background: blue");
        back_consecutive_fail = 0;
        back_prev_result = true;
    }
}

void MainWindow::Log_and_Process_Results(int calculated_index)
{

    // add both front and back logging here

    output_file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);

    output_stream.setDevice(&output_file);

    QDateTime qdt = QDateTime::currentDateTime();

    QString date_string = qdt.toString("[dd/MM/yyyy, hh:mm:ss:zzz]");

    double avg = measurement.result[7+(int) measurement.result[6]];
    double min_distance = measurement.is[calculated_index].min_distance;
    double max_distance = measurement.is[calculated_index].max_distance;

    bool is_front = false;

    if ((calculated_index % 2) == 0)
    {
        // that is, it is a front capture
        is_front = true;
    }

    QString result;

    if (avg <= min_distance || avg >= max_distance)
    {
        result = "FAIL";
    }
    else
    {
        result = "PASS";
    }

    if (is_front)
    {
        output_stream << date_string << ":\t" << ui->Product_Type->currentText() << "\t" << "Front Camera\t" << avg << " mm, Min: " << measurement.is[calculated_index].min_distance << " mm, Max: " << measurement.is[calculated_index].max_distance << "\t" << result << "\n\r";
    }
    else
    {
         output_stream << date_string << ":\t" << ui->Product_Type->currentText() << "\t" << "Back Camera\t" << avg << " mm, Min: " << measurement.is[calculated_index].min_distance << " mm, Max: " << measurement.is[calculated_index].max_distance << "\t" << result << "\n\r";
    }

    output_file.close();


}

void MainWindow::Log_Extraction_Error(int calculated_index)
{
    output_file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);

    output_stream.setDevice(&output_file);

    QDateTime qdt = QDateTime::currentDateTime();

    QString date_string = qdt.toString("[dd/MM/yyyy, hh:mm:ss:zzz]");

    QString error_string;

    if (measurement.result[0] == NO_LOCATOR)
    {
        error_string = "Locator Not Found";
    }
    else if (measurement.result[0] == LOCATOR_INVALID)
    {
        error_string = "Locator Invalid";
    }
    else if (measurement.result[0] == ROI_INVALID)
    {
        error_string = "Material Out Of Screen";
    }

    bool is_front = false;

    if ((calculated_index % 2) == 0)
    {
        // that is, it is a front capture
        is_front = true;
    }

    if (is_front)
    {
        output_stream << date_string << ":\tFront Camera Error: " << measurement.result[0] << "\t" << error_string << "\n\r";
    }
    else
    {
        output_stream << date_string << ":\tBack Camera Error: " << measurement.result[0] << "\t" << error_string << "\n\r";
    }

    output_file.close();
}

void MainWindow::On_Action_Start_Data_Logging(bool toggled)
{
    if (ui->actionStart_Data_Logging->isChecked())
    {
        if (camera_thread.isRunning())
        {
            if (Register_Lot_Number())
            {
                ready_and_do_data_logging = true;

                // open file stream and get ready to write data

                QString path = output_folder + "/" + lot_number + "/Results.txt";
                output_file.setFileName(path);

                lj.Set_Ready_Status(1);
            }
            else
            {
                 QMessageBox::warning(this, tr("Start Data Logging"), tr("Cannot register lot number under output folder. Check administration rigtht!"), QMessageBox::Ok, QMessageBox::Ok);
                 disconnect(ui->actionStart_Data_Logging, SIGNAL(toggled(bool)), this, SLOT(On_Action_Start_Data_Logging(bool)));
                 ui->actionStart_Data_Logging->setChecked(false);
                 connect(ui->actionStart_Data_Logging, SIGNAL(toggled(bool)), this, SLOT(On_Action_Start_Data_Logging(bool)));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Start Data Logging"), tr("Start camera before requesting data logging."), QMessageBox::Ok, QMessageBox::Ok);

            //disconnect for a while so that manipulating the toggle will not trigger a new slot
            disconnect(ui->actionStart_Data_Logging, SIGNAL(toggled(bool)), this, SLOT(On_Action_Start_Data_Logging(bool)));
            ui->actionStart_Data_Logging->setChecked(false);
            connect(ui->actionStart_Data_Logging, SIGNAL(toggled(bool)), this, SLOT(On_Action_Start_Data_Logging(bool)));
        }
    }
    else
    {
        ready_and_do_data_logging = false;

        // turn off ready status light as well
        lj.Set_Ready_Status(0);
    }
}

void MainWindow::On_Request_Counter_Update(long counter)
{
    ui->Counter->setText(QString("%1").arg(counter));
}

void MainWindow::On_Action_Counter_Reset()
{
    lj.Reset_Counter0();

    // do a read to so that next read will be the value after reset

    //lj.Get_Counter0();

}

void MainWindow::On_Green_Button_Triggered()
{
    // check if camera thread is running or not, if so, enable data logging and fail checking

    if (camera_thread.isRunning() && !ready_and_do_data_logging)
    {
            ui->actionStart_Data_Logging->setChecked(true);
            ui->statusbar->showMessage(tr("Data logging started!"));
    }
    else if (camera_thread.isRunning() && ready_and_do_data_logging)
    {
        ui->actionStart_Data_Logging->setChecked(false);
        ui->statusbar->showMessage(tr("Data logging stopped!"));
    }
    else
    {
        qDebug() << "Camera not started!";
    }
}

void MainWindow::On_Red_Button_Triggered()
{
    On_Action_Clear_Alarm();
}

void MainWindow::On_Front_Min_Distance_Changed(double value)
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    measurement.is[current_index * 2].min_distance = value;
    measurement.Save_Settings(current_index * 2, product_type[current_index].vision_ini_front);
}

void MainWindow::On_Front_Max_Distance_Changed(double value)
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    measurement.is[current_index * 2].max_distance = value;
    measurement.Save_Settings(current_index * 2, product_type[current_index].vision_ini_front);
}

void MainWindow::On_Back_Min_Distance_Changed(double value)
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    measurement.is[current_index * 2 + 1].min_distance = value;
    measurement.Save_Settings(current_index * 2 + 1, product_type[current_index].vision_ini_back);
}

void MainWindow::On_Back_Max_Distance_Changed(double value)
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    measurement.is[current_index * 2 + 1].max_distance = value;
    measurement.Save_Settings(current_index * 2 + 1, product_type[current_index].vision_ini_back);
}

void MainWindow::On_Action_Bypass_Results()
{
    if (ui->actionBypass_Results->isChecked())
    {
        bypass_result = true;;
    }
    else
    {
        bypass_result = false;
    }
}

void MainWindow::On_Action_Clear_Alarm()
{
    if (front_fail_alarmed)
    {
        // Clear the current alarm state if this is triggered
        front_fail_alarmed = false;
        front_consecutive_fail = 0;
        lj.Set_Front_Fail_Status(0);
    }
    if (back_fail_alarmed)
    {
        back_fail_alarmed = false;
        back_consecutive_fail = 0;
        lj.Set_Back_Fail_Status(0);
    }
}

void MainWindow::On_Front_Camera_Shutter_Changed(int value)
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    // remember to multiply by 2 since 2 slots are reserved for each product
    measurement.is[2*current_index].camera_shutter = value;
    vw.Set_Exposure(FRONT_CAM, value);

    qDebug() << "Front shutter settings saved:" <<  product_type[current_index].vision_ini_front;
    measurement.Save_Settings(current_index * 2, product_type[current_index].vision_ini_front);
}

void MainWindow::On_Back_Camera_Shutter_Changed(int value)
{
    QString current_product = ui->Product_Type->currentText();
    int current_index = ui->Product_Type->findText(current_product);

    // remember to multiply by 2 since 2 slots are reserved for each product, offset by one to indicate back camera
    measurement.is[2*current_index + 1].camera_shutter = value;
    //vw.Set_Exposure(BACK_CAM, value);

    qDebug() << "Back shutter settings saved:" <<  product_type[current_index].vision_ini_back;

    measurement.Save_Settings(current_index * 2 + 1, product_type[current_index].vision_ini_back);
}




