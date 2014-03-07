
#include "Mouse.h"

using namespace std;
using namespace ci;
using namespace ci::app;

Mouse::Mouse(){
		mTouchManagerPntr=NULL;
		mTouchId=-1;
}

void Mouse::connect(TouchManager *touchMngerPntr){
	//Connect to the application window touch event signals
	ci::app::WindowRef window = cinder::app::getWindow();

	mMouseBeganCallBack	= window->getSignalMouseDown().connect( std::bind( &Mouse::mouseBegan, this, std::_1 ));
    mMouseMovedCallBack	= window->getSignalMouseDrag().connect( std::bind( &Mouse::mouseMoved, this, std::_1 ));
    mMouseEndCallBack	= window->getSignalMouseUp()  .connect( std::bind( &Mouse::mouseEnded, this, std::_1 ));

	// Connect to the Touch Manager
	mTouchManagerPntr = touchMngerPntr;
}

Mouse::~Mouse(){
	//Disconnect from the mouse signals
	mMouseBeganCallBack	.disconnect();
	mMouseMovedCallBack	.disconnect();
	mMouseEndCallBack	.disconnect();
	
	//Release the callback objects
	mMouseBeganCallBack	.release();
	mMouseMovedCallBack	.release();
	mMouseEndCallBack	.release();

	// Remove the pointer to the touch manager
	mTouchManagerPntr=NULL;
}

void Mouse::disconnect(){
	//Disconnect from the mouse signals
	mMouseBeganCallBack	.disconnect();
	mMouseMovedCallBack	.disconnect();
	mMouseEndCallBack	.disconnect();

	// Remove the pointer to the touch manager
	mTouchManagerPntr=NULL;
}
void Mouse::mouseBegan(  cinder::app::MouseEvent &event){

	if(mTouchManagerPntr){
		mTouchManagerPntr->touchesBegan(mTouchId,event.getPos());
	}
	
}
void Mouse::mouseMoved(  cinder::app::MouseEvent &event){

	if(mTouchManagerPntr){
		mTouchManagerPntr->touchesMoved(mTouchId,event.getPos());
	}
}
void Mouse::mouseEnded(  cinder::app::MouseEvent &event){

	if(mTouchManagerPntr){
		mTouchManagerPntr->touchesEnded(mTouchId,event.getPos());
	}
}
