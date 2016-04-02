#pragma once

typedef unsigned char			BYTE;

class CRingBuffer
{
protected:
	BYTE*					m_sBuf	;
	int						m_iFront;		// 위치는 절대값
	int						m_iRear	;		// 위치는 절대값
	int						m_iSpace;		// 남은 공간
	int						m_iWidth;		// 찬 공간
	int						m_iTotal;		// 전체크기

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