#pragma once

// �游��� ��û�� �⺻������ ���� ����ü
struct ROOMCREATEINFO
{
	char   Room_Name[ROOM_NAME_SIZE] ;   // ����
	char   Room_PW[ROOM_PW_SIZE]  ;   // �� ��й�ȣ
};

struct ROOMDATA
{
	int    Index      ;   // ���ȣ
	int    State      ;   // ���� �����
	int    Number      ;   // ���� �ο���
	int    MAX_NUMBER     ;   // ���ο�
	char   Room_Name[ROOM_NAME_SIZE] ;   // ����
	char   Room_PW[ROOM_PW_SIZE]  ;   // �� ��й�ȣ

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


	// ���� ����� �Լ�
	static unsigned int WINAPI SendThread(LPVOID arg);			// Send������ ó�� ������
	static unsigned int WINAPI WorkThread(LPVOID arg);			// Recv������ ó�� ������
	static unsigned int WINAPI PacketThread(LPVOID arg);			// Work������

	// �α�����
	virtual INT	LogPrint(Signal_Type signal_type, void* pPacket, int Packet_Size);
	virtual INT	LogPrintState( void* pPacket);
	
	// ��Ŷ ó�� �Լ�
	virtual	INT	StateFunction	(PACKETDATA pPacket);			// ���������� ó��? �κ�ó��? ��ó��?�� �з��Ѵ�.
	
	// �ش� EXE�� ó���Ѵ�.
	virtual INT	ClientConnection(PACKETDATA* pPacket);			// ���ӿ�   �ʿ��� ó���� �Ѵ�.
	virtual INT	ClientLobby		(PACKETDATA* pPacket);			// �κ񿡼� �ʿ��� ó���� �Ѵ�.
	virtual INT	ClientRoom		(PACKETDATA* pPacket);			// �뿡��   �ʿ��� ó���� �Ѵ�.
	
	virtual INT	ClientRoomExit(PACKETDATA* pPacket);

	void SendPacket(char   INDEX, char STATE, char EXETYPE
		,char  ROOMNUMBER, char*  NAME,  char ORDER, char DEFINITION
		,BYTE  DATASIZE,   void*  pData );
	
	// ���� ó�����
	virtual INT	ExeFunction(PACKETDATA* packet);
	virtual INT	OrderFunction(PACKETDATA* packet);


};
extern CClient*		g_pNetAdapter;
extern CRingBuffer	SendBuffer;
extern CRingBuffer	RecvBuffer;
