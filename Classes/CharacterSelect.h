#pragma once

class CChatacterIcon
{
public:
	D3DXVECTOR3 vcPos;			//ĳ���� ������ ��ġ
	INT		number;					//ĳ���� ��ȣ
	INT		xmin;					//ĳ���� ������ x��ǥ min
	INT		xmax;                //ĳ���� ������ x��ǥ max
	INT		ymin;					//ĳ���� ������ y��ǥ min
	INT		ymax;					//ĳ���� ������ y��ǥ max

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
	char*			m_strAnimationSetName[4];	//�ִϸ��̼Ǽ� �̸�
	RECT			m_rc;
	D3DXVECTOR3	m_vcPos;						//�����ġ

	BOOL m_bEverybodyReady;						//��Ƽ�÷��̽� ��� �÷��̾ �÷��� �غ� �Ǿ����� TRUE

	BOOL m_bAllRandomSelect;						//ĳ���� ���� ���� ������ �� TRUE�� �Ǿ�� ��
	BOOL m_bSelectFinished;						//ĳ���� ������ ������ �� TRUE�� �Ǿ�� ��
	BOOL m_bPlayer1SelectFinished;				//�÷��̾� ĳ���� ������ ������ �� TRUE�� �Ǿ�� ��
	BOOL m_bPartnerSelectFinished;				//��Ʈ�� ������ ������ �� TRUE�� �Ǿ�� ��
	BOOL m_bReady;									//ĳ���� ������ ������ ��ǻ�� ĳ���Ͱ� ���õǸ� TRUE�� �Ǿ�� ��
	BOOL m_bOKClicked;							//OK��ư Ŭ���Ͽ��� ��
	BOOL m_bGameStart;							//���� ����-> m_nGamePhase = 1�� ����!

	CCharacterSelect(void);
	virtual ~CCharacterSelect(void);

	void Destroy();

	void InitVal();
	void CharacterRandomSelect();				//�÷��̾� ĳ����1�� ������ ������ ĳ���� 2,3,4�� ���� ����

	int CharacterSelectFrameMove();				//ĳ���ͼ���ȭ�� ����
	void CharaterSelectRender();					//ĳ���ͼ���ȭ�� ������

	HRESULT CharacterRender();					//ĳ���� ���� �� ������ ĳ���͸� ������

	void CharacterChooseRender();				//ĳ���Ͱ� ���õȰ��� ����(�ִϸ��̼Ǽ°� �����Ͽ�...)

	//09-08-26 ������
	HRESULT CharacterAnimationPlayTypeChange(Character* pModel);  //ĳ���;ִϸ��̼� �÷���Ÿ�� ����

	char* AniSetNameRandom(INT randnum);							//ĳ���;ִϸ��̼Ǽ��̸� ��������

	
	INT RectCheck(D3DXVECTOR3 vcDelta, int nXmin, int nXmax, int nYmin, int nYmax); //Ư�� ���� �ȿ��� ��ư Ŭ���� �Ǿ�����?
	/////////////////////////////////////////////////////////////////////////////
	void SpriteDraw();				
	void SpriteDrawSinglePlay(D3DXVECTOR3 vcDelta);		//�̱� �÷���: �޴� ��������Ʈ ��� & Ŀ�� ���ÿ� ���� ������Ʈ ����
	void SpriteDrawMultiPlay(D3DXVECTOR3 vcDelta);			//��Ƽ �÷���: �޴� ��������Ʈ ��� & Ŀ�� ���ÿ� ���� ������Ʈ ����
	void SpriteDrawEtc(D3DXVECTOR3 vcDelta);				//�޴�, ��ư, ���� ��ư ó�� ���...
	void SpriteDrawOKClicked(D3DXVECTOR3 vcDelta);			//OK��ư Ŭ�� ���� �� ��������Ʈ ���� ó��

	void SpriteDrawTeamSelect(D3DXVECTOR3 vcDelta);		//��Ƽ�÷��̽ÿ� �� ���� ��ư ó��

	void MultiPlayComputerCharacterRandomSelect();			//��Ƽ�÷��̽� ��ǻ�� ĳ���� ���� ���� :������ ȣ��Ʈ�� ��ǻ�� �÷��̾�� ���� �����Ͽ� ������ ������Ʈ
	
	void MultiPlayStatus();									//��Ƽ�÷��̽ÿ� �����Ȳ �ؽ�Ʈ ���

	BOOL MultiPlayMyCharacterSelect(BOOL bMyRoomPositionCheck,INT nCharacterNumber);//��Ƽ�÷��̽� �ٸ� �÷��̾ ������ ĳ���͸� �����ϰ� ���� ĳ���� ����
	
	BOOL MultiPlayAllPlayerReadyCheck();					//�ٸ��÷��̾���� ���� �Ǿ��°�? ȣ��Ʈ�� ���õ��� ���� �÷��̾��ȣ�� ��ŵ�ؼ� üũ

	BOOL MultiPlayEveryBodyReady();							//��� �÷��̾ ���� �Ǿ�����?										
};
