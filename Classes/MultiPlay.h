//������ ���� �����͸� ������Ʈ �޴´�. 
struct CLIENTROOMINFO
{
	char sName[PACKET_NAME];					//���̸�:
	int  State;								//���� ���ӹ��� ����
	int  RoomNumber;							//���ӹ� ��ȣ
	int  TotalNumber;							//�� �÷��̾� ��
	int  nRedTeam[4];							//���� ���� ��� 1, ��� ���� ��� 0, ���õ��� �ʾ����� 3
	int  nComputer[4];						//�� �÷��̾ ����: ��ǻ�� �÷��̾��ΰ�?
	BOOL bReadyState[4];						//�� �÷��̾ ����: ���°� �������� �ƴ���
	char sCharacterName[4][PACKET_NAME];	//�� �÷��̾��� ID
	int	 nCharIndex[4];						//�� �÷��̾ ����: ���õ� ĳ���� ��ȣ
	int	 nGameScore;							//���� �Ʈ�� �̰ܾ� ������ ��������?
	int	 nSetScore;							//1��Ʈ�� ������ ���� �ϴ���?

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

//�� �ѹ� ������ ���� �����͸� ������Ʈ �޾ƿ�
class MyPlayInfo 
{
public:
	int		RoomPosition;				//�ڸ���ġ: �������� ������ ���� ��� ��ȣ�� �߱� �޴´�. //������ ���� ������Ʈ �ؾ���
	int		State;						//���� ��ġ ������ �۾���? �κ�? ��? //������ ���� ������Ʈ �ؾ� ��
	int		RoomNumber;					//���ȣ										

	MyPlayInfo()
	{
		RoomPosition = 0				;
		State = CLIENT_DISCONNECT		;
		RoomNumber = ROOM_NONE			;
	}
	~MyPlayInfo(){};
};



/////////////////////////////////////////////////////////////////////////////////



//Ŭ���̾�Ʈ(ȣ��Ʈ,�Խ�Ʈ)�� ������ ������Ʈ �� ����ü (ĳ����/�� ����)
class MyInfoForUpdate
{
public:
	//ȣ��Ʈ �Խ�Ʈ �������� ������ ������Ʈ �� ������
	int		nRedTeam;					//���� ���� ��� 1, ��� ���� ��� 0, ���õ��� �ʾ����� 3
	int		nComputer;					//���� ��ǻ�� �÷��̾��ΰ�?
	BOOL	bReadyState;				//���� ���°� �������� �ƴ���
	int		nCharIndex;					//���� ���õ� ĳ���� ��ȣ

	MyInfoForUpdate()
	{
	nRedTeam=3							;
	nComputer=3						;
	bReadyState=FALSE					;
	nCharIndex = 10					;
	}

	~MyInfoForUpdate(){}
};


//Ŭ���̾�Ʈ(ȣ��Ʈ)�϶� ������ ������Ʈ �� ����ü
class MyHostInfoForUpdate
{
public:
	//��� �÷��̾��� ĳ���� ������ ������ ��
	//ȣ��Ʈ�� ��ǻ�� �÷��̾��� ���� ���õ� ĳ���͸� ������ ������Ʈ �Ѵ�!
	int  nRedTeam[4];				//���� ���� ��� 1, ��� ���� ��� 0, ���õ��� �ʾ����� 3
	int  nComputer[4];			//�� �÷��̾ ����: ��ǻ�� �÷��̾��ΰ�?
	int	 nCharIndex[4];			//�� �÷��̾ ����: ���õ� ĳ���� ��ȣ
	BOOL bReadyState[4];			//�� �÷��̾ ����: ���°� �������� �ƴ���
	MyHostInfoForUpdate()
	{
		for(int i=0; i<4; ++i){nRedTeam[i]=3;}
		for(int i=0; i<4; ++i){nComputer[i]=3;}
		for(int i=0; i<4; ++i){nCharIndex[i]=10;}
		memset(bReadyState,0,sizeof(bReadyState));
	}
	~MyHostInfoForUpdate(){}
};

//Ŭ���̾�Ʈ(ȣ��Ʈ)�϶� ������ ������Ʈ �� ���� ���ھ� ���� ������
class MyHostGameScoreForUpdate
{
public:
	int		nGameScore;				//���� �Ʈ�� �̰ܾ� ������ ��������?
	int		nSetScore;					//1��Ʈ�� ������ ���� �ϴ���?

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
	char	sName[PACKET_NAME];		//���� ID	: ������ �����ϱ� ���� �Է��� ID�� �ѹ� ���� �ǵ��� �Ѵ�.


	//������ �Ʒ��� �� �ν��Ͻ��� ����������� ������Ʈ �ؾ���
	CLIENTROOMINFO		m_RoomInfo;
	MyPlayInfo			m_MyInfo;

	//Ŭ���̾�Ʈ���� ������ ������Ʈ �ϴµ� ���Ǵ� �ν��Ͻ���
	MyInfoForUpdate	m_CurMyInfo;				//ȣ��Ʈ/�Խ�Ʈ ����
	MyInfoForUpdate	m_OldMyInfo;				//ȣ��Ʈ/�Խ�Ʈ ����
	MyHostInfoForUpdate m_CurMyHostInfo;		//ȣ��Ʈ ����
	MyHostInfoForUpdate m_OldMyHostInfo;		//ȣ��Ʈ ����
	MyHostGameScoreForUpdate m_CurGameScore;	//ȣ��Ʈ ����
	MyHostGameScoreForUpdate	m_OldGameScore;	//ȣ��Ʈ ����



	CMultiPlay(void);
	virtual ~CMultiPlay(void);

	INT	MultiPlayMyPlayUpdate(int State,int RoomNumber,int RoomPosition);
	INT	MultiPlayRoomUpdate(PACKETDATA*	pPacket);		// ������ ������Ʈ
	INT	Client_Room_Create_Update(PACKETDATA* pPacket);
	INT	Client_Room_Join_Update(PACKETDATA* pPacket);
	INT	Client_Room_Reset();
	INT Client_Room_Exit();
	INT	Client_Room_Game_Start();
	INT MultiPlayFrameMove();							//��Ƽ�÷��� ����
	void MultiPlayRender();								//��Ƽ�÷��� ������

	void UpdateToServer();								//m_OldMyInfo�� m_CurMyInfo�� ��� ������ �ٸ��ٸ� ������ �ٲﳻ�� ������Ʈ!

	void InitVal();										//��Ƽ�÷��̰� ������ ���� ���� �ʱ�ȭ!

	void MultiPlayCharacterSelectInit();				//ȣ��Ʈ�� ��� ������ �ʱ�ȭ�� ��û������ ����Ǵ� �Լ�

	void SpriteDraw();									//��������Ʈ ��ο�
};