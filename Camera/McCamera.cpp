// Implementation of the CMcCamera class.
//
////////////////////////////////////////////////////////////////////////////////


#include "../Include/INCLUDE.h"


CMcCamera::CMcCamera()
{
	m_pDev	= NULL;

	D3DXMatrixIdentity(&m_mtViw);
	D3DXMatrixIdentity(&m_mtPrj);
	
	
	m_vcEye		= D3DXVECTOR3(0,0,-1);													// Camera position
	m_vcLook	= D3DXVECTOR3(0,0, 0);													// Look vector
	m_vcUp		= D3DXVECTOR3(0,1, 0);													// up vector
	
	m_fFov		= D3DX_PI/4.f;
	m_fAsp		= 800.f/600.f;
	m_fNr		= 1.f;
	m_fFr		= 10000.f;
}

CMcCamera::~CMcCamera()
{
	
}


INT CMcCamera::Create(LPDIRECT3DDEVICE9 pDev)
{
	m_pDev = pDev;

	SetParamView(D3DXVECTOR3(-100.f,80.f,-200.f), D3DXVECTOR3( 50.f, 0.f, -50.f), D3DXVECTOR3(0,1,0));

	return 0;
}


INT CMcCamera::FrameMove()
{
	if(GMAIN->m_nGamePhase !=0 && GMAIN->m_nGamePhase !=5 &&GMAIN->m_GameSceneManager->m_bEventSceneFinished ==TRUE)
	{
		D3DXVECTOR3 vcDelta = g_pApp->m_pInput->GetMouseEps();

		if(vcDelta.z !=0.f)
			MoveForward(-vcDelta.z* .1f, 1.f);

		if(g_pApp->m_pInput->KeyState('W'))
			MoveForward( 50.f*GMAIN->m_fElapsedTime, 1.f);

		if(g_pApp->m_pInput->KeyState('S'))
			MoveForward(-50.f*GMAIN->m_fElapsedTime, 1.f);

		if(g_pApp->m_pInput->KeyState('A'))
			MoveSideward(-50.f*GMAIN->m_fElapsedTime);

		if(g_pApp->m_pInput->KeyState('D'))
			MoveSideward(50.f*GMAIN->m_fElapsedTime);

		if(g_pApp->m_pInput->BtnState(1))
			Rotate(&vcDelta, .2f);

		UpdateViewProj();
	}
	return 0;
}


void CMcCamera::SetEye(D3DXVECTOR3 v)
{
	m_vcEye = v;
}

void CMcCamera::SetLook(D3DXVECTOR3 v)
{
	m_vcLook = v;
}

void CMcCamera::SetUP(D3DXVECTOR3 v)
{
	m_vcUp = v;
}


void CMcCamera::SetParamView(D3DXVECTOR3 vcEye, D3DXVECTOR3 vcLook, D3DXVECTOR3 vcUp)
{
	m_vcEye		= vcEye;
	m_vcLook	= vcLook;
	m_vcUp		= vcUp;
}

void CMcCamera::SetParamProj(FLOAT fFov,FLOAT fAsp,FLOAT fNear,FLOAT fFar)
{
	m_fFov	= fFov;
	m_fAsp	= fAsp;
	m_fNr	= fNear;
	m_fFr	= fFar;
}


void CMcCamera::UpdateViewProj()
{
	D3DXMatrixPerspectiveFovLH(&m_mtPrj, m_fFov, m_fAsp, m_fNr, m_fFr);
	D3DXMatrixLookAtLH(&m_mtViw, &m_vcEye, &m_vcLook, &m_vcUp);
	
	D3DXMatrixInverse(&m_mtViwI, NULL, &m_mtViw);
	m_mtBill = m_mtViwI;
	
	m_mtBill._41 = 0.f;
	m_mtBill._42 = 0.f;
	m_mtBill._43 = 0.f;
	
	m_vcAxisX.x = m_mtViw._11;
	m_vcAxisX.y = m_mtViw._21;
	m_vcAxisX.z = m_mtViw._31;
	
	m_vcAxisY.x = m_mtViw._12;
	m_vcAxisY.y = m_mtViw._22;
	m_vcAxisY.z = m_mtViw._32;
	
	m_vcAxisZ.x = m_mtViw._13;
	m_vcAxisZ.y = m_mtViw._23;
	m_vcAxisZ.z = m_mtViw._33;
}



void CMcCamera::SetTransformViw()
{
	m_pDev->SetTransform(D3DTS_VIEW, &m_mtViw);
}

void CMcCamera::SetTransformPrj()
{
	m_pDev->SetTransform(D3DTS_PROJECTION, &m_mtPrj);	
}




void CMcCamera::MoveSideward(FLOAT fSpeed)
{
	D3DXVECTOR3 tmp(m_mtViw._11, 0, m_mtViw._31);
	D3DXVec3Normalize(&tmp,&tmp);

	m_vcEye  += tmp * fSpeed;
	m_vcLook += tmp * fSpeed;
}


void CMcCamera::MoveForward(FLOAT fSpeed, FLOAT fY)
{
	D3DXVECTOR3 tmp(m_mtViw._13, m_mtViw._23*fY, m_mtViw._33);
	D3DXVec3Normalize(&tmp,&tmp);

	m_vcEye  += tmp * fSpeed;
	m_vcLook += tmp * fSpeed;
}



void CMcCamera::Rotate(const D3DXVECTOR3* vcDelta, float fSpeed)
{
	D3DXVECTOR3 vcEps = *vcDelta;

	FLOAT	fYaw	;
	FLOAT	fPitch	;

	D3DXVECTOR3 vcZ;
	D3DXVECTOR3 vcY;
	D3DXVECTOR3 vcX;

	D3DXMATRIX rtY;
	D3DXMATRIX rtX;

	// 월드 좌표 y 축에 대한 회전
	fYaw	= D3DXToRadian(vcEps.x * fSpeed);
	D3DXMatrixRotationY(&rtY, fYaw);

	vcZ = m_vcLook-m_vcEye;
	vcY = D3DXVECTOR3(m_mtViw._12, m_mtViw._22, m_mtViw._32);

	D3DXVec3TransformCoord(&vcZ, &vcZ, &rtY);
	D3DXVec3TransformCoord(&vcY, &vcY, &rtY);

	m_vcLook= vcZ + m_vcEye;
	m_vcUp	= vcY;
	D3DXMatrixLookAtLH(&m_mtViw, &m_vcEye, &m_vcLook, &m_vcUp);


	// 카메라의 x 축에 대한 회전
	fPitch	= D3DXToRadian(vcEps.y * fSpeed);
	vcX	= D3DXVECTOR3(m_mtViw._11, m_mtViw._21, m_mtViw._31);
	vcY = D3DXVECTOR3(m_mtViw._12, m_mtViw._22, m_mtViw._32);
	vcZ = m_vcLook-m_vcEye;

	D3DXMatrixRotationAxis(&rtX, &vcX, fPitch);
	D3DXVec3TransformCoord(&vcZ, &vcZ, &rtX);
	D3DXVec3TransformCoord(&vcY, &vcY, &rtX);

	m_vcLook= vcZ + m_vcEye;
	m_vcUp	= vcY;
	D3DXMatrixLookAtLH(&m_mtViw, &m_vcEye, &m_vcLook, &m_vcUp);
}

D3DXVECTOR3 CMcCamera::GetInverseViewMatrix()
{
	D3DXVECTOR3 mtViwI = D3DXVECTOR3(m_mtViwI._31,m_mtViwI._32,m_mtViwI._33);
	return mtViwI;
}