#include "../Include/INCLUDE.h"

CHowtoPlay::CHowtoPlay(void)
{
}

CHowtoPlay::~CHowtoPlay(void)
{
}

//���� ���۹� ���� ȭ�� ����
INT CHowtoPlay::HowtoPlayFrameMove()
{
	FRAMEMOVE( GMAIN->m_pBall, GMAIN->m_fElapsedTime );
	FRAMEMOVE( GMAIN->m_pBackground, GMAIN->m_fElapsedTime );
	FRAMEMOVE( GMAIN->m_pNet, GMAIN->m_fElapsedTime );

	SAFE_FRMOV(	GMAIN->m_pInput	);
	SAFE_FRMOV(	GMAIN->m_pCamMn	);
	
	return 0;
}

//���� ���۹� ���� ȭ�� ������
HRESULT CHowtoPlay::HowtoPlayRender()
{
	HRESULT hr=S_OK;
	/////////////////////////////////////////////////////////////////////////
	//09-08-04 ������
	GMAIN->m_pCamMn->SpecificCameraON(0);
	GMAIN->m_pCamMn->m_pCam0->SetTransformViw();
	GMAIN->m_pCamMn->m_pCam0->SetTransformPrj();
	g_matViewProj	= GMAIN->m_pCamMn->m_pCam0->GetMatrixViw()*GMAIN->m_pCamMn->m_pCam0->GetMatrixPrj();
	/////////////////////////////////////////////////////////////////////////


	//09-08-13 ������
	SpriteDraw();

	return S_OK;
}


void CHowtoPlay::ReadyPlay(){}

void CHowtoPlay::InitVal(){}


void CHowtoPlay::SpriteDraw()
{
		int i=0;
	
		D3DXMATRIX	mtW;
	
		D3DXVECTOR3 vcDelta = g_pApp->m_pInput->GetMousePos();
	
		GMAIN->m_pd3dSprite->Begin(D3DXSPRITE_ALPHABLEND);
	
		//HowToPlay �̹��� ���
		GMAIN->m_vcPos=D3DXVECTOR3(0,0,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[54].m_pTex, &GMAIN->m_rc, NULL, &GMAIN->m_vcPos, D3DXCOLOR(1,1,1,1.f));

		//���� ��ư BOOL �� TRUE�� ����
		GMAIN->m_vcPos=D3DXVECTOR3(479,5,0);
		GMAIN->m_pd3dSprite->Draw(GMAIN->m_pGameTex[42].m_pTex, &GMAIN->m_rc, NULL, &GMAIN->m_vcPos, D3DXCOLOR(1,1,1,1.f));
	
		//�׸��α� ��ư�� Ŭ���Ͽ��� ���
		if(vcDelta.x > GMAIN->m_rcWindowClient.right*482/1024
			&& vcDelta.x < GMAIN->m_rcWindowClient.right*540/1024
			&& vcDelta.y > GMAIN->m_rcWindowClient.bottom*7/768
			&& vcDelta.y < GMAIN->m_rcWindowClient.bottom*26/768)
		{
			if(g_pApp->m_pInput->BtnDown(0))
			{
				GMAIN->m_pMp[2]->Reset();
				GMAIN->m_pMp[2]->Stop();
				GMAIN->m_pMp[2]->Play();
				GMAIN->m_bQuitButton=TRUE;
			}
		}

	
		if(GMAIN->QuitButtonClickCheck())
		{
			InitVal();
		}
	
		GMAIN->m_pd3dSprite->End();
}
