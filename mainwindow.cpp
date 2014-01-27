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

        learn_color_on_next_capture = false;
        front_inspection_count = 0;
        front_cycle_count;
        actual_count = 0;
        ready_and_do_data_logging = false;
        consecutive_fail = 0;
        prev_result = true;
        product_count = 0;
        bypass_result = false;
        fail_alarmed = false;

        Load_Settings();

        Disable_Back_Camera_Controls();

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
                if (front_m.Load_Settings(i, product_type[i].vision_ini))
                {
                    qDebug("Load in parameters successful for product %d, %s", i, product_type[i].vision_ini.toLatin1().constData());
                    load_ok = true;
                }
                else
                {
                    load_ok = false;
                    break;
                }
            }

            // load in default meaurement settings for front
            if (load_ok)
            {
                // check which camera status, whether they are opened properly


                if (vw.cameras_opened == 2)
                {
                    // display front camera controls as an default
                    ui->actionEnable_Front_Camera_2->setChecked(true);


                    if (vw.found_list[0] == 1 && vw.found_list[1] == 0)
                    {
                        ui->actionEnable_Front_Camera_2->setChecked(true);
                        ui->actionEnable_Back_Camera->setEnabled(false);
                    }
                    else if (vw.found_list[0] == 0 && vw.found_list[1] == 1)
                    {
                        ui->actionEnable_Back_Camera->setChecked(true);
                        ui->actionEnable_Front_Camera_2->setEnabled(false);
                    }

                }
                else if   (vw.cameras_opened == 1)
                {
                    qDebug() << "here!!!";
                    ui->actionEnable_Front_Camera_2->setChecked(true);
                    ui->actionEnable_Back_Camera->setEnabled(true);
                }





                // fill in reference color informations

                Display_Front_Settings(C1_FRONT);
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
    connect(ui->Front_Type, SIGNAL(currentIndexChanged(int)), this, SLOT(On_Front_Type_Changed(int)));
    connect(ui->Start, SIGNAL(clicked()), this, SLOT(On_Front_Start()));
    connect(ui->Stop, SIGNAL(clicked()), this, SLOT(On_Front_Stop()));
    connect(ui->Front_Learn, SIGNAL(clicked()), this, SLOT(On_Front_Learn()));
    connect(ui->actionDelay_Inspection, SIGNAL(triggered()), this, SLOT(On_Delay_State_Changed()));
    connect(ui->actionStart_Data_Logging, SIGNAL(toggled(bool)), this, SLOT(On_Action_Start_Data_Logging(bool)));
    connect(ui->Front_Threshold, SIGNAL(valueChanged(int)), this, SLOT(On_Front_Locator_Threshold_Changed(int)));
    connect(ui->Front_Threshold, SIGNAL(sliderReleased()), this, SLOT(On_Front_Locator_Threshold_Finished()));
    connect(ui->Camera_Shutter, SIGNAL(valueChanged(int)), this, SLOT(On_Camera_Shutter_Changed(int)));

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(On_Action_Quit()));
    connect(ui->actionSet_Output_Folder, SIGNAL(triggered()), this, SLOT(On_Action_Select_Output_Folder()));
    connect(ui->actionSet_Lot_Number, SIGNAL(triggered()), this, SLOT(On_Action_Register_Lot_Number()));

    connect(&lj_thread, SIGNAL(Counter0Read(long)), this, SLOT(On_Request_Counter_Update(long)));
    connect(&lj_thread, SIGNAL(GreenButtonTriggered()), this, SLOT(On_Green_Button_Triggered()));
    connect(&lj_thread, SIGNAL(RedButtonTriggered()), this, SLOT(On_Red_Button_Triggered()));

    connect(ui->actionActual_Counter_Reset, SIGNAL(triggered()), this, SLOT(On_Action_Counter_Reset()));

    connect(ui->Front_Min_Silver, SIGNAL(valueChanged(double)), this, SLOT(On_Front_Min_Silver_Changed(double)));

    connect(ui->actionEnable_Front_Camera_2, SIGNAL(triggered()), this, SLOT(On_Front_Camera_State_Changed()));
    connect(ui->actionEnable_Back_Camera, SIGNAL(triggered()), this, SLOT(On_Back_Camera_State_Changed()));

    connect(ui->actionBypass_Results, SIGNAL(triggered()), this, SLOT(On_Action_Bypass_Results()));
    connect(ui->actionClear_Alarm, SIGNAL(triggered()), this, SLOT(On_Action_Clear_Alarm()));
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



    if (ui->Front_Type->currentIndex() == C1_FRONT)
    {
        front_m.Set_Image(vw.internal_image_rgb[camera_index]);
        qtime.restart();
        front_m.Perform_Extraction(C1_FRONT, front_m.is[C1_FRONT].locator_threshold);
        process_time = qtime.elapsed();
        Display_Image(ui->View, front_m.result_image.data, front_m.result_image.cols, front_m.result_image.rows);

        ui->Results->clear();

        // check result status
        if (front_m.result[0] == CAL_OK)
        {
            Display_Front_Inspection_Results(C1_FRONT);
            ui->Results->append(tr("Acquisition time: %1 secs, Processing time: %2 secs").arg(camera_thread.capture_time/1000.0).arg(process_time/1000.0));

            Display_Production_Status(C1_FRONT);

            if (ready_and_do_data_logging)
            {
                Log_and_Process_Results(C1_FRONT);
                ui->Results->append("Data are recorded");
            }
            else
            {
                ui->Results->append(tr("Data recording not performed."));
            }

                // if there was an request to learn color, do it
            if (learn_color_on_next_capture)
            {
                front_m.is[C1_FRONT].copper_h = front_m.result[1];
                front_m.is[C1_FRONT].copper_s = front_m.result[2];
                front_m.is[C1_FRONT].silver_h = front_m.result[3];
                front_m.is[C1_FRONT].silver_s = front_m.result[4];

                Display_Front_Settings(C1_FRONT);

                front_m.Save_Settings(C1_FRONT, product_type[C1_FRONT].vision_ini);
                learn_color_on_next_capture = false;
            }
            ////////////////////////////////////////////////
        }
        else
        {
            Display_NA_Inspection_Results();

            if (ready_and_do_data_logging)
            {
                Log_Extraction_Error(C1_FRONT);
            }
        }




    }
    else if (ui->Front_Type->currentIndex() == C2_FRONT)
    {
        //qDebug("result: %d", front_m.result[0]);
        /*
        Mat out;

        cvtColor(vw.internal_image_rgb[camera_index], out, CV_RGB2GRAY);

        imwrite("test.bmp", out);
        */
        front_m.Set_Image(vw.internal_image_rgb[camera_index]);

        qtime.restart();
        front_m.Perform_Extraction(C2_FRONT, front_m.is[C2_FRONT].locator_threshold);
        process_time = qtime.elapsed();

        Display_Image(ui->View, front_m.result_image.data, front_m.result_image.cols, front_m.result_image.rows);

        ui->Results->clear();

        if (front_m.result[0] == CAL_OK)
        {
            Display_Front_Inspection_Results(C2_FRONT);
            ui->Results->append(tr("Acquisition time: %1 secs, Processing time: %2 secs").arg(camera_thread.capture_time/1000.0).arg(process_time/1000.0));

            Display_Production_Status(C2_FRONT);

            if (ready_and_do_data_logging)
            {
                Log_and_Process_Results(C2_FRONT);
                ui->Results->append("Data are recorded");
            }
            else
            {
                ui->Results->append(tr("Data recording not performed."));
            }

            // if there was an request to learn color, do it
            if (learn_color_on_next_capture)
            {
                front_m.is[C2_FRONT].copper_h = front_m.result[1];
                front_m.is[C2_FRONT].copper_s = front_m.result[2];
                front_m.is[C2_FRONT].silver_h = front_m.result[3];
                front_m.is[C2_FRONT].silver_s = front_m.result[4];

                front_m.Save_Settings(C2_FRONT, product_type[C2_FRONT].vision_ini);

                Display_Front_Settings(C2_FRONT);

                learn_color_on_next_capture = false;
            }


            ////////////////////////////////////////////////
        }
        else
        {
            Display_NA_Inspection_Results();
            if (ready_and_do_data_logging)
            {
                Log_Extraction_Error(C2_FRONT);
            }
        }


    }
    else if (ui->Front_Type->currentIndex() == C3_FRONT)
    {
        front_m.Set_Image(vw.internal_image_rgb[camera_index]);
        qtime.restart();
        front_m.Perform_Extraction(C3_FRONT, front_m.is[C3_FRONT].locator_threshold);
        process_time = qtime.elapsed();
        Display_Image(ui->View, front_m.result_image.data, front_m.result_image.cols, front_m.result_image.rows);

        ui->Results->clear();

        // check result status
        if (front_m.result[0] == CAL_OK)
        {
            Display_Front_Inspection_Results(C3_FRONT);
            ui->Results->append(tr("Acquisition time: %1 secs, Processing time: %2 secs").arg(camera_thread.capture_time/1000.0).arg(process_time/1000.0));

            Display_Production_Status(C3_FRONT);

            if (ready_and_do_data_logging)
            {
                Log_and_Process_Results(C3_FRONT);
                ui->Results->append("Data are recorded");
            }
            else
            {
                ui->Results->append(tr("Data recording not performed."));
            }

            // if there was an request to learn color, do it
            if (learn_color_on_next_capture)
            {
                front_m.is[C3_FRONT].copper_h = front_m.result[1];
                front_m.is[C3_FRONT].copper_s = front_m.result[2];
                front_m.is[C3_FRONT].silver_h = front_m.result[3];
                front_m.is[C3_FRONT].silver_s = front_m.result[4];

                Display_Front_Settings(C3_FRONT);

                front_m.Save_Settings(C3_FRONT, product_type[C3_FRONT].vision_ini);
                learn_color_on_next_capture = false;
            }
            ////////////////////////////////////////////////
        }
        else
        {
            Display_NA_Inspection_Results();
            if (ready_and_do_data_logging)
            {
                Log_Extraction_Error(C3_FRONT);
            }
        }
    }

    /// tell camera thread that processing has finished
    camera_thread.mutex.unlock();
}

void MainWindow::Display_Front_Inspection_Results(int index)
{

        QString cr;
        QString sr;
        QString dis;


    // clear result textbox;



        Display_Image(ui->H, front_m.front_roi_h.data, front_m.front_roi_h.cols, front_m.front_roi_h.rows);
        Display_Image(ui->S, front_m.front_roi_s.data, front_m.front_roi_s.cols, front_m.front_roi_s.rows);
        Display_Image(ui->Color, front_m.front_roi_color.data, front_m.front_roi_color.cols, front_m.front_roi_color.rows);

        // display current readings
        cr = QString("%1, %2").arg(front_m.result[1]).arg(front_m.result[2]);
        sr = QString("%1, %2").arg(front_m.result[3]).arg(front_m.result[4]);

        if (front_m.result[5] > 0.35)
        {
            dis = QString(tr("%1 - Good")).arg(front_m.result[5]);
        } else if (front_m.result[5] > 0.2)
        {
            dis = QString(tr("%1 - OK")).arg(front_m.result[5]);
        } else
        {
            dis = QString(tr("%1 - Bad")).arg(front_m.result[5]);
        }

        ui->Silver_Reading->setText(sr);
        ui->Copper_Reading->setText(cr);
        ui->Current_Discrepancy->setText(dis);

        // Display results

        QString result_string;



        result_string = QString(tr("Cycle: %1")).arg(front_cycle_count);
        ui->Results->append(result_string);
        result_string = QString(tr("Samples Inspected: %1")).arg(front_inspection_count);
        ui->Results->append(result_string);

        for (int i=0; i<front_m.result[6]; i++)
        {
            result_string = QString(tr("Sample %1: %2 mm")).arg(i).arg(front_m.result[i+7]);
            ui->Results->append(result_string);
        }

        double avg = front_m.result[7+(int) front_m.result[6]];

        result_string = QString(tr("--------------------\nAverage: %1 mm")).arg(avg);

        if (avg < front_m.is[index].min_silver)
        {
            result_string = result_string + ", Min: " + QString("%1").arg(front_m.is[index].min_silver) + " mm, FAIL";


        }
        else
        {
            result_string = result_string + ", Min: " + QString("%1").arg(front_m.is[index].min_silver) + " mm, PASS";


        }

        ui->Results->append(result_string);

        front_inspection_count = front_inspection_count + front_m.result[6];
        front_cycle_count++;

    //////////////////////////////////////////////////////////////
}

void MainWindow::Display_NA_Inspection_Results()
{
    if (front_m.result[0] == NO_LOCATOR)
    {
        ui->Results->append(tr("Error #1: No locator found! Check locator sensitivity settings."));
    }
    else if (front_m.result[0] == LOCATOR_INVALID)
    {
        ui->Results->append(tr("Error #2: Locator all invalid! Check locator sensitivity settings."));
    }
    else if (front_m.result[0] == ROI_INVALID)
    {
        ui->Results->append(tr("Error #3: Roi's of of screen."));
    }

    ui->H->setPixmap(QPixmap());
    ui->S->setPixmap(QPixmap());
    ui->Color->setPixmap(QPixmap());

    // display current readings
    QString cr = QString("H: %1, S: %2").arg("N/A").arg("N/A");
    QString sr = QString("H: %1, S: %2").arg("N/A").arg("N/A");
    QString dis = QString("%1").arg("N/A");

    ui->Silver_Reading->setText(sr);
    ui->Copper_Reading->setText(cr);
    ui->Current_Discrepancy->setText(dis);
    //////////////////////////////////////////////////////////////

    ui->Front_Result->setStyleSheet("background: white");
    ui->Front_Result->setText("N/A");

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

void MainWindow::On_Front_Type_Changed(int type)
{
    front_m.Load_Settings(type, product_type[type].vision_ini);

    // disconnect some signals for a while to prvent problems
    disconnect(ui->Front_Threshold, SIGNAL(sliderReleased()), this, SLOT(On_Front_Locator_Threshold_Finished()));
    disconnect(ui->Front_Min_Silver, SIGNAL(valueChanged(double)), this, SLOT(On_Front_Min_Silver_Changed(double)));
    disconnect(ui->Camera_Shutter, SIGNAL(valueChanged(int)), this, SLOT(On_Camera_Shutter_Changed(int)));

    Display_Front_Settings(type);

    connect(ui->Front_Threshold, SIGNAL(sliderReleased()), this, SLOT(On_Front_Locator_Threshold_Finished()));
    connect(ui->Front_Min_Silver, SIGNAL(valueChanged(double)), this, SLOT(On_Front_Min_Silver_Changed(double)));
    connect(ui->Camera_Shutter, SIGNAL(valueChanged(int)), this, SLOT(On_Camera_Shutter_Changed(int)));


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
    {
        product_type[i].name = set.value(QString("Product%1/Name").arg(i)).toString();
        ui->Front_Type->addItem(product_type[i].name);
        ui->Back_Type->addItem(product_type[i].name);

        product_type[i].vision_ini = set.value(QString("Product%1/Vision_Ini_Name").arg(i)).toString();
    }

    output_folder = set.value("Main/Output_Folder").toString();

    return true;
}

void MainWindow::On_Front_Start()
{
    // Check if there are any cameras being enabled at all

    if (!ui->actionEnable_Front_Camera_2->isChecked() && !ui->actionEnable_Back_Camera->isChecked())
    {
        QMessageBox::warning(this, tr("Start Capture"), tr("No cameras are enabled, not starting capture."), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }


    if (!camera_thread.isRunning())
    {
        qDebug("Start front capture!");

        Disable_Manual_Controls();

        // Load in settings everytime to make changes in ini file easier to reflect

        if (ui->Front_Type->currentIndex() == C1_FRONT)
        {
            front_m.Load_Settings(C1_FRONT, product_type[C1_FRONT].vision_ini);
        }
        else if (ui->Front_Type->currentIndex() == C2_FRONT)
        {
            front_m.Load_Settings(C2_FRONT, product_type[C2_FRONT].vision_ini);
        }
        else if (ui->Front_Type->currentIndex() == C3_FRONT)
        {
            front_m.Load_Settings(C3_FRONT, product_type[C3_FRONT].vision_ini);
        }
        else if (ui->Front_Type->currentIndex() == C1_BACK)
        {
            front_m.Load_Settings(C1_BACK, product_type[C1_BACK].vision_ini);
        }
        else if (ui->Front_Type->currentIndex() == C2_BACK)
        {
            front_m.Load_Settings(C2_BACK, product_type[C2_BACK].vision_ini);
        }
        else
        {
            QMessageBox::warning(this, tr("Cannot Peform Request"), tr("Cannot start inspection as settings cannot be loaded!"), QMessageBox::Ok);
            return;
        }

        // result number of inspections done so far
        front_inspection_count = 0;
        back_inspection_count = 0;
        front_cycle_count = 0;
        back_cycle_count = 0;

        camera_thread.do_front_camera = true;

        if (ui->actionEnable_Front_Camera_2->isChecked())
        {
            vw.Set_Exposure(FRONT_CAM, front_m.is[ui->Front_Type->currentIndex()].camera_shutter);
            camera_thread.camera_index = FRONT_CAM;
        }
        else if (ui->actionEnable_Back_Camera->isChecked())
        {
            vw.Set_Exposure(BACK_CAM, front_m.is[ui->Front_Type->currentIndex()].camera_shutter);
            camera_thread.camera_index = BACK_CAM;
        }

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

void MainWindow::On_Front_Stop()
{
    if (camera_thread.isRunning())
    {
        qDebug("Stop front capture!");
        camera_thread.keep_capturing = false;
        Enable_Manual_Controls();

        // reset fail accumulation on stop camera
        consecutive_fail = 0;
        prev_result = true;

        Stop_LJ();

        ui->actionStart_Data_Logging->setChecked(false);
    }

}

void MainWindow::Display_Front_Settings(int index)
{
    QString qs = QString("%1, %2").arg(front_m.is[index].silver_h).arg(front_m.is[index].silver_s);
    QString qc = QString("%1, %2").arg(front_m.is[index].copper_h).arg(front_m.is[index].copper_s);

    ui->Silver_Reference->setText(qs);
    ui->Copper_Reference->setText(qc);

    ui->Front_Threshold->setValue(front_m.is[index].locator_threshold);

    ui->Front_Min_Silver->setValue(front_m.is[index].min_silver);

    ui->Camera_Shutter->setValue(front_m.is[index].camera_shutter);

}

void MainWindow::Display_Back_Settings(int index)
{
    QString qs = QString("%1, %2").arg(front_m.is[index].silver_h).arg(front_m.is[index].silver_s);
    QString qc = QString("%1, %2").arg(front_m.is[index].copper_h).arg(front_m.is[index].copper_s);

    ui->Silver_Reference_Back->setText(qs);
    ui->Copper_Reference_Back->setText(qc);

    ui->Back_Threshold->setValue(front_m.is[index].locator_threshold);

    ui->Back_Min_Silver->setValue(front_m.is[index].min_silver);
}

void MainWindow::Disable_Manual_Controls()
{
    ui->Front_Type->setEnabled(false);

    // disable the camera menu bar so that user cannot change config during inspections!
    ui->menuCamera->setEnabled(false);
}

void MainWindow::Enable_Manual_Controls()
{
    ui->Front_Type->setEnabled(true);


    ui->menuCamera->setEnabled(true);

}

void MainWindow::On_Front_Learn()
{
    if (camera_thread.isRunning())
    {
        int ret = QMessageBox::information(this, tr("Learn colors"), tr("Press OK to learn Hue and saturation values for silver and copper colors."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

        if (ret == QMessageBox::Ok)
        {
            learn_color_on_next_capture = true;
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
    ui->View->setVisible(false);
    ui->Color->setVisible(false);
    ui->H->setVisible(false);
    ui->S->setVisible(false);

    ui->Front_Controls->setVisible(false);
}

void MainWindow::Enable_Front_Camera_Controls()
{
    ui->View->setVisible(true);
    ui->Color->setVisible(true);
    ui->H->setVisible(true);
    ui->S->setVisible(true);

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
    switch (ui->Front_Type->currentIndex())
    {
    case C1_FRONT:
        {
            front_m.is[C1_FRONT].locator_threshold = value;
            return;
        }
    case C2_FRONT:
        {
            front_m.is[C2_FRONT].locator_threshold = value;
            return;
        }
    case C3_FRONT:
        {
            front_m.is[C3_FRONT].locator_threshold = value;
            return;
        }
    case C1_BACK:
        {
            front_m.is[C1_BACK].locator_threshold = value;
            return;
        }
    case C2_BACK:
        {
            front_m.is[C2_BACK].locator_threshold = value;
            return;
        }
    default:
        {
            return;
        }
    }

}

void MainWindow::On_Front_Locator_Threshold_Finished()
{
    switch (ui->Front_Type->currentIndex())
    {
    case C1_FRONT:
        {
            front_m.Save_Settings(C1_FRONT, product_type[C1_FRONT].vision_ini);
            return;
        }
    case C2_FRONT:
        {
            front_m.Save_Settings(C2_FRONT, product_type[C2_FRONT].vision_ini);
            return;
        }
    case C3_FRONT:
        {
            front_m.Save_Settings(C3_FRONT, product_type[C3_FRONT].vision_ini);
            return;
        }
    case C1_BACK:
        {
            front_m.Save_Settings(C1_BACK, product_type[C1_BACK].vision_ini);
            return;
        }
    case C2_BACK:
        {
            front_m.Save_Settings(C2_BACK, product_type[C2_BACK].vision_ini);
            return;
        }
    default:
        {
            return;
        }
    }
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

void MainWindow::On_Action_Register_Lot_Number()
{
    bool ok;

    lot_number = QInputDialog::getText(this, tr("Register"), tr("Enter lot number:"), QLineEdit::Normal, "", &ok);


    QDir qdir(output_folder);

    if (ok)
    {
        if (qdir.mkdir(lot_number) || qdir.exists(lot_number))
        {
            QMessageBox::information(this, tr("Register"), QString(tr("Lot %1 registered at %2.")).arg(lot_number).arg(output_folder), QMessageBox::Ok, QMessageBox::Ok);
        }
        else
        {
            QMessageBox::information(this, tr("Register"), QString(tr("Error# RF1: Fail to register. Check administration right at selected folder!")), QMessageBox::Ok, QMessageBox::Ok);
        }
    }

}

void MainWindow::Display_Production_Status(int index)
{
    double avg = front_m.result[7+(int) front_m.result[6]];
    double min_silver = front_m.is[index].min_silver;

    if (!bypass_result)
    {
        if (avg <= min_silver)
        {
            if (prev_result == false)
            {
                // only increment fail counter if system is ready
                if (ready_and_do_data_logging)
                {
                    consecutive_fail++;
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

            if (consecutive_fail > front_m.is[index].fail_threshold)
            {
                ui->Front_Result->setText(QString(tr("Fail - %1/%2, Alarm!")).arg(consecutive_fail).arg(front_m.is[index].fail_threshold));
                fail_alarmed = true;

                // Send signal out frrom labjack

                lj.Set_Fail_Status(1);

            }
            else
            {
                ui->Front_Result->setText(QString(tr("Fail - %1/%2")).arg(consecutive_fail).arg(front_m.is[index].fail_threshold));
            }

            prev_result = false;
        }
        else
        {

            // if there is a pass, reset consecutive fail
            // also need to check if this pass is immediately after a fail alarm if so, notify the user as well

            if (ready_and_do_data_logging)
            {
                if (fail_alarmed)
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


            if (fail_alarmed)
            {
                ui->Front_Result->setText(tr("Pass - Alarm Triggered"));
                prev_result = true;
                consecutive_fail = 0;
            }
            else
            {
                ui->Front_Result->setText(tr("Pass"));
                prev_result = true;
                consecutive_fail = 0;
            }
        }
    }
    else
    {
        // ignore inspection results
        ui->Front_Result->setText(QString(tr("Bypass")));
        ui->Front_Result->setStyleSheet("background: blue");
        consecutive_fail = 0;
        prev_result = true;
    }
}

void MainWindow::Log_and_Process_Results(int index)
{

    output_file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);

    output_stream.setDevice(&output_file);

    QDateTime qdt = QDateTime::currentDateTime();

    QString date_string = qdt.toString("[dd/MM/yyyy, hh:mm:ss:zzz]");

    double avg = front_m.result[7+(int) front_m.result[6]];
    double min_silver = front_m.is[index].min_silver;

    QString result;

    if (avg <= min_silver)
    {
        result = "FAIL";
    }
    else
    {
        result = "PASS";
    }

    output_stream << date_string << ":\t" << ui->Front_Type->currentText() << "\t" << avg << " mm, Min: " << front_m.is[index].min_silver << " mm, " << result << "\n\r";

    output_file.close();


}

void MainWindow::Log_Extraction_Error(int index)
{
    output_file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);

    output_stream.setDevice(&output_file);

    QDateTime qdt = QDateTime::currentDateTime();

    QString date_string = qdt.toString("[dd/MM/yyyy, hh:mm:ss:zzz]");

    output_stream << date_string << ":\tError: " << front_m.result[0] << "\n\r";

    output_file.close();
}

void MainWindow::On_Action_Start_Data_Logging(bool toggled)
{
    if (ui->actionStart_Data_Logging->isChecked())
    {
        if (camera_thread.isRunning())
        {
            if (!lot_number.isNull())
            {
                ready_and_do_data_logging = true;

                // open file stream and get ready to write data

                QString path = output_folder + "/" + lot_number + "/Results.txt";
                output_file.setFileName(path);

                lj.Set_Ready_Status(1);
            }
            else
            {
                QMessageBox::warning(this, tr("Start Data Logging"), tr("Please register lot number first."), QMessageBox::Ok, QMessageBox::Ok);
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
    else
    {

    }
}

void MainWindow::On_Red_Button_Triggered()
{
    On_Action_Clear_Alarm();
}

void MainWindow::On_Front_Min_Silver_Changed(double value)
{
    switch (ui->Front_Type->currentIndex())
    {
    case C1_FRONT:
        {
            front_m.is[C1_FRONT].min_silver = value;
            front_m.Save_Settings(C1_FRONT, product_type[C1_FRONT].vision_ini);
            return;
        }
    case C2_FRONT:
        {
            front_m.is[C2_FRONT].min_silver = value;
            front_m.Save_Settings(C2_FRONT, product_type[C2_FRONT].vision_ini);
            return;
        }
    case C3_FRONT:
        {
            front_m.is[C3_FRONT].min_silver = value;
            front_m.Save_Settings(C3_FRONT, product_type[C3_FRONT].vision_ini);
            return;
        }
    case C1_BACK:
        {
            front_m.is[C1_BACK].min_silver = value;
            front_m.Save_Settings(C1_BACK, product_type[C1_BACK].vision_ini);
            return;
        }
    case C2_BACK:
        {
            front_m.is[C2_BACK].min_silver = value;
            front_m.Save_Settings(C2_BACK, product_type[C2_BACK].vision_ini);
            return;
        }
    default:
        {
            return;
        }
    }

}

void MainWindow::On_Front_Camera_State_Changed()
{
    if (ui->actionEnable_Front_Camera_2->isChecked())
    {
        Enable_Front_Camera_Controls();
        ui->actionEnable_Back_Camera->setChecked(false);
    }
    else
    {
        Disable_Front_Camera_Controls();
    }

}

void MainWindow::On_Back_Camera_State_Changed()
{


    if (ui->actionEnable_Back_Camera->isChecked())
    {

        Enable_Front_Camera_Controls();
        ui->actionEnable_Front_Camera_2->setChecked(false);
    }
    else
    {
        // Note, it is FRONT that is being disabled
        Disable_Front_Camera_Controls();
    }

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
    if (fail_alarmed)
    {
        // Clear the current alarm state if this is triggered
        fail_alarmed = false;
        consecutive_fail = 0;
        lj.Set_Fail_Status(0);
    }

}

void MainWindow::On_Camera_Shutter_Changed(int value)
{
    front_m.is[ui->Front_Type->currentIndex()].camera_shutter = value;

    if (ui->actionEnable_Front_Camera_2->isChecked())
    {
        vw.Set_Exposure(0, value);
    }
    else if (ui->actionEnable_Back_Camera->isChecked())
    {
        vw.Set_Exposure(1, value);
    }

    front_m.Save_Settings(ui->Front_Type->currentIndex(), product_type[ui->Front_Type->currentIndex()].vision_ini);

}




