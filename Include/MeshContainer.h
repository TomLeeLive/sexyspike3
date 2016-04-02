
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
	//	����Ŭ������ D3D X MATERIAL�̹Ƿ� Texture������ material������ ������ ����
	LPDIRECT3DTEXTURE9	*m_ppTextures;
	D3DMATERIAL9		*m_pMaterials;

	//	��Ű�� ���� ����
	LPD3DXMESH				m_pOriginalMesh;		//	ConverToBlendXXXMesh���� ���
	D3DXMATRIX**			m_ppBoneMatrixPtrs;		//	CFrame	m_CombinedTransformationMatrix�� ���� ������
	D3DXMATRIX*				m_pBoneOffsetMatrices;	//	�޽������̽�->�������̽�

	DWORD					m_numAttributeGroups;	//	��Ʈ����Ʈ�׷��(Ȥ�� D3DXBONECOMBINATION�� ��)
	DWORD					m_numInfl;				//	��Ʈ����Ʈ�׷��(Ȥ�� D3DXBONECOMBINATION) ��Ű�׿� ������ ��ġ�� �ִ� ��(��Ʈ����)��
	LPD3DXBUFFER			m_pBoneCombinationBuf;	//	D3DXBONECOMBINATION* �� ���� ���������͸� ������ �ִ�.
	DWORD					m_numPaletteEntries;	//	Indexed blending�� ����� �ȷ�Ʈ�� ����
	BOOL					m_bUseSofeWareVP;		//	�������������� �������� ������ ����� ��Ʈ�������� �ϵ���� �������� ���� �� ����Ʈ���� VP�� �ٲ���(����̽������� MIXEDVP�ʿ�)
	DWORD					m_nAttributeSW;			//	�� ���� m_numAttributeGroups���� ������ SWVP�� �ʿ��� ��Ʈ�� ����( �� �� ������ HW, �� �����ʹ� SW )

public:
	//------------------------------------------------------------------------
	// GenerateSkinnedMesh:
	// ---------------------
	// �� ���� ��Ű�� ���(SKINNING_TYPE ��ũ�ο� ���ǵǾ� �ִ�)�� ���� �޽��� �� �����Ѵ�.
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