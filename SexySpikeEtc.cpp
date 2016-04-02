
#include	"./Include/DEFAULT_INCLUDE.h"
#include	"./Include/INCLUDE.h"

//09-08-26 ������
void SexySpike::SoundPlay(eBehavior BEHAVIOR)
{
	CMiMp3*		pMpPlayer	= NULL;
	switch( BEHAVIOR )
	{
	case SPIKE:
		pMpPlayer	= m_pMp[5];
		break;
	case TOSS:
		pMpPlayer	= m_pMp[4];
		break;
	case RECEIVE:
		pMpPlayer	= m_pMp[3];
		break;
	case SERVE_OVER:
		pMpPlayer	= m_pMp[4];
		break;
	case SERVE_UNDER:
		pMpPlayer	= m_pMp[3];
		break;
	case SLIDING:
		pMpPlayer	= m_pMp[3];
		break;
	}
	if( pMpPlayer )
	{
		pMpPlayer->Reset();
		pMpPlayer->Stop();
		pMpPlayer->Play();
	}
}

void SexySpike::SpriteDraw()
{
	int i=0;

	D3DXMATRIX	mtW;

	D3DXVECTOR3 vcDelta = g_pApp->m_pInput->GetMousePos();

	D3DXMatrixIdentity(&mtW);
	mtW._11 =  float(m_rcWindowClient.right/1024.f);							// x�� ������
	mtW._22 =  float(m_rcWindowClient.bottom/768.f);							// y�� ������
	mtW._41	=  0;	// x ��ġ
	mtW._42	=  0;	// y ��ġ
	m_pd3dSprite->SetTransform(&mtW);

	//������ũ ������ ���
	for(int i=0; i<4; ++i)
	{
		m_rc.right = m_PlayerInfo[i].iSpikeEnergy/2;
		m_pd3dSprite->Draw(m_pGameTex[53].m_pTex, &m_rc, NULL, &m_vcIcon[i].vcPos1, D3DXCOLOR(1,1,1,1.f));
	}

	m_rc.right=1024;

	//���ھ� ���� ���
	m_vcPos=D3DXVECTOR3(264,8,0);
	m_pd3dSprite->Draw(m_pGameTex[36].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

	//ĳ���� ������ ���
	for(int i=0; i<4; ++i)
	{
		m_pd3dSprite->Draw(m_pGameTex[m_PlayerInfo[i].iCharacterNum+13].m_pTex, &m_rc, NULL, &m_vcIcon[i].vcPos2, D3DXCOLOR(1,1,1,1.f));
	}

	if(m_nSetNumber < 6)
	{
		//��Ʈ ī��Ʈ ���
		m_vcPos=D3DXVECTOR3(487,78,0);
		m_pd3dSprite->Draw(m_pGameTex[m_nSetNumber+30].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}

	if(m_nRedTeamScore <10)
	{
		//������ ���ھ� ���
		m_vcPos=D3DXVECTOR3(472,39,0);
		m_pd3dSprite->Draw(m_pGameTex[m_nRedTeamScore+21].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}
	else if(m_nRedTeamScore <20)
	{
		//1
		m_vcPos=D3DXVECTOR3(464,39,0);
		m_pd3dSprite->Draw(m_pGameTex[22].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

		m_vcPos=D3DXVECTOR3(476,39,0);
		m_pd3dSprite->Draw(m_pGameTex[m_nRedTeamScore+11].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}
	else{}

	if(m_nBlueTeamScore <10)
	{
		//����� ���ھ� ���
		m_vcPos=D3DXVECTOR3(539,39,0);
		m_pd3dSprite->Draw(m_pGameTex[m_nBlueTeamScore+21].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}
	else if(m_nBlueTeamScore <20)
	{	
		//1
		m_vcPos=D3DXVECTOR3(531,39,0);
		m_pd3dSprite->Draw(m_pGameTex[22].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

		m_vcPos=D3DXVECTOR3(543,39,0);
		m_pd3dSprite->Draw(m_pGameTex[m_nBlueTeamScore+11].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}
	else{}



	//������ ��Ʈ �¸�Ƚ�� ������ ���
	m_rc.left = 0+20*m_nRedTeamWins; m_rc.right = 20+20*m_nRedTeamWins;
	m_vcPos=D3DXVECTOR3(437,7,0);
	m_pd3dSprite->Draw(m_pGameTex[43].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

	//����� ��Ʈ �¸�Ƚ�� ������ ���
	m_rc.left = 0+20*m_nBlueTeamWins  ; m_rc.right = 20+20*m_nBlueTeamWins;
	m_vcPos=D3DXVECTOR3(565,7,0);
	m_pd3dSprite->Draw(m_pGameTex[43].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

	m_rc.left = 0  ; m_rc.right = 1024;

	//�׸��α� ��ư�� Ŭ���Ͽ��� ���
	INT nCheck = RectCheck(vcDelta,482,540,7,26);
	if(nCheck==1)
	{
		m_bQuitButton=TRUE;
	}
	else if(nCheck==2)
	{
		//���� ��ư BOOL �� TRUE�� ����
		m_vcPos=D3DXVECTOR3(479,5,0);
		m_pd3dSprite->Draw(m_pGameTex[42].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}

	QuitButtonClickCheck();
}

BOOL SexySpike::QuitButtonClickCheck(void)
{
	BOOL	bOk = FALSE;

	D3DXVECTOR3 vcDelta = g_pApp->m_pInput->GetMousePos();

	INT nCheck = 0;

	//Ÿ��Ʋ�� ���ư��ϱ�?
	if(m_bQuitButton == TRUE)
	{
		m_vcPos=D3DXVECTOR3(220,348,0);
		m_pd3dSprite->Draw(m_pGameTex[37].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		m_vcPos=D3DXVECTOR3(320,422,0);
		m_pd3dSprite->Draw(m_pGameTex[40].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

		nCheck = RectCheck(vcDelta,326,505,429,503);

		if(nCheck==1)
		{
			m_bQuitButton = FALSE;
			m_nGamePhase =0;
			bOk = TRUE;

			if(m_nGamePhase == 1)
			{
				if(GMAIN->m_bMultiGame)
				{
					//��Ƽ�÷���:
					//�������� ������ ����
					GMAIN->m_MultiPlay->Client_Room_Exit();
				}
				Initval();			//���� ���� ������ �ʱ�ȭ
			}
		}
		else if(nCheck==2)
		{
			//��: ùȭ������ ����. m_nGamePhase = 0
			m_rc.left = 0  ; m_rc.right = 184;
			m_pd3dSprite->Draw(m_pGameTex[41].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
			m_rc.left = 0  ; m_rc.right = 1024;
		}
		else if(nCheck==0)
		{
			nCheck=RectCheck(vcDelta,518,696,429,503);
			if(nCheck==1)
			{
				m_bQuitButton = FALSE;
			}
			else if(nCheck==2)
			{
				//�ƴϿ�
				m_vcPos.x += 192;
				m_rc.left = 192  ; m_rc.right = 384;
				m_pd3dSprite->Draw(m_pGameTex[41].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
				m_rc.left = 0  ; m_rc.right = 1024;
			}
		}
	}
	

	return bOk;
}

INT SexySpike::RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax)
{
	INT nClicked = 0;

	if(vcDelta.x > GMAIN->m_rcWindowClient.right*nXmin/1024
		&& vcDelta.x < GMAIN->m_rcWindowClient.right*nXmax/1024
		&& vcDelta.y > GMAIN->m_rcWindowClient.bottom*nYmin/768
		&& vcDelta.y < GMAIN->m_rcWindowClient.bottom*nYmax/768)
	{
		nClicked = 2;

		if(g_pApp->m_pInput->BtnDown(0))
		{
			GMAIN->m_pMp[2]->Reset();
			GMAIN->m_pMp[2]->Stop();
			GMAIN->m_pMp[2]->Play();

			nClicked = 1;
		}

	}
	return nClicked;

}

void SexySpike::Initval()
{
	if(g_ppPlayers != NULL)
	{
		GMAIN->m_eFlow=READYPLAY;
		g_ppPlayers	= NULL;
		for( int i=0; i<NUMPLAYERS; ++i )
		{
			SAFE_DELETE( GMAIN->m_ppPlayers[i] );
		}
		//SAFE_DELETE_ARRAY( GMAIN->m_pSpectators );
	}

	for(int i=0; i<4; ++i)
	{
		memset(m_PlayerInfo[i].strName,0,sizeof(m_PlayerInfo[i].strName));
		m_PlayerInfo[i].bHost=FALSE;
		m_PlayerInfo[i].bComputer=TRUE;
		m_PlayerInfo[i].nRedTeam=3;
		m_PlayerInfo[i].iCharacterNum=10;
		m_PlayerInfo[i].iSpikeEnergy=0;
	}

	m_nRedTeamScore=0;							//������ ���ھ�
	m_nBlueTeamScore=0;							//����� ���ھ�
	m_nSetNumber=1;								//���� ��Ʈ

	m_pCameraAniModel->m_pAniController->m_pAniController->SetTrackPosition(0,0);
	m_pCameraAniModel->m_pAniController->m_pAniController->SetTrackPosition(1,0);

	//09-08-27 ������
	m_bQuitButton = FALSE;

	m_nRedTeamWins=0;
	m_nBlueTeamWins=0;

	ZeroMemory(	g_score, sizeof(g_score) );
	ZeroMemory(	g_score, sizeof(g_score) );
}

void	SexySpike::ChangeCoat()
{
	for( int i=0; i<NUMPLAYERS; ++i )
	{
		int			SIDE	= m_ppPlayers[i]->GetSide();
		float		fRot	= SIDE&RIGHT? 1.5f*D3DX_PI : 0.5f*D3DX_PI ;
		D3DXVECTOR3	vPos	= D3DXVECTOR3(0.f, 0.f, 0.f);
		switch( SIDE )
		{
		case LEFT | FRONT :
			m_ppPlayers[i]->m_playInfo.SIDE = (RIGHT|FRONT);
			vPos		= D3DXVECTOR3(   20.f, 0.f,  -70.f );
			break;
		case LEFT | BACK :
			m_ppPlayers[i]->m_playInfo.SIDE = (RIGHT|BACK);
			vPos		= D3DXVECTOR3(   50.f, 0.f, -90.f );
			break;
		case RIGHT | FRONT :
			m_ppPlayers[i]->m_playInfo.SIDE = (LEFT|FRONT);
			vPos		= D3DXVECTOR3(  -20.f, 0.f,  -70.f );
			break;
		case RIGHT | BACK :
			m_ppPlayers[i]->m_playInfo.SIDE = (LEFT|BACK);
			vPos		= D3DXVECTOR3(  -50.f, 0.f, -90.f );
			break;
		}
		m_ppPlayers[i]->SetRotation( fRot );
		m_ppPlayers[i]->SetPosition( vPos.x, vPos.y, vPos.z );
		m_ppPlayers[i]->SetAttackingPos( vPos );

		// ������
		if( m_ppPlayers[i]->GetPLAYSTATE() & SERVE )
		{
			m_pBall->SetSideBefore( SIDE&LEFT? LEFT: RIGHT );
			m_pBall->SetSideCurrent( m_pBall->GetSideBefore() );
			m_pBall->SetShowFallingPoint( FALSE );
			m_pBall->SetPosition( 0.f, 10000.f, 0.f );
			m_pBall->SetVelocity( D3DXVECTOR3(0.f, 10000.f, 0.f) );
		}

		if( m_ppPlayers[i]->GetSelfUpdate() )
		{
			D3DXVECTOR3	vPos	= m_ppPlayers[i]->GetPosition();
		}
	}

	g_CntBallTouched	= -1;

	if(m_bMultiGame)
	{
		INT MyPlayerNumber = m_GameSceneManager->MyTeamAndNumberCheck(GMAIN->m_MultiPlay->m_MyInfo.RoomPosition);

		//��Ʈ �ű��� 0���÷��̾ ���� ���� ��ȯ
		int SIDE	= m_ppPlayers[MyPlayerNumber]->GetSide();
		BOOL	bSIDE	= SIDE&LEFT? TRUE : FALSE ;
		if(bSIDE)
		{m_pCamMn->m_pCam0->SetParamView(D3DXVECTOR3(-100.f,80.f,-200.f), D3DXVECTOR3( 50.f, 0.f, -50.f), D3DXVECTOR3(0,1,0));}
		else
		{m_pCamMn->m_pCam0->SetParamView(D3DXVECTOR3(100.f,80.f,0.f), D3DXVECTOR3( -50.f, 0.f, -150.f), D3DXVECTOR3(0,1,0));}
	}
	else
	{
		//��Ʈ �ű��� 0���÷��̾ ���� ���� ��ȯ
		int SIDE	= m_ppPlayers[0]->GetSide();
		BOOL	bSIDE	= SIDE&LEFT? TRUE : FALSE ;
		if(bSIDE)
		{m_pCamMn->m_pCam0->SetParamView(D3DXVECTOR3(-100.f,80.f,-200.f), D3DXVECTOR3( 50.f, 0.f, -50.f), D3DXVECTOR3(0,1,0));}
		else
		{m_pCamMn->m_pCam0->SetParamView(D3DXVECTOR3(100.f,80.f,0.f), D3DXVECTOR3( -50.f, 0.f, -150.f), D3DXVECTOR3(0,1,0));}
	}



	//��Ʈ�� �ٲپ����Ƿ� VallyActionPoint�� ���
	//AlignCoat();
	m_ppPlayers[0]->ComputeValleyActionPoint( m_ppPlayers[0]->GetRotationAngle() );
	m_ppPlayers[1]->ComputeValleyActionPoint( m_ppPlayers[1]->GetRotationAngle() );
	m_ppPlayers[2]->ComputeValleyActionPoint( m_ppPlayers[2]->GetRotationAngle() );
	m_ppPlayers[3]->ComputeValleyActionPoint( m_ppPlayers[3]->GetRotationAngle() );

	g_bNowServe	= TRUE;
}

INT	SexySpike::TextureLoad()
{
	HRESULT hr=0;

	m_pGameTex[0].Create(m_pd3dDevice,"./Texture/title.png",0x0);                //����ȭ��
	m_pGameTex[1].Create(m_pd3dDevice,"./Texture/singleplayicon.png",0x0);
	m_pGameTex[2].Create(m_pd3dDevice,"./Texture/multiplayicon.png",0x0);
	m_pGameTex[3].Create(m_pd3dDevice,"./Texture/howtoplayicon.png",0x0);
	m_pGameTex[4].Create(m_pd3dDevice,"./Texture/endgameicon.png",0x0);
	m_pGameTex[5].Create(m_pd3dDevice,"./Texture/characterselect.png",0x0);			//ĳ���� ����ȭ��
	m_pGameTex[6].Create(m_pd3dDevice,"./Texture/red_player.png",0x0);				//������ Player
	m_pGameTex[7].Create(m_pd3dDevice,"./Texture/red_com.png",0x0);					//������ Com
	m_pGameTex[8].Create(m_pd3dDevice,"./Texture/blue_player.png",0x0);				//����� player
	m_pGameTex[9].Create(m_pd3dDevice,"./Texture/blue_com.png",0x0);				//����� Com
	m_pGameTex[10].Create(m_pd3dDevice,"./Texture/ok_ready.png",0x0);				//ok��ư ready�Ǿ����� : ĳ���ͼ���ȭ��
	m_pGameTex[11].Create(m_pd3dDevice,"./Texture/ok_on.png",0x0);					//ok��ư�� ���콺�� ���� �� :ĳ���� ����ȭ��
	m_pGameTex[12].Create(m_pd3dDevice,"./Texture/quit_on.png",0x0);				//�׸��α��ư

	m_pGameTex[13].Create(m_pd3dDevice,"./Texture/icons/1_manami.png",0x0);
	m_pGameTex[14].Create(m_pd3dDevice,"./Texture/icons/2_esk.png",0x0);
	m_pGameTex[15].Create(m_pd3dDevice,"./Texture/icons/3_eo.png",0x0);
	m_pGameTex[16].Create(m_pd3dDevice,"./Texture/icons/4_reiko.png",0x0);
	m_pGameTex[17].Create(m_pd3dDevice,"./Texture/icons/5_maria.png",0x0);
	m_pGameTex[18].Create(m_pd3dDevice,"./Texture/icons/6_maya.png",0x0);
	m_pGameTex[19].Create(m_pd3dDevice,"./Texture/icons/7_bael.png",0x0);
	m_pGameTex[20].Create(m_pd3dDevice,"./Texture/icons/8_fei.png",0x0);

	m_pGameTex[21].Create(m_pd3dDevice,"./Texture/icons/num_0.png",0x0);
	m_pGameTex[22].Create(m_pd3dDevice,"./Texture/icons/num_1.png",0x0);
	m_pGameTex[23].Create(m_pd3dDevice,"./Texture/icons/num_2.png",0x0);
	m_pGameTex[24].Create(m_pd3dDevice,"./Texture/icons/num_3.png",0x0);
	m_pGameTex[25].Create(m_pd3dDevice,"./Texture/icons/num_4.png",0x0);
	m_pGameTex[26].Create(m_pd3dDevice,"./Texture/icons/num_5.png",0x0);
	m_pGameTex[27].Create(m_pd3dDevice,"./Texture/icons/num_6.png",0x0);
	m_pGameTex[28].Create(m_pd3dDevice,"./Texture/icons/num_7.png",0x0);
	m_pGameTex[29].Create(m_pd3dDevice,"./Texture/icons/num_8.png",0x0);
	m_pGameTex[30].Create(m_pd3dDevice,"./Texture/icons/num_9.png",0x0);

	m_pGameTex[31].Create(m_pd3dDevice,"./Texture/icons/set_1.png",0x0);
	m_pGameTex[32].Create(m_pd3dDevice,"./Texture/icons/set_2.png",0x0);
	m_pGameTex[33].Create(m_pd3dDevice,"./Texture/icons/set_3.png",0x0);
	m_pGameTex[34].Create(m_pd3dDevice,"./Texture/icons/set_4.png",0x0);
	m_pGameTex[35].Create(m_pd3dDevice,"./Texture/icons/set_5.png",0x0);
	m_pGameTex[36].Create(m_pd3dDevice,"./Texture/scoreboard.png",0x0);

	m_pGameTex[37].Create(m_pd3dDevice,"./Texture/q_backtothetitle.tga",0x0);
	m_pGameTex[38].Create(m_pd3dDevice,"./Texture/q_ok.tga",0x0);
	m_pGameTex[39].Create(m_pd3dDevice,"./Texture/q_quitgame.tga",0x0);
	m_pGameTex[40].Create(m_pd3dDevice,"./Texture/q_yesorno1.tga",0x0);
	m_pGameTex[41].Create(m_pd3dDevice,"./Texture/q_yesorno2.tga",0x0);

	m_pGameTex[42].Create(m_pd3dDevice,"./Texture/quit_on2.png",0x0);
	m_pGameTex[43].Create(m_pd3dDevice,"./Texture/setwins.png",0x0);					//�� ��Ʈ�� �̰���� ���ھ�� ���� ǥ��

	m_pGameTex[44].Create(m_pd3dDevice,"./Texture/_GetReady.png",0x0);					//���� ���� �� Get Ready
	m_pGameTex[45].Create(m_pd3dDevice,"./Texture/_CourtChange.png",0x0);				//��Ʈ �ٲ� ��
	m_pGameTex[46].Create(m_pd3dDevice,"./Texture/_RedTeamWins.png",0x0);				//�������� �̰��� ��
	m_pGameTex[47].Create(m_pd3dDevice,"./Texture/_BlueTeamwins.png",0x0);				//������� �̰��� ��

	m_pGameTex[48].Create(m_pd3dDevice,"./Texture/_ScoreSet.png",0x0);					//ĳ���� �����ϰ� ���ھ� �����Ҷ�
	m_pGameTex[49].Create(m_pd3dDevice,"./Texture/red_blue.png",0x0);					//��Ƽ�÷��̽ÿ� �� ���� ��ư
	m_pGameTex[50].Create(m_pd3dDevice,"./Texture/red_blue_on1.png",0x0);				//������ ���ý� �ѿ��� �̹��� 
	m_pGameTex[51].Create(m_pd3dDevice,"./Texture/red_blue_on2.png",0x0);				//����� ���ý� �ѿ��� �̹���

	m_pGameTex[52].Create(m_pd3dDevice,"./Texture/multiplaystatus.png",0x0);			//��Ƽ�÷��� ���� ǥ��
	m_pGameTex[53].Create(m_pd3dDevice,"./Texture/guage.tga",0x0);						//������	ǥ��

	m_pGameTex[54].Create(m_pd3dDevice,"./Texture/HowToPlay.jpg",0x0);					//HowToPlay �޴��� ���� ������ �Ǵ� �̹���

	return hr;
}

INT SexySpike::InitSoundObjects()
{
	//We need Initialize COM(must be defined _WIN32_WINNT	over 0x400)
	if(FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
		return -1;

	//m_pMp  0:BGM, 1:�ĵ��Ҹ�, 2~5:ȿ����
	m_pMp[1] = new CMiMp3;
	m_pMp[1]->Create("Sound/bgm/title_wave.mp3");
	m_pMp[1]->m_bRepeat=TRUE;

	m_pMp[2] = new CMiMp3;
	m_pMp[2]->Create("Sound/select.wav");
	m_pMp[2]->SetVolume(9000L);
	m_pMp[2]->m_bRepeat=FALSE;

	m_pMp[3] = new CMiMp3;
	m_pMp[3]->Create("Sound/receive.wav");
	m_pMp[3]->SetVolume(9000L);
	m_pMp[3]->m_bRepeat=FALSE;

	m_pMp[4] = new CMiMp3;
	m_pMp[4]->Create("Sound/toss.wav");
	m_pMp[4]->SetVolume(9000L);
	m_pMp[4]->m_bRepeat=FALSE;

	m_pMp[5] = new CMiMp3;
	m_pMp[5]->Create("Sound/spike.wav");
	m_pMp[5]->SetVolume(9000L);
	m_pMp[5]->m_bRepeat=FALSE;

	m_pMp[6] = new CMiMp3;
	m_pMp[6]->Create("Sound/blocking.wav");
	m_pMp[6]->SetVolume(9000L);
	m_pMp[6]->m_bRepeat=FALSE;

	m_pMp[7] = new CMiMp3;
	m_pMp[7]->Create("Sound/clap.wav");
	m_pMp[7]->SetVolume(9000L);
	m_pMp[7]->m_bRepeat=FALSE;

	return S_OK;
}

void SexySpike::BGMPlay()
{
	m_pMp[1]->Play();

	//������� �������� �÷���
	if(m_pMp[0]==NULL)
	{
		m_pMp[0] = new CMiMp3;

		srand((int)time(NULL));

		switch(rand()%5)
		{
		case 0:
			m_pMp[0]->Create("Sound/bgm/doax_bgm_00.mp3");
			m_pMp[0]->m_bRepeat=FALSE;
			m_pMp[0]->SetVolume(9000L);
			break;
		case 1:
			m_pMp[0]->Create("Sound/bgm/doax_bgm_01.mp3");
			m_pMp[0]->m_bRepeat=FALSE;
			m_pMp[0]->SetVolume(9000L);
			break;
		case 2:
			m_pMp[0]->Create("Sound/bgm/doax_bgm_03.mp3");
			m_pMp[0]->m_bRepeat=FALSE;
			m_pMp[0]->SetVolume(9000L);
			break;
		case 3:
			m_pMp[0]->Create("Sound/bgm/doax_bgm_05.mp3");
			m_pMp[0]->m_bRepeat=FALSE;
			m_pMp[0]->SetVolume(9000L);
			break;
		case 4:
			m_pMp[0]->Create("Sound/bgm/doax_bgm_08.mp3");
			m_pMp[0]->m_bRepeat=FALSE;
			m_pMp[0]->SetVolume(9000L);
			break;
		default:
			m_pMp[0]->m_bRepeat=FALSE;
			m_pMp[0]->m_bPlayState=FALSE;
			break;
		}	
	}
	else if(m_pMp[0]->m_bPlayState==FALSE&&m_pMp[0] != NULL)
	{
		SAFE_DELETE(m_pMp[0]);
	}
	else{m_pMp[0]->Play();m_pMp[0]->m_bPlayState=TRUE;}
}

void SexySpike::CharacterSet()
{
	if(m_bMultiGame) //��Ƽ�÷��� �϶�
	{
		GMAIN->m_GameSceneManager->DefineTeamMember();

		m_GameSceneManager->m_nGameFinishSet=m_MultiPlay->m_RoomInfo.nGameScore;
		m_GameSceneManager->m_nSetFinishScore=m_MultiPlay->m_RoomInfo.nSetScore;

		if(m_MultiPlay->m_MyInfo.RoomPosition ==0)
			m_PlayerInfo[m_MultiPlay->m_MyInfo.RoomPosition].bHost = TRUE;

		//��Ʈ��ũ�� m_RoomInfo�ν��Ͻ��� ���� m_PlayerInfo�ν��Ͻ��� �����͸� ������Ʈ
		//������
		for(int i=0; i<2; ++i)
		{
			memcpy(m_PlayerInfo[i].strName,m_MultiPlay->m_RoomInfo.sCharacterName[m_GameSceneManager->m_nRedMember[i]],sizeof(m_PlayerInfo[i].strName));
			m_PlayerInfo[i].nRedTeam = m_MultiPlay->m_RoomInfo.nRedTeam[m_GameSceneManager->m_nRedMember[i]];
			m_PlayerInfo[i].iCharacterNum = m_MultiPlay->m_RoomInfo.nCharIndex[m_GameSceneManager->m_nRedMember[i]];
			m_PlayerInfo[i].bComputer = m_MultiPlay->m_RoomInfo.nComputer[m_GameSceneManager->m_nRedMember[i]];
		}
		//�����
		for(int i=0; i<2; ++i)
		{
			memcpy(m_PlayerInfo[i+2].strName,m_MultiPlay->m_RoomInfo.sCharacterName[m_GameSceneManager->m_nBlueMember[i]],sizeof(m_PlayerInfo[i+2].strName));
			m_PlayerInfo[i+2].nRedTeam = m_MultiPlay->m_RoomInfo.nRedTeam[m_GameSceneManager->m_nBlueMember[i]];
			m_PlayerInfo[i+2].iCharacterNum = m_MultiPlay->m_RoomInfo.nCharIndex[m_GameSceneManager->m_nBlueMember[i]];
			m_PlayerInfo[i+2].bComputer = m_MultiPlay->m_RoomInfo.nComputer[m_GameSceneManager->m_nBlueMember[i]];
		}

		INT MyPlayerNumber = m_GameSceneManager->MyTeamAndNumberCheck(GMAIN->m_MultiPlay->m_MyInfo.RoomPosition);

		//������
		if(MyPlayerNumber==0)
		{m_ppPlayers[0]	= CUser::Create( *m_pModels[m_PlayerInfo[0].iCharacterNum], 1, "1P" );}
		else
		{m_ppPlayers[0]	= CNpc::Create( *m_pModels[m_PlayerInfo[0].iCharacterNum], 1, "1P" );}
		
		if(MyPlayerNumber==1)
		{m_ppPlayers[1]	= CUser::Create(  *m_pModels[m_PlayerInfo[1].iCharacterNum], 2, "2P" );}
		else
		{m_ppPlayers[1]	= CNpc::Create(  *m_pModels[m_PlayerInfo[1].iCharacterNum], 2, "2P" );}
		
		//�����
		if(MyPlayerNumber==2)
		{m_ppPlayers[2]	= CUser::Create(  *m_pModels[m_PlayerInfo[2].iCharacterNum], 3, "3P" );}
		else
		{m_ppPlayers[2]	= CNpc::Create(  *m_pModels[m_PlayerInfo[2].iCharacterNum], 3, "3P" );}
		
		if(MyPlayerNumber==3)
		{m_ppPlayers[3]	= CUser::Create(  *m_pModels[m_PlayerInfo[3].iCharacterNum], 4, "4P" );}
		else
		{m_ppPlayers[3]	= CNpc::Create(  *m_pModels[m_PlayerInfo[3].iCharacterNum], 4, "4P" );}
		
		//������
		m_ppPlayers[0]->SetPLAYSTATE( SERVE );
		m_ppPlayers[0]->SetSide( LEFT | BACK );
		m_ppPlayers[0]->SetTeam( RED );
		m_ppPlayers[1]->SetPLAYSTATE( OFFENSE );
		m_ppPlayers[1]->SetSide( LEFT | FRONT );
		m_ppPlayers[1]->SetTeam( RED );

		//�����
		m_ppPlayers[2]->SetPLAYSTATE( DEFENSE );
		m_ppPlayers[2]->SetSide( RIGHT | BACK );
		m_ppPlayers[2]->SetTeam( BLUE );
		m_ppPlayers[3]->SetPLAYSTATE( DEFENSE );
		m_ppPlayers[3]->SetSide( RIGHT | FRONT );
		m_ppPlayers[3]->SetTeam( BLUE);

		//��Ʈ �ű��� �� �÷��̾� ��ȣ�� ���� ���� ��ȯ
		int SIDE	= m_ppPlayers[MyPlayerNumber]->GetSide();
		BOOL	bSIDE	= SIDE&LEFT? TRUE : FALSE ;
		if(bSIDE)
		{m_pCamMn->m_pCam0->SetParamView(D3DXVECTOR3(-100.f,80.f,-200.f), D3DXVECTOR3( 50.f, 0.f, -50.f), D3DXVECTOR3(0,1,0));}
		else
		{m_pCamMn->m_pCam0->SetParamView(D3DXVECTOR3(100.f,80.f,0.f), D3DXVECTOR3( -50.f, 0.f, -150.f), D3DXVECTOR3(0,1,0));}

		PlayerSelfUpdateSetting();	//����������Ʈ ����
	}
	else //�̱��÷��� �϶�
	{
		if(m_bTest)
		{
			m_ppPlayers[0]	= CUser::Create( *m_pModels[0], 1, "1P" );
			m_ppPlayers[0]->SetPLAYSTATE( SERVE );
			m_ppPlayers[0]->SetSide( LEFT | BACK );
			m_ppPlayers[0]->SetTeam( RED );
			m_ppPlayers[1]	= CNpc::Create(  *m_pModels[1], 2, "2P" );
			m_ppPlayers[1]->SetPLAYSTATE( OFFENSE );
			m_ppPlayers[1]->SetSide( LEFT | FRONT );
			m_ppPlayers[1]->SetTeam( RED );
			m_ppPlayers[2]	= CNpc::Create(  *m_pModels[2], 3, "3P" );
			m_ppPlayers[2]->SetPLAYSTATE( DEFENSE );
			m_ppPlayers[2]->SetSide( RIGHT | BACK );
			m_ppPlayers[2]->SetTeam( BLUE );
			m_ppPlayers[3]	= CNpc::Create(  *m_pModels[3], 4, "4P" );
			m_ppPlayers[3]->SetPLAYSTATE( DEFENSE );
			m_ppPlayers[3]->SetSide( RIGHT | FRONT );
			m_ppPlayers[3]->SetTeam( BLUE);
		}
		else
		{
			m_ppPlayers[0]	= CUser::Create( *m_pModels[m_PlayerInfo[0].iCharacterNum], 1, "1P" );
			m_ppPlayers[0]->SetPLAYSTATE( SERVE );
			m_ppPlayers[0]->SetSide( LEFT | BACK );
			m_ppPlayers[0]->SetTeam( RED );
			m_ppPlayers[1]	= CNpc::Create(  *m_pModels[m_PlayerInfo[1].iCharacterNum], 2, "2P" );
			m_ppPlayers[1]->SetPLAYSTATE( OFFENSE );
			m_ppPlayers[1]->SetSide( LEFT | FRONT );
			m_ppPlayers[1]->SetTeam( RED );
			m_ppPlayers[2]	= CNpc::Create(  *m_pModels[m_PlayerInfo[2].iCharacterNum], 3, "3P" );
			m_ppPlayers[2]->SetPLAYSTATE( DEFENSE );
			m_ppPlayers[2]->SetSide( RIGHT | BACK );
			m_ppPlayers[2]->SetTeam( BLUE );
			m_ppPlayers[3]	= CNpc::Create(  *m_pModels[m_PlayerInfo[3].iCharacterNum], 4, "4P" );
			m_ppPlayers[3]->SetPLAYSTATE( DEFENSE );
			m_ppPlayers[3]->SetSide( RIGHT | FRONT );
			m_ppPlayers[3]->SetTeam( BLUE);

			//for( int i=0; i<NUMPLAYERS; ++i )
			//{
			//	switch( m_PlayerInfo[i].iCharacterNum )
			//	{
			//	case MANAMI:
			//	case MAYA:
			//		{
			//			m_ppPlayers[i]->SetSpecialAttack( PHANTOM );
			//		}
			//		break;
			//	case ESK:
			//	case FEI:
			//		{
			//			m_ppPlayers[i]->SetSpecialAttack( BLACKHOLE );
			//		}
			//		break;
			//	default:
			//		m_ppPlayers[i]->SetSpecialAttack( ILLUSION );

			//	}
			//}
			m_ppPlayers[0]->SetSpecialAttack( ILLUSION );
			m_ppPlayers[1]->SetSpecialAttack( COMMON );
			m_ppPlayers[2]->SetSpecialAttack( BLACKHOLE );
			m_ppPlayers[3]->SetSpecialAttack( PHANTOM );
		}
	}
}

void SexySpike::MultiPlayIDTextDraw()
{
	g_text.Begin();

	if(m_bMultiGame)
	{
		if(g_NetworkState ==CLIENT_DISCONNECT)
		{
			g_text.Draw("���� ����: �������",5,340);
		}
	}

	//������
	for(int i=0; i<2; ++i)
	{
		if(m_PlayerInfo[i].bComputer==TRUE)
		{
			g_text.Draw("COM",m_rcWindowClient.right*280/1024+m_rcWindowClient.right*83/1024*i,m_rcWindowClient.bottom*100/768,D3DXCOLOR(1,1,0,1));
		}
		else if(m_PlayerInfo[i].bComputer==FALSE)
		{
			g_text.Draw(m_PlayerInfo[i].strName,m_rcWindowClient.right*280/1024+m_rcWindowClient.right*83/1024*i,m_rcWindowClient.bottom*100/768,D3DXCOLOR(1,1,0,1));
		}
	}

	//�����
	for(int i=0; i<2; ++i)
	{
		if(m_PlayerInfo[i+2].bComputer==TRUE)
		{
			g_text.Draw("COM",m_rcWindowClient.right*610/1024+m_rcWindowClient.right*83/1024*i,m_rcWindowClient.bottom*100/768,D3DXCOLOR(1,1,0,1));
		}
		else if(m_PlayerInfo[i+2].bComputer==FALSE)
		{
			g_text.Draw(m_PlayerInfo[i+2].strName,m_rcWindowClient.right*610/1024+m_rcWindowClient.right*83/1024*i,m_rcWindowClient.bottom*100/768,D3DXCOLOR(1,1,0,1));
		}
	}

	g_text.End();
}

void SexySpike::PlayerSelfUpdateSetting()
{
	//������� ������ ����
	//���� Ʈ��
	//�츮�� �÷��̾ �����ϰ�� ���� NPC�ϰ�� Ʈ��

	INT MyPlayerNumber = m_GameSceneManager->MyTeamAndNumberCheck(GMAIN->m_MultiPlay->m_MyInfo.RoomPosition);
	
	//��� ���� ��
	if(MyPlayerNumber >1)
	{
		if(MyPlayerNumber == 2)
		{
			m_ppPlayers[2]->SetSelfUpdate(TRUE);
			if(m_PlayerInfo[3].bComputer)
			{
				m_ppPlayers[3]->SetSelfUpdate(TRUE);
			}
			else
			{
				m_ppPlayers[3]->SetSelfUpdate(FALSE);
			}
		}
		else if(MyPlayerNumber == 3)
		{
			m_ppPlayers[3]->SetSelfUpdate(TRUE);
			if(m_PlayerInfo[2].bComputer)
			{
				m_ppPlayers[2]->SetSelfUpdate(TRUE);
			}
			else
			{
				m_ppPlayers[2]->SetSelfUpdate(FALSE);
			}
		}
		m_ppPlayers[0]->SetSelfUpdate(FALSE);
		m_ppPlayers[1]->SetSelfUpdate(FALSE);
	}
	else if(MyPlayerNumber <2) //���� ���� ��
	{
		if(MyPlayerNumber == 0)
		{
			m_ppPlayers[0]->SetSelfUpdate(TRUE);
			if(m_PlayerInfo[1].bComputer)
			{
				m_ppPlayers[1]->SetSelfUpdate(TRUE);
			}
			else
			{
				m_ppPlayers[1]->SetSelfUpdate(FALSE);
			}
		}
		else if(MyPlayerNumber == 1)
		{
			m_ppPlayers[1]->SetSelfUpdate(TRUE);
			if(m_PlayerInfo[0].bComputer)
			{
				m_ppPlayers[0]->SetSelfUpdate(TRUE);
			}
			else
			{
				m_ppPlayers[0]->SetSelfUpdate(FALSE);
			}
		}
		m_ppPlayers[2]->SetSelfUpdate(FALSE);
		m_ppPlayers[3]->SetSelfUpdate(FALSE);
	}
}

//09-09-07 ������
void	Send( const char* pName, char DEFINITION,  char SIZE, void* pData, char EXETYPE, char ORDER )
{
	//if( C_TRANSLATION == DEFINITION )
	//{
	//	++g_cntNetWork;
	//	if( g_cntNetWork < 3 )
	//		return;
	//	g_cntNetWork = 0;
	//}
	ZeroMemory( g_packet, PACKET_TOTAL*sizeof(BYTE) );
	//	g_pNetAdapter->PacketAssemble( g_packet, NULL, EXETYPE, pName, ORDER, DEFINITION, SIZE, pData);





	//09-09-09 ������ �Ʒ� ���� �ּ� ó�� �Ͽ��� -> ���� �߻�!
	//g_pNetAdapter->PacketEnCrypt(g_packet,g_packet[0]);
	//EnterCriticalSection( &cs_Send );
	//SendBuffer.EnRingBuffer( g_packet, g_packet[0] );
	//LeaveCriticalSection( &cs_Send );
}
