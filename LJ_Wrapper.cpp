#include "LJ_Wrapper.h"
#include <QSettings>

LJ_Wrapper::LJ_Wrapper(void)
{
	counter0_reference = 0;
}


LJ_Wrapper::~LJ_Wrapper(void)
{
}

bool LJ_Wrapper::Initialize_LabJack()
{
	long ret = OpenLabJack(LJ_dtU3, LJ_ctUSB, "1", 1, &handle);

    qDebug("Open labjack error code: %d", ret);

	if (ret != 0) 
	{
		return false;
	}

	ePut(handle, ::LJ_ioPUT_CONFIG, ::LJ_chNUMBER_TIMERS_ENABLED, 0, 0);
	ePut(handle, ::LJ_ioPUT_CONFIG, ::LJ_chTIMER_COUNTER_PIN_OFFSET, 4, 0);
	ePut(handle, ::LJ_ioPUT_CONFIG, ::LJ_chTIMER_CLOCK_BASE, ::LJ_tc4MHZ, 0);
	ePut(handle, ::LJ_ioPUT_CONFIG, ::LJ_chTIMER_CLOCK_DIVISOR, 0, 0);
	ePut(handle, ::LJ_ioPUT_COUNTER_ENABLE, 0, 1, 0); // counter 0 is enabled
	ePut(handle, ::LJ_ioPUT_COUNTER_ENABLE, 1, 0, 0);
	ePut(handle, ::LJ_ioPUT_DAC_ENABLE, 1, 0, 0);
	ePut(handle, ::LJ_ioPUT_ANALOG_ENABLE_PORT, 0, 0, 16);

	eDO(handle, 0, 0); //FIO 0 to output low
	eDO(handle, 1, 0); //FIO 1 to output low
	eDO(handle, 2, 0); //FIO 2 to output low
	eDO(handle, 3, 0); //FIO 3 to output low

	long state;

	eDI(handle, 5, &state); //FIO 5 to input
	eDI(handle, 6, &state); //FIO 6 to input
	eDI(handle, 7, &state); //FIO 7 to input

	return true;
}

long LJ_Wrapper::Get_Green_Button_State()
{
	long state;

	eDI(handle, green_button, &state); 

    //qDebug("%d", state);

	return (state);
}

long LJ_Wrapper::Get_Red_Button_State()
{
	long state;

	eDI(handle, red_button, &state);

	return state;
}

long LJ_Wrapper::Get_Counter0()
{
	double value;

	eGet(handle, ::LJ_ioGET_COUNTER, 0, &value, 0);
	

	return value;
}

void LJ_Wrapper::Counter0_Make_Reference()
{
	double value;

	eGet(handle, ::LJ_ioGET_COUNTER, 0, &value, 0);

	counter0_reference = (long) value;
}

bool LJ_Wrapper::Reset_Counter0()
{
	long ret = ePut(handle, ::LJ_ioPUT_COUNTER_RESET, 0, 1, 0);

    qDebug("Counter 0 Reset: %d", ret);

	if (ret == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void LJ_Wrapper::Set_Output_State(int pin, int state)
{
	eDO(handle, pin, state);
}


bool LJ_Wrapper::Load_Settings()
{
	QSettings set("settings/labjack_settings.ini", QSettings::IniFormat);
	
	bool ok;

	green_button = set.value("Input/Green_Button").toInt(&ok);
	if (!ok) return false;

	red_button = set.value("Input/Red_Button").toInt(&ok);
	if (!ok) return false;

	fail_out = set.value("Output/Fail").toInt(&ok);
	if (!ok) return false;

	ready_out = set.value("Output/Ready").toInt(&ok);
	if (!ok) return false;

	return true;

}

void LJ_Wrapper::Set_Fail_Status(int state)
{
	eDO(handle, fail_out, state);
}
void LJ_Wrapper::Set_Ready_Status(int state)
{
	eDO(handle, ready_out, state);
}
