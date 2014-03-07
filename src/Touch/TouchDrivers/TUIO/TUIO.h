
//+---------------------------------------------------------------------------
//
//  Bluecadet Interactive
//  
//	Developer: Paul Rudolph
//	Date:		2014 - 1 - 15
//  Contents:  Code for TUIO driver to be used with the touch manager class
//  Comments:
//
//----------------------------------------------------------------------------

#pragma once
#include "cinder/app/AppBasic.h"
#include "TuioClient.h"
#include "TouchManager.h"

class Tuio{
public:
	Tuio();
	~Tuio();
	void connect(TouchManager* touchMngerPntr );
	void disconnect();
private:
	 //Mouse Event Callbacks
	cinder::tuio::Client  mTuioConnection;
	cinder::CallbackId					mTuioTouchBeganCallBack,
                                        mTuioTouchMovedCallBack,
                                        mTuioTouchEndCallBack;
   
	void tuioTouchesBegan(cinder::app::TouchEvent event);
    void tuioTouchesMoved(cinder::app::TouchEvent event);
    void tuioTouchesEnded(cinder::app::TouchEvent event);
   TouchManager *mTouchManagerPntr; 

};