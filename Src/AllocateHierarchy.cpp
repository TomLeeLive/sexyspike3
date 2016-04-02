
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/Frame.h"
#include		"../Include/MeshContainer.h"
#include		"../Include/Allocatehierarchy.h"


//-----------------------------------------------------------------------------
//	Name : CreateFrame
//	DESC : Callback function used by the load hierarchy function to create each frame
//	Create a frame using the derived structure
//	Initialize the passed in frame to NULL
//	Put the name in the frame
//	Initialize the rest of the frame to NULL
//	Set the output frame to the one that we created
//-----------------------------------------------------------------------------
HRESULT	CAllocateHierarchy::CreateFrame( LPCSTR	name, LPD3DXFRAME* ppNewFrame )
{
	HRESULT		hr	= S_OK;

	if( !m_bCreationFlag )
	{
		*ppNewFrame	= (*m_p_V_Frame)[ m_dwFrame++ ];
		return	S_OK;
	}

	CFrame*		pNewFrame	= NULL;
	pNewFrame				= new CFrame;
	if( NULL == pNewFrame )
		return	E_OUTOFMEMORY;

	ZeroMemory( pNewFrame, sizeof CFrame );

	//	Frame 이름 복사
	if(	NULL == name || 0 == strlen( name ) )
	{
		pNewFrame->Name	= NULL;
	}
	else
	{
		size_t	length	= strlen( name ) + 1;
		CHAR*	pNewName	= NULL;
		pNewName			= new CHAR[ length ];
		if( NULL == pNewName )
		{	hr	= E_OUTOFMEMORY;
			goto	E_EXIT;
		}

		strcpy_s( pNewName, length, name );
		pNewFrame->Name	= pNewName;
	}

	//	기타 멤버 초기화
	D3DXMatrixIdentity( &pNewFrame->TransformationMatrix );
	D3DXMatrixIdentity( &pNewFrame->m_CombinedTransformationMatrix );

	pNewFrame->pMeshContainer	= NULL;
	pNewFrame->pFrameSibling	= NULL;
	pNewFrame->pFrameFirstChild	= NULL;

	*ppNewFrame	= pNewFrame;
	if(NULL != m_p_V_Frame)
	{
		m_p_V_Frame->push_back( pNewFrame );
	}
	//	밑에 E_EXIT에서 생성한 인스턴스가 해제 안되게 pNewFrame에 NULL을 할당한다.
	pNewFrame	= NULL;
E_EXIT:
	SAFE_DELETE( pNewFrame );
	return	hr;
}





//-----------------------------------------------------------------------------
//	Name : DestroyFrame
//	DESC : Callback function used by the D3DXDestroyFrame function
//	Convert the frame to the derived structure
//	Delete the name
//	Delete the frame
//-----------------------------------------------------------------------------
HRESULT	CAllocateHierarchy::DestroyFrame( LPD3DXFRAME	pFrameToFree )
{
	CFrame*	pFrame	= (CFrame*)pFrameToFree ;
	SAFE_DELETE_ARRAY( pFrame->Name );
	SAFE_DELETE( pFrame );
	pFrameToFree	= NULL;
	return	S_OK;
}

//-----------------------------------------------------------------------------
//	Name : CreateMeshContainer
//	DESC : Callback function used by the load hierarchy function to create each mesh.
//	Create a Temp mesh container using my derived structure 
//		Initialize passed in Container to NULL
//	Put the name in the mesh container
//	Put the mesh into the mesh container
//	Get the number of Faces for adjacency
//	Get the number of materials
//	Create the arrays for the materials and the textures
//	create the array for the adjacency data multiply by 3 because there are 
//		three adjacent triangles
//	Copy all the Adjacency data
//	Zero out the textures
//	Copy the materials
//		(Copy the material)
//		(Set the ambient color for the material)
//	Get the D3D device from the mesh
//	For all the materials
//		(Use D3DXCreateTextureFromFile to load the texture and put it)
//		(into the mesh container)
//
//	if(pSkinInfo)
//	{
//		first put the SkinInfo into the mesh container
//		create a copy of the mesh and set it to the pOrigMesh
//		Get the number of bones from SkinInfo
//		create the array of bone offsets
//		for each bone get each of the bone offset matrices 
//			so that we don't need to get them later
//		for (UINT i = 0; i < uBones; i++)
//		{
//			Set the bone offsets
//		}
//	}
//	else
//	{
//		initialize the rest to NULL
//	}
//	Set the output mesh container to the temp one
//	Release the D3D device
//-----------------------------------------------------------------------------
HRESULT	CAllocateHierarchy::CreateMeshContainer(
							LPCSTR						name
						,	CONST	D3DXMESHDATA		*pMeshData
						,	CONST	D3DXMATERIAL		*pMaterials
						,	CONST	D3DXEFFECTINSTANCE	*pEffectInstance
						,	DWORD						numMaterials
						,	CONST	DWORD				*pAdjacency
						,	LPD3DXSKININFO				pSkinInfo
						,	LPD3DXMESHCONTAINER			*ppNewMeshContainer
						)
{
	HRESULT				hr					= S_OK;
	CMeshContainer		*pNewMeshContainer	= NULL;
	LPD3DXMESH			pMesh				= NULL;
	LPDIRECT3DDEVICE9	pd3dDevice			= NULL;

	if( !m_bCreationFlag )
	{
		*ppNewMeshContainer	= (*m_p_V_MeshContainer)[ m_dwMeshContainer++ ];
		return	S_OK;
	}

	//	프로그레시브메쉬와 패치메쉬는 지원 ㄴㄴ
	if( (NULL == pMeshData) || (pMeshData->Type != D3DXMESHTYPE_MESH) )
	{
		hr	= E_FAIL;
		goto E_EXIT;
	}

	//	LPD3DXMESHCONTAINER를 확장한 CMeshContainer클래스를 생성하고 0으로 초기화시킨다.
	//	pMeshData에서 메쉬정보를 얻어온다(pMesh로 )
	//	pMesh가 유효하면 디바이스 정보도 얻어온다
	pNewMeshContainer					= new CMeshContainer;
	if( NULL == pNewMeshContainer )
		return	E_OUTOFMEMORY;
	ZeroMemory( pNewMeshContainer,	sizeof	CMeshContainer );

	pMesh	= pMeshData->pMesh;
	hr = pMesh->GetDevice( &pd3dDevice );
	if( FAILED(hr) )
		goto	E_EXIT;


	//	이름 복사
	if( NULL == name || 0 == strlen(name) )
	{
		pNewMeshContainer->Name	= NULL;
	}
	else
	{
		size_t	length	= strlen( name ) + 1;
		CHAR	*pName	= NULL;
		pName			= new CHAR[ length ];
		if( NULL == pName )
		{
			hr	= E_OUTOFMEMORY;
			goto	E_EXIT;
		}

		strcpy_s( pName, length, name );
		pNewMeshContainer->Name	= pName;
		pName	= NULL;
	}

	//	인수로 받은 메쉬정보를 메쉬컨테이너에 넣는다.
	if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
	{
		hr = pMesh->CloneMeshFVF(
				pMesh->GetOptions()
			,	pMesh->GetFVF() | D3DFVF_NORMAL
			,	pd3dDevice
			,	&(pNewMeshContainer->MeshData.pMesh)
			);

		if( FAILED(hr) )
			goto E_EXIT;		

		pNewMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
		pMesh	= pNewMeshContainer->MeshData.pMesh;

		D3DXComputeNormals( pNewMeshContainer->MeshData.pMesh, NULL );
	}
	else
	{
		pNewMeshContainer->MeshData.Type = pMeshData->Type;
		pNewMeshContainer->MeshData.pMesh= pMesh;
		//D3DXComputeNormals( pNewMeshContainer->MeshData.pMesh, NULL );

		//	함수호출이 _stdcall이므로 pMeshData(및 COM객체)에 대한 관리는 직접하지 않는다.
		pMesh->AddRef();
	}


	//	Adjacency정보를 복사
	//	Adjacency정보를 얻기위해 페이스갯수를 얻어온다( adjacency = face*3(삼각형각변당 인접정보)
	DWORD	numFaces = pMesh->GetNumFaces();

	pNewMeshContainer->pAdjacency = new DWORD[ numFaces * 3 ];
	if( NULL == pNewMeshContainer->pAdjacency )
	{
		hr	= E_OUTOFMEMORY;
		goto	E_EXIT;
	}

	memcpy( pNewMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * 3 *  numFaces );


	//	Material과 Texture정보를 복사한다
	//	material정보가 없어도 기본 material을 적용하기 위해 material갯수를 최소 1개는 설정한다.
	pNewMeshContainer->NumMaterials = max( 1, numMaterials );
	pNewMeshContainer->pMaterials	= NULL;	//	사용안한다.
	pNewMeshContainer->m_pMaterials	= new D3DMATERIAL9[ pNewMeshContainer->NumMaterials ];
	
	pNewMeshContainer->m_ppTextures	= new LPDIRECT3DTEXTURE9[ pNewMeshContainer->NumMaterials ];
	ZeroMemory( pNewMeshContainer->m_pMaterials, sizeof(D3DMATERIAL9) * pNewMeshContainer->NumMaterials );
	ZeroMemory( pNewMeshContainer->m_ppTextures, sizeof(LPDIRECT3DTEXTURE9) * pNewMeshContainer->NumMaterials );
	
	if( NULL == pNewMeshContainer->m_pMaterials || NULL == pNewMeshContainer->m_ppTextures )
	{
		hr = E_OUTOFMEMORY;
		goto E_EXIT;
	}

	if( numMaterials > 0 )
	{
		LPSTR pTextureFilename = NULL;
		for( DWORD i =0; i<pNewMeshContainer->NumMaterials; ++i )
		{
			//	매테리얼 정보를 복사
			pNewMeshContainer->m_pMaterials[i]	= pMaterials[i].MatD3D;
			pNewMeshContainer->m_pMaterials[i].Ambient	= pNewMeshContainer->m_pMaterials[i].Diffuse;

			//	텍스쳐정보 생성
			pTextureFilename = pMaterials[i].pTextureFilename;
			if( NULL == pTextureFilename || 0 == strlen( pTextureFilename ) )
			{
				pNewMeshContainer->m_ppTextures[i] = NULL;
			}
			else
			{
				pNewMeshContainer->m_ppTextures[i] = NULL;
				hr = D3DXCreateTextureFromFileA( pd3dDevice, pTextureFilename, &(pNewMeshContainer->m_ppTextures[i]) );
				if( FAILED(hr) )
				{
					if( NULL != m_pXFilePath )
					{					
						//	경로를 바꿔서 다시 시도
						CHAR	tempFilename[MAX_PATH] = {0};
						memset( tempFilename, 0, MAX_PATH );
						strcpy_s( tempFilename, MAX_PATH, m_pXFilePath);
						strcat_s( tempFilename, MAX_PATH, pTextureFilename );
						hr = D3DXCreateTextureFromFileA( pd3dDevice, tempFilename, &(pNewMeshContainer->m_ppTextures[i]) );
					}

					//	최종적으로 텍스쳐파일을 찾지못한 경우에는 NULL을 입력
					if( FAILED(hr) ) 
						pNewMeshContainer->m_ppTextures[i] = NULL;
				}
					
			}
		}
	}
	else
	{
		//	material정보가 없을 때는 기본 material을 설정한다
		pNewMeshContainer->pMaterials->pTextureFilename = NULL;
		pNewMeshContainer->m_pMaterials[0].Diffuse	= D3DXCOLOR( 1.f, 1.f, 1.f, 0.f );
		pNewMeshContainer->m_pMaterials[0].Specular	= D3DXCOLOR( 1.f, 1.f, 1.f, 0.f );
		pNewMeshContainer->m_pMaterials[0].Power	= 8.f;
		pNewMeshContainer->m_pMaterials[0].Ambient	= D3DXCOLOR( 1.f, 1.f, 1.f, 0.f );		
	}

	//	스키닝 정보가 있으면, 스키닝 관련 정보를 메쉬컨테이너에 셋팅한다.
	if( NULL != pSkinInfo )
	{
		pNewMeshContainer->pSkinInfo		= pSkinInfo;
		//	역시 콜백이므로 레펀런스카운트 증가 필요
		pSkinInfo->AddRef();

		pNewMeshContainer->m_pOriginalMesh	= pMesh;
		pMesh->AddRef();

		//	메쉬를 메쉬의 로컬 스페이스에서 본의 로컬 스페이스로 이동시키기 위해 본 옵셋 메트리스정보를 복사한다.
		DWORD	numBones	= pSkinInfo->GetNumBones();
		pNewMeshContainer->m_pBoneOffsetMatrices	= new D3DXMATRIX[ numBones ];
		if( NULL == pNewMeshContainer->m_pBoneOffsetMatrices )
		{
			hr = E_OUTOFMEMORY;
			goto E_EXIT;
		}

		for( DWORD i=0; i<numBones; ++i )
		{
			pNewMeshContainer->m_pBoneOffsetMatrices[i] = *(pSkinInfo->GetBoneOffsetMatrix( i ));
		}

		//	각 각의 스키닝 방법(SKINNING_TYPE 매크로에 정의되어 있다)에 따른 메쉬를 재 생성한다.
		hr	= pNewMeshContainer->GenerateSkinnedMesh();
		if( FAILED( hr ) )
			goto	E_EXIT;
	}


	*ppNewMeshContainer	= pNewMeshContainer;
	if( NULL != m_p_V_MeshContainer)
	{
		m_p_V_MeshContainer->push_back( pNewMeshContainer );
	}
	//	중요
	pNewMeshContainer	= NULL;

E_EXIT:
	SAFE_RELEASE( pd3dDevice );
	DestroyMeshContainer( pNewMeshContainer );
	return	hr;
}






//-----------------------------------------------------------------------------
//	Name : DestroyMeshContainer()
//	Desc : Callback function used by the D3DXDestroyFrame function
//	Convert to my derived structure type
//	Delete name
//	Delete materials
//	Release the textures
//	Delete textures
//	Delete bones in the mesh
//	Delete adjacency data
//	Delete bone offsets
//	Delete frame matrices
//	Delete mesh
//	Delete skin information
//	Delete copy of the mesh
//	Delete the mesh container
//-----------------------------------------------------------------------------
HRESULT	CAllocateHierarchy::DestroyMeshContainer( LPD3DXMESHCONTAINER pMeshContainerToFree )
{
	HRESULT	hr	= S_OK;

	if( NULL != pMeshContainerToFree )
	{

		CMeshContainer*	pMeshContainer	= (CMeshContainer*) pMeshContainerToFree ;

		SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
		SAFE_RELEASE( pMeshContainer->pSkinInfo );
		SAFE_RELEASE( pMeshContainer->m_pOriginalMesh );
		SAFE_RELEASE( pMeshContainer->m_pBoneCombinationBuf );

		for( DWORD i=0; i< pMeshContainer->NumMaterials; ++i )
		{
			SAFE_RELEASE( pMeshContainer->m_ppTextures[i] );
		}
		
		SAFE_DELETE_ARRAY( pMeshContainer->m_ppTextures );
		SAFE_DELETE_ARRAY( pMeshContainer->m_pMaterials);
		SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
		SAFE_DELETE_ARRAY( pMeshContainer->Name );	
		SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
		SAFE_DELETE_ARRAY( pMeshContainer->m_pBoneOffsetMatrices);
		SAFE_DELETE_ARRAY( pMeshContainer->m_ppBoneMatrixPtrs );

		SAFE_DELETE( pMeshContainer );
	}

	return	hr;
}