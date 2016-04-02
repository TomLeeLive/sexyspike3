// Implementation of the CMcCamManager class.
//
////////////////////////////////////////////////////////////////////////////////


#include "../Include/INCLUDE.h"


CMcCamManager::CMcCamManager()
{
	m_pCam0		= NULL;
	m_pCam1		= NULL;
	m_pCam2		= NULL;
	m_pCam3		= NULL;
	m_pCam4		= NULL;
	m_pCam5		= NULL;

	m_pCamInfo = NULL;
	m_bJumpAnimationOK = FALSE;

	m_pos1		= D3DXVECTOR3(0,0,0);
	m_pos2		= D3DXVECTOR3(0,0,0);

	m_nActivatedCamNum = 0;
	m_nFocusCharacterNum = 0;
}

CMcCamManager::~CMcCamManager()
{
	Destroy();
}




INT CMcCamManager::Create(LPDIRECT3DDEVICE9 pDev)
{
	SAFE_NEWCREATE1(	m_pCam0,	CMcCamera	, pDev	);
	SAFE_NEWCREATE1(	m_pCam1,	CMcCamera	, pDev	);
	SAFE_NEWCREATE1(	m_pCam2,	CMcCamera	, pDev	);
	SAFE_NEWCREATE1(	m_pCam3,	CMcCamera	, pDev	);
	SAFE_NEWCREATE1(	m_pCam4,	CMcCamera	, pDev	);
	SAFE_NEWCREATE1(	m_pCam5,	CMcCamera	, pDev	);

	m_pCamInfo = new CCamInfo();
	return 0;
}




void CMcCamManager::Destroy()
{
	SAFE_DELETE(	m_pCam0		);
	SAFE_DELETE(	m_pCam1		);
	SAFE_DELETE(	m_pCam2		);
	SAFE_DELETE(	m_pCam3		);
	SAFE_DELETE(	m_pCam4		);
	SAFE_DELETE(	m_pCam5		);

	SAFE_DELETE( m_pCamInfo  );
}




INT CMcCamManager::FrameMove()
{
	SAFE_FRMOV(	m_pCam0		);

	m_pCam0->UpdateViewProj();

	m_pCamInfo->vcEye = m_pCam0->GetEye();
	m_pCamInfo->vcLook= m_pCam0->GetLook();
	m_pCamInfo->vcUp	= m_pCam0->GetUP();

	m_pCamInfo->vcAxisX = m_pCam0->GetAxisX();
	m_pCamInfo->vcAxisY = m_pCam0->GetAxisY();
	m_pCamInfo->vcAxisZ = m_pCam0->GetAxisZ();

	switch(m_nActivatedCamNum)
	{
	case 1:
		m_pCam1->UpdateViewProj();
		Camera1FrameMove(m_pCamInfo);
		break;
	case 2:
		m_pCam2->UpdateViewProj();
		Camera2FrameMove(m_pCamInfo);
		break;
	case 3:
		m_pCam3->UpdateViewProj();
		Camera3FrameMove(m_pCamInfo);
		break;
	case 4:
		m_pCam4->UpdateViewProj();
		Camera4FrameMove(m_pCamInfo);
		break;
	case 5:
		m_pCam5->UpdateViewProj();
		Camera5FrameMove(m_pCamInfo);
		break;
	default:
		break;
	}

	//m_nGamePhase==0(시작화면)에 들어가면 게임에 쓰는 카메라 m_pCam0의 위치가 초기화 됨.
	if(GMAIN->m_nGamePhase==0)
	{
		Camera0FrameMove(m_pCamInfo);
	}

	return 0;
}




////////////////////////////////////////////////////////////////////////////////
//
//게임 플레이 중에 사용되는 카메라
void	CMcCamManager::Camera0FrameMove(CCamInfo*	pCamInfo)
{
	m_pCam0->SetParamView(D3DXVECTOR3(-100.f,80.f,-200.f), D3DXVECTOR3( 50.f, 0.f, -50.f), D3DXVECTOR3(0,1,0));
}





////////////////////////////////////////////////////////////////////////////////
//캐릭터 선택화면에서의 카메라
//Camera.x 파일의 애니메이션 셋의 Frame위치 정보를 알아와서 카메라의 Look벡터와 Eye벡터에 적용
void	CMcCamManager::Camera1FrameMove(CCamInfo*	pCamInfo)
{
	LPD3DXFRAME pFrameCameraTarget = NULL;

	LPD3DXFRAME pFrameCameraEye = NULL;

	if(GMAIN->m_CharacterSelect->m_bOKClicked==TRUE)
	{
		if(m_bJumpAnimationOK == FALSE)
		{
			GMAIN->m_pCameraAniModel->m_pAniController->JumpAnimationSet(0);
			m_bJumpAnimationOK = TRUE;
		}
	}
	else
	{
		if(m_bJumpAnimationOK == FALSE)
		{
			GMAIN->m_pCameraAniModel->m_pAniController->JumpAnimationSet(1);
			m_bJumpAnimationOK = TRUE;
		}
	}


	D3DXMATRIX TM;
	D3DXMatrixIdentity(&TM);

	GMAIN->m_pCameraAniModel->FrameMove(GMAIN->m_fElapsedTime,&TM);


	pFrameCameraTarget = D3DXFrameFind(GMAIN->m_pCameraAniModel->m_pFrameRoot, "Anim_MatrixFrame_Camera01_Target");

	pFrameCameraEye = D3DXFrameFind(GMAIN->m_pCameraAniModel->m_pFrameRoot, "Anim_MatrixFrame_Camera01");

	if(pFrameCameraTarget !=0 && pFrameCameraEye !=0)
	{
		m_pCam1->SetParamView(
			D3DXVECTOR3(pFrameCameraEye->TransformationMatrix._41-10,
			pFrameCameraEye->TransformationMatrix._42-0.1f,
			pFrameCameraEye->TransformationMatrix._43+20),
			D3DXVECTOR3(pFrameCameraTarget->TransformationMatrix._41-10,
			pFrameCameraTarget->TransformationMatrix._42-0.1f,
			pFrameCameraTarget->TransformationMatrix._43+20),
			pCamInfo->vcUp);
	}
}




////////////////////////////////////////////////////////////////////////////////
//게임 시작시와 게임 완료 후 보여지게 되는 카메라
//코트의 중심을 Look벡터로 하고 일정 거리를 유지하며 원을 그리는 Eye벡터를 이용하여 카메라를 움직임
void	CMcCamManager::Camera2FrameMove(CCamInfo*	pCamInfo)
{
	FLOAT fAppTime        = DXUtil_Timer( TIMER_GETAPPTIME );

	m_pos1.x = 200.f*(FLOAT)cos(0.7f*fAppTime);
	m_pos1.z = 200.f*(FLOAT)sin(0.7f*fAppTime)-100.f;
	m_pos1.y = 25.f;
	
	m_pCam2->SetParamView(D3DXVECTOR3(m_pos1.x,m_pos1.y,m_pos1.z), D3DXVECTOR3(0,0,-100),	D3DXVECTOR3(0,1,0));
}




////////////////////////////////////////////////////////////////////////////////
//공격 성공시에 공격 성공한 팀의 마지막 공격자를 보여주는 이벤트에서 사용되는 카메라
//캐릭터의 골반 Frame의 위치 정보를 알아와서 카메라의 Look벡터에 적용
void	CMcCamManager::Camera3FrameMove(CCamInfo*	pCamInfo)
{
	LPD3DXFRAME pFrame = NULL;

	if(GMAIN->m_ppPlayers[m_nFocusCharacterNum] != NULL)
	{

		pFrame = D3DXFrameFind(GMAIN->m_ppPlayers[m_nFocusCharacterNum]->m_pFrameRoot, "Anim_MatrixFrame_o01_J_Neck");
		FLOAT fAppTime        = DXUtil_Timer( TIMER_GETAPPTIME );

		m_pos2.x = 30.f*(FLOAT)cos(0.7f*fAppTime)+( (CFrame*)pFrame )->m_CombinedTransformationMatrix._41;
		m_pos2.z = 30.f*(FLOAT)sin(0.7f*fAppTime)+( (CFrame*)pFrame )->m_CombinedTransformationMatrix._43;
		m_pos2.y = 1.f;

		m_pCam3->SetParamView(D3DXVECTOR3(m_pos2.x,m_pos2.y,m_pos2.z),
								D3DXVECTOR3(( (CFrame*)pFrame )->m_CombinedTransformationMatrix._41,
											( (CFrame*)pFrame )->m_CombinedTransformationMatrix._42,
											( (CFrame*)pFrame )->m_CombinedTransformationMatrix._43),
											D3DXVECTOR3(0,1,0));
	}
}



////////////////////////////////////////////////////////////////////////////////
//코트 체인지시에 캐릭터의 림보동작을 보여주는 이벤트시에 사용되는 카메라
//특정 캐릭터의 골반으로 부터 일정 거리를 유지한다
void	CMcCamManager::Camera4FrameMove(CCamInfo*	pCamInfo)
{
	m_pCam4->SetParamView(pCamInfo->vcEye, pCamInfo->vcEye - 100.f * pCamInfo->vcAxisZ,	D3DXVECTOR3(0,1,0));
}



////////////////////////////////////////////////////////////////////////////////
//
//
void	CMcCamManager::Camera5FrameMove(CCamInfo*	pCamInfo)
{
	m_pCam5->SetParamView(pCamInfo->vcEye, pCamInfo->vcLook,	pCamInfo->vcUp);
}


void	CMcCamManager::SpecificCameraON(INT CamNum)
{
	m_nActivatedCamNum = CamNum;
}

void CMcCamManager::FocusCharacterNum(INT FocusCharacterNum)
{
	m_nFocusCharacterNum = FocusCharacterNum;
}

