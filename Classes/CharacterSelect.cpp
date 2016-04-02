#include "../Include/INCLUDE.h"

CCharacterSelect::CCharacterSelect(void)
{
	//09-08-12 ������
	SetRect(&m_rc,0,0,1024,768);
	m_vcPos= D3DXVECTOR3(0,0,0);			//�����ġ

	m_bAllRandomSelect = FALSE;
	m_bSelectFinished = FALSE;
	m_bPlayer1SelectFinished = FALSE;
	m_bPartnerSelectFinished = FALSE;
	m_bReady = FALSE;
	m_bOKClicked = FALSE;

	m_bEverybodyReady=FALSE;

	//ĳ���� ������ ��ġ, ĳ���� ��ȣ, ĳ���� ������ x��ǥ min, x��ǥ max, y��ǥ min, y��ǥ max
	m_Character[0]= new CChatacterIcon(D3DXVECTOR3(9,614,0),0,19,99,657,740);         //"Manami"
	m_Character[1]= new CChatacterIcon(D3DXVECTOR3(116,614,0),1,125,204,657,740);     //"Esk"
	m_Character[2]= new CChatacterIcon(D3DXVECTOR3(228,614,0),2,237,316,657,740);     //"Eo"
	m_Character[3]= new CChatacterIcon(D3DXVECTOR3(341,614,0),3,350,429,657,740);     //"Reiko"
	m_Character[4]= new CChatacterIcon(D3DXVECTOR3(453,614,0),4,462,541,657,740);     //"Maria"
	m_Character[5]= new CChatacterIcon(D3DXVECTOR3(570,614,0),5,579,659,657,740);     //"Maya"
	m_Character[6]= new CChatacterIcon(D3DXVECTOR3(685,614,0),6,694,773,657,740);     //"Bael"
	m_Character[7]= new CChatacterIcon(D3DXVECTOR3(799,614,0),7,808,888,657,740);     //"Fei"
	m_Character[8]= new CChatacterIcon(D3DXVECTOR3(916,614,0),9,925,1004,657,740);    //"Random"

	srand((int)time(NULL));

	for(int i=0; i<4; ++i){m_strAnimationSetName[i] = AniSetNameRandom(rand()%9);}
}

CCharacterSelect::~CCharacterSelect(void)
{
	Destroy();
}

void CCharacterSelect::Destroy()
{
	int i=0;
	for(i=0; i<9 ; ++i){SAFE_DELETE( m_Character[i] );}
}


//ĳ���ͼ���ȭ�� ����
int CCharacterSelect::CharacterSelectFrameMove()
{
	//������ �ο� & ����� �ο��� �����ϱ� ����
	GMAIN->m_GameSceneManager->DefineTeamMember();

	if(GMAIN->m_bMultiGame)
	{
		MultiPlayEveryBodyReady();//������ �÷��̾���� ��� �����϶� ���ȭ������ �Ѿ�� ��
	}

	FRAMEMOVE(	GMAIN->m_pBackground, GMAIN->m_fElapsedTime);
	FRAMEMOVE( GMAIN->m_pNet, GMAIN->m_fElapsedTime);

	SAFE_FRMOV(	GMAIN->m_pInput	);
	SAFE_FRMOV(	GMAIN->m_pCamMn	);


	if(GMAIN->m_bMultiGame)
	{
		GMAIN->m_MultiPlay->UpdateToServer();
	}

	//���콺 ������ ��ư Ŭ���� ĳ���� ���� ����
	if(g_pApp->m_pInput->BtnDown(1))
	{
		if(GMAIN->m_bMultiGame)
		{
			if(m_bOKClicked==FALSE)
			{
				//��Ƽ�÷���:
				//�ش� Ŭ���̾�Ʈ�� ȣ��Ʈ�� ��� ��� ������ �ʱ�ȭ �� �� �ִ�.
				if(GMAIN->m_MultiPlay->m_MyInfo.RoomPosition ==0)
				{
					GMAIN->m_MultiPlay->Client_Room_Reset();
				}
			}
		}
		else
		{
			InitVal();
		}
	}
	
	return 0;
}
//ĳ���ͼ���ȭ�� ������
void CCharacterSelect::CharaterSelectRender()
{
	/////////////////////////////////////////////////////////////////////////
	//09-08-04 ������
	GMAIN->m_pCamMn->SpecificCameraON(1);
	GMAIN->m_pCamMn->m_pCam1->SetTransformViw();
	GMAIN->m_pCamMn->m_pCam1->SetTransformPrj();
	g_matViewProj	= GMAIN->m_pCamMn->m_pCam1->GetMatrixViw()*GMAIN->m_pCamMn->m_pCam1->GetMatrixPrj();
	/////////////////////////////////////////////////////////////////////////

	GMAIN->m_pBackground->Render(GMAIN->m_pd3dDevice);
	GMAIN->m_pNet->Render(GMAIN->m_pd3dDevice);


	D3DXMATRIXA16 mWorld;
	D3DXMatrixTranslation(&mWorld,0,0,0);
	GMAIN->m_pd3dDevice->SetTransform(D3DTS_WORLD,&mWorld);


	//09-08-25 ������
	CharacterChooseRender();

	if(m_bOKClicked)
	{
		CharacterRender();
	}


	SpriteDraw(); //�̱��÷���&��Ƽ�÷���: �޴� ��������Ʈ ��� & ������Ʈ ����

	//Output statistics
	GMAIN->m_pFont->DrawText(   2,  0, D3DCOLOR_ARGB(255,255,255,0), GMAIN->m_strFrameStats );
	GMAIN->m_pFont->DrawText(   2, 20, D3DCOLOR_ARGB(255,255,255,0), GMAIN->m_strDeviceStats );

	//��Ƽ�÷���:
	if(GMAIN->m_bMultiGame)
	{
		MultiPlayStatus();
	}


}

void CCharacterSelect::MultiPlayStatus()
{
	char strTotalNumber[80];
	sprintf(strTotalNumber, "���� �����ο� : %d��",GMAIN->m_MultiPlay->m_RoomInfo.TotalNumber);

	g_text.Begin();

	g_text.Draw("==============",5,300);
	g_text.Draw(strTotalNumber,5,320);

	 if(g_NetworkState ==CLIENT_DISCONNECT)
	 {
		g_text.Draw("���� ����: �������",5,340);
	 }
	 else
	 {
		 if(GMAIN->m_MultiPlay->m_MyInfo.RoomPosition ==0)
		 {g_text.Draw("���� ����: HOST",5,340);}
		 else
		 {g_text.Draw("���� ����: GUEST",5,340);}
	 }


	g_text.Draw("==============",5,360);
	g_text.Draw("[������]",5,380);
	for(int i=0; i<2; ++i)
	{
		if(GMAIN->m_GameSceneManager->m_nRedMember[i] ==4)
			continue;

		if(GMAIN->m_MultiPlay->m_RoomInfo.bReadyState[GMAIN->m_GameSceneManager->m_nRedMember[i]]==TRUE)
		{g_text.Draw("(O)",5,400+20*i);}
		else 
		{g_text.Draw("(X)",5,400+20*i);}
	}

	for(int i=0; i<2; ++i)
	{
		if(GMAIN->m_GameSceneManager->m_nRedMember[i] ==4)
			continue;

		if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[GMAIN->m_GameSceneManager->m_nRedMember[i]]==1)
		{
			if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[GMAIN->m_GameSceneManager->m_nRedMember[i]]==1)
			{
				g_text.Draw("COMPUTER",40,400+20*i);
			}
			else if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[GMAIN->m_GameSceneManager->m_nRedMember[i]]==0)
			{
				g_text.Draw(GMAIN->m_MultiPlay->m_RoomInfo.sCharacterName[GMAIN->m_GameSceneManager->m_nRedMember[i]],40,400+20*i);
			}
		}
	}

	g_text.Draw("[�����]",5,440);
	for(int i=0; i<2; ++i)
	{
		if(GMAIN->m_GameSceneManager->m_nBlueMember[i] ==4)
			continue;

		if(GMAIN->m_MultiPlay->m_RoomInfo.bReadyState[GMAIN->m_GameSceneManager->m_nBlueMember[i]]==TRUE)
		{g_text.Draw("(O)",5,460+20*i);}
		else
		{g_text.Draw("(X)",5,460+20*i);}
	}

	for(int i=0; i<2; ++i)
	{
		if(GMAIN->m_GameSceneManager->m_nBlueMember[i] ==4)
			continue;

		if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[GMAIN->m_GameSceneManager->m_nBlueMember[i]]==0)
		{
			if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[GMAIN->m_GameSceneManager->m_nBlueMember[i]]==1)
			{
				g_text.Draw("COMPUTER",40,460+20*i);
			}
			else if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[GMAIN->m_GameSceneManager->m_nBlueMember[i]]==0)
			{
				g_text.Draw(GMAIN->m_MultiPlay->m_RoomInfo.sCharacterName[GMAIN->m_GameSceneManager->m_nBlueMember[i]],40,460+20*i);
			}
		}
	}

	g_text.Draw("==============",5,500);

	g_text.End();

}

void CCharacterSelect::InitVal()
{

	for(int i=0; i<4; ++i)
	{
		GMAIN->m_PlayerInfo[i].bComputer=TRUE;
		GMAIN->m_PlayerInfo[i].nRedTeam=3;
		GMAIN->m_PlayerInfo[i].iCharacterNum=10;
	}



	if(g_ppPlayers != NULL)
	{
		GMAIN->m_eFlow=READYPLAY;
		g_ppPlayers	= NULL;
		for( int i=0; i<NUMPLAYERS; ++i )
		{
			SAFE_DELETE( GMAIN->m_ppPlayers[i] );
		}
	}

	srand((int)time(NULL));

	for(int i=0; i<4; ++i)
	{
		m_strAnimationSetName[i] = AniSetNameRandom(rand()%9);
	}

	m_bAllRandomSelect = FALSE;
	m_bSelectFinished = FALSE;
	m_bPlayer1SelectFinished = FALSE;
	m_bPartnerSelectFinished = FALSE;
	m_bReady = FALSE;
	m_bOKClicked = FALSE;

	m_bEverybodyReady=FALSE;

	GMAIN->m_pCamMn->m_bJumpAnimationOK=FALSE;
	GMAIN->m_pCameraAniModel->m_pAniController->m_bAniFinished = FALSE;
}

void CCharacterSelect::CharacterRandomSelect()
{
	int nPlayer1 = 0;
	int nPlayer2 = 0;
	int nPlayer3 = 0;

	srand((int)time(NULL));

	if(GMAIN->m_PlayerInfo[1].iCharacterNum==9)
	{
		nPlayer1=GMAIN->m_PlayerInfo[0].iCharacterNum;

		//�÷��̾�2 ���� ����
		while(nPlayer1==GMAIN->m_PlayerInfo[0].iCharacterNum)
		{
			nPlayer1=rand()%8; //0���� 7���� ���� �� �߻�
		}
		GMAIN->m_PlayerInfo[1].iCharacterNum=nPlayer1;
		GMAIN->m_PlayerInfo[1].bComputer=TRUE;
		GMAIN->m_PlayerInfo[1].nRedTeam=1;

		m_bSelectFinished=TRUE;
	}


	if(m_bAllRandomSelect==FALSE) // �� ĳ���Ͱ� ���� ������ �ƴ� ��
	{
		nPlayer2=GMAIN->m_PlayerInfo[1].iCharacterNum;

		//�÷��̾�3 ���� ����
		while(nPlayer2==GMAIN->m_PlayerInfo[1].iCharacterNum || nPlayer2== GMAIN->m_PlayerInfo[0].iCharacterNum)
		{
			nPlayer2=rand()%8; //0���� 7���� ���� �� �߻�
		}
		GMAIN->m_PlayerInfo[2].iCharacterNum=nPlayer2;
		GMAIN->m_PlayerInfo[2].bComputer=TRUE;
		GMAIN->m_PlayerInfo[2].nRedTeam=0;


		nPlayer3=GMAIN->m_PlayerInfo[2].iCharacterNum;

		//�÷��̾�4 ���� ����
		while(nPlayer3==GMAIN->m_PlayerInfo[2].iCharacterNum 
			|| nPlayer3== GMAIN->m_PlayerInfo[1].iCharacterNum
			|| nPlayer3== GMAIN->m_PlayerInfo[0].iCharacterNum)
		{
			nPlayer3=rand()%8; //0���� 7���� ���� �� �߻�
		}
		GMAIN->m_PlayerInfo[3].iCharacterNum=nPlayer3;
		GMAIN->m_PlayerInfo[3].bComputer=TRUE;
		GMAIN->m_PlayerInfo[3].nRedTeam=0;
	}
	else//�� ĳ���Ͱ� ���� ���� �϶�
	{
		//�÷��̾�1 ���� ����
		GMAIN->m_PlayerInfo[0].iCharacterNum=rand()%8; //0���� 7���� ���� �� �߻�;
		GMAIN->m_PlayerInfo[0].bComputer=FALSE;
		GMAIN->m_PlayerInfo[0].nRedTeam=1;


		nPlayer1=GMAIN->m_PlayerInfo[0].iCharacterNum;

		//�÷��̾�2 ���� ����
		while(nPlayer1==GMAIN->m_PlayerInfo[0].iCharacterNum)
		{
			nPlayer1=rand()%8; //0���� 7���� ���� �� �߻�
		}
		GMAIN->m_PlayerInfo[1].iCharacterNum=nPlayer1;
		GMAIN->m_PlayerInfo[1].bComputer=TRUE;
		GMAIN->m_PlayerInfo[1].nRedTeam=1;



		nPlayer2=GMAIN->m_PlayerInfo[1].iCharacterNum;

		//�÷��̾�3 ���� ����
		while(nPlayer2==GMAIN->m_PlayerInfo[1].iCharacterNum || nPlayer2== GMAIN->m_PlayerInfo[0].iCharacterNum)
		{
			nPlayer2=rand()%8; //0���� 7���� ���� �� �߻�
		}
		GMAIN->m_PlayerInfo[2].iCharacterNum=nPlayer2;
		GMAIN->m_PlayerInfo[2].bComputer=TRUE;
		GMAIN->m_PlayerInfo[2].nRedTeam=0;


		nPlayer3=GMAIN->m_PlayerInfo[2].iCharacterNum;

		//�÷��̾�4 ���� ����
		while(nPlayer3==GMAIN->m_PlayerInfo[2].iCharacterNum 
			|| nPlayer3== GMAIN->m_PlayerInfo[1].iCharacterNum
			|| nPlayer3== GMAIN->m_PlayerInfo[0].iCharacterNum)
		{
			nPlayer3=rand()%8; //0���� 7���� ���� �� �߻�
		}
		GMAIN->m_PlayerInfo[3].iCharacterNum=nPlayer3;
		GMAIN->m_PlayerInfo[3].bComputer=TRUE;
		GMAIN->m_PlayerInfo[3].nRedTeam=0;
	}


	m_bReady=TRUE;
}



HRESULT CCharacterSelect::CharacterRender()
{
	HRESULT hr = S_OK;


	switch( GMAIN->m_eFlow )
	{
	case READYPLAY:
		{
			GMAIN->ReadyPlay();
			break;
		}
	case PLAY:
		{
			for( int i=0; i<NUMPLAYERS; ++i )
			{				
				FRAMEMOVE( GMAIN->m_ppPlayers[i], GMAIN->m_fElapsedTime );
			}
			//-------------------------------------------------------------------
			for( int i=0; i<NUMPLAYERS; ++i )
			{
				CHECK_FAILED( GMAIN->m_ppPlayers[i]->Render(GMAIN->m_pd3dDevice) );
			}
			break;
		}
	}
	
	

	return hr;
}

void CCharacterSelect::CharacterChooseRender()
{
	if(GMAIN->m_bMultiGame) //��Ƽ�÷��̶�
	{
		for(int i=0; i<4; ++i)
		{
			if( GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i] == 10)
				continue;

			GMAIN->m_pModels[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->m_pAniController->SetAnimationSet(m_strAnimationSetName[i]);

			CharacterAnimationPlayTypeChange(GMAIN->m_pModels[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]);

			GMAIN->m_pModels[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->m_matT._41=-160.f;
			GMAIN->m_pModels[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->m_matT._42=8.5f;
			GMAIN->m_pModels[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->m_matT._43=-145.f-40.f*i;
			GMAIN->m_pModels[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->m_fRotationAngle=D3DXToRadian(90.f);
			D3DXMatrixScaling(&GMAIN->m_pModels[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->m_matS, 3.f, 3.f, 3.f);

			GMAIN->m_pModels[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->FrameMove(GMAIN->m_fElapsedTime);
			GMAIN->m_pModels[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->Render(GMAIN->m_pd3dDevice);
		}
	}
	else //�̱��÷��̶�
	{
		for(int i=0; i<4; ++i)
		{
			if( GMAIN->m_PlayerInfo[i].iCharacterNum == 10)
				continue;

			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_pAniController->SetAnimationSet(m_strAnimationSetName[i]);

			CharacterAnimationPlayTypeChange(GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]);

			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matT._41=-160.f;
			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matT._42=8.5f;
			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matT._43=-145.f-40.f*i;
			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_fRotationAngle=D3DXToRadian(90.f);
			D3DXMatrixScaling(	&GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->m_matS, 3.f, 3.f, 3.f);

			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->FrameMove(GMAIN->m_fElapsedTime);
			GMAIN->m_pModels[GMAIN->m_PlayerInfo[i].iCharacterNum]->Render(GMAIN->m_pd3dDevice);
		}
	}
}

HRESULT CCharacterSelect::CharacterAnimationPlayTypeChange(Character* pModel)
{

	HRESULT hr = S_OK;

	LPD3DXANIMATIONSET	pSet	= NULL;
	D3DXTRACK_DESC		track;
	DOUBLE				period	= 0.f;
	DOUBLE				pos		= 0.f;
	LPCTSTR				pName	= NULL;

	ZeroMemory( &track, sizeof track );

	hr	= pModel->m_pAniController->m_ppAniController[pModel->m_pAniController->m_currentAniController]->GetTrackDesc(pModel->m_pAniController->m_currentTrack, &track );
	CHECK_FAILED( hr );

	hr	= pModel->m_pAniController->m_ppAniController[pModel->m_pAniController->m_currentAniController]->GetTrackAnimationSet(pModel->m_pAniController->m_currentTrack, &pSet );
	CHECK_FAILED( hr );

	period	= pSet->GetPeriod();
	pos		= track.Position;
	pName	= pSet->GetName();

	pModel->m_pAniController->m_mapAniSets[pName].PlayType = LOOP;

	return hr;
}

char* CCharacterSelect::AniSetNameRandom(INT randnum)
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

void CCharacterSelect::SpriteDraw()
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


	//�̱��÷��� �϶��� ��Ƽ�÷��� �϶��� ó���� �ٸ���
	if(GMAIN->m_bMultiGame)
	{SpriteDrawMultiPlay(vcDelta);}
	else
	{SpriteDrawSinglePlay(vcDelta);}
	
	//�޴�, ��ư, ���� ��ư ó�� ���...
	SpriteDrawEtc(vcDelta);

	//OK��ư Ŭ�� �Ͽ��� ��
	if(m_bOKClicked == TRUE)
	{
		SpriteDrawOKClicked(vcDelta);
	}

	GMAIN->m_pd3dSprite->End();
}
void CCharacterSelect::SpriteDrawSinglePlay(D3DXVECTOR3 vcDelta)
{
	if(m_bSelectFinished==FALSE)
	{
		if(m_bPlayer1SelectFinished==FALSE)
		{

			for (int i=0;i<9;++i)
			{
				INT nCheck = RectCheck(vcDelta, m_Character[i]->xmin, m_Character[i]->xmax, m_Character[i]->ymin, m_Character[i]->ymax);
				if(1==nCheck)
				{
					GMAIN->m_PlayerInfo[0].bComputer=FALSE;
					GMAIN->m_PlayerInfo[0].nRedTeam=1;
					GMAIN->m_PlayerInfo[0].iCharacterNum=m_Character[i]->number;

					if(m_Character[i]->number==9)
					{
						m_bAllRandomSelect=TRUE;
					}
					m_bPlayer1SelectFinished=TRUE;

					break;
				}
				else if(2==nCheck)
				{
					GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[6].m_pTex, &m_rc, NULL, &m_Character[i]->vcPos, D3DXCOLOR(1,1,1,1.f));
					break;
				}
			}
		}
		else if(m_bReady==FALSE) //��Ʈ�� ���� �κ�
		{
			for (int i=0;i<9;++i)
			{
				if(GMAIN->m_PlayerInfo[0].iCharacterNum==i){continue;}

				INT nCheck = RectCheck(vcDelta,m_Character[i]->xmin,m_Character[i]->xmax,m_Character[i]->ymin,m_Character[i]->ymax);

				if(nCheck ==1)
				{
					GMAIN->m_PlayerInfo[1].bComputer=TRUE;
					GMAIN->m_PlayerInfo[1].nRedTeam=1;
					GMAIN->m_PlayerInfo[1].iCharacterNum=m_Character[i]->number;

					if(GMAIN->m_PlayerInfo[1].iCharacterNum !=9)
					{
						m_bSelectFinished=TRUE;
					}
					break;
				}
				else if(nCheck ==2)
				{
					GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[7].m_pTex, &m_rc, NULL, &m_Character[i]->vcPos, D3DXCOLOR(1,1,1,1.f));
					break;
				}
			}

		}
	}
	else if(m_bReady==FALSE && m_bSelectFinished==TRUE)//m_bSelectFinished==TRUE�ϰ�� ��ǻ�� ĳ���͵�(3,4 �� �÷��̾�)�� ���� �����Ѵ�.
	{CharacterRandomSelect();}

	if(m_bAllRandomSelect == TRUE && m_bReady==FALSE)//���� ������ ��� ĳ���͵�(1,2,3,4 �� �÷��̾�)�� ���� �����Ѵ�.
	{CharacterRandomSelect();}

	if(GMAIN->m_PlayerInfo[1].iCharacterNum==9)//��Ʈ�ʰ� ���� ������ ��� ĳ���͵�(2,3,4 �� �÷��̾�)�� ���� �����Ѵ�.
	{CharacterRandomSelect();}
}
void CCharacterSelect::SpriteDrawMultiPlay(D3DXVECTOR3 vcDelta)
{
	m_vcPos=D3DXVECTOR3(0,293,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[52].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

	SpriteDrawTeamSelect(vcDelta);

	if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[GMAIN->m_MultiPlay->m_MyInfo.RoomPosition] ==3)
		return;

	if(m_bSelectFinished==FALSE)
	{
		if(m_bPlayer1SelectFinished==FALSE)
		{
			for (int i=0;i<9;++i)
			{
				INT nCheck = RectCheck(vcDelta,m_Character[i]->xmin,m_Character[i]->xmax,m_Character[i]->ymin,m_Character[i]->ymax);

				if(nCheck == 1)
				{
					//��Ƽ�÷���: ���� ������ ĳ���Ͱ� �ٸ� �÷��̾ ���õ��� �ʾҴٸ� �ش�Ǵ� ���� ó��
					if(MultiPlayMyCharacterSelect(TRUE,m_Character[i]->number))
					{
						GMAIN->m_MultiPlay->m_CurMyInfo.nCharIndex=m_Character[i]->number;

						m_bPlayer1SelectFinished=TRUE;
						m_bSelectFinished=TRUE;
						m_bReady = TRUE;
					}

					if(m_Character[i]->number==9)
					{
						srand((int)time(NULL));

						//��Ƽ�÷���: ĳ���͸� ���� ������ �Ұ��
						//����ٸ� �������� �ش� ĳ���͸� ���� �Ͽ����� �ٽ� rand()%8
						BOOL bRandomSelect = TRUE;
						while(bRandomSelect)
						{
							INT nRandom=rand()%8;
							if(MultiPlayMyCharacterSelect(TRUE,nRandom))
							{
								GMAIN->m_MultiPlay->m_CurMyInfo.nComputer=0;
								GMAIN->m_MultiPlay->m_CurMyInfo.nCharIndex=nRandom;

								m_bPlayer1SelectFinished=TRUE;
								m_bSelectFinished=TRUE;
								m_bReady = TRUE;

								bRandomSelect=FALSE;
							}
						}
					}
					break;
				}
				else if(nCheck ==2)
				{
					if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[GMAIN->m_MultiPlay->m_MyInfo.RoomPosition] == 1)
					{
						GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[6].m_pTex, &m_rc, NULL, &m_Character[i]->vcPos, D3DXCOLOR(1,1,1,1.f));
					}
					else if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[GMAIN->m_MultiPlay->m_MyInfo.RoomPosition] == 0)
					{
						GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[8].m_pTex, &m_rc, NULL, &m_Character[i]->vcPos, D3DXCOLOR(1,1,1,1.f));
					}
					break;
				}
			}
		}
	}
}

void CCharacterSelect::SpriteDrawOKClicked(D3DXVECTOR3 vcDelta)
{
	//���ھ� ����
	m_vcPos=D3DXVECTOR3(289,196,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[48].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

	if(GMAIN->m_bMultiGame)		//��Ƽ�÷��� �϶�
	{
		//���� ���ھ� ���: �� ��Ʈ �̰ܾ� �ϴ���....
		m_vcPos=D3DXVECTOR3(605,219,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[GMAIN->m_MultiPlay->m_RoomInfo.nGameScore+21].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

		//��Ʈ ���ھ� ���: �Ѽ�Ʈ�� ������ ���� �ϴ���....
		if(GMAIN->m_MultiPlay->m_RoomInfo.nSetScore <10)
		{
			m_vcPos=D3DXVECTOR3(605,305,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[GMAIN->m_MultiPlay->m_RoomInfo.nSetScore+21].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		}
		else if(GMAIN->m_MultiPlay->m_RoomInfo.nSetScore <20)
		{	
			//1
			m_vcPos=D3DXVECTOR3(598,305,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[22].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

			m_vcPos=D3DXVECTOR3(610,305,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[GMAIN->m_MultiPlay->m_RoomInfo.nSetScore+11].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		}
	}
	else	//�̱��÷��� �϶�
	{
		//���� ���ھ� ���: �� ��Ʈ �̰ܾ� �ϴ���....
		m_vcPos=D3DXVECTOR3(605,219,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[GMAIN->m_GameSceneManager->m_nGameFinishSet+21].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

		//��Ʈ ���ھ� ���: �Ѽ�Ʈ�� ������ ���� �ϴ���....
		if(GMAIN->m_GameSceneManager->m_nSetFinishScore <10)
		{
			m_vcPos=D3DXVECTOR3(605,305,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[GMAIN->m_GameSceneManager->m_nSetFinishScore+21].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		}
		else if(GMAIN->m_GameSceneManager->m_nSetFinishScore <20)
		{	
			//1
			m_vcPos=D3DXVECTOR3(598,305,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[22].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

			m_vcPos=D3DXVECTOR3(610,305,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[GMAIN->m_GameSceneManager->m_nSetFinishScore+11].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		}
	}
	//��Ƽ�÷��̽ÿ��� ȣ��Ʈ�� �̰ɷ� �����ϱ�? �������� "�� �ƴϿ�"�� �����ϵ��� �Ѵ�.
	if(GMAIN->m_bMultiGame){if(GMAIN->m_MultiPlay->m_MyInfo.RoomPosition !=0){return;}}

	//�̰ɷ� �����ϱ�?
	m_vcPos=D3DXVECTOR3(220,348,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[38].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

	//�� �ƴϿ� ��ư
	m_vcPos=D3DXVECTOR3(320,422,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[40].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));


	INT nCheck = RectCheck(vcDelta,326,505,429,503);
	if(nCheck ==1)
	{
		if(GMAIN->m_bMultiGame)
		{
			//��Ƽ�÷���:
			//������ ��� Ŭ���̾�Ʈ���� ������ ���۵Ǿ�� ���� �˷��ش�.
			GMAIN->m_MultiPlay->Client_Room_Game_Start();
		}
		else
		{
			GMAIN->m_nGamePhase =1;
		}
	}
	else if(nCheck ==2)
	{
		//��
		m_rc.left = 0  ; m_rc.right = 184;
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[41].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		m_rc.left = 0  ; m_rc.right = 1024;
	}
	else if(nCheck==0)
	{
		nCheck = RectCheck(vcDelta,518,696,429,503);

		if(nCheck ==1)
		{
			if(GMAIN->m_bMultiGame)
			{
				//��Ƽ�÷���:
				//������ ��� �÷��̾�, �������� ��ȿȭ�Ѵ�.
				GMAIN->m_MultiPlay->Client_Room_Reset();
			}
			else
			{
				InitVal();
			}
		}
		else if(nCheck ==2)
		{
			//�ƴϿ�
			m_vcPos.x += 192;
			m_rc.left = 192  ; m_rc.right = 384;
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[41].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
			m_rc.left = 0  ; m_rc.right = 1024;
		}
		else if(nCheck ==0)
		{
			nCheck=RectCheck(vcDelta,582,594,212,247);
			if(nCheck ==1)
			{
				if(GMAIN->m_bMultiGame)
				{
					if(GMAIN->m_MultiPlay->m_OldGameScore.nGameScore >1)
					{GMAIN->m_MultiPlay->m_CurGameScore.nGameScore--;}
				}
				else
				{
					if(GMAIN->m_GameSceneManager->m_nGameFinishSet > 1)
					{GMAIN->m_GameSceneManager->m_nGameFinishSet--;}
				}
			}
			else if(nCheck==0)
			{
				nCheck=RectCheck(vcDelta,631,644,212,247);

				if(nCheck ==1)
				{
					if(GMAIN->m_bMultiGame)
					{
						if(GMAIN->m_MultiPlay->m_OldGameScore.nGameScore <3)
						{GMAIN->m_MultiPlay->m_CurGameScore.nGameScore++;}
					}
					else
					{
						if(GMAIN->m_GameSceneManager->m_nGameFinishSet < 3)
						{GMAIN->m_GameSceneManager->m_nGameFinishSet++;}
					}
				}
				else if(nCheck==0)
				{
					nCheck=RectCheck(vcDelta,580,592,300,328);
					if(nCheck==1)
					{
						if(GMAIN->m_bMultiGame)
						{
							if(GMAIN->m_MultiPlay->m_OldGameScore.nSetScore >3)
							{GMAIN->m_MultiPlay->m_CurGameScore.nSetScore--;}
						}
						else
						{
							if(GMAIN->m_GameSceneManager->m_nSetFinishScore > 3)
							{GMAIN->m_GameSceneManager->m_nSetFinishScore--;}
						}	
					}
					else if(nCheck==0)
					{
						nCheck = RectCheck(vcDelta,632,644,300,328);

						if(nCheck==1)
						{
							if(GMAIN->m_bMultiGame)
							{
								if(GMAIN->m_MultiPlay->m_OldGameScore.nSetScore <15)
								{GMAIN->m_MultiPlay->m_CurGameScore.nSetScore++;}
							}
							else
							{
								if(GMAIN->m_GameSceneManager->m_nSetFinishScore < 15)
								{GMAIN->m_GameSceneManager->m_nSetFinishScore++;}	
							}
							
						}
					}
				}
			}
		}
	}
}

void CCharacterSelect::SpriteDrawEtc(D3DXVECTOR3 vcDelta)
{
	if(m_bPlayer1SelectFinished==TRUE && m_bSelectFinished==FALSE 
		&& m_bAllRandomSelect==FALSE)
	{
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[6].m_pTex, &m_rc, NULL, &m_Character[GMAIN->m_PlayerInfo[0].iCharacterNum]->vcPos, D3DXCOLOR(1,1,1,1.f));
	}

	if(GMAIN->m_bMultiGame) //��Ƽ�÷��̶�
	{
		for(int i=0; i<4; ++i)
		{
			if(GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i] == 10)
				continue;

			if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[i] ==1)
			{
				if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[i]==0)
				{
					GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[6].m_pTex, &m_rc, NULL, &m_Character[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->vcPos, D3DXCOLOR(1,1,1,1.f));
				}
				else if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[i]==1)
				{
					GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[7].m_pTex, &m_rc, NULL, &m_Character[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->vcPos, D3DXCOLOR(1,1,1,1.f));
				}
			}
			else if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[i] ==0)
			{
				if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[i]==0)
				{
					GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[8].m_pTex, &m_rc, NULL, &m_Character[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->vcPos, D3DXCOLOR(1,1,1,1.f));
				}
				else if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[i]==1)
				{
					GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[9].m_pTex, &m_rc, NULL, &m_Character[GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i]]->vcPos, D3DXCOLOR(1,1,1,1.f));
				}
			}
		}
	}

	if(!GMAIN->m_bMultiGame)
	{
		if(m_bReady==TRUE)
		{
			//�̱��÷��̶�
			//���õ� ĳ���͵��� �������� ���õǾ� �ִ� ���� ������
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[6].m_pTex, &m_rc, NULL, &m_Character[GMAIN->m_PlayerInfo[0].iCharacterNum]->vcPos, D3DXCOLOR(1,1,1,1.f));
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[7].m_pTex, &m_rc, NULL, &m_Character[GMAIN->m_PlayerInfo[1].iCharacterNum]->vcPos, D3DXCOLOR(1,1,1,1.f));
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[9].m_pTex, &m_rc, NULL, &m_Character[GMAIN->m_PlayerInfo[2].iCharacterNum]->vcPos, D3DXCOLOR(1,1,1,1.f));
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[9].m_pTex, &m_rc, NULL, &m_Character[GMAIN->m_PlayerInfo[3].iCharacterNum]->vcPos, D3DXCOLOR(1,1,1,1.f));
		}
	}



	//�޴�ȭ�� ���
	m_vcPos=D3DXVECTOR3(0,0,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[5].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));



	INT nCheck = 0;

	if(m_bReady==TRUE && m_bOKClicked !=TRUE && GMAIN->m_MultiPlay->m_RoomInfo.bReadyState[GMAIN->m_MultiPlay->m_MyInfo.RoomPosition] == FALSE)
	{
		//OK ��ư(���� �Ǿ��� ��)
		m_vcPos=D3DXVECTOR3(914,534,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[10].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

		nCheck = RectCheck(vcDelta,915,1008,534,566);

		if(nCheck ==1)
		{
			//GMAIN->m_nGamePhase=1;
			if(	m_bOKClicked == FALSE)
			{
				if(GMAIN->m_bMultiGame) //��Ƽ�÷���:	
				{		
					//if(GMAIN->m_MultiPlay->m_RoomInfo.TotalNumber >1)
					if(GMAIN->m_GameSceneManager->m_nComMemberTotal <=2)
					{
						if(GMAIN->m_MultiPlay->m_MyInfo.RoomPosition==0)
						{
							if(MultiPlayAllPlayerReadyCheck()) //������ �÷��̾���� ��� �����϶�
							{
								{MultiPlayComputerCharacterRandomSelect();}	//������ ȣ��Ʈ�� ��ǻ�� �÷��̾�� ���� �����Ͽ� ������ ������Ʈ
							}
						}
						else
						{
							//������ �ش� ������ ������ ���� �˷� �־����!
							GMAIN->m_MultiPlay->m_CurMyInfo.bReadyState=TRUE;
						}
					}
				}
				else
				{
					GMAIN->m_pCamMn->m_bJumpAnimationOK = FALSE;
					m_bOKClicked = TRUE;
				}
			}
			return;
		}
		else if(nCheck ==2)
		{
			//OK ��ư(���콺�� ���� ���� ��)
			m_vcPos=D3DXVECTOR3(897,515,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[11].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

			return;
		}
	}


	nCheck = RectCheck(vcDelta,916,1008,577,609);

	if(nCheck==1)
	{
		//�޴� ȭ������ �������� ĳ���� ���ð��� ������ �ʱ�ȭ
		InitVal();
		GMAIN->m_nGamePhase=0;

		if(GMAIN->m_bMultiGame)
		{
			//��Ƽ�÷���:
			//�������� ������ ����
			GMAIN->m_MultiPlay->Client_Room_Exit();
		}
		
		return;
	}
	else if(nCheck ==2)
	{
		//���� ������ ���
		m_vcPos=D3DXVECTOR3(907,574,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[12].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		return;
	}
}

void CCharacterSelect::SpriteDrawTeamSelect(D3DXVECTOR3 vcDelta)
{
	if(m_bOKClicked)
		return;

	//��Ƽ�÷���:
	INT nCheck;

	//��ư ���
	m_vcPos=D3DXVECTOR3(12,534,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[49].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

	if(GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[GMAIN->m_MultiPlay->m_MyInfo.RoomPosition] !=3)
		return;

	//������ ��ư(���콺�� ���� ���� ��)
	nCheck = RectCheck(vcDelta,14,106,536,565);
	if(nCheck == 1)
	{
		//Ŭ�� �Ͽ��� �� ó�� ����
		//�濡 ������ �ο��� 2�� �ʰ��� ó������ ����
		if(GMAIN->m_GameSceneManager->m_nRedMemberTotal <2)
		{
			GMAIN->m_MultiPlay->m_CurMyInfo.nComputer=0;
			GMAIN->m_MultiPlay->m_CurMyInfo.nRedTeam= 1;
		}
		return;
	}
	else if(nCheck ==2)
	{
		m_vcPos=D3DXVECTOR3(4,530,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[50].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}
	
	//����� ��ư(���콺�� ���� ���� ��)
	nCheck = RectCheck(vcDelta,14,106,579,610);
	if(nCheck ==1)
	{
		//Ŭ�� �Ͽ��� �� ó�� ����
		//�濡 ����� �ο��� 2�� �ʰ��� ó������ ����
		if(GMAIN->m_GameSceneManager->m_nBlueMemberTotal <2)
		{
			GMAIN->m_MultiPlay->m_CurMyInfo.nComputer=0;
			GMAIN->m_MultiPlay->m_CurMyInfo.nRedTeam= 0;
		}
		return;
	}
	else if(nCheck ==2)
	{
		m_vcPos=D3DXVECTOR3(4,576,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[51].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}
}

INT CCharacterSelect::RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax)
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

//������ ȣ��Ʈ�� ��ǻ�� �÷��̾�� ���� �����Ͽ� ������ ������Ʈ
void CCharacterSelect::MultiPlayComputerCharacterRandomSelect()
{
	GMAIN->m_MultiPlay->m_CurMyInfo.bReadyState=TRUE;

	for(int i=0; i<4; ++i)
	{
		if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[i] == 0)
		{
			GMAIN->m_MultiPlay->m_CurMyHostInfo.nComputer[i] = GMAIN->m_MultiPlay->m_RoomInfo.nComputer[i];
			GMAIN->m_MultiPlay->m_CurMyHostInfo.nCharIndex[i] = GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i];
			GMAIN->m_MultiPlay->m_CurMyHostInfo.nRedTeam[i] = GMAIN->m_MultiPlay->m_RoomInfo.nRedTeam[i];
			if(i==0)
			{
				GMAIN->m_MultiPlay->m_CurMyHostInfo.bReadyState[0]=TRUE;
			}
			else
			{
				GMAIN->m_MultiPlay->m_CurMyHostInfo.bReadyState[i] = GMAIN->m_MultiPlay->m_RoomInfo.bReadyState[i];
			}
		}
	}

	srand((INT)time(NULL));
	BOOL bRandomSlect = TRUE;

	INT HowManyRedRandomMember = 2-GMAIN->m_GameSceneManager->m_nRedMemberTotal;
	INT HowManyBlueRandomMember = 2-GMAIN->m_GameSceneManager->m_nBlueMemberTotal;

	if(HowManyRedRandomMember >0)
	{
		for(int i=0; i<HowManyRedRandomMember; ++i)
		{
			while(bRandomSlect)
			{
				INT nRandom=rand()%8;
				if(MultiPlayMyCharacterSelect(FALSE,nRandom))
				{
					//�ش� �Ǵ� ó��
					GMAIN->m_MultiPlay->m_CurMyHostInfo.nComputer[GMAIN->m_GameSceneManager->m_nRedComputer[i]]=1;
					GMAIN->m_MultiPlay->m_CurMyHostInfo.nCharIndex[GMAIN->m_GameSceneManager->m_nRedComputer[i]]=nRandom;
					GMAIN->m_MultiPlay->m_CurMyHostInfo.nRedTeam[GMAIN->m_GameSceneManager->m_nRedComputer[i]]=1;
					GMAIN->m_MultiPlay->m_CurMyHostInfo.bReadyState[GMAIN->m_GameSceneManager->m_nRedComputer[i]]=TRUE;
					bRandomSlect = FALSE;
				}
			}
			bRandomSlect = TRUE;
		}
	}

	bRandomSlect = TRUE;

	if(HowManyBlueRandomMember >0)
	{
		for(int i=0; i<HowManyBlueRandomMember; ++i)
		{
			while(bRandomSlect)
			{
				INT nRandom = rand()%8;
				if(MultiPlayMyCharacterSelect(FALSE,nRandom))
				{
					//�ش� �Ǵ� ó��
					GMAIN->m_MultiPlay->m_CurMyHostInfo.nComputer[GMAIN->m_GameSceneManager->m_nBlueComputer[i]]=1;
					GMAIN->m_MultiPlay->m_CurMyHostInfo.nCharIndex[GMAIN->m_GameSceneManager->m_nBlueComputer[i]]=nRandom;
					GMAIN->m_MultiPlay->m_CurMyHostInfo.nRedTeam[GMAIN->m_GameSceneManager->m_nBlueComputer[i]]=0;
					GMAIN->m_MultiPlay->m_CurMyHostInfo.bReadyState[GMAIN->m_GameSceneManager->m_nBlueComputer[i]]=TRUE;
					bRandomSlect = FALSE;
				}
			}
			bRandomSlect = TRUE;
		}
	}	
}

//��Ƽ�÷��̽� �ٸ� �÷��̾ ������ ĳ���͸� �����ϰ� ���� ĳ���� ����
BOOL CCharacterSelect::MultiPlayMyCharacterSelect(BOOL bMyRoomPositionCheck, INT nCharacterNumber)
{
	BOOL bSelectOK = TRUE;

	if(bMyRoomPositionCheck)
	{
		for(int i=0; i<4; ++i)
		{
			if(i==GMAIN->m_MultiPlay->m_MyInfo.RoomPosition)
				continue;

			if(GMAIN->m_MultiPlay->m_RoomInfo.nCharIndex[i] == nCharacterNumber)
			{
				bSelectOK = FALSE;
				break;
			}
		}
	}
	else
	{
		for(int i=0; i<4; ++i)
		{
			if(GMAIN->m_MultiPlay->m_CurMyHostInfo.nCharIndex[i] == nCharacterNumber)
			{
				bSelectOK = FALSE;
				break;
			}
		}
	}
	return bSelectOK;
}

BOOL CCharacterSelect::MultiPlayAllPlayerReadyCheck()
{
	BOOL bResult = TRUE;

	for(int i=0; i<4; ++i)
	{

		if(i==0)
			continue;


		if(GMAIN->m_MultiPlay->m_RoomInfo.nComputer[i] ==3)
			continue;


		if(GMAIN->m_MultiPlay->m_RoomInfo.bReadyState[i] ==FALSE)
		{
			bResult = FALSE;

			return bResult;
		}
	}

	return bResult;
}

BOOL CCharacterSelect::MultiPlayEveryBodyReady()
{
	BOOL bResult = TRUE;
	for(int i=0; i<4; ++i)
	{
		if(GMAIN->m_MultiPlay->m_RoomInfo.bReadyState[i]==FALSE)
		{
			bResult = FALSE;
			break;
		}
	}

	//ȣ��Ʈ�� ���� ������ �������� ���ȭ������ �Ѿ
	if(bResult)
	{
		if(m_bEverybodyReady==FALSE)
		{
			m_bOKClicked=TRUE;

			GMAIN->m_pCamMn->m_bJumpAnimationOK = FALSE;

			m_bEverybodyReady = TRUE;
		}
	}

	return bResult;
}