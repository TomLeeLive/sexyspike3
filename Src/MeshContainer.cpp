
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/MeshContainer.h"

HRESULT	CMeshContainer::GenerateSkinnedMesh()
{
	HRESULT	hr	= S_OK;
	
	if( NULL == this->pSkinInfo )
		return hr;

	//	��Ű�װ������� ��ü ����
	SAFE_RELEASE( this->MeshData.pMesh );
	SAFE_RELEASE( this->m_pBoneCombinationBuf );

	//	��Ű�� ����� ���� ���� ��Ű�� ���� ���� ����
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

	//	���ؽ����� ������ �޽��� ����
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


	//	�޽��� ������ ��ġ�� ��(��Ʈ����) ���� �ϵ���� �������� ������ ����Ʈ���� ���ؽ� ���μ����� �ϵ��� �ٲ��ش�.
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
			//	�ش��ε����� ���� ���ǰ� �ִ��� ����( UINT_MAX �� ���x )
			if( pBoneCombinations[this->m_nAttributeSW].BoneId[i] != UINT_MAX )
			{
				++cInfl;
			}
		}

		//	�ϵ���� �������� ū�� ����
		if( cInfl > caps.MaxVertexBlendMatrices )
		{
			break;
		}		
	}

	//	�ϵ���� ���� ���� ���� ���� ����ϸ� ����Ʈ���� ���ؽ� ��μ����� �ϵ��� ����
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
	//	1.	IndexedBlendMesh�� ���۴�.
	//	2.	FVF����->�޽����ݿ�
	//	3.	UBYTE4���� Declaration����
	//	4.	MatrixPallet����

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