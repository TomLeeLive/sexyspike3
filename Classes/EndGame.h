#pragma once

class CEndGame
{
public:
	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;						//배경위치

	CEndGame(void);
	virtual ~CEndGame(void);

	INT EndPlayFrameMove(); //게임 종료화면 갱신
	void EndPlayRender(); //게임 종료화면 렌더링

	void SpriteDraw();
	INT RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax);
};
