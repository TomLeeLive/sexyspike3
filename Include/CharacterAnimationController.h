
#pragma		once

#ifndef		__CHARACTER_ANIMATION_CONTROLLER_H_
#define		__CHARACTER_ANIMATION_CONTROLLER_H_



enum	ePlayType
{
		ONETIME		= '1'
	,	LOOP		= '2'
	,	TO_DEFAULT	= '0'
};

struct	AniSetInfo
{
	DWORD		AnimationGroup;		//	�ִϸ��̼� �� �׷�
	DWORD		Index;				//	�׷쳻 �ִϸ��̼� �� �ε���
	DOUBLE		Period;				//	
	ePlayType	PlayType;			//
};

typedef	std::map< LPCSTR, AniSetInfo, StrCmpI >	ANISETMAP;

class		CharacterAnimationController
{
public:

	friend	class	CAnimationModel;

	LPD3DXANIMATIONCONTROLLER*	m_ppAniController;

	BOOL						m_bPaused;				//	�ִϸ��̼��� ������ �������� Ȯ��
	BOOL						m_bKeyPressed;			//	Ű�� ������ ���������ƴ���( ĳ���� ���ϸ��̼� ���� ������ �� ��� );
	
	enum ePlayType				m_ePlayType;			//	�ϳ��� �ִϸ��̼� �� ����� �Ϸ� �� �� ���� �ൿ ����
	
	DWORD						m_numAniController;		//	�ִϸ��̼� ��Ʈ�ѷ� ���̼�
	DWORD*						m_pNumAniSet;			//	�ִϸ��̼� �� ����
	DWORD						m_currentAniController;	//���� �ִϸ��̼� ��Ʈ�ѷ�
	DWORD						m_currentAniSet;		//	���� �ִϸ��̼� ��
	DWORD						m_currentTrack;			//	���� �ִϸ��̼� Ʈ��
	ANISETMAP					m_mapAniSets;			//	�ִϸ��̼� �� �ε����� �����ϴ� ��
	DOUBLE						m_fTransitionTime;
	DOUBLE						m_fSpeed;

	BOOL*						m_pOwnerMoveLock;

public:
	CharacterAnimationController( DWORD	numAniController = 0 );
	CharacterAnimationController( const	CharacterAnimationController& SrcController );
	virtual	~CharacterAnimationController();

	HRESULT	ConvertToCompressedAnimationSets( LPD3DXANIMATIONCONTROLLER	pAniController, DWORD* numAniSet, DWORD group );

	HRESULT	InitDeviceObjects( LPDIRECT3DDEVICE9 pDevice );
	HRESULT	RestoreDeviceObjects();
	HRESULT	InvalidateDeviceObjects();
	HRESULT	DeleteDeviceObjects();

	HRESULT	FrameMove( float	fElapsedTime );

	//	Ư�� �ִϸ��̼� ������ ��ȯ
	HRESULT	TransitionAnimation( DWORD	dwNewSet );
	//	Ư�� �ִϸ��̼� ������ ����
	HRESULT	JumpAnimationSet( DWORD	dwNewSet );
	
	//	������ �ӵ�, ���ӵ��� ����ؼ� �����ش�
	HRESULT	CalcJumpStuff( float ability, float *fVelocity, float *fAccel );

	//	Inline	Function
	//	...
	LPD3DXANIMATIONCONTROLLER*	GetAnimaitionControllerPointer()
	{
		return	this->m_ppAniController;
	}

	DOUBLE	GetPeriod( LPCSTR pAniSetName ) 
	{
		DOUBLE period = 0;
		LPD3DXANIMATIONSET	pSet = NULL;
		if( FAILED(m_ppAniController[m_mapAniSets[pAniSetName].AnimationGroup]->GetAnimationSet( m_mapAniSets[pAniSetName].Index, &pSet ) ) )
			return 0;
		period	= pSet->GetPeriod();
		SAFE_RELEASE( pSet );
		return period;
	}

	LPCSTR	GetCurrentAnimationSetName() {
		LPD3DXANIMATIONSET pSet	= NULL;
		LPCSTR	pName			= NULL;
		m_ppAniController[ m_currentAniController ]->GetAnimationSet( m_currentAniSet, &pSet );
		pName	= pSet->GetName();
		SAFE_RELEASE( pSet );
		return	pName;
	}

	HRESULT	SetAnimationSet( LPCSTR	pAniSetName );
	void	SetOwnerMoveLock( BOOL* pMoveLock ) { m_pOwnerMoveLock = pMoveLock; }
};

#endif