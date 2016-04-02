
#pragma		once

#ifndef		__USER_H_
#define		__USER_H_

class		CUser	:	public CPlayer
{
	CUser( const Character& SrcModel, int id, LPCSTR pName ) 
		: CPlayer( SrcModel, id, pName ) { m_pAniController->SetOwnerMoveLock( &m_bMoveLock); }
	BOOL	m_bMoveLock;

public:
	~CUser();

	static		CUser* Create( const Character& SrcModel, int id, LPCSTR pName );

	HRESULT		FrameMove( float fElapsedTime = 0.f );

	void		ConfirmCharacterKeyState();					//	키 입력 확인
	void		ProcessMacro(float fElapsedTime);
	void		SetMacro(eBehavior BEHAVIOR);
	eBehavior	GetNextBehavior();

	inline		void	SetMoveLock( BOOL bLock ) { m_bMoveLock = bLock; }
	inline		BOOL	GetMoveLock()	{ return	m_bMoveLock; }
};

#endif