#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"


varying vec3 vPos;
#ifdef COMPILEPS
uniform vec3 cSunPosition;
uniform vec3 cSunColor;
uniform float cSunSize;
uniform float cSunFalloff;
#endif

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
	
    vPos = worldPos;
}

void PS()
{
	vec3 posn = normalize(vPos);
    float d = clamp(dot(posn, normalize(cSunPosition)), 0.0, 1.0);
    float c = smoothstep(1.0 - cSunSize * 32.0, 1.0 - cSunSize, d);
    c += pow(d, cSunFalloff) * 0.5;
    vec3 color = mix(cSunColor, vec3(1,1,1), c);
    gl_FragColor = vec4(color, c);
}
