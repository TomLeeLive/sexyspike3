#include "../Include/INCLUDE.h"

CRingBuffer::CRingBuffer()
{
	m_sBuf		= NULL	;
	m_iFront	= 0		;
	m_iRear		= 0		;
	m_iSpace	= 0		;
	m_iWidth	= 0		;
	m_iTotal	= 0		;		

}

CRingBuffer::CRingBuffer(int iWidth)
{
	m_sBuf		= new BYTE[iWidth];
	m_iFront	= 0		;
	m_iRear		= 0		;
	m_iSpace	= iWidth;		// 비여있는 공간
	m_iWidth	= 0		;		// 차있는 공간
	m_iTotal	= iWidth;
	
}

CRingBuffer::~CRingBuffer()
{
	if( m_sBuf )
	{
		delete [] m_sBuf;
	}
	m_sBuf = NULL;
}
int	CRingBuffer::EnRingBuffer(void* pData, int iSize)
{
	if( m_iSpace <= 2 || m_iSpace < iSize) { return -1; }

	BYTE* sBuf = (BYTE*)pData;
	
	int i;
	for(i=0;i<iSize;++i)
	{
		m_sBuf[m_iRear] = sBuf[i];
		++m_iRear	;				// Rear 이동
		--m_iSpace	;				// 
		m_iRear = (m_iRear+m_iTotal)%m_iTotal;
	}
	
	m_iWidth = m_iTotal - m_iSpace;
	
	return S_OK;
}
int	CRingBuffer::DeRingBuffer(char* pOut, int* pSize)
{
	
	if(m_iWidth < 2 )
	{
		*pSize = 0;
		return -1;
	}


	int i;
	//// 전체보내기
	//*pSize = m_iWidth;
	//for(i=0;i<m_iWidth;++i)
	//{
	//	pOut[i] = m_sBuf[m_iFront];
	//	++m_iFront	;
	//	++m_iSpace	;
	//	m_iFront = (m_iFront+m_iTotal)%m_iTotal;
	//}
	
	//하나씩 보내기
	*pSize = m_sBuf[m_iFront];
	int iSize = *pSize;
	for(i=0;i<iSize;++i)
	{
		pOut[i] = m_sBuf[m_iFront];
		++m_iFront	;
		++m_iSpace	;
		m_iFront = (m_iFront+m_iTotal)%m_iTotal;
	}

	m_iWidth = m_iTotal - m_iSpace;
	
	return S_OK;
}
void CRingBuffer::Clear()
{
	m_iRear = 0;
	memset(m_sBuf, 0, sizeof(BYTE)*(m_iWidth+1) );
}
int	CRingBuffer::GetWidth()
{
	return m_iWidth;
}
int CRingBuffer::GetRemain()
{
	return m_iSpace;
}
int	CRingBuffer::Resize()
{
	return S_OK;
}
