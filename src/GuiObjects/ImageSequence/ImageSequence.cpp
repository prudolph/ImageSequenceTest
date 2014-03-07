/*
 *  ImageSequence.cpp
 *
 */


#include "ImageSequence.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace guiObject;

ImageSequence::ImageSequence() :BaseGuiObject(),
// Max Surfaces and textures the application can handle
mMaxSurfaces (30),
mMaxTextures (10),
mLooping(false),
mPaused(true),
mPlaying(false),
mComplete(false),
mReverse(false)
{

	//how many Frames are in the folder
	mTotalFrames = 0;

	// How many background threads available to load surfaces from disk to memory in the background
	mMaxThreads = 5;

	//Which frame the player is starting at
	mPlayheadPosition = 0;

	//How many frames it shoulde iterate through
	mPlayheadFrameInc = 1;
	mReverse = false;
}

ImageSequence::~ImageSequence(void){
    // stop loader threads and wait for it to finish
    TextureStoreThreadPool::const_iterator itr;
	for (itr = mLoadingThreads.begin(); itr != mLoadingThreads.end(); ++itr) {
        (*itr)->interrupt();
        (*itr)->join();
    }   
}

void ImageSequence::setup(const std::string &imagesFolderPath, cinder::Vec2f position, cinder::Vec2f size, const float &fps){
	
	mFps = fps;
	gui_setPosition(position);
	gui_setSize(size.x, size.y);

	//get the list of files
	loadImageList(imagesFolderPath);

	startLoadingThreads();
	setupFrames();



}
void ImageSequence::shutdown(){
	//stop play back
	mPlaying = false;
	mPaused = false;
	
	//stop threads
	stopLoadingThreads();

	// dump assets
	mFrames.clear();
	mFramesTobeLoadedQueue.clear();

	for (auto &surf: mSurfaceFramesMap){
		surf.second.reset();
	}
	mSurfaceFramesMap.clear();
	
	for (auto &tex : mTextureFramesMap){
		tex.second.reset();
	}
	mTextureFramesMap.clear();

	mCurrentFrameTexture.reset();

}
void ImageSequence::loadImageList( const std::string &imagesFolderPath){
	
	//Get the path for the image sequence
	fs::path mAssetPath( getAssetPath(imagesFolderPath));
	 if( fs::is_directory(mAssetPath) && fs::exists(mAssetPath)){
		 //Go through the directory and find all the images in order
		for ( fs::directory_iterator it( mAssetPath ); it != fs::directory_iterator(); ++it ){
			
			if ( fs::is_regular_file( *it ) ){//if we the file path is acutally a file load the frame object
			
				string fileName =  it->path().filename().string();
				 if( !( fileName.compare( ".DS_Store" ) == 0 ) ){// if its not that damn ds file
					 fs::path fullPath(mAssetPath.generic_string()+"\\"+fileName);
					
					 //create a frame object
					 SequenceFrame frame;
					 frame.frameId =mTotalFrames;
					 frame.imgFullPath = fullPath;
					 frame.onQueue=false;
					 frame.hasSurface=false;
					 frame.hasTexture=false;

					mFrames[frame.frameId] = frame;
					mTotalFrames++;// update the total frames we have in the system

				 }
			}
		}
	}
}

/***
*	This will create the threads that will load the image from disk to memory in the background so the needed frames will have the 
*	surfaces ready to be converted into texturesont he main thread
*/
void ImageSequence::startLoadingThreads(){
    
	mThreadsStopRequested =false;
	int numThreads = mLoadingThreads.size();
	while ((int)mLoadingThreads.size()<mMaxThreads){
		boost::shared_ptr<boost::thread> surfaceLoadingThread=boost::shared_ptr<boost::thread>(new boost::thread(&ImageSequence::threadImageLoad, this));
		mLoadingThreads.push_back(surfaceLoadingThread);
    }
}

void ImageSequence::stopLoadingThreads(){
    mThreadsStopRequested=true;//tell the threads to stop.

	TextureStoreThreadPool::const_iterator itr;
	for (itr = mLoadingThreads.begin(); itr != mLoadingThreads.end(); ++itr) {
        (*itr)->join();
    }
	mLoadingThreads.clear();
}

void ImageSequence::threadImageLoad(){
    
    ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread- This will take care of the leaking memory warning
    
	bool			empty;
	Surface			surface;
	ImageSourceRef	image;
	int				frameId =-1;
    
	// run until interrupted
	while(!mThreadsStopRequested) {  
        // fetch first url from the queue, if any. We put it in a mutex lock so two threads dont try to get the same url at the same time
		mUrlsMutex.lock();
			empty = mFramesTobeLoadedQueue.empty();
			if(!empty){
				frameId = mFramesTobeLoadedQueue.front();// get the first frame on the list
				mFramesTobeLoadedQueue.pop_front();// then remove it from the list
			}
		mUrlsMutex.unlock();
        
		if( frameId != -1 ){
			
			// try to load from relative file
			 try {
				image = loadImage( loadFile(mFrames[frameId].imgFullPath) );
			} catch(exception& e) {
				// All attempts to load the url failed
               // console() << "CANT LOAD IMAGE :"<< e.what() <<endl;
				continue;
			}
            
            // While there are too many surfaces currently loaded- tell this thread to sleep a while and check back
            while(mSurfaceFramesMap.size()>=mMaxSurfaces){
                this_thread::sleep_for(chrono::nanoseconds(10));//Standard thread only takes nanoseconds-wait 1/10 of a second
                this_thread::yield();
				if (mThreadsStopRequested){ break; break; }
            }
            
            // succeeded, check if thread was interrupted
            if(mThreadsStopRequested)break;
            
				// create Surface from the image
				surface = Surface(image);
            
			// check if thread was interrupted
            if(mThreadsStopRequested)break;
            
        	// copy surface to shared  frame object space with the main thread
            mLoadedSurfacesMutex.lock();
				try{
					mSurfaceFramesMap[frameId]= surface;
				} catch(exception& e) {
					// All attempts to load the IMage failed
					//console() << "CANT add image to surfaces map :"<< e.what() <<endl;
					continue;
				}
				mFrames[frameId].hasSurface=true;
				mFrames[frameId].onQueue=false;

			  mLoadedSurfacesMutex.unlock();
        
		}
    }
}


void ImageSequence::setupFrames(){
	int frameIndex=mPlayheadPosition;

	//LoadTextures
	for(frameIndex; frameIndex<mPlayheadPosition+mMaxTextures;frameIndex++){
	
		mTextureFramesMap[frameIndex] = Texture( loadImage( loadFile(		mFrames[frameIndex].imgFullPath ) ) );
		mFrames[frameIndex].hasTexture=true;
	}
	
	for(frameIndex; frameIndex<mPlayheadPosition+mMaxTextures+mMaxSurfaces;frameIndex++){
		mSurfaceFramesMap[frameIndex]= Surface( loadImage( loadFile(      mFrames[frameIndex].imgFullPath) ) );
		mFrames[frameIndex].hasSurface=true;
	}

}

void ImageSequence::preload(){
	int frameIndex=mPlayheadPosition;
	//Make sure textures are loaded;


	//Check if surfaces need to be created;
	for(int frameIndex =  mPlayheadPosition+mMaxTextures; frameIndex<mPlayheadPosition+mMaxTextures+mMaxSurfaces;frameIndex++){
		int currentFrame = frameIndex;
		//COnvert frames surface to a texture
		
		if(currentFrame > mTotalFrames-1){//if Index is before the begining of the sequence move it to the end of the frames
			int diff = currentFrame-mTotalFrames;
			currentFrame=diff;
		}
	
		if( !mFrames[currentFrame].hasSurface &&  !mFrames[currentFrame].onQueue && !mFrames[currentFrame].hasTexture){
			addFrameToSurfaceLoadingQueue( currentFrame );
		}
	}

}
void ImageSequence::update(){
	updatePlayback();
	manageLoadedImages();
}

void ImageSequence::outputHistogram(){
string histogramTexture,histogramSurface,histogramEmpty,histogramFrameID,histogramFramePos;
	histogramTexture="T ";
	histogramSurface="S ";
	histogramEmpty="E ";
	histogramFrameID="I ";
	histogramFramePos="P ";

	map<int, SequenceFrame>::iterator sequenceFrameIter = mFrames.begin();
	for(sequenceFrameIter; sequenceFrameIter!=mFrames.end(); sequenceFrameIter++) {
		if(sequenceFrameIter->second.hasTexture){
			histogramTexture+=" - ";
			histogramSurface+="   ";
			histogramEmpty+="   ";
		}else if(sequenceFrameIter->second.hasSurface ){
			histogramTexture+="   ";
			histogramSurface+=" - ";
			histogramEmpty+="   ";
		}else{
			histogramTexture+="   ";
			histogramSurface+="   ";
			histogramEmpty+=" - ";
		}  
		if(sequenceFrameIter->second.frameId<10){
			histogramFrameID+=" ";
		}
		histogramFrameID+=to_string( sequenceFrameIter->second.frameId)+ " ";
	}
	for(int i =0;i<mPlayheadPosition;i++){
		histogramFramePos+="   ";
	}
	histogramFramePos+=" ^ ";

	console()<<histogramTexture<<endl;
	console()<<histogramSurface<<endl;
	console()<<histogramEmpty<<endl;
	console()<<histogramFrameID<<endl;
	console()<<histogramFramePos<<endl;
	console()<<"--------------------\n\n"<<endl;
}

/**
 *  On each update, 10  surfaces are converted into textures and then removed from the loaded surfaces map.
 */ 
void ImageSequence::manageLoadedImages(){
	//Update frames based on the playheadpsotion

	//empty Frame
	int emptyframePosition = mPlayheadPosition-1;
	if(emptyframePosition < 0){//if position is before the begining of the sequence move it to the end of the frames
		emptyframePosition= mTotalFrames-1;
	}
	emptyFrame(emptyframePosition);
	
	//COnvert frames surface to a texture
	int textureframeIndex = mPlayheadPosition + mMaxTextures;
	if(textureframeIndex > mTotalFrames-1){//if position is before the begining of the sequence move it to the end of the frames
			int diff = textureframeIndex-mTotalFrames;
		textureframeIndex=diff;
	}
	if( mFrames[textureframeIndex].hasSurface &&  !mFrames[textureframeIndex].hasTexture ){
		convertFrameSurfaceToTexture( textureframeIndex );
	}


	//COnvert frames surface to a texture
	int surfaceFrameIndex = mPlayheadPosition + mMaxTextures+mMaxSurfaces;
	if(surfaceFrameIndex > mTotalFrames-1){//if Index is before the begining of the sequence move it to the end of the frames
		int diff = surfaceFrameIndex-mTotalFrames;
		surfaceFrameIndex=diff;
	}
	
	if( !mFrames[surfaceFrameIndex].hasSurface &&  !mFrames[surfaceFrameIndex].onQueue && !mFrames[surfaceFrameIndex].hasTexture){
		addFrameToSurfaceLoadingQueue( surfaceFrameIndex );
	}
	
}

void ImageSequence::emptyFrame(int frameId){
	emptyTexture(frameId);
	emptySurface(frameId);
	mFrames[frameId].onQueue=false;
}

void ImageSequence::emptyTexture(int frameId){
	mTextureFramesMap.erase(frameId);
	mFrames[frameId].hasTexture=false;
}

void ImageSequence::emptySurface(int frameId){

	 mLoadedSurfacesMutex.lock();
		mSurfaceFramesMap.erase(frameId);
	mLoadedSurfacesMutex.unlock();
	mFrames[frameId].hasSurface=false;
	
}

void ImageSequence::addFrameToSurfaceLoadingQueue(int frameId){		

	mLoadedSurfacesMutex.lock();
		mFrames[frameId].onQueue = true;
		mFramesTobeLoadedQueue.push_back(frameId);
	mLoadedSurfacesMutex.unlock();
}

void ImageSequence::convertFrameSurfaceToTexture(int frameId){
		
	if(mFrames[frameId].hasSurface && ! mFrames[frameId].hasTexture){
		Surface s;
		Texture t;
		try {
			s = mSurfaceFramesMap[frameId];
		} catch(exception& e) {
			// All attempts to load the url failed
			console() << "CANT CONVERT SURFACE TO TEXTURE :"<< e.what() <<endl;
		}
		
		t = Texture(s);
		mTextureFramesMap[frameId] = Texture(mSurfaceFramesMap[frameId] );
		mFrames[frameId].hasTexture=true;
		emptySurface(frameId);
	}
}

void ImageSequence::dumpAssets(){
	mFramesTobeLoadedQueue.clear();
	mSurfaceFramesMap.clear();
	mTextureFramesMap.clear();
}




////////////////////////////////////////////////////PLAYBACKFUNCTIONS////////////////////////////////////////

/**
 *  -- Begins playback of sequence
 */
void ImageSequence::play() {
	//startLoadingThreads();
	//preload();

    mPaused = false;
    mPlaying = true;
	gui_setAcceptTouch( true );	
}

/**
 *  -- Stops playback and resets the playhead to zero
 */
void ImageSequence::stop() {
	stopLoadingThreads();

    mPlayheadPosition = 0;
    mPlaying = false;
    mPaused =  false;
	gui_setAcceptTouch( true );	

}

/**
 *  -- Pauses playback
 */
void ImageSequence::pause() {
	//stopLoadingThreads();
    mPaused =  true;
    mPlaying = false;
	gui_setAcceptTouch( true );	
}

/**
 *  -- Seek to a new position in the sequence
 */
void ImageSequence::setPlayheadPosition( int newPosition ){
    mPlayheadPosition = max( 0, min( newPosition, mTotalFrames - 1 ) );
}
/**
 *  -- Seek to a new position in the sequence
 */
void ImageSequence::setPlayheadPositionByPerc( float perc ){
//    perc = max( 0.0f, min( perc, 1.0f ) );
    perc = constrain( perc, 0.0f, 1.0f );
    setPlayheadPosition( perc * (mTotalFrames - 1) );
}

void ImageSequence::updatePlayback(){

	 if(mFps && (getElapsedSeconds()-mStartTime) < 1.0f/mFps ){
        return;
    }
    if( !mPaused && mPlaying ){

       int newPosition = mPlayheadPosition + mPlayheadFrameInc;
        
	   //if weve reached the end decide to restart or stop based on looping
		if( newPosition > mTotalFrames - 1 ){
            if( mLooping ){
                mComplete = false;
                //mPlayheadPosition = newPosition - mTotalFrames;
				mPlayheadPosition = 0;
			} else {
                mComplete = true;
            }
            
        } 
		/*
		else if( newPosition < 0 ) {//if were plaing in reverse....
				if( mLooping ){
					mComplete = false;
					mPlayheadPosition = mTotalFrames - abs( newPosition );
				} else {
					mComplete = true;
				}
        } 
		*/
		else {
            mComplete = false;
            mPlayheadPosition = newPosition;
        }

		if( mTextureFramesMap[mPlayheadPosition])
			mCurrentFrameTexture = mTextureFramesMap[mPlayheadPosition];

    }
    mStartTime = getElapsedSeconds();
}




void ImageSequence::gui_draw(){
	if( mCurrentFrameTexture ){
		gl::color( 1.0f, 1.0f, 1.0f );
		gl::draw(mCurrentFrameTexture, Rectf(gui_getPosition(), gui_getPosition()+gui_getSize()));
	}
}