#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/Allocatehierarchy.h"
#include		"../Include/Frame.h"
#include		"../Include/MeshContainer.h"
#include		"../Include/Background.h"

CBackground::~CBackground()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();
}

HRESULT		CBackground::InitDeviceObjects( LPDIRECT3DDEVICE9	pDevice )
{
	HRESULT		hr	= S_OK;

	assert( pDevice );

	LPTSTR	pFilepath	= _T("./XFILE/TERRAIN/");
	LPCSTR	pFilename	= "./XFILE/TERRAIN/terrain.x";
	CAllocateHierarchy	Alloc;
	Alloc.SetXFilePath(pFilepath);

	hr	= D3DXLoadMeshHierarchyFromX(
			pFilename
		,	D3DXMESH_MANAGED
		,	pDevice
		,	&Alloc
		,	NULL
		,	&m_pFrameRoot
		,	&m_pAniController
		);
	CHECK_FAILED(hr);

	m_pAniController->SetTrackSpeed(0, 0.2f);

	return		hr;
}
HRESULT		CBackground::RestoreDeviceObjects()
{
	HRESULT		hr	= S_OK;
	D3DXMatrixRotationY( &m_matWorld, D3DXToRadian( 90 ) );
	m_matWorld._41 = -10;
	m_matWorld._42 = -0.1f;
	m_matWorld._43 =   5;
	return		hr;
}
HRESULT		CBackground::InvalidateDeviceObjects()
{
	HRESULT		hr	= S_OK;
	return		hr;
}
HRESULT		CBackground::DeleteDeviceObjects()
{
	HRESULT		hr	= S_OK;

	SAFE_RELEASE(m_pAniController);
	if(NULL!=m_pFrameRoot)
	{
		CAllocateHierarchy	Alloc;
		D3DXFrameDestroy(m_pFrameRoot, &Alloc);
		m_pFrameRoot	= NULL;
	}
	return		hr;
}

HRESULT		CBackground::FrameMove(float fElapsedTime)
{
	HRESULT		hr	= S_OK;
	m_pAniController->AdvanceTime(fElapsedTime, NULL);
	UpdateFrameMatrices( m_pFrameRoot,  &m_matWorld );
	return		hr;
}

HRESULT		CBackground::Render(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT		hr	= S_OK;
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0x80);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	CHECK_FAILED( DrawFrame(pDevice, m_pFrameRoot));
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return		hr;
}
void		CBackground::UpdateFrameMatrices(LPD3DXFRAME pFrame, const LPD3DXMATRIX pParentMatrix)
{
	CFrame			*pFrame_Derived		= (CFrame*) pFrame;

	if( NULL != pParentMatrix )
	{
		D3DXMatrixMultiply( &pFrame_Derived->m_CombinedTransformationMatrix, &pFrame_Derived->TransformationMatrix, pParentMatrix );
	}
	else
	{
		pFrame_Derived->m_CombinedTransformationMatrix	= pFrame_Derived->TransformationMatrix;
	}

	if( NULL != pFrame_Derived->pFrameSibling )
	{
		UpdateFrameMatrices( pFrame_Derived->pFrameSibling, pParentMatrix );
	}

	if( NULL != pFrame_Derived->pFrameFirstChild )
	{
		UpdateFrameMatrices( pFrame_Derived->pFrameFirstChild, &pFrame_Derived->m_CombinedTransformationMatrix );
	}
}

HRESULT		CBackground::DrawFrame(LPDIRECT3DDEVICE9	pDevice,  LPD3DXFRAME	pFrame )
{
	HRESULT	hr	= S_OK;

	LPD3DXMESHCONTAINER	pMeshContainer	= pFrame->pMeshContainer;

	while( NULL != pMeshContainer )
	{
		hr	= DrawMeshContainer(pDevice, pFrame, pMeshContainer );
		CHECK_FAILED( hr );

		pMeshContainer	= pMeshContainer->pNextMeshContainer;
	}

	if( pFrame->pFrameSibling )
	{
		hr = DrawFrame(pDevice, pFrame->pFrameSibling );
		CHECK_FAILED( hr );
	}

	if( pFrame->pFrameFirstChild )
	{
		hr	= DrawFrame(pDevice, pFrame->pFrameFirstChild );
		CHECK_FAILED( hr );
	}

	return hr;
}

HRESULT		CBackground::DrawMeshContainer(LPDIRECT3DDEVICE9	pDevice,  LPD3DXFRAME pFrameBase, LPD3DXMESHCONTAINER	pSuperMeshContainer )
{
	HRESULT	hr	= S_OK;

	CFrame	*pFrame					= (CFrame*) pFrameBase;
	CMeshContainer	*pMeshContainer	= (CMeshContainer*) pSuperMeshContainer;
	D3DXMATRIX	matWorld= pFrame->m_CombinedTransformationMatrix;

	for( DWORD i=0; i<pMeshContainer->NumMaterials; ++i )
	{
		CHECK_FAILED(pDevice->SetTransform(D3DTS_WORLD, &matWorld));
		CHECK_FAILED(pDevice->SetMaterial(&pMeshContainer->m_pMaterials[i]));
		CHECK_FAILED(pDevice->SetTexture(0, pMeshContainer->m_ppTextures[i]));
		CHECK_FAILED(pMeshContainer->MeshData.pMesh->DrawSubset( i ));
	}

	return hr;
}
