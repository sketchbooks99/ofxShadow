#version 150

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