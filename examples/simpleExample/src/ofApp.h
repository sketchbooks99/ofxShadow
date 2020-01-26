#pragma once

#include "ofMain.h"
#include "ofxShadow.h"
#include "ofxGui.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void renderScene();
		void setPerspective(bool &isSoft);
		void setSoft(bool& isPerspective);
		void setBlurSize(float& blurSize);
		void setSigma(float& sigma);
		void setShadowFactor(float& shadowFactor);
		void setFarClip(float& farClip);
		void setNearClip(float& nearClip);
		void setResolution(float& resolution);

		ofxShadow shadow;
		float time;
		ofEasyCam cam;

		ofxPanel gui;
		ofParameter<bool> isSoft;
		ofParameter<bool> isPerspective;
		ofParameter<float> blurSize;
		ofParameter<float> sigma;
		ofParameter<float> shadowFactor;
		ofParameter<float> farClip;
		ofParameter<float> nearClip;
		ofParameter<float> resolution;
		
};
