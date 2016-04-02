

#pragma		once

#ifndef		__KEYMANAGER_H_
#define		__KEYMANAGER_H_

class		CKeyManager
{
	BYTE		m_arrKeyState[256];				//	�˻��� �� ����� Ű �迭
	BYTE		m_arrCurrentKeys[256];			//	���� Ű
	BYTE		m_arrPreviousKeys[256];			//	���� Ű
	BOOL		m_arrAcceptOverlapping[256];	//	�ߺ� �Է����

public:
	CKeyManager();
	~CKeyManager();

	//	Ű�� ������ ���¸� ��ȯ�Ѵ�.
	BOOL		GetCurrentState( BYTE key );
	BOOL		GetPreviousState( BYTE key );
	//	256���� Ű���� ���¸� �����ϰ� ���� Ű�� �ߺ��Է¼����� �����ؼ� ���� Ű������ �����.
	void		ScanKeyState();
	//	�ߺ��Է� ����, �� ��° ���ڸ� FALSE�� �ָ� �ߺ��Է� ������
	void		SetAcceptOverlapping( BYTE key, BOOL bIsAccept=TRUE ); 
	
};

#endif