#pragma once

#include "LabJackUD.h"

class LJ_Wrapper
{
public:
	LJ_Wrapper(void);
	~LJ_Wrapper(void);

	bool Load_Settings();

	bool Initialize_LabJack();

	long Get_Green_Button_State();
	long Get_Red_Button_State();
	long Get_Counter0();
	bool Reset_Counter0();
	void Set_Output_State(int pin, int state);
	void Counter0_Make_Reference();
    void Set_Front_Fail_Status(int state);
    void Set_Back_Fail_Status(int state);
	void Set_Ready_Status(int state);


private:

	LJ_HANDLE handle;

	int green_button;
	int red_button;

    int front_fail_out;
    int back_fail_out;
	int ready_out;

	long counter0_reference;

};

