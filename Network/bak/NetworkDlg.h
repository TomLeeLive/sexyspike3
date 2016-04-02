#pragma once

#pragma warning(disable:4065)

#define DIALOG_HIDE			0
#define DIALOG_WAIT			1
#define DIALOG_CONNECTION	2
#define DIALOG_LOGIN		3
#define DIALOG_JOIN			4
#define DIALOG_LOBBY		5
#define DIALOG_CREATE_ROOM	6

class CNetworkDlg
{
protected:

	HINSTANCE		m_hInst				;
public:
	int				m_State				;					// ���̾�α� �÷���
	HWND			m_hWait				;					// Wait�޼���
	HWND			m_hConnection		;					// ��������
	HWND			m_hLogin			;					// �α���
	HWND			m_hLobby			;					// �κ�
	HWND			m_hJoin				;					// ĳ����
	HWND			m_hCreateRoom		;					// �游���
	HWND*			m_phWnd				;					// ���� ���õ� hWnd;


	std::vector<ROOMDATA*>		vtRoomList;


	CNetworkDlg();
	~CNetworkDlg();

	VOID			Create();					// ���̾�α׸� �����.
	INT				Destroy();
	VOID			ChageDlg();
	INT				SetWindowPosition(HWND);

	INT				Client_Lobby_Show();							// �κ� ���̰� �Ѵ�.
	INT				CLIENT_LOBBY_ROOM_INSERT(PACKETDATA* pData);	// �渮��Ʈ�� �߰��Ѵ�.
	INT				CLIENT_LOBBY_CLEAR();							// �渮��Ʈ �ʱ�ȭ�Ѵ�.
	INT				CLIENT_LOBBY_JOIN(int RoomIndex);				// �濡 �����ϱ�
	INT				CLIENT_ROOM_SHOW();								// ��(�ɸ��ͼ���) ȭ������ �Ѿ��.
	
	
	
	
	
	static BOOL CALLBACK DlgWaitProc		(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgConnectionProc	(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgLoginProc		(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgLobbyProc		(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgJoinProc		(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgCreateRoomProc	(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	
};