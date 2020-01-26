#version 150

uniform sampler2D shadowMap;
uniform bool isPerspective;
uniform bool isSoft;
uniform float shadowFactor;

in vec4 colorVarying;
in vec2 texCoordVarying;
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