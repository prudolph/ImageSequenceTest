/*
Bluecadet 2014
*/

#include "Globals.h"
#include "cinder/app/AppNative.h"

//For splitting strings
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string.hpp>



using namespace ci;
using namespace ci::app;
using namespace std;


//TESTING
bool  Globals::TESTING = true;
bool  Globals::DEBUG_MOUSE_ON = true;
float Globals::DEBUG_SCALE = 0.5f;
Vec2f Globals::DEBUG_TRANSLATION = Vec2f(0.0f, 0.0f);
Vec2f Globals::DEBUG_POSITION = Vec2f(0.0f, 0.0f);
float Globals::DEBUG_ADA_LINE = 100.0f;//fake for now

float Globals::DEBUG_OFFSET_THRESH = 8.0f;
float Globals::DEBUG_SCROLL_MULTIPLIER = 0.7f;

float Globals::DEBUG_SCROLL_MOMENTUM= 2.0f;
cinder::Vec2f Globals::DEBUG_OFFSET = cinder::Vec2f(0,0);


//SORTING TESTING

float Globals::TESTING_NUM_X = 0.0f;
float Globals::TESTING_NUM_Y = 0.0f;


std::deque<std::function <void()>> Globals::mRenderFunctionQueue;

std::mutex Globals::renderMutex;

void Globals:: addRenderQueueFunction(std::function <void()> func){
	bool locked = renderMutex.try_lock();
	if (locked){
		mRenderFunctionQueue.push_back(func);
		renderMutex.unlock();
	}
}
void Globals::renderFunctionQueue(){

	if (!mRenderFunctionQueue.empty()){
		
		bool locked = renderMutex.try_lock();
		if (locked){
			for (std::function <void()> func : mRenderFunctionQueue){
				func();
			}
			
			mRenderFunctionQueue.clear();

			renderMutex.unlock();
		}
	}
}


