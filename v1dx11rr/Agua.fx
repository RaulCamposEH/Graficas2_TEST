Texture2D colorMap : register(t0);
Texture2D colorMap2 : register(t1);
Texture2D colorMap3 : register(t2);

Texture2D blendMap : register(t3);
Texture2D blendMap2 : register(t4);

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
    float MoveText;
};

cbuffer cbChangesTextLocation : register(b4)
{
    float3 cameraPos;
};

cbuffer cbChangesTextLocation : register(b5)
{
    float3 sunPos;
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
    float3 lightVec : TEXCOORD1;
    float3 cameraVec : TEXCOORD2;
	
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
    vsOut.lightVec = normalize(sunPos - worldPos);
    
    // Calculate camera vector
    vsOut.cameraVec = normalize(cameraPos - worldPos);
	
	// normalizacion
	//vsOut.normal = normalize(mul(vertex.normal, worldMatrix));
	//vsOut.tangent = normalize(mul(vertex.tangente, worldMatrix));
	//vsOut.binorm = normalize(mul(vertex.binormal, worldMatrix));

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{		
	//moviento de la textura
    pix.tex0.y += MoveText;
	
	//luces
	float3 ambient = float3(0.1f, 0.1f, 0.1f);
    float3 DiffuseDirection = float3(0.5f, -1.0f, 0.0f);	
    float3 diffuseColor = float3(1.0f, 1.0f, 1.0f);	
    float3 specularColor = float3(0.8f, 0.8f, 0.8f);
	
	// texturas
	float4 text1 = colorMap.Sample(colorSampler, pix.tex0);	
	float4 text2 = colorMap2.Sample(colorSampler, pix.tex0);	
	float4 text3 = colorMap3.Sample(colorSampler, pix.tex0);
    
    text3 = saturate(text3);
	
	// Normalizacion
    float3 normal = normalize(pix.normal);
    float3 lightVec = normalize(pix.lightVec);
    float3 cameraVec = normalize(pix.cameraVec);
	
	//BumpMap
    text2 = (text2 * 2.0f) - 1.0f;	
	
	//calcular bump normals
    float3 bumpnormal = (text2.r * pix.tangent) + (text2.g * pix.binorm) + (text2.b * pix.normal);
    bumpnormal = normalize(bumpnormal);	
    
    lightVec = -lightVec;
	
	//diffuse basada en el bump map
    float3 diffuseTerm = saturate(dot(bumpnormal, lightVec));
    diffuseTerm = normalize(diffuseTerm); //light intensity
	
    //especular
    float3 R = normalize(-lightVec + cameraVec);
    float specularTerm = pow(saturate(dot(normal, R)), 100); // puntito de luz
		
	// Luzes que multiplicaaran por la textura
    float3 finalLight = saturate(ambient + diffuseColor * diffuseTerm) + (specularColor * specularTerm);
    	
	//color final
    return float4(text1.rgb  * finalLight, 0.5f);
}