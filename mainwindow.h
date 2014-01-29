#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "Vimba_Wrapper.h"
#include "Measurement.h"
#include "Camera_Thread.h"
#include "LJ_Wrapper.h"
#include "LJ_Thread.h"
#include <QLabel>
#include <QTextStream>

#define FRONT_CAM	0
#define	BACK_CAM	1

struct Product_Type
{
    int index;
    QString name;
    int use_front;
    int use_back;
    QString vision_ini_front;
    QString vision_ini_back;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void Connect_Signals();

    bool Initialize_Vimba_System();
    bool Load_Settings();

    void Display_Front_Settings(int index);
    void Display_Back_Settings(int index);
    void Display_Settings(int product_index);

    void Disable_Manual_Controls();
    void Enable_Manual_Controls();

    void Disable_Front_Camera_Controls();
    void Disable_Back_Camera_Controls();

    void Enable_Front_Camera_Controls();
    void Enable_Back_Camera_Controls();


    void Start_LJ();
    void Stop_LJ();


public slots:

    void On_Frame_Received(int camera_index);
    void On_Product_Type_Changed(int index);
    void On_Start();
    void On_Stop();
    void On_Front_Learn();
    void On_Back_Learn();
    void On_Delay_State_Changed();
    void On_Front_Locator_Threshold_Changed(int value);
    void On_Front_Locator_Threshold_Finished();
    void On_Back_Locator_Threshold_Changed(int value);
    void On_Back_Locator_Threshold_Finished();
    void On_Action_Quit();
    void On_Action_Select_Output_Folder();
    void On_Action_Start_Data_Logging(bool toggled);
    void On_Request_Counter_Update(long counter);
    void On_Action_Counter_Reset();
    void On_Green_Button_Triggered();
    void On_Red_Button_Triggered();
    void On_Front_Min_Distance_Changed(double value);
    void On_Front_Max_Distance_Changed(double value);
    void On_Back_Min_Distance_Changed(double value);
    void On_Back_Max_Distance_Changed(double value);
    void On_Action_Bypass_Results();
    void On_Action_Clear_Alarm();
    void On_Front_Camera_Shutter_Changed(int value);
    void On_Back_Camera_Shutter_Changed(int value);



protected:

   void closeEvent(QCloseEvent *event);



private:

    Ui::MainWindow *ui;


    AVT::VmbAPI::Vimba_Wrapper vw;

    Measurement measurement;

    Camera_Thread	camera_thread;

    void Display_Image(QLabel* view, uchar* data, int width, int height);

    Product_Type* product_type;

    bool learn_front_color_on_next_capture;
    bool learn_back_color_on_next_capture;

    long front_inspection_count;
    long back_inspection_count;
    long front_cycle_count;
    long back_cycle_count;
    long actual_count; // counter value from lj to count actual connectors
    long product_count;

    void Display_Front_Inspection_Results(int index);
    void Display_Back_Inspection_Results(int index);
    void Display_Front_Production_Status(int index);
    void Display_Back_Production_Status(int index);
    void Display_Front_NA_Inspection_Results();
    void Display_Back_NA_Inspection_Results();
    void Log_and_Process_Results(int calculated_index);
    void Log_Extraction_Error();

    LJ_Wrapper lj;
    LJ_Thread lj_thread;

    int use_labjack;
    bool labjack_ok;

    QString output_folder;
    QString lot_number;

    bool ready_and_do_data_logging;
    bool bypass_result;

    QFile output_file;
    QTextStream output_stream;

    long front_consecutive_fail;
    long back_consecutive_fail;
    bool front_prev_result;
    bool back_prev_result;
    bool front_fail_alarmed;
    bool back_fail_alarmed;

    bool Register_Lot_Number();


};

#endif // MAINWINDOW_H
