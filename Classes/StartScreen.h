#pragma once

class CStartScreen
{
public:
	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;						//�����ġ


	CStartScreen(void);
	virtual ~CStartScreen(void);

	INT StartScreenFrameMove(); //����ȭ�� ����
	void StartScreenRender();  //����ȭ�� ������

	void SpriteDraw();
	INT RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax);	//Ư�� ���� ���콺 �۾� Ȯ��
};
