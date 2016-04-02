
#pragma		once

#ifndef		__CANIMATIONCONTROLLER_H_
#define		__CANIMATIONCONTROLLER_H_

class		CAnimationController
{
private:

	friend	class	CAnimationModel;

	LPDIRECT3DDEVICE9			m_pd3dDevice;
	LPD3DXANIMATIONCONTROLLER	m_pAniController;

	BOOL						m_bPaused;			//	�ִϸ��̼��� ������ �������� Ȯ��
	BOOL						m_bKeyPressed;		//	Ű�� ������ ���������ƴ���( ĳ���� ���ϸ��̼� ���� ������ �� ��� );
	
	DWORD						m_numAniSet;		//	�ִϸ��̼� �� ����
	DWORD						m_currentAniSet;	//	���� �ִϸ��̼� ��
	DWORD						m_currentTrack;		//	���� �ִϸ��̼� Ʈ��
	DOUBLE						m_fTransitionTime;

public:
	CAnimationController();
	virtual	~CAnimationController();

	HRESULT	InitDeviceObjects( LPDIRECT3DDEVICE9 pDevice );
	HRESULT	InitDeviceObjects( LPDIRECT3DDEVICE9 pDevice, LPD3DXANIMATIONCONTROLLER pAniController );
	HRESULT	RestoreDeviceObjects();
	HRESULT	InvalidateDeviceObjects();
	HRESULT	DeleteDeviceObjects();

	HRESULT	FrameMove( float	fElapsedTime );

	//	Ư�� �ִϸ��̼� ������ ����
	HRESULT	TransitionAnimation( DWORD	dwNewSet );
	
	//	Ű�Է¿� ���� �ִϸ��̼� ����
	LRESULT	HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//	Inline	Function
	//	...
	
};

#endif