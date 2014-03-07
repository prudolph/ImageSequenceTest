//
//  BaseButton.cpp
//  BaseButton
//
//  Created by Paul Rudolph on 7/2/13.
//
//

#include "BaseButton.h"

using namespace std;
using namespace ci;
using namespace ci::app;

BaseButton::BaseButton() :BaseGuiObject(),
mTouchCanceled(false)
{

}
BaseButton::~BaseButton(){
}

void BaseButton::setup(cinder::Vec2f pos, cinder::Vec2f size, cinder::ColorA clr, std::function <void(guiObject::GuiObjectRef)>callBackFn){
	gui_registerWithTouchMngr();
	gui_setPosition(pos);
	gui_setSize(size.x, size.y);
	gui_setObjectColor(clr);
	setCallBackFn(callBackFn);
}

void BaseButton::gui_touchesBeganHandler(int touchID, cinder::Vec2f touchPnt){
	//keep track of the touch position so we can later test how far it has moved
	//THis object only takes one touch
	if (gui_ObjectTouchIDs.empty()){
		gui_ObjectTouchIDs.push_back(touchID);
	}
}

void BaseButton::gui_touchesMovedHandler(int touchID, cinder::Vec2f touchPnt){
	//if the touch point is out of the space of the button = touch canceled.
	if (!gui_ObjectTouchIDs.empty()){
		int currentTouchId = gui_ObjectTouchIDs.front();
		if ((currentTouchId == touchID) && (!gui_getRect().contains(touchPnt))){

			//went out of bounds, to the touch was canceled
			mTouchCanceled = true;

			//the button no longer has this touch
			gui_ObjectTouchIDs.clear();
		}
	}
}

void BaseButton::gui_touchesEndedHandler(int touchID, cinder::Vec2f touchPnt){

	if (!gui_ObjectTouchIDs.empty()){
		int currentTouchId = gui_ObjectTouchIDs.front();
		if (currentTouchId == touchID){
			//clear the touch ids
			gui_ObjectTouchIDs.clear();
		}

		//if the touch wasn't canceled already by going out of bounds, then trigger the function call
		if (!mTouchCanceled){
			//Fire off the selected signal here
			try{
				mOnSelectSignal(shared_from_this());
			}
			catch (...){
				console() << "Missing Function to call" << endl;
			}
		}
		mTouchCanceled = false;
	}

}
void BaseButton::gui_draw(){
	gl::color(gui_getObjectColor());
	gui_drawDebugBox();

	if (!gui_ObjectTouchIDs.empty()){
		gl::color(0, 1, 0);
		gl::drawSolidCircle(gui_getCenter(), 50.0f);
	}
}



