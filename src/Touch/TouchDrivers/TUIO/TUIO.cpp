
#include "Tuio.h"

using namespace std;
using namespace ci;
using namespace ci::app;

Tuio::Tuio(){
		mTouchManagerPntr=NULL;
		
}

void Tuio::connect(TouchManager *touchMngerPntr){
	//Connect to the application window touch event signals
	ci::app::WindowRef window = cinder::app::getWindow();

	 //Touch Events
	mTuioConnection.connect();
    mTuioTouchBeganCallBack =  mTuioConnection.registerTouchesBegan(this, &Tuio::tuioTouchesBegan);
    mTuioTouchMovedCallBack =  mTuioConnection.registerTouchesMoved(this, &Tuio::tuioTouchesMoved);
    mTuioTouchEndCallBack   =  mTuioConnection.registerTouchesEnded(this, &Tuio::tuioTouchesEnded);


	// Connect to the Touch Manager
	mTouchManagerPntr = touchMngerPntr;
}

Tuio::~Tuio(){
	//disconnect();
}

void Tuio::disconnect(){
	//Disconnect from the TUIO TOUCH signals
	try{
		mTuioConnection.unregisterTouchesBegan(mTuioTouchBeganCallBack);
		mTuioConnection.unregisterTouchesMoved(mTuioTouchMovedCallBack);
		mTuioConnection.unregisterTouchesEnded(mTuioTouchEndCallBack);
	}catch(...){
		console()<<"Couldnt disconnect TUIO From the touch manager. Touch manager may have already been deleted"<<endl;
	}
	// Remove the pointer to the touch manager
	mTouchManagerPntr=NULL;
}

   
void Tuio::tuioTouchesBegan(cinder::app::TouchEvent event){
	//console()<<"TUIO touches BEGAN"<<endl;
	if(mTouchManagerPntr){
		for(TouchEvent::Touch touch :event.getTouches() ){
			mTouchManagerPntr->touchesBegan(touch.getId(),touch.getPos());
		}
		
	}
}
void Tuio::tuioTouchesMoved(cinder::app::TouchEvent event){
	//console()<<"TUIO touches MOVED"<<endl;
	if(mTouchManagerPntr){
		for(TouchEvent::Touch touch :event.getTouches() ){
			mTouchManagerPntr->touchesMoved(touch.getId(),touch.getPos());
		}
	}
}
void Tuio::tuioTouchesEnded(cinder::app::TouchEvent event){
	//console()<<"TUIO touches ENDED"<<endl;
		
	if(mTouchManagerPntr){
		for(TouchEvent::Touch touch :event.getTouches() ){
			mTouchManagerPntr->touchesEnded(touch.getId(),touch.getPos());
		}
	}
}