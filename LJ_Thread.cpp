#include "LJ_Thread.h"

LJ_Thread::LJ_Thread(void)
{
	keep_running = true;
	do_counter_read = false;
	do_green_button = false;
	do_red_button = false;
}


LJ_Thread::~LJ_Thread(void)
{
}

void LJ_Thread::Initialize(LJ_Wrapper* ljw)
{
	ljww = ljw;
}

void LJ_Thread::run()
{
	long counter;
	long prev_green_button_state = 1;
	long current_green_button_state;

	long prev_red_button_state = 1;
	long current_red_button_state;

	while (keep_running)
	{
			if (do_counter_read)
			{
				counter = ljww->Get_Counter0();
				emit Counter0Read(counter);
			}


			if (1)
			{
				// look for a falling edge for green button
				current_green_button_state = ljww->Get_Green_Button_State();

				if ( (prev_green_button_state == 1) && (current_green_button_state == 0))
				{
					prev_green_button_state = current_green_button_state;
					emit GreenButtonTriggered();
				}
				else
				{
					prev_green_button_state = current_green_button_state;
				}
			}

			if (1)
			{	
				// look for a falling edge for Red button
				current_red_button_state = ljww->Get_Red_Button_State();

				if ( (prev_red_button_state == 1) && (current_red_button_state == 0) )
				{
					prev_red_button_state = current_red_button_state;
					emit RedButtonTriggered();
				}
				else
				{
					prev_red_button_state = current_red_button_state;
				}
			}

			msleep(300);

	}

}
