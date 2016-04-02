#include "../Include/INCLUDE.h"

CGameSceneManager::CGameSceneManager(void)
{
	this->InitVal();
}

void CGameSceneManager::InitVal()
{
	SetRect(&m_rc,0,0,1024,768);
	m_vcPos= D3DXVECTOR3(0,0,0);

	m_nRedOldScore = 0;			
	m_nRedCurScore = 0;			
	m_nBlueOldScore = 0;			
	m_nBlueCurScore = 0;			

	m_nOldSetNum = 1;				
	m_nCurSetNum = 1;				

	m_bGameFinished = FALSE;			
	m_bRedTeamWins = FALSE;			

	m_bEventSceneFinished = FALSE;

	m_dTime			= 0;
	m_dTimeEnd		= 0;
	m_bTimeInit	= FALSE;

	m_bGameStart	= TRUE;

	m_bAniSetChanged = FALSE;

	m_nFocusCharacterNum = 0;

	for(int i=0; i<4; ++i)
	{
		m_strAnimationSetName[i] = AniSetNameRandom(rand()%9);
	}

	m_nSetFinishScore = 3;
	m_nGameFinishSet = 2;

	m_nRedMemberTotal=0;
	m_nBlueMemberTotal=0;
	m_nComMemberTotal=0;


	for(int i=0; i<2; ++i){m_nRedMember[i] = 4;}
	for(int i=0; i<2; ++i){m_nBlueMember[i] = 4;}
	for(int i=0; i<2; ++i){m_nRedComputer[i] = 4;}
	for(int i=0; i<2; ++i){m_nBlueComputer[i] = 4;}
}

CGameSceneManager::~CGameSceneManager(void)
{
}



//����
INT CGameSceneManager::FrameMove()
{
	/////////////////////////////////////////////////////////////////////////////
	//09-08-04 ������
	if(m_bGameFinished){SAFE_FRMOV(	GMAIN->m_pInput	);}
	SAFE_FRMOV(	GMAIN->m_pCamMn	);
	/////////////////////////////////////////////////////////////////////////////
	FRAMEMOVE(	GMAIN->m_pBackground, GMAIN->m_fElapsedTime);
	FRAMEMOVE( GMAIN->m_pNet, GMAIN->m_fElapsedTime );

	
	for(int i=0; i<4; ++i)
	{
		if(GMAIN->m_ppPlayers[i] != NULL)
		{
			if(GMAIN->m_ppPlayers[i]->m_playInfo.SIDE == (LEFT|FRONT) || GMAIN->m_ppPlayers[i]->m_playInfo.SIDE == (LEFT|BACK))
			{
				D3DXMatrixIdentity(&GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matR);
				GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_fRotationAngle=D3DXToRadian(-90.f);
			}
			else if(GMAIN->m_ppPlayers[i]->m_playInfo.SIDE == (RIGHT|FRONT) || GMAIN->m_ppPlayers[i]->m_playInfo.SIDE == (RIGHT|BACK) )
			{
				D3DXMatrixIdentity(&GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matR);
				GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_fRotationAngle=D3DXToRadian(90.f);
			}
			GMAIN->m_CharacterSelect->CharacterAnimationPlayTypeChange(GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]);

			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matT._41=GMAIN->m_ppPlayers[i]->m_matT._41;
			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matT._42=GMAIN->m_ppPlayers[i]->m_matT._42;
			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matT._43=GMAIN->m_ppPlayers[i]->m_matT._43;
			D3DXMatrixScaling(	&GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matS, 1.f, 1.f, 1.f);

			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->FrameMove(GMAIN->m_fElapsedTime);
		}
	}

	m_nRedCurScore = GMAIN->m_nRedTeamScore;
	m_nBlueCurScore = GMAIN->m_nBlueTeamScore;


	if(m_bGameStart)			//������ ���۵Ǿ����� �ش� ������ ó��
	{GameStart();}
	else if(m_bGameFinished) //������ ������ ó���� �� ����
	{GameFinished();}

	if(m_nCurSetNum != m_nOldSetNum && m_bGameFinished == FALSE)
	{
		SetChange();
	}
	else if(m_nGameFinishSet+1 != GMAIN->m_nRedTeamWins || m_nGameFinishSet+1 != GMAIN->m_nBlueTeamWins)
	{
		//������ �Ǿ����� �ش� ������ ó��
		if(m_nRedOldScore != m_nRedCurScore)
		{GotPoint(FALSE);}
		else if( m_nBlueOldScore != m_nBlueCurScore)
		{GotPoint(TRUE);}

		if(m_nGameFinishSet == GMAIN->m_nRedTeamWins ||m_nGameFinishSet == GMAIN->m_nBlueTeamWins)
		{
			m_bGameFinished = TRUE;
		}
	}
	return 0;
}

INT CGameSceneManager::GameStart()
{
	m_bEventSceneFinished = FALSE;

	m_dTime	= timeGetTime();

	//������ �ð� �ʱ�ȭ
	if(m_bTimeInit == FALSE)
	{
		m_dTimeEnd = m_dTime+4500;
		m_bTimeInit = TRUE;

		//�ڼ��Ҹ�
		GMAIN->m_pMp[7]->Reset();
		GMAIN->m_pMp[7]->Stop();
		GMAIN->m_pMp[7]->Play();
	}

	//���� ������ �ʱ�ȭ& ����
	if((m_dTime - m_dTimeEnd)  > 0)
	{
		m_bGameStart = FALSE;

		m_bEventSceneFinished = TRUE;
		m_dTime	 = 0;
		m_dTimeEnd = 0;
		m_bTimeInit = FALSE;
	}

	//ó�� �� ����

	/////////////////////////////////////////////////////////////////////////
	//09-08-04 ������
	GMAIN->m_pCamMn->SpecificCameraON(2);
	GMAIN->m_pCamMn->m_pCam2->SetTransformViw();
	GMAIN->m_pCamMn->m_pCam2->SetTransformPrj();
	g_matViewProj	= GMAIN->m_pCamMn->m_pCam2->GetMatrixViw()*GMAIN->m_pCamMn->m_pCam2->GetMatrixPrj();
	/////////////////////////////////////////////////////////////////////////


	return 0;
}

//������
HRESULT CGameSceneManager::Render()
{
	HRESULT hr = S_OK;
	/////////////////////////////////////////////////////////////////////////////
	CHECK_FAILED(GMAIN->m_pBackground->Render(GMAIN->m_pd3dDevice));
	CHECK_FAILED(GMAIN->m_pNet->Render(GMAIN->m_pd3dDevice));
	//CHECK_FAILED(GMAIN->m_pBall->Render(GMAIN->m_pd3dDevice));
	for(int i=0; i<4; ++i)
	{
		GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->Render(GMAIN->m_pd3dDevice);
	}
	/////////////////////////////////////////////////////////////////////////////
	SpriteDraw();
	return hr;
}


//���� ���ھ�� ���� ���ھ �ٸ��� �Ǿ��ٸ� ó�� �� ����
HRESULT CGameSceneManager::GotPoint(BOOL bBlueTeam)
{
	HRESULT hr = S_OK;

	m_bEventSceneFinished = FALSE;

	m_dTime	= timeGetTime();

	//������ �ð� �ʱ�ȭ
	if(m_bTimeInit == FALSE)
	{
		m_dTimeEnd = m_dTime+3000;
		m_bTimeInit = TRUE;

		//�ڼ� �Ҹ�
		GMAIN->m_pMp[7]->Reset();
		GMAIN->m_pMp[7]->Stop();
		GMAIN->m_pMp[7]->Play();
	}

	//ó�� �� ����
	if(m_bAniSetChanged == FALSE)
	{
		srand((int)time(NULL));

		if(bBlueTeam){m_nFocusCharacterNum = rand()%2+2;}
		else{m_nFocusCharacterNum = rand()%2;}
		
		for(int i=0; i<4; ++i)
		{
			if(i==m_nFocusCharacterNum)
			{
				AnimationInit(AniSetNameRandom(rand()%9),i);
				continue;
			}
			AnimationInit("WAITING",i);
		}

		m_bAniSetChanged  = TRUE;
	}
	else
	{
		GMAIN->m_pCamMn->FocusCharacterNum(m_nFocusCharacterNum);
		/////////////////////////////////////////////////////////////////////////
		//09-08-04 ������
		GMAIN->m_pCamMn->SpecificCameraON(3);
		GMAIN->m_pCamMn->m_pCam3->SetTransformViw();
		GMAIN->m_pCamMn->m_pCam3->SetTransformPrj();
		g_matViewProj	= GMAIN->m_pCamMn->m_pCam3->GetMatrixViw()*GMAIN->m_pCamMn->m_pCam3->GetMatrixPrj();
		/////////////////////////////////////////////////////////////////////////

		//���� ������ �ʱ�ȭ& ����
		if((m_dTime - m_dTimeEnd)  > 0)
		{
			m_nCurSetNum = GMAIN->m_nSetNumber;

			if(bBlueTeam)
			{
				m_nBlueOldScore = m_nBlueCurScore;
				if(m_nBlueOldScore >= m_nSetFinishScore)
					m_nCurSetNum++;

				if(m_nBlueCurScore > m_nRedCurScore)
					for(int i=0; i<2;++i){if(GMAIN->m_PlayerInfo[i].iSpikeEnergy <100){GMAIN->m_PlayerInfo[i].iSpikeEnergy+=50;}}
			}
			else
			{
				m_nRedOldScore = m_nRedCurScore;
				if(m_nRedOldScore >= m_nSetFinishScore)
					m_nCurSetNum++;

				if(m_nRedCurScore > m_nBlueCurScore)
					for(int i=2; i<4;++i){if(GMAIN->m_PlayerInfo[i].iSpikeEnergy <100){GMAIN->m_PlayerInfo[i].iSpikeEnergy+=50;}}
			}


			if(m_nRedOldScore >= m_nSetFinishScore)
			{GMAIN->m_nRedTeamWins++;}
			else if (m_nBlueOldScore >= m_nSetFinishScore)
			{GMAIN->m_nBlueTeamWins++;}

			m_bEventSceneFinished = TRUE;
			m_dTime	 = 0;
			m_dTimeEnd = 0;
			m_bTimeInit = FALSE;

			m_bAniSetChanged = FALSE;
		}
	}
	return hr;
}

//m_nOldSetNum != m_nCurSetNum �̶�� ó���� ����
void CGameSceneManager::SetChange()
{
	HRESULT hr = S_OK;

	m_bEventSceneFinished = FALSE;
	
	m_dTime	= timeGetTime();


	//������ �ð� �ʱ�ȭ
	if(m_bTimeInit == FALSE)
	{
		m_dTimeEnd = m_dTime+3000;
		m_bTimeInit = TRUE;
	}

	//ó�� �� ����
	if(m_bAniSetChanged == FALSE)
	{
		srand((int)time(NULL));

		for(int i=0; i<4; ++i)
		{
			AnimationInit(AniSetNameRandom(rand()%9),i);
		}

		m_bAniSetChanged  = TRUE;
	}
	else
	{
		/////////////////////////////////////////////////////////////////////////
		//09-08-04 ������
		GMAIN->m_pCamMn->SpecificCameraON(2);
		GMAIN->m_pCamMn->m_pCam2->SetTransformViw();
		GMAIN->m_pCamMn->m_pCam2->SetTransformPrj();
		g_matViewProj	= GMAIN->m_pCamMn->m_pCam2->GetMatrixViw()*GMAIN->m_pCamMn->m_pCam2->GetMatrixPrj();
		/////////////////////////////////////////////////////////////////////////

		//���� ������ �ʱ�ȭ& ����
		if((m_dTime - m_dTimeEnd)  > 0)
		{
			m_nOldSetNum=m_nCurSetNum;
			GMAIN->m_nSetNumber++;
			GMAIN->ChangeCoat();
			this->AfterSetInit();

			m_bEventSceneFinished = TRUE;
			m_dTime	 = 0;
			m_dTimeEnd = 0;
			m_bTimeInit = FALSE;

			m_bAniSetChanged = FALSE;
		}
	}
}

void CGameSceneManager::GameFinished()
{
	m_bEventSceneFinished = FALSE;

	//ó�� �� ����
	if(m_bAniSetChanged == FALSE)
	{
		srand((int)time(NULL));

		for(int i=0; i<4; ++i)
		{
			AnimationInit(AniSetNameRandom(rand()%9),i);
		}

		m_bAniSetChanged  = TRUE;
	}

	/////////////////////////////////////////////////////////////////////////
	//09-08-04 ������
	GMAIN->m_pCamMn->SpecificCameraON(2);
	GMAIN->m_pCamMn->m_pCam2->SetTransformViw();
	GMAIN->m_pCamMn->m_pCam2->SetTransformPrj();
	g_matViewProj	= GMAIN->m_pCamMn->m_pCam2->GetMatrixViw()*GMAIN->m_pCamMn->m_pCam2->GetMatrixPrj();
	/////////////////////////////////////////////////////////////////////////
}


void	CGameSceneManager::SpriteDraw()
{
	D3DXMATRIX	mtW;

	D3DXVECTOR3 vcDelta = g_pApp->m_pInput->GetMousePos();

	GMAIN->m_pd3dSprite->Begin(D3DXSPRITE_ALPHABLEND);

	D3DXMatrixIdentity(&mtW);
	mtW._11 =  float(GMAIN->m_rcWindowClient.right/1024.f);							// x�� ������
	mtW._22 =  float(GMAIN->m_rcWindowClient.bottom/768.f);							// y�� ������
	mtW._41	=  0;	// x ��ġ
	mtW._42	=  0;	// y ��ġ
	GMAIN->m_pd3dSprite->SetTransform(&mtW);



	if(m_bGameStart)			//������ ���۵Ǿ����� �ش� ������ ó��
	{
		//Get Ready
		m_vcPos=D3DXVECTOR3(349,333,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[44].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

		GMAIN->SpriteDraw();//��Ȳ��
	}
	else if(m_bGameFinished) //������ ������ ó���� �� ����
	{
		if(m_nGameFinishSet == GMAIN->m_nRedTeamWins)
		{
			//Red Team Wins
			m_vcPos=D3DXVECTOR3(320,338,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[46].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		}
		else if (m_nGameFinishSet == GMAIN->m_nBlueTeamWins)
		{
			//Blue Team Wins
			m_vcPos=D3DXVECTOR3(296,321,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[47].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		}
		GMAIN->SpriteDraw();//��Ȳ��

	}

	if(m_nCurSetNum != m_nOldSetNum && m_bGameFinished == FALSE)						//��Ʈ ü����
	{
		//Court Change
		m_vcPos=D3DXVECTOR3(349,333,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[45].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

		GMAIN->SpriteDraw();//��Ȳ��
	}


	GMAIN->m_pd3dSprite->End();
}

char* CGameSceneManager::AniSetNameRandom(INT randnum)
{
	char* AniSetName;

	switch(randnum)
	{
	case 0:
		AniSetName = "CONGCONG";      
		break;

	case 1:
		AniSetName = "STRETCHING";    
		break;

	case 2:
		AniSetName = "CHARACTERSELECT";
		break;

	case 3:
		AniSetName = "CHARACTERMOTION1";
		break;

	case 4:
		AniSetName = "CHARACTERMOTION2";
		break;

	case 5:
		AniSetName = "BREASTTOUCHING";
		break;

	case 6:
		AniSetName = "BREASTTOUCHING_SECOND";
		break;

	case 7:
		AniSetName = "SHOWER";
		break;

	case 8:
		AniSetName = "LOTION";
		break;

	default:
		AniSetName = "WAITING";
		break;
	}

	return AniSetName;

}


void CGameSceneManager::AfterSetInit()
{
	this->m_nRedOldScore = 0;			
	this->m_nRedCurScore = 0;			
	this->m_nBlueOldScore = 0;			
	this->m_nBlueCurScore = 0;

	GMAIN->m_nRedTeamScore = 0;
	GMAIN->m_nBlueTeamScore = 0;

	ZeroMemory(	g_score, sizeof(g_score) );
	ZeroMemory(	g_score, sizeof(g_score) );
}

void CGameSceneManager::AnimationInit(char* strAniName,INT modelNum)
{
	GMAIN->m_pModels[GMAIN->m_PlayerInfo[modelNum].iCharacterNum]->m_pAniController->SetAnimationSet(strAniName);
}

void CGameSceneManager::DefineTeamMember()
{
	if(GMAIN->m_bMultiGame)
	{
		/////////////////////////////////////////////////////////////////////////////
		//���� ��, ����� �����Ͽ� pvRedData�� pvBlueData �� push_back�� �Ѵ�.
		for(int i=0; i<4; ++i)
		{
			if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[i] == 1)
			{
				pvRedData.push_back( new CDefineMember(i) );
			}
			else if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[i] == 0)
			{
				pvBlueData.push_back( new CDefineMember(i) );
			}
		}

		INT i=0;

		m_nRedMemberTotal=pvRedData.size();
		m_nBlueMemberTotal=pvBlueData.size();

		/////////////////////////////////////////////////////////////////////////////
		//�ݺ��ڷ� �ش� �������� �ε����� ������ �ͼ� m_nRedMember�� �迭���ҿ� ����
		std::vector< CDefineMember* >::iterator    _FR = pvRedData.begin();
		std::vector< CDefineMember* >::iterator    _LR = pvRedData.end();

		for(; _FR != _LR; ++_FR)
		{
			m_nRedMember[i]=(*_FR)->nIdxNumber;
			i++;
		}

		_FR = pvRedData.begin();
		_LR = pvRedData.end();

		for(; _FR != _LR; ++_FR){delete (*_FR);}
		pvRedData.clear();


		i=0;
		/////////////////////////////////////////////////////////////////////////////
		//�ݺ��ڷ� �ش� �������� �ε����� ������ �ͼ� m_nBlueMember�� �迭���ҿ� ����
		std::vector< CDefineMember* >::iterator    _FB = pvBlueData.begin();
		std::vector< CDefineMember* >::iterator    _LB = pvBlueData.end();

		for(; _FB != _LB; ++_FB)
		{
			m_nBlueMember[i]=(*_FB)->nIdxNumber;
			i++;
		}

		_FB = pvBlueData.begin();
		_LB = pvBlueData.end();

		for(; _FB != _LB; ++_FB){delete (*_FB);}
		pvBlueData.clear();


		/////////////////////////////////////////////////////////////////////////////
		for(int k=0; k<4; ++k)
		{
			if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[k]==3)
			{
				pvComData.push_back( new CDefineMember(k) );
			}
		}
		INT HowManyRedComPlayer = 2-m_nRedMemberTotal;
		INT HowManyBlueComPlayer = 2-m_nBlueMemberTotal;

		INT k=0;

		m_nComMemberTotal = pvComData.size();

		std::vector< CDefineMember* >::iterator    _FC = pvComData.begin();
		std::vector< CDefineMember* >::iterator    _LC = pvComData.end();

		for(; _FC != _LC;)
		{
			m_nRedComputer[k]=(*_FC)->nIdxNumber;
			 ++_FC;
			if(k==HowManyRedComPlayer-1){break;}
			k++;
		}

		k=0;

		for(; _FC != _LC;)
		{
			m_nBlueComputer[k]=(*_FC)->nIdxNumber;
			++_FC;
			if(k==HowManyBlueComPlayer-1){break;}
			k++;
		}

		_FC = pvComData.begin();
		_LC = pvComData.end();

		for(; _FC != _LC; ++_FC){delete (*_FC);}
		pvComData.clear();
	}
	else
	{
		m_nRedMember[0]=0;
		m_nRedMember[1]=1;
		m_nBlueMember[0]=2;
		m_nBlueMember[1]=3;
	}
}

INT CGameSceneManager::MyTeamAndNumberCheck(INT nRoomPosition)
{
	if(m_nRedMember[0] == nRoomPosition)
	{
		return 0;
	}
	if(m_nRedMember[1] == nRoomPosition)
	{
		return 1;
	}
	if(m_nBlueMember[0] == nRoomPosition)
	{
		return 2;
	}
	if(m_nBlueMember[1] == nRoomPosition)
	{
		return 3;
	}

	return -1;
}

HRESULT CGameSceneManager::MultiPlayGotPoint(BOOL bBlueTeam)
{
	HRESULT hr=S_OK;
	m_nCurSetNum = GMAIN->m_nSetNumber;

	if(bBlueTeam)
	{
		m_nBlueOldScore = m_nBlueCurScore;
		if(m_nBlueOldScore >= m_nSetFinishScore)
			m_nCurSetNum++;

		if(m_nBlueCurScore > m_nRedCurScore)
			for(int i=0; i<2;++i){if(GMAIN->m_PlayerInfo[i].iSpikeEnergy <100){GMAIN->m_PlayerInfo[i].iSpikeEnergy+=50;}}
	}
	else
	{
		m_nRedOldScore = m_nRedCurScore;
		if(m_nRedOldScore >= m_nSetFinishScore)
			m_nCurSetNum++;

		if(m_nRedCurScore > m_nBlueCurScore)
			for(int i=2; i<4;++i){if(GMAIN->m_PlayerInfo[i].iSpikeEnergy <100){GMAIN->m_PlayerInfo[i].iSpikeEnergy+=50;}}
	}


	if(m_nRedOldScore >= m_nSetFinishScore)
	{GMAIN->m_nRedTeamWins++;}
	else if (m_nBlueOldScore >= m_nSetFinishScore)
	{GMAIN->m_nBlueTeamWins++;}

	m_bEventSceneFinished = TRUE;
	m_dTime	 = 0;
	m_dTimeEnd = 0;
	m_bTimeInit = FALSE;

	m_bAniSetChanged = FALSE;

	return hr;
}