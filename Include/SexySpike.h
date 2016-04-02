#pragma		once

//09-08-12 ������ ����ü �߰�
struct CPlayerInfo		//�÷��̾� ���� ����, ���õ� ĳ���� ��ȣ ��...
{
	//��Ʈ��ũ �ÿ� ��� �Ǵ� ����
	//---------------------------------
	//����ID
	//����ȣ��Ʈ(����)�ΰ�?
	//���������ΰ�����ΰ�?
	//ĳ���͹�ȣ

	char strName[PACKET_NAME];	//ID
	BOOL bHost;					//����ȣ��Ʈ�ΰ�?

	INT nRedTeam;					//���� ���� ��� 1, ��� ���� ��� 0, ���õ��� �ʾ����� 3
	INT iCharacterNum;			//���õ� ĳ���� ��ȣ. cf) 10�� ĳ������ ��� �ش� ĳ���ʹ� ���õ��� ������
	BOOL bComputer;				//�ش� �÷��̾�� ��ǻ���ΰ�? ����Ʈ: TRUE

	INT	iSpikeEnergy;				//������ũ ������ ��ġ

	CPlayerInfo(){memset(strName,0,sizeof(strName));bHost=FALSE;nRedTeam=3;iCharacterNum=10;bComputer=TRUE;iSpikeEnergy=0;};
	~CPlayerInfo(){};
};

//09-09-08 ������
struct CVector
{
	D3DXVECTOR3 vcPos1;	//������ũ ��������
	D3DXVECTOR3 vcPos2;	//ĳ���� �����ܿ�

	CVector(){vcPos1=D3DXVECTOR3(0,0,0);vcPos2=D3DXVECTOR3(0,0,0);}
	~CVector(){};
};

class SexySpike : public CD3DApplication
{
public:
	//09-09-08 ������
	CVector			m_vcIcon[4];

	//09-09-04	������
	BOOL			m_bTest;											//�׽�Ʈ �� ����,  TRUE�� ��� ĳ���� 1���� �ҷ��´�.

	//09-08-28 ������
	CGameSceneManager* m_GameSceneManager;

	//09-08-27	������													
	INT				m_nRedTeamWins;									//������ �¸�Ƚ��
	INT				m_nBlueTeamWins;									//����� �¸�Ƚ��

	//09-08-27 ������
	BOOL			m_bQuitButton;										//�����ư FALSE: Ŭ�� �ȵ�
	BOOL			m_bLoadingApp;										// TRUE, if the app is loading

	//09-08-25	������
	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;											//�����ġ

	//09-08-20 ������ Camera Animation Model
	CCameraAnimationModel* m_pCameraAniModel;

	//09-08-12 ������
	CPlayerInfo			m_PlayerInfo[4];							//0: player1, 1: player2, 2: player3, 3: player4 
	BOOL					m_bMultiGame;

	INT						m_nRedTeamScore;							//������ ���ھ�
	INT						m_nBlueTeamScore;							//����� ���ھ�
	INT						m_nSetNumber;								//���� ��Ʈ

	//09-08-11 ������
	CStartScreen*			m_StartScreen;
	CMultiPlay*			m_MultiPlay;
	CHowtoPlay*			m_HowtoPlay;
	CEndGame*				m_EndGame;
	CCharacterSelect*		m_CharacterSelect;

	//Mp3
	CMiMp3*					m_pMp[8];

	//CGameTexture2 m_pTexture;
	CGameTexture3			m_pGameTex[55];


	// ī�޶� �Ŵ��� 09-08-04 ������
	CMcCamManager*			m_pCamMn;

	// ��ǲ Ŭ���� 09-08-04 ������
	CMcInput*				m_pInput	;

	//09-09-09 �̿���
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
	//09-08-06 ������ �޴����� ���� �޴�, ���� ����, ���ھ� 
	INT		m_nGamePhase;					//���� ���� ����(����ȭ��, ����, ����)

	////////////////////////////////////////////////////////////////////////////////


	INT		TextureLoad();				//�ؽ��� ���� �ε�
	INT		InitSoundObjects();		//���� ���� �ε�
	void	BGMPlay();					//BGM ���� ���

	//09-08-25	������
	HRESULT SinglePlayFrameMove();	//�̱��÷��� ������ ����
	HRESULT SinglePlayRender();		//�̱��÷��� ����

	//09-08-25	������
	void SpriteDraw();				//����, ĳ���� ������ ���

	//09-08-25 ������
	void SoundPlay(eBehavior BEHAVIOR);

	//09-08-27 ������
	BOOL QuitButtonClickCheck(void);	//���� ��ư Ŭ�� �Ǿ����� üũ& ���� ó��
	void Initval();						//���� ���� ������ �ʱ�ȭ

	void ChangeCoat();					//��Ʈ �ٲٱ�

	void CharacterSet();					//ĳ���� ����, �÷��̾����� ��ǻ�� ����...

	INT RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax);

	void MultiPlayIDTextDraw();			//�ؽ�Ʈ�� ��Ƽ�÷��� �÷��̾� ID ���
			
	void PlayerSelfUpdateSetting();		//��Ƽ�÷��� �÷��̾��������Ʈ ����
};
extern CRITICAL_SECTION		cs_Send;
extern CRITICAL_SECTION		cs_Recv;
extern CRITICAL_SECTION		cs_File;


//09-08-04 ������
extern SexySpike*	g_pApp;
extern CDrawText	g_text;

//09-09-09 �̿���
extern CNetworkDlg*	g_pNetworkDlg;
extern int				g_NetworkState;
extern	BYTE			g_packet[256];
extern	PLAYERSMAP		g_mapNetObjects;