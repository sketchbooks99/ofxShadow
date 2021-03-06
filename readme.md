# ofxShadow

This addon is easily to cast shadow on scene geometries. Implementation is mainly based on the following site.
https://github.com/yasuhirohoshino/oF-ShadowExamples

## Version
- OpenGL 3.2, GLSL 150 ~
- need to specify OpenGL version in main.cpp
```cpp
int main() {
    ofGLWindowSettings settings;
    settings.setGLVersion(3,2);
    settings.setSize(1024, 768);

    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
```

## Features
- hard shadow
- soft shadow

## Usage
- ofApp.h
```cpp
ofxShadow shadow;
ofEasyCam cam;
```

- ofApp.cpp
```cpp
void setup() {
    shadow.setup();
}

void draw() {
    shadow.beginDepth();
    <-- RENDER SCENE -->
    shadow.endDepth();

    shadow.begin(cam);
    <-- RENDER SCENE -->
    shadow.end(cam);
}
```

## TODO:
- Multiple shadow
- Add SSAO pass]
- Enable to cast shadow to custom shaded object
