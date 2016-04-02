#include "../Include/INCLUDE.h"

CStartScreen::CStartScreen(void)
{
	//09-08-07 이재준
	SetRect(&m_rc,0,0,1024,768);
	m_vcPos= D3DXVECTOR3(0,0,0);			//배경위치
}

CStartScreen::~CStartScreen(void)
{
}

//시작화면 갱신
INT CStartScreen::StartScreenFrameMove()
{
//	ShowWindow(GMAIN->m_NetworkDlg->m_hConnection,SW_HIDE);

	SAFE_FRMOV(	GMAIN->m_pInput	);
	SAFE_FRMOV(	GMAIN->m_pCamMn	);
	return 0;
}
//시작화면 렌더링
void CStartScreen::StartScreenRender()
{
	GMAIN->m_pd3dSprite->Begin(D3DXSPRITE_ALPHABLEND);
	SpriteDraw();
	GMAIN->m_pd3dSprite->End();

	//m_pTexture.Render();

	D3DXMATRIXA16 mWorld;
	D3DXMatrixTranslation(&mWorld,0,0,0);
	GMAIN->m_pd3dDevice->SetTransform(D3DTS_WORLD,&mWorld);

	//GMAIN->m_pCamMn->m_pCam0->SetTransformViw();
	//GMAIN->m_pCamMn->m_pCam0->SetTransformPrj();
}


INT CStartScreen::RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax)
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

void CStartScreen::SpriteDraw()
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


	INT nCheck = RectCheck(vcDelta, 403, 632, 384, 436);

	if(nCheck ==1)
	{
		GMAIN->m_bMultiGame=FALSE;
		GMAIN->Initval();
		GMAIN->m_CharacterSelect->InitVal();
		GMAIN->m_GameSceneManager->InitVal();

		GMAIN->m_nGamePhase=5;
	}
	else if(nCheck ==2)
	{
		//싱글 플레이 아이콘 출력
		m_vcPos=D3DXVECTOR3(370,376,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[1].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
	}
	else if(nCheck==0)
	{
		nCheck = RectCheck(vcDelta, 403, 632, 458,512);

		if(nCheck==1)
		{
			GMAIN->Initval();
			GMAIN->m_CharacterSelect->InitVal();
			GMAIN->m_GameSceneManager->InitVal();
			GMAIN->m_MultiPlay->InitVal();

			GMAIN->m_bMultiGame=TRUE;
			GMAIN->m_nGamePhase=2;


			if( !GMAIN->m_bWindowed )
			{
			GMAIN->Pause( true );
			if( FAILED( GMAIN->ToggleFullscreen() ) )
				GMAIN->DisplayErrorMsg( D3DAPPERR_RESETFAILED, MSGERR_APPMUSTEXIT );
			GMAIN->Pause( false ); 
			}

			ShowWindow(GMAIN->m_NetworkDlg->m_hConnection,SW_SHOW);
		}
		else if(nCheck==2)
		{
			//멀티 플레이 아이콘 출력
			m_vcPos=D3DXVECTOR3(370,451,0);
			GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[2].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
		}
		if(nCheck==0)
		{
			nCheck=RectCheck(vcDelta,403,632,532,584);
			if(nCheck==1)
			{
				GMAIN->m_nGamePhase=3;
			}
			else if(nCheck==2)
			{
				//How to Play 아이콘 출력
				m_vcPos=D3DXVECTOR3(370,524,0);
				GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[3].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
			}
			else if(nCheck==0)
			{
				nCheck=RectCheck(vcDelta,403,632,589,655);
				if(nCheck==1)
				{
					GMAIN->m_nGamePhase=4;
				}
				else if(nCheck==2)
				{
					//End Game 아이콘 출력
					m_vcPos=D3DXVECTOR3(370,596,0);
					GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[4].m_pTex, &m_rc, NULL, &m_vcPos, D3DXCOLOR(1,1,1,1.f));
				}
			}
		}
	}

}