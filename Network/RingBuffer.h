#pragma once

typedef unsigned char			BYTE;

class CRingBuffer
{
protected:
	BYTE*					m_sBuf	;
	int						m_iFront;		// ��ġ�� ���밪
	int						m_iRear	;		// ��ġ�� ���밪
	int						m_iSpace;		// ���� ����
	int						m_iWidth;		// �� ����
	int						m_iTotal;		// ��üũ��

public:
	CRingBuffer();
	CRingBuffer(int iWidth);
    virtual	~CRingBuffer();
	
	int		EnRingBuffer(void* pData, int iSize);
	int		DeRingBuffer(char* pOut, int* iSize);
	void	Clear()								;
	int		GetWidth()							;
	int		GetRemain()							;
	int		Resize()							;

};