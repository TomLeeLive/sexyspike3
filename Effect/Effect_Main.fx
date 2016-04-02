
texture tex0;
sampler samp0 = sampler_state
{
	MagFilter	= LINEAR;
	MinFilter	= LINEAR;
	
	texture		= (tex0);
};

float4	DiluteAlpha( 
				float4	Pos		: POSITION
			,	float2	Tex0	: TEXCOORD0
			) : COLOR0
{
	float4	Ret	= tex2D( samp0, Tex0 );
	Ret.a		= Ret.a<0.5f? 0.f: Ret.a-0.05f;
	//Ret			= float4(1.f, 1.f, 0.9f, Ret.a<0.5f? 0.f: Ret.a-0.05f);
	return	Ret;
};

float4	Inversion(
				float4	Pos		: POSITION
			,	float2	Tex0	: TEXCOORD0
			)	: COLOR0
{
	float4	Inverse = float4(1.f, 1.f, 1.f, 1.f);
	float4	TexCol	= tex2D( samp0, Tex0 );
	return	Inverse-TexCol;
}

technique	TAfterImage
{
	pass	P0
	{
		ZWRITEENABLE		= FALSE;
		VertexShader		= NULL;
		PixelShader			= compile ps_2_0 DiluteAlpha();
	}
}

technique	TInversion
{
	pass	P0
	{
		AlphaBlendEnable	= TRUE;
		AlphaTestEnable		= FALSE;
		SRCBLEND			= INVDESTCOLOR;
		DESTBLEND			= ZERO;
		ZWRITEENABLE		= FALSE;
		VertexShader		= NULL;
		PixelShader			= compile ps_2_0 Inversion();
	}
}