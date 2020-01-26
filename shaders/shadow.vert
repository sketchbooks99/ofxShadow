#version 150

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec4 globalColor;

in vec4 position;
in vec4 color;
in vec3 normal;
in vec2 texcoord;

out vec4 colorVarying;
out vec2 texCoordVarying;
out vec3 normalVarying;
out vec4 positionVarying;

uniform mat4 viewMatrix;
uniform mat4 lightSpaceMatrix;
uniform vec3 lightPosition;
uniform bool isPerspective;

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