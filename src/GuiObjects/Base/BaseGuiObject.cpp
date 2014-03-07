 //
//  BaseGuiObject.cpp
//  BaseGuiObjects
//
//  Created by Paul Rudolph on 7/2/13.
//
//


#include "cinder/gl/gl.h"
#include "TouchManager.h"
#include "BaseGuiObject.h"




using namespace std;
using namespace ci;
using namespace ci::app;


namespace guiObject {


	/*Total object count is a value that is incremented when an gui object is created and decremented when a gui object is destroyed that way we will allways know how many gui objects exist at any moment.
		the gui_ObjectID is only incremented when a new object is created, so any time a new object is created it gets its own unique number. 
	*/

// This keeps track of how many gui objects are currently alive, this number is incremented and decremented 
int		BaseGuiObject::gui_TotalObjectCount = 0;
// This number will only be incremented when a gui object is created, thus making it unique.
int		BaseGuiObject::gui_ObjectID = 0;

GuiObjectMap	BaseGuiObject::gui_UniqueIDLookupMap;


BaseGuiObject::BaseGuiObject() :
gui_Position(Vec2f::zero()),
gui_ParentPosition(Vec2f::zero()),
gui_Width(0.0f),
gui_Height(0.0f),
gui_ObjectColor(ColorA::white()),
gui_TouchesCallbackId(-1),
gui_AcceptTouch(true),
gui_UniqueID(gui_ObjectID),
gui_IsVisible(true),
gui_XLocked(false),
gui_YLocked(false)
{

		gui_TotalObjectCount++;
		gui_ObjectID++;			
		
}

BaseGuiObject::~BaseGuiObject(){
	// remove all children safely
	gui_removeChildren();

	
	gui_TotalObjectCount--;

	// remove from lookup table
	gui_UniqueIDLookupMap.erase(gui_UniqueID);

	
}
void BaseGuiObject::setup(cinder::Vec2f pos,cinder::Vec2f size, cinder::ColorA clr ){
		// store Object in lookup table
		 gui_UniqueIDLookupMap[gui_UniqueID] = GuiObjectWeakRef( shared_from_this() );
			gui_setPosition(pos);
			gui_setSize(size.x,size.y);
			gui_setObjectColor(clr);
			gui_registerWithTouchMngr();
			gui_setAxisLocked(false,false);
}
//THis can be overridden for each subclass object
void BaseGuiObject::gui_draw(){
   
    if(gui_IsVisible){
		gui_drawDebugBox();
		gui_drawChildren();
	}
}

 void  BaseGuiObject::gui_drawDebugBox(){
	gl::lineWidth(1.0f);
	gl::drawStrokedRect(gui_getRect());
}


void BaseGuiObject::gui_drawChildren(){
	   //Draw child Objects if any
   		for (auto childRef : gui_ChildObjects){
			if (childRef) childRef->gui_draw();
		}
}

void BaseGuiObject::gui_setVisible( bool state) {
    gui_IsVisible = state;
	
	for(GuiObjectList::iterator itr=gui_ChildObjects.begin();itr!=gui_ChildObjects.end();++itr) {
		GuiObjectRef obj = std::dynamic_pointer_cast<BaseGuiObject>(*itr);
		if(obj) obj->gui_setVisible(state);
	}


}
void BaseGuiObject::gui_setPosition ( cinder::Vec2f pt,bool overrideLocking ){
	float dx = gui_getPosition().x;
	float dy = gui_getPosition().y;
	if(overrideLocking){
		dx=pt.x;
		dy=pt.y;
	}else{
		if(!gui_XLocked)dx=pt.x;
		if(!gui_YLocked)dy=pt.y;
	}


	gui_Position.value().x =dx;
	gui_Position.value().y =dy;
	
	if (!gui_ChildObjects.empty() )gui_setChildrenPositions();
	

}
void BaseGuiObject::	gui_setChildrenPositions(){
	// Set Children Position
		for (auto childRef : gui_ChildObjects){
			childRef->gui_setParentPosition(gui_getGlobalPosition());
			childRef->gui_setChildrenPositions();
	}
	
}
void BaseGuiObject::	gui_setParentPosition ( cinder::Vec2f pt){
		gui_ParentPosition=pt;
}

void BaseGuiObject::gui_registerWithTouchMngr(){
	TouchManager::getInstance()->registerObjectForInput(shared_from_this());
   
}
void BaseGuiObject::gui_unRegisterWithTouchMngr(){
	TouchManager::getInstance()->unregisterObjectForInput(shared_from_this());	
}
void BaseGuiObject::gui_endTouches(){
	for(int i :gui_ObjectTouchIDs ){
		TouchManager::getInstance()->endTouch(i);
	}

}
bool BaseGuiObject::gui_hasTouchPoint( Vec2f pnt ){
	
	if( gui_getRect().contains(pnt)){
		return true;//point is inside the bounding box 
    }else return false;//point is outside bounding box;

}


void BaseGuiObject::gui_setAcceptTouch(bool state , bool recursive ){
	
		gui_AcceptTouch = state; 
		if (recursive){
			for (guiObject::GuiObjectRef obj : gui_ChildObjects){
				obj->gui_setAcceptTouch(state, true);
			}
		}
}
void BaseGuiObject::gui_addChild(GuiObjectRef obj){
   	if(obj && !hasChild(obj))
	{
		// remove child from current parent
		GuiObjectRef parent = obj->gui_getParent();
		if(parent) parent->gui_removeChild(obj);

		// add to children
		gui_ChildObjects.push_back(obj);

		// set parent
		obj->gui_setParent( shared_from_this() );
		obj->gui_setParentPosition(gui_getGlobalPosition());


	}
}
void BaseGuiObject::gui_removeChild(GuiObjectRef obj){
	
	GuiObjectList::iterator itr = std::find(gui_ChildObjects.begin(), gui_ChildObjects.end(), obj);
	if(itr != gui_ChildObjects.end()) 
	{
		// reset parent
		(*itr)->gui_setParent( GuiObjectRef() );

		// remove from children
		gui_ChildObjects.erase(itr);
	}


}
void BaseGuiObject::gui_removeChildren()
{
	
	for(GuiObjectList::iterator itr=gui_ChildObjects.begin();itr!=gui_ChildObjects.end();)
	{
		// reset parent
		(*itr)->gui_setParent( GuiObjectRef() );

		// remove from children
		itr = gui_ChildObjects.erase(itr);
	}
}

bool BaseGuiObject::hasChild(GuiObjectRef node) const 
{
	GuiObjectList::const_iterator itr = std::find(gui_ChildObjects.begin(), gui_ChildObjects.end(), node);
	return(itr != gui_ChildObjects.end());
}


//ToString
std::string BaseGuiObject::gui_getDebugString(){
	return "BaseGuiObject: UID: "+gui_UniqueID;
}

};
