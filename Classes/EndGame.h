#pragma once

class CEndGame
{
public:
	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;						//�����ġ

	CEndGame(void);
	virtual ~CEndGame(void);

	INT EndPlayFrameMove(); //���� ����ȭ�� ����
	void EndPlayRender(); //���� ����ȭ�� ������

	void SpriteDraw();
	INT RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax);
};
