#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	shadow.setup();
	shadow.setPerspective(false);
	shadow.setSoftShadow(false);

	gui.setup();
	gui.add(isPerspective.set("Perspective", false));
	gui.add(isSoft.set("Soft", false));
	gui.add(blurSize.set("Blur size", 50.0, 0.1, 1000.0));
	gui.add(sigma.set("Blur sigma", 5.0, 1.0, 5.0));
	gui.add(shadowFactor.set("Shadow Factor", 75.0, 1.0, 250.0));
	gui.add(nearClip.set("Near Clip", 100, 10, 1000));
	gui.add(farClip.set("Far Clip", 6000, 1000, 10000));
	gui.add(resolution.set("Resolution", 1920, 480, 3840));

	isPerspective.addListener(this, &ofApp::setPerspective);
	isSoft.addListener(this, &ofApp::setSoft);
	blurSize.addListener(this, &ofApp::setBlurSize);
	sigma.addListener(this, &ofApp::setSigma);
	shadowFactor.addListener(this, &ofApp::setShadowFactor);
	nearClip.addListener(this, &ofApp::setNearClip);
	farClip.addListener(this, &ofApp::setFarClip);
	resolution.addListener(this, &ofApp::setResolution);
}

//--------------------------------------------------------------
void ofApp::update(){
	time = ofGetElapsedTimef();

	shadow.setPosition(ofVec3f(500 * cos(time * 2), 1500, 500 * sin(time * 2)));
	shadow.lookAt(ofVec3f(0.0, 0.0, 0.0));
}

//--------------------------------------------------------------
void ofApp::draw(){
	shadow.beginDepth();
	renderScene();
	shadow.endDepth();

	shadow.begin(cam);
	renderScene();
	shadow.end(cam);

	gui.draw();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	glEnable(GL_DEPTH_TEST);

	float time = ofGetElapsedTimef();

	// Cone
	ofPushMatrix();
	ofTranslate(-500, 300, -500);
	ofRotate(time * 30.0, 1.0, 0.7, 0.4);
	ofDrawCone(200, 300);
	ofPopMatrix();

	// Box
	ofPushMatrix();
	ofTranslate(0, 300, -500);
	ofRotate(time * 30.0, 1.0, 0.7, 0.4);
	ofDrawBox(200, 200, 200);
	ofPopMatrix();

	// IcoSphere
	ofPushMatrix();
	ofTranslate(500, 300, -500);
	ofRotate(time * 30.0, 1.0, 0.7, 0.4);
	ofDrawIcoSphere(200);
	ofPopMatrix();

	// Cylinder
	ofPushMatrix();
	ofTranslate(-500, 300, 500);
	ofRotate(time * 30.0, 1.0, 0.7, 0.4);
	ofDrawCylinder(200, 300);
	ofPopMatrix();

	// Plane
	ofPushMatrix();
	ofTranslate(0, 300, 500);
	ofRotate(time * 30.0, 1.0, 0.7, 0.4);
	ofDrawPlane(300, 300);
	ofPopMatrix();

	// Sphere
	ofPushMatrix();
	ofTranslate(500, 300, 500);
	ofRotate(time * 30.0, 1.0, 0.7, 0.4);
	ofDrawSphere(200);
	ofPopMatrix();

	// Ground
	ofDrawBox(0, -5, 0, 5000, 10, 5000);

	glDisable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
void ofApp::setPerspective(bool& isPerspective) {
	shadow.setPerspective(isPerspective);
}

//--------------------------------------------------------------
void ofApp::setSoft(bool& isSoft) {
	shadow.setSoftShadow(isSoft);
}

//--------------------------------------------------------------
void ofApp::setBlurSize(float& blurSize) {
	shadow.setBlurSize(blurSize);
}

//--------------------------------------------------------------
void ofApp::setSigma(float& sigma) {
	shadow.setSigma(sigma);
}

//--------------------------------------------------------------
void ofApp::setShadowFactor(float& shadowFactor) {
	shadow.setShadowFactor(shadowFactor);
}

//--------------------------------------------------------------
void ofApp::setNearClip(float& nearClip) {
	shadow.setNearClip(nearClip);
}

//--------------------------------------------------------------
void ofApp::setFarClip(float& farClip) {
	shadow.setFarClip(farClip);
}

//--------------------------------------------------------------
void ofApp::setResolution(float& resolution) {
	shadow.setResolution(resolution);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
