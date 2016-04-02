
#include	"./Include/DEFAULT_INCLUDE.h"
#include	"./Include/INCLUDE.h"

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT SexySpike::FrameMove()
{

	if(m_MultiPlay->m_MyInfo.State == CLIENT_PLAY)
		m_nGamePhase = 1;

	//09-08-25 이재준
	m_nRedTeamScore = g_score[RED];
	m_nBlueTeamScore = g_score[BLUE];

	switch(m_nGamePhase)
	{
	case 0:	 //첫화면
		m_StartScreen->StartScreenFrameMove();
		break;
	case 1: //싱글플레이
		if(m_bTest)
		{
			SinglePlayFrameMove();
		}
		else
		{
			m_GameSceneManager->FrameMove();
			if(m_GameSceneManager->m_bEventSceneFinished == TRUE)
			{SinglePlayFrameMove();}
		}
		break;

	case 2: //멀티플레이
		m_MultiPlay->MultiPlayFrameMove();
		break;

	case 3: //HowtoPlay
		m_HowtoPlay->HowtoPlayFrameMove();
		break;
	case 4: //게임종료
		m_EndGame->EndPlayFrameMove();
		break;
	case 5: //캐릭터 선택 화면
		m_CharacterSelect->CharacterSelectFrameMove();
		break;
	}


	this->BGMPlay();

	//if(m_pMp[0] != NULL && m_pMp[1] != NULL )
	//{
	//	this->BGMPlay();
	//}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT SexySpike::Render()
{
	// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , 0x00002244, 1.f, 0 );


		switch(m_nGamePhase)
		{
		case 0:	 //첫화면
			m_StartScreen->StartScreenRender();
			break;
		case 1: //싱글플레이
			if(m_bTest)
			{
				SinglePlayRender();
			}
			else
			{
				if(m_GameSceneManager->m_bEventSceneFinished == TRUE)
				{SinglePlayRender();}
				else{m_GameSceneManager->Render();}
			}
			break;
		case 2: //멀티플레이
			m_MultiPlay->MultiPlayRender();
			break;

		case 3: //HowtoPlay
			m_HowtoPlay->HowtoPlayRender();
			break;
		case 4: //게임종료
			m_EndGame->EndPlayRender();
			break;
		case 5: //캐릭터 선택 화면
			m_CharacterSelect->CharaterSelectRender();
			break;
		}

		// End the scene.
		m_pd3dDevice->EndScene();
	}

	//m_pd3dDevice->Present(NULL,NULL,NULL,NULL);

	return S_OK;
}



