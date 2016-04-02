
#pragma		once

#ifndef		__BACKGROUND_H_
#define		__BACKGROUND_H_

class		CBackground
{
	LPD3DXFRAME					m_pFrameRoot;
	LPD3DXANIMATIONCONTROLLER	m_pAniController;
	D3DXMATRIX					m_matWorld;

	//	�޽������̳ʿ� ������ �ִ� �� ���� ���߿� ���庯ȯ�� �ϴµ� ����� Ư�� �������� TM�� ��������ش�.
	void	UpdateFrameMatrices( LPD3DXFRAME pFrame, const LPD3DXMATRIX	pParentMatrix );
	HRESULT	DrawFrame(LPDIRECT3DDEVICE9	pDevice,  LPD3DXFRAME	pFrame );
	HRESULT	DrawMeshContainer(LPDIRECT3DDEVICE9	pDevice,  LPD3DXFRAME pFrameBase, LPD3DXMESHCONTAINER	pSuperMeshContainer );

public:
	CBackground() : m_pFrameRoot(NULL), m_pAniController(NULL), m_matWorld(UNIT_MATRIX){}
	~CBackground();

	HRESULT		FrameMove(float fElapsedTime);
	HRESULT		Render(LPDIRECT3DDEVICE9 pDevice);

	HRESULT		InitDeviceObjects( LPDIRECT3DDEVICE9	pDevice );
	HRESULT		RestoreDeviceObjects();
	HRESULT		InvalidateDeviceObjects();
	HRESULT		DeleteDeviceObjects();
};

#endif