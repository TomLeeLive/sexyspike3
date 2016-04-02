
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/CharacterAnimationController.h"


CharacterAnimationController::CharacterAnimationController(DWORD	numAniController)
{
	m_ppAniController		= NULL;

	m_bPaused				= FALSE;
	m_bKeyPressed			= FALSE;

	m_ePlayType				= TO_DEFAULT;

	m_numAniController		= numAniController;
	m_pNumAniSet			= NULL;
	m_currentAniController	= 0;
	m_currentAniSet			= 0;
	m_currentTrack			= 0;
	m_fTransitionTime		= 0.2f;
	m_fSpeed				= 1.f;
	m_pOwnerMoveLock		= NULL;

	if( 0 != m_numAniController )
	{
		m_ppAniController		= new LPD3DXANIMATIONCONTROLLER[ m_numAniController ];
		m_pNumAniSet			= new DWORD[ m_numAniController ];
		ZeroMemory( m_ppAniController, sizeof( LPD3DXANIMATIONCONTROLLER ) * m_numAniController );
		ZeroMemory( m_pNumAniSet,		sizeof( DWORD ) * m_numAniController );
	}
}

CharacterAnimationController::CharacterAnimationController(const	CharacterAnimationController& SrcController )
{
	m_bPaused				= FALSE;
	m_bKeyPressed			= FALSE;

	m_numAniController	=	SrcController.m_numAniController;
	m_currentAniController=	0;
	m_currentAniSet		=	0;
	m_currentTrack		=	0;
	m_fTransitionTime	=	SrcController.m_fTransitionTime;
	m_fSpeed			=	SrcController.m_fSpeed;
	m_ePlayType			=	SrcController.m_ePlayType;
	m_pOwnerMoveLock	= NULL;

	m_ppAniController	=	new	LPD3DXANIMATIONCONTROLLER[ m_numAniController ];
	m_pNumAniSet		=	new DWORD[ m_numAniController ];
	for( DWORD i=0; i<m_numAniController; ++i )
	{
		m_pNumAniSet[i]	=	SrcController.m_pNumAniSet[i];
		SrcController.m_ppAniController[i]->CloneAnimationController(
							SrcController.m_ppAniController[i]->GetMaxNumAnimationOutputs()
						,	SrcController.m_ppAniController[i]->GetMaxNumAnimationSets()
						,	SrcController.m_ppAniController[i]->GetMaxNumTracks()
						,	SrcController.m_ppAniController[i]->GetMaxNumEvents()
						,	&m_ppAniController[i]
		);
		for( DWORD x=0; x<SrcController.m_ppAniController[i]->GetMaxNumTracks(); ++x )
		{
			m_ppAniController[i]->SetTrackEnable( x, FALSE );
		}
	}

	ANISETMAP::const_iterator	it	= SrcController.m_mapAniSets.begin();
	while( it != SrcController.m_mapAniSets.end() )
	{
		LPSTR		pName	= NULL;
		CopyString( it->first, &pName );
		m_mapAniSets.insert( ANISETMAP::value_type( pName, it->second) );
		it++; 
	}
}
CharacterAnimationController::~CharacterAnimationController()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();	

	SAFE_DELETE_ARRAY( m_ppAniController );
	SAFE_DELETE_ARRAY( m_pNumAniSet );
}


HRESULT		CharacterAnimationController::ConvertToCompressedAnimationSets( LPD3DXANIMATIONCONTROLLER pAniController, DWORD* numAniSet, DWORD group )
{
	HRESULT	hr	= S_OK;

	assert( pAniController );

	LPD3DXKEYFRAMEDANIMATIONSET		*ppOldSet	= NULL;				//	압축할 애니메이션 셋
	LPD3DXCOMPRESSEDANIMATIONSET	*ppNewSet	= NULL;				//	압축될 애니메이션 셋
	LPD3DXBUFFER					pCompressBuf= NULL;				//	압축된 애니메이션 셋 정보가 담길 버퍼

	*numAniSet	= pAniController->GetNumAnimationSets();

	ppOldSet	= new LPD3DXKEYFRAMEDANIMATIONSET[ *numAniSet ];
	ppNewSet	= new LPD3DXCOMPRESSEDANIMATIONSET[*numAniSet ];

	if( NULL== ppOldSet || NULL== ppNewSet )
	{
		hr	= E_OUTOFMEMORY;
		goto	RELEASE;
	}
	ZeroMemory( ppOldSet, *numAniSet * sizeof( LPD3DXKEYFRAMEDANIMATIONSET  ) );
	ZeroMemory( ppNewSet, *numAniSet * sizeof( LPD3DXCOMPRESSEDANIMATIONSET ) );

	//	애니메이션 셋을 압축하고, 압축된 정보를 가지고 새로운 압축된 애니메이션 셋을 만든다.
	for( DWORD i=0; i<*numAniSet; ++i )
	{
		hr	= pAniController->GetAnimationSet( i, (LPD3DXANIMATIONSET*)&ppOldSet[i] );
		CHECK_FAILED_GOTO( hr, RELEASE );
		
		hr	= ppOldSet[i]->Compress( D3DXCOMPRESS_DEFAULT, COMPRESS_RATIO, NULL, &pCompressBuf );
		CHECK_FAILED_GOTO( hr, RELEASE );

		hr	= D3DXCreateCompressedAnimationSet(
								ppOldSet[i]->GetName()
							,	ppOldSet[i]->GetSourceTicksPerSecond()
							,	ppOldSet[i]->GetPlaybackType()
							,	pCompressBuf
							,	NULL
							,	NULL
							,	&ppNewSet[i]
		);
		CHECK_FAILED_GOTO( hr, RELEASE );
	}

	//	기존의 애니메이션 셋과 압축한 애니메이션 셋을 교체한다
	for( DWORD i=0; i<(*numAniSet); ++i )
	{
		hr	= pAniController->UnregisterAnimationSet( ppOldSet[i] );
		CHECK_FAILED_GOTO( hr, RELEASE );
	}

	for( DWORD i=0; i<*numAniSet; ++i )
	{
		hr	= pAniController->RegisterAnimationSet( ppNewSet[i] );
		CHECK_FAILED_GOTO( hr, RELEASE );
	}
	//	각 애니메이션 셋의 이름에 맞는 인덱스 정보를 저장한다
	for( DWORD i=0; i<*numAniSet; ++i )
	{
		LPCSTR		name			= NULL;
		AniSetInfo	aniSetInfo;
		name						= ppNewSet[i]->GetName();

		if( NULL == name  )
		{
			goto RELEASE;
		}

		size_t	length	= strlen( name ) + 1;
		LPSTR	pNewName= new CHAR[ length ];

		strcpy_s( pNewName, length, name );

		aniSetInfo.AnimationGroup = group;
		aniSetInfo.Index			= i;
		aniSetInfo.Period			= ppNewSet[i]->GetPeriod();
		aniSetInfo.PlayType		= _strnicmp( pNewName, "WALK", 4 ) ? TO_DEFAULT: LOOP;

		m_mapAniSets.insert( ANISETMAP::value_type( pNewName, aniSetInfo ) );
	}

RELEASE:
	for( DWORD i=0; i<*numAniSet; ++ i )
	{
		SAFE_RELEASE( ppOldSet[i] );
		SAFE_RELEASE( ppNewSet[i] );		
	}
	SAFE_RELEASE( pCompressBuf );
	SAFE_DELETE_ARRAY( ppOldSet );
	SAFE_DELETE_ARRAY( ppNewSet );

	return	hr;
}
HRESULT		CharacterAnimationController::InitDeviceObjects( LPDIRECT3DDEVICE9 pDevice )
{
	HRESULT	hr	= S_OK;
	
	if( 0 == m_numAniController )
		return	hr;

	for( DWORD i=0; i<m_numAniController; ++i )
	{
		ConvertToCompressedAnimationSets( m_ppAniController[i], &m_pNumAniSet[i], i );
	}

	//	현재 애니메이션 셋과 트랙을 설정
	m_currentAniController	= 0;
	m_currentAniSet			= m_mapAniSets[ "WAITING" ].Index;
	m_currentTrack			= 0;

	LPD3DXANIMATIONSET	pAniSet	= NULL;
	hr	= m_ppAniController[m_currentAniController]->GetAnimationSet( m_currentAniSet, &pAniSet );
	CHECK_FAILED( hr );
	hr	= m_ppAniController[m_currentAniController]->SetTrackAnimationSet( m_currentTrack, pAniSet );
	SAFE_RELEASE( pAniSet );
	
	m_ppAniController[m_currentAniController]->SetTrackEnable( m_currentTrack, TRUE );
	m_ppAniController[m_currentAniController]->SetTrackSpeed ( m_currentTrack, 1.f  );
	m_ppAniController[m_currentAniController]->SetTrackWeight( m_currentTrack, 1.f  );
	m_ppAniController[m_currentAniController]->SetTrackPosition( m_currentTrack, 0.f);
	m_ppAniController[m_currentAniController]->ResetTime();

	return	hr;
}

HRESULT		CharacterAnimationController::RestoreDeviceObjects()
{
	HRESULT	hr	= S_OK;

	return	hr;
}

HRESULT		CharacterAnimationController::InvalidateDeviceObjects()
{
	HRESULT	hr	= S_OK;

	return	hr;
}

HRESULT		CharacterAnimationController::DeleteDeviceObjects()
{
	HRESULT	hr	= S_OK;

	ANISETMAP::iterator	it = m_mapAniSets.begin();
	while( it != m_mapAniSets.end() )
	{
		if( NULL != it->first )
			delete[] it->first;
		++it;
	}
	m_mapAniSets.clear();

	for( DWORD i=0; i<m_numAniController; ++i )
	{
		SAFE_RELEASE( m_ppAniController[i] );
	}

	return	hr;
}

HRESULT		CharacterAnimationController::FrameMove( float	fElapsedTime )
{
	HRESULT	hr	= S_OK;

	if( NULL == m_ppAniController )
		return E_FAIL;

	LPD3DXANIMATIONSET	pSet	= NULL;
	D3DXTRACK_DESC		track;
	DOUBLE				period	= 0.f;
	DOUBLE				pos		= 0.f;
	LPCTSTR				pName	= NULL;

	ZeroMemory( &track, sizeof track );

	hr	= m_ppAniController[m_currentAniController]->GetTrackDesc( m_currentTrack, &track );
	CHECK_FAILED( hr );
	hr	= m_ppAniController[m_currentAniController]->GetTrackAnimationSet( m_currentTrack, &pSet );
	CHECK_FAILED( hr );

	period	= pSet->GetPeriod();
	pos		= track.Position;
	pName	= pSet->GetName();

	if( (pos+fElapsedTime) > 0.2495f && (pos+fElapsedTime)< 0.2505f )
		int a = 0;

	SAFE_RELEASE( pSet );

	if( (pos-period) >= 0.0001f  )
	{
		switch( m_mapAniSets[pName].PlayType )
		{
			case	ONETIME :
				break;
			case	LOOP :
				break;
			case	TO_DEFAULT :
				if( !m_bKeyPressed )
				{
					if( m_pOwnerMoveLock )
						*m_pOwnerMoveLock	= FALSE;
					if( m_currentAniController != 0 )
					{
						m_currentAniController	= 0;
						hr	= JumpAnimationSet( m_mapAniSets["WAITING"].Index );
						CHECK_FAILED( hr );
						break;
					}
					else
					{
						hr	= TransitionAnimation( m_mapAniSets["WAITING"].Index );
						CHECK_FAILED( hr );
						break;
					}
				}
		}

		hr	= m_ppAniController[m_currentAniController]->ResetTime();
		CHECK_FAILED( hr );
	}

	hr	= m_ppAniController[m_currentAniController]->AdvanceTime( fElapsedTime, NULL );

	return	hr;
}

HRESULT		CharacterAnimationController::TransitionAnimation( DWORD	dwNewSet )
{
	HRESULT	hr	= S_OK;

	if( dwNewSet == m_currentAniSet )
		return hr;

	//	새로운 애니메이션 셋을 새 트랙에 셋팅한다.
	DWORD	dwNewTrack			= m_currentTrack==0? 1: 0;
	LPD3DXANIMATIONSET	pNewSet	= NULL;

	hr	= m_ppAniController[m_currentAniController]->GetAnimationSet( dwNewSet, &pNewSet );
	CHECK_FAILED( hr );

	hr	= m_ppAniController[m_currentAniController]->SetTrackAnimationSet( dwNewTrack, pNewSet );
	CHECK_FAILED_RELEASE( hr, pNewSet );

	SAFE_RELEASE( pNewSet );

	
	//	기존 트랙과 새 트랙을 블렌딩 시킨다.
	hr	= m_ppAniController[m_currentAniController]->ResetTime();
	CHECK_FAILED( hr );

	hr	= m_ppAniController[m_currentAniController]->KeyTrackEnable( m_currentTrack, FALSE, m_fTransitionTime );
	CHECK_FAILED( hr );
	hr	= m_ppAniController[m_currentAniController]->KeyTrackSpeed ( m_currentTrack, 0.f, 0.f, m_fTransitionTime, D3DXTRANSITION_LINEAR );
	CHECK_FAILED( hr );
	hr	= m_ppAniController[m_currentAniController]->KeyTrackWeight( m_currentTrack, 0.f, 0.f, m_fTransitionTime, D3DXTRANSITION_LINEAR );
	CHECK_FAILED( hr );

	hr	= m_ppAniController[m_currentAniController]->SetTrackPosition( dwNewTrack, 0.f );
	CHECK_FAILED( hr );
	hr	= m_ppAniController[m_currentAniController]->SetTrackEnable( dwNewTrack, TRUE );
	CHECK_FAILED( hr );
	hr	= m_ppAniController[m_currentAniController]->KeyTrackSpeed ( dwNewTrack, 1.f, 0.f, m_fTransitionTime, D3DXTRANSITION_LINEAR );
	CHECK_FAILED( hr );
	hr	= m_ppAniController[m_currentAniController]->KeyTrackWeight( dwNewTrack, 1.f, 0.f, m_fTransitionTime, D3DXTRANSITION_LINEAR );
	CHECK_FAILED( hr );

	m_currentAniSet	= dwNewSet;
	m_currentTrack	= dwNewTrack;

	return	hr;
}


HRESULT		CharacterAnimationController::JumpAnimationSet( DWORD	dwNewSet )
{
	HRESULT		hr	= S_OK;

	LPD3DXANIMATIONSET	pAniSet	= NULL;

	hr	= m_ppAniController[m_currentAniController]->GetAnimationSet( dwNewSet, &pAniSet );
	CHECK_FAILED( hr );

	hr	= m_ppAniController[m_currentAniController]->SetTrackAnimationSet( m_currentTrack, pAniSet );
	CHECK_FAILED_RELEASE( hr, pAniSet );

	hr	= m_ppAniController[m_currentAniController]->SetTrackPosition( m_currentTrack, 0 );
	CHECK_FAILED_RELEASE( hr, pAniSet );

	hr	= m_ppAniController[m_currentAniController]->SetTrackEnable( m_currentTrack, TRUE );
	hr	= m_ppAniController[m_currentAniController]->SetTrackSpeed ( m_currentTrack, 1.f );
	hr	= m_ppAniController[m_currentAniController]->SetTrackWeight( m_currentTrack, 1.f );

	SAFE_RELEASE( pAniSet );

	m_currentAniSet	= dwNewSet;

	return		hr;
}

HRESULT		CharacterAnimationController::CalcJumpStuff( float ability,  float* fVelocity, float* fAccel )
{
	HRESULT		hr	= S_OK;

	LPD3DXANIMATIONSET	pAniSet	= NULL;
	DOUBLE				period	= 0.f;
	hr	=	m_ppAniController[0]->GetAnimationSet( m_mapAniSets["SPIKE_JUMP"].Index, &pAniSet );
	CHECK_FAILED( hr );

	period	= pAniSet->GetPeriod();

	*fVelocity	= float(period)*ability;
	*fAccel		= -2* *fVelocity;

	SAFE_RELEASE( pAniSet );

	return		hr;
}

HRESULT		CharacterAnimationController::SetAnimationSet( LPCSTR pAniSetName )
{
	HRESULT		hr	= S_OK;

	if( NULL == pAniSetName )
		return	E_FAIL;

	if( 0 == _stricmp(GetCurrentAnimationSetName(), pAniSetName) )
		return	S_OK;

	AniSetInfo	aniSetInfo	= m_mapAniSets[ pAniSetName];

	if( aniSetInfo.AnimationGroup	== m_currentAniController )
	{
		if( aniSetInfo.Index != m_currentAniSet )
		{
			D3DXTRACK_DESC	track;
			ZeroMemory( &track, sizeof D3DXTRACK_DESC );
			m_ppAniController[m_currentAniController]->GetTrackDesc( m_currentTrack, &track );

			if( track.Position > 0.2f )
			{
				hr	= TransitionAnimation( aniSetInfo.Index );
			}
			else
			{
				hr	= JumpAnimationSet( aniSetInfo.Index );
			}
		}
	}
	else
	{
		m_currentAniController	= aniSetInfo.AnimationGroup;
		hr	= JumpAnimationSet( aniSetInfo.Index );
	}

	return		hr;
}