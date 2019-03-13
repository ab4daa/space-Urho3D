#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "classicnoise4D.hlsl"

#ifdef COMPILEPS
	#ifndef D3D11
	uniform float3 cNebularColor;
	uniform float3 cNebularOffset;
	uniform float cNebularScale;
	uniform float cNebularIntensity;
	uniform float cNebularFalloff;
	#else
	cbuffer CustomPS
	{
		float3 cNebularColor;
		float3 cNebularOffset;
		float cNebularScale;
		float cNebularIntensity;
		float cNebularFalloff;
	}
	#endif
	float noise_nebula(float3 p) {
		return 0.5 * cnoise(float4(p, 0)) + 0.5;
	}

	float nebula(float3 p) {
		const int steps = 6;
		float scale = pow(2.0, float(steps));
		float3 displace = (float3)0.0;
		for (int i = 0; i < steps; i++) {
			displace = float3(
				noise_nebula(p.xyz * scale + displace),
				noise_nebula(p.yzx * scale + displace),
				noise_nebula(p.zxy * scale + displace)
			);
			scale *= 0.5;
		}
		return noise_nebula(p * scale + displace);
	}
#endif

void VS(float4 iPos : POSITION,
    out float3 vPos : TEXCOORD0,    
    out float4 oPos : OUTPOSITION)
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
	vPos = worldPos;
}

void PS(
    float3 vPos : TEXCOORD0,
    out float4 oColor : OUTCOLOR0)
{	
	float3 posn = normalize(vPos) * cNebularScale;
	float c = min(1.0, nebula(posn + cNebularOffset) * cNebularIntensity);
    c = pow(c, cNebularFalloff);
	oColor = float4(cNebularColor, c);
}
