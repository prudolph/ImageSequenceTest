//
//  BaseButton.h
//  BaseButton
//
//  Created by Paul Rudolph on 7/2/13.
//
//

#pragma once
#include "cinder/app/AppNative.h"
#include "BaseGuiObject.h"

class BaseButton :public guiObject::BaseGuiObject{

public://anyone can get access to this stuff with the "." accessor
	BaseButton();
	virtual ~BaseButton();
	virtual void				setup(cinder::Vec2f pos, cinder::Vec2f size, cinder::ColorA clr, std::function <void(guiObject::GuiObjectRef)>callBackFn = std::function <void(guiObject::GuiObjectRef)>());
	//Drawing Functions
	virtual void				gui_draw();

	virtual	void				gui_touchesBeganHandler(int touchID, cinder::Vec2f touchPnt);
	virtual void				gui_touchesMovedHandler(int touchID, cinder::Vec2f touchPnt);
	virtual void				gui_touchesEndedHandler(int touchID, cinder::Vec2f touchPnt);

	void						setCallBackFn(std::function <void(guiObject::GuiObjectRef)> fn)     { mOnSelectSignal.connect(fn); };
	boost::signals2::signal<void(guiObject::GuiObjectRef)>&		gui_getSelectedSignal()				{ return mOnSelectSignal; }

		
protected:
	bool													mTouchCanceled;
	boost::signals2::signal<void(guiObject::GuiObjectRef)>  mOnSelectSignal;

};