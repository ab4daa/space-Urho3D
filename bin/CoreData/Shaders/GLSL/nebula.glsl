#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "classicnoise4D.glsl"

varying vec3 vPos;
#ifdef COMPILEPS
uniform vec3 cNebularColor;
uniform vec3 cNebularOffset;
uniform float cNebularScale;
uniform float cNebularIntensity;
uniform float cNebularFalloff;

float noise(vec3 p) {
    return 0.5 * cnoise(vec4(p, 0)) + 0.5;
}

float nebula(vec3 p) {
    const int steps = 6;
    float scale = pow(2.0, float(steps));
    vec3 displace;
    for (int i = 0; i < steps; i++) {
        displace = vec3(
            noise(p.xyz * scale + displace),
            noise(p.yzx * scale + displace),
            noise(p.zxy * scale + displace)
        );
        scale *= 0.5;
    }
    return noise(p * scale + displace);
}
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
	vec3 posn = normalize(vPos) * cNebularScale;
    float c = min(1.0, nebula(posn + cNebularOffset) * cNebularIntensity);
    c = pow(c, cNebularFalloff);
    gl_FragColor = vec4(cNebularColor, c);
}
