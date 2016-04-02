#include "../Include/INCLUDE.h"

CEndGame::CEndGame(void)
{
	//09-08-07 이재준
	SetRect(&m_rc,0,0,1024,768);
	m_vcPos= D3DXVECTOR3(0,0,0);			//배경위치
}

CEndGame::~CEndGame(void)
{
}


//게임 종료화면 갱신
INT CEndGame::EndPlayFrameMove()
{
	SAFE_FRMOV(	GMAIN->m_pInput	);
	SAFE_FRMOV(	GMAIN->m_pCamMn	);

	return 0;
}
//게임 종료화면 렌더링
void CEndGame::EndPlayRender()
{
	GMAIN->m_pd3dSprite->Begin(D3DXSPRITE_ALPHABLEND);
	SpriteDraw();
	GMAIN->m_pd3dSprite->End();

	//m_pTexture.Render();

	D3DXMATRIXA16 mWorld;
	D3DXMatrixTranslation(&mWorld,0,0,0);
	GMAIN->m_pd3dDevice->SetTransform(D3DTS_WORLD,&mWorld);
}

void CEndGame::SpriteDraw()
{
	D3DXMATRIX	mtW;
	D3DXVECTOR3 vcDelta = g_pApp->m_pInput->GetMousePos();

	D3DXMatrixIdentity(&mtW);
	mtW._11 =  float(GMAIN->m_rcWindowClient.right/1024.f);							// x축 스케일
	mtW._22 =  float(GMAIN->m_rcWindowClient.bottom/768.f);							// y축 스케일
	mtW._41	=  0;	// x 위치
	mtW._42	=  0;	// y 위치
	GMAIN->m_pd3dSprite->SetTransform(&mtW);

	//메뉴화면 출력
	m_vcPos=D3DXVECTOR3(0,0,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[0].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));


	/////////////////////////////////////////////////////////////////////////////
	//게임을 종료 합니까?
	m_vcPos=D3DXVECTOR3(220,348,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[39].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	m_vcPos=D3DXVECTOR3(320,422,0);
	GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[40].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));

	INT nCheck = RectCheck(vcDelta,326,505,429,503);
	if(nCheck==1)
	{
		PostQuitMessage(0);
	}
	else if(nCheck==2)
	{
		//예: 	PostQuitMessage(0);
		m_rc.left = 0  ; m_rc.right = 184;
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[41].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}
	else if(nCheck==0)
	{
		nCheck=RectCheck(vcDelta,518,696,429,503);
		if(nCheck==1)
		{
			GMAIN->m_nGamePhase=0;
		}
		else if(nCheck==2)
		{
			//아니오
			m_vcPos.x += 192;
			m_rc.left = 192  ; m_rc.right = 384;
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[41].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		}
	}


	m_rc.left = 0  ; m_rc.right = 1024;
}
INT CEndGame::RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax)
{
	INT nClicked = 0;

	if(vcDelta.x > GMAIN->m_rcWindowClient.right*nXmin/1024
		&& vcDelta.x < GMAIN->m_rcWindowClient.right*nXmax/1024
		&& vcDelta.y > GMAIN->m_rcWindowClient.bottom*nYmin/768
		&& vcDelta.y < GMAIN->m_rcWindowClient.bottom*nYmax/768)
	{
		nClicked = 2;

		if(g_pApp->m_pInput->BtnDown(0))
		{
			GMAIN->m_pMp[2]->Reset();
			GMAIN->m_pMp[2]->Stop();
			GMAIN->m_pMp[2]->Play();

			nClicked = 1;
		}

	}
	return nClicked;

}