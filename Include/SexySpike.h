#pragma		once

//09-08-12 이재준 구조체 추가
struct CPlayerInfo		//플레이어 정보 관련, 선택된 캐릭터 번호 등...
{
	//네트워크 시에 사용 되는 정보
	//---------------------------------
	//나의ID
	//내가호스트(방장)인가?
	//내가레드인가블루인가?
	//캐릭터번호

	char strName[PACKET_NAME];	//ID
	BOOL bHost;					//내가호스트인가?

	INT nRedTeam;					//레드 팀일 경우 1, 블루 팀일 경우 0, 선택되지 않았을때 3
	INT iCharacterNum;			//선택된 캐릭터 번호. cf) 10번 캐릭터인 경우 해당 캐릭터는 선택되지 않은것
	BOOL bComputer;				//해당 플레이어는 컴퓨터인가? 디폴트: TRUE

	INT	iSpikeEnergy;				//스파이크 게이지 수치

	CPlayerInfo(){memset(strName,0,sizeof(strName));bHost=FALSE;nRedTeam=3;iCharacterNum=10;bComputer=TRUE;iSpikeEnergy=0;};
	~CPlayerInfo(){};
};

//09-09-08 이재준
struct CVector
{
	D3DXVECTOR3 vcPos1;	//스파이크 게이지용
	D3DXVECTOR3 vcPos2;	//캐릭터 아이콘용

	CVector(){vcPos1=D3DXVECTOR3(0,0,0);vcPos2=D3DXVECTOR3(0,0,0);}
	~CVector(){};
};

class SexySpike : public CD3DApplication
{
public:
	//09-09-08 이재준
	CVector			m_vcIcon[4];

	//09-09-04	이재준
	BOOL			m_bTest;											//테스트 용 변수,  TRUE일 경우 캐릭터 1개만 불러온다.

	//09-08-28 이재준
	CGameSceneManager* m_GameSceneManager;

	//09-08-27	이재준													
	INT				m_nRedTeamWins;									//레드팀 승리횟수
	INT				m_nBlueTeamWins;									//블루팀 승리횟수

	//09-08-27 이재준
	BOOL			m_bQuitButton;										//종료버튼 FALSE: 클릭 안됨
	BOOL			m_bLoadingApp;										// TRUE, if the app is loading

	//09-08-25	이재준
	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;											//배경위치

	//09-08-20 이재준 Camera Animation Model
	CCameraAnimationModel* m_pCameraAniModel;

	//09-08-12 이재준
	CPlayerInfo			m_PlayerInfo[4];							//0: player1, 1: player2, 2: player3, 3: player4 
	BOOL					m_bMultiGame;

	INT						m_nRedTeamScore;							//레드팀 스코어
	INT						m_nBlueTeamScore;							//블루팀 스코어
	INT						m_nSetNumber;								//현재 세트

	//09-08-11 이재준
	CStartScreen*			m_StartScreen;
	CMultiPlay*			m_MultiPlay;
	CHowtoPlay*			m_HowtoPlay;
	CEndGame*				m_EndGame;
	CCharacterSelect*		m_CharacterSelect;

	//Mp3
	CMiMp3*					m_pMp[8];

	//CGameTexture2 m_pTexture;
	CGameTexture3			m_pGameTex[55];


	// 카메라 매니저 09-08-04 이재준
	CMcCamManager*			m_pCamMn;

	// 인풋 클래스 09-08-04 이재준
	CMcInput*				m_pInput	;

	//09-09-09 이웅재
	CNetworkDlg*			m_NetworkDlg;
	CClient					m_netAdapter;

	//	Flow State
	eFlowState				m_eFlow;
	eRenderMode				m_eRenderMode;

	//	Animation Model
	Character				*m_pModels[NUMMODELS];
	CPlayer					*m_ppPlayers[NUMPLAYERS];

	//	Background
	CBackground				*m_pBackground;

	//	Net
	CNet					*m_pNet;

	//	Ball
	CBall					*m_pBall;
	BOOL					m_bBallReflected;

	//
	CFirstPersonCamera		m_camera;

	//
	CrashManager			*m_pCrashManager;

    // Font for drawing text
    CD3DFont* m_pFont;
    CD3DFont* m_pFontSmall;
	
	// Navigation
    FLOAT       m_fSpeed;
    FLOAT       m_fAngularSpeed;
    BOOL        m_bShowHelp;

    D3DXVECTOR3 m_vVelocity;
    D3DXVECTOR3 m_vAngularVelocity;

	//	After Image
	LPDIRECT3DTEXTURE9	m_pPostTex[2];
	LPDIRECT3DSURFACE9	m_pPostSuf[2];
	LPDIRECT3DSURFACE9	m_pPostDep[2];
	LPDIRECT3DVERTEXBUFFER9	m_pPost;
	DWORD				m_LoofCounter;

	LPDIRECT3DTEXTURE9	m_pSlapTex;
	LPDIRECT3DVERTEXBUFFER9	m_pSlap;
	DWORD				m_slapcount;
	D3DXVECTOR3			m_vSlapPos;

protected:
    HRESULT 	OneTimeSceneInit();
    HRESULT 	InitDeviceObjects();
    HRESULT 	RestoreDeviceObjects();
    HRESULT 	InvalidateDeviceObjects();
    HRESULT 	DeleteDeviceObjects();
    HRESULT 	FinalCleanup();
    HRESULT 	Render();
	HRESULT		RenderNormal();
	HRESULT		RenderAfterImage();
	HRESULT		RenderInversion();
    HRESULT 	FrameMove();
    HRESULT 	ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
	D3DFORMAT	adapterFormat, D3DFORMAT backBufferFormat );
    LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	HRESULT		ReadyAfterImage();
	HRESULT		GetRenderTarget(LPDIRECT3DSURFACE9* ppSuf, LPDIRECT3DSURFACE9* ppDep, D3DVIEWPORT9*	pVp);
	HRESULT		SetRenderTarget(LPDIRECT3DSURFACE9	pSuf, LPDIRECT3DSURFACE9 pDep, D3DVIEWPORT9*	pVp);
public:
	void		ReadyPlay();
	void		AlignCoat();
	void		CalcScore(eTeam TEAM);
	D3DXVECTOR3	ComputeAttackingDest( int SIDE, eAttackingSide eAttack );
	void		ComputeCameraBasedDirections();
	void		CheckCollisions();
	void		PlayerStateChange();

public:
    SexySpike();

	////////////////////////////////////////////////////////////////////////////////
	//09-08-06 이재준 메뉴별로 게임 메뉴, 게임 실행, 스코어 
	INT		m_nGamePhase;					//게임 상태 변경(시작화면, 게임, 종료)

	////////////////////////////////////////////////////////////////////////////////


	INT		TextureLoad();				//텍스쳐 파일 로드
	INT		InitSoundObjects();		//사운드 파일 로드
	void	BGMPlay();					//BGM 랜덤 재생

	//09-08-25	이재준
	HRESULT SinglePlayFrameMove();	//싱글플레이 프레임 무브
	HRESULT SinglePlayRender();		//싱글플레이 렌더

	//09-08-25	이재준
	void SpriteDraw();				//점수, 캐릭터 아이콘 출력

	//09-08-25 이재준
	void SoundPlay(eBehavior BEHAVIOR);

	//09-08-27 이재준
	BOOL QuitButtonClickCheck(void);	//종료 버튼 클릭 되었는지 체크& 종료 처리
	void Initval();						//게임 관련 변수들 초기화

	void ChangeCoat();					//코트 바꾸기

	void CharacterSet();					//캐릭터 셋팅, 플레이어인지 컴퓨터 인지...

	INT RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax);

	void MultiPlayIDTextDraw();			//텍스트로 멀티플레이 플레이어 ID 출력
			
	void PlayerSelfUpdateSetting();		//멀티플레이 플레이어셀프업데이트 설정
};
extern CRITICAL_SECTION		cs_Send;
extern CRITICAL_SECTION		cs_Recv;
extern CRITICAL_SECTION		cs_File;


//09-08-04 이재준
extern SexySpike*	g_pApp;
extern CDrawText	g_text;

//09-09-09 이웅재
extern CNetworkDlg*	g_pNetworkDlg;
extern int				g_NetworkState;
extern	BYTE			g_packet[256];
extern	PLAYERSMAP		g_mapNetObjects;