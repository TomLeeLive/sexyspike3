#pragma once

class CChatacterIcon
{
public:
	D3DXVECTOR3 vcPos;			//캐릭터 아이콘 위치
	INT		number;					//캐릭터 번호
	INT		xmin;					//캐릭터 아이콘 x좌표 min
	INT		xmax;                //캐릭터 아이콘 x좌표 max
	INT		ymin;					//캐릭터 아이콘 y좌표 min
	INT		ymax;					//캐릭터 아이콘 y좌표 max

	CChatacterIcon(D3DXVECTOR3 _vcPos, INT _number, INT _xmin, INT _xmax, INT _ymin, INT _ymax)
	{
		vcPos  = _vcPos;
		number = _number;
		xmin = _xmin;
		xmax = _xmax;
		ymin = _ymin;
		ymax = _ymax;
	}
	virtual ~CChatacterIcon()
	{
	}
};


class CCharacterSelect
{
	CChatacterIcon* m_Character[9];
public:
	char*			m_strAnimationSetName[4];	//애니매이션셋 이름
	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;						//배경위치

	BOOL m_bEverybodyReady;						//멀티플레이시 모든 플레이어가 플레이 준비가 되었을때 TRUE

	BOOL m_bAllRandomSelect;						//캐릭터 선택 전부 랜덤일 때 TRUE가 되어야 함
	BOOL m_bSelectFinished;						//캐릭터 선택이 끝났을 때 TRUE가 되어야 함
	BOOL m_bPlayer1SelectFinished;				//플레이어 캐릭터 선택이 끝났을 때 TRUE가 되어야 함
	BOOL m_bPartnerSelectFinished;				//파트너 선택이 끝났을 때 TRUE가 되어야 함
	BOOL m_bReady;									//캐릭터 선택이 끝나고 컴퓨터 캐릭터가 선택되면 TRUE가 되어야 함
	BOOL m_bOKClicked;							//OK버튼 클릭하였을 때
	BOOL m_bGameStart;							//게임 시작-> m_nGamePhase = 1로 변경!

	CCharacterSelect(void);
	virtual ~CCharacterSelect(void);

	void Destroy();

	void InitVal();
	void CharacterRandomSelect();				//플레이어 캐릭터1을 제외한 나머지 캐릭터 2,3,4를 랜덤 선택

	int CharacterSelectFrameMove();				//캐릭터선택화면 갱신
	void CharaterSelectRender();					//캐릭터선택화면 렌더링

	HRESULT CharacterRender();					//캐릭터 선택 후 각각의 캐릭터를 렌더링

	void CharacterChooseRender();				//캐릭터가 선택된것을 렌더(애니메이션셋과 관련하여...)

	//09-08-26 이재준
	HRESULT CharacterAnimationPlayTypeChange(Character* pModel);  //캐릭터애니메이션 플레이타입 변경

	char* AniSetNameRandom(INT randnum);							//캐릭터애니메이션셋이름 랜덤변경

	
	INT RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax); //특정 영역 안에서 버튼 클릭이 되었는지?
	/////////////////////////////////////////////////////////////////////////////
	void SpriteDraw();				
	void SpriteDrawSinglePlay(D3DXVECTOR3 vcDelta);		//싱글 플레이: 메뉴 스프라이트 출력 & 커서 선택에 따른 스테이트 변경
	void SpriteDrawMultiPlay(D3DXVECTOR3 vcDelta);			//멀티 플레이: 메뉴 스프라이트 출력 & 커서 선택에 따른 스테이트 변경
	void SpriteDrawEtc(D3DXVECTOR3 vcDelta);				//메뉴, 버튼, 종료 버튼 처리 등등...
	void SpriteDrawOKClicked(D3DXVECTOR3 vcDelta);			//OK버튼 클릭 했을 때 스프라이트 관련 처리

	void SpriteDrawTeamSelect(D3DXVECTOR3 vcDelta);		//멀티플레이시에 팀 선택 버튼 처리

	void MultiPlayComputerCharacterRandomSelect();			//멀티플레이시 컴퓨터 캐릭터 랜덤 선택 :공석을 호스트가 컴퓨터 플레이어로 랜덤 선택하여 서버에 업데이트
	
	void MultiPlayStatus();									//멀티플레이시에 현재상황 텍스트 출력

	BOOL MultiPlayMyCharacterSelect(BOOL bMyRoomPositionCheck,INT nCharacterNumber);//멀티플레이시 다른 플레이어가 선택한 캐릭터를 제외하고 나의 캐릭터 선택
	
	BOOL MultiPlayAllPlayerReadyCheck();					//다른플레이어들이 레디가 되었는가? 호스트와 선택되지 않은 플레이어번호는 스킵해서 체크

	BOOL MultiPlayEveryBodyReady();							//모든 플레이어가 레디가 되었는지?										
};
