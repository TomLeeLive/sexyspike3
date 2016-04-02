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
	int				m_State				;					// 다이얼로그 플래그
	HWND			m_hWait				;					// Wait메세지
	HWND			m_hConnection		;					// 서버연결
	HWND			m_hLogin			;					// 로그인
	HWND			m_hLobby			;					// 로비
	HWND			m_hJoin				;					// 캐릭터
	HWND			m_hCreateRoom		;					// 방만들기
	HWND*			m_phWnd				;					// 현재 선택된 hWnd;


	std::vector<ROOMDATA*>		vtRoomList;


	CNetworkDlg();
	~CNetworkDlg();

	VOID			Create();					// 다이얼로그를 만든다.
	INT				Destroy();
	VOID			ChageDlg();
	INT				SetWindowPosition(HWND);

	INT				Client_Lobby_Show();							// 로비를 보이게 한다.
	INT				CLIENT_LOBBY_ROOM_INSERT(PACKETDATA* pData);	// 방리스트를 추가한다.
	INT				CLIENT_LOBBY_CLEAR();							// 방리스트 초기화한다.
	INT				CLIENT_LOBBY_JOIN(int RoomIndex);				// 방에 접속하기
	INT				CLIENT_ROOM_SHOW();								// 룸(케릭터선택) 화면으로 넘어간다.
	
	
	
	
	
	static BOOL CALLBACK DlgWaitProc		(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgConnectionProc	(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgLoginProc		(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgLobbyProc		(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgJoinProc		(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static BOOL CALLBACK DlgCreateRoomProc	(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	
};