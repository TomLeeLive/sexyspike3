
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/MeshContainer.h"

HRESULT	CMeshContainer::GenerateSkinnedMesh()
{
	HRESULT	hr	= S_OK;
	
	if( NULL == this->pSkinInfo )
		return hr;

	//	스키닝관련정보 객체 해제
	SAFE_RELEASE( this->MeshData.pMesh );
	SAFE_RELEASE( this->m_pBoneCombinationBuf );

	//	스키닝 방법에 따라 각각 스키닝 관련 정보 생성
	switch( SKINNING_TYPE )
	{
		case NONINDEXED:
			hr = GenerateSkinnedMeshByNonIndexed();
			break;
		case INDEXED:
			hr = GenerateSkinnedMeshByIndexed();
			break;
		case VS:
			hr = GenerateSkinnedMeshByVS();
			break;
		case HLSL:
			hr = GenerateSkinnedMeshByHLSL();
			break;
		case SW:
			hr = GenerateSkinnedMeshBySW();
			break;
	}

	return hr;
}

HRESULT	CMeshContainer::GenerateSkinnedMeshByNonIndexed()
{
	HRESULT	hr	= S_OK;

	//	버텍스블렌딩 가능한 메쉬로 변경
	//	Takes a mesh and returns a new mesh with per-vertex blend weights and a bone combination table. The table describes which bones affect which subsets of the mesh.
	hr = this->pSkinInfo->ConvertToBlendedMesh(
					this->m_pOriginalMesh
				,	this->m_pOriginalMesh->GetOptions() | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE
				,	this->pAdjacency
				,	NULL
				,	NULL
				,	NULL
				,	&this->m_numInfl
				,	&this->m_numAttributeGroups
				,	&(this->m_pBoneCombinationBuf)
				,	&(this->MeshData.pMesh)
				);

	if( FAILED(hr) )
		return hr;


	//	메쉬에 영향을 미치는 본(매트릭스) 수가 하드웨어 지원보다 많으면 소프트웨어 버텍스 프로세싱을 하도록 바꿔준다.
	LPD3DXBONECOMBINATION	pBoneCombinations	= (LPD3DXBONECOMBINATION)this->m_pBoneCombinationBuf->GetBufferPointer();

	LPDIRECT3DDEVICE9	pDevice = NULL;
	this->MeshData.pMesh->GetDevice( &pDevice );
	D3DCAPS9			caps;
	ZeroMemory( &caps, sizeof( D3DCAPS9 ) );
	pDevice->GetDeviceCaps( &caps );

	for( this->m_nAttributeSW=0; this->m_nAttributeSW < this->m_numAttributeGroups; ++m_nAttributeSW )
	{
		DWORD	cInfl	= 0;
		for( DWORD i=0; i<this->m_numInfl; ++i )
		{
			//	해당인덱스에 본이 사용되고 있는지 조사( UINT_MAX 면 사용x )
			if( pBoneCombinations[this->m_nAttributeSW].BoneId[i] != UINT_MAX )
			{
				++cInfl;
			}
		}

		//	하드웨어 지원보다 큰지 조사
		if( cInfl > caps.MaxVertexBlendMatrices )
		{
			break;
		}		
	}

	//	하드웨워 지원 보다 많은 본을 사용하면 소프트웨어 버텍스 페로세싱을 하도록 변경
	if( this->m_nAttributeSW < this->m_numAttributeGroups )
	{
		LPD3DXMESH	pMeshTemp = NULL;

		hr = this->MeshData.pMesh->CloneMeshFVF(
					D3DXMESH_SOFTWAREPROCESSING | this->MeshData.pMesh->GetOptions()
				,	this->MeshData.pMesh->GetFVF()
				,	pDevice
				,	&pMeshTemp
				);

		if( FAILED(hr) )
		{
			SAFE_RELEASE( pDevice );
			return hr;
		}

		SAFE_RELEASE( this->MeshData.pMesh );
		this->MeshData.pMesh	= pMeshTemp;
		pMeshTemp				= NULL;
	}

	SAFE_RELEASE( pDevice );

	return	hr;
}

HRESULT	CMeshContainer::GenerateSkinnedMeshByIndexed()
{
	HRESULT	hr	= S_OK;
	return	hr;
}

HRESULT	CMeshContainer::GenerateSkinnedMeshByVS()
{
	HRESULT	hr	= S_OK;
	return	hr;
}

HRESULT	CMeshContainer::GenerateSkinnedMeshByHLSL()
{
	//	1.	IndexedBlendMesh로 빠꾼다.
	//	2.	FVF수정->메쉬에반영
	//	3.	UBYTE4관련 Declaration수정
	//	4.	MatrixPallet수정

	HRESULT	hr	= S_OK;
	
	//	1.
	this->m_numPaletteEntries	= this->pSkinInfo->GetNumBones();
	hr	= this->pSkinInfo->ConvertToIndexedBlendedMesh(
				this->m_pOriginalMesh
			,	D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE
			,	this->m_numPaletteEntries
			,	this->pAdjacency
			,	NULL
			,	NULL
			,	NULL
			,	&this->m_numInfl
			,	&this->m_numAttributeGroups
			,	&this->m_pBoneCombinationBuf
			,	&this->MeshData.pMesh
			);
	CHECK_FAILED( hr );

	//	2.
	DWORD	NewFVF	= (this->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1;
	if( NewFVF != this->MeshData.pMesh->GetFVF() )
	{
		LPDIRECT3DDEVICE9	pDevice	= NULL;
		LPD3DXMESH			pMesh	= NULL;
		CHECK_FAILED(this->MeshData.pMesh->GetDevice( &pDevice ));

		hr	= this->MeshData.pMesh->CloneMeshFVF(
				this->MeshData.pMesh->GetOptions()
			,	NewFVF
			,	pDevice
			,	&pMesh
			);

		SAFE_RELEASE( pDevice );
		CHECK_FAILED(hr);

		SAFE_RELEASE( this->MeshData.pMesh );
		this->MeshData.pMesh	= pMesh;
		pMesh					= NULL;
	}

	//	3.
	D3DVERTEXELEMENT9	decls[MAX_FVF_DECL_SIZE];
	D3DVERTEXELEMENT9	*pDecl	= NULL;
	CHECK_FAILED(this->MeshData.pMesh->GetDeclaration( decls ) );
	pDecl	= decls;
	while( pDecl->Stream	!= 0xFF )
	{
		if( (pDecl->Usage==D3DDECLUSAGE_BLENDINDICES) && (pDecl->UsageIndex==0) )
		{
			pDecl->Type = D3DDECLTYPE_D3DCOLOR;
		}
		++pDecl;
	}
	CHECK_FAILED( this->MeshData.pMesh->UpdateSemantics(decls));

	//	4.
	if( this->m_numPaletteEntries > g_numPalette )
	{
		SAFE_DELETE_ARRAY( g_pPalette );
		g_numPalette= this->m_numPaletteEntries;
		g_pPalette	= new D3DXMATRIX[ g_numPalette ];
		if( NULL == g_pPalette )
		{
			return	E_OUTOFMEMORY;
		}
		for( DWORD i=0; i<g_numPalette; ++i )
		{
			g_pPalette[i] = UNIT_MATRIX;
		}
	}

	return	hr;
}

HRESULT	CMeshContainer::GenerateSkinnedMeshBySW()
{
	HRESULT	hr	= S_OK;
	return	hr;
}