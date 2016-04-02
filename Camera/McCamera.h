// Interface for the CMcCamera class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _McCamera_H_
#define _McCamera_H_

class CMcCamera
{
public:
	D3DXMATRIX	m_mtViw;														// View Matrix
	D3DXMATRIX	m_mtPrj;														// Projection Matrix
protected:
	LPDIRECT3DDEVICE9	m_pDev;
	
	D3DXMATRIX	m_mtViwI;														// Inverse View Matrix
	D3DXMATRIX	m_mtBill;														// Billboard Projection Matrix
	
	
	// For View Matrix
	D3DXVECTOR3	m_vcEye;														// Camera position
	D3DXVECTOR3	m_vcLook;														// Look vector
	D3DXVECTOR3	m_vcUp;															// up vector
	
	// For Projection Matrix
	FLOAT		m_fFov;															// Field of View
	FLOAT		m_fAsp;															// Aspect
	FLOAT		m_fNr;															// Near
	FLOAT		m_fFr;															// Far

	D3DXVECTOR3		m_vcAxisX;														// Camera Axis X
	D3DXVECTOR3		m_vcAxisY;														// Camera Axis Y
	D3DXVECTOR3		m_vcAxisZ;														// Camera Axis Z
	
public:
	CMcCamera();
	virtual ~CMcCamera();

	INT		Create(LPDIRECT3DDEVICE9);
	INT		FrameMove();

	D3DXMATRIX	GetMatrixViw()			{	return m_mtViw;		}
	D3DXMATRIX	GetMatrixPrj()			{	return m_mtPrj;		}
	D3DXMATRIX	GetMatrixViwI()			{	return m_mtViwI;	}
	D3DXMATRIX	GetMatrixBill()			{	return m_mtBill;	}
	
	D3DXVECTOR3	GetEye()				{	return m_vcEye;		}
	D3DXVECTOR3	GetLook()				{	return m_vcLook;	}
	D3DXVECTOR3	GetUP()					{	return m_vcUp;		}

	void	SetEye(D3DXVECTOR3 v);
	void	SetLook(D3DXVECTOR3 v);
	void	SetUP(D3DXVECTOR3 v);
	
	void	SetFov(FLOAT _fFov)		{	m_fFov = _fFov;		}
	void	SetAspect(FLOAT _fAsp)	{	m_fAsp = _fAsp;		}
	void	SetNear(FLOAT _fNear)	{	m_fNr  = _fNear;	}
	void	SetFar(FLOAT _fFar)		{	m_fFr  = _fFar;		}

	FLOAT	GetFov()				{	return m_fFov;		}
	FLOAT	GetAspect()				{	return m_fAsp;		}
	FLOAT	GetNear()				{	return m_fNr;		}	
	FLOAT	GetFar()				{	return m_fFr;		}

	D3DXVECTOR3	GetAxisX()				{	return m_vcAxisX;	}
	D3DXVECTOR3	GetAxisY()				{	return m_vcAxisY;	}
	D3DXVECTOR3	GetAxisZ()				{	return m_vcAxisZ;	}
	
	
	void	SetParamView(D3DXVECTOR3 vcEye, D3DXVECTOR3 vcLook, D3DXVECTOR3 vcUp);
	void	SetParamProj(FLOAT fFov, FLOAT fAsp, FLOAT fNear, FLOAT fFar);
	
	void	UpdateViewProj();
	void	SetTransformViw();
	void	SetTransformPrj();
	
protected:
	void	MoveSideward(FLOAT	fSpeed);
	void	MoveForward	(FLOAT	fSpeed, FLOAT fY=0);
	void	Rotate(const D3DXVECTOR3* vcDelta, float fSpeed);

public:
	//09-08-25
	D3DXVECTOR3 GetInverseViewMatrix();
};

#endif
