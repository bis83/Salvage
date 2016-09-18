// GrobalVariable
float4x4 matW;
float4x4 matWVP;
//float4x4 matWIT;
//float4x4 matVI;

float3 vecLightDir;

float3 vecEyePos;

float2 wave;
float height;

texture ReflectMap;
texture NormalMap;

sampler NormalSamp = sampler_state
{
    Texture = <NormalMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = Wrap;
    AddressV = Wrap;
};

sampler ReflectSamp = sampler_state
{
    Texture = <ReflectMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = Wrap;
    AddressV = Wrap;
};

float3 tangent;
float4 color;

// Define
struct VS_OUT {
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 light : TEXCOORD1;
	float3 eye : TEXCOORD2;
};

// VertexShader
VS_OUT VS(float4 pos : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD0) {
	VS_OUT Out;
	
	Out.pos = mul(pos, matWVP);
	Out.tex = tex+wave;
	
	normal.y += height;
	float3 wNormal = mul(normal, matW);
	float3 wTangent = mul(tangent, matW);
	
	float3 biNormal = cross(wNormal, wTangent);
	
	float3x3 matTan = {wTangent, biNormal, wNormal};
	matTan = transpose(matTan);
	
	Out.light = mul(-vecLightDir, matTan);
	Out.eye = mul(vecEyePos-pos.xyz, matTan);
	
	return Out;
}

// PixelShader
float4 PS(VS_OUT In) : COLOR {

	float3 Normal = 2.0 * tex2D(NormalSamp, In.tex).xyz - 1.0;
	Normal.y = (1-Normal.y)*height;
	Normal = Normal*2-1;
	float3 Reflect = reflect(normalize(In.eye), Normal);
	float2 ref = float2(Normal.x, Normal.y);
	
	float4 diffuse = 0.5f * color * max(0, dot(Normal, In.light));
	float4 specular = 0.5f * pow(max(0, dot(Reflect, In.light)), 2);

	return diffuse + specular + tex2D(ReflectSamp, ref)/4;
}

technique render {
	pass P0 {
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}