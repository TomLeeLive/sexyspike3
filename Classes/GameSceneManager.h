#pragma once

class CDefineMember
{
public:
	INT nIdxNumber;
	CDefineMember(INT _nIdxNumber){nIdxNumber = _nIdxNumber;};
	~ CDefineMember(){};
};

class CGameSceneManager
{
public:
	INT m_nComMemberTotal;						//���ݰ����� ����ִ���?
	INT	m_nRedMemberTotal;						//�ѷ����� �����
	INT	m_nBlueMemberTotal;						//�Ѻ���� �����
	INT m_nRedMember[2];							//�������� ���� ����: ��� �÷��̾�����?
	INT m_nRedComputer[2];						//��������ǻ�Ϳ� ���� ����: ��� �÷��̾�����?
	INT m_nBlueMember[2];							//������� ���� ����: ��� �÷��̾�����?
	INT m_nBlueComputer[2];						//�������ǻ�Ϳ� ���� ����: ��� �÷��̾�����?
	std::vector<CDefineMember*>	pvRedData;		//�������� ���� ����
	std::vector<CDefineMember*>	pvBlueData;	//������� ���� ����
	std::vector<CDefineMember*>	pvComData;		//��ǻ���÷��̾ ���� ����

	INT m_nSetFinishScore;						//���� 1��Ʈ�� ������ ���ھ�
	INT m_nGameFinishSet;							//���� �� ��Ʈ�� �̰ܾ� �ϴ���?

	char* m_strAnimationSetName[4];				//�ִϸ��̼� �� �̸� �Ҵ�� ���� 0~3 �� �÷��̾ ����...

	INT m_nFocusCharacterNum;					//ī�޶�� ������ ĳ���� �ѹ�

	BOOL	m_bAniSetChanged;						//�ִϸ��̼� ���� �ٲ���°�?

	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;						//�����ġ


	BOOL m_bGameStart;							//������ ���۵Ǿ��°�?
	INT m_dTime;									//���� Ÿ��
	INT m_dTimeEnd	;								//�̺�Ʈ ������ Ÿ��
	BOOL m_bTimeInit;								//�̺�Ʈ ������ Ÿ���� �ʱ�ȭ �Ǿ��°�?

	INT m_nRedOldScore;							//���� ������ ���ھ�
	INT m_nRedCurScore;							//���� ������ ���ھ�
	INT m_nBlueOldScore;							//���� ����� ���ھ�
	INT m_nBlueCurScore;							//���� ����� ���ھ�

	INT m_nOldSetNum;								//���� ��Ʈ ��
	INT m_nCurSetNum;								//���� ��Ʈ ��

	BOOL m_bGameFinished;							//������ ��� ������ �� TRUE
	BOOL m_bRedTeamWins;							//TRUE�϶� �������� ��, FALSE�϶� ������� ��

	BOOL m_bEventSceneFinished;					//�ش� ���� �̺�Ʈ�� ��ġ�� ��: FALSE

	CGameSceneManager(void);
	virtual ~CGameSceneManager(void);


	INT FrameMove();								//��Ƽ�÷��� ����
	HRESULT Render();								//��Ƽ�÷��� ������

	void InitVal();								//���� ��� ���� ���� �ʱ�ȭ

	HRESULT GotPoint(BOOL bBlueTeam);			//���� �� ó�� �Ǵ� ����

	void SetChange();								//��Ʈ���� �ö� �� ó�� �Ǵ� ����

	void GameFinished();							//���� ��Ʈ�� ������ �� ���� ���� �̺�Ʈ�� ó�� �Ǵ� ����

	INT GameStart();								//������ ���۵Ǹ� �������� �̺�Ʈ ó��

	void	SpriteDraw();							//��������Ʈ ��ο�

	char* AniSetNameRandom(INT randnum);		//�ִϸ��̼� �� �̸� ���� ����

	void AfterSetInit();							//��Ʈ�� ������ ���� ����Ʈ �ʱ�ȭ & ������Ʈ

	void AnimationInit(char* strAniName,INT modelNum);		//�ִϸ��̼� �� ������ ������...

	void DefineTeamMember();						//������ 0,1  ����� 0,1 �� ���� ���� ������ �ϴ� �Լ�

	INT	MyTeamAndNumberCheck(INT nRoomPosition);//���� ������̰� ��� ����ΰ�? ���� 0,1 ��� 0,1 

	HRESULT MultiPlayGotPoint(BOOL bBlueTeam);	//��Ƽ�÷���: �����Ҷ� ī�޶� ó���� ���� ����
};
