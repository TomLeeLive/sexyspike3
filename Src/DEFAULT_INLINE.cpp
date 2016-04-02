#include	<windows.h>
#include	<iostream>
#include	<d3d9.h>
#include	<d3dx9.h>

BOOL	CopyString( const CHAR* pSrc, CHAR** ppDest )
{
	if(NULL == pSrc || NULL == ppDest)
		return FALSE;

	size_t	LENGTH	= strlen( pSrc ) + 1;
	*ppDest		= new CHAR[ LENGTH ];

	if( NULL == *ppDest )
		return FALSE;

	strcpy_s( *ppDest, LENGTH, pSrc );

	return	TRUE;
}


#ifdef	UNICODE

BOOL	CopyString( LPCTSTR pSrc, LPTSTR* ppDest )
{
	if(NULL == pSrc || NULL == ppDest)
		return FALSE;

	int	LENGTH	= lstrlen( pSrc ) + 1;
	*ppDest		= new TCHAR[ LENGTH ];

	if( NULL == *ppDest )
		return FALSE;

	lstrcpy( *ppDest, pSrc );

	return	TRUE;
}

#endif

D3DXVECTOR3	Reflect(const D3DXVECTOR3* pN,	const D3DXVECTOR3* pD)
{
	D3DXVECTOR3	n	= *pN;
	D3DXVECTOR3	d	= *pD;
	D3DXVec3Normalize( &n, &n );
	float fDot	= D3DXVec3Dot( &n, &(-d) );
	
	return	2*fDot*n + d;
}
