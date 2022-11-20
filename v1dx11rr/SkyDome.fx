Texture2D blend1 : register(t0);
Texture2D blend2 : register(t1);

SamplerState colorSampler : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
	float4 valores;
};

cbuffer cbChangerEveryFrame : register(b1)
{
    float MoveText;
};

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
    float4 blendtext1 = blend1.Sample(colorSampler, pix.tex0);
    float4 blendtext2 = blend2.Sample(colorSampler, pix.tex0);
	
    float4 lerpcolor = lerp(blendtext1, blendtext2, MoveText);
    
    return lerpcolor;
}