//Grobal Variable
float4x4 matWVP;

float4 color;

// VertexShader
float4 VS(float4 pos : POSITION) : POSITION {
	return mul(pos, matWVP);
}

// PixelShader
float4 PS() : COLOR {
	return color;
}

technique render {
	pass P0 {
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}