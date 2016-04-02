
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
	//09-08-25	������
	D3DXMATRIX			m_matS;									//	ĳ���� ��ü�� ���� TM(SCALE)
	D3DXMATRIX			m_matR;									//	ĳ���� ��ü�� ���� TM(ROTATION)
	D3DXMATRIX			m_matT;									//	ĳ���� ��ü�� ���� TM(TRANSLATION)
	D3DXMATRIX			m_matShadow;
	float				m_fRotationAngle;						//	ĳ���� ȸ������
	CharacterAnimationController* m_pAniController;		//	�ִϸ��̼� ��Ʈ�Ѷ�

	//	ĳ���� ���� ����
	LPD3DXFRAME			m_pFrameRoot;			//	D3DXFRAME�������� ����� ���� ���� Ŭ������ CFrame���̴�.

protected:
	//	ĳ���� ���� ����
	BOOL				m_bFrameOwner;				//	�������� ���������� ��ü����
	D3DXMATRIX			m_matWorld;				//	S*R*T
	float				m_fElapsedTime;			//

	bool				m_bDrawShadow;

	LPTSTR				m_pCharacterFilePath;
	LPTSTR*				m_ppCharacterFiles;		//	�ִϸ��̼� ����(XFILE )
	int					m_cntCharacterFiles;		//	X���� ����

	CharacterAbility	m_ability;					//	ĳ���� �ɷ�ġ
	//	����
	BOOL				m_bNowJumping;				//
	float				m_fJumpTime;				//
	float				m_fJumpVelocity;			//
	float				m_fJumpAccel;				//

public:
	BoundingSphere		m_BoundingBody;			//	��豸(ĳ���� ��ü)
	BoundingCylinder	m_BoundingSpike;			//	���Ǹ���( ������ũ Ȯ�ο� )
	BoundingCylinder	m_BoundingDefense;		//	���Ǹ���( ���ú�, �佺�� )
	BoundingSphere		m_BoundingHand;			//	��豸(ĳ���� ��)

	D3DXVECTOR3			m_vReceivePoint;		//	m_matT�� ���ú���� ���� ��ġ�� ����
	D3DXVECTOR3			m_vSlidingPoint;
	D3DXVECTOR3			m_vSpikePoint;
	D3DXVECTOR3			m_vTossPoint;
	D3DXVECTOR3			m_vBlockingPoint;
	D3DXVECTOR3			m_vOverServePoint;
	D3DXVECTOR3			m_vUnderServePoint;

	CBall					*m_pBall;				//	�浹�˻翡 ����� ��
	
protected:
	//	�޽������̳ʿ� ������ �ִ� �� ���� ���߿� ���庯ȯ�� �ϴµ� ����� Ư�� �������� TM�� ��������ش�.
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
	void	ComputeValleyActionPoint( float	fRotationAngle );														//	�� �豸���۽� �հ� ĳ���� ��Ʈ������ ���� ���	
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