
#pragma		once

#ifndef		__PLAYER_H_
#define		__PLAYER_H_

struct	ValleyActionMacro
{
	float		ElapsedTime;		//	경과시간
	float		MacroTime;			//	총 매크로 진행시간
	float		TransitionPoint;	//	배구동작호출시점
	D3DXVECTOR3 Velocity;			//	움직일 속도
	D3DXVECTOR3	Destination;		//	도착지
	eBehavior	MovingAction;		//	움직일 동작
	eBehavior	ValleyAction;		//	배구동작
};

class	CPlayer;

struct	PLAYINFO
{
	PLAY_STATE				PLAYSTATE;			//	공격/수비/서브
	int						SIDE;				//
	int						TEAM;
	CPlayer*				TEAMMATE;
};

class		CPlayer	:	public	Character
{
protected:
	int					m_id;
	LPSTR				m_pName;
	BOOL				m_bIsNpc;				//	NPC
	BOOL				m_bIsAttacker;			//	공격수
	BOOL				m_bSelfUpdate;			//	스스로 갱신, 네트워크 갱신
	BOOL				m_bUseMacro;			//	매크로 사용여부
	ValleyActionMacro	m_macro;				//	매크로 구조체
	eAttackingSide		m_eAttackingSide;
	D3DXVECTOR3			m_vAttackingPos;		//	공격지점
	PlayerPacket		m_lastPacket;
	eBallType			m_eSpecialAttack;		//	필살기
public:
	PLAYINFO			m_playInfo;


public	:
	CPlayer( const Character& SrcModel, int id, LPCSTR pName ) ;
	~CPlayer();

	//void	Send( void* pData, int size, int PTYPE = 3, int ORDER = 3 );

	bool	operator==( LPCSTR	cmp );
	bool	operator==( CPlayer* );

	LPCSTR	GetName() { return	m_pName; }
	virtual	void	ProcessMacro(float fElapsedTime)	= 0;
	void	Translation( void* t ) { m_matT._41=((float*)t)[0]; m_matT._42=((float*)t)[1]; m_matT._43=((float*)t)[2]; }
	void	SendPlayerStateToNetwork();
	void	UpdatePlayerStateFromNetwork(const PlayerPacket *packet);

	void	SetAnimationSet( LPCSTR pAniSetName ) { m_pAniController->SetAnimationSet(pAniSetName); }
	void	SetAttackingSide( eAttackingSide eAttack ) { m_eAttackingSide = eAttack; }
	void	SetAttackingPos( D3DXVECTOR3 vPos ) { m_vAttackingPos = vPos; }
	void	SetSpecialAttack( eBallType balltype ) { m_eSpecialAttack = balltype; }
	void	SetPLAYSTATE( PLAY_STATE STATE ) { m_playInfo.PLAYSTATE = STATE; }
	void	SetIsNpc( BOOL bIsNpc ) { m_bIsNpc = bIsNpc; }
	void	SetIsAttacker( BOOL bIsAttack) { m_bIsAttacker = bIsAttack; }
	void	virtual SetMacro(eBehavior BEHAVIOR) = 0;
	void	SetMacro( char SIZE, void* pMacro) { m_bUseMacro = TRUE; memcpy( &m_macro, pMacro, SIZE ); }
	void	SetRotation( void*  pFRotation ) { m_fRotationAngle = *( (float*)pFRotation); }
	void	SetRotation( float  fRotation ) { m_fRotationAngle = fRotation; }
	void	SetPosition(float x, float y, float z);
	void	SetSelfUpdate( BOOL bSelfUpdate ) { m_bSelfUpdate = bSelfUpdate; }
	void	SetSide	( int	SIDE ) { m_playInfo.SIDE = SIDE; }
	void	SetTeam ( int	TEAM ) { m_playInfo.TEAM = TEAM; } 
	void	SetTeammate ( CPlayer* TEAMMATE) { m_playInfo.TEAMMATE = TEAMMATE; } 
	PLAY_STATE			GetPLAYSTATE() { return m_playInfo.PLAYSTATE; }
	eAttackingSide		GetAttackingSide()	{ return m_eAttackingSide; }
	eBehavior			GetCurrentBehavior();
	eBallType			GetSpecialAttack() { return m_eSpecialAttack; }
	virtual	eBehavior	GetNextBehavior()	= 0;
	BOOL				GetIsNpc() { return m_bIsNpc; }
	BOOL				GetIsAttacker(){ return m_bIsAttacker; }
	const	PLAYINFO*	GetPlayInfo(){ return	&m_playInfo; }
	const	LPD3DXVECTOR3	GetAttackingPosition() const { return (LPD3DXVECTOR3)&m_vAttackingPos; }
	D3DXVECTOR3			GetPosition() const { return D3DXVECTOR3( m_matT._41, m_matT._42, m_matT._43 ); }
	float				GetRotationAngle(){ return m_fRotationAngle; }
	BOOL				GetSelfUpdate() { return	m_bSelfUpdate; }
	int					GetSide() { return	m_playInfo.SIDE; }
	int					GetTeam() { return	m_playInfo.TEAM; }
	CPlayer*			GetTeammate() { return m_playInfo.TEAMMATE; }
};

#endif