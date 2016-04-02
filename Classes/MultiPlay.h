//서버로 부터 데이터를 업데이트 받는다. 
struct CLIENTROOMINFO
{
	char sName[PACKET_NAME];					//방이름:
	int  State;								//현재 게임방의 상태
	int  RoomNumber;							//게임방 번호
	int  TotalNumber;							//총 플레이어 수
	int  nRedTeam[4];							//레드 팀일 경우 1, 블루 팀일 경우 0, 선택되지 않았을때 3
	int  nComputer[4];						//각 플레이어에 대해: 컴퓨터 플레이어인가?
	BOOL bReadyState[4];						//각 플레이어에 대해: 상태가 레디인지 아닌지
	char sCharacterName[4][PACKET_NAME];	//각 플레이어의 ID
	int	 nCharIndex[4];						//각 플레이어에 대해: 선택된 캐릭터 번호
	int	 nGameScore;							//최종 몇세트를 이겨야 게임이 끝나는지?
	int	 nSetScore;							//1세트는 몇점을 따야 하는지?

	CLIENTROOMINFO()
	{
		memset(sName,0,sizeof(sName));
		State = ROOM_STATE_EMPTY;
		RoomNumber = ROOM_NONE;
		TotalNumber = 0;
		for(int i=0; i<4; ++i){nRedTeam[i]=3;}
		for(int i=0; i<4; ++i){nComputer[i]=3;}
		memset(bReadyState,0,sizeof(bReadyState));
		memset(sCharacterName,0,sizeof(sCharacterName));
		for(int i=0; i<4; ++i){nCharIndex[i]=10;}
		nGameScore=2;
		nSetScore=3;
	}
};

//딱 한번 서버로 부터 데이터를 업데이트 받아옴
class MyPlayInfo 
{
public:
	int		RoomPosition;				//자리위치: 서버에서 접속한 순서 대로 번호를 발급 받는다. //서버로 부터 업데이트 해야함
	int		State;						//현재 위치 서버와 작업중? 로비? 방? //서버로 부터 업데이트 해야 함
	int		RoomNumber;					//방번호										

	MyPlayInfo()
	{
		RoomPosition = 0				;
		State = CLIENT_DISCONNECT		;
		RoomNumber = ROOM_NONE			;
	}
	~MyPlayInfo(){};
};



/////////////////////////////////////////////////////////////////////////////////



//클라이언트(호스트,게스트)가 서버로 업데이트 할 구조체 (캐릭터/팀 변경)
class MyInfoForUpdate
{
public:
	//호스트 게스트 공통으로 서버로 업데이트 할 변수들
	int		nRedTeam;					//레드 팀일 경우 1, 블루 팀일 경우 0, 선택되지 않았을때 3
	int		nComputer;					//나는 컴퓨터 플레이어인가?
	BOOL	bReadyState;				//나의 상태가 레디인지 아닌지
	int		nCharIndex;					//나의 선택된 캐릭터 번호

	MyInfoForUpdate()
	{
	nRedTeam=3							;
	nComputer=3						;
	bReadyState=FALSE					;
	nCharIndex = 10					;
	}

	~MyInfoForUpdate(){}
};


//클라이언트(호스트)일때 서버로 업데이트 할 구조체
class MyHostInfoForUpdate
{
public:
	//모든 플레이어의 캐릭터 선택이 끝났을 때
	//호스트가 컴퓨터 플레이어의 랜덤 선택된 캐릭터를 서버에 업데이트 한다!
	int  nRedTeam[4];				//레드 팀일 경우 1, 블루 팀일 경우 0, 선택되지 않았을때 3
	int  nComputer[4];			//각 플레이어에 대해: 컴퓨터 플레이어인가?
	int	 nCharIndex[4];			//각 플레이어에 대해: 선택된 캐릭터 번호
	BOOL bReadyState[4];			//각 플레이어에 대해: 상태가 레디인지 아닌지
	MyHostInfoForUpdate()
	{
		for(int i=0; i<4; ++i){nRedTeam[i]=3;}
		for(int i=0; i<4; ++i){nComputer[i]=3;}
		for(int i=0; i<4; ++i){nCharIndex[i]=10;}
		memset(bReadyState,0,sizeof(bReadyState));
	}
	~MyHostInfoForUpdate(){}
};

//클라이언트(호스트)일때 서버로 업데이트 할 게임 스코어 관련 변수들
class MyHostGameScoreForUpdate
{
public:
	int		nGameScore;				//최종 몇세트를 이겨야 게임이 끝나는지?
	int		nSetScore;					//1세트는 몇점을 따야 하는지?

	MyHostGameScoreForUpdate()
	{
		nGameScore=2;
		nSetScore=3;
	}
	~MyHostGameScoreForUpdate(){}
};



class CMultiPlay
{
public:
	char	sName[PACKET_NAME];		//나의 ID	: 서버에 접속하기 전에 입력한 ID가 한번 저장 되도록 한다.


	//서버가 아래의 두 인스턴스의 멤버변수들을 업데이트 해야함
	CLIENTROOMINFO		m_RoomInfo;
	MyPlayInfo			m_MyInfo;

	//클라이언트에서 서버로 업데이트 하는데 사용되는 인스턴스들
	MyInfoForUpdate	m_CurMyInfo;				//호스트/게스트 공통
	MyInfoForUpdate	m_OldMyInfo;				//호스트/게스트 공통
	MyHostInfoForUpdate m_CurMyHostInfo;		//호스트 전용
	MyHostInfoForUpdate m_OldMyHostInfo;		//호스트 전용
	MyHostGameScoreForUpdate m_CurGameScore;	//호스트 전용
	MyHostGameScoreForUpdate	m_OldGameScore;	//호스트 전용



	CMultiPlay(void);
	virtual ~CMultiPlay(void);

	INT	MultiPlayMyPlayUpdate(int State,int RoomNumber,int RoomPosition);
	INT	MultiPlayRoomUpdate(PACKETDATA*	pPacket);		// 방정보 업데이트
	INT	Client_Room_Create_Update(PACKETDATA* pPacket);
	INT	Client_Room_Join_Update(PACKETDATA* pPacket);
	INT	Client_Room_Reset();
	INT Client_Room_Exit();
	INT	Client_Room_Game_Start();
	INT MultiPlayFrameMove();							//멀티플레이 갱신
	void MultiPlayRender();								//멀티플레이 렌더링

	void UpdateToServer();								//m_OldMyInfo와 m_CurMyInfo의 멤버 변수가 다르다면 서버에 바뀐내용 업데이트!

	void InitVal();										//멀티플레이가 끝나면 관련 변수 초기화!

	void MultiPlayCharacterSelectInit();				//호스트가 모든 데이터 초기화를 요청했을때 실행되는 함수

	void SpriteDraw();									//스프라이트 드로우
};