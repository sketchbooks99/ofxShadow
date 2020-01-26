#pragma once

#include "ofMain.h"

#define STRINGIFY(A) #A

class ofxShadow {
public:
    ofxShadow();

    void setup();

    // Initial settings of camera
    void setupCamera();

    // Initial settings of FBO
    void setupFBOs();

    // Initial settings of
    void setupShaders();

    // bake depth information to depth map
    void beginDepth();
    void endDepth();

    // shadow mapping 
    void begin(ofCamera& cam);
    void end(ofCamera& cam);

    // for soft shadow
    void blur();

    // debug textures
    void debug(int x, int y);

    void setPerspective(bool _perspective);
    bool isPerspective() { return perspective; }

	void setSoftShadow(bool _softshadow) { softshadow = _softshadow; }
    bool isSoftShadow() { return softshadow; }

    void setResolution(float _resolution);
    float getResolution() { return resolution; }

    void setScale(float _scale);
    float getScale() { return scale; }

    void setShadowFactor(float _shadowFactor) { shadowFactor = _shadowFactor; }
    float getShadowFactor() { return shadowFactor; }

    void setBlurSize(float _blurSize) { blurSize = _blurSize; }
    float getBlurSize() { return blurSize; }

    void setSigma(float _sigma) { sigma = _sigma; }
    float getSigma() { return sigma; }

    void setPosition(ofVec3f pos) { cam.setGlobalPosition(pos); }
    ofVec3f getPosition() { return cam.getGlobalPosition(); }

    void lookAt(ofVec3f pos) { cam.lookAt(pos); }

    void setNearClip(float _nearClip) { 
        nearClip = _nearClip;
        cam.setNearClip(nearClip); 
    }
    float getNearClip() { return nearClip; }
    void setFarClip(float _farClip){ 
        farClip = _farClip; 
        cam.setFarClip(farClip);
    }
    float getFarClip() { return farClip; }

private:
    ofCamera cam;
    ofFbo depthMap, onepassFbo, twopassFbo;
    ofShader shadowShader, blurShader;
    ofMatrix4x4 shadowTransMatrix, biasMatrix;
    ofFbo::Settings settings;
    float time;
    bool perspective, softshadow;
    float nearClip, farClip;
    float scale;
    float resolution;
    float shadowFactor;
	float blurSize;
    float sigma;
	string shaderPath;
};