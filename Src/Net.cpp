
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/Net.h"

CNet::CNet()
{
	m_bNowDance		= false;
	m_fDanceTime	= 0.f;
	m_fDanceWeight	= 0.f;
	ZeroMemory( &m_BoundingNet, sizeof( BoundingPlane ) );
	ZeroMemory( &m_BoundingMesh, sizeof( BoundingPlane ) );
	ZeroMemory( &m_vAxis, sizeof(D3DXVECTOR3));
	D3DXMatrixIdentity( &m_matWorld );	
	D3DXMatrixIdentity( &m_matWorld1 );
}

CNet::~CNet()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}

HRESULT		CNet::InitDeviceObjects( LPDIRECT3DDEVICE9	pDevice ){
	HRESULT		hr	= S_OK;

	LPD3DXMESH		pMesh			= NULL;
	D3DXVECTOR3		vMin;
	D3DXVECTOR3		vMax;
	D3DXVECTOR3		*pFirstPosition	= NULL;

	//	네트기둥
	CHECK_FAILED( m_pillar.Create( pDevice, "\\XFILE\\NET\\net_pillars.x") );
	CHECK_FAILED( m_net.Create( pDevice, "\\XFILE\\NET\\net_net.X") );
	DWORD	dwFVF	= D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_TEX1;
	m_net.SetFVF( pDevice, dwFVF );

	pMesh	= m_net.GetSysMemMesh();

	pMesh->LockVertexBuffer( 0, (void**) &pFirstPosition );

	D3DXComputeBoundingBox( 
				pFirstPosition
			,	pMesh->GetNumVertices()
			,	D3DXGetFVFVertexSize( pMesh->GetFVF() )
			,	&vMin
			,	&vMax );

	pMesh->UnlockVertexBuffer();

	m_BoundingNet.n	= D3DXVECTOR3( 1, 0, 0 );
	m_BoundingNet.d	= 0;
	//	최종적으로 넷을 90도 회전 시키므로 x와 z값을 바꿔준다
	m_BoundingNet.LB.z	= -vMax.x-90.f;
	m_BoundingNet.RT.z	= -vMin.x-110.f;
	m_BoundingNet.RT.y	= vMax.y;

	pMesh	= NULL;
	
//	그물용 메쉬를 만든다
	DWORD	temp;
	hr	= D3DXLoadMeshFromX(
					"./XFILE/NET/net_center.x"
				,	D3DXMESH_MANAGED
				,	pDevice
				,	NULL
				,	NULL
				,	NULL
				,	&temp
				,	&pMesh
				);
	
	pMesh->LockVertexBuffer( 0, (void**) &pFirstPosition );

	D3DXComputeBoundingBox( 
				pFirstPosition
			,	pMesh->GetNumVertices()
			,	D3DXGetFVFVertexSize( pMesh->GetFVF() )
			,	&vMin
			,	&vMax );

	pMesh->UnlockVertexBuffer();

	m_BoundingMesh.n	= D3DXVECTOR3( 1, 0, 0 );
	m_BoundingMesh.d	= 0;
	m_BoundingMesh.LB.y	= vMin.y+1.f;
	m_BoundingMesh.LB.z	= -vMax.x-90.f;
	m_BoundingMesh.RT.y	= vMax.y-1.f;
	m_BoundingMesh.RT.z	= -vMin.x-110.f;

	g_BoundingMesh		= m_BoundingMesh;

	SAFE_RELEASE( pMesh );

	return		hr;
}
HRESULT		CNet::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT		hr	= S_OK;
	D3DXMatrixRotationY( &m_matWorld, D3DXToRadian( 90 ) );
	m_matWorld._43		= -100.f;

	CHECK_FAILED(m_pillar.RestoreDeviceObjects(pDevice));
	CHECK_FAILED(m_net.RestoreDeviceObjects(pDevice));

	ComputeBlendFactor();

	return		hr;
}
HRESULT		CNet::InvalidateDeviceObjects()
{
	HRESULT		hr	= S_OK;
	CHECK_FAILED(hr	= m_pillar.InvalidateDeviceObjects());
	CHECK_FAILED(hr	= m_net.InvalidateDeviceObjects());
	return		hr;
}
HRESULT		CNet::DeleteDeviceObjects()
{
	HRESULT		hr	= S_OK;

	CHECK_FAILED(m_pillar.Destroy());
	CHECK_FAILED(m_net.Destroy());

	return		hr;
}

HRESULT		CNet::FrameMove( float fElapsedTime )
{
	HRESULT		hr	= S_OK;

	if( m_bNowDance )
	{
		D3DXMatrixRotationAxis( &m_matWorld1, &m_vAxis, sinf(m_fDanceTime*m_fDanceWeight)*0.1f);
		m_matWorld1		= m_matWorld*m_matWorld1;
		m_fDanceTime	-= fElapsedTime;
		if( m_fDanceTime < 0.f )
		{
			m_bNowDance	= false;
		}
	}

	return		hr;
}

HRESULT		CNet::Render(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT		hr	= S_OK;

	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	pDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
	m_pillar.Render(pDevice);

	pDevice->SetTransform( D3DTS_WORLD1, &m_matWorld1 );
	pDevice->SetRenderState( D3DRS_ALPHAFUNC,	D3DCMP_GREATER);
	pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_ALPHAREF,	0x00000030 );	
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	pDevice->SetRenderState( D3DRS_VERTEXBLEND, m_bNowDance?D3DVBF_1WEIGHTS:D3DVBF_DISABLE );
	m_net.Render(pDevice);
	pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );

	return		hr;
}
HRESULT		CNet::ComputeBlendFactor()
{
	HRESULT	hr	= S_OK;

	LPD3DXMESH	pMesh	= m_net.GetLocalMesh();

	SimpleBlendVertex1	*pVertices	= NULL;
	LPDIRECT3DVERTEXBUFFER9	pVB		= NULL;
	DWORD				dwNumVertices	= 0;
	float				fMin		= 3.4e38f;
	float				fMax		= -3.4e38f;
	float				scale		= 0.f;
	
	dwNumVertices	= pMesh->GetNumVertices();
	CHECK_FAILED(pMesh->GetVertexBuffer(&pVB));
	CHECK_FAILED(pVB->Lock(0, NULL, (void**)&pVertices, 0 ));

	for(DWORD i=0; i<dwNumVertices; ++i)
	{
		fMin	= fMin>pVertices[i].P.x? pVertices[i].P.x: fMin;
		fMax	= fMax<pVertices[i].P.x? pVertices[i].P.x: fMax;
	}

	scale	= fMax-fMin;
	for(DWORD i=0; i<dwNumVertices; ++i)
	{
		float	w	= (pVertices[i].P.x-fMin)/scale;
		pVertices[i].B1	= 1.f-sinf(w*D3DX_PI);
	}

	CHECK_FAILED(pVB->Unlock());
	SAFE_RELEASE(pVB);

	return	hr;
}
void		CNet::MakeNetDance(LPD3DXVECTOR3	vIncidence)
{
	//	1. 속도로 부터 네트가 뒤틀릴 축, 크기, 시간을 구한다.
	D3DXVECTOR3	UP	= D3DXVECTOR3(0.f, vIncidence->x>0.f?-1.f:1.f, 0.f);
	D3DXVec3Cross(&m_vAxis, vIncidence, &UP);

	float	fLength	= D3DXVec3Length(&m_vAxis);
	if( fLength < EPSILON )
		return;

	m_fDanceWeight	= 0.1f*fLength;
	m_fDanceTime	= 0.01f*fLength;

	m_bNowDance		= true;
}