
//+---------------------------------------------------------------------------
//
//  Bluecadet Interactive
//  
//	Developer: Paul Rudolph
//	Date:		2014 - 1 - 15
//  Contents:  Code for mouse driver to be used with the touch manager class
//  Comments:This mouse driver was based off of Cinder's listenerBasic sample 
//
//----------------------------------------------------------------------------

#pragma once
#include "cinder/app/AppBasic.h"
#include "TouchManager.h"

class Mouse{
public:
	Mouse();
	~Mouse();
	void connect(TouchManager* touchMngerPntr );
	void disconnect();
private:
	 //Mouse Event Callbacks
    boost::signals2::scoped_connection  mMouseBeganCallBack,
                                        mMouseMovedCallBack,
                                        mMouseEndCallBack;
   
   void mouseBegan(  cinder::app::MouseEvent &event);
   void mouseMoved(  cinder::app::MouseEvent &event);
   void mouseEnded(  cinder::app::MouseEvent &event);
   
   TouchManager *mTouchManagerPntr; 
   int mTouchId;
};