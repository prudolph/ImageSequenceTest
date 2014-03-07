//
//  TouchManager.h
//  cinder_gui_interface
//
//  Created by Paul Rudolph on 8/20/13.
//
//
#pragma once
#include "cinder/app/AppBasic.h"
#include  <boost/thread.hpp>

#include "BaseGuiObject.h"
 
class TouchManager{
    
public:
		struct TouchObject{
			int touchId;
			cinder::Vec2f touchPnt;
			guiObject::GuiObjectRef touchingObjectPntr;
		};

    static TouchManager* getInstance();
    
    void drawTouches(); //This can be called from anywhere and draws all the ojects
    
	void touchesBegan(int touchID, cinder::Vec2f touchPnt);
    void touchesMoved(int touchID, cinder::Vec2f touchPnt);
    void touchesEnded(int touchID, cinder::Vec2f touchPnt);
	
	void registerObjectForInput(guiObject::GuiObjectRef obj);
	void unregisterObjectForInput(guiObject::GuiObjectRef obj);

	cinder::Vec2f getCurrentTouchPoint(int touchId);
    void endTouch(int touchID);
	void reassignTouch(int touchID,guiObject::GuiObjectRef obj );
private:
    //These are private to make the Touchmanager a singleton
    TouchManager();
    ~TouchManager();
    
    //Private functions
    void setup();

    guiObject::GuiObjectRef findTouchingObject(TouchObject &touchObj);
    
	static TouchManager* mManagerInstance;
	boost::mutex mTouchMapLock;
	std::map<int,TouchObject>mTouchMap;
	std::deque<guiObject::GuiObjectRef>mRegisteredObjectsDeque;
};