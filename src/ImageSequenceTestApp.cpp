#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "Globals.h"

//Touch Headers
#include "TouchManager.h"
#include "Mouse.h"
#include "TUIO.h"


//ImageSequence
#include "ImageSequence.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace guiObject;

class ImageSequenceTestApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	params::InterfaceGlRef	mParams;
	void keyDown(KeyEvent event);

private:
	Mouse mMouseConnection;
	Tuio  mTuioConnection;
	vector< std::shared_ptr<ImageSequence>>			mTestingImageSequences;
	int targetSequence;
};


void ImageSequenceTestApp::keyDown(KeyEvent event) {
	if (event.getChar() == 'q')quit();




	if (event.getChar() == 'p'){
		if (targetSequence - 1 < mTestingImageSequences.size()){
			mTestingImageSequences[targetSequence - 1]->play();
		}

	}


	if (event.getChar() == 's'){
		if (targetSequence - 1 < mTestingImageSequences.size()){
			mTestingImageSequences[targetSequence - 1]->pause();
		}


	}


	if (event.getChar() == '1')targetSequence = 1;
	if (event.getChar() == '2')targetSequence = 2;
	if (event.getChar() == '3')targetSequence = 3;
	if (event.getChar() == '4')targetSequence = 4;
	if (event.getChar() == '5')targetSequence = 5;
	if (event.getChar() == '6')targetSequence = 6;
}

void ImageSequenceTestApp::setup()
{

	mParams = params::InterfaceGl::create(getWindow(), "App parameters", toPixels(Vec2i(200, 100)));

	mParams->addParam("DEBUG SCALE", &Globals::DEBUG_SCALE, "min=-1000.0  step=0.1");
	mParams->addParam("DEBUG TRANSLATE X", &Globals::DEBUG_TRANSLATION.x, "min=-10000.0  step=1");
	mParams->addParam("DEBUG TRANSLATE Y", &Globals::DEBUG_TRANSLATION.y, "min=-10000.0  step=1");

	mParams->addSeparator();
	mParams->addParam("TESTING_NUM_X", &Globals::TESTING_NUM_X, "min=-1000, step=1");
	mParams->addParam("TESTING_NUM_Y", &Globals::TESTING_NUM_Y, "min=-1000, step=1");

	if (Globals::DEBUG_MOUSE_ON){
		//Setup Touch interaction
		mMouseConnection.connect(TouchManager::getInstance());
	}
	else{
		mTuioConnection.connect(TouchManager::getInstance());
	}

	targetSequence = 1;

	for (int i = 0; i < 4; i++){

		std::shared_ptr<ImageSequence> seq = std::shared_ptr<ImageSequence>(new ImageSequence());

		fs::path imgSeqPath("imgseq_resized" + to_string(i));
		console() << "imgSeqPath " << imgSeqPath << endl;
		seq->setup(imgSeqPath.generic_string(), Vec2f(50.0f + i * 1080.0f, (i>3) ? 800.0f : 0.0f), Vec2f(1080.0f, 710.0f), 15.0f);
		console() << "Finished loading" << endl;
		mTestingImageSequences.push_back(seq);
	}

}

void ImageSequenceTestApp::mouseDown( MouseEvent event )
{
}

void ImageSequenceTestApp::update()
{
	Globals::renderFunctionQueue();
	for (auto seq : mTestingImageSequences){
		seq->update();
	}
}

void ImageSequenceTestApp::draw()
{
	// clear out the window with black
	gl::clear(Color(0.0f, 0.0f, 0.0f));

	if (Globals::TESTING){
		gl::pushMatrices();
		gl::scale(Globals::DEBUG_SCALE, Globals::DEBUG_SCALE);
		gl::translate(Globals::DEBUG_TRANSLATION);
	}
	for (auto seq : mTestingImageSequences){
		seq->gui_draw();
	}

	//Draw params
	mParams->draw();

	if (Globals::TESTING){
		gl::popMatrices();

		//Draw Framerate
		gl::drawString(to_string(getAverageFps()), Vec2f(0.0f, 0.0f), ColorA(1.0f, 0.0f, 0.0f, 1.0f), Font("Arial", 20.0f));
	}

}

CINDER_APP_NATIVE( ImageSequenceTestApp, RendererGl )
