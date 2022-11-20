Texture2D colorMap : register(t0);
Texture2D colorMap2 : register(t1);
Texture2D colorMap3 : register(t2);

Texture2D blendMap : register(t3);
Texture2D blendMap2 : register(t4);

Texture2D normalMap : register(t5);
Texture2D normalMap2 : register(t6);
Texture2D normalMap3 : register(t7);

SamplerState colorSampler : register(s0);

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

cbuffer cbChangesTextLocation : register(b3)
{
    float3 cameraPos;
};


cbuffer cbChangesTextLocation : register(b4)
{
    float3 sunPos;
};

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float2 blendTex : TEXCOORD1;
	float3 normal : NORMAL0;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float2 blendTex : TEXCOORD1;
    float3 lightVec : TEXCOORD2;
    float3 cameraVec : TEXCOORD3;
	
    float3 normal : NORMAL0;
    float3 tangent : TANGENT;
    float3 binorm : BINORMAL;
};

PS_Input VS_Main(VS_Input vertex)
{
    //initial positions
	PS_Input vsOut = (PS_Input)0;
    
    float4 worldPos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

    //uv
	vsOut.tex0 = vertex.tex0;
	vsOut.blendTex = vertex.blendTex;
    
    //normal
    vsOut.normal = mul(vertex.normal, (float3x3) worldMatrix);
    vsOut.normal = normalize(vsOut.normal);
	
	//Tangente
    float3 tangent;
    float3 c1 = cross(vsOut.normal, float3(0.0, 0.0, 1.0));
    float3 c2 = cross(vsOut.normal, float3(0.0, 1.0, 0.0));
    if (length(c1) > length(c2))
    {
        tangent = c1;
    }
    else
    {
        tangent = c2;
    }
    vsOut.tangent = normalize(tangent);
    
    // Binormal
    float3 binormal;
    binormal = cross(vsOut.normal, vsOut.tangent);
    vsOut.binorm = normalize(binormal);

    // Calculate light vector
    vsOut.lightVec = normalize(-sunPos - worldPos);
    
    // Calculate camera vector
    vsOut.cameraVec = normalize(cameraPos - worldPos);
	
	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
	//luces
    float3 ambient = float3(0.1f, 0.1f, 1.0f);
    float3 DiffuseDirection = float3(0.5f, -1.0f, 0.0f);
    float3 diffuseColor = float3(1.0f, 1.0f, 1.0f);
    float3 specularColor = float3(0.8f, 0.8f, 0.8f);

	//blend texturas
	float4 text1 = colorMap.Sample(colorSampler, pix.tex0);
	float4 text2 = colorMap2.Sample(colorSampler, pix.tex0);
	float4 text3 = colorMap3.Sample(colorSampler, pix.tex0);	
	float4 alphaBlend1 = blendMap.Sample(colorSampler, pix.blendTex);
	float4 alphaBlend2 = blendMap2.Sample(colorSampler, pix.blendTex);
	float4 textblend = (text1 * alphaBlend1) + ((1.0 - alphaBlend1) * text2);
    textblend = (text3 * alphaBlend2) + ((1.0 - alphaBlend2) * textblend);
	
    //blend normales
    float4 text4 = normalMap.Sample(colorSampler, pix.tex0);
    float4 text5 = normalMap2.Sample(colorSampler, pix.tex0);
    float4 text6 = normalMap3.Sample(colorSampler, pix.tex0);
    float4 Normalblend = (text4 * alphaBlend1) + ((1.0 - alphaBlend1) * text5);
    Normalblend = (text6 * alphaBlend2) + ((1.0 - alphaBlend2) * Normalblend);
	
    // Normalizacion
    float3 normal = normalize(pix.normal);
    float3 lightVec = normalize(pix.lightVec);
    float3 cameraVec = normalize(pix.cameraVec);
	
    //BumpMap
    Normalblend = (Normalblend * 2.0f) - 1.0f;
	
	//calcular bump normals
    float3 bumpnormal = (Normalblend.r * pix.tangent) + (Normalblend.g * pix.binorm) + (Normalblend.b * pix.normal);
    bumpnormal = normalize(bumpnormal);
	
    //diffuse basada en el bump map
    float3 diffuseTerm = saturate(dot(bumpnormal, lightVec));
    diffuseTerm = normalize(diffuseTerm); //light intensity
    
    //especular
    float3 R = normalize(lightVec + cameraVec);
    float specularTerm = pow(saturate(dot(normal, R)), 5000); // puntito de luz
		
	// Luzes que multiplicaaran por la textura
    float3 finalLight = saturate(ambient + diffuseColor * diffuseTerm + specularColor * specularTerm);	
    
    
	
	//color final
    return float4( textblend.rgb * finalLight, 1.0f);
}