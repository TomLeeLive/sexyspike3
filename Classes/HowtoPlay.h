#pragma once

class CHowtoPlay
{
public:
	CHowtoPlay(void);
	virtual ~CHowtoPlay(void);

	INT HowtoPlayFrameMove();  //���� ���۹� ���� ȭ�� ����
	HRESULT HowtoPlayRender();    //���� ���۹� ���� ȭ�� ������

	void ReadyPlay();
	void InitVal();
	void SpriteDraw();
};
