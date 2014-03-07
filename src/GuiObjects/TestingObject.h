//
//  TestingObject.h
//  TestingObject
//
//  Created by Paul Rudolph on 7/2/13.
//
//
#include "BaseGuiObject.h"
	typedef std::shared_ptr<class TestingObject>		TestingObjectRef;

class TestingObject : public guiObject::BaseGuiObject{

public:
		TestingObject();
		virtual ~TestingObject();
		
		virtual void setup(cinder::Vec2f pos,cinder::Vec2f size, cinder::ColorA clr );
	
		virtual	void gui_touchesBeganHandler(int touchID, cinder::Vec2f touchPnt);
		virtual void gui_touchesMovedHandler(int touchID, cinder::Vec2f touchPnt);
		virtual void gui_touchesEndedHandler(int touchID, cinder::Vec2f touchPnt);
		virtual void gui_draw();

private:
	bool mHasTouch;
	cinder::Vec2f mCurrentTouchPosition,mPrevTouchPosition;
	float mVerticalThreshHold;
};