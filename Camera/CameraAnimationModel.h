
#pragma		once

#ifndef		__CAMERA_ANIMATION_MODEL_H_
#define		__CAMERA_ANIMATION_MODEL_H_

class		CCameraAnimationModel
{ 
public:
	CCameraAnimationController*		m_pAniController;		//	�ִϸ��̼� ��Ʈ�Ѷ�	
	LPD3DXFRAME						m_pFrameRoot;			//	D3DXFRAME�������� ����� ���� ���� Ŭ������ CFrame���̴�.
protected:
	LPTSTR								m_pXFileName;			//	�ε��� X���ϸ�
	LPDIRECT3DDEVICE9					m_pd3dDevice;


	std::vector< LPD3DXFRAME> vFrame;
	std::vector< LPD3DXMESHCONTAINER > vMeshContainer;

protected:

	//	�޽������̳ʿ� ������ �ִ� �� ���� ���߿� ���庯ȯ�� �ϴµ� ����� Ư�� �������� TM�� ��������ش�.
	HRESULT	SetupBoneMatrixPointers( LPD3DXFRAME	pFrame );
	HRESULT	SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pSuperMeshContainer );

	HRESULT	DrawFrame( LPD3DXFRAME	pFrame );
	HRESULT	DrawMeshContainer( LPD3DXFRAME pFrameBase, LPD3DXMESHCONTAINER	pSuperMeshContainer );
	HRESULT	DrawMeshContainerByNonIndexed( LPD3DXMESHCONTAINER pMeshContainer );
	HRESULT	DrawMeshContainerByIndexed( LPD3DXMESHCONTAINER pMeshContainer );
	HRESULT	DrawMeshContainerByVS( LPD3DXMESHCONTAINER pMeshContainer );
	HRESULT	DrawMeshContainerByHLSL( LPD3DXMESHCONTAINER pMeshContainer );
	HRESULT	DrawMeshContainerBySW( LPD3DXMESHCONTAINER pMeshContainer );

public:
	CCameraAnimationModel();
	CCameraAnimationModel( LPCTSTR	lpszXFileName );
	virtual	~CCameraAnimationModel();

	HRESULT	InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT	RestoreDeviceObjects();
	HRESULT	InvalidateDeviceObjects();
	HRESULT	DeleteDeviceObjects();	

	//	���� ��ȯ�� ���� ��Ʈ���� ����
	void	UpdateFrameMatrices( LPD3DXFRAME pFrame, const LPD3DXMATRIX	pParentMatrix );	

	HRESULT	FrameMove( float fElapsedTime = 0.f, LPD3DXMATRIX pTM = NULL);
	HRESULT	Render();

	//	Ű�Է¿� ���� �ִϸ��̼� ����
	LRESULT	HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif