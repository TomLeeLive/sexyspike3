
#pragma		once

#ifndef		__BALL_H_
#define		__BALL_H_

struct		BallMacro
{
	float		ElapsedTime;		//	경과시간
	float		MacroTime;			//	총 매크로 진행시간
	float		TransitionPoint;	//	공속도 변화시점
	D3DXVECTOR3 TempVelocity;		//	변화시점 전까지 움직일 방향
};

class		CBall
{
	LPD3DXMESH				m_pBall;
	char					m_pName[8];
	DWORD					m_numMaterials;
	D3DMATERIAL9			*m_pMaterials;
	LPDIRECT3DTEXTURE9		*m_ppTex;
	D3DXMATRIX				m_matS;
	D3DXMATRIX				m_matR;
	D3DXMATRIX				m_matT;
	D3DXMATRIX				m_matWorld;
	D3DXVECTOR3				m_vRotationAxis;
	float					m_fRotationAngle;

	BOOL					m_bUseMacro;
	BallMacro				m_macro;

	eBallType				m_balltype;

	//	낙하지점
	D3DXMATRIX				m_matFalling;
	LPDIRECT3DTEXTURE9		m_pFallingTex;
	SimpleVertex			m_pFalling[4];
	BOOL					m_bShowFallingPoint;

	//	
	LPDIRECT3DTEXTURE9		m_pBlackholeTex;
	LPDIRECT3DVERTEXBUFFER9	m_pBallVB;
	float					m_fBallWeight;
	float					m_fBallIncr;

	//
	bool					m_bNeedUpdate;

public:
	D3DXVECTOR3				m_vInitPosition;
	D3DXVECTOR3				m_vVelocity;
	D3DXVECTOR3				m_vAccel;
	D3DXVECTOR3				m_vExtraDest;
	D3DXVECTOR3				m_vDest;
	BoundingSphere			m_BoundingBall;
	eBehavior				m_eLastTouchedByXXX;
	eSide					m_eSideBefore;
	eSide					m_eSideCurrent;

public:
	CBall();
	~CBall();

	HRESULT		FrameMove( float fElapsedTime );
	HRESULT		Render(LPDIRECT3DDEVICE9 pDevice);
	HRESULT		InitDeviceObjects(LPDIRECT3DDEVICE9	pDevice);
	HRESULT		RestoreDeviceObjects(LPDIRECT3DDEVICE9	pDevice);
	HRESULT		InvalidateDeviceObjects();
	HRESULT		DeleteDeviceObjects();

	void		ComputeFallingSpot( D3DXMATRIX* m );
	void		ComputeRotationAxis();
	void		ReflectByXXX( const D3DXVECTOR3* pN, eBehavior XXX, float fWeight = 1.f);
	void		SetMacro( eBallMacroType eUsage, const void* param );
	void		ProcessMacro(float fElapsedTime);

	void		UpdateBallStateFromNetwork(const BallPacket *packet);

	void		MoveTo( float x, float y, float z ) { m_matT._41+=x;m_matT._42+=y;m_matT._43+=z; }
	void		SetBalltype( eBallType balltype ) {m_balltype = balltype;}
	void		SetPosition( float x, float y, float z ) { m_matT._41=x;m_matT._42=y;m_matT._43=z; }
	void		SetAccel( D3DXVECTOR3 vAccel ) { m_vAccel = vAccel; }
	void		SetDest ( D3DXVECTOR3 vDest  ) { m_vDest  = vDest; }
	void		SetVelocity( D3DXVECTOR3 vVelocity ) { m_vVelocity = vVelocity; }
	void		SetExtraDest( D3DXVECTOR3 vDest) { m_vExtraDest= vDest; }
	void		SetExtraDest( float x, float y, float z ) { m_vExtraDest.x+=x; m_vExtraDest.y+=y; m_vExtraDest.z+=z;}
	void		SetNeedUpdate(bool flag) { m_bNeedUpdate = flag; }
	void		SetSideBefore( eSide SIDE ) { m_eSideBefore  = SIDE; }
	void		SetSideCurrent( eSide SIDE ) { m_eSideCurrent= SIDE; }
	void		SetShowFallingPoint( BOOL Flag) { m_bShowFallingPoint = Flag; }
	bool		GetNeedUpdate()		{ return m_bNeedUpdate; }
	float		GetRotationAngle()	{ return m_fRotationAngle; }
	eBallType	GetBalltype()		{ return m_balltype; }
	eBehavior	GetLastTouchedBy()	{ return m_eLastTouchedByXXX; }
	float		GetBoundingRadius()	{ return	m_BoundingBall.fRadius; }
	float		GetHeight();
	const		LPD3DXVECTOR3	GetAccel() { return &m_vAccel; }
	const		LPD3DXVECTOR3	GetAxis()			{ return &m_vRotationAxis; }
	const		LPD3DXVECTOR3	GetBoundingCenter() { return &m_BoundingBall.vCenter; }
	const		LPD3DXVECTOR3	GetDest()			{ return &m_vDest; }
	const		LPD3DXVECTOR3	GetExtraDest()		{ return &m_vExtraDest; }
	D3DXVECTOR3					GetRotationAxis()	{ return m_vRotationAxis; }
	D3DXVECTOR3					GetFallingSpot()	{ return D3DXVECTOR3(m_matFalling._41, m_matFalling._42, m_matFalling._43); }
	D3DXVECTOR3					GetPosition()		{ return D3DXVECTOR3(m_matT._41, m_matT._42, m_matT._43); }
	const		LPD3DXVECTOR3	GetVelocity()		{ return &m_vVelocity; }
	eSide		GetSideBefore()						{ return m_eSideBefore; }
	eSide		GetSideCurrent()					{ return m_eSideCurrent; }
	const		BoundingSphere*	GetBoundingBall()	{ return &m_BoundingBall; }
	const		char*			GetName()			{ return m_pName; }
};

#endif