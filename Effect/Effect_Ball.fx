
sampler		samp0;
float4x4	SHADOW;
float		BLACKHOLEWeight;

struct SHADOW_OUT
{
	float4	Pos		: POSITION;
	float4	Diffuse	: COLOR0;
};

float4	PSRedShift( float4 Diffuse	: COLOR0
				,	float4 Specular	: COLOR1
				,	float4 Tex0		: TEXCOORD0) : COLOR
{
	float4	RedShift= float4( 1.2f, 0.8f, 0.8f, 1.f );
	float4  texcolor= tex3D( samp0, Tex0 );
	float4	Color	= saturate( ((Diffuse+Specular)*texcolor) * RedShift );
	
	return Color;
}

SHADOW_OUT	VS_Shadow( float4 Pos : POSITION )
{
		SHADOW_OUT	O = (SHADOW_OUT)0;
		O.Pos		= mul( float4(Pos.xyz, 1.f), SHADOW );
		O.Diffuse	= float4(0.2f, 0.2f, 0.2f, 0.7f);
		
		return O;
};

float4		PS_Shadow( SHADOW_OUT In )	: COLOR0
{
	return	In.Diffuse;
}

float4		PS_BLACKHOLE(float4 Pos : POSITION, float2 Tex0 : TEXCOORD0)	: COLOR
{
	float4 OUT	= tex2D( samp0, Tex0 );
	float   sum	= OUT.r + OUT.g + OUT.b;
	OUT		= BLACKHOLEWeight*OUT;
	OUT.a	= (sum<1.f?0.f:1.f);
	return	OUT;
}

technique	TDrawNormal
{
	pass P0
	{
		VertexShader	= NULL;
		PixelShader		= NULL;
	}
	
	pass P1
	{
		AlphaBlendEnable= TRUE;
		SrcBlend		= SrcAlpha;
		DestBlend		= InvSrcAlpha;
		VertexShader	= compile vs_2_0 VS_Shadow();
		PixelShader		= compile ps_2_0 PS_Shadow();
	}
}

technique	TDrawRedShift
{
	pass P0
	{
		VertexShader	= NULL;
		PixelShader		= compile ps_2_0 PSRedShift();
	}
	
	pass P1
	{
		AlphaBlendEnable= TRUE;
		SrcBlend		= SrcAlpha;
		DestBlend		= InvSrcAlpha;
		VertexShader	= compile vs_2_0 VS_Shadow();
		PixelShader		= compile ps_2_0 PS_Shadow();
	}
}

technique	TBLACKHOLE
{
	pass P0
	{
		AlphaBlendEnable= TRUE;
		SrcBlend		= SrcAlpha;
		DestBlend		= InvSrcAlpha;		
		PixelShader		= compile ps_2_0 PS_BLACKHOLE();
	}
}