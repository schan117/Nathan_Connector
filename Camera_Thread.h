#pragma once

#include <QThread>
#include "Vimba_Wrapper.h"

class Camera_Thread : public QThread
{

Q_OBJECT

public:
	Camera_Thread(void);
	~Camera_Thread(void);

	void Initialize(AVT::VmbAPI::Vimba_Wrapper* vw);

	void run();

	int camera_index;
	
	bool keep_capturing;

	bool do_front_camera;
	bool do_back_camera;

	bool do_delay;

	bool processing_finished;

	int capture_time;

    QMutex mutex;

signals:

	void FrameReceived(int camera_index);

private:

	AVT::VmbAPI::Vimba_Wrapper* vvw;

};

