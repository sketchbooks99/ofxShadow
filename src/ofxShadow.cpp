#include "ofxShadow.h"

//-------------------------------------------------------------------
ofxShadow::ofxShadow() {
	ofDirectory dir;
	string currentDir = dir.getOriginalDirectory();

	cout << currentDir << endl;
    perspective = false;
    resolution  = ofGetWidth();
    scale       = 1.0;
    nearClip    = 100;
    farClip     = 6000.0;
    sigma       = 2.0;
    biasMatrix  = ofMatrix4x4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
	blurSize = 5.0;
    shadowFactor = 75.0;
}

//-------------------------------------------------------------------
void ofxShadow::setup() {
    setupCamera();
    setupFBOs();
    setupShaders();
}

//-------------------------------------------------------------------
void ofxShadow::setupCamera() {
    cam.setupPerspective();
    cam.setPosition(1000, 1000, 1000);
    cam.lookAt(ofVec3f(0.0, 0.0, 0.0));
    cam.setNearClip(nearClip);
    cam.setFarClip(farClip);
    cam.setFov(90);
    cam.setForceAspectRatio(1.0);
}

//-------------------------------------------------------------------
void ofxShadow::setupFBOs() {
    settings.width                  = resolution * 2.0;
    settings.height                 = resolution * 2.0;
    settings.textureTarget          = GL_TEXTURE_2D;
    settings.internalformat         = GL_R8;
    settings.useDepth               = true;
    settings.depthStencilAsTexture  = true;
    settings.useStencil             = true;
    settings.minFilter              = GL_LINEAR;
    settings.maxFilter              = GL_LINEAR;
    settings.wrapModeHorizontal     = GL_CLAMP_TO_EDGE;
    settings.wrapModeVertical		= GL_CLAMP_TO_EDGE;
    // depth map
    depthMap.allocate(settings);

    // blur pass
    settings.internalformat = GL_R32F;
    onepassFbo.allocate(settings);
    twopassFbo.allocate(settings);
}

//-------------------------------------------------------------------
void ofxShadow::setupShaders() {
    // Shadow shader
    string vertShaderSrc = STRINGIFY(
        uniform mat4 modelViewMatrix;
        uniform mat4 modelViewProjectionMatrix;
        uniform mat4 viewMatrix;
        uniform mat4 lightSpaceMatrix;
        uniform vec3 lightPosition;
        uniform bool isPerspective;

        in vec4 position;
        in vec3 normal;

        out vec3 normalVarying;
        out vec4 positionVarying;
        out vec3 vNormalVarying;
        out vec4 vPositionVarying;
        out vec4 vShadowCoord;
        out vec3 vLightPosition;

        void main() {
            normalVarying = normal;
            positionVarying = position;

            mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));
            vNormalVarying = normalize(vec3(normalMatrix * normal));
            vPositionVarying = modelViewMatrix * position;

            vShadowCoord = lightSpaceMatrix * vPositionVarying;

            if(isPerspective) {
                vLightPosition = vec3(viewMatrix * vec4(lightPosition, 1.0));
            } else {
                vLightPosition = vec3(viewMatrix * vec4(lightPosition, 0.0));
            }

            gl_Position = modelViewProjectionMatrix * position;
        }
    );

    string fragShaderSrc = STRINGIFY(
        uniform sampler2D shadowMap;
        uniform bool isPerspective;
        uniform bool isSoft;
        uniform float shadowFactor;

        in vec3 normalVarying;
        in vec4 positionVarying;
        in vec3 vNormalVarying;
        in vec4 vPositionVarying;
        in vec4 vShadowCoord;
        in vec3 vLightPosition;

        out vec4 fragColor;

        float SoftShadowCalculation(vec4 shadowCoord) {
            vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
            float currentDepth = projCoords.z;
            
            float shadow = 0.0;
            float depth = texture(shadowMap, projCoords.xy).r;
            shadow = exp(shadowFactor * depth) * exp(-shadowFactor * currentDepth);
            
            if(projCoords.x <= 0.0 || projCoords.y <= 0.0 ||
            projCoords.x >= 1.0 || projCoords.y >= 1.0 ||
            projCoords.z >= 1.0 || projCoords.z <= 0.0){
                shadow = 1.0;
            }
            
            return clamp(shadow, 0.0, 1.0);
        }

        float ShadowCalculation(vec4 shadowCoord) {
            vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
            float currentDepth = projCoords.z;
            float shadow = 0.0;

            if(currentDepth - 0.0001 < texture(shadowMap, projCoords.xy).r) {
                shadow = 1.0;
            }

            if(projCoords.x <= 0.0 || projCoords.y <= 0.0 ||
            projCoords.x >= 1.0 || projCoords.y >= 1.0 ||
            projCoords.z >= 1.0 || projCoords.z <= 0.0) {
                shadow = 1.0;
            }

            return shadow;
        }

        void main() {
            float lambert = 0.0;
            if(isPerspective) {
                lambert = max(dot(normalize(vLightPosition - vPositionVarying.xyz), vNormalVarying), 0.0);
            } else {
                lambert = max(dot(normalize(vLightPosition), vNormalVarying), 0.0);
            }
            float shadowVal = isSoft ? SoftShadowCalculation(vShadowCoord) : ShadowCalculation(vShadowCoord);
            fragColor = vec4(vec3(lambert * shadowVal), 1.0);
        }
    );

    ostringstream oss;
    oss << "#version 150" << endl;
    oss << vertShaderSrc << endl;
    shadowShader.setupShaderFromSource(GL_VERTEX_SHADER, oss.str());

    oss.str("");
    oss << "#version 150" << endl;
    oss << fragShaderSrc << endl;
    shadowShader.setupShaderFromSource(GL_FRAGMENT_SHADER, oss.str());

    shadowShader.bindDefaults();
    shadowShader.linkProgram();

    // Blur shader
    vertShaderSrc = STRINGIFY(
        uniform mat4 modelViewProjectionMatrix;

        in vec4 position;
        in vec2 texcoord;

        out vec2 vTexCoord;

        void main(){
            gl_Position = modelViewProjectionMatrix * position;
            vTexCoord = texcoord;
        }
    );

    fragShaderSrc = STRINGIFY(
        uniform sampler2D tex;
        uniform float sigma;
        uniform vec2 direction;
        uniform float blurSize;
        uniform float resolution;

        const float PI = 3.14159265;

        in vec2 vTexCoord;

        out vec4 fragColor;

        void main() {
            const int N = 16;
            float delta = (blurSize / float(N)) / resolution;

            vec3 incrementalGaussian;
            incrementalGaussian.x = 1.0 / (sqrt(2.0 * PI) * sigma);
            incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
            incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;
            
            vec4 color = vec4(0.0);
            float coefficeientSum = 0.0;

            color += texture(tex, vTexCoord) * incrementalGaussian.x;
            coefficeientSum += incrementalGaussian.x;
            incrementalGaussian.xy *= incrementalGaussian.yz;

            for(int i=0; i<N; i++) {
                vec2 d = direction * float(i) * delta;
                color += texture(tex, vTexCoord + d) * incrementalGaussian.x;
                color += texture(tex, vTexCoord - d) * incrementalGaussian.x;
                coefficeientSum += 2.0 * incrementalGaussian.x;
                incrementalGaussian.xy *= incrementalGaussian.yz;
            }
            color /= coefficeientSum;
            fragColor = color;
        }
    );
    
    oss.str("");
    oss << "#version 150" << endl;
    oss << vertShaderSrc << endl;
    blurShader.setupShaderFromSource(GL_VERTEX_SHADER, oss.str());

    oss.str("");
    oss << "#version 150" << endl;
    oss << fragShaderSrc << endl;
    blurShader.setupShaderFromSource(GL_FRAGMENT_SHADER, oss.str());

    blurShader.bindDefaults();
    blurShader.linkProgram();
}

//-------------------------------------------------------------------
void ofxShadow::beginDepth() {
    depthMap.begin();
    ofClear(255, 0, 0, 255);
    cam.begin();
}

//-------------------------------------------------------------------
void ofxShadow::endDepth() {
    cam.end();
    depthMap.end();

    if(isSoftShadow()) blur();
}

//-------------------------------------------------------------------
void ofxShadow::begin(ofCamera& mainCam) {
    ofMatrix4x4 view, projection;
    view = cam.getModelViewMatrix();
    if(isPerspective()) {
        projection = cam.getProjectionMatrix();
        cout << "Perspective!" << endl;
    } else {
        projection.makeOrthoMatrix(-depthMap.getWidth() * 0.5, depthMap.getWidth() * 0.5, -depthMap.getHeight() * 0.5, depthMap.getHeight() * 0.5, cam.getNearClip(), cam.getFarClip());
        cout << "Orthogonal!" << endl;
    }
    ofMatrix4x4 inverseCameraMatrix = ofMatrix4x4::getInverseOf( mainCam.getModelViewMatrix() );
    ofMatrix4x4 shadowTransMatrix = inverseCameraMatrix * view * projection * biasMatrix;
    
    mainCam.begin();
    shadowShader.begin();
    shadowShader.setUniformMatrix4f("lightSpaceMatrix", shadowTransMatrix);
    if(isSoftShadow()) {
        shadowShader.setUniformTexture("shadowMap", twopassFbo.getTexture(), 1);
        shadowShader.setUniform1f("shadowFactor", shadowFactor);
    } else {
        shadowShader.setUniformTexture("shadowMap", depthMap.getDepthTexture(), 1);
    }
    shadowShader.setUniform3f("lightPosition", cam.getGlobalPosition());
    shadowShader.setUniformMatrix4f("viewMatrix", ofGetCurrentViewMatrix());
    shadowShader.setUniform1i("isPerspective", isPerspective());
    shadowShader.setUniform1i("isSoft", isSoftShadow());
}

//-------------------------------------------------------------------
void ofxShadow::end(ofCamera& mainCam) {
	mainCam.end();
    shadowShader.end();
}

//-------------------------------------------------------------------
void ofxShadow::blur() {
    // First pass
    onepassFbo.begin();
    ofClear(0, 255);
    blurShader.begin();
    blurShader.setUniform1f("sigma", sigma);
    blurShader.setUniform1f("blurSize", blurSize);
    blurShader.setUniform1f("resolution", onepassFbo.getWidth());
    blurShader.setUniform2f("direction", 1.0, 0.0); // Horizontal 
    blurShader.setUniformTexture("tex", depthMap.getDepthTexture(), 1);
    depthMap.getDepthTexture().draw(0, 0);
    blurShader.end();
    onepassFbo.end();

    // Second pass
    twopassFbo.begin();
    ofClear(0, 255);
    blurShader.begin();
    blurShader.setUniform1f("sigma", sigma);
    blurShader.setUniform1f("blurSize", blurSize);
    blurShader.setUniform1f("resolution", twopassFbo.getWidth());
    blurShader.setUniform2f("direction", 0.0, 1.0); // Vertical 
    blurShader.setUniformTexture("tex", onepassFbo.getTexture(), 1);
    onepassFbo.draw(0, 0);
    blurShader.end();
    twopassFbo.end();
}

//-------------------------------------------------------------------
void ofxShadow::debug(int x, int y) {
    float w = depthMap.getWidth() * 0.1;
    float h = depthMap.getHeight() * 0.1;
    depthMap.getDepthTexture().draw(x, y, w, h);
    if(isSoftShadow()) {
        onepassFbo.draw(x + w, y, w, h);
        twopassFbo.draw(x + w * 2, y, w, h);
    }
}

//-------------------------------------------------------------------
void ofxShadow::setPerspective(bool _perspective) {
	perspective = _perspective;
    if(perspective) {
        cam.disableOrtho();
        cam.setFov(90);
    } else {
        cam.enableOrtho();
        cam.setScale(scale);
    }
}

//-------------------------------------------------------------------
void ofxShadow::setResolution(float _resolution) {
    settings.width = _resolution;
    settings.height = _resolution;
    settings.internalformat = GL_R8;
    depthMap.allocate(settings);

    settings.internalformat = GL_R32F;
    onepassFbo.allocate(settings);
    twopassFbo.allocate(settings);
}

//-------------------------------------------------------------------
void ofxShadow::setScale(float _scale) {
    if(!isPerspective()) cam.setScale(scale);
}