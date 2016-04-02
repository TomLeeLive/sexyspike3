#pragma once

// ���� ��ũ��
#define RS   g_pApp->m_pd3dDevice->SetRenderState
#define TSS  g_pApp->m_pd3dDevice->SetTextureStageState
#define SAMP g_pApp->m_pd3dDevice->SetSamplerState


//09-08-07 ������
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
typedef struct{
	FLOAT x,y,z, rhw; //��ũ�� ��ǥ������ ��ġ
	DWORD color;      //���� ��
	FLOAT tu, tv;		//�ؽ��� ��ǥ
} CUSTOMVERTEX;

//�⺻ �ؽ��� Ŭ����
class CGameTexture
{
public:
	LPDIRECT3DTEXTURE9	m_pTex; //Texture(Image)
	//09-08-07 ������
	//LPDIRECT3DTEXTURE9 m_pTexture;		//�ؽ���						

	HRESULT TextureLoad(LPCSTR pSrcFile);
	
	virtual void Destroy();

	CGameTexture(void);
	virtual ~CGameTexture(void);
};

//���� ���� ���� �߰� �ؽ��� Ŭ����
class CGameTexture2 : public CGameTexture
{
public:
	VOID* pVertices;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;		//���� ���� ����
	CUSTOMVERTEX vertices;

	HRESULT CreateVertexBuffer();
	void InvalidateDeviceObjects();

	void Render();

	CGameTexture2(void);
	virtual ~CGameTexture2(void);
};

//��������Ʈ �� �ؽ��� Ŭ����
class CGameTexture3 : public CGameTexture
{
public:
	LPDIRECT3DDEVICE9		m_pDev;
	D3DXIMAGE_INFO			m_pImg; //Image ����

	CGameTexture3(void);
	virtual ~CGameTexture3(void);

public:
	virtual INT	Create(LPDIRECT3DDEVICE9 pDev, char* sFile, DWORD	dRscID=0);
	virtual void	Destroy();
	INT		GetImageWidth();
	INT		GetImageHeight();
	void	GetImageRect(RECT* pRc);

	LPDIRECT3DTEXTURE9	GetTexture();
};