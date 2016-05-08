
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/Allocatehierarchy.h"
#include		"../Include/Frame.h"
#include		"../Include/MeshContainer.h"
#include		"../Include/CharacterAnimationController.h"
#include		"../Include/Ball.h"
#include		"../Include/Character.h"

Character::Character( LPCTSTR pCharacterFilePath, LPTSTR* ppCharacterFiles, int cntCharacterFiles )
{
	m_pFrameRoot			= NULL;
	m_pAniController		= NULL;
	m_ppCharacterFiles		= NULL;
	m_pCharacterFilePath	= NULL;
	m_pBall					= NULL;

	m_bDrawShadow			= false;

	m_bFrameOwner			= FALSE;
	m_bNowJumping			= FALSE;
	m_fElapsedTime			= 0.f;
	m_fRotationAngle		= 0.f;
	m_cntCharacterFiles		= cntCharacterFiles;
	
	ZeroMemory( &m_ability, sizeof( CharacterAbility ) );
	ZeroMemory( &m_BoundingBody, sizeof( BoundingSphere) );
	ZeroMemory( &m_BoundingHand, sizeof( BoundingSphere) );
	ZeroMemory( &m_BoundingSpike, sizeof( BoundingCylinder) );
	ZeroMemory( &m_BoundingDefense, sizeof( BoundingCylinder) );
	ZeroMemory( &m_vReceivePoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vSlidingPoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vSpikePoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vTossPoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vOverServePoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vUnderServePoint,	sizeof( D3DXVECTOR3 ) );

	D3DXMatrixIdentity( &m_matS );
	D3DXMatrixIdentity( &m_matR );
	D3DXMatrixIdentity( &m_matT );
	D3DXMatrixIdentity( &m_matWorld);
	

	if( 0 == cntCharacterFiles )
		return;

	//	X파일명 복사
	m_ppCharacterFiles	= new LPTSTR[ m_cntCharacterFiles ];
	ZeroMemory( m_ppCharacterFiles, sizeof( LPTSTR ) * m_cntCharacterFiles );

	CopyString( pCharacterFilePath, &m_pCharacterFilePath );

	for( int i=0; i<m_cntCharacterFiles; ++i )
	{
		CopyString( ppCharacterFiles[i], &m_ppCharacterFiles[i] );
	}

}

Character::Character( const	Character& SrcModel )
{
	m_pFrameRoot		= SrcModel.m_pFrameRoot;
	m_bFrameOwner		= FALSE;
	m_pAniController	= new CharacterAnimationController( *SrcModel.m_pAniController );
	m_fElapsedTime		= 0.f;
	m_pCharacterFilePath= NULL;
	m_ppCharacterFiles	= NULL;
	m_cntCharacterFiles	= 0;
	m_bNowJumping		= FALSE;
	m_fRotationAngle	= 0.f;
	m_pBall				= SrcModel.m_pBall;
	m_bDrawShadow		= false;
	
	ZeroMemory( &m_ability, sizeof( CharacterAbility ) );
	ZeroMemory( &m_BoundingBody, sizeof( BoundingSphere) );
	ZeroMemory( &m_BoundingHand, sizeof( BoundingSphere) );
	ZeroMemory( &m_BoundingSpike, sizeof( BoundingCylinder) );
	ZeroMemory( &m_BoundingDefense, sizeof( BoundingCylinder) );
	ZeroMemory( &m_vReceivePoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vSlidingPoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vSpikePoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vTossPoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vOverServePoint,	sizeof( D3DXVECTOR3 ) );
	ZeroMemory( &m_vUnderServePoint,	sizeof( D3DXVECTOR3 ) );

	D3DXMatrixIdentity( &m_matS );
	D3DXMatrixIdentity( &m_matR );
	D3DXMatrixIdentity( &m_matT );
	D3DXMatrixIdentity( &m_matWorld);
}
Character::~Character()
{
	InvalidateDeviceObjects();
	DeleteDeviceObjects();

	for( int i=0; i<m_cntCharacterFiles; ++i )
	{
		SAFE_DELETE_ARRAY( m_ppCharacterFiles[i] );
	}
	
	SAFE_DELETE_ARRAY( m_pCharacterFilePath );
	SAFE_DELETE_ARRAY( m_ppCharacterFiles );
}

HRESULT	Character::SetupBoneMatrixPointers( LPD3DXFRAME	pFrame )
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

HRESULT	Character::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pSuperMeshContainer)
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

HRESULT	Character::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT	hr		= S_OK;

	if( NULL == pd3dDevice )
		return	E_INVALIDARG;

	m_bFrameOwner	= TRUE;

	m_matS		= UNIT_MATRIX;
	m_matR		= UNIT_MATRIX;
	m_matT		= UNIT_MATRIX;
	m_matWorld	= UNIT_MATRIX;	

	m_ability.Spike	= 9;

	//	커스텀 애니메이션컨트롤러 를 생성
	m_pAniController	= new CharacterAnimationController(	m_cntCharacterFiles ) ;

	//	X파이을 읽어들여 애니메이션 가능한 모델을 만든다.
	assert( m_ppCharacterFiles );

	CAllocateHierarchy	alloc;
	std::vector< LPD3DXFRAME > V_Frame;
	std::vector< LPD3DXMESHCONTAINER > V_MeshContainer;

	//	새로운 프레임을 생성하도록 옵션조절
	alloc.SetCreationFlag(TRUE);
	alloc.SetDwFrame( 0 );
	alloc.SetDwMeshContainer( 0 );
	alloc.SetFrameVector( &V_Frame );
	alloc.SetMeshContainerVector( &V_MeshContainer );
	alloc.SetXFilePath( m_pCharacterFilePath );

	hr = D3DXLoadMeshHierarchyFromX(
						m_ppCharacterFiles[0]
					,	D3DXMESH_MANAGED
					,	pd3dDevice
					,	&alloc
					,	NULL
					,	&m_pFrameRoot
					,	&( (m_pAniController->GetAnimaitionControllerPointer())[0] )
					);

	CHECK_FAILED( hr );

	for( int i=1; i<m_cntCharacterFiles; ++i )
	{
		//	새로운프레임을 생선 안 하도록 옵션 조절
		alloc.SetCreationFlag(FALSE);
		alloc.SetDwFrame( 0 );
		alloc.SetDwMeshContainer( 0 );

		LPD3DXFRAME	pFrame	= NULL;
		hr = D3DXLoadMeshHierarchyFromX(
						m_ppCharacterFiles[i]
					,	D3DXMESH_MANAGED
					,	pd3dDevice
					,	&alloc
					,	NULL
					,	&m_pFrameRoot
					,	&( (m_pAniController->GetAnimaitionControllerPointer())[i] )
					);
		CHECK_FAILED( hr );
	}

	hr = SetupBoneMatrixPointers( m_pFrameRoot );
	CHECK_FAILED( hr );

	hr	= m_pAniController->InitDeviceObjects( pd3dDevice );
	CHECK_FAILED( hr );

	//	경계구 생성
	hr	= CreateBoundingObjects();
	CHECK_FAILED( hr );

	//	각 액션에 따른 손위치 계산
	ComputeValleyActionPoint(1.5f*D3DX_PI);

	m_BoundingBody.fRadius	+= -4.f;

	for( DWORD i=0; i<V_Frame.size(); ++i )
	{
		V_Frame[i] = NULL;
	}
	V_Frame.clear();

	for( DWORD i=0; i<V_MeshContainer.size(); ++i )
	{
		V_MeshContainer[i]  = NULL;
	}
	V_MeshContainer.clear();

	return hr;
}

HRESULT	Character::RestoreDeviceObjects()
{
	HRESULT	hr	= S_OK;

	m_pAniController->RestoreDeviceObjects();

	return	hr;
}

HRESULT	Character::InvalidateDeviceObjects()
{
	HRESULT	hr	= S_OK;

	m_pAniController->InvalidateDeviceObjects();

	return hr;
}

HRESULT	Character::DeleteDeviceObjects()
{
	HRESULT	hr	= S_OK;

	m_pAniController->DeleteDeviceObjects();

	if( m_bFrameOwner && m_pFrameRoot )
	{
		CAllocateHierarchy alloc;
		hr = D3DXFrameDestroy( m_pFrameRoot, &alloc );
		m_pFrameRoot	= NULL;
	}
	else
	{
		m_pFrameRoot	= NULL;
	}

	return hr;
}


//09-08-25 이재준
HRESULT	Character::FrameMove( float fElapsedTime )
{
	HRESULT	hr = S_OK;

	float fMove = fElapsedTime * 0.6f;

	//	애니메이션 상태 갱신( 여러 인스턴스에서 같이 사용하므로 FrameMove와 Render두군데서 갱신한다.
	m_fElapsedTime	= fElapsedTime;	

	D3DXMatrixRotationY( &m_matR, m_fRotationAngle );
	m_matWorld	= m_matS*m_matR*m_matT;

	m_pAniController->FrameMove(fMove);
	UpdateFrameMatrices( m_pFrameRoot,  &m_matWorld );	

	m_BoundingBody.UpdateBoundingCenter();
	m_BoundingHand.UpdateBoundingCenter();
	m_BoundingSpike.UpdateBoundingCenter();
	m_BoundingDefense.UpdateBoundingCenter();

	return hr;
}

HRESULT	Character::Render(LPDIRECT3DDEVICE9	pDevice)
{
	HRESULT	hr	= S_OK;

	hr	= pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	CHECK_FAILED( hr );
	hr	= pDevice->SetRenderState( D3DRS_ALPHAREF, 0x00000010 );
	CHECK_FAILED( hr );
	hr	= pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	CHECK_FAILED( hr );

	hr	= pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	hr	= pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	hr	= pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

	hr	= pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	hr	= pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	hr	= pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	m_pAniController->FrameMove( 0.0f );
	UpdateFrameMatrices( m_pFrameRoot,  &m_matWorld );

	hr	= DrawFrame(pDevice, m_pFrameRoot );
	CHECK_FAILED( hr );

	hr	= pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	CHECK_FAILED( hr );

	return	hr;
}

HRESULT	Character::DrawFrame(LPDIRECT3DDEVICE9	pDevice,  LPD3DXFRAME	pFrame )
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

HRESULT	Character::DrawMeshContainer(LPDIRECT3DDEVICE9	pDevice,  LPD3DXFRAME pFrameBase, LPD3DXMESHCONTAINER	pSuperMeshContainer )
{
	HRESULT	hr	= S_OK;

	if( NULL != pSuperMeshContainer->pSkinInfo )
	{
		//	스키닝 방법에 따라 각각 다른 방법으로 렌더링
		switch( SKINNING_TYPE )
		{
			case NONINDEXED:
				hr = DrawMeshContainerByNonIndexed(pDevice,pSuperMeshContainer);
				break;
			case INDEXED:
				hr = DrawMeshContainerByIndexed(pSuperMeshContainer);
				break;
			case VS:
				hr = DrawMeshContainerByVS(pSuperMeshContainer);
				break;
			case HLSL:
				hr = DrawMeshContainerByHLSL(pDevice,pSuperMeshContainer);
				break;
			case SW:
				hr = DrawMeshContainerBySW(pSuperMeshContainer);
				break;
		}
		CHECK_FAILED( hr );
	}
	else
	{
		switch( SKINNING_TYPE )
		{
			case NONINDEXED:
				{
				}
				break;
			case HLSL:
				{
					CFrame	*pFrame					= (CFrame*) pFrameBase;
					CMeshContainer	*pMeshContainer	= (CMeshContainer*) pSuperMeshContainer;
					LPD3DXEFFECT	pEffect			= g_Effects["CHARACTER"];

					D3DXMATRIX	matWorld= pFrame->m_CombinedTransformationMatrix;
					D3DXMATRIX	matShadow=matWorld * g_matShadow;
					pEffect->SetMatrix( "ViewProj", &g_matViewProj );

					D3DXVECTOR3	vDir	= g_light.Direction;
					D3DXVECTOR4	vLight	= D3DXVECTOR4( -vDir.x, -vDir.y, -vDir.z, 0.f );
					pEffect->SetVector( "Light", &vLight );

					for( DWORD i=0; i<pMeshContainer->NumMaterials; ++i )
					{
						D3DMATERIAL9 material	= pMeshContainer->m_pMaterials[i];
						D3DXCOLOR	Diffuse	= material.Diffuse;
						D3DXCOLOR	Ambient	= D3DXCOLOR(
									material.Ambient.r*g_light.Ambient.r
								,	material.Ambient.g*g_light.Ambient.g
								,	material.Ambient.b*g_light.Ambient.b
								,	material.Ambient.a*g_light.Ambient.a
								);

						pEffect->SetVector( "ColorDiffuse", (D3DXVECTOR4*)&Diffuse );
						pEffect->SetVector( "ColorAmbient", (D3DXVECTOR4*)&Ambient );

						hr	= pDevice->SetTexture( 0, pMeshContainer->m_ppTextures[i] );
						CHECK_FAILED( hr );

						pEffect->SetTechnique( "IndexedSkinning" );
						pEffect->Begin( NULL, 0 );
						pEffect->SetMatrix( "World", &matWorld );
						pEffect->BeginPass(2);
						hr	= pMeshContainer->MeshData.pMesh->DrawSubset( i );
						pEffect->EndPass();
						pEffect->BeginPass(3);
						pEffect->SetMatrix( "World", &matShadow );
						hr	= pMeshContainer->MeshData.pMesh->DrawSubset( i );
						pEffect->EndPass();
						CHECK_FAILED( hr );
						pEffect->End();
					}

				}
				break;
		}
	}

	return hr;
}

HRESULT	Character::DrawMeshContainerByNonIndexed(LPDIRECT3DDEVICE9	pDevice, LPD3DXMESHCONTAINER pSuperMeshContainer)
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

				pDevice->SetTransform( D3DTS_WORLDMATRIX( h ), &iMatWorld );				
			}
		}

		pDevice->SetRenderState( D3DRS_VERTEXBLEND, numBlend );

		pDevice->SetMaterial(	&pMeshContainer->m_pMaterials[ pBoneCombinations[i].AttribId ] );
		pDevice->SetTexture( 0, pMeshContainer->m_ppTextures[ pBoneCombinations[i].AttribId ] );

		pMeshContainer->MeshData.pMesh->DrawSubset(i);
	}

	//	SW VP
	//	소프트웨어 버텍스프로세싱을 하는 것 말고는 위랑 똑같은 동작을 한다.
	for( DWORD i=pMeshContainer->m_nAttributeSW; i< pMeshContainer->m_numAttributeGroups; ++i )
	{
		pDevice->SetSoftwareVertexProcessing( TRUE );
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

				pDevice->SetTransform( D3DTS_WORLDMATRIX( iBone ), &iMatWorld );				
			}
		}

		pDevice->SetRenderState( D3DRS_VERTEXBLEND, numBlend );

		pDevice->SetMaterial(	&pMeshContainer->m_pMaterials[i] );
		pDevice->SetTexture( i, pMeshContainer->m_ppTextures[i] );

		pMeshContainer->MeshData.pMesh->DrawSubset(i);
	}
	pDevice->SetSoftwareVertexProcessing( FALSE );
	pDevice->SetRenderState( D3DRS_VERTEXBLEND, FALSE );

	return	hr;
}

HRESULT	Character::DrawMeshContainerByIndexed(LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	HRESULT	hr = S_OK;

	return	hr;
}

HRESULT	Character::DrawMeshContainerByVS(LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	HRESULT	hr = S_OK;

	return	hr;
}

HRESULT	Character::DrawMeshContainerByHLSL(LPDIRECT3DDEVICE9	pDevice, LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	//	메쉬의 각 서브셋(어트리뷰트그룹)을 돌면서 아래 과정을 진행
	//	1.	각 BlendWeight에 해당하는 매트릭스 팔레트를 설정한다.
	//	2.	이펙트 변수를 설정한다.
	//	3.	메쉬를 그린다.

	HRESULT	hr = S_OK;
	CMeshContainer			*pMeshContainer		= (CMeshContainer*)pSuperMeshContainer;
	LPD3DXBONECOMBINATION	pBoneCombination	= (LPD3DXBONECOMBINATION)pMeshContainer->m_pBoneCombinationBuf->GetBufferPointer();
	LPD3DXEFFECT			pEffect				= g_Effects["CHARACTER"];

	for( DWORD I=0; I<pMeshContainer->m_numAttributeGroups; ++I )
	{
		DWORD	AttrId	= pBoneCombination[I].AttribId;
		//	1.
		for( DWORD H=0; H<pMeshContainer->m_numPaletteEntries; ++H )
		{
			DWORD	iPalette	= pBoneCombination[I].BoneId[H];
			if( iPalette != UINT_MAX )
			{
				D3DXMatrixMultiply( &g_pPalette[H], &pMeshContainer->m_pBoneOffsetMatrices[iPalette], pMeshContainer->m_ppBoneMatrixPtrs[iPalette] );
			}
		}

		//	2.
		pEffect->SetInt( "CurrentNumWeights", pMeshContainer->m_numInfl-1 );
		pEffect->SetMatrixArray( "WorldArray", g_pPalette, g_numPalette );
		pEffect->SetMatrix( "ViewProj", &g_matViewProj );
		D3DXMATRIX	matSVP	= g_matShadow*g_matViewProj;
		pEffect->SetMatrix( "ShadowViewProj", &matSVP);

		D3DXVECTOR3	vDir	= g_light.Direction;
		D3DXVECTOR4	vLight	= D3DXVECTOR4( -vDir.x, -vDir.y, -vDir.z, 0.f );
		pEffect->SetVector( "Light", &vLight );

		D3DMATERIAL9 material	= pMeshContainer->m_pMaterials[AttrId];
		D3DXCOLOR	Diffuse	= material.Diffuse;
		D3DXCOLOR	Ambient	= D3DXCOLOR(
					material.Ambient.r*g_light.Ambient.r
				,	material.Ambient.g*g_light.Ambient.g
				,	material.Ambient.b*g_light.Ambient.b
				,	material.Ambient.a*g_light.Ambient.a
				);
		pEffect->SetVector( "ColorDiffuse", (D3DXVECTOR4*)&Diffuse );
		pEffect->SetVector( "ColorAmbient", (D3DXVECTOR4*)&Ambient );

		//	3.
		pDevice->SetTexture( 0, pMeshContainer->m_ppTextures[AttrId] );
		pEffect->SetTechnique( "IndexedSkinning" );
		CHECK_FAILED(pEffect->Begin( NULL, 0 ));
		CHECK_FAILED(pEffect->BeginPass(0));
		CHECK_FAILED(pMeshContainer->MeshData.pMesh->DrawSubset(AttrId));
		CHECK_FAILED(pEffect->EndPass());
		if( m_bDrawShadow )
		{
			CHECK_FAILED(pEffect->BeginPass(1));
			CHECK_FAILED(pMeshContainer->MeshData.pMesh->DrawSubset(AttrId));
			CHECK_FAILED(pEffect->EndPass());
		}
		CHECK_FAILED(pEffect->End());
	}

	return	hr;
}

HRESULT	Character::DrawMeshContainerBySW(LPD3DXMESHCONTAINER pSuperMeshContainer)
{
	HRESULT	hr = S_OK;

	return	hr;
}

void	Character::UpdateFrameMatrices(LPD3DXFRAME pFrame, const LPD3DXMATRIX pParentMatrix)
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

HRESULT	Character::CreateBoundingObjects()
{
	HRESULT	hr	= S_OK;

	//	컈릭터 몸체에 대한 경계구 생성
	hr	= D3DXFrameCalculateBoundingSphere( m_pFrameRoot, &m_BoundingBody.vCenter, &m_BoundingBody.fRadius);
	CHECK_FAILED( hr );

	//	경계구 센터와 이동행렬의 차이 저장
	m_BoundingBody.vDiff.x	= m_matT._41 - m_BoundingBody.vCenter.x;
	m_BoundingBody.vDiff.y	= m_matT._42 - m_BoundingBody.vCenter.y;
	m_BoundingBody.vDiff.z	= m_matT._43 - m_BoundingBody.vCenter.z;
	m_BoundingBody.pTM		= &m_matT;
	m_BoundingBody.fRadius	*= 0.5f;

	//	배구공하고 캐릭터 손의 충돌을 검출할 경계구 생성
	hr	= CreateHandBoundingMesh();
	CHECK_FAILED( hr );

	return	hr;
}
HRESULT	Character::CreateHandBoundingMesh()
{
	HRESULT		hr		= S_OK;

	LPD3DXMESH	pMesh	= NULL;
	FindMeshByName( m_pFrameRoot, "meshNode0_o01_P_Body", &pMesh, &m_BoundingHand.pTM);

	if( NULL == pMesh )
		return	E_FAIL;

	char	*pVB	= NULL;
	pMesh->LockVertexBuffer( 0, (void**)&pVB );

	hr	= D3DXComputeBoundingSphere(
					(D3DXVECTOR3*)pVB
				,	pMesh->GetNumVertices()
				,	D3DXGetFVFVertexSize( pMesh->GetFVF() )
				,	&m_BoundingHand.vCenter
				,	&m_BoundingHand.fRadius
				);				

	pMesh->UnlockVertexBuffer();
	CHECK_FAILED( hr );

	m_BoundingHand.fRadius	*= 0.7f;
	ZeroMemory( &m_BoundingHand.vDiff, sizeof( D3DXVECTOR3 ) );

	LPD3DXFRAME	pFrame	= D3DXFrameFind( m_pFrameRoot, "Anim_MatrixFrame_o01_J_Naka01_R" );
	m_BoundingHand.pTM	= &( (CFrame*)pFrame )->m_CombinedTransformationMatrix;

	return		hr;
}

void	Character::FindMeshByName( LPD3DXFRAME pFrame, LPCSTR pName, LPD3DXMESH *ppMesh, LPD3DXMATRIX *ppMat )
{
	CMeshContainer	*pMC	= (CMeshContainer*)pFrame->pMeshContainer;

	while( NULL!= pMC )
	{
		if( 0 == _strcmpi( pName, pMC->Name ) )
		{
			*ppMesh	= pMC->MeshData.pMesh;
			*ppMat	= &((CFrame*)pFrame)->m_CombinedTransformationMatrix;
			return;
		}

		pMC	= (CMeshContainer*)pMC->pNextMeshContainer;
	}

	if( NULL != pFrame->pFrameSibling )
	{
		FindMeshByName( pFrame->pFrameSibling, pName, ppMesh, ppMat );
	}

	if( NULL != pFrame->pFrameFirstChild )
		FindMeshByName( pFrame->pFrameFirstChild, pName, ppMesh, ppMat );

	return;
}

void	Character::ComputeValleyActionPoint(float	fRotationAngle)
{
	m_fRotationAngle	= fRotationAngle;
	D3DXMatrixRotationY( &m_matR, m_fRotationAngle );
	m_matWorld	= m_matS*m_matR*m_matT;

	memcpy( &m_BoundingDefense, &m_BoundingBody, sizeof( BoundingSphere ) );
	m_pAniController->SetAnimationSet( "RECEIVE" );
	m_pAniController->FrameMove( RECEIVETIME );
	UpdateFrameMatrices( m_pFrameRoot, &m_matWorld );
	m_vReceivePoint = D3DXVECTOR3( 
			m_BoundingHand.pTM->_41 - m_matT._41
		,	m_BoundingHand.pTM->_42 - m_matT._42
		,	m_BoundingHand.pTM->_43 - m_matT._43 );

	m_pAniController->SetAnimationSet( "TOSS" );
	m_pAniController->FrameMove( TOSSTIME );
	UpdateFrameMatrices( m_pFrameRoot, &m_matWorld );
	m_vTossPoint = D3DXVECTOR3(
			m_BoundingHand.pTM->_41 - m_matT._41
		,	m_BoundingHand.pTM->_42 - m_matT._42
		,	m_BoundingHand.pTM->_43 - m_matT._43 );

	m_BoundingDefense.vCenter.y	= RECEIVEPOINTLOW;
	m_BoundingDefense.fHeight	= 6*m_BoundingBody.fRadius - RECEIVEPOINTLOW;
	m_BoundingDefense.fRadius	= m_ability.Defense*2.f + 5.f;
	m_BoundingDefense.vDiff.y	+= m_BoundingBody.vCenter.y - m_BoundingDefense.vCenter.y;	

	m_pAniController->SetAnimationSet( "SLIDING" );
	m_pAniController->FrameMove( SLIDINGTIME );
	UpdateFrameMatrices( m_pFrameRoot, &m_matWorld );
	m_vSlidingPoint = D3DXVECTOR3(
			m_BoundingHand.pTM->_41 - m_matT._41
		,	m_BoundingHand.pTM->_42 - m_matT._42
		,	m_BoundingHand.pTM->_43 - m_matT._43 );

	memcpy( &m_BoundingSpike, &m_BoundingBody, sizeof( BoundingSphere ) );
	m_pAniController->SetAnimationSet( "SPIKE" );
	m_pAniController->FrameMove( SPIKETIME );
	UpdateFrameMatrices( m_pFrameRoot, &m_matWorld );
	m_vSpikePoint = D3DXVECTOR3(
			m_BoundingHand.pTM->_41 - m_matT._41
		,	m_BoundingHand.pTM->_42 - m_matT._42
		,	m_BoundingHand.pTM->_43 - m_matT._43 );
	m_BoundingSpike.vCenter.y	+= 20.f + m_ability.Spike;
	m_BoundingSpike.fHeight		= (float)m_ability.Spike;
	m_BoundingSpike.fRadius		=  m_ability.Spike*2.f - 4.f;
	m_BoundingSpike.vDiff.y		+= m_BoundingBody.vCenter.y - m_BoundingSpike.vCenter.y;

	m_pAniController->SetAnimationSet( "SERVE_OVER" );
	m_pAniController->FrameMove( SERVEOVERTIME );
	UpdateFrameMatrices( m_pFrameRoot, &m_matWorld );
	m_vOverServePoint = D3DXVECTOR3(
			m_BoundingHand.pTM->_41 - m_matT._41
		,	m_BoundingHand.pTM->_42 - m_matT._42
		,	m_BoundingHand.pTM->_43 - m_matT._43 );

	m_pAniController->SetAnimationSet( "SERVE_UNDER" );
	m_pAniController->FrameMove( SERVEUNDERTIME );
	UpdateFrameMatrices( m_pFrameRoot, &m_matWorld );
	m_vUnderServePoint = D3DXVECTOR3(
			m_BoundingHand.pTM->_41 - m_matT._41
		,	m_BoundingHand.pTM->_42 - m_matT._42
		,	m_BoundingHand.pTM->_43 - m_matT._43 );

	m_pAniController->SetAnimationSet( "BLOCKING" );
	m_pAniController->FrameMove( BLOCKINGTIME );
	UpdateFrameMatrices( m_pFrameRoot, &m_matWorld );
	m_vBlockingPoint = D3DXVECTOR3(
			m_BoundingHand.pTM->_41 - m_matT._41
		,	m_BoundingHand.pTM->_42 - m_matT._42
		,	m_BoundingHand.pTM->_43 - m_matT._43 );

	m_pAniController->SetAnimationSet( "WAITING" );
}