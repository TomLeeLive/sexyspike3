// Interface for the CLcHangul class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcHangul_H_
#define _LcHangul_H_


#define MAX_CHAT_LEN	256														// 최대 채팅 길이


class CLcHangul
{
protected:
	TCHAR	m_sWrd[4];															// 조합중인 문자!!
	TCHAR	m_sStr[MAX_CHAT_LEN*2+8];											// Output buffer..
	TCHAR	m_sOut[MAX_CHAT_LEN*2+8];											// Output buffer..
	BOOL	m_bUse;																// 현재 IME를 사용하고 있는지 체크
	BOOL	m_bCmp;																// 현재 조합중인지를 나타내는 플래그

	DWORD	m_dStB;																// String Table을 사용해서 Back Space Key를
																				// 재정의한 경우 WM_COMMAND를 사용해야 한다.

public:
	CLcHangul();
	virtual ~CLcHangul();
	
	INT		Create(DWORD dBackSpace=0);
	void	Destroy();
	LRESULT	MsgProc(HWND, UINT, WPARAM, LPARAM);

	void	Set();
	void	Reset();

	BOOL	GetState();
	void	SetState(BOOL use);

	int		OutStringBeam(TCHAR* sOut);
	void	OutStringStar(TCHAR* sOut);
	int		OutString(TCHAR* sOut);

protected:
	void	EraseWord();
};

#endif
