#pragma once
#include "LJ_Wrapper.h"
#include <QThread>



class LJ_Thread : public QThread
{

Q_OBJECT

public:
	LJ_Thread(void);
	~LJ_Thread(void);

	void Initialize(LJ_Wrapper* ljw);

	void run();

	bool keep_running;
	bool do_counter_read;
	bool do_green_button;
	bool do_red_button;

signals:

	void Counter0Read(long value);
	void GreenButtonTriggered();
	void RedButtonTriggered();

private:

	LJ_Wrapper* ljww;


};

