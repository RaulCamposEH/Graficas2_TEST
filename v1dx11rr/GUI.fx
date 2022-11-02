Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

cbuffer cbWorldMatrix : register(b0)
{
    matrix worldMatrix;
}

struct VS_INPUT
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

PS_INPUT VS_Main(VS_INPUT vertex)
{
    PS_INPUT psOut;
    
    psOut.pos = mul(vertex.pos, worldMatrix);
    psOut.normal = mul(vertex.normal, worldMatrix);
    psOut.uv = vertex.uv;
    
    return psOut;
}

float4 PS_Main(PS_INPUT pix) : SV_TARGET
{
    float4 textura = colorMap.Sample(colorSampler, pix.uv);
    clip(textura.a - 0.9);
    return textura;
}