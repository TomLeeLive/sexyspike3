
#pragma		once

#ifndef		__MACRO_H_
#define		__MACRO_H_

//	볼(공) 매크로 타입
enum	eBallMacroType
{
	BALLANDNET
};

//	캐릭터 상태(경기)
enum	PLAY_STATE
{
		OFFENSE
	,	DEFENSE
	,	SERVE
};

//	배구동작
enum	eBehavior
{
		BLOCKING
	,	RECEIVE
	,	SERVE_OVER
	,	SERVE_UNDER
	,	SLIDING
	,	SPIKE
	,	THROWING
	,	TOSS
	,	WAITING
	,	WALKBACKWARD
	,	WALKFORWARD
	,	WALKLEFT
	,	WALKRIGHT
};

//	사용하는 코트, 앞 뒤 포지션
enum	eSide
{ 
		LEFT	= 0xf
	,	RIGHT	= 0xf0
	,	FRONT	= 0xf00
	,	BACK	= 0xf000
};

enum	eAttackingSide
{
	//	T=TOP, M=MIDDLE, B=BOTTOM, L=LEFT, C=CENTER, R=RIGHT
		TL,	TC,	TR
	,	ML,	MC,	MR
	,	BL,	BC,	BR
};

//	드로우 타입
enum	SKINNINGMETHOD 
{
		NONINDEXED
	,	INDEXED
	,	VS
	,	HLSL
	,	SW

};

//	모델타입
//	모델타입
enum	eModels
{
		MANAMI
	,	ESK
	,	EO
	,	REIKO
	,	MARIA
	,	MAYA
	,	BAEL
	,	FEI
};

//	데이터 해석타입
enum	eDefinition
{
		DUMMY
	,	U_CHARACTER
	,	U_BALL
};

enum	eFlowState
{
		READYPLAY
	,	PLAY
	,	CLEANPLAY
};

enum	eRenderMode
{
		NORMAL
	,	AFTERIMAGE
	,	INVERSION
};

enum	eBallType
{
		COMMON
	,	REDSHIFT
	,	BLACKHOLE
	,	PHANTOM
	,	ILLUSION
};

enum	eTeam
{
		RED
	,	BLUE
};

enum	eDirection
{
	U, B, L, R
};

#define		CHARACTERSTRIDE					0.3f
#define		COMPRESS_RATIO					0.f
#define		EPSILON							0.0001f
#define		NUMMODELS						8
#define		NUMPLAYERS						4
#define		SKINNING_TYPE					HLSL

//	Character Motion
#define	RECEIVEPOINTHIGH	20.f
#define	RECEIVEPOINTLOW		10.f
#define	SLIDINGRADIUSNEAR	10.f
#define	SLIDINGRADIUSFAR	30.f
#define	BLOCKINGTIME		0.6f
#define	RECEIVETIME			0.3f
#define SERVEOVERTIME		0.77f
#define	SERVEUNDERTIME		0.66f
#define	SLIDINGTIME			0.25f
#define	SPIKETIME			0.47f
#define	TOSSTIME			0.5f
#define	THROWINGTIME		0.5f

#define		CHECK_FAILED( hr )				{ if( FAILED( (hr) )) {return (hr); } }
#define		CHECK_FAILED_GOTO( hr, to )		{ if( FAILED( (hr) )) { goto to; } }
#define		CHECK_FAILED_RELEASE( hr, p )	{ if( FAILED(hr) ){ SAFE_RELEASE(p); return hr; } }
#define		FRAMEMOVE( p, f )				{ if( (p) ) { CHECK_FAILED( (p)->FrameMove((f)) ); } }
#define		RENDER( p )						{ if( (p) ) { CHECK_FAILED( (p)->Render() ); } }
#define		RENDERSHADOW( p )				{ if( (p) ) { CHECK_FAILED( (p)->RenderShadow() ); } }
#define		INITDEVICEOBJECTS( p, d )		{ if( (p) ) { CHECK_FAILED( (p)->InitDeviceObjects((d)) ); } }
#define		RESTOREDEVICEOBJECTS( p )		{ if( (p) ) { CHECK_FAILED( (p)->RestoreDeviceObjects() ); } }
#define		INVALIDATEDEVICEOBJECTS( p )	{ if( (p) ) { CHECK_FAILED( (p)->InvalidateDeviceObjects() ); } }
#define		DELETEDEVICEOBJECTS( p )		{ if( (p) ) { CHECK_FAILED( (p)->DeleteDeviceObjects() ); } }

const	D3DXMATRIX	UNIT_MATRIX	=	D3DXMATRIX(
								1.f,	0.f,	0.f,	0.f
							,	0.f,	1.f,	0.f,	0.f
							,	0.f,	0.f,	1.f,	0.f
							,	0.f,	0.f,	0.f,	1.f
							);

const char g_arrBehaviorName [][16]	= 
{
		"BLOCKING"
	,	"RECEIVE"
	,	"SERVE_OVER"
	,	"SERVE_UNDER"
	,	"SLIDING"
	,	"SPIKE"
	,	"THROWING"
	,	"TOSS"
	,	"WAITING"
	,	"WALKBACKWARD"
	,	"WALKFORWARD"
	,	"WALKLEFT"
	,	"WALKRIGHT"
};

const	char	g_arrModelPath[][62] =
{
		"./XFILE/CHARACTER/MANAMI/"
	,	"./XFILE/CHARACTER/ESK/"
	,	"./XFILE/CHARACTER/EO/"
	,	"./XFILE/CHARACTER/REIKO/"
	,	"./XFILE/CHARACTER/MARIA/"
	,	"./XFILE/CHARACTER/MAYA/"
	,	"./XFILE/CHARACTER/BAEL/"
	,	"./XFILE/CHARACTER/FEI/"
};

//	경계구 구조체
struct	BoundingSphere
{
	D3DXVECTOR3		vCenter;			//	경계구 중심
	D3DXVECTOR3		vDiff;				//	경계구 중심과 이동행렬과의 차이
	float			fRadius;			//	경께구 반지름
	LPD3DXMATRIX	pTM;				//	이동행렬 또는 백터의 포인트

	void	UpdateBoundingCenter()
	{
		D3DXVec3TransformCoord( &vCenter, &-vDiff, pTM );
	};
};

//	경계실린더	구조체
struct	BoundingCylinder	: public BoundingSphere
{
	float			fHeight;			//	실린더의 높이
};

//	경계 면 구조체
struct	BoundingPlane
{
	D3DXVECTOR3	n;			//	법선백터
	float		d;			//	원점과의 거리
	D3DXVECTOR3	LB;			//	유한평면일 경우 좌하
	D3DXVECTOR3	RT;			//	유한평면일 경우 우상
};

//	맵 
struct	StrCmpI
{
	bool operator()( LPCSTR str1, LPCSTR str2 ) const
	{
		return (0 > _strcmpi( str1, str2 ));
	}
};

//	팀

struct	TeamInfo
{	
	void*		PLAYER1;
	void*		PLAYER2;
	eTeam		TEAMID;
	void*		LASTSERVER;
	PLAY_STATE	PLAYSTATE;

	TeamInfo() : PLAYER1(NULL), PLAYER2(NULL), TEAMID(RED), LASTSERVER(NULL), PLAYSTATE(SERVE) {}
	TeamInfo( void* P1, void* P2, eTeam TID, void* SERVER, PLAY_STATE PSTATE)
		: PLAYER1(P1), PLAYER2(P2), TEAMID(TID), LASTSERVER(SERVER), PLAYSTATE(PSTATE){};
};

struct		SimpleVertex
{
	D3DXVECTOR3		p;
	float			tu, tv;
	enum	{ SimpleVertexFVF = (D3DFVF_XYZ | D3DFVF_TEX1), };
};

struct		SimpleVertex2
{
	D3DXVECTOR4		p;
	float			tu, tv;
	enum	{ SimpleVertex2FVF = (D3DFVF_XYZRHW | D3DFVF_TEX1) };
};

struct		SimpleBlendVertex1
{
	D3DXVECTOR3	P;
	float		B1;
	D3DXVECTOR3	N;
	float		tu, tv;
	enum	{ SimpleBlendVertex1FVF = (D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX1) };
};

struct		BallPacket
{
	char		Name[20];
	eBallType	BALLTYPE;
	float		ROTATION;
	D3DXVECTOR3	AXIS;
	D3DXVECTOR3	POSITION;
	D3DXVECTOR3	VELOCITY;
	int			CNTTOUCHED;
	bool		NOWSERVER;

	BallPacket() : BALLTYPE(COMMON), ROTATION(0.f), AXIS(0.f, 0.f, 0.f), POSITION(0.f, 0.f, 0.f), VELOCITY(0.f,0.f,0.f), CNTTOUCHED(0), NOWSERVER(false) { ZeroMemory(Name, sizeof(Name)); }
	BallPacket( char* pName, eBallType B, float R, D3DXVECTOR3 A, D3DXVECTOR3 P, D3DXVECTOR3 V, int CNT, bool SERVE )
		: BALLTYPE(B), ROTATION(R), AXIS(A), POSITION(P), VELOCITY(V), CNTTOUCHED(CNT), NOWSERVER(SERVE) { if(pName) {strcpy_s(Name, sizeof(Name), pName);} else { Name[0] = '\0';}  }
};

struct	PlayerPacket
{
	char		Name[20];
	eBehavior	BEHAVIOR;
	float		ROTATION;
	D3DXVECTOR3	POSITION;

	PlayerPacket() : BEHAVIOR(WAITING), ROTATION(0.f), POSITION(0.f, 0.f, 0.f) { ZeroMemory(Name, sizeof(Name)); }
	PlayerPacket(char* pName, eBehavior B, float R, float X, float Y, float Z)
		: BEHAVIOR(B), ROTATION(R), POSITION(X, Y, Z) { if(pName) {strcpy_s(Name, sizeof(Name), pName);} else { Name[0] = '\0';} }
};

typedef	std::map<LPCSTR, const ID3DXEffect* const, StrCmpI>	EffectMap;

#endif