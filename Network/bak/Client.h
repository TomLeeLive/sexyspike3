#pragma once

// 방만들기 요청시 기본적으로 받을 구조체
struct ROOMCREATEINFO
{
	char   Room_Name[ROOM_NAME_SIZE] ;   // 방제
	char   Room_PW[ROOM_PW_SIZE]  ;   // 방 비밀번호
};

struct ROOMDATA
{
	int    Index      ;   // 방번호
	int    State      ;   // 현재 방상태
	int    Number      ;   // 접속 인원수
	int    MAX_NUMBER     ;   // 총인원
	char   Room_Name[ROOM_NAME_SIZE] ;   // 방제
	char   Room_PW[ROOM_PW_SIZE]  ;   // 방 비밀번호

	ROOMDATA()
	{
		Index = ROOM_NONE;
		State = ROOM_STATE_EMPTY;
		Number = 0;
		memset(Room_Name,0,ROOM_NAME_SIZE);
		memset(Room_PW,  0,ROOM_PW_SIZE);
	}

	ROOMDATA(int _RoomNumber,int _State, int _Number, int _Max_Number, char* _Room_Name, char* _Room_PW )
	{
		Index	= _RoomNumber;
		State	= _State;
		Number	= _Number;
		MAX_NUMBER = _Max_Number;
		strcpy(Room_Name,_Room_Name);
		if(_Room_PW){strcpy(Room_PW,_Room_PW);}
		
	}
};

class CNetworkInfo
{
public:
	SOCKET*				socket;
	WSAEVENT*			hEvent;
};
class CClient : public CPacket
{
protected:
	WSADATA				m_Wsa			;
	SOCKET				m_Listen_Sock	;
	SOCKADDR_IN 		m_serveraddr	;

	
	HANDLE				m_hClientThread	;
	DWORD				m_ClientThreadId;

			
public:

	CClient();
	~CClient();

	virtual INT			ClientStart(char* IPAddress)			;
	virtual INT			ClientStop()			;

	virtual void		err_quit(char* msg)		;
	virtual void		err_display(char* msg)	;


	// 소켓 입출력 함수
	static unsigned int WINAPI SendThread(LPVOID arg);			// Send링버퍼 처리 스레드
	static unsigned int WINAPI WorkThread(LPVOID arg);			// Recv링버퍼 처리 스레드
	static unsigned int WINAPI PacketThread(LPVOID arg);			// Work스레드

	// 로그파일
	virtual INT	LogPrint(Signal_Type signal_type, void* pPacket, int Packet_Size);
	virtual INT	LogPrintState( void* pPacket);
	
	// 패킷 처리 함수
	virtual	INT	StateFunction	(PACKETDATA pPacket);			// 서버내에서 처리? 로비처리? 룸처리?를 분류한다.
	
	// 해당 EXE를 처리한다.
	virtual INT	ClientConnection(PACKETDATA* pPacket);			// 접속에   필요한 처리를 한다.
	virtual INT	ClientLobby		(PACKETDATA* pPacket);			// 로비에서 필요한 처리를 한다.
	virtual INT	ClientRoom		(PACKETDATA* pPacket);			// 룸에서   필요한 처리를 한다.
	
	virtual INT	ClientRoomExit(PACKETDATA* pPacket);

	void SendPacket(char   INDEX, char STATE, char EXETYPE
		,char  ROOMNUMBER, char*  NAME,  char ORDER, char DEFINITION
		,BYTE  DATASIZE,   void*  pData );
	
	// 예전 처리방법
	virtual INT	ExeFunction(PACKETDATA* packet);
	virtual INT	OrderFunction(PACKETDATA* packet);


};
extern CClient*		g_pNetAdapter;
extern CRingBuffer	SendBuffer;
extern CRingBuffer	RecvBuffer;
