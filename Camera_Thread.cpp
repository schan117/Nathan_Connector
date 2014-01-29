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
    do_back_camera = false;
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

        if (do_front_camera && do_back_camera)
        {
            // both cameras have to be used, do a sequential trigger
            camera_index = 0;

            mutex.lock();
            vvw->Trigger_Frame(camera_index);

            connect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));
            q.exec();
            disconnect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));

            vvw->Transfer_Frame(camera_index);

            camera_index = 1;

            vvw->Trigger_Frame(camera_index);

            connect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));
            q.exec();
            disconnect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));

            vvw->Transfer_Frame(camera_index);

            emit FrameReceived(camera_index);

        }
        else
        {
            if (do_front_camera)
            {
                camera_index = 0; // front camera

                mutex.lock();
                vvw->Trigger_Frame(camera_index);

                // block thread until frame is ready
                connect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));
                q.exec();
                disconnect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));


                vvw->Transfer_Frame(camera_index);

                emit FrameReceived(camera_index);

            }
            else if (do_back_camera)
            {

                camera_index = 1; // back camera

                mutex.lock();
                vvw->Trigger_Frame(camera_index);

                // block thread until frame is ready
                connect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));
                q.exec();
                disconnect(vvw, SIGNAL(Frame_Received(int)), &q, SLOT(quit()));

                vvw->Transfer_Frame(camera_index);

                emit FrameReceived(camera_index);
            }
        }

		capture_time = qtime.elapsed();

		// if there is a do delay request, hold the thread for a while
		if (!do_delay)
		{
            //msleep(10);
		}
		else
		{
			msleep(1000);
		}
		//////////////////////////////////////////////////////////////
	}
}
