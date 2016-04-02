
#pragma		once

#ifndef		__DEFAULT_INLINE_H_
#define		__DEFAULT_INLINE_H_

//	문자열 복사 
extern	BOOL	CopyString( const CHAR* pSrc, CHAR** ppDest );

//	노말백터와 방향백터로부터 반사 백터를 구한다.
extern	D3DXVECTOR3	Reflect(const D3DXVECTOR3* pN,	const D3DXVECTOR3* pD);

#endif