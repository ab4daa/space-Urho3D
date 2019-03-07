#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"


varying vec3 vPos;
#ifdef COMPILEPS
uniform vec3 cStarPosition;
uniform vec3 cStarColor;
uniform float cStarSize;
uniform float cStarFalloff;
#endif

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
	vPos = worldPos;
    gl_Position = GetClipPos(worldPos);
}

void PS()
{
	vec3 posn = normalize(vPos);
    float d = 1.0 - clamp(dot(posn, normalize(cStarPosition)), 0.0, 1.0);
    float i = exp(-(d - cStarSize) * cStarFalloff);
    float o = clamp(i, 0.0, 1.0);
    gl_FragColor = vec4(cStarColor + i, o);
}
