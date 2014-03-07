//
//  TouchManager.cpp
//  cinder_gui_interface
//
//  Created by Paul Rudolph on 8/20/13.
//
//

#include "TouchManager.h"
#include "boost/lexical_cast.hpp"
#include "..\Globals.h"

using namespace std;
using namespace ci;
using namespace ci::app;






TouchManager* TouchManager::mManagerInstance=NULL;

TouchManager::TouchManager(){
	
}

TouchManager::~TouchManager(){
	console()<<"Touch Manager delete"<<endl;
	mRegisteredObjectsDeque.clear();
	mTouchMap.clear();

}

TouchManager* TouchManager::getInstance(){
    //If the manager is not set up yet. Create it and return a pointer to the caller
    if(!mManagerInstance){
        mManagerInstance = new TouchManager();
        mManagerInstance->setup();
             
    }
    return mManagerInstance;
}

void TouchManager::setup(){

}

void TouchManager::drawTouches(){
		
	
	if(	mTouchMapLock.try_lock()){
		for (auto& t : mTouchMap) {
				int touchId = t.first;
				Vec2f touchPnt =  t.second.touchPnt;
				//console()<<"Touch: " <<touchId << " @ " << touchPnt << std::endl;
				gl::color(0,1,0);
				gl::lineWidth(1.0f);
				gl::drawStrokedCircle(touchPnt,25.0f);
		
				//gl::drawString(to_string(touchId),touchPnt-Vec2f(-1.0f,+40.0),cinder::Color(1,0,0),Font("arial",12.0f));
			}
		mTouchMapLock.unlock();
	}
}





void TouchManager::touchesBegan(int touchID, cinder::Vec2f touchPnt){

			if(Globals::TESTING){
			touchPnt =touchPnt*(1/Globals::DEBUG_SCALE)-(Globals::DEBUG_TRANSLATION);
		}


  	
	//Initialize a new touch object 
	TouchObject touch;
	touch.touchId			= touchID;
	touch.touchPnt			= touchPnt;
	touch.touchingObjectPntr;

	// Add this touch to the touch map all touch objects are added to the map regardless if they are touching an object
	mTouchMap[touch.touchId]	=touch;

	mTouchMapLock.lock();
		//Find if this touch point is touching an object
		 guiObject::GuiObjectRef foundObject=findTouchingObject(touch);
		 
		 if(foundObject){
			 // if we found an object set the touch objects pointer to the found object
			mTouchMap[touchID].touchingObjectPntr=foundObject;
			// call the touch began on the found object
			mTouchMap[touchID].touchingObjectPntr->gui_touchesBeganHandler(touchID,touchPnt);
		 }
		
	mTouchMapLock.unlock();
}


void TouchManager::touchesMoved(int touchID, cinder::Vec2f touchPnt){

	if(Globals::TESTING){
			touchPnt =touchPnt*(1/Globals::DEBUG_SCALE)-(Globals::DEBUG_TRANSLATION);
		}

	mTouchMapLock.lock();


			//update the touch's position
			mTouchMap[touchID].touchPnt= touchPnt;

			//if this touch is actually touching an object
			if(mTouchMap[touchID].touchingObjectPntr){
				//lookup who is being touched and let it know that their point has moved
				mTouchMap[touchID].touchingObjectPntr->gui_touchesMovedHandler(touchID,touchPnt);
			}
	
	mTouchMapLock.unlock();
}

void TouchManager::touchesEnded(int touchID, cinder::Vec2f touchPnt){
		if(Globals::TESTING){
			touchPnt =touchPnt*(1/Globals::DEBUG_SCALE)-(Globals::DEBUG_TRANSLATION);
		}

	mTouchMapLock.lock();
		//If the touch has a touching object, call the objects touches ended handler
		if(mTouchMap[touchID].touchingObjectPntr){
			mTouchMap[touchID].touchingObjectPntr->gui_touchesEndedHandler(touchID,touchPnt);
		}
		//remove the touch object from the map.
		mTouchMap.erase(touchID);
	mTouchMapLock.unlock();
}

void TouchManager::endTouch(int touchID){
	mTouchMapLock.lock();
		mTouchMap[touchID].touchingObjectPntr=guiObject::GuiObjectRef();
	mTouchMapLock.unlock();
}

void TouchManager::reassignTouch(int touchID,guiObject::GuiObjectRef obj ){

		mTouchMap[touchID].touchingObjectPntr=obj;
		obj->gui_touchesBeganHandler(touchID, mTouchMap[touchID].touchPnt);

}
void TouchManager::registerObjectForInput(guiObject::GuiObjectRef obj){
	mRegisteredObjectsDeque.push_front(obj);
}
void TouchManager::unregisterObjectForInput(guiObject::GuiObjectRef obj){
	
	std::deque<guiObject::GuiObjectRef>::iterator itr;
	int position =0;
	bool found=false;
	for(std::deque<guiObject::GuiObjectRef>::iterator itr=mRegisteredObjectsDeque.begin();itr!=mRegisteredObjectsDeque.end();++itr) {

		if((*itr)->gui_getObjectID() == obj->gui_getObjectID()){
			found=true;
			break;
		}
				position++;
	}
	if(found){
		mRegisteredObjectsDeque.erase(mRegisteredObjectsDeque.begin()+position);	
	}
		
		
}


guiObject::GuiObjectRef TouchManager::findTouchingObject(TouchObject &touchObj){	 

	/*
	we need to decide who gets the touch, and not based on who was registered with the touch manager first. That is what the current setup is doing.
	do we first loop through a
	
	
	*/

	//loop through all the registered object
	std::deque<guiObject::GuiObjectRef>::iterator itr;
	for(itr=mRegisteredObjectsDeque.begin();itr!=mRegisteredObjectsDeque.end();++itr){

		//get the pointer to the gui object
		guiObject::GuiObjectRef curObject = *itr;
		
		//check if the object has the touch point
		if(curObject->gui_hasTouchPoint(touchObj.touchPnt) && curObject->gui_isAcceptingTouch() ){	
			return curObject;
		}
	}
	return NULL;
}

cinder::Vec2f TouchManager::getCurrentTouchPoint(int touchId){
	
	Vec2f pnt= Vec2f(-1,-1);
	std::map<int,TouchObject>::iterator itr = mTouchMap.find(touchId);
	if(itr!=mTouchMap.end()){
		pnt=mTouchMap[touchId].touchPnt;
	}
	return pnt;
	

}