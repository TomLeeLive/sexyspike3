#pragma once

class CStartScreen
{
public:
	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;						//배경위치


	CStartScreen(void);
	virtual ~CStartScreen(void);

	INT StartScreenFrameMove(); //시작화면 갱신
	void StartScreenRender();  //시작화면 렌더링

	void SpriteDraw();
	INT RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax);	//특정 영역 마우스 작업 확인
};
