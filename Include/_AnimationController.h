
#pragma		once

#ifndef		__CANIMATIONCONTROLLER_H_
#define		__CANIMATIONCONTROLLER_H_

class		CAnimationController
{
private:

	friend	class	CAnimationModel;

	LPDIRECT3DDEVICE9			m_pd3dDevice;
	LPD3DXANIMATIONCONTROLLER	m_pAniController;

	BOOL						m_bPaused;			//	애니메이션이 정지된 상태인지 확인
	BOOL						m_bKeyPressed;		//	키가 눌려진 상태인지아닌지( 캐리터 에니메이션 셋을 변경할 때 사용 );
	
	DWORD						m_numAniSet;		//	애니메이션 셋 갯수
	DWORD						m_currentAniSet;	//	현재 애니메이션 셋
	DWORD						m_currentTrack;		//	현재 애니메이션 트랙
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

	//	특정 애니메이션 셋으로 점프
	HRESULT	TransitionAnimation( DWORD	dwNewSet );
	
	//	키입력에 따른 애니메이션 조작
	LRESULT	HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//	Inline	Function
	//	...
	
};

#endif