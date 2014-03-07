/*
Bluecadet 2014
*/
#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/app/AppNative.h"
#include <mutex>
#include "BaseGuiObject.h"


class Globals{
public:
	static bool						TESTING;
	static bool						DEBUG_MOUSE_ON;
	static cinder::Vec2f			DEBUG_TRANSLATION;
	static float					DEBUG_SCALE;
	static cinder::Vec2f			DEBUG_POSITION;

	static float					DEBUG_ADA_LINE; //always reference when testing
	
	static  float				    DEBUG_OFFSET_THRESH;
	static  float				    DEBUG_SCROLL_MULTIPLIER;
	static  float				    DEBUG_SCROLL_MOMENTUM;
	static  cinder::Vec2f			DEBUG_OFFSET;

	//sorting tests
	static float					TESTING_NUM_X, TESTING_NUM_Y;

	static std::deque<std::function <void()>> mRenderFunctionQueue;
	static std::mutex renderMutex;
	static void addRenderQueueFunction(std::function <void()> func);
	static void renderFunctionQueue();
	
};
