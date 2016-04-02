#include "../Include/INCLUDE.h"
CNetworkDlg* g_pNetworkDlg;

CNetworkDlg::CNetworkDlg()
{
	g_pNetworkDlg = this;
	m_State = DIALOG_HIDE;
	m_hWait			= NULL;
	m_hConnection	= NULL;
	m_hLogin		= NULL;
	m_hLobby		= NULL;
	m_hJoin			= NULL;
	m_hCreateRoom	= NULL;

}

CNetworkDlg::~CNetworkDlg()
{
	Destroy();
}
VOID CNetworkDlg::Create()
{
	m_phWnd = NULL;
	m_hInst	= (HINSTANCE)GetModuleHandle(NULL);
	// ���̾�α׸� �����.
	
	m_hWait			= CreateDialog(m_hInst,MAKEINTRESOURCE(IDD_WAIT),		GHWND,DlgWaitProc		);
	m_hConnection	= CreateDialog(m_hInst,MAKEINTRESOURCE(IDD_CONNECTION),	GHWND,DlgConnectionProc);
	m_hLogin		= CreateDialog(m_hInst,MAKEINTRESOURCE(IDD_LOGIN),		GHWND,DlgLoginProc		);
	m_hLobby		= CreateDialog(m_hInst,MAKEINTRESOURCE(IDD_LOBBY),		GHWND,DlgLobbyProc		);
	m_hJoin			= CreateDialog(m_hInst,MAKEINTRESOURCE(IDD_JOIN),		GHWND,DlgJoinProc		);
	m_hCreateRoom	= CreateDialog(m_hInst,MAKEINTRESOURCE(IDD_CREATEROOM),	GHWND,DlgCreateRoomProc);

	//ShowWindow(m_hConnection,SW_SHOW);


}
INT	CNetworkDlg::Destroy()
{


	return S_OK;
}
VOID CNetworkDlg::ChageDlg()
{
	// ���̾�α� �ٲٱ�
}

INT	CNetworkDlg::SetWindowPosition(HWND hWnd)
{
			
	RECT	hWndRect;
	
	int x,y;
	GetWindowRect(hWnd,&hWndRect);

	x = ((GMAIN->m_rcWindowClient.right - GMAIN->m_rcWindowClient.left) / 2) - ((hWndRect.right - hWndRect.left)/2);
	y = ((GMAIN->m_rcWindowClient.bottom - GMAIN->m_rcWindowClient.top) / 2) - ((hWndRect.bottom - hWndRect.top)/2);
	MoveWindow(hWnd,x,y,hWndRect.right - hWndRect.left,hWndRect.bottom - hWndRect.top,FALSE);
	
	return S_OK;
}
INT	CNetworkDlg::Client_Lobby_Show()
{
	// �κ� �켱 ����.
	CLIENT_LOBBY_CLEAR();
	int		retval = 0;
	ShowWindow(g_pNetworkDlg->m_hWait,SW_HIDE);
	ShowWindow(g_pNetworkDlg->m_hConnection,SW_HIDE);
	ShowWindow(g_pNetworkDlg->m_hLobby,SW_SHOW);

	// �ڽ��� ĳ���� �̸��� ������Ʈ�Ѵ�.
	g_pNetAdapter->SendPacket(NULL
							,CLIENT_LOBBY
							,CLIENT_CONNECT_UPDATE
							,NULL
							,g_pApp->m_MultiPlay->sName
							,CLIENT_LOBBY_ROOM_LIST
							,NULL
							,NULL
							,NULL);
	
	return S_OK;
}



INT CNetworkDlg::CLIENT_LOBBY_ROOM_INSERT(PACKETDATA* pPacket)
{
	
	// �渮��Ʈ(����)�� ���Ϳ� �־ �����Ѵ�.
	ROOMDATA*	tempData = (ROOMDATA*)pPacket->DATA;
	vtRoomList.push_back(new ROOMDATA(	tempData->Index
										,tempData->State
										,tempData->Number
										,tempData->MAX_NUMBER
										,tempData->Room_Name
										,tempData->Room_PW));

	return S_OK;
}


INT	CNetworkDlg::CLIENT_LOBBY_CLEAR()
{

	// ����Ʈ�� �ʱ�ȭ �����ش�.
	ListView_DeleteAllItems(GetDlgItem(m_hLobby,IDC_LV_ROOMLIST));

	return S_OK;
}

INT	CNetworkDlg::CLIENT_LOBBY_JOIN(int RoomIndex)
{
	
	// ������ �� ���� ��û�� �Ѵ�.
	g_pNetAdapter->SendPacket(NULL,CLIENT_LOBBY,CLIENT_LOBBY_ROOM_JOIN,RoomIndex,NULL,NULL,NULL,NULL,NULL);

	return S_OK;
}


INT CNetworkDlg::CLIENT_ROOM_SHOW()
{
	
	ShowWindow(m_hWait,SW_HIDE);
	ShowWindow(m_hLobby,SW_HIDE);
	GMAIN->m_MultiPlay->m_MyInfo.State = CLIENT_ROOM;

	return S_OK;
}
BOOL CALLBACK CNetworkDlg::DlgWaitProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
			g_pNetworkDlg->SetWindowPosition(hWnd);
			return FALSE;
		case WM_COMMAND:
			switch(wParam)
			{
				default:
					break;
			}
			return FALSE;
		case WM_CLOSE:
			ShowWindow(hWnd,SW_HIDE);
			ShowWindow(g_pNetworkDlg->m_hConnection,SW_SHOW);
			g_pNetAdapter->ClientStop();
			return FALSE;
		default:
			return FALSE;
	}
	
	return FALSE;
}
BOOL CALLBACK CNetworkDlg::DlgConnectionProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

	DWORD	IP;
	char	IPAdress[15];

	switch(msg)
	{
		case WM_INITDIALOG:
			
			g_pNetworkDlg->SetWindowPosition(hWnd);
			SetFocus(GetDlgItem(hWnd,IDC_CB_CHARNAME));
			return FALSE;
		case WM_COMMAND:
			switch(wParam)
			{
				case IDC_CB_CONNECT:
					
					if(g_NetworkState == CLIENT_CONNECT){	break;	}
					
					if(GetWindowTextLength(GetDlgItem(hWnd,IDC_CB_CHARNAME)) == 0)
					{	MessageBox(hWnd,"ĳ���� �̸��� �Է��ϼ���","Error",0);
						SetFocus(GetDlgItem(hWnd,IDC_CB_CHARNAME));
						break;
					}

					if(GetWindowTextLength(GetDlgItem(hWnd,IDC_CB_CHARNAME)) > PACKET_NAME)
					{
						MessageBox(hWnd,"ĳ���� �̸��� �ʹ� ��ϴ�.","Error",0);
						SetFocus(GetDlgItem(hWnd,IDC_CB_CHARNAME));
						break;
					}
					
					// ��Ƽ�÷��̾� ������ �ڽ��� �̸��� �ִ´�.
					GetWindowText(GetDlgItem(hWnd,IDC_CB_CHARNAME),GMAIN->m_MultiPlay->sName,PACKET_NAME);

					ShowWindow(g_pNetworkDlg->m_hWait,SW_SHOW);
					SendDlgItemMessage(hWnd,IDC_IP_ADDRESS,IPM_GETADDRESS,NULL,(LPARAM)&IP);
					
					sprintf(IPAdress,"%d.%d.%d.%d",FIRST_IPADDRESS(IP),SECOND_IPADDRESS(IP),THIRD_IPADDRESS(IP),FOURTH_IPADDRESS(IP));
					g_pNetAdapter->SetSecretkey(2015);
					g_pNetAdapter->ClientStart(IPAdress);
					break;
			}
			return FALSE;
		case WM_CLOSE:
			return FALSE;
		default:
			return FALSE;
	}
	

	return FALSE;
}
BOOL CALLBACK CNetworkDlg::DlgLoginProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
			g_pNetworkDlg->SetWindowPosition(hWnd);
			return FALSE;
		case WM_COMMAND:
			switch(wParam)
			{
				default:
					break;
			}
			return FALSE;
		case WM_CLOSE:
			return FALSE;
		default:
			return FALSE;
	}

	return FALSE;
}
BOOL CALLBACK CNetworkDlg::DlgLobbyProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	LVCOLUMN	COL;
	
	switch(msg)
	{	
		case WM_CREATE:
		case WM_INITDIALOG:
		{	
			
			HWND tempHwnd = GetDlgItem(hWnd,IDC_LV_ROOMLIST);
			COL.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			COL.fmt = LVCFMT_LEFT;

			COL.cx = 50;
			COL.pszText = "��ȣ";
			COL.iSubItem = 0;
			ListView_InsertColumn(tempHwnd,0,&COL);

			COL.cx = 50;
			COL.pszText = "����";
			COL.iSubItem = 1;
			ListView_InsertColumn(tempHwnd,1,&COL);

			COL.cx = 100;
			COL.pszText = "�ο�";
			COL.iSubItem = 2;
			ListView_InsertColumn(tempHwnd,2,&COL);


			COL.cx = 250;
			COL.pszText = "�� ����";
			COL.iSubItem = 3;
			ListView_InsertColumn(tempHwnd,3,&COL);

			ListView_SetExtendedListViewStyle(tempHwnd,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

			g_pNetworkDlg->SetWindowPosition(hWnd);
			return 0;
		}
		case WM_COMMAND:
			switch(wParam)
			{
				case IDC_CB_LIST_AFRESH:
				{
					g_pNetworkDlg->CLIENT_LOBBY_CLEAR();
					// �κ� ����� �ٽ� �Ѹ���.
					LVITEM LI;
					char temp[10]={0};
					int i;

					LI.mask = LVIF_TEXT;

					char tt[10]={0};
					sprintf(tt,"%d",g_pNetworkDlg->vtRoomList.size());
					SetWindowText(hWnd,tt);
					for(i=0;i<g_pNetworkDlg->vtRoomList.size();++i)
					{
						std::vector<ROOMDATA*>::reference it = g_pNetworkDlg->vtRoomList[i]; 

						// ��ȣ
						memset(temp,0,sizeof(temp));
						sprintf(temp,"%d",it->Index);
						LI.iItem = it->Index;
						LI.iSubItem = 0;
						LI.pszText = temp;
						ListView_InsertItem(GetDlgItem(g_pNetworkDlg->m_hLobby,IDC_LV_ROOMLIST),&LI);

						// ����
						memset(temp,0,sizeof(temp));
						sprintf(temp,"%d",it->State);
						LI.iItem = it->Index;
						LI.iSubItem = 1;
						LI.pszText = temp;
						ListView_SetItem(GetDlgItem(g_pNetworkDlg->m_hLobby,IDC_LV_ROOMLIST),&LI);

						// ����
						LI.iItem = it->Index;
						LI.iSubItem = 2;
						LI.pszText = it->Room_Name;
						ListView_SetItem(GetDlgItem(g_pNetworkDlg->m_hLobby,IDC_LV_ROOMLIST),&LI);

						// �ο�
						memset(temp,0,sizeof(temp));
						sprintf(temp,"%d/%d",it->Number,it->MAX_NUMBER);
						LI.iItem = it->Index;
						LI.iSubItem = 3;
						LI.pszText = temp;
						ListView_SetItem(GetDlgItem(g_pNetworkDlg->m_hLobby,IDC_LV_ROOMLIST),&LI);

					}
					return 0;
				}
				case IDC_CB_CREATEROOM:
					ShowWindow(hWnd,SW_HIDE);
					ShowWindow(g_pNetworkDlg->m_hCreateRoom,SW_SHOW);
					return 0;
				case IDC_CB_JOIN:
					int RoomIndx;
					RoomIndx = ListView_GetNextItem(GetDlgItem(hWnd,IDC_LV_ROOMLIST),-1,LVNI_SELECTED);
					if(RoomIndx!=-1)
					{
						// �����
						//ListView_GetItem
						g_pNetworkDlg->CLIENT_LOBBY_JOIN(g_pNetworkDlg->vtRoomList[RoomIndx]->Index);
					}

					return 0;
				default:
					return 0;
			}
			return 0;
		case WM_LBUTTONDOWN:
			int RoomIndx;
			RoomIndx = ListView_GetNextItem(GetDlgItem(hWnd,IDC_LV_ROOMLIST),-1,LVNI_SELECTED);
			if(RoomIndx!=-1)
			{
				// �����
				//ListView_GetItem
				g_pNetworkDlg->CLIENT_LOBBY_JOIN(RoomIndx);
			}

			return 0;
		case WM_CLOSE:
			ShowWindow(hWnd,SW_HIDE);
			ShowWindow(g_pNetworkDlg->m_hConnection,SW_SHOW);
			g_pNetAdapter->ClientStop();
			g_pNetworkDlg->vtRoomList.clear();
			return 0;
		default:
			return 0;
	}

	return FALSE;
}
BOOL CALLBACK CNetworkDlg::DlgJoinProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
			g_pNetworkDlg->SetWindowPosition(hWnd);
			return FALSE;
		case WM_COMMAND:
			switch(wParam)
			{
				default:
					break;
			}
			return FALSE;
		case WM_CLOSE:
			return FALSE;
		default:
			return FALSE;
	}

	return FALSE;
}
BOOL CALLBACK CNetworkDlg::DlgCreateRoomProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	// ��Ŷ��
	BYTE sBuf[PACKET_TOTAL]={0};
	switch(msg)
	{
		case WM_INITDIALOG:
			g_pNetworkDlg->SetWindowPosition(hWnd);
			return FALSE;
		case WM_COMMAND:
			switch(wParam)
			{
				case IDC_CB_ROOMCREATE:
					{
					if( GetWindowTextLength( GetDlgItem(hWnd,IDC_CB_ROOMNAME)) == 0	)
					{
						MessageBox(NULL,"������ �Է��ϼ���","Error",0);
						SetFocus(GetDlgItem(hWnd,IDC_CB_ROOMNAME));
						break;
					}

					// ��Ŷ�� ���� ������ ������ �Ѵ�.
					ShowWindow(g_pNetworkDlg->m_hWait,SW_SHOW);
					ShowWindow(hWnd,SW_HIDE);

					
					// �� ���������� ��Ƽ� ������ ������.
					ROOMDATA	temp;
					memset(&temp,0,sizeof(temp));
					temp.Index = 0;
					temp.MAX_NUMBER  = MAX_ROOM_MEMBER;
					temp.Number = 0;
					temp.State = ROOM_STATE_READY;


					//������ �����´�.
					GetWindowText(GetDlgItem(hWnd,IDC_CB_ROOMNAME),(char*)temp.Room_Name,ROOM_NAME_SIZE);
					if(GetWindowTextLength( GetDlgItem(hWnd,IDC_CB_ROOMPW)) != 0)
					{
						GetWindowText(GetDlgItem(hWnd,IDC_CB_ROOMPW),(char*)temp.Room_PW,ROOM_PW_SIZE);
					}
					
					// ��Ŷ�� �����...������.
					g_pNetAdapter->SendPacket(NULL,CLIENT_LOBBY,CLIENT_LOBBY_ROOM_CREATE,NULL,NULL,NULL,NULL,sizeof(ROOMDATA),&temp);
					
					
					break;
					}
				default:
					break;
				
			}
			return FALSE;
		case WM_CLOSE:
			return FALSE;
		default:
			return FALSE;
	}

	return FALSE;
}