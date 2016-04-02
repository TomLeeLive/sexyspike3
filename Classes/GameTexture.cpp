#include "../Include/INCLUDE.h"

CGameTexture::CGameTexture(void)
{
	m_pTex=NULL;
}

CGameTexture::~CGameTexture(void)
{
	Destroy();
}

CGameTexture2::CGameTexture2(void)
{
	m_pVB=NULL;
}

CGameTexture2::~CGameTexture2(void)
{
}

CGameTexture3::CGameTexture3(void)
{
	m_pDev	= NULL;
	memset(&m_pImg, 0, sizeof m_pImg);
}

CGameTexture3::~CGameTexture3(void)
{
	Destroy();
}




void CGameTexture::Destroy()
{
	SAFE_RELEASE(m_pTex);//텍스쳐 해제
}

HRESULT CGameTexture::TextureLoad(LPCSTR pSrcFile)
{
	//09-08-07 이재준
	if(FAILED(D3DXCreateTextureFromFile(g_pApp->m_pd3dDevice,pSrcFile,&m_pTex)))
		return E_FAIL;

	return S_OK;
};

HRESULT CGameTexture2::CreateVertexBuffer()
{
	//09-08-07 이재준
	CUSTOMVERTEX vertices[] = {
		//x,y,z, rhw, 정점과 색 정보 (적, 녹, 청,a) U V
		{0.0f,0.0f,0.0f, 1.0f, D3DCOLOR_RGBA(0xff,0xff,0xff,0xff), 0, 0},
		{float(g_pApp->m_rcWindowClient.right),0.0f,0.0f,1.0f, D3DCOLOR_RGBA(0xff,0xff,0xff,0xff), 1, 0},
		{0.0f,float(g_pApp->m_rcWindowClient.bottom),0.0f,1.0f, D3DCOLOR_RGBA(0xff,0xff,0xff,0xff), 0, 1},
		{float(g_pApp->m_rcWindowClient.right),float(g_pApp->m_rcWindowClient.bottom),0.0f,1.0f, D3DCOLOR_RGBA(0xff,0xff,0xff,0xff), 1, 1}
	};
	//정점 버퍼 생성
	if(FAILED(g_pApp->m_pd3dDevice->CreateVertexBuffer(
		4*sizeof(CUSTOMVERTEX), //정점버퍼크기
		0,D3DFVF_CUSTOMVERTEX, //사용법, 정점 포맷
		D3DPOOL_DEFAULT, //메모리 클래스
		&m_pVB, NULL))) // 정점 버퍼 리소스 예약
		return E_FAIL;
	//정점 버퍼 정보를 저장
	VOID* pVertices;
	if(FAILED(m_pVB->Lock(0,sizeof(vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices,vertices,sizeof(vertices));
	m_pVB->Unlock();


	// 렌더링 상태 설정
	RS( D3DRS_DITHERENABLE,   FALSE );
	RS( D3DRS_SPECULARENABLE, FALSE );
	RS( D3DRS_ZENABLE,        TRUE );
	RS( D3DRS_AMBIENT,        0x000F0F0F );

	// ★★★ 추가:텍스처에 관한 설정
	TSS( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	TSS( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	return S_OK;
}

void CGameTexture2::InvalidateDeviceObjects()
{
	SAFE_RELEASE( m_pVB );
}

void CGameTexture2::Render()
{
	g_pApp->m_pd3dDevice->SetTexture(0,m_pTex);
	g_pApp->m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pApp->m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pApp->m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}




void CGameTexture3::Destroy()
{
	SAFE_RELEASE ( m_pTex );
}


INT CGameTexture3::Create(LPDIRECT3DDEVICE9 pDev, char* sFile, DWORD	dRsc)
{
	m_pDev	= pDev;

	DWORD	dColorKey	= 0x00FFFFFF;

	if(sFile && strlen(sFile)>6)
	{
		dColorKey	= dRsc;
		if(FAILED(D3DXCreateTextureFromFileEx(
			m_pDev
			, sFile
			, D3DX_DEFAULT
			, D3DX_DEFAULT
			, 1
			, 0
			, D3DFMT_UNKNOWN
			, D3DPOOL_MANAGED
			, D3DX_FILTER_NONE
			, D3DX_FILTER_NONE
			, dColorKey
			, &m_pImg
			, NULL
			, &m_pTex
			)) )
		{
			m_pTex = NULL;
			MessageBox( GetActiveWindow()
				, "Create Texture Failed"
				, "Err"
				, MB_ICONEXCLAMATION
				);
			return -1;
		}
	}

	else if(dRsc)
	{
		HINSTANCE	hInst	= GetModuleHandle(NULL);
		HRSRC		hrsc	= FindResource(hInst, MAKEINTRESOURCE( dRsc ), "png");
		HGLOBAL		hglobal = LoadResource(hInst, hrsc);
		DWORD		dwSize	= SizeofResource(hInst,hrsc);
		void*		pMemory = LockResource(hglobal);

		if(FAILED(D3DXCreateTextureFromFileInMemoryEx(
			m_pDev
			, pMemory
			, dwSize
			, D3DX_DEFAULT
			, D3DX_DEFAULT
			, 1
			, 0
			, D3DFMT_UNKNOWN
			, D3DPOOL_MANAGED
			, D3DX_FILTER_NONE
			, D3DX_FILTER_NONE
			, dColorKey
			, &m_pImg
			, NULL
			, &m_pTex
			)) )
		{
			m_pTex = NULL;
			MessageBox( GetActiveWindow()
				, "Create Texture Failed"
				, "Err"
				, MB_ICONEXCLAMATION
				);
			return -1;
		}

		UnlockResource(hglobal);
		FreeResource(hglobal);
	}


	return 0;
}



INT CGameTexture3::GetImageWidth()
{
	return m_pImg.Width;
}


INT CGameTexture3::GetImageHeight()
{
	return m_pImg.Height;
}

void CGameTexture3::GetImageRect(RECT* pRc)
{
	pRc->left	= 0;
	pRc->top	= 0;
	pRc->right	= m_pImg.Width;
	pRc->bottom	= m_pImg.Height;
}

LPDIRECT3DTEXTURE9 CGameTexture3::GetTexture()
{
	return m_pTex;
}


