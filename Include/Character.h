
#pragma		once

#ifndef		__CHARACTER_ANIMATION_MODEL_H_
#define		__CHARACTER_ANIMATION_MODEL_H_

struct	CharacterAbility
{
	int	Block;
	int	Defense;
	int	Speed;
	int	Spike;
	int	Lucky;
};

class	Character
{
public:
	//09-08-25	이재준
	D3DXMATRIX			m_matS;									//	캐릭터 전체에 대한 TM(SCALE)
	D3DXMATRIX			m_matR;									//	캐릭터 전체에 대한 TM(ROTATION)
	D3DXMATRIX			m_matT;									//	캐릭터 전체에 대한 TM(TRANSLATION)
	D3DXMATRIX			m_matShadow;
	float				m_fRotationAngle;						//	캐릭터 회전각도
	CharacterAnimationController* m_pAniController;		//	애니메이션 컨트롤라

	//	캐릭터 생성 관련
	LPD3DXFRAME			m_pFrameRoot;			//	D3DXFRAME형이지만 사실은 하위 구현 클래스인 CFrame형이다.

protected:
	//	캐릭터 생성 관련
	BOOL				m_bFrameOwner;				//	프레임을 직접생성한 객체인지
	D3DXMATRIX			m_matWorld;				//	S*R*T
	float				m_fElapsedTime;			//

	bool				m_bDrawShadow;

	LPTSTR				m_pCharacterFilePath;
	LPTSTR*				m_ppCharacterFiles;		//	애니메이션 파일(XFILE )
	int					m_cntCharacterFiles;		//	X파일 갯수

	CharacterAbility	m_ability;					//	캐릭터 능력치
	//	점프
	BOOL				m_bNowJumping;				//
	float				m_fJumpTime;				//
	float				m_fJumpVelocity;			//
	float				m_fJumpAccel;				//

public:
	BoundingSphere		m_BoundingBody;			//	경계구(캐릭터 몸체)
	BoundingCylinder	m_BoundingSpike;			//	경계실린더( 스파이크 확인용 )
	BoundingCylinder	m_BoundingDefense;		//	경계실린더( 리시브, 토스용 )
	BoundingSphere		m_BoundingHand;			//	경계구(캐릭터 손)

	D3DXVECTOR3			m_vReceivePoint;		//	m_matT와 리시브시의 손의 위치의 차이
	D3DXVECTOR3			m_vSlidingPoint;
	D3DXVECTOR3			m_vSpikePoint;
	D3DXVECTOR3			m_vTossPoint;
	D3DXVECTOR3			m_vBlockingPoint;
	D3DXVECTOR3			m_vOverServePoint;
	D3DXVECTOR3			m_vUnderServePoint;

	CBall					*m_pBall;				//	충돌검사에 사용할 볼
	
protected:
	//	메쉬컨테이너에 영향을 주는 각 본에 나중에 월드변환을 하는데 사용할 특정 프레임의 TM을 연결시켜준다.
	HRESULT	SetupBoneMatrixPointers( LPD3DXFRAME	pFrame );
	HRESULT	SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pSuperMeshContainer );

	HRESULT	DrawFrame(LPDIRECT3DDEVICE9	pDevice, LPD3DXFRAME	pFrame);
	HRESULT	DrawMeshContainer(LPDIRECT3DDEVICE9	pDevice,  LPD3DXFRAME pFrameBase, LPD3DXMESHCONTAINER	pSuperMeshContainer );
	HRESULT	DrawMeshContainerByNonIndexed(LPDIRECT3DDEVICE9	pDevice,  LPD3DXMESHCONTAINER pMeshContainer );
	HRESULT	DrawMeshContainerByIndexed( LPD3DXMESHCONTAINER pMeshContainer );
	HRESULT	DrawMeshContainerByVS( LPD3DXMESHCONTAINER pMeshContainer );
	HRESULT	DrawMeshContainerByHLSL(LPDIRECT3DDEVICE9	pDevice,  LPD3DXMESHCONTAINER pMeshContainer );
	HRESULT	DrawMeshContainerBySW( LPD3DXMESHCONTAINER pMeshContainer );

	HRESULT	CreateHandBoundingMesh();
	void	FindMeshByName( LPD3DXFRAME pFrame, LPCSTR pName , LPD3DXMESH *ppMesh, LPD3DXMATRIX *ppMat );

public:
	Character( LPCTSTR pCharacterFilePath, LPTSTR* ppCharacterFiles = NULL, int cntCharacterFiles = 0);
	Character( const	Character& SrcModel );
	virtual	~Character();
	HRESULT	virtual	InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT	virtual	RestoreDeviceObjects();
	HRESULT	virtual	InvalidateDeviceObjects();
	HRESULT	virtual	DeleteDeviceObjects();

	HRESULT	virtual	FrameMove( float fElapsedTime = 0.f );
	HRESULT	virtual	Render(LPDIRECT3DDEVICE9	pDevice);
	void	UpdateFrameMatrices( LPD3DXFRAME pFrame, const LPD3DXMATRIX	pParentMatrix );

	HRESULT	CreateBoundingObjects();
	void	ComputeValleyActionPoint( float	fRotationAngle );														//	각 배구동작시 손과 캐리터 매트릭스의 차이 계산	
	inline	void	MoveTo( float x, float y, float z ) { m_matT._41+=x;m_matT._42+=y;m_matT._43+=z; }

	void	SetAbility( const CharacterAbility& Ability ) { m_ability = Ability; }
	void	SetBall( CBall* pBall ) { m_pBall = pBall; }

	BOOL	GetNowJumping()	{ return m_bNowJumping; }
	const	BoundingSphere*		GetBoundingBody()	const {return &m_BoundingBody; }
	const	BoundingSphere*		GetBoundingHand()	const {return &m_BoundingHand; }
	const	BoundingCylinder*	GetBoundingSpike()	const {return &m_BoundingSpike;}
	const	BoundingCylinder*	GetBoundingDefense()const {return &m_BoundingDefense;}
	const	CharacterAnimationController* GetCharacterAnimationController() { return m_pAniController; }
	const	LPD3DXANIMATIONCONTROLLER	GetAnimationController( DWORD i ) { return m_pAniController->GetAnimaitionControllerPointer()[i]; }
	const	CharacterAbility*			GetCharacterAbility() { return	&m_ability; }
	const	LPD3DXFRAME					GetFrameRoot() { return m_pFrameRoot; }

};

#endif