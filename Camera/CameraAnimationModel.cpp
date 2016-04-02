
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/Allocatehierarchy.h"
#include		"../Include/Frame.h"
#include		"../Include/MeshContainer.h"
#include		"../Camera/CameraAnimationController.h"
#include		"../Camera/CameraAnimationModel.h"

CCameraAnimationModel::CCameraAnimationModel()
{
	m_pXFileName	= NULL;
	m_pd3dDevice	= NULL;
	m_pFrameRoot	= NULL;
	m_pAniController= NULL;
}

CCameraAnimationModel::CCameraAnimationModel( LPCTSTR	lpszXFileName )
{
	m_pXFileName	= NULL;
	m_pd3dDevice	= NULL;
	m_pFrameRoot	= NULL;
	m_pAniController= NULL;
	
	if( NULL != lpszXFileName && 0 != lstrlen(lpszXFileName) )
	{
		int length		= lstrlen( lpszXFileName ) + 1;
		m_pXFileName	= new TCHAR[ length ];
		if( NULL == m_pXFileName )
			return;

		lstrcpy( m_pXFileName, lpszXFileName );
	}
}

CCameraAnimationModel::~CCameraAnimationModel()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();

	SAFE_DELETE_ARRAY( m_pXFileName );
}




HRESULT	CCameraAnimationModel::SetupBoneMatrixPointers( LPD3DXFRAME	pFrame )
{
	HRESULT	hr = S_OK;

	LPD3DXMESHCONTAINER	pMeshContainer	= NULL;
	pMeshContainer	= pFrame->pMeshContainer;

	while( NULL != pMeshContainer )
	{
		hr = SetupBoneMatrixPointersOnMesh( pMeshContainer );
		CHECK_FAILED( hr );
		pMeshContainer	= pMeshContainer->pNextMeshContainer;
	}

	if( NULL != pFrame->pFrameSibling )
	{
		SetupBoneMatrixPointers( pFrame->pFrameSibling );
		CHECK_FAILED( hr );
	}

	if( NULL != pFrame->pFrameFirstChild )
	{
		SetupBoneMatrixPointers( pFrame->pFrameFirstChild );
		CHECK_FAILED( hr );
	}

	return hr;
}

HRESULT	CCameraAnimationModel::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	HRESULT	hr	= S_OK;
	CMeshContainer	*pMeshContainer	= (CMeshContainer*) pSuperMeshContainer;

	if( NULL != pMeshContainer->pSkinInfo )
	{
		DWORD	numBones	= pMeshContainer->pSkinInfo->GetNumBones();
		pMeshContainer->m_ppBoneMatrixPtrs	= NULL;
		pMeshContainer->m_ppBoneMatrixPtrs	= new LPD3DXMATRIX[ numBones ];

		if( NULL == pMeshContainer->m_ppBoneMatrixPtrs )
			return E_OUTOFMEMORY;

		for( DWORD i=0; i<numBones;	++i )
		{
			LPCSTR		pBoneName	= pMeshContainer->pSkinInfo->GetBoneName( i );
			LPD3DXFRAME	pFrame		= D3DXFrameFind( m_pFrameRoot, pBoneName );

			if( NULL == pFrame )
			{
				return	E_FAIL;				
			}
			pMeshContainer->m_ppBoneMatrixPtrs[i] = &( (CFrame*)pFrame )->m_CombinedTransformationMatrix;
		}
	}

	return	hr;
}

HRESULT	CCameraAnimationModel::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT	hr		= S_OK;

	if( NULL == pd3dDevice )
		return	E_INVALIDARG;

	m_pd3dDevice	= pd3dDevice;
	m_pd3dDevice->AddRef();

	//	커스텀 애니메신컨트롤러 를 생성
	m_pAniController	= new CCameraAnimationController;

	//	X파이을 읽어들여 애니메이션 가능한 모델을 만든다.
	if( NULL == m_pXFileName )
		return E_FAIL;

	CAllocateHierarchy	alloc;
	alloc.SetCreationFlag( TRUE );
	alloc.SetDwFrame( 0 );
	alloc.SetDwMeshContainer( 0 );
	alloc.SetFrameVector( &vFrame );
	alloc.SetMeshContainerVector( &vMeshContainer );

	hr = D3DXLoadMeshHierarchyFromX( 
						m_pXFileName
					,	D3DXMESH_MANAGED
					,	m_pd3dDevice
					,	&alloc
					,	NULL
					,	&m_pFrameRoot
					,	&m_pAniController->m_pAniController
					);

	CHECK_FAILED( hr );

	hr = SetupBoneMatrixPointers( m_pFrameRoot );
	CHECK_FAILED( hr );

	m_pAniController->InitDeviceObjects( m_pd3dDevice );

	return hr;
}

HRESULT	CCameraAnimationModel::RestoreDeviceObjects()
{
	HRESULT	hr	= S_OK;

	m_pAniController->RestoreDeviceObjects();

	return	hr;
}

HRESULT	CCameraAnimationModel::InvalidateDeviceObjects()
{
	HRESULT	hr	= S_OK;

	m_pAniController->InvalidateDeviceObjects();

	return hr;
}

HRESULT	CCameraAnimationModel::DeleteDeviceObjects()
{
	HRESULT	hr	= S_OK;

	SAFE_RELEASE( m_pd3dDevice );	

	if( m_pFrameRoot )
	{
		CAllocateHierarchy alloc;
		hr = D3DXFrameDestroy( m_pFrameRoot, &alloc );
		m_pFrameRoot	= NULL;
	}

	m_pAniController->DeleteDeviceObjects();

	return hr;
}

void	CCameraAnimationModel::UpdateFrameMatrices(LPD3DXFRAME pFrame, const LPD3DXMATRIX pParentMatrix)
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

HRESULT	CCameraAnimationModel::FrameMove( float fElapsedTime, LPD3DXMATRIX pTM)
{
	HRESULT	hr = S_OK;

	m_pAniController->FrameMove( fElapsedTime );

	UpdateFrameMatrices( m_pFrameRoot, pTM );

	return hr;
}

HRESULT	CCameraAnimationModel::Render()
{
	HRESULT	hr	= S_OK;

	hr	= m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	CHECK_FAILED( hr );
	hr	= m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );
	CHECK_FAILED( hr );
	hr	= m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0x10000000 );
	CHECK_FAILED( hr );

	hr	= DrawFrame( m_pFrameRoot );
	CHECK_FAILED( hr );

	hr	= m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	CHECK_FAILED( hr );

	return	hr;
}

HRESULT	CCameraAnimationModel::DrawFrame( LPD3DXFRAME	pFrame )
{
	HRESULT	hr	= S_OK;

	LPD3DXMESHCONTAINER	pMeshContainer	= pFrame->pMeshContainer;

	while( NULL != pMeshContainer )
	{
		hr	= DrawMeshContainer( pFrame, pMeshContainer );
		CHECK_FAILED( hr );

		pMeshContainer	= pMeshContainer->pNextMeshContainer;
	}

	if( pFrame->pFrameSibling )
	{
		hr = DrawFrame( pFrame->pFrameSibling );
		CHECK_FAILED( hr );
	}

	if( pFrame->pFrameFirstChild )
	{
		hr	= DrawFrame( pFrame->pFrameFirstChild );
		CHECK_FAILED( hr );
	}

	return hr;
}

HRESULT	CCameraAnimationModel::DrawMeshContainer( LPD3DXFRAME pFrameBase, LPD3DXMESHCONTAINER	pSuperMeshContainer )
{
	HRESULT	hr	= S_OK;

	if( NULL != pSuperMeshContainer->pSkinInfo )
	{
		//	스키닝 방법에 따라 각각 다른 방법으로 렌더링
		switch( SKINNING_TYPE )
		{
			case NONINDEXED:
				hr = DrawMeshContainerByNonIndexed(pSuperMeshContainer);
				break;
			case INDEXED:
				hr = DrawMeshContainerByIndexed(pSuperMeshContainer);
				break;
			case VS:
				hr = DrawMeshContainerByVS(pSuperMeshContainer);
				break;
			case HLSL:
				hr = DrawMeshContainerByHLSL(pSuperMeshContainer);
				break;
			case SW:
				hr = DrawMeshContainerBySW(pSuperMeshContainer);
				break;
		}
		CHECK_FAILED( hr );
	}
	else
	{
		CFrame	*pFrame					= (CFrame*) pFrameBase;
		CMeshContainer	*pMeshContainer	= (CMeshContainer*) pSuperMeshContainer;

		m_pd3dDevice->SetTransform( D3DTS_WORLD, &pFrame->m_CombinedTransformationMatrix );

		for( DWORD i=0; i<pMeshContainer->NumMaterials; ++i )
		{
			hr	= m_pd3dDevice->SetMaterial( &pMeshContainer->m_pMaterials[i] );
			CHECK_FAILED( hr );
			hr	= m_pd3dDevice->SetTexture( 0, pMeshContainer->m_ppTextures[i] );
			CHECK_FAILED( hr );

			hr	= pMeshContainer->MeshData.pMesh->DrawSubset( i );
			CHECK_FAILED( hr );
		}
	}

	return hr;
}

HRESULT	CCameraAnimationModel::DrawMeshContainerByNonIndexed(LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	HRESULT	hr = S_OK;

	CMeshContainer			*pMeshContainer		= NULL;
	LPD3DXBONECOMBINATION	pBoneCombinations	= NULL;
	D3DXMATRIX				iMatWorld;

	pMeshContainer		= (CMeshContainer*)pSuperMeshContainer;
	pBoneCombinations	= (LPD3DXBONECOMBINATION) pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer();

	if( NULL == pBoneCombinations )
		return	hr;

	//	HW VP
	for( DWORD i=0; i< pMeshContainer->m_nAttributeSW; ++i )
	{
		DWORD	numBlend	= 0;
		for( DWORD h=0; h< pMeshContainer->m_numInfl; ++h )
		{
			if( pBoneCombinations[i].BoneId[h] != UINT_MAX )
			{
				numBlend = h;

				DWORD	iBone = pBoneCombinations[i].BoneId[h];
				D3DXMatrixMultiply( 
						&iMatWorld
					,	&pMeshContainer->m_pBoneOffsetMatrices[iBone]
					,	pMeshContainer->m_ppBoneMatrixPtrs[iBone]
				);

				m_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( h ), &iMatWorld );				
			}
		}

		m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, numBlend );

		m_pd3dDevice->SetMaterial(	&pMeshContainer->m_pMaterials[ pBoneCombinations[i].AttribId ] );
		m_pd3dDevice->SetTexture( 0, pMeshContainer->m_ppTextures[ pBoneCombinations[i].AttribId ] );

		pMeshContainer->MeshData.pMesh->DrawSubset(i);
	}

	//	SW VP
	//	소프트웨어 버텍스프로세싱을 하는 것 말고는 위랑 똑같은 동작을 한다.
	for( DWORD i=pMeshContainer->m_nAttributeSW; i< pMeshContainer->m_numAttributeGroups; ++i )
	{
		m_pd3dDevice->SetSoftwareVertexProcessing( TRUE );
		DWORD	numBlend	= 0;
		for( DWORD h=0; h< pMeshContainer->m_numInfl; ++h )
		{
			if( pBoneCombinations[i].BoneId[h] != UINT_MAX )
			{
				++numBlend;

				DWORD	iBone = pBoneCombinations[i].BoneId[h];
				D3DXMatrixMultiply( 
						&iMatWorld
					,	&pMeshContainer->m_pBoneOffsetMatrices[iBone]
					,	pMeshContainer->m_ppBoneMatrixPtrs[iBone]
				);

				m_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( iBone ), &iMatWorld );				
			}
		}

		m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, numBlend );

		m_pd3dDevice->SetMaterial(	&pMeshContainer->m_pMaterials[i] );
		m_pd3dDevice->SetTexture( i, pMeshContainer->m_ppTextures[i] );

		pMeshContainer->MeshData.pMesh->DrawSubset(i);
	}
	m_pd3dDevice->SetSoftwareVertexProcessing( FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, FALSE );

	return	hr;
}

HRESULT	CCameraAnimationModel::DrawMeshContainerByIndexed(LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	HRESULT	hr = S_OK;

	return	hr;
}

HRESULT	CCameraAnimationModel::DrawMeshContainerByVS(LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	HRESULT	hr = S_OK;

	return	hr;
}

HRESULT	CCameraAnimationModel::DrawMeshContainerByHLSL(LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	HRESULT	hr = S_OK;

	return	hr;
}

HRESULT	CCameraAnimationModel::DrawMeshContainerBySW(LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	HRESULT	hr = S_OK;

	return	hr;
}

LRESULT	CCameraAnimationModel::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT	ret	= m_pAniController->HandleMessages( hWnd, uMsg, wParam, lParam );
	
	if( ret==-1 )
		return ret;

	return	0;
}