
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
	DWORD		AnimationGroup;		//	애니메이션 셋 그룹
	DWORD		Index;				//	그룹내 애니메이션 셋 인덱스
	DOUBLE		Period;				//	
	ePlayType	PlayType;			//
};

typedef	std::map< LPCSTR, AniSetInfo, StrCmpI >	ANISETMAP;

class		CharacterAnimationController
{
public:

	friend	class	CAnimationModel;

	LPD3DXANIMATIONCONTROLLER*	m_ppAniController;

	BOOL						m_bPaused;				//	애니메이션이 정지된 상태인지 확인
	BOOL						m_bKeyPressed;			//	키가 눌려진 상태인지아닌지( 캐리터 에니메이션 셋을 변경할 때 사용 );
	
	enum ePlayType				m_ePlayType;			//	하나의 애니메이션 셋 재생이 완료 된 후 다음 행동 결정
	
	DWORD						m_numAniController;		//	애니메이션 컨트롤러 개ㅜ수
	DWORD*						m_pNumAniSet;			//	애니메이션 셋 갯수
	DWORD						m_currentAniController;	//현재 애니메이션 컨트롤러
	DWORD						m_currentAniSet;		//	현재 애니메이션 셋
	DWORD						m_currentTrack;			//	현재 애니메이션 트랙
	ANISETMAP					m_mapAniSets;			//	애니메이션 셋 인덱스를 저장하는 맵
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

	//	특정 애니메이션 셋으로 변환
	HRESULT	TransitionAnimation( DWORD	dwNewSet );
	//	특정 애니메이션 셍으로 점프
	HRESULT	JumpAnimationSet( DWORD	dwNewSet );
	
	//	점프시 속도, 가속도를 계산해서 돌려준다
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