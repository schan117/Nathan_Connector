#include "Camera_Thread.h"
#include <QEventLoop>
#include <QDebug>

Camera_Thread::Camera_Thread(void)
{
	keep_capturing = false;
	camera_index = 0;
	do_delay = false;
	processing_finished = true;
	do_front_camera = false;
	do_back_camera = true;
}


Camera_Thread::~Camera_Thread(void)
{
}

void Camera_Thread::Initialize(AVT::VmbAPI::Vimba_Wrapper* vw)
{
	vvw = vw;
}

void Camera_Thread::run()
{
	keep_capturing = true;

	QTime qtime;

	qtime.start();

    QEventLoop q;

	while (keep_capturing)
	{
		
		qtime.restart();

		if (do_front_camera)
		{	

            mutex.lock();
            vvw->Trigger_Frame(camera_index);

            // block thread until frame is ready
            connect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));
            q.exec();
            disconnect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));

            vvw->Transfer_Frame(camera_index);

			emit FrameReceived(camera_index);
			
		}
		/*
		if (do_back_camera)
		{
			camera_index = 1;
			vvw->Trigger_Frame(camera_index);

			// block thread until frame is ready
			while (vvw->frame_ready == false)
			{
				msleep(10);
			}

			vvw->Transfer_Frame(camera_index);

			while (processing_finished == false)
			{
				msleep(10);
			}

			emit FrameReceived(camera_index);
		}
		*/
		capture_time = qtime.elapsed();

		// wait unit the main thread has finished processing the previous frame before emitting new signal
			
		// toggle flag immediately so that the next loop will be block if image comes faster than the processing
		processing_finished = false;

		// if there is a do delay request, hold the thread for a while
		if (!do_delay)
		{
			msleep(10);
		}
		else
		{
			msleep(1000);
		}
		//////////////////////////////////////////////////////////////




	}
}
