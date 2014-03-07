#include "TestingObject.h"
#include "TouchManager.h"

//#include "..\Globals.h"
#include "cinder\Rand.h"
using namespace std;
using namespace ci;
using namespace ci::app;
using namespace guiObject;

TestingObject::TestingObject(): BaseGuiObject(),
	mHasTouch(false),
	mCurrentTouchPosition(Vec2f::zero()),
	mPrevTouchPosition(Vec2f::zero()),
	mVerticalThreshHold(2.0f)
{

	
}
TestingObject:: ~TestingObject(){
	console()<<"Deleting Testing object"<<endl;
}
 
void TestingObject::setup(cinder::Vec2f pos,cinder::Vec2f size, cinder::ColorA clr ){
		// store Object in lookup table
		 gui_UniqueIDLookupMap[gui_getObjectID()] = GuiObjectWeakRef( shared_from_this() );
			gui_setPosition(pos);
			gui_setSize(size.x,size.y);
			
			int object_movement_case = randInt(0,3);
			if(object_movement_case==0){//x locked
				gui_setAxisLocked(true,false);
				gui_setObjectColor(ColorA(1,1,0));
			
			}else if(object_movement_case==1){//y locked
				gui_setAxisLocked(false,true);
					gui_setObjectColor(ColorA(1,0,0));
			}else{//Free movement
				gui_setAxisLocked(false,false);
					gui_setObjectColor(ColorA(0,1,0));
			
			}

	

			gui_registerWithTouchMngr();
		

}

void	TestingObject::			gui_touchesBeganHandler(int touchID, cinder::Vec2f touchPnt){
	
	//THis object only takes one touch
	if(gui_ObjectTouchIDs.empty()){
		console()<<"Object has touch : "<<endl;
	for(int i :gui_ObjectTouchIDs ){
		console()<<"Touch ID "<< i<<endl;
	}
		gui_ObjectTouchIDs.push_back(touchID);
		mHasTouch=true;
		//Since we are going to handle moving we need to keep track of where the point is, vs a button that just needs to know if there is a touch
		//gui_setPosition(touchPnt);
		mCurrentTouchPosition= touchPnt;
	}
	
}
void	TestingObject::			gui_touchesMovedHandler(int touchID, cinder::Vec2f touchPnt){


	int currentTouchId = gui_ObjectTouchIDs.front();
	if(currentTouchId ==touchID ){
			mPrevTouchPosition=mCurrentTouchPosition;
			mCurrentTouchPosition = touchPnt;
			Vec2f offsetAmount = mCurrentTouchPosition -mPrevTouchPosition;//Get the difference between the PREV touch point and the moved touch point


		
			gui_setOffset(offsetAmount);	//update my postion
		
		
	}
}
void	TestingObject::			gui_touchesEndedHandler(int touchID, cinder::Vec2f touchPnt){
	console()<<"TESTING OBJECT: "<< gui_getObjectID() <<"touches ENED "<<endl;
	int currentTouchId = gui_ObjectTouchIDs.front();

	if(currentTouchId ==touchID ){
		mHasTouch=false;
		gui_ObjectTouchIDs.clear();
	}
}

void TestingObject::gui_draw(){

	gui_drawDebugBox();
	if(mHasTouch){
		gl::color(0,1,0);
		gl::drawSolidCircle(gui_getCenter()+gui_ParentPosition,20.0f);
		

	}
			
	gui_drawChildren();
}