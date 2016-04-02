

#pragma		once

#ifndef		__KEYMANAGER_H_
#define		__KEYMANAGER_H_

class		CKeyManager
{
	BYTE		m_arrKeyState[256];				//	검사할 때 사용할 키 배열
	BYTE		m_arrCurrentKeys[256];			//	현재 키
	BYTE		m_arrPreviousKeys[256];			//	이전 키
	BOOL		m_arrAcceptOverlapping[256];	//	중복 입력허용

public:
	CKeyManager();
	~CKeyManager();

	//	키가 눌려진 상태를 반환한다.
	BOOL		GetCurrentState( BYTE key );
	BOOL		GetPreviousState( BYTE key );
	//	256개의 키보드 상태를 조사하고 이전 키와 중복입력설정을 참고해서 현재 키설정을 만든다.
	void		ScanKeyState();
	//	중복입력 설정, 두 번째 인자를 FALSE로 주면 중복입력 허용안함
	void		SetAcceptOverlapping( BYTE key, BOOL bIsAccept=TRUE ); 
	
};

#endif