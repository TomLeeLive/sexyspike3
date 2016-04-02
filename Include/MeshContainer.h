
#pragma		once

#ifndef		__MESHCONTAINER_H_
#define		__MESHCONTAINER_H_

//----------------------------------------------------------------------------
// D3DXMESHCONTAINER:
// ------------------
// This struct encapsulates a mesh object in a transformation frame
// hierarchy. The app can derive from this structure to add other app specific
// data to this.
//----------------------------------------------------------------------------
//typedef struct _D3DXMESHCONTAINER
//{
//    LPSTR                   Name;
//
//    D3DXMESHDATA            MeshData;
//
//    LPD3DXMATERIAL          pMaterials;
//    LPD3DXEFFECTINSTANCE    pEffects;
//    DWORD                   NumMaterials;
//    DWORD                  *pAdjacency;
//
//    LPD3DXSKININFO          pSkinInfo;
//
//    struct _D3DXMESHCONTAINER *pNextMeshContainer;
//} D3DXMESHCONTAINER, *LPD3DXMESHCONTAINER;

struct		CMeshContainer	: public	D3DXMESHCONTAINER
{
	//	상위클래스는 D3D X MATERIAL이므로 Texture정보와 material정보를 나눠서 저장
	LPDIRECT3DTEXTURE9	*m_ppTextures;
	D3DMATERIAL9		*m_pMaterials;

	//	스키닝 관련 정보
	LPD3DXMESH				m_pOriginalMesh;		//	ConverToBlendXXXMesh에서 사용
	D3DXMATRIX**			m_ppBoneMatrixPtrs;		//	CFrame	m_CombinedTransformationMatrix에 대한 포인터
	D3DXMATRIX*				m_pBoneOffsetMatrices;	//	메쉬스페이스->본스페이스

	DWORD					m_numAttributeGroups;	//	어트리뷰트그룹수(혹은 D3DXBONECOMBINATION의 수)
	DWORD					m_numInfl;				//	어트리뷰트그룹당(혹은 D3DXBONECOMBINATION) 스키닝에 영향을 미치는 최대 본(매트릭스)수
	LPD3DXBUFFER			m_pBoneCombinationBuf;	//	D3DXBONECOMBINATION* 에 대한 버퍼포인터를 가지고 있다.
	DWORD					m_numPaletteEntries;	//	Indexed blending시 사용할 팔레트의 갯수
	BOOL					m_bUseSofeWareVP;		//	고정파이프라인 블렌딩사용시 블렌딩에 사용할 매트릭스수가 하드웨어 지원보다 많을 때 소프트웨어 VP로 바꿔줌(디바이스설정에 MIXEDVP필요)
	DWORD					m_nAttributeSW;			//	이 값이 m_numAttributeGroups보다 작으면 SWVP가 필요한 파트가 있음( 이 값 이전은 HW, 이 값부터는 SW )

public:
	//------------------------------------------------------------------------
	// GenerateSkinnedMesh:
	// ---------------------
	// 각 각의 스키닝 방법(SKINNING_TYPE 매크로에 정의되어 있다)에 따른 메쉬를 재 생성한다.
	//
	// Parameters:
	//  NONE
	//
	//------------------------------------------------------------------------
	HRESULT				GenerateSkinnedMesh();
	HRESULT				GenerateSkinnedMeshByNonIndexed();
	HRESULT				GenerateSkinnedMeshByIndexed();
	HRESULT				GenerateSkinnedMeshByVS();
	HRESULT				GenerateSkinnedMeshByHLSL();
	HRESULT				GenerateSkinnedMeshBySW();
};



#endif