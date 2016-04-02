
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/AnimationController.h"

CAnimationController::CAnimationController()
{
	m_pd3dDevice		= NULL;
	m_pAniController	= NULL;

	m_bPaused			= FALSE;
	m_bKeyPressed		= FALSE;

	m_numAniSet			= 0;
	m_currentAniSet		= 0;
	m_currentTrack		= 0;
	m_fTransitionTime	= 0.25f;
}

CAnimationController::~CAnimationController()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}

HRESULT		CAnimationController::InitDeviceObjects( LPDIRECT3DDEVICE9 pDevice )
{
	HRESULT	hr	= S_OK;

	//m_pd3dDevice	= pDevice;
	//m_pd3dDevice->AddRef();

	if( NULL == m_pAniController )
	return	E_FAIL;

	LPD3DXKEYFRAMEDANIMATIONSET		*ppOldSet	= NULL;				//	압축할 애니메이션 셋
	LPD3DXCOMPRESSEDANIMATIONSET	*ppNewSet	= NULL;				//	압축될 애니메이션 셋
	LPD3DXBUFFER					pCompressBuf= NULL;				//	압축된 애니메이션 셋 정보가 담길 버퍼

	DWORD	mx	= m_pAniController->GetMaxNumAnimationSets();
	m_numAniSet	= m_pAniController->GetNumAnimationSets();

	ppOldSet	= new LPD3DXKEYFRAMEDANIMATIONSET[ m_numAniSet ];
	ppNewSet	= new LPD3DXCOMPRESSEDANIMATIONSET[m_numAniSet ];

	if( NULL== ppOldSet || NULL== ppNewSet )
	{
		hr	= E_OUTOFMEMORY;
		goto	RELEASE;
	}
	ZeroMemory( ppOldSet, m_numAniSet * sizeof( LPD3DXKEYFRAMEDANIMATIONSET  ) );
	ZeroMemory( ppNewSet, m_numAniSet * sizeof( LPD3DXCOMPRESSEDANIMATIONSET ) );

	//	애니메이션 셋을 압축하고, 압축된 정보를 가지고 새로운 압축된 애니메이션 셋을 만든다.
	for( DWORD i=0; i<m_numAniSet; ++i )
	{
		hr	= m_pAniController->GetAnimationSet( i, (LPD3DXANIMATIONSET*)&ppOldSet[i] );
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
	for( DWORD i=0; i<m_numAniSet; ++i )
	{
		hr	= m_pAniController->UnregisterAnimationSet( ppOldSet[i] );
		CHECK_FAILED_GOTO( hr, RELEASE );
	}

	for( DWORD i=0; i<m_numAniSet; ++i )
	{
		hr	= m_pAniController->RegisterAnimationSet( ppNewSet[i] );
		CHECK_FAILED_GOTO( hr, RELEASE );
	}

	//	현재 애니메이션 셋과 트랙을 설정
	m_currentAniSet	= 0;
	m_currentTrack	= 0;
	m_pAniController->SetTrackAnimationSet( m_currentTrack, ppNewSet[m_currentAniSet] );
	m_pAniController->SetTrackEnable( m_currentTrack, TRUE );
	m_pAniController->SetTrackSpeed ( m_currentTrack, 1.f  );
	m_pAniController->SetTrackWeight( m_currentTrack, 1.f  );
	m_pAniController->SetTrackPosition( m_currentTrack, 0.f);
	m_pAniController->ResetTime();
		
RELEASE:
	for( DWORD i=0; i<m_numAniSet; ++ i )
	{
		SAFE_RELEASE( ppOldSet[i] );
		SAFE_RELEASE( ppNewSet[i] );		
	}
	SAFE_RELEASE( pCompressBuf );
	SAFE_DELETE_ARRAY( ppOldSet );
	SAFE_DELETE_ARRAY( ppNewSet );

	return	hr;
}

HRESULT		CAnimationController::InitDeviceObjects( LPDIRECT3DDEVICE9 pDevice, LPD3DXANIMATIONCONTROLLER pAniController )
{
	HRESULT	hr	= S_OK;

	if( NULL == pDevice || NULL == pAniController)
	{
		hr	= E_INVALIDARG;
		return hr;
	}
	m_pd3dDevice	= pDevice;
	m_pd3dDevice->AddRef();

	hr = pAniController->CloneAnimationController(
						    pAniController->GetMaxNumAnimationOutputs()
						,	pAniController->GetMaxNumAnimationSets()
						,	pAniController->GetMaxNumTracks()
						,	pAniController->GetMaxNumEvents()
						,	&m_pAniController
						);
	CHECK_FAILED( hr );

	for( DWORD i=0; i<m_pAniController->GetMaxNumTracks(); ++i )
	{
		m_pAniController->SetTrackEnable( i, FALSE );
	}

	return	hr;
}

HRESULT		CAnimationController::RestoreDeviceObjects()
{
	HRESULT	hr	= S_OK;

	return	hr;
}

HRESULT		CAnimationController::InvalidateDeviceObjects()
{
	HRESULT	hr	= S_OK;

	return	hr;
}

HRESULT		CAnimationController::DeleteDeviceObjects()
{
	HRESULT	hr	= S_OK;

	SAFE_RELEASE( m_pAniController );
	SAFE_RELEASE( m_pd3dDevice );

	return	hr;
}

LRESULT		CAnimationController::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HRESULT	hr	= S_OK;

	switch( uMsg )
	{
		case	WM_KEYUP:
			m_bKeyPressed	= FALSE;
			return 0;

		case	WM_KEYDOWN:
			
			//	이미 키가 눌러진 상태면 같은 애니메이션 셋으로 갱신하지 않는다.
			if( m_bKeyPressed ) 
				return 0;

			m_bKeyPressed	= TRUE;

			switch( wParam )
			{
				case	'N' :
					hr	= TransitionAnimation( m_currentAniSet + 1 < m_numAniSet ? m_currentAniSet+1 : 0  );
					if( FAILED( hr ) )
						return -1;

					return 0;
				case	'P' :
					hr	= TransitionAnimation( m_currentAniSet > 0? m_currentAniSet - 1 : m_numAniSet - 1 );
					if( FAILED( hr ) )
						return -1;

					return 0;
				case	VK_F9 :
					m_bPaused	= !m_bPaused;
					return 0;
			}
			return 0;
	}

	return	0;
}

HRESULT		CAnimationController::FrameMove( float	fElapsedTime )
{
	HRESULT	hr	= S_OK;

	if( NULL == m_pAniController )
		return E_FAIL;

	LPD3DXANIMATIONSET	pSet	= NULL;
	D3DXTRACK_DESC		track;
	DOUBLE				period	= 0.f;
	DOUBLE				pos		= 0.f;

	ZeroMemory( &track, sizeof track );

	hr	= m_pAniController->GetTrackDesc( m_currentTrack, &track );
	CHECK_FAILED( hr );
	hr	= m_pAniController->GetTrackAnimationSet( m_currentTrack, &pSet );
	CHECK_FAILED( hr );

	period	= pSet->GetPeriod();
	pos		= track.Position;

	SAFE_RELEASE( pSet );

	hr	= m_pAniController->AdvanceTime( fElapsedTime, NULL );

	return	hr;
}

HRESULT		CAnimationController::TransitionAnimation( DWORD	dwNewSet )
{
	HRESULT	hr	= S_OK;

	if( dwNewSet == m_currentAniSet )
		return hr;

	//	새로운 애니메이션 셋을 새 트랙에 셋팅한다.
	DWORD	dwNewTrack			= m_currentTrack==0? 1: 0;
	LPD3DXANIMATIONSET	pNewSet	= NULL;

	hr	= m_pAniController->GetAnimationSet( dwNewSet, &pNewSet );
	CHECK_FAILED( hr );

	hr	= m_pAniController->SetTrackAnimationSet( dwNewTrack, pNewSet );
	CHECK_FAILED_RELEASE( hr, pNewSet );

	SAFE_RELEASE( pNewSet );

	
	//	기존 트랙과 새 트랙을 블렌딩 시킨다.
	hr	= m_pAniController->ResetTime();
	CHECK_FAILED( hr );

	hr	= m_pAniController->KeyTrackEnable( m_currentTrack, FALSE, m_fTransitionTime );
	CHECK_FAILED( hr );
	hr	= m_pAniController->KeyTrackSpeed ( m_currentTrack, 0.f, 0.f, m_fTransitionTime, D3DXTRANSITION_LINEAR );
	CHECK_FAILED( hr );
	hr	= m_pAniController->KeyTrackWeight( m_currentTrack, 0.f, 0.f, m_fTransitionTime, D3DXTRANSITION_LINEAR );
	CHECK_FAILED( hr );

	hr	= m_pAniController->SetTrackPosition( dwNewTrack, 0.f );
	CHECK_FAILED( hr );
	hr	= m_pAniController->SetTrackEnable( dwNewTrack, TRUE );
	CHECK_FAILED( hr );
	hr	= m_pAniController->KeyTrackSpeed ( dwNewTrack, 1.f, 0.f, m_fTransitionTime, D3DXTRANSITION_LINEAR );
	CHECK_FAILED( hr );
	hr	= m_pAniController->KeyTrackWeight( dwNewTrack, 1.f, 0.f, m_fTransitionTime, D3DXTRANSITION_LINEAR );
	CHECK_FAILED( hr );

	m_currentAniSet	= dwNewSet;
	m_currentTrack	= dwNewTrack;

	return	hr;
}