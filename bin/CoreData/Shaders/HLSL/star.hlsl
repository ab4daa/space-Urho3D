#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"

#ifdef COMPILEPS
	#ifndef D3D11
	uniform float3 cStarPosition;
	uniform float3 cStarColor;
	uniform float cStarSize;
	uniform float cStarFalloff;
	#else
	cbuffer CustomPS
	{
		float3 cStarPosition;
		float3 cStarColor;
		float cStarSize;
		float cStarFalloff;
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
	float d = 1.0 - clamp(dot(posn, normalize(cStarPosition)), 0.0, 1.0);
	float i = exp(-(d - cStarSize) * cStarFalloff);
    float o = clamp(i, 0.0, 1.0);
	oColor = float4(cStarColor + i, o);
}
