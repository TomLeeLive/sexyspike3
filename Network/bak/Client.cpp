#include "../Include/INCLUDE.h"

CClient* g_pNetAdapter;
extern CNetworkDlg* g_pNetworkDlg;


CRITICAL_SECTION		cs_Send;
CRITICAL_SECTION		cs_Recv;
CRITICAL_SECTION		cs_File;

int				g_NetworkState;					// 클라이언트 사용하는지 확인
CRingBuffer		SendBuffer = CRingBuffer(MAX_PACKET);
CRingBuffer		RecvBuffer = CRingBuffer(MAX_PACKET);


int SendThreadState = THREAD_RESUME;
int WorkThreadState = THREAD_RESUME;
//int PacketThreadState = THREAD_RESUME;

HANDLE		g_hSendThread	;
HANDLE		g_hWorkThread	;
//HANDLE		g_hPacketThread	;
WSAEVENT		hEvent;

CClient::CClient()
{

	g_pNetAdapter	= this	;
	m_Listen_Sock	= NULL	;

	m_hClientThread	= NULL	;

	ZeroMemory( &m_serveraddr,sizeof(SOCKADDR_IN) );

	

}

CClient::~CClient()
{
	ClientStop();
}


INT CClient::ClientStart(char* IPAddress)
{
	// 로그파일 설정
	time_t sTime;
	tm*		pTime;
	time(&sTime);
	pTime = localtime(&sTime);

	char	LogFile[512] = {0};

	sprintf_s(LogFile,"./LogFiles/%d-%d-%d_%d-%d-%d.csv"
		,pTime->tm_year
		,pTime->tm_min
		,pTime->tm_mday
		,pTime->tm_hour
		,pTime->tm_min
		,pTime->tm_sec
		);

	LogSet(LogFile);
	
	InitializeCriticalSection( &cs_Recv );
	InitializeCriticalSection( &cs_Send );
	InitializeCriticalSection( &cs_File );

	int retval;

	// 윈속 초기화
	if( WSAStartup(MAKEWORD(2,2), &m_Wsa) != 0 ) { return -1; }

	// socket()
	m_Listen_Sock = socket(AF_INET, SOCK_STREAM, 0 );
	if( m_Listen_Sock == INVALID_SOCKET) { g_pNetAdapter->err_quit("socket()"); }

	BOOL optval = TRUE; // Nagle 알고리즘작동중지
	if(setsockopt(m_Listen_Sock, IPPROTO_TCP, TCP_NODELAY,
		(char *)&optval, sizeof(optval)) == SOCKET_ERROR)
	{
		g_pNetAdapter->err_quit("setsockopt()");
	}

	// WSACreate
	hEvent = WSACreateEvent();
	retval = WSAEventSelect( m_Listen_Sock, hEvent, FD_READ |FD_WRITE|FD_CONNECT|FD_CLOSE|FD_OOB);

	if(retval == SOCKET_ERROR) { g_pNetAdapter->err_quit("WSAEventSelect()"); }

	// SOCKADDR_IN
	m_serveraddr.sin_family = AF_INET;
	m_serveraddr.sin_port = htons(9000);
	m_serveraddr.sin_addr.s_addr = inet_addr(IPAddress);
	retval = connect(m_Listen_Sock,(SOCKADDR*)&m_serveraddr,sizeof(SOCKADDR_IN));

	if(retval == SOCKET_ERROR)
	{ 
		retval = WSAGetLastError();
		if(retval != WSAEWOULDBLOCK)
		{
			g_pNetAdapter->err_quit("connect()"); 
			closesocket(m_Listen_Sock);
		}

	}


	UINT		SendThreadID;
	UINT		WorkThreadID;
	//UINT		PacketThreadID;

	SendThreadState = THREAD_RESUME;
	WorkThreadState = THREAD_RESUME;
	
	g_hSendThread = (HANDLE)_beginthreadex(NULL,0,SendThread,&m_Listen_Sock,0,&SendThreadID);
	g_hWorkThread = (HANDLE)_beginthreadex(NULL,0,WorkThread,&m_Listen_Sock,0,&WorkThreadID);
	//g_hPacketThread = (HANDLE)_beginthreadex(NULL,0,PacketThread,&m_Listen_Sock,0,&PacketThreadID);

	

	return S_OK;
}

INT CClient::ClientStop()
{
	
	if(g_NetworkState != CLIENT_CONNECT)
	{
		return -1;
	}

	g_NetworkState	= CLIENT_DISCONNECT;
	SendThreadState = THREAD_EXIT;
	WorkThreadState = THREAD_EXIT;
	//PakcetThreadState = THREAD_EXIT;

	Sleep(300);

	DeleteCriticalSection( &cs_Send );
	DeleteCriticalSection( &cs_Recv );
	DeleteCriticalSection( &cs_File );

	WSACleanup();
	closesocket(m_Listen_Sock);
	CloseHandle(g_hSendThread);
	CloseHandle(g_hWorkThread);
	//CloseHandle(g_hPacketThread);

	return S_OK;
}

void CClient::err_quit(char* msg)
{
	LPVOID	lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
		,NULL
		,WSAGetLastError()
		,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT)
		,(LPTSTR)&lpMsgBuf
		,0
		,NULL
		);

	MessageBox(NULL,(LPCTSTR)lpMsgBuf,msg,MB_ICONERROR);
	LocalFree(lpMsgBuf);
	//exit(-1);
}
void CClient::err_display(char* msg)
{
	LPVOID	lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
		,NULL
		,WSAGetLastError()
		,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT)
		,(LPTSTR)&lpMsgBuf
		,0
		,NULL
		);

	MessageBox(NULL,(LPCTSTR)lpMsgBuf,msg,MB_ICONERROR);
	LocalFree(lpMsgBuf);
	//printf("%s\n",lpMsgBuf);
	//exit(-1);
}


unsigned int WINAPI CClient::SendThread(LPVOID arg)
{
	
	int tTime=0;
	int retval;
	SOCKET* sock = (SOCKET*)arg;

	static int c=0;

	while(1)
	{

		if(SendBuffer.GetWidth() > 0)
		{
			int iSize=0;
			char sBuf[MAX_PACKET] ={0};
			EnterCriticalSection( &cs_Send );	
			SendBuffer.DeRingBuffer(sBuf,&iSize);
			LeaveCriticalSection(  &cs_Send );

			// 링버퍼에서 꺼낸 데이터가 없다면
			if( iSize == 0)
			{
				continue;
			}
			
			
			while(1)
			{
				// 전송실패하면 다시 보낸다.	
				retval = send(*sock,sBuf,iSize,0);
				if(retval == SOCKET_ERROR)
				{
					g_pNetAdapter->err_display("send()");
					continue;
				}
				break;
			}

			// 패킷복호화해서 로그파일에 출력하도록 한다.
			g_pNetAdapter->PacketDecrypt((BYTE*)sBuf,iSize);

			EnterCriticalSection( &cs_File);
			g_pNetAdapter->LogPrintState(sBuf);
			LeaveCriticalSection( &cs_File);
			//전송이 실패했을 경우 제 전송한다.
			
		}
		
		

		/*if(tTime > THREAD_WAIT_TIME)
		{
		SendThreadState = THREAD_SUSPEND;
		SuspendThread(g_hSendThread);
		}*/
		
		if( SendThreadState == THREAD_EXIT )
		{
			return S_OK;
		}

	}
	
	return S_OK;

}
unsigned int WINAPI CClient::WorkThread(LPVOID arg)
{
	int retval;
	SOCKET* sock = (SOCKET*)arg;
	
	while(1)
	{
		
		if( WorkThreadState == THREAD_EXIT )
		{
			return S_OK;
		}
		
		WSANETWORKEVENTS		hNetwork;
		retval = WSAWaitForMultipleEvents(1,&hEvent,FALSE,INFINITE,FALSE);
		if(retval == WSA_WAIT_FAILED)
		{
			g_pNetAdapter->err_quit("");
			continue;
		}

		retval = WSAEnumNetworkEvents(*sock,hEvent,&hNetwork);

		if( hNetwork.lNetworkEvents & FD_CONNECT )
		{
			if(hNetwork.iErrorCode[FD_CONNECT_BIT] != 0)
			{
				WorkThreadState = THREAD_EXIT;
				ShowWindow(g_pApp->m_NetworkDlg->m_hWait,SW_HIDE);
				g_pNetAdapter->err_quit("FD_CONNECT");
				return S_OK;
			}
			else
			{
				// 네트워크 상태 업데이트
				g_NetworkState = CLIENT_CONNECT;
				// 접속이 완료 되면 로비(추부 변경:로그인화면)으로 전환한다.
				
			}
		}

		
		if( hNetwork.lNetworkEvents & FD_READ )
		{
			if(hNetwork.iErrorCode[FD_READ_BIT] != 0)
			{
				g_pNetAdapter->err_quit("FD_READ");
			}
			else
			{
				
				char sBuf[MAX_PACKET] = {0};
				int Size=0;
				retval = recv(*sock,sBuf,sizeof(sBuf),0);

				if(retval == SOCKET_ERROR)
				{
					//g_pNetAdapter->err_display("recv()");
					continue;
				}

				// 패킷 복호화
				Size = sBuf[0];
				g_pNetAdapter->PacketDecrypt((BYTE*)sBuf,(BYTE)sBuf[0]);
				
				PACKETDATA	pp;
				memcpy(&pp,sBuf,sizeof(PACKETDATA));
				
				//// 링버퍼에 넣는다
				//EnterCriticalSection( &cs_Recv );
				//RecvBuffer.EnRingBuffer(sBuf,retval);
				//LeaveCriticalSection( &cs_Recv );
				
				// 패킷을 처리한다.
				g_pNetAdapter->StateFunction(pp);
				
				// 로그를 출력한다.
				EnterCriticalSection( &cs_File );
				//g_pNetAdapter->LogPrint(PACKET_RECV,sBuf,Size);
				LeaveCriticalSection( &cs_File );


			}
		}

		if(hNetwork.lNetworkEvents & FD_CLOSE)
		{
			if(hNetwork.iErrorCode[FD_CLOSE_BIT] != 0)
			{
				// 상대방이 접속을 끊으면 네트워크를 종료시킨다.
				g_pNetAdapter->ClientStop();
				//g_pNetAdapter->err_quit("FD_CLOSE");
			}
			else
			{
				return S_OK;
			}
		}

	}

	return S_OK;
}

unsigned int WINAPI CClient::PacketThread(LPVOID arg)
{
	
	while(1)
	{
		if( RecvBuffer.GetWidth() > 0 )
		{
			char sBuf[PACKET_TOTAL]={0};
			int  iSize;
			PACKETDATA		Pakcet;

			EnterCriticalSection( &cs_Recv );
			RecvBuffer.DeRingBuffer(sBuf,&iSize);
			LeaveCriticalSection( &cs_Recv );	
			
			// 암호를 해석한다.
			g_pNetAdapter->PacketEnCrypt((BYTE*)sBuf,sBuf[0]);
			
			// 패킷을 분해한다
			g_pNetAdapter->PacketPieces(&Pakcet,sBuf,iSize);

			// 패킷을 꺼내서 처리한다.
			g_pNetAdapter->StateFunction(Pakcet);
		
		}

		if(WorkThreadState == THREAD_EXIT)
		{
			return S_OK;
		}
	}
	
	return S_OK;
}

INT	CClient::StateFunction(PACKETDATA Packet)
{


	switch(Packet.STATE)
	{
		case CLIENT_DISCONNECT:					// 사용안함
			break;
		case CLIENT_CONNECT:					// 서버내에서 처리
			ClientConnection(&Packet);
			break;
		case CLIENT_LOBBY:						// 로비내에서 처리
			ClientLobby(&Packet);
			break;
		case CLIENT_ROOM:						// 룸에서 처리
			ClientRoom(&Packet);
			break;
	}
	return S_OK;

}

INT	CClient::ClientConnection(PACKETDATA* pPacket)
{
	// 패킷을 만들 변수를 만든다.
	BYTE	sBuf[PACKET_TOTAL] = {0};
	int		retval = 0;

	switch(pPacket->EXETYPE)
	{
	case CLIENT_CONNECT_NONE:										// 서버에 접속을 성공하면 다음 화면으로 넘어가라고 한다.
		break;
	case CLIENT_CONNECT_LOGIN:
		break;

	}
	
	return S_OK;
}


INT	CClient::ClientLobby(PACKETDATA* pPacket)
{
	// 패킷을 만들 변수를 만든다.
	

	switch(pPacket->EXETYPE)
	{
	case 0:							// 사용안함
		break;
	case CLIENT_LOBBY_SHOW:			// 로비접속하기 (클라이언트만 처리)
		g_pNetworkDlg->Client_Lobby_Show();	
		break;
	case CLIENT_LOBBY_ROOM_LIST:	// 방접속목록 요청하기
		break;
	case CLIENT_LOBBY_ROOM_INSERT:	// 방목록추가
		g_pNetworkDlg->CLIENT_LOBBY_ROOM_INSERT(pPacket);
		break;
	case CLIENT_LOBBY_ROOM_DELETE:	// 방목록삭제
		break;
	case CLIENT_LOBBY_ROOM_JOIN:	// 방접속하기
		break;
	case CLIENT_LOBBY_CHAT:			// 로비에서 채팅
		break;
	}


	return S_OK;
}


INT	CClient::ClientRoom(PACKETDATA* pPacket)
{

	switch(pPacket->EXETYPE)		
	{
		case CLIENT_ROOM_SHOW:
			{
				g_pNetworkDlg->CLIENT_ROOM_SHOW();
				if(pPacket->ORDER == CLIENT_ROOM_CREATE_UPDATE)
				{
					GMAIN->m_MultiPlay->Client_Room_Create_Update(pPacket);
				}

				if(pPacket->ORDER == CLIENT_ROOM_UPDATE)
				{
					GMAIN->m_MultiPlay->MultiPlayMyPlayUpdate(CLIENT_ROOM,pPacket->ROOMNUMBER,pPacket->DEFINITION);
				}
				break;
			}
		case CLIENT_ROOM_CREATE_UPDATE:
			break;
		case CLIENT_ROOM_UPDATE:
			{
				GMAIN->m_MultiPlay->MultiPlayRoomUpdate(pPacket);
				break;
			}
		case CLIENT_ROOM_USER_UPDATE:
			break;
		case CLIENT_ROOM_USER_HOST_UPDATE:
			break;
		case CLIENT_ROOM_USER_SCORE_UPDATE:
			break;
		case CLIENT_ROOM_DELETE:
			{
				GMAIN->m_MultiPlay->Client_Room_Reset();
				break;
			}
		case CLIENT_ROOM_EXIT:
			break;
		case CLIENT_ROOM_POSITION_RESET:
			break;
		case CLIENT_ROOM_PLAY:
			break;
		case CLIENT_ROOM_RESET:
			break;
		case CLIENT_ROOM_INIT:
			break;
		case CLIENT_ROOM_CREATE_MODEL:
			{
				GMAIN->ReadyPlay();
				break;
			}
		default:
			break;

	}
	
	return S_OK;
}

INT	CClient::ClientRoomExit(PACKETDATA* pPacket)
{
	// 나가기 요청
	


	return S_OK;
}

INT CClient::ExeFunction(PACKETDATA* pPacket)
{
	// 패킷이 공지or에러 처리인지 채팅인지 데이터부분인지 먼저확인안다.
	// 데이터 관련부분은 OrderFunction()함수를 사용해서 처리한다.

	PACKETDATA		Packet;
	memcpy(&Packet,(PACKETDATA*)pPacket,sizeof(PACKETDATA));

	switch(Packet.EXETYPE)
	{
	case 0:	// 사용안함
		break;
	case 1:	// 공지or에러관련 부분
		break;
	case 2:	// 채팅관련부분
		break;
	case 3:	// 데이터에 관한 명령어부분 함수를 이용해서 따로 처리한다
		OrderFunction(&Packet);
		break;
	}
	
	return S_OK;
}
INT CClient::OrderFunction(PACKETDATA* pPacket)
{
	
	switch(pPacket->ORDER)
	{
	case 0:
		break;
	case 1:	// 등록
		break;
	case 2:	// 삭제
		break;
	case 3:	// 업데이트
		{
			/*switch( pPacket->DEFINITION )
			{
			case	C_TRANSLATION:
				{
					((CPlayer*)g_mapNetObjects[pPacket->NAME])->Translation( pPacket->DATA );
					break;
				}
			case	C_ROTATION:
				{
					((CPlayer*)g_mapNetObjects[pPacket->NAME])->SetRotation( pPacket->DATA );
					break;
				}
			case	C_ANIMATION:
				{
					((CPlayer*)g_mapNetObjects[pPacket->NAME])->SetAnimationSet( pPacket->DATA );
					break;
				}
			case	C_MACRO:
				{
					((CPlayer*)g_mapNetObjects[pPacket->NAME])->SetMacro( pPacket->DATASIZE, pPacket->DATA );
					break;
				}
			case	B_UPDATE:
				{
					((CBall*)g_mapNetObjects[pPacket->NAME])->UpdateBall( pPacket->DATA );
				}
			}*/
		}
	}

	return S_OK;
}

INT	CClient::LogPrint(Signal_Type signal_type, void* pPacket, int Packet_Size)
{

	//if(!pPacket)
	//{
	//	return -1;
	//}

	//// 임시로 패킷을 복사한다
	//PACKETDATA			TempPacket;
	//memset(&TempPacket,0,sizeof(PACKETDATA));
	//memcpy(&TempPacket,(PACKETDATA*)pPacket,sizeof(PACKETDATA));

	//// 현재 시간구하기
	//time_t present_time;
	//tm* ptime;
	//time(&present_time);
	//ptime = localtime(&present_time);


	//char state[8]={0};

	//switch(signal_type)
	//{
	//	case PACKET_RECV:
	//		strcpy(state,"Recv");
	//		break;
	//	case PACKET_SEND:
	//		strcpy(state,"Send");
	//		break;
	//}


	//switch(TempPacket.DEFINITION)
	//{
	//	case C_TRANSLATION :
	//	{
	//		float Data[4]={0};
	//		memcpy(&Data,TempPacket.DATA,TempPacket.DATASIZE);
	//		printf("\n[ %d패킷] [%s] [%13s] [ %s ] [ %d:%d:%d ] [_41: %.2f  _42: %.2f  _43: %.2f  _44: %.2f]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data[0]
	//				,Data[1]
	//				,Data[2]
	//				,Data[3]
	//			);
	//		fprintf(m_fp
	//				,"\n[ %d패킷],[%s],[%13s],[ %s ],[ %d:%d:%d ],[_41: %.2f  _42: %.2f  _43: %.2f  _44: %.2f]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data[0]
	//				,Data[1]
	//				,Data[2]
	//				,Data[3]
	//			);
	//		break;
	//	}	
	//	case C_ROTATION :
	//	{
	//		float Data = 0;
	//		memcpy(&Data,TempPacket.DATA,TempPacket.DATASIZE);

	//		printf("\n[ %d패킷] [%s] [%13s] [ %s ] [ %d:%d:%d ] [%.2f]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data
	//			);
	//		fprintf(m_fp
	//				,"\n[ %d패킷],[%s],[%13s],[ %s ],[%s],[ %d:%d:%d ],[%.2f]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data
	//		);

	//		break;
	//	}
	//	case C_ANIMATION :
	//	{
	//		char Data[PACKET_DATA]={0};
	//		memcpy(Data,TempPacket.DATA,TempPacket.DATASIZE);
	//		int indx = atoi(Data);
	//		printf("\n[ %d패킷] [%s] [%13s] [ %s ] [ %d:%d:%d ] [%12s]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,g_arrBehaviorName[indx]
	//		);
	//		fprintf(m_fp
	//				,"\n[ %d패킷],[%s],[%13s],[ %s ],[ %d:%d:%d ],[%12s]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,g_arrBehaviorName[indx]
	//			);
	//		break;
	//	}
	//	case C_MACRO :
	//	{
	//		ValleyActionMacro	Data;
	//		memset(&Data,0,sizeof(ValleyActionMacro));
	//		memcpy(&Data,TempPacket.DATA,TempPacket.DATASIZE);

	//		printf("\n[ %d패킷] [%s] [%13s] [ %s ] [ %d:%d:%d ] [ ET: %.2f] [ TP: %.2f] [ vX: %.2f   vY: %.2f   vZ: %.2f] [ dX: %.2f  dY: %.2f  dZ: %.2f] [MAt: %12s] [ Vat: %12s]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data.ElapsedTime
	//				,Data.TransitionPoint
	//				,Data.Velocity.x
	//				,Data.Velocity.y
	//				,Data.Velocity.z
	//				,Data.Destination.x
	//				,Data.Destination.y
	//				,Data.Destination.z
	//				,g_arrBehaviorName[Data.MovingAction]
	//				,g_arrBehaviorName[Data.ValleyAction]
	//			);
	//		fprintf(m_fp
	//				,"\n[ %d패킷],[%s],[%13s],[ %s ],[ %d:%d:%d ],[ ET: %.2f],[ TP: %.2f],[ vX: %.2f   vY: %.2f   vZ: %.2f],[ dX: %.2f  dY: %.2f  dZ: %.2f],[MAt: %12s],[ Vat: %12s]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data.ElapsedTime
	//				,Data.TransitionPoint
	//				,Data.Velocity.x
	//				,Data.Velocity.y
	//				,Data.Velocity.z
	//				,Data.Destination.x
	//				,Data.Destination.y
	//				,Data.Destination.z
	//				,g_arrBehaviorName[Data.MovingAction]
	//				,g_arrBehaviorName[Data.ValleyAction]
	//			);
	//		break;
	//	}
	//	case B_UPDATE :
	//	{
	//		PacketBall	Data;
	//		memset(&Data,0,sizeof(PacketBall));
	//		memcpy(&Data,TempPacket.DATA,TempPacket.DATASIZE);

	//		printf("\n[ %d패킷] [%s] [%13s] [ %s ] [ %d:%d:%d ] [x:%.2f  y:%.2f  z:%.2f] [vX: %.2f  vY: %.2f  vZ: %.2f] [aX: %.2f  aY: %.2f  aZ: %.2f]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data.POSITION.x
	//				,Data.POSITION.y
	//				,Data.POSITION.z
	//				,Data.VELOCITY.x
	//				,Data.VELOCITY.y
	//				,Data.VELOCITY.z
	//				,Data.AXIS.x
	//				,Data.AXIS.y
	//				,Data.AXIS.z
	//			);
	//		fprintf(m_fp
	//				,"\n[ %d패킷],[%s],[%13s],[ %s ],[ %d:%d:%d ],[x:%.2f  y:%.2f  z:%.2f],[vX: %.2f  vY: %.2f  vZ: %.2f],[aX: %.2f  aY: %.2f  aZ: %.2f]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data.POSITION.x
	//				,Data.POSITION.y
	//				,Data.POSITION.z
	//				,Data.VELOCITY.x
	//				,Data.VELOCITY.y
	//				,Data.VELOCITY.z
	//				,Data.AXIS.x
	//				,Data.AXIS.y
	//				,Data.AXIS.z
	//			);
	//		break;
	//	}
	//	case B_MACRO :
	//	{
	//		BallMacro	Data;
	//		memset(&Data,0,sizeof(BallMacro));
	//		memcpy(&Data,TempPacket.DATA,TempPacket.DATASIZE);

	//		printf("\n[ %d패킷] [%s] [%13s] [ %s ] [ %d:%d:%d ] [ET: %.2f] [MT: %.2f] [TP: %.2f] [TvX: %.2f  TvY: %.2f  TvZ: %.2f]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data.ElapsedTime
	//				,Data.MacroTime
	//				,Data.TransitionPoint
	//				,Data.TempVelocity.x
	//				,Data.TempVelocity.y
	//				,Data.TempVelocity.z
	//		);
	//		fprintf(m_fp
	//				,"\n[ %d패킷],[%s],[%13s],[ %s ],[ %d:%d:%d ],[ET: %.2f],[MT: %.2f],[TP: %.2f],[TvX: %.2f  TvY: %.2f  TvZ: %.2f]"
	//				,TempPacket.INDEX
	//				,state
	//				,g_arreDefinitionName[TempPacket.DEFINITION]
	//				,TempPacket.NAME
	//				,ptime->tm_hour
	//				,ptime->tm_min
	//				,ptime->tm_sec
	//				,Data.ElapsedTime
	//				,Data.MacroTime
	//				,Data.TransitionPoint
	//				,Data.TempVelocity.x
	//				,Data.TempVelocity.y
	//				,Data.TempVelocity.z
	//			);
	//		break;
	//	}
	//}

	return S_OK;
}


INT	CClient::LogPrintState( void* pPacket)
{
	PACKETDATA  Packet;
	memcpy(&Packet,(PACKETDATA*)pPacket,sizeof(PACKETDATA));
	
	fprintf(m_fp,"\nState:%d,EXE:%d",Packet.STATE,Packet.EXETYPE);
	
	return S_OK;
}

void CClient::SendPacket( char   INDEX, char STATE, char EXETYPE
				 ,char  ROOMNUMBER, char*  NAME,  char ORDER, char DEFINITION
				 ,BYTE  DATASIZE,   void*  pData )
{
	BYTE Pakcet[PACKET_TOTAL]={0};
	
	PacketAssemble(Pakcet,INDEX,STATE, EXETYPE,  ROOMNUMBER,NAME,ORDER, DEFINITION,DATASIZE,  pData );
	PacketEnCrypt(Pakcet,(BYTE)Pakcet[0]);
	EnterCriticalSection( &cs_Send );
	SendBuffer.EnRingBuffer(Pakcet,(BYTE)Pakcet[0]);
	LeaveCriticalSection( &cs_Send );

}