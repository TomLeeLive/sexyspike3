// Interface for the CMcCamManager class.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _McCamManager_H_
#define _McCamManager_H_

struct CCamInfo
{
	D3DXVECTOR3	vcEye;
	D3DXVECTOR3	vcLook;
	D3DXVECTOR3	vcUp;

	D3DXVECTOR3	vcAxisX;
	D3DXVECTOR3	vcAxisY;
	D3DXVECTOR3	vcAxisZ;
	CCamInfo()
	{
		vcEye = D3DXVECTOR3(0,0,0); vcLook = D3DXVECTOR3(0,0,0); vcUp = D3DXVECTOR3(0,0,0);
		vcAxisX = D3DXVECTOR3(0,0,0); vcAxisY= D3DXVECTOR3(0,0,0); vcAxisZ = D3DXVECTOR3(0,0,0);
	}
	virtual ~CCamInfo(){};
};

class CMcCamManager
{
	//09-09-01 ������
	INT	m_nFocusCharacterNum;		//��Ŀ���� ���� ĳ���� �ѹ�
	INT m_nActivatedCamNum;		//Ȱ��ȭ�� ī�޶� �ѹ�

	D3DXVECTOR3	m_pos1;
	D3DXVECTOR3	m_pos2;
	CCamInfo*		m_pCamInfo;
public:
	CMcCamera*		m_pCam0		;
	CMcCamera*		m_pCam1		;
	CMcCamera*		m_pCam2		;
	CMcCamera*		m_pCam3		;
	CMcCamera*		m_pCam4		;
	CMcCamera*		m_pCam5		;

	BOOL			m_bJumpAnimationOK;

public:
	CMcCamManager();
	virtual ~CMcCamManager();

	INT		Create(LPDIRECT3DDEVICE9);
	void	Destroy();

	INT		FrameMove();

	void	Camera0FrameMove(CCamInfo*	pCamInfo);
	void	Camera1FrameMove(CCamInfo*	pCamInfo);
	void	Camera2FrameMove(CCamInfo*	pCamInfo);
	void	Camera3FrameMove(CCamInfo*	pCamInfo);
	void	Camera4FrameMove(CCamInfo*	pCamInfo);
	void	Camera5FrameMove(CCamInfo*	pCamInfo);

	void	SpecificCameraON(INT CamNum); //Ư�� ī�޶� Ȱ��ȭ
	void	FocusCharacterNum(INT FocusCharacterNum); //Ư�� ĳ���Ϳ� ��Ŀ�� ->3�� ī�޶� ��!
};

#endif