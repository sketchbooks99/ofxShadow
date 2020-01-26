#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLWindowSettings s;
	s.setSize(1920, 1080);
	s.setGLVersion(3, 2);
	ofCreateWindow(s);

	ofRunApp(new ofApp());

}
