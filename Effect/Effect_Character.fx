
float4x3	WorldArray[28];
float4x4	World;
float4x4	ViewProj;
float4x4	ShadowViewProj;
float4		Light;
float4		ColorDiffuse;
float4		ColorAmbient;
int			CurrentNumWeights;

struct	VS_IN
{
	float4	Pos		: POSITION;
	float4	BWeights: BLENDWEIGHT;
	float4	BIndices: BLENDINDICES;
	float3	Normal	: NORMAL;
	float2	Tex0	: TEXCOORD0;
};

struct	VS_IN_NOSKINNING
{
	float4	Pos		: POSITION;
	float3	Normal	: NORMAL;
	float2	Tex0	: TEXCOORD0;
};

struct	VS_OUT
{
	float4	Pos		: POSITION;
	float4	Diffuse	: COLOR0;
	float2	Tex0	: TEXCOORD0;
};

struct	SHADOW_OUT
{
	float4	Pos		: POSITION;
	float4	Diffuse	: COLOR0;
};

VS_OUT	HLSLSkinning( VS_IN In, uniform int NumWeights )
{
	VS_OUT	O		= (VS_OUT)0;
	float3	Pos		= 0.f;
	float3	Normal	= 0.f;
	float	SumWeights	= 0.f;
	
	//	애플리케이션에서 UBYTE4->D3DCOLOR 해준것을 다시 UBYTE4로 바꿔준다.
	int		BIndices[4]	= (int[4])D3DCOLORtoUBYTE4(In.BIndices);
	float	BWeights[4]	= (float[4])In.BWeights;
	
	for( int I=0; I<NumWeights; ++I )
	{
		SumWeights	+=	BWeights[I];
		Pos			+=	BWeights[I]*mul(In.Pos,		WorldArray[BIndices[I]]);
		Normal		+=	BWeights[I]*mul(In.Normal,	WorldArray[BIndices[I]]);
	}
	SumWeights	= 1-SumWeights;
	Pos		+=	SumWeights*mul(In.Pos,		WorldArray[BIndices[NumWeights]]);
	Normal	+=	SumWeights*mul(In.Normal,	WorldArray[BIndices[NumWeights]]);
	O.Pos	=	mul( float4(Pos,1.f), ViewProj);
	O.Diffuse	= ColorAmbient+(ColorDiffuse*max(0.1f, dot(Normal,Light)));
	O.Tex0	=	In.Tex0;
	
	return	O;
};

VS_OUT	HLSLNoSkinning( VS_IN_NOSKINNING In )
{
	VS_OUT	O	= (VS_OUT)0;
	float4	Pos	= mul( In.Pos, World);
	O.Pos		= mul( Pos, ViewProj);
	
	float3	N	= mul( In.Normal, World);
	O.Diffuse	= ColorAmbient+(ColorDiffuse*max(0.1f, dot(N,Light)));
	O.Tex0	=	In.Tex0;
	
	return	O;
};

float4	DrawShadow( VS_IN In ) : POSITION
{
	float3		Pos		= 0.f;

	//	애플리케이션에서 UBYTE4->D3DCOLOR 해준것을 다시 UBYTE4로 바꿔준다.
	int		BIndices[4]	= (int[4])D3DCOLORtoUBYTE4(In.BIndices);
	float	BWeights[4]	= (float[4])In.BWeights;
	
	Pos			= mul( In.Pos, WorldArray[BIndices[0]] );
	float4 RET	= mul( float4(Pos,1.f), ShadowViewProj);
	
	return RET;
};

float4	DrawShadowNoSkinning( VS_IN_NOSKINNING In ) : POSITION
{
	float4	Pos	= mul( In.Pos, World);
	Pos			= mul( Pos, ViewProj);

	return	Pos;
};

float4	DrawShadowPs( SHADOW_OUT In ) : COLOR
{
	float4	RET	= float4(0.2f, 0.2f, 0.2f, 0.8f );
	return RET;
};

technique	IndexedSkinning
{
	pass	P0
	{
		VertexShader = compile vs_2_0 HLSLSkinning( CurrentNumWeights );
	}
	
	pass	P1
	{
		StencilEnable	= TRUE;
		StencilFunc		= Equal;
		StencilRef		= 0x0;
		StencilMask		= 0xFFFFFFFF;
		StencilFail		= keep;
		StencilZFail	= keep;
		StencilPass		= Incr;

		AlphaBlendEnable= TRUE;
		SrcBlend		= SrcAlpha;
		DestBlend		= InvSrcAlpha;
		ZWriteEnable	= False;
		VertexShader	= compile vs_2_0 DrawShadow();
		PixelShader		= compile ps_2_0 DrawShadowPs();
	}
	
	pass	P2
	{
		VertexShader = compile vs_2_0 HLSLNoSkinning();
	}
	
	pass	P3
	{
		StencilEnable	= TRUE;
		StencilFunc		= Equal;
		StencilRef		= 0x0;
		StencilMask		= 0xFFFFFFFF;
		StencilFail		= keep;
		StencilZFail	= keep;
		StencilPass		= Incr;

		AlphaBlendEnable= TRUE;
		SrcBlend		= SrcAlpha;
		DestBlend		= InvSrcAlpha;
		ZWriteEnable	= False;
		VertexShader	= compile vs_2_0 DrawShadowNoSkinning();
		PixelShader		= compile ps_2_0 DrawShadowPs();
	}
}