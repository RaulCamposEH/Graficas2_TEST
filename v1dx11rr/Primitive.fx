cbuffer cbChangerEveryFrame : register(b0)
{
    matrix worldMatrix;
};

cbuffer cbNeverChanges : register(b1)
{
    matrix viewMatrix;
};

cbuffer cbChangeOnResize : register(b2)
{
    matrix projMatrix;
};


struct VS_Input
{
    float4 pos : POSITION;
    float2 tex0 : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 normal : TEXCOORD1;
};

PS_Input VS_Main(VS_Input vertex)
{
    float4 worldPosition;

    PS_Input vsOut = (PS_Input) 0;

    vsOut.pos = mul(vertex.pos, worldMatrix);
    vsOut.pos = mul(vsOut.pos, viewMatrix);
    vsOut.pos = mul(vsOut.pos, projMatrix);
 
    vsOut.tex0 = 0;
    vsOut.normal = 0;

    return vsOut;
}


cbuffer cbPrimitiveColor : register(b0)
{
    float4 primitiveColor;
}
    
float4 PS_Main(PS_Input pix) : SV_TARGET
{
    float4 color;
    
    color = primitiveColor;

    return float4(color.rgb, color.a);
}