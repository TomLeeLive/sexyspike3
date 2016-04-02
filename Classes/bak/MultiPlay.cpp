#include "../Include/INCLUDE.h"

CMultiPlay::CMultiPlay(void)
{
	memset(sName,0,sizeof(sName))	;
}

//멀티플레이가 끝나면 관련 변수 초기화!
void CMultiPlay::InitVal()
{
	memset(sName,0,sizeof(sName))	;
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
		GMAIN->m_MultiPlay->m_CurMyInfo.nCharIndex=10;
		GMAIN->m_MultiPlay->m_CurMyInfo.nRedTeam=3;
		GMAIN->m_MultiPlay->m_CurMyInfo.bReadyState=FALSE;
		GMAIN->m_MultiPlay->m_CurMyInfo.nComputer=3;

		GMAIN->m_MultiPlay->m_OldMyInfo.nCharIndex=10;
		GMAIN->m_MultiPlay->m_OldMyInfo.nRedTeam=3;
		GMAIN->m_MultiPlay->m_OldMyInfo.bReadyState=FALSE;
		GMAIN->m_MultiPlay->m_OldMyInfo.nComputer=3;

		GMAIN->m_CharacterSelect->m_bPlayer1SelectFinished = FALSE;
		GMAIN->m_CharacterSelect->m_bSelectFinished = FALSE;
		GMAIN->m_CharacterSelect->m_bReady = FALSE;

		GMAIN->m_pCamMn->m_bJumpAnimationOK=FALSE;
		GMAIN->m_pCameraAniModel->m_pAniController->m_bAniFinished = FALSE;

		for(int i=0; i<2; ++i){GMAIN->m_GameSceneManager->m_nRedMember[i] = 4;}
		for(int i=0; i<2; ++i){GMAIN->m_GameSceneManager->m_nBlueMember[i] = 4;}
		for(int i=0; i<2; ++i){GMAIN->m_GameSceneManager->m_nRedComputer[i] = 4;}
		for(int i=0; i<2; ++i){GMAIN->m_GameSceneManager->m_nBlueComputer[i] = 4;}


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
//멀티플레이 갱신
INT CMultiPlay::MultiPlayFrameMove()
{
	if(m_MyInfo.State == CLIENT_ROOM)
		GMAIN->m_nGamePhase = 5;

	return 0;
}
//멀티플레이 렌더링
void CMultiPlay::MultiPlayRender(){}

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
