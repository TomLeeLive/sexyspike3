
#pragma		once

#ifndef		__CAMERA_ANIMATION_MODEL_H_
#define		__CAMERA_ANIMATION_MODEL_H_

class		CCameraAnimationModel
{ 
public:
	CCameraAnimationController*		m_pAniController;		//	애니메이션 컨트롤라	
	LPD3DXFRAME						m_pFrameRoot;			//	D3DXFRAME형이지만 사실은 하위 구현 클래스인 CFrame형이다.
protected:
	LPTSTR								m_pXFileName;			//	로드할 X파일명
	LPDIRECT3DDEVICE9					m_pd3dDevice;


	std::vector< LPD3DXFRAME> vFrame;
	std::vector< LPD3DXMESHCONTAINER > vMeshContainer;

protected:

	//	메쉬컨테이너에 영향을 주는 각 본에 나중에 월드변환을 하는데 사용할 특정 프레임의 TM을 연결시켜준다.
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

	//	월드 변환을 위한 매트릭스 적용
	void	UpdateFrameMatrices( LPD3DXFRAME pFrame, const LPD3DXMATRIX	pParentMatrix );	

	HRESULT	FrameMove( float fElapsedTime = 0.f, LPD3DXMATRIX pTM = NULL);
	HRESULT	Render();

	//	키입력에 따른 애니메이션 조작
	LRESULT	HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif