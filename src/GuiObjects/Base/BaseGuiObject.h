//
//  BaseGuiObject.h
//  BaseGuiObject
//
//  Created by Paul Rudolph on 7/2/13.
//
//

#pragma once
#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"// this is needes so position and size variables can be animated.

namespace guiObject {
	
	typedef std::shared_ptr<class BaseGuiObject>		GuiObjectRef;
	typedef std::shared_ptr<const class BaseGuiObject>	GuiObjectConstRef;
	typedef std::weak_ptr  <class	BaseGuiObject>		GuiObjectWeakRef;
	typedef std::deque     <GuiObjectRef>				GuiObjectList;
	typedef std::map       <int, GuiObjectWeakRef>		GuiObjectMap;
	
	class BaseGuiObject : public std::enable_shared_from_this<BaseGuiObject> {
   
	public://anyone can get access to this stuff with the "." accessor
		BaseGuiObject();
		virtual ~BaseGuiObject();
		virtual void setup(cinder::Vec2f pos,cinder::Vec2f size, cinder::ColorA clr );
		//Drawing Functions
		virtual void gui_draw();
		virtual void gui_drawChildren();
		//Draws an the outer box of the object, and the objects tostring in the center.
		virtual void gui_drawDebugBox();
    

		//Positioning Functions
		virtual void				gui_setPosition ( cinder::Vec2f pt,bool overrideLocking = false);
		cinder::Vec2f				gui_getPosition () const                                { return gui_Position; }
		virtual void				gui_setParentPosition ( cinder::Vec2f pt);
		cinder::Vec2f				gui_getParentPosition () const                          { return gui_ParentPosition; }
		virtual	void				gui_setChildrenPositions();
		cinder::Vec2f				gui_getGlobalPosition()									{ return gui_ParentPosition.value() + gui_Position.value();};
		virtual	void				gui_setOffset(cinder::Vec2f pt)						    {  gui_setPosition( pt+gui_getPosition());};
		
		//Size
		virtual void                gui_setSize( float w, float h )							{ gui_Width = w; gui_Height = h; }
		cinder::Vec2f				gui_getSize()											{ return ci::Vec2f(gui_Width, gui_Height); }
		float						gui_getWidth()											{ return gui_Width; }
		float						gui_getHeight()											{ return gui_Height; }
		
		//Get position and size as a rectObject
		cinder::Rectf				gui_getRect()											{ return  cinder::Rectf(gui_Position.value().x + gui_ParentPosition.value().x,
																													gui_Position.value().y + gui_ParentPosition.value().y,
																													gui_Position.value().x + gui_ParentPosition.value().x + gui_Width,
																													gui_Position.value().y + gui_ParentPosition.value().y + gui_Height); }

		cinder::Vec2f				gui_getCenter()											 { return gui_getRect().getCenter();}
    
		//Color
		virtual void                gui_setObjectColor(  cinder::ColorA &clr )				{ gui_ObjectColor = clr; };
		cinder::ColorA              gui_getObjectColor()									{ return gui_ObjectColor; };
	
		//Visible
		
		virtual void                gui_setVisible( bool state = true );						        
		bool                        gui_isVisble()											{ return gui_IsVisible; }
		//Accept Touch
		virtual void                gui_setAcceptTouch(bool state, bool recursive = false);	     
		bool                        gui_isAcceptingTouch()									{ return gui_AcceptTouch; }
    
		//ID TAG
		int                         gui_getObjectID()										{ return gui_UniqueID; }

		//Touches
		virtual void                gui_registerWithTouchMngr();
		virtual void                gui_unRegisterWithTouchMngr();
	 
		virtual bool                gui_hasTouchPoint(  cinder::Vec2f pnt );
		int                         gui_getNumTouches()										{ return gui_ObjectTouchIDs.size(); }
		
		virtual void				gui_endTouches();
		
		virtual	void				gui_touchesBeganHandler(int touchID, cinder::Vec2f touchPnt){};
		virtual void				gui_touchesMovedHandler(int touchID, cinder::Vec2f touchPnt){};
		virtual void				gui_touchesEndedHandler(int touchID, cinder::Vec2f touchPnt){};


		//Movement 

		virtual void				gui_setAxisLocked(  bool xAxisLocked, bool yAxisLocked ){ gui_XLocked=xAxisLocked; gui_YLocked = yAxisLocked;};
		bool						gui_isXAxisLocked()												{ return gui_XLocked;};
		bool						gui_isYAxisLocked()												{ return gui_YLocked; };
  
    

		//Parent / Child Relationships

		//sets the node's parent node (using weak reference to avoid objects not getting destroyed)
		virtual void                gui_setParent( GuiObjectRef obj )					{ gui_ParentObject = GuiObjectWeakRef(obj);};
		//returns the node's parent node 
		GuiObjectRef				gui_getParent() const								{ return gui_ParentObject.lock(); }
		virtual void                gui_addChild(GuiObjectRef obj); 
		//returns wether this node has a specific child
		bool hasChild(GuiObjectRef obj) const ;

		virtual void				gui_removeChild(GuiObjectRef obj);
		virtual void				gui_removeChildren();

    
     
		//Public Class members - these are public to allow for use with timeline
		cinder::Anim<cinder::Vec2f>         gui_Position,
											gui_ParentPosition;
    
		cinder::Anim<float>                 gui_Width,
											gui_Height;
    
		

   
		//TOString - prints what the object actully is 

		virtual std::string gui_getDebugString();

		GuiObjectList		gui_ChildObjects; //testing this here for access on flags

	protected://Only children of this class have access to these variables, to allow access use "->" acessor(i.e make an accessor method)
    
   
    
		//Text Objects
		cinder::ColorA      gui_ObjectColor;


	   //Parent child relations
		GuiObjectWeakRef	gui_ParentObject;
//		GuiObjectList		gui_ChildObjects;
		
		//String for debug purposes
		std::string gui_ToString;
		//A vector containing the touch ids of the touches objects that are touching the guiObject
		std::vector<int> gui_ObjectTouchIDs;
		
		
		// gui_uniqueIDLookup allows us to quickly find a Node by id
		static GuiObjectMap	gui_UniqueIDLookupMap;
	private://No one other than this class can access these variables

    
		bool               gui_AcceptTouch;

		//Object Ids
		int				   gui_UniqueID,
						   gui_TouchesCallbackId;

		//Movement
		 bool				gui_XLocked,
							gui_YLocked; // IS the object allowed to move

		 //Visibility
		 bool				gui_IsVisible;

		 
		 

	//Static Memebers
	//gui_TotalObjectCount is used to count the number of Object instances for debugging purposes
	static int				gui_TotalObjectCount;
	// gui_ObjectID is used to generate new unique id's any time a gui object is created. 
	static int				gui_ObjectID;

	
	

};

}//End BaseGuiObject namespace