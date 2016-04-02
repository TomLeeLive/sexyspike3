
#pragma		once

#ifndef		__CCAMERAANIMATIONCONTROLLER_H_
#define		__CCAMERAANIMATIONCONTROLLER_H_

enum	eCameraPlayType{ ONETIME_PLAY = '0', LOOP_PLAY	= '1'};

class		CCameraAnimationController
{
public:
	enum eCameraPlayType			m_eCameraPlayType;	//	하나의 애니메이션 셋 재생이 완료 된 후 다음 행동 결정
	DWORD							m_currentAniSet;		//	현재 애니메이션 셋
	LPD3DXANIMATIONCONTROLLER	m_pAniController;
	BOOL							m_bAniFinished;		//  현재 애니메이션 셋이 끝나면 TRUE
private:
	friend	class	CCameraAnimationModel;
	LPDIRECT3DDEVICE9				m_pd3dDevice;
	BOOL							m_bPaused;				//	애니메이션이 정지된 상태인지 확인
	BOOL							m_bKeyPressed;			//	키가 눌려진 상태인지아닌지( 캐리터 에니메이션 셋을 변경할 때 사용 );
	DWORD							m_numAniSet;			//	애니메이션 셋 갯수
	DWORD							m_currentTrack;		//	현재 애니메이션 트랙
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

	//	특정 애니메이션 셋으로 점프, 블랜딩
	HRESULT	 TransitionAnimation( DWORD	dwNewSet );

	//  블랜딩 없이
	HRESULT	 JumpAnimationSet( DWORD	dwNewSet );
	
	//	키입력에 따른 애니메이션 조작
	LRESULT	HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//	Inline	Function
	//	...
	
};

#endif