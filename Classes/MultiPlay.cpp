#include "../Include/INCLUDE.h"

CMultiPlay::CMultiPlay(void)
{
	memset(sName,0,sizeof(sName))	;
}

//멀티플레이가 끝나면 관련 변수 초기화!
void CMultiPlay::InitVal()
{
	memset(sName,0,sizeof(sName))	;


	//서버로부터 업데이트되는 인스턴스들을 초기화하기위해
	CLIENTROOMINFO temp_RoomInfo;
	MyPlayInfo temp_MyInfo;

	//클라이언트에서 서버로 업데이트 하는데 사용되는 인스턴스들을 초기화하기 위해
	MyInfoForUpdate	temp_CurMyInfo;			
	MyInfoForUpdate	temp_OldMyInfo;			
	MyHostInfoForUpdate temp_CurMyHostInfo;	
	MyHostInfoForUpdate temp_OldMyHostInfo;	
	MyHostGameScoreForUpdate temp_CurGameScore;
	MyHostGameScoreForUpdate	temp_OldGameScore;

	memcpy(&m_RoomInfo,&temp_RoomInfo,sizeof(CLIENTROOMINFO));
	memcpy(&m_MyInfo,&temp_MyInfo,sizeof(MyPlayInfo));
	memcpy(&m_CurMyInfo,&temp_CurMyInfo,sizeof(MyInfoForUpdate));
	memcpy(&m_OldMyInfo,&temp_OldMyInfo,sizeof(MyInfoForUpdate));
	memcpy(&m_CurMyHostInfo,&temp_CurMyHostInfo,sizeof(MyHostInfoForUpdate));
	memcpy(&m_OldMyHostInfo,&temp_OldMyHostInfo,sizeof(MyHostInfoForUpdate));
	memcpy(&m_CurGameScore,&temp_CurGameScore,sizeof(MyHostGameScoreForUpdate));
	memcpy(&m_OldGameScore,&temp_OldGameScore,sizeof(MyHostGameScoreForUpdate));

}

CMultiPlay::~CMultiPlay(void)
{
}
INT	CMultiPlay::MultiPlayMyPlayUpdate(int State,int RoomNumber,int RoomPosition)
{
	m_MyInfo.State = State;
	m_MyInfo.RoomNumber = RoomNumber;
	m_MyInfo.RoomPosition = RoomPosition;
	
	return S_OK;
}
INT CMultiPlay::MultiPlayRoomUpdate(PACKETDATA*	pPacket)
{
	
	CLIENTROOMINFO*  tempRoomInfo = (CLIENTROOMINFO*)pPacket->DATA;
	
	m_RoomInfo.State = tempRoomInfo->State;
	m_RoomInfo.TotalNumber = tempRoomInfo->TotalNumber;
	memcpy(m_RoomInfo.nRedTeam,tempRoomInfo->nRedTeam,sizeof(m_RoomInfo.nRedTeam));
	memcpy(m_RoomInfo.bReadyState,tempRoomInfo->bReadyState,sizeof(m_RoomInfo.bReadyState));
	memcpy(m_RoomInfo.sCharacterName,tempRoomInfo->sCharacterName,sizeof(m_RoomInfo.sCharacterName));
	memcpy(m_RoomInfo.nComputer,tempRoomInfo->nComputer,sizeof(m_RoomInfo.nComputer));
	memcpy(m_RoomInfo.nCharIndex,tempRoomInfo->nCharIndex,sizeof(m_RoomInfo.nCharIndex));
	m_RoomInfo.nGameScore = tempRoomInfo->nGameScore;
	m_RoomInfo.nSetScore = tempRoomInfo->nSetScore;
	

	//클라이언트 관련 변수 초기화
	if(pPacket->ORDER == CLIENT_ROOM_INIT)
	{
		MultiPlayCharacterSelectInit();
	}

	return S_OK;
}


INT	CMultiPlay::Client_Room_Create_Update(PACKETDATA* pPacket)
{

	CLIENTROOMINFO* tempRoominfo = (CLIENTROOMINFO*)pPacket->DATA;

	// 방정보를 업데이트한다.
	// 방제목 업데이트
	strcpy(GMAIN->m_MultiPlay->m_RoomInfo.sName,tempRoominfo->sName);
	GMAIN->m_MultiPlay->m_RoomInfo.State		= tempRoominfo->State;
	GMAIN->m_MultiPlay->m_RoomInfo.RoomNumber	= tempRoominfo->RoomNumber;
	GMAIN->m_MultiPlay->m_RoomInfo.TotalNumber	= tempRoominfo->TotalNumber;
	strcpy(GMAIN->m_MultiPlay->m_RoomInfo.sCharacterName[0],tempRoominfo->sCharacterName[0]);

	// 개인정보를 업데이트한다.
	GMAIN->m_MultiPlay->m_MyInfo.RoomPosition	= 0;
	GMAIN->m_MultiPlay->m_MyInfo.RoomNumber		= pPacket->ROOMNUMBER;
	GMAIN->m_MultiPlay->m_MyInfo.State			= CLIENT_ROOM;

	return S_OK;
}
INT	CMultiPlay::Client_Room_Join_Update(PACKETDATA* pPacket)
{
	
	return S_OK;
}
INT	CMultiPlay::Client_Room_Reset()
{

	g_pNetAdapter->SendPacket(NULL
							,CLIENT_ROOM
							,CLIENT_ROOM_RESET
							,GMAIN->m_MultiPlay->m_RoomInfo.RoomNumber
							,NULL
							,NULL
							,NULL
							,NULL
							,NULL);

	return S_OK;
}
INT CMultiPlay::Client_Room_Exit()
{

	g_pNetAdapter->SendPacket(NULL
							,CLIENT_ROOM
							,CLIENT_ROOM_EXIT
							,GMAIN->m_MultiPlay->m_RoomInfo.RoomNumber
							,NULL
							,NULL
							,NULL
							,NULL
							,NULL);

	return S_OK;
}

INT CMultiPlay::Client_Room_Game_Start()
{
	
	g_pNetAdapter->SendPacket(NULL
							,CLIENT_ROOM
							,CLIENT_ROOM_GAME_START
							,GMAIN->m_MultiPlay->m_RoomInfo.RoomNumber
							,NULL
							,NULL
							,NULL
							,NULL
							,NULL);
	return S_OK;
}



//멀티플레이 갱신
INT CMultiPlay::MultiPlayFrameMove()
{
	SAFE_FRMOV(	GMAIN->m_pInput	);

	if(m_MyInfo.State == CLIENT_ROOM)
		GMAIN->m_nGamePhase = 5;

	return 0;
}
//멀티플레이 렌더링
void CMultiPlay::MultiPlayRender()
{
	//09-08-13 이재준
	SpriteDraw();
}

//m_OldMyInfo와 m_CurMyInfo의 멤버 변수가 다르다면 서버에 바뀐내용 업데이트!
void CMultiPlay::UpdateToServer()
{
	//호스트 게스트 공통
	if(0!=memcmp(&m_OldMyInfo,&m_CurMyInfo,sizeof(MyInfoForUpdate)))
	{
		//서버로 m_CurMyInfo 구조체를 업데이트함		
		g_pNetAdapter->SendPacket(NULL
								,CLIENT_ROOM
								,CLIENT_ROOM_USER_UPDATE
								,m_MyInfo.RoomNumber
								,NULL
								,NULL
								,NULL
								,sizeof(m_CurMyInfo)
								,&m_CurMyInfo);
		memcpy(&m_OldMyInfo,&m_CurMyInfo,sizeof(MyInfoForUpdate));
	}

	//호스트 일때
	if(m_MyInfo.RoomPosition==0)
	{
		if(0!=memcmp(&m_OldMyHostInfo,&m_CurMyHostInfo,sizeof(MyHostInfoForUpdate)))
		{
			//서버로 m_CurMyHostInfo 구조체를 업데이트함
			g_pNetAdapter->SendPacket(NULL
									,CLIENT_ROOM
									,CLIENT_ROOM_USER_HOST_UPDATE
									,m_MyInfo.RoomNumber
									,NULL
									,CLIENT_ROOM_CREATE_MODEL
									,NULL
									,sizeof(m_CurMyHostInfo)
									,&m_CurMyHostInfo);
								
			memcpy(&m_OldMyHostInfo,&m_CurMyHostInfo,sizeof(MyHostInfoForUpdate));
		}

		if(0!=memcmp(&m_OldGameScore,&m_CurGameScore,sizeof(MyHostGameScoreForUpdate)))
		{
			//서버로 m_CurGameScore 구조체를 업데이트함
			g_pNetAdapter->SendPacket(NULL
									,CLIENT_ROOM
									,CLIENT_ROOM_USER_SCORE_UPDATE
									,m_MyInfo.RoomNumber
									,NULL
									,NULL
									,NULL
									,sizeof(m_CurGameScore)
									,&m_CurGameScore);
			memcpy(&m_OldGameScore,&m_CurGameScore,sizeof(MyHostGameScoreForUpdate));
		}
	}
}

void CMultiPlay::MultiPlayCharacterSelectInit()
{
	if(g_ppPlayers != NULL)
	{
		GMAIN->m_eFlow=READYPLAY;
		g_ppPlayers	= NULL;
		for( int i=0; i<NUMPLAYERS; ++i )
		{
			SAFE_DELETE( GMAIN->m_ppPlayers[i] );
		}
	}

	GMAIN->m_MultiPlay->m_CurMyInfo.nCharIndex=10;
	GMAIN->m_MultiPlay->m_CurMyInfo.nRedTeam=3;
	GMAIN->m_MultiPlay->m_CurMyInfo.bReadyState=FALSE;
	GMAIN->m_MultiPlay->m_CurMyInfo.nComputer=3;

	GMAIN->m_MultiPlay->m_OldMyInfo.nCharIndex=10;
	GMAIN->m_MultiPlay->m_OldMyInfo.nRedTeam=3;
	GMAIN->m_MultiPlay->m_OldMyInfo.bReadyState=FALSE;
	GMAIN->m_MultiPlay->m_OldMyInfo.nComputer=3;

	GMAIN->m_CharacterSelect->m_bAllRandomSelect = FALSE;
	GMAIN->m_CharacterSelect->m_bSelectFinished = FALSE;
	GMAIN->m_CharacterSelect->m_bPlayer1SelectFinished = FALSE;
	GMAIN->m_CharacterSelect->m_bPartnerSelectFinished = FALSE;
	GMAIN->m_CharacterSelect->m_bReady = FALSE;
	GMAIN->m_CharacterSelect->m_bOKClicked = FALSE;

	GMAIN->m_CharacterSelect->m_bEverybodyReady= FALSE;

	GMAIN->m_pCameraAniModel->m_pAniController->JumpAnimationSet(0);
	GMAIN->m_pCamMn->m_bJumpAnimationOK=FALSE;
	GMAIN->m_pCameraAniModel->m_pAniController->m_bAniFinished = FALSE;

	for(int i=0; i<2; ++i){GMAIN->m_GameSceneManager->m_nRedMember[i] = 4;}
	for(int i=0; i<2; ++i){GMAIN->m_GameSceneManager->m_nBlueMember[i] = 4;}
	for(int i=0; i<2; ++i){GMAIN->m_GameSceneManager->m_nRedComputer[i] = 4;}
	for(int i=0; i<2; ++i){GMAIN->m_GameSceneManager->m_nBlueComputer[i] = 4;}
}


void CMultiPlay::SpriteDraw()
{
	int i=0;

	D3DXMATRIX	mtW;

	D3DXVECTOR3 vcDelta = g_pApp->m_pInput->GetMousePos();

	GMAIN->m_pd3dSprite->Begin(D3DXSPRITE_ALPHABLEND);



	//종료 버튼 BOOL 값 TRUE로 변경
	GMAIN->m_vcPos=D3DXVECTOR3(479,5,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[42].m_pTex, &GMAIN->m_rc, NULL, &GMAIN->m_vcPos, D3DXCOLOR(1,1,1,1.f));

	//그만두기 버튼을 클릭하였을 경우
	if(vcDelta.x > GMAIN->m_rcWindowClient.right*482/1024
		&& vcDelta.x < GMAIN->m_rcWindowClient.right*540/1024
		&& vcDelta.y > GMAIN->m_rcWindowClient.bottom*7/768
		&& vcDelta.y < GMAIN->m_rcWindowClient.bottom*26/768)
	{
		if(g_pApp->m_pInput->BtnDown(0))
		{
			GMAIN->m_pMp[2]->Reset();
			GMAIN->m_pMp[2]->Stop();
			GMAIN->m_pMp[2]->Play();
			GMAIN->m_bQuitButton=TRUE;
		}
	}


	if(GMAIN->QuitButtonClickCheck())
	{
		//InitVal();
		ShowWindow(GMAIN->m_NetworkDlg->m_hConnection,SW_HIDE);
	}

	GMAIN->m_pd3dSprite->End();
}
