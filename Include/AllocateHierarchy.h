
#pragma		once

#ifndef		__ALLOCATEHIERARCHY_H_
#define		__ALLOCATEHIERARCHY_H_

class	CAllocateHierarchy : public ID3DXAllocateHierarchy
{
	std::vector< LPD3DXFRAME >			*m_p_V_Frame;
	std::vector< LPD3DXMESHCONTAINER>	*m_p_V_MeshContainer;
	DWORD								m_dwFrame;
	DWORD								m_dwMeshContainer;
	BOOL								m_bCreationFlag;
	LPTSTR								m_pXFilePath;

public:
	CAllocateHierarchy() 
		:	m_p_V_Frame( NULL )
		,	m_p_V_MeshContainer( NULL )
		,	m_dwFrame( 0 )
		,	m_dwMeshContainer( 0 )
		,	m_bCreationFlag( TRUE )
		,	m_pXFilePath( NULL )
	{}

	void	SetXFilePath( LPTSTR pXFilePath ) { m_pXFilePath=pXFilePath; };
	void	SetFrameVector( std::vector< LPD3DXFRAME > *p_V_Frame ) {m_p_V_Frame = p_V_Frame; }
	void	SetMeshContainerVector( std::vector< LPD3DXMESHCONTAINER> *p_V_MeshContainer ) 
			{ m_p_V_MeshContainer = p_V_MeshContainer; }
	void	SetDwFrame( DWORD dwFrame ) { m_dwFrame	= dwFrame; }
	void	SetDwMeshContainer( DWORD dwMeshContainer ) { m_dwMeshContainer = dwMeshContainer;}
	void	SetCreationFlag( BOOL bCreationFlag ) { m_bCreationFlag = bCreationFlag ;}

public:
	//------------------------------------------------------------------------
	// CreateFrame:
	// ------------
	// Requests allocation of a frame object.
	//
	// Parameters:
	//  Name
	//		Name of the frame to be created
	//	ppNewFrame
	//		Returns the created frame object
	//
	//------------------------------------------------------------------------
	HRESULT	__stdcall	CreateFrame( LPCSTR name, LPD3DXFRAME *ppNewFrame );


	//------------------------------------------------------------------------
	// CreateMeshContainer:
	// --------------------
	// Requests allocation of a mesh container object.
	//
	// Parameters:
	//  Name
	//		Name of the mesh
	//	pMesh
	//		Pointer to the mesh object if basic polygon data found
	//	pPMesh
	//		Pointer to the progressive mesh object if progressive mesh data found
	//	pPatchMesh
	//		Pointer to the patch mesh object if patch data found
	//	pMaterials
	//		Array of materials used in the mesh
	//	pEffectInstances
	//		Array of effect instances used in the mesh
	//	NumMaterials
	//		Num elements in the pMaterials array
	//	pAdjacency
	//		Adjacency array for the mesh
	//	pSkinInfo
	//		Pointer to the skininfo object if the mesh is skinned
	//	ppNewMeshContainer
	//		The	Output mesh container
	//
	//------------------------------------------------------------------------
	HRESULT	__stdcall	CreateMeshContainer(
								LPCSTR						name
							,	CONST	D3DXMESHDATA		*pMeshData
							,	CONST	D3DXMATERIAL		*pMaterials
							,	CONST	D3DXEFFECTINSTANCE	*pEffectInstance
							,	DWORD						numMaterials
							,	CONST	DWORD				*pAdjacency
							,	LPD3DXSKININFO				pSkininfo
							,	LPD3DXMESHCONTAINER			*ppNewMeshContainer
							);

	//------------------------------------------------------------------------
	// DestroyFrame:
	// -------------
	// Requests de-allocation of a frame object.
	//
	// Parameters:
	//  pFrameToFree
	//		Pointer to the frame to be de-allocated
	//
	//------------------------------------------------------------------------
	HRESULT	__stdcall	DestroyFrame( LPD3DXFRAME	pFrameToFree );

	//------------------------------------------------------------------------
	// DestroyMeshContainer:
	// ---------------------
	// Requests de-allocation of a mesh container object.
	//
	// Parameters:
	//  pMeshContainerToFree
	//		Pointer to the mesh container object to be de-allocated
	//
	//------------------------------------------------------------------------
	HRESULT	__stdcall	DestroyMeshContainer( LPD3DXMESHCONTAINER	pMeshContainerToFree );
};

#endif