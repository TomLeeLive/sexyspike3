
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/KeyManager.h"

CKeyManager::CKeyManager()
{
	memset( m_arrKeyState			, 0, sizeof m_arrKeyState );
	memset( m_arrCurrentKeys		, 0, sizeof m_arrCurrentKeys );
	memset( m_arrPreviousKeys		, 0, sizeof m_arrPreviousKeys );
	memset( m_arrAcceptOverlapping	, 0, sizeof m_arrAcceptOverlapping );
}
CKeyManager::~CKeyManager()
{
}

BOOL	CKeyManager::GetCurrentState(BYTE key)
{
	return	0x80 & m_arrCurrentKeys[ key ];
}

BOOL	CKeyManager::GetPreviousState(BYTE key)
{
	return	0x80 & m_arrPreviousKeys[ key ];
}

void	CKeyManager::ScanKeyState()
{
	ZeroMemory( m_arrPreviousKeys, sizeof m_arrPreviousKeys );
	memcpy( m_arrPreviousKeys, m_arrKeyState, sizeof m_arrKeyState );

	ZeroMemory( m_arrKeyState, sizeof m_arrKeyState );
	GetKeyboardState( m_arrKeyState );

	for( int i=0; i<256; ++i )
	{
		//	현재 키가 눌러지고 이전 키가 안눌러진 상태일 때만 입력허용 한다
		m_arrCurrentKeys[i] = m_arrAcceptOverlapping[i]? m_arrKeyState[i] 
							: m_arrKeyState[i] & (m_arrKeyState[i] ^ m_arrPreviousKeys[i] ) ;
	}	
}

void	CKeyManager::SetAcceptOverlapping( BYTE key, BOOL bIsAccept )
{
	m_arrAcceptOverlapping[ key ] = bIsAccept;
}