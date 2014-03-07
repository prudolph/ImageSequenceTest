/*
 *  ImageSequence.h
 *
 * 
 *
 */

#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include  <boost/thread.hpp>
#include "BaseGuiObject.h"


using namespace ci;
using namespace ci::app;
using namespace std;

typedef std::vector< boost::shared_ptr<boost::thread> > TextureStoreThreadPool;


class ImageSequence: public guiObject::BaseGuiObject {
	
	struct SequenceFrame{
		int frameId;
		fs::path imgFullPath;
		bool hasTexture,hasSurface,onQueue;
	};

	public:
		ImageSequence();
		~ImageSequence();
		//Setup Functions
		void setup(const std::string &imagesFolderPath,cinder::Vec2f position , cinder::Vec2f size,const float &fps = 0.0f );
		void shutdown();
		void loadImageList(const std::string &imagesFolderPath);
		void startLoadingThreads();     
		void stopLoadingThreads();
		void threadImageLoad();
		void setupFrames();
		void preload();
	
		void update();
		void manageLoadedImages();

		void convertFrameSurfaceToTexture(int frameId);
		void addFrameToSurfaceLoadingQueue(int frameId);
		void loadFrameTexture(int frameId);

		void emptyFrame(int frameId);
		void emptyTexture(int frameId);
		void emptySurface(int frameId);


	
		void dumpAssets();
		
		void outputHistogram();

		//PlayBackFunctions
		void setFps(float fps){ mFps = fps; };
		void updatePlayback();
		void play();
		void stop();
		void pause();

		virtual void gui_draw();
			
		int  getPlayheadFrameInc() const { return mPlayheadFrameInc; };
		void setPlayheadFrameInc( int frameInc ) { mPlayheadFrameInc = frameInc; };
		
		int  getPlayheadPosition() const { return mPlayheadPosition; };
		void setPlayheadPosition( int newPosition );
		void setPlayheadPositionByPerc( float perc );

		void setLooping( bool doLoop ) { mLooping = doLoop; };
		 
		//Status Checks
		bool isPlaying()		{ return mPlaying; }
		bool isComplete()const	{ return mComplete;};

		int getTotalFrames()const{ return mTotalFrames; };
  
private:

	//Image loading 

	boost::mutex                mUrlsMutex,mLoadedSurfacesMutex;
	map<int, SequenceFrame>		mFrames;
	deque<int>					mFramesTobeLoadedQueue;
    map<int, Surface>			mSurfaceFramesMap;
	map<int, gl::Texture>       mTextureFramesMap;
	gl::Texture					mCurrentFrameTexture;
	
	
    TextureStoreThreadPool				mLoadingThreads;
	TextureStoreThreadPool				mUpdateThread;
	bool 								mThreadsStopRequested;
	
	int		mMaxSurfaces,
			mMaxTextures,
			mMaxThreads,
			mTotalLoadedSurfaces,
			mTotalLoadedTextures;

	 //PlayBack
	int		mPlayheadPosition,
			mPlayheadFrameInc,
			mTotalFrames;

    bool	mLooping,
			mPaused,
			mPlaying,
			mComplete,
			mReverse;
    double  mStartTime;
    float	mFps;

	cinder::Rectf mScreenSize;


};