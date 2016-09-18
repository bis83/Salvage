texture DecaleMap;

sampler DecaleSamp = sampler_state {
	Texture = <DecaleMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_OUT {
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
};

VS_OUT VS(float4 pos : POSITION, float2 uv : TEXCOORD0) {
	VS_OUT Out;
	Out.pos = pos;
	Out.uv = uv;
	return Out;
}

float4 PS(VS_OUT In) : COLOR {
	return tex2D(DecaleSamp, In.uv);
}

technique render {
	pass P0 {
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}