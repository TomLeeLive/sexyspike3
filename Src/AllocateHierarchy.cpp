
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

	//	Frame �̸� ����
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

	//	��Ÿ ��� �ʱ�ȭ
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
	//	�ؿ� E_EXIT���� ������ �ν��Ͻ��� ���� �ȵǰ� pNewFrame�� NULL�� �Ҵ��Ѵ�.
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

	//	���α׷��ú�޽��� ��ġ�޽��� ���� ����
	if( (NULL == pMeshData) || (pMeshData->Type != D3DXMESHTYPE_MESH) )
	{
		hr	= E_FAIL;
		goto E_EXIT;
	}

	//	LPD3DXMESHCONTAINER�� Ȯ���� CMeshContainerŬ������ �����ϰ� 0���� �ʱ�ȭ��Ų��.
	//	pMeshData���� �޽������� ���´�(pMesh�� )
	//	pMesh�� ��ȿ�ϸ� ����̽� ������ ���´�
	pNewMeshContainer					= new CMeshContainer;
	if( NULL == pNewMeshContainer )
		return	E_OUTOFMEMORY;
	ZeroMemory( pNewMeshContainer,	sizeof	CMeshContainer );

	pMesh	= pMeshData->pMesh;
	hr = pMesh->GetDevice( &pd3dDevice );
	if( FAILED(hr) )
		goto	E_EXIT;


	//	�̸� ����
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

	//	�μ��� ���� �޽������� �޽������̳ʿ� �ִ´�.
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

		//	�Լ�ȣ���� _stdcall�̹Ƿ� pMeshData(�� COM��ü)�� ���� ������ �������� �ʴ´�.
		pMesh->AddRef();
	}


	//	Adjacency������ ����
	//	Adjacency������ ������� ���̽������� ���´�( adjacency = face*3(�ﰢ�������� ��������)
	DWORD	numFaces = pMesh->GetNumFaces();

	pNewMeshContainer->pAdjacency = new DWORD[ numFaces * 3 ];
	if( NULL == pNewMeshContainer->pAdjacency )
	{
		hr	= E_OUTOFMEMORY;
		goto	E_EXIT;
	}

	memcpy( pNewMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * 3 *  numFaces );


	//	Material�� Texture������ �����Ѵ�
	//	material������ ��� �⺻ material�� �����ϱ� ���� material������ �ּ� 1���� �����Ѵ�.
	pNewMeshContainer->NumMaterials = max( 1, numMaterials );
	pNewMeshContainer->pMaterials	= NULL;	//	�����Ѵ�.
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
			//	���׸��� ������ ����
			pNewMeshContainer->m_pMaterials[i]	= pMaterials[i].MatD3D;
			pNewMeshContainer->m_pMaterials[i].Ambient	= pNewMeshContainer->m_pMaterials[i].Diffuse;

			//	�ؽ������� ����
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
						//	��θ� �ٲ㼭 �ٽ� �õ�
						CHAR	tempFilename[MAX_PATH] = {0};
						memset( tempFilename, 0, MAX_PATH );
						strcpy_s( tempFilename, MAX_PATH, m_pXFilePath);
						strcat_s( tempFilename, MAX_PATH, pTextureFilename );
						hr = D3DXCreateTextureFromFileA( pd3dDevice, tempFilename, &(pNewMeshContainer->m_ppTextures[i]) );
					}

					//	���������� �ؽ��������� ã������ ��쿡�� NULL�� �Է�
					if( FAILED(hr) ) 
						pNewMeshContainer->m_ppTextures[i] = NULL;
				}
					
			}
		}
	}
	else
	{
		//	material������ ���� ���� �⺻ material�� �����Ѵ�
		pNewMeshContainer->pMaterials->pTextureFilename = NULL;
		pNewMeshContainer->m_pMaterials[0].Diffuse	= D3DXCOLOR( 1.f, 1.f, 1.f, 0.f );
		pNewMeshContainer->m_pMaterials[0].Specular	= D3DXCOLOR( 1.f, 1.f, 1.f, 0.f );
		pNewMeshContainer->m_pMaterials[0].Power	= 8.f;
		pNewMeshContainer->m_pMaterials[0].Ambient	= D3DXCOLOR( 1.f, 1.f, 1.f, 0.f );		
	}

	//	��Ű�� ������ ������, ��Ű�� ���� ������ �޽������̳ʿ� �����Ѵ�.
	if( NULL != pSkinInfo )
	{
		pNewMeshContainer->pSkinInfo		= pSkinInfo;
		//	���� �ݹ��̹Ƿ� ���ݷ���ī��Ʈ ���� �ʿ�
		pSkinInfo->AddRef();

		pNewMeshContainer->m_pOriginalMesh	= pMesh;
		pMesh->AddRef();

		//	�޽��� �޽��� ���� �����̽����� ���� ���� �����̽��� �̵���Ű�� ���� �� �ɼ� ��Ʈ���������� �����Ѵ�.
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

		//	�� ���� ��Ű�� ���(SKINNING_TYPE ��ũ�ο� ���ǵǾ� �ִ�)�� ���� �޽��� �� �����Ѵ�.
		hr	= pNewMeshContainer->GenerateSkinnedMesh();
		if( FAILED( hr ) )
			goto	E_EXIT;
	}


	*ppNewMeshContainer	= pNewMeshContainer;
	if( NULL != m_p_V_MeshContainer)
	{
		m_p_V_MeshContainer->push_back( pNewMeshContainer );
	}
	//	�߿�
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