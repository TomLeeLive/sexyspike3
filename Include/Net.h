
#pragma		once

#ifndef		__NET_H_
#define		__NET_H_

class		CNet
{
	CD3DMesh				m_pillar;
	CD3DMesh				m_net;
	BoundingPlane			m_BoundingNet;
	BoundingPlane			m_BoundingMesh;
	D3DXMATRIX				m_matWorld;
	D3DXMATRIX				m_matWorld1;
	D3DXVECTOR3				m_vAxis;
	bool					m_bNowDance;
	float					m_fDanceTime;
	float					m_fDanceWeight;

public:
	CNet();
	~CNet();

	HRESULT		FrameMove( float fElapsedTime );
	HRESULT		Render(LPDIRECT3DDEVICE9 pDevice);

	HRESULT		InitDeviceObjects( LPDIRECT3DDEVICE9	pDevice );
	HRESULT		RestoreDeviceObjects(LPDIRECT3DDEVICE9 pDevice);
	HRESULT		InvalidateDeviceObjects();
	HRESULT		DeleteDeviceObjects();

	const	BoundingPlane*	GetBoundingNet() { return	&m_BoundingNet; }
	const	BoundingPlane*	GetBoundingMesh(){ return	&m_BoundingMesh; }

	HRESULT	ComputeBlendFactor();
	void	MakeNetDance(LPD3DXVECTOR3 vIncidence);
};



#endif