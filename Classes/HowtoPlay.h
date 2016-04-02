#pragma once

class CHowtoPlay
{
public:
	CHowtoPlay(void);
	virtual ~CHowtoPlay(void);

	INT HowtoPlayFrameMove();  //게임 조작법 설명 화면 갱신
	HRESULT HowtoPlayRender();    //게임 조작법 설명 화면 렌더링

	void ReadyPlay();
	void InitVal();
	void SpriteDraw();
};
