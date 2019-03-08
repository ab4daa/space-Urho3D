#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"

#ifdef COMPILEPS
	#ifndef D3D11
	uniform float3 cSunPosition;
	uniform float3 cSunColor;
	uniform float cSunSize;
	uniform float cSunFalloff;
	#else
	cbuffer CustomPS
	{
		float3 cSunPosition;
		float3 cSunColor;
		float cSunSize;
		float cSunFalloff;
	}
	#endif
#endif

void VS(float4 iPos : POSITION,
    float4 iColor : COLOR0,    
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
	float3 posn = normalize(vPos);
	float d = clamp(dot(posn, normalize(cSunPosition)), 0.0, 1.0);
	float c = smoothstep(1.0 - cSunSize * 32.0, 1.0 - cSunSize, d);
	c += pow(d, cSunFalloff) * 0.5;
	float3 color = lerp(cSunColor, (float3)1.0, c);
	oColor = float4(color, c);
}
