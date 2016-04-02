#pragma once

// 단축 매크로
#define RS   g_pApp->m_pd3dDevice->SetRenderState
#define TSS  g_pApp->m_pd3dDevice->SetTextureStageState
#define SAMP g_pApp->m_pd3dDevice->SetSamplerState


//09-08-07 이재준
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
typedef struct{
	FLOAT x,y,z, rhw; //스크린 좌표에서의 위치
	DWORD color;      //정점 색
	FLOAT tu, tv;		//텍스쳐 좌표
} CUSTOMVERTEX;

//기본 텍스쳐 클래스
class CGameTexture
{
public:
	LPDIRECT3DTEXTURE9	m_pTex; //Texture(Image)
	//09-08-07 이재준
	//LPDIRECT3DTEXTURE9 m_pTexture;		//텍스쳐						

	HRESULT TextureLoad(LPCSTR pSrcFile);
	
	virtual void Destroy();

	CGameTexture(void);
	virtual ~CGameTexture(void);
};

//정점 버퍼 생성 추가 텍스쳐 클래스
class CGameTexture2 : public CGameTexture
{
public:
	VOID* pVertices;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;		//정점 정보 저장
	CUSTOMVERTEX vertices;

	HRESULT CreateVertexBuffer();
	void InvalidateDeviceObjects();

	void Render();

	CGameTexture2(void);
	virtual ~CGameTexture2(void);
};

//스프라이트 용 텍스쳐 클래스
class CGameTexture3 : public CGameTexture
{
public:
	LPDIRECT3DDEVICE9		m_pDev;
	D3DXIMAGE_INFO			m_pImg; //Image 정보

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