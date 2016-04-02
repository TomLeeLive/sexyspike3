// Interface for the CLcHangul class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LcHangul_H_
#define _LcHangul_H_


#define MAX_CHAT_LEN	256														// �ִ� ä�� ����


class CLcHangul
{
protected:
	TCHAR	m_sWrd[4];															// �������� ����!!
	TCHAR	m_sStr[MAX_CHAT_LEN*2+8];											// Output buffer..
	TCHAR	m_sOut[MAX_CHAT_LEN*2+8];											// Output buffer..
	BOOL	m_bUse;																// ���� IME�� ����ϰ� �ִ��� üũ
	BOOL	m_bCmp;																// ���� ������������ ��Ÿ���� �÷���

	DWORD	m_dStB;																// String Table�� ����ؼ� Back Space Key��
																				// �������� ��� WM_COMMAND�� ����ؾ� �Ѵ�.

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
