#include "FrameObserver.h"

namespace AVT {
namespace VmbAPI {


void FrameObserver::FrameReceived( const FramePtr pFrame )
{
	VmbFrameStatusType eReceiveStatus;

	pFrame->GetReceiveStatus(eReceiveStatus);

	if (eReceiveStatus == VmbErrorSuccess)
	{
        //qDebug("callback!");

		mutex.lock();
		/*
		VmbUint32_t w, h, s;

		pFrame->GetWidth(w);
		pFrame->GetHeight(h);
		pFrame->GetImageSize(s);

        qDebug("Image captured with parameters:");
        qDebug(" Width: %d", w);
        qDebug(" Height %d", h);
        qDebug(" Size: %d", s);
		*/

		frames.append(pFrame);
		//VmbErrorType error = m_pCamera->QueueFrame(pFrame);
        //qDebug("error: %d", error);
			
		//is_frame_ready = true;
		mutex.unlock();

        emit CallbackFinished(index);
		
	}	
	else
	{
        qDebug("Frame receive status not successful!");
		mutex.lock();
		m_pCamera->QueueFrame(pFrame);
		mutex.unlock();
	}
	
}

FramePtr FrameObserver::GetFrame()
{
	   // Lock the frame queue
    mutex.lock();
    // Pop frame from queue
    FramePtr res = frames.front();
	frames.removeFirst();
    // Unlock frame queue

	//is_frame_ready = false;
	
    mutex.unlock();

	
	return res;
}

}}
