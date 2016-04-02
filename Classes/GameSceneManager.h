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
	INT m_nComMemberTotal;						//지금공석이 몇명있는지?
	INT	m_nRedMemberTotal;						//총레드팀 멤버수
	INT	m_nBlueMemberTotal;						//총블루팀 멤버스
	INT m_nRedMember[2];							//레드팀에 대한 정보: 몇번 플레이어인지?
	INT m_nRedComputer[2];						//레드팀컴퓨터에 대한 정보: 몇번 플레이어인지?
	INT m_nBlueMember[2];							//블루팀에 대한 정보: 몇번 플레이어인지?
	INT m_nBlueComputer[2];						//블루팀컴퓨터에 대한 정보: 몇번 플레이어인지?
	std::vector<CDefineMember*>	pvRedData;		//레드팀에 대한 정렬
	std::vector<CDefineMember*>	pvBlueData;	//블루팀에 대한 정렬
	std::vector<CDefineMember*>	pvComData;		//컴퓨터플레이어에 대한 정렬

	INT m_nSetFinishScore;						//게임 1세트가 끝나는 스코어
	INT m_nGameFinishSet;							//최종 몇 세트를 이겨야 하는지?

	char* m_strAnimationSetName[4];				//애니메이션 셋 이름 할당용 변수 0~3 번 플레이어에 대해...

	INT m_nFocusCharacterNum;					//카메라로 보여질 캐릭터 넘버

	BOOL	m_bAniSetChanged;						//애니메이션 셋이 바뀌었는가?

	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;						//배경위치


	BOOL m_bGameStart;							//게임이 시작되었는가?
	INT m_dTime;									//현재 타임
	INT m_dTimeEnd	;								//이벤트 끝나는 타임
	BOOL m_bTimeInit;								//이벤트 끝나는 타임이 초기화 되었는가?

	INT m_nRedOldScore;							//이전 레드팀 스코어
	INT m_nRedCurScore;							//현재 레드팀 스코어
	INT m_nBlueOldScore;							//이전 블루팀 스코어
	INT m_nBlueCurScore;							//현재 블루팀 스코어

	INT m_nOldSetNum;								//이전 세트 수
	INT m_nCurSetNum;								//현재 세트 수

	BOOL m_bGameFinished;							//게임이 모두 끝났을 때 TRUE
	BOOL m_bRedTeamWins;							//TRUE일때 레드팀이 승, FALSE일때 블루팀이 승

	BOOL m_bEventSceneFinished;					//해당 게임 이벤트를 마치면 값: FALSE

	CGameSceneManager(void);
	virtual ~CGameSceneManager(void);


	INT FrameMove();								//멀티플레이 갱신
	HRESULT Render();								//멀티플레이 렌더링

	void InitVal();								//게임 운용 관련 변수 초기화

	HRESULT GotPoint(BOOL bBlueTeam);			//득점 시 처리 되는 내용

	void SetChange();								//세트수가 올라갈 때 처리 되는 내용

	void GameFinished();							//최종 세트가 끝났을 때 게임 종료 이벤트가 처리 되는 내용

	INT GameStart();								//게임이 시작되면 보여지는 이벤트 처리

	void	SpriteDraw();							//스프라이트 드로우

	char* AniSetNameRandom(INT randnum);		//애니메이션 셋 이름 랜덤 지정

	void AfterSetInit();							//세트가 끝난후 관련 포인트 초기화 & 업데이트

	void AnimationInit(char* strAniName,INT modelNum);		//애니메이션 셋 지정된 셋으로...

	void DefineTeamMember();						//레드팀 0,1  블루팀 0,1 에 대한 정보 저장을 하는 함수

	INT	MyTeamAndNumberCheck(INT nRoomPosition);//나는 어느팀이고 몇번 멤버인가? 레드 0,1 블루 0,1 

	HRESULT MultiPlayGotPoint(BOOL bBlueTeam);	//멀티플레이: 득점할때 카메라 처리를 하지 않음
};
