
#pragma		once

#ifndef		__CCAMERAANIMATIONCONTROLLER_H_
#define		__CCAMERAANIMATIONCONTROLLER_H_

enum	eCameraPlayType{ ONETIME_PLAY = '0', LOOP_PLAY	= '1'};

class		CCameraAnimationController
{
public:
	enum eCameraPlayType			m_eCameraPlayType;	//	�ϳ��� �ִϸ��̼� �� ����� �Ϸ� �� �� ���� �ൿ ����
	DWORD							m_currentAniSet;		//	���� �ִϸ��̼� ��
	LPD3DXANIMATIONCONTROLLER	m_pAniController;
	BOOL							m_bAniFinished;		//  ���� �ִϸ��̼� ���� ������ TRUE
private:
	friend	class	CCameraAnimationModel;
	LPDIRECT3DDEVICE9				m_pd3dDevice;
	BOOL							m_bPaused;				//	�ִϸ��̼��� ������ �������� Ȯ��
	BOOL							m_bKeyPressed;			//	Ű�� ������ ���������ƴ���( ĳ���� ���ϸ��̼� ���� ������ �� ��� );
	DWORD							m_numAniSet;			//	�ִϸ��̼� �� ����
	DWORD							m_currentTrack;		//	���� �ִϸ��̼� Ʈ��
	DOUBLE							m_fTransitionTime;

public:
	CCameraAnimationController();
	virtual	~CCameraAnimationController();

	HRESULT	InitDeviceObjects( LPDIRECT3DDEVICE9 pDevice );
	HRESULT	InitDeviceObjects( LPDIRECT3DDEVICE9 pDevice, LPD3DXANIMATIONCONTROLLER pAniController );
	HRESULT	RestoreDeviceObjects();
	HRESULT	InvalidateDeviceObjects();
	HRESULT	DeleteDeviceObjects();

	HRESULT	FrameMove( float	fElapsedTime );

	//	Ư�� �ִϸ��̼� ������ ����, ����
	HRESULT	 TransitionAnimation( DWORD	dwNewSet );

	//  ���� ����
	HRESULT	 JumpAnimationSet( DWORD	dwNewSet );
	
	//	Ű�Է¿� ���� �ִϸ��̼� ����
	LRESULT	HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//	Inline	Function
	//	...
	
};

#endif