
#include	"./Include/DEFAULT_INCLUDE.h"
#include	"./Include/INCLUDE.h"

#ifdef	_DEBUG
int				g_cnt;
bool			g_bool;
char			g_buf[256];
char			g_filename[] = {"./DEBUGLOG.txt"};
char			g_filemode[] = {"a+"};
FILE			*g_fp;
HWND			g_hWnd	= NULL;
D3DXMATRIX		g_matT	= UNIT_MATRIX;
#endif

CEffectManager	g_Effects;
D3DXMATRIX		g_matInvView	= UNIT_MATRIX;
D3DXMATRIX		g_matViewProj	= UNIT_MATRIX;
D3DXMATRIX		g_matShadow		= UNIT_MATRIX;
D3DXMATRIX		*g_pPalette		= NULL;
DWORD			g_numPalette		=0;
BoundingPlane	g_BoundingMesh;									//	넷트평면
D3DLIGHT9		g_light;
CKeyManager		g_keys;											//	키입력
int				g_CntBallTouched	= 0;
void*			g_pPlayerLastTouch	= NULL;
void*			g_pPlayerRedShift	= NULL;
void**			g_ppPlayers			= NULL;						//	플레이어 포인터
TeamInfo		g_Teams[2];										//	팀정보
int				g_score[2]			= {0,0};
BOOL			g_bNowServe			= FALSE;
D3DXVECTOR3		g_dir[4];

BYTE			g_packet[PACKET_TOTAL];	
PLAYERSMAP		g_mapNetObjects;

//09-08-04 이재준
SexySpike*		g_pApp  = NULL;

//09-09-07 이재준
CDrawText		g_text;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    SexySpike d3dApp;

	//09-08-04 이재준
	g_pApp  = &d3dApp;

	D3DXVECTOR3 vA = D3DXVECTOR3( 1.f, 0, 0 );

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-----------------------------------------------------------------------------
// Name: SexySpike()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
SexySpike::SexySpike()
{
	m_pBackground						= NULL;
	m_pNet								= NULL;
	m_pBall								= NULL;
	m_pFont								= NULL;
	m_pFontSmall						= NULL;
	m_pCrashManager						= NULL;
	m_bBallReflected					= FALSE;

	m_eRenderMode						= AFTERIMAGE;
	m_LoofCounter						= 0;

    m_strWindowTitle					= _T("섹시 스파이크 v0.2");
	m_d3dEnumeration.AppUsesMixedVP		= TRUE;
    m_d3dEnumeration.AppUsesDepthBuffer	= TRUE;
	m_dwCreationWidth					= 1024;
    m_dwCreationHeight					= 768; 

    m_fSpeed							= 5.0f;
    m_fAngularSpeed						= 1.0f;
    m_bShowHelp							= FALSE;

    m_vVelocity							= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vAngularVelocity					= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	ZeroMemory( m_ppPlayers, NUMPLAYERS*sizeof(CPlayer*) );
	ZeroMemory( m_pModels, NUMMODELS*sizeof(Character*) );
	ZeroMemory( m_pPostTex, sizeof(m_pPostTex));
	ZeroMemory( m_pPostSuf, sizeof(m_pPostSuf));
	ZeroMemory( m_pPostDep, sizeof(m_pPostDep));

	m_pSlapTex	= NULL;
	m_pSlap		= NULL;
	m_slapcount= 4;
	ZeroMemory(&m_vSlapPos, sizeof(D3DXVECTOR3));


	//09-08-04 이재준
	m_pCamMn = NULL;
	m_pInput = NULL;

	m_nGamePhase = 0;

	//09-08-07 이재준
	int i=0;
	for(i=0;i<8;++i)
	{
		m_pMp[i]		= NULL;
	}

	//09-08-11 이재준
	m_bMultiGame = FALSE;

	//09-08-12 이재준
	m_nRedTeamScore=0;							//레드팀 스코어
	m_nBlueTeamScore=0;							//블루팀 스코어
	m_nSetNumber=1;								//현재 세트

	m_bLoadingApp               = TRUE;

	//m_pCameraAniModel			   = NULL;

	m_pCameraAniModel			   = new CCameraAnimationModel( TEXT("./XFILE/CAMERA/camera.x" ) );

	m_StartScreen		= new CStartScreen;
	m_MultiPlay		= new CMultiPlay;
	m_HowtoPlay		= new CHowtoPlay;
	m_EndGame			= new CEndGame;
	m_CharacterSelect	= new CCharacterSelect;

	//09-08-28 이재준
	m_GameSceneManager= new CGameSceneManager;

	//09-08-25 이재준
	SetRect(&m_rc,0,0,1024,768);
	m_vcPos= D3DXVECTOR3(0,0,0);			//배경위치

	//09-08-27 이재준
	m_bQuitButton = FALSE;

	m_nRedTeamWins=0;
	m_nBlueTeamWins=0;

	m_bTest = FALSE;

	if(m_bTest)
	{m_nGamePhase = 1;}
	else
	{m_nGamePhase = 0;}

	////////////////////////////////////////////
	////멀티플레이 Test
	//m_nGamePhase=5;
	//m_bMultiGame=TRUE;
	////////////////////////////////////////////

	//09-09-08 이재준
	m_vcIcon[0].vcPos1=D3DXVECTOR3(280,94,0);m_vcIcon[0].vcPos2=D3DXVECTOR3(269,14,0);
	m_vcIcon[1].vcPos1=D3DXVECTOR3(362,94,0);m_vcIcon[1].vcPos2=D3DXVECTOR3(351,14,0);
	m_vcIcon[2].vcPos1=D3DXVECTOR3(609,94,0);m_vcIcon[2].vcPos2=D3DXVECTOR3(599,14,0);
	m_vcIcon[3].vcPos1=D3DXVECTOR3(692,94,0);m_vcIcon[3].vcPos2=D3DXVECTOR3(680,14,0);


	//09-09-04 이웅재
	m_NetworkDlg = new CNetworkDlg;
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT SexySpike::OneTimeSceneInit()
{
	//09-08-25 이재준
	// Drawing loading status message until app finishes loading
	SendMessage( m_hWnd, WM_PAINT, 0, 0 );



	m_pBackground						= new CBackground;
	m_pNet								= new CNet;
	m_pBall								= new CBall;

	if(m_bTest)
	{
		LPTSTR	XFILES[2];
		XFILES[0]							= new TCHAR[MAX_PATH];//TEXT("./XFILE/CHARACTER/ESK/_test_1_bin.x" );
		XFILES[1]							= new TCHAR[MAX_PATH];//TEXT("./XFILE/CHARACTER/ESK/_test_2_bin.x" );
		for( int i=0; i<NUMMODELS; ++i )
		{
			lstrcpy( XFILES[0], g_arrModelPath[MARIA] );
			lstrcpy( XFILES[1], g_arrModelPath[MARIA] );
			lstrcat( XFILES[0], TEXT("_character_1_bin.x") );
			lstrcat( XFILES[1], TEXT("_character_2_bin.x") );
			m_pModels[i]	= new	Character( g_arrModelPath[MARIA], XFILES, 2 );
		}
		SAFE_DELETE_ARRAY( XFILES[0] );
		SAFE_DELETE_ARRAY( XFILES[1] );	
	}
	else
	{
		/////////////////////////////////////////////////////////////////////////////
		//09-08-25 이재준
		LPTSTR	XFILES[NUMMODELS*2];
		for( int i=0; i<NUMMODELS*2; ++i)
		{
			XFILES[i]							= new TCHAR[MAX_PATH];//TEXT("./XFILE/CHARACTER/ESK/_test_1_bin.x" );
		}

		for( int i=0; i<NUMMODELS; ++i)
		{
			lstrcpy( XFILES[i*2], g_arrModelPath[i] );
			lstrcpy( XFILES[i*2+1], g_arrModelPath[i] );
			lstrcat( XFILES[i*2], TEXT("_character_1_bin.x") );
			lstrcat( XFILES[i*2+1], TEXT("_character_2_bin.x") );
			m_pModels[i]	= new	Character( g_arrModelPath[i], &XFILES[i*2], 2 );
		}

		for( int i=0; i<NUMMODELS*2; ++i)
		{
			SAFE_DELETE_ARRAY( XFILES[i] );
		}
		/////////////////////////////////////////////////////////////////////////////
	}	

	m_pFont								= new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_pFontSmall						= new CD3DFont( _T("Arial"),  9, D3DFONT_BOLD );

	D3DXVECTOR3 from( 0.0f, 0.0f, -3.0f );
	D3DXVECTOR3 at( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );

	//	중복입력을 허용할 키 입력
	g_keys.SetAcceptOverlapping( VK_UP );
	g_keys.SetAcceptOverlapping( VK_LEFT );
	g_keys.SetAcceptOverlapping( VK_DOWN );
	g_keys.SetAcceptOverlapping( VK_RIGHT);


	// 09-09-04 이웅재
	m_NetworkDlg->Create();


	//09-08-25 이재준
	m_bLoadingApp = FALSE;


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT SexySpike::InitDeviceObjects()
{
	HRESULT	hr	= S_OK;

	hr	= g_Effects.InitDeviceObjects( m_pd3dDevice );
	CHECK_FAILED( hr );

    hr	= m_pFont->InitDeviceObjects( m_pd3dDevice );
	CHECK_FAILED( hr );

    hr	= m_pFontSmall->InitDeviceObjects( m_pd3dDevice );
	CHECK_FAILED( hr );


	hr	= m_pBackground->InitDeviceObjects( m_pd3dDevice );
	CHECK_FAILED( hr );

	hr	= m_pNet->InitDeviceObjects( m_pd3dDevice );
	CHECK_FAILED( hr );

	INITDEVICEOBJECTS( m_pBall, m_pd3dDevice );

	for( int i=0; i<NUMMODELS; ++i )
	{
		INITDEVICEOBJECTS( m_pModels[i], m_pd3dDevice );
		m_pModels[i]->SetBall( m_pBall );
	}

	m_eFlow	= READYPLAY;

	m_pCrashManager	= new CrashManager;

	hr	= D3DXCreateTextureFromFile(
			m_pd3dDevice
		,	"./TEXTURE/slap.tga"
		,	&m_pSlapTex
		);
	CHECK_FAILED(hr);

	hr	= m_pd3dDevice->CreateVertexBuffer(
			4*sizeof(SimpleVertex)
		,	0
		,	SimpleVertex::SimpleVertexFVF
		,	D3DPOOL_MANAGED
		,	&m_pSlap
		,	NULL
		);
	CHECK_FAILED(hr);
	SimpleVertex	*pSlap	= NULL;
	float	size	= 5.f*m_pBall->GetBoundingRadius();
	CHECK_FAILED(m_pSlap->Lock(0, 0, (void**)&pSlap, 0));
	pSlap[0].p	= D3DXVECTOR3(-size, -size, 0.f);
	pSlap[1].p	= D3DXVECTOR3(-size,  size, 0.f);
	pSlap[2].p	= D3DXVECTOR3( size,  size, 0.f);
	pSlap[3].p	= D3DXVECTOR3( size, -size, 0.f);
	pSlap[0].tu	= 0.f;		pSlap[0].tv	= 1.f;
	pSlap[1].tu	= 0.f;		pSlap[1].tv	= 0.f;
	pSlap[2].tu	= 0.25f;	pSlap[2].tv	= 0.f;
	pSlap[3].tu	= 0.25f;	pSlap[3].tv	= 1.f;
	CHECK_FAILED(m_pSlap->Unlock());
	

#ifdef	_DEBUG
	g_hWnd	= m_hWnd;
#endif


	//09-09-09 이웅재
	m_netAdapter.SetSecretkey(1234);
	//m_netAdapter.ClientStart();
	g_pNetAdapter = &m_netAdapter;
	//g_hWnd	= m_hWnd;


	//09-08-04 이재준
	SAFE_NEWCREATE1(m_pCamMn,	CMcCamManager, m_pd3dDevice);
	SAFE_NEWCREATE1(m_pInput , CMcInput		, m_hWnd);

	//m_pTexture.TextureLoad("./Texture/title.png");

	hr=this->TextureLoad();

	hr=this->InitSoundObjects();

	//09-08-20 이재준
	m_pCameraAniModel->InitDeviceObjects( m_pd3dDevice );

	//09-09-07 이재준
	g_text.Create(m_pd3dDevice);



    return hr;
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT SexySpike::RestoreDeviceObjects()
{
    HRESULT hr = S_OK;

	ZeroMemory( &g_light, sizeof	D3DLIGHT9 );
	g_light.Type		= D3DLIGHT_DIRECTIONAL;
	g_light.Ambient		= D3DXCOLOR( 0.3f, 0.3f, 0.3f, 0.3f );
	g_light.Diffuse		= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	g_light.Specular	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
	D3DXVECTOR3 d		= D3DXVECTOR3( -0.6f, -1.f, 0.5f );
	D3DXVec3Normalize( &d, &d );
	g_light.Direction	= d;
	
	m_pd3dDevice->SetLight(0, &g_light );
	m_pd3dDevice->LightEnable(0, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );


	//	Reset Device
	CHECK_FAILED( hr = g_Effects.RestoreDeviceObjects() );
    CHECK_FAILED( hr = m_pFont->RestoreDeviceObjects() );
    CHECK_FAILED( hr = m_pFontSmall->RestoreDeviceObjects() );

	CHECK_FAILED( hr = m_pBackground->RestoreDeviceObjects() );
	CHECK_FAILED( hr = m_pNet->RestoreDeviceObjects(m_pd3dDevice) );

	CHECK_FAILED( hr = m_pBall->RestoreDeviceObjects(m_pd3dDevice) );

	switch( m_eFlow )
	{
		case PLAY:
		{
			for( int i=0; i<NUMPLAYERS; ++i )
			{
				RESTOREDEVICEOBJECTS( m_ppPlayers[i] );	
			}
			break;
		}
	}

	//	포스트이펙트 준비
	m_LoofCounter = 0;
	D3DSURFACE_DESC	descColor;
	D3DSURFACE_DESC	descDepth;
	LPDIRECT3DSURFACE9 pTemp = NULL;
	m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pTemp );
	pTemp->GetDesc( &descColor);
	pTemp->Release();

	m_pd3dDevice->GetDepthStencilSurface(&pTemp);
	pTemp->GetDesc(&descDepth);
	pTemp->Release();

	for( int i=0; i<2; ++i )
	{
		hr	= D3DXCreateTexture(
				m_pd3dDevice
			,	descColor.Width
			,	descColor.Height
			,	1
			,	D3DUSAGE_RENDERTARGET
			,	D3DFMT_A8R8G8B8
			,	D3DPOOL_DEFAULT
			,	&m_pPostTex[i]
			);
		CHECK_FAILED(hr);
		CHECK_FAILED( m_pPostTex[i]->GetSurfaceLevel(0, &m_pPostSuf[i]) );

		hr	= m_pd3dDevice->CreateDepthStencilSurface(
				descDepth.Width
			,	descDepth.Height
			,	descDepth.Format
			,	descDepth.MultiSampleType
			,	descDepth.MultiSampleQuality
			,	TRUE
			,	&m_pPostDep[i]
			,	NULL
			);
		CHECK_FAILED(hr);
	}

	hr	= m_pd3dDevice->CreateVertexBuffer(
			4*sizeof( SimpleVertex2 )
		,	0
		,	SimpleVertex2::SimpleVertex2FVF
		,	D3DPOOL_MANAGED
		,	&m_pPost
		,	NULL
		);
	CHECK_FAILED(hr);

	SimpleVertex2	*pVertices	= NULL;
	m_pPost->Lock( NULL, NULL, (void**)&pVertices, 0 );

	pVertices[0].p	= D3DXVECTOR4( 0.f, (float)descColor.Height, 0.f, 1.f );
	pVertices[1].p	= D3DXVECTOR4( 0.f, 0.f, 0.f, 1.f );
	pVertices[2].p	= D3DXVECTOR4( (float)descColor.Width, 0.f, 0.f, 1.f );
	pVertices[3].p	= D3DXVECTOR4( (float)descColor.Width, (float)descColor.Height, 0.f, 1.f );
	pVertices[0].tu	= 0.f;	pVertices[0].tv	= 1.f;
	pVertices[1].tu	= 0.f;	pVertices[1].tv	= 0.f;
	pVertices[2].tu	= 1.f;	pVertices[2].tv	= 0.f;
	pVertices[3].tu	= 1.f;	pVertices[3].tv	= 1.f;

	m_pPost->Unlock();


	//09-08-20 이재준
	m_pCameraAniModel->RestoreDeviceObjects();

	g_text.Reset();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT SexySpike::InvalidateDeviceObjects()
{
	HRESULT hr = S_OK;

	CHECK_FAILED( hr = g_Effects.InvalidateDeviceObjects() );
    CHECK_FAILED( hr = m_pFont->InvalidateDeviceObjects() );
    CHECK_FAILED( hr = m_pFontSmall->InvalidateDeviceObjects() );
	CHECK_FAILED( hr = m_pBackground->InvalidateDeviceObjects() );
	CHECK_FAILED( hr = m_pNet->InvalidateDeviceObjects() );
	INVALIDATEDEVICEOBJECTS( m_pBall );

	switch( m_eFlow )
	{
		case PLAY:
		{
			for( int i=0; i<NUMPLAYERS; ++i )
			{
				INVALIDATEDEVICEOBJECTS( m_ppPlayers[i] );
			}
			break;
		}
	}

	for( int i=0; i<NUMMODELS; ++i )
	{
		INVALIDATEDEVICEOBJECTS( m_pModels[i] );
	}

	for( int i=0; i<2; ++i )
	{
		SAFE_RELEASE( m_pPostTex[i] );
		SAFE_RELEASE( m_pPostSuf[i] );
		SAFE_RELEASE( m_pPostDep[i] );
	}
	SAFE_RELEASE( m_pPost );


	//09-08-20 이재준
	m_pCameraAniModel->InvalidateDeviceObjects();


	// 09-09-04 이웅재
	SAFE_DELETE( m_NetworkDlg );

	g_text.Lost();

    return hr;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT SexySpike::DeleteDeviceObjects()
{
	//m_netAdapter.ClientStop();

	CHECK_FAILED( g_Effects.DeleteDeviceObjects() );
	CHECK_FAILED( m_pFont->DeleteDeviceObjects() );
    CHECK_FAILED( m_pFontSmall->DeleteDeviceObjects() );
	CHECK_FAILED( m_pBackground->DeleteDeviceObjects() );
	CHECK_FAILED( m_pNet->DeleteDeviceObjects() );
	DELETEDEVICEOBJECTS( m_pBall );


	for( int i=0; i<NUMPLAYERS; ++i )
	{
		SAFE_DELETE( m_ppPlayers[i] );
	}
	g_ppPlayers = NULL;
	m_eFlow		= READYPLAY;

	for( int i=0; i<NUMMODELS; ++i )
	{
		if( m_pModels[i] )
			m_pModels[i]->DeleteDeviceObjects();
	}

	SAFE_RELEASE(m_pSlapTex);
	SAFE_RELEASE(m_pSlap);


	//09-08-07 이재준
	int i=0;for(i=0;i<55;++i){m_pGameTex[i].Destroy();}

	//09-08-07 이재준
	for(i=0;i<8;++i){SAFE_DELETE(	m_pMp[i]	);}

	// Release COM
	CoUninitialize();

	//09-08-12 이재준
	m_CharacterSelect->Destroy();

	//09-08-20이재준
	m_pCameraAniModel->DeleteDeviceObjects();


	SAFE_DELETE( m_StartScreen);
	SAFE_DELETE( m_MultiPlay);
	SAFE_DELETE( m_HowtoPlay);
	SAFE_DELETE( m_CharacterSelect);
	SAFE_DELETE( m_EndGame);
	SAFE_DELETE( m_GameSceneManager);

	g_text.Release();

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT SexySpike::FinalCleanup()
{
	SAFE_DELETE( m_pBackground );
	SAFE_DELETE( m_pNet );
	SAFE_DELETE( m_pBall );
	SAFE_DELETE( m_pCrashManager );
    SAFE_DELETE( m_pFont );
    SAFE_DELETE( m_pFontSmall );
	SAFE_DELETE( m_pCrashManager );

	for( int i=0; i<NUMPLAYERS; ++i )
	{
		SAFE_DELETE( m_ppPlayers[i] );
	}

	for( int i=0; i<NUMMODELS; ++i )
	{
		SAFE_DELETE( m_pModels[i] );
	}


	//09-08-04 이재준
	SAFE_DELETE(	m_pInput	);
	SAFE_DELETE(	m_pCamMn	);

	//09-08-20 이재준
	SAFE_DELETE( m_pCameraAniModel );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT SexySpike::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                      D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL;

    // If device doesn't support vs_1_1 in hardware, switch to 
    // software vertex processing
    if( pCaps->VertexShaderVersion < D3DVS_VERSION(2,0) )
    {
        if( (dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) == 0 )
        {
            return E_FAIL;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT SexySpike::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
						   LPARAM lParam )
{
	//if( uMsg != WM_KEYDOWN || ( wParam != VK_LEFT && wParam != VK_RIGHT && wParam != VK_UP && wParam != VK_DOWN ) )
	//	m_camera.HandleMessages( hWnd, uMsg, wParam, lParam );

	// Handle key presses
	switch( uMsg )
	{
	case WM_PAINT:
		{
			if( m_bLoadingApp )
			{
				// Draw on the window tell the user that the app is loading
				// TODO: change as needed
				HDC hDC = GetDC( hWnd );
				TCHAR strMsg[MAX_PATH];
				wsprintf( strMsg, TEXT("Loading... Please wait") );
				RECT rct;
				GetClientRect( hWnd, &rct );
				DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
				ReleaseDC( hWnd, hDC );
			}
			break;
		}
	case WM_KEYUP:
		break;

	case WM_KEYDOWN:
		break;

	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDM_CHANGEDEVICE:
				for(int i=0;i<8;++i)
				{
					/*GMAIN->m_pMp[i]->Reset();*/
					GMAIN->m_pMp[i]->Stop();
				}
				// Prompt the user to select a new device or mode
				Pause(true);
				UserSelectNewDevice();
				Pause(false);
				return 0;

			case IDM_TOGGLEFULLSCREEN:
				for(int i=0;i<8;++i)
				{
					/*GMAIN->m_pMp[i]->Reset();*/
					GMAIN->m_pMp[i]->Stop();
				}
				// Toggle the fullscreen/window mode
				Pause( true );
				if( FAILED( ToggleFullscreen() ) )
					DisplayErrorMsg( D3DAPPERR_RESETFAILED, MSGERR_APPMUSTEXIT );
				Pause( false );                        
				return 0;

			case IDM_TOGGLEHELP:
				m_bShowHelp = !m_bShowHelp;
				break;
			case IDM_EXIT:

				for(int i=0;i<8;++i)
				{
					/*GMAIN->m_pMp[i]->Reset();*/
					GMAIN->m_pMp[i]->Stop();
				}

				// Recieved key/menu command to exit app
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
				return 0;
			}
		}

	case WM_SIZE:
		// Pick up possible changes to window style due to maximize, etc.
		if( m_bWindowed && m_hWnd != NULL )
			m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );

		if( SIZE_MINIMIZED == wParam )
		{
			for(int i=0;i<8;++i)
			{
				/*GMAIN->m_pMp[i]->Reset();*/
				GMAIN->m_pMp[i]->Stop();
			}

			if( m_bClipCursorWhenFullscreen && !m_bWindowed )
				ClipCursor( NULL );
			Pause( true ); // Pause while we're minimized
			m_bMinimized = true;
			m_bMaximized = false;
		}
		else if( SIZE_MAXIMIZED == wParam )
		{
			for(int i=0;i<8;++i)
			{
				/*	GMAIN->m_pMp[i]->Reset();*/
				GMAIN->m_pMp[i]->Stop();
			}

			if( m_bMinimized )
				Pause( false ); // Unpause since we're no longer minimized
			m_bMinimized = false;
			m_bMaximized = true;
			HandlePossibleSizeChange();
		}
		else if( SIZE_RESTORED == wParam )
		{
			if( m_bMaximized )
			{
				m_bMaximized = false;
				HandlePossibleSizeChange();
			}
			else if( m_bMinimized)
			{
				Pause( false ); // Unpause since we're no longer minimized
				m_bMinimized = false;
				HandlePossibleSizeChange();
			}
			else
			{
				// If we're neither maximized nor minimized, the window size 
				// is changing by the user dragging the window edges.  In this 
				// case, we don't reset the device yet -- we wait until the 
				// user stops dragging, and a WM_EXITSIZEMOVE message comes.
			}
		}
		break;
	case WM_DSHOW_EVENT:
		{
			CMiMp3* pMp3 = (CMiMp3*)lParam;
			pMp3->ProcessEvent();
			break;
		}
	}

	// Pass remaining messages to default handler
	return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}
