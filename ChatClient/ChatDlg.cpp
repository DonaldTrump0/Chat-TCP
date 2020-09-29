#include "pch.h"
#include "framework.h"
#include "Chat.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include "CLoginDlg.h"
#include "../ChatServer/Packet.h"
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CChatDlg::CChatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHAT_DIALOG, pParent)
	, m_nCurId(0)
	, m_strCurName("")
	, m_strSend(_T(""))
	, m_strMessage(_T(""))
	, m_strSelName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONTACT, m_ContactList);
	DDX_Text(pDX, IDC_EDIT_SEND, m_strSend);
	DDX_Control(pDX, IDC_EDIT_SEND, m_SendEdit);
	DDX_Text(pDX, IDC_EDIT_MESSAGE, m_strMessage);
	DDX_Text(pDX, IDC_EDIT_CONTACT, m_strSelName);
}

void CChatDlg::OnSetUserList(char* pData)
{
	UserInfoListPacket* pUserInfoListPacket = (UserInfoListPacket*)pData;
	m_nCurId = pUserInfoListPacket->m_snCurId;
	int nUserCnt = pUserInfoListPacket->m_snCnt;
	UserInfo* pUserInfo = pUserInfoListPacket->m_aryUserInfo;

	m_ContactList.InsertItem(0, "群聊");
	m_ContactList.SetItemData(0, 0);

	for (int i = 1; i <= nUserCnt; i++)
	{
		m_ContactList.InsertItem(i, pUserInfo->m_szName);
		m_ContactList.SetItemData(i, pUserInfo->m_snId);
		pUserInfo = pUserInfo->Next();
	}
}

void CChatDlg::OnAddUser(char* pData)
{
	UserInfoPacket* pUserInfoPacket = (UserInfoPacket*)pData;
	int nUserCnt = m_ContactList.GetItemCount();
	m_ContactList.InsertItem(nUserCnt, pUserInfoPacket->m_szName);
	m_ContactList.SetItemData(nUserCnt, pUserInfoPacket->m_snId);
}

void CChatDlg::OnPrivateChat(char* pData)
{
	UserMsgPacket* pUserMsgPacket = (UserMsgPacket*)pData;
	int nFromId = pUserMsgPacket->m_snFromId;
	char* szMsg = pUserMsgPacket->m_szMsg;

	AppendMsg(nFromId, szMsg);
}

void CChatDlg::OnBroadcast(char* pData)
{
	UserMsgPacket* pUserMsgPacket = (UserMsgPacket*)pData;
	char* szMsg = pUserMsgPacket->m_szMsg;

	AppendMsg(0, szMsg);
}

void CChatDlg::OnDeleteUser(char* pData)
{
	UserIdPacket* pUserIdPacket = (UserIdPacket*)pData;
	int nId = pUserIdPacket->m_snId;

	for (int i = 0; i < m_ContactList.GetItemCount(); i++)
	{
		if (m_ContactList.GetItemData(i) == nId)
		{
			m_ContactList.DeleteItem(i);
			m_mapMessage.erase(nId);
			break;
		}
	}
}

void CChatDlg::AppendMsg(int nFromId, char* szMsg)
{	
	// 追加到消息映射表
	m_mapMessage[nFromId] += szMsg;

	// 如果是当前选中的用户发来的消息，则更新消息框
	if (nFromId == m_nSelId)
	{
		m_strMessage = m_mapMessage[nFromId];
		UpdateData(FALSE);
	}
	else
	{
		// 插入到有未查看消息的用户集合
		m_set.insert(nFromId);
		m_ContactList.RedrawWindow();
	}
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatDlg::OnBnClickedButtonSend)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_CLICK, IDC_LIST_CONTACT, &CChatDlg::OnClickListContact)
	ON_COMMAND(IDOK, &CChatDlg::OnIdok)
	ON_WM_COPYDATA()
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_CONTACT, &CChatDlg::OnCustomdrawListContact)
END_MESSAGE_MAP()

// CChatDlg message handlers

BOOL CChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application'm_TCPServer main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	// 用户登录
	CLoginDlg loginDlg;
	loginDlg.DoModal();
	m_strCurName = loginDlg.m_strUsername;

	LoginPacket loginPacket(S_CMD_LOGIN, m_strCurName.GetString());
	// 向服务端发送登录信息
	m_TCPServer.Send((char*)&loginPacket, loginPacket.Length());

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CChatDlg::OnBnClickedButtonSend()
{
	UpdateData(TRUE);

	int nItem = m_ContactList.GetSelectionMark();
	int nToId = m_ContactList.GetItemData(nItem);
	CString strToName = m_ContactList.GetItemText(nItem, 0);

	// 显示
	CString strSendMsg = m_strCurName + ": " + m_strSend + "\r\n";
	m_strMessage += strSendMsg;
	m_strSend = "";
	m_mapMessage[nToId] = m_strMessage;

	ChatPacket chatPacket(S_CMD_CHAT, m_nCurId, nToId, strSendMsg.GetString());
	// 发送给服务器
	m_TCPServer.Send((char*)&chatPacket, chatPacket.Length());

	UpdateData(FALSE);
}


void CChatDlg::OnClickListContact(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int nItem = pNMItemActivate->iItem;
	if (nItem >= 0)
	{
		m_nSelId = m_ContactList.GetItemData(nItem);
		m_strSelName = m_ContactList.GetItemText(nItem, 0);
		m_strMessage = m_mapMessage[m_nSelId];

		// 去除选中用户的红色标记
		m_set.erase(m_nSelId);
		m_ContactList.RedrawWindow();

		UpdateData(FALSE);
	}
}


void CChatDlg::OnIdok()
{
}

// 处理服务器发来的消息
BOOL CChatDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	char* pData = (char*)pCopyDataStruct->lpData;
	PacketHeader* pPacketHeader = (PacketHeader*)pData;

	switch (pPacketHeader->m_snCmd)
	{
	case C_CMD_SET_USER_LIST:
		OnSetUserList(pData);
		break;
	case C_CMD_ADD_USER:
		OnAddUser(pData);
		break;
	case C_CMD_PRIVATE_CHAT:
		OnPrivateChat(pData);
		break;
	case C_CMD_BROADCAST:
		OnBroadcast(pData);
		break;
	case C_CMD_DELETE_USER:
		OnDeleteUser(pData);
		break;
	default:
		break;
	}

	//int nPos = strData.Find(':', 0);
	//CString strName = strData.Left(nPos);
	//m_mapMessage[strName] += strData;

	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


void CChatDlg::OnClose()
{
	UserIdPacket userIdPacket(S_CMD_LOGOUT, m_nCurId);

	m_TCPServer.Send((char*)&userIdPacket, userIdPacket.Length());

	CDialogEx::OnClose();
}


void CChatDlg::OnCustomdrawListContact(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	*pResult = 0;

	switch (pNMCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
	{
		if (m_set.find(pNMCD->nmcd.lItemlParam) != m_set.end())
		{
			pNMCD->clrText = RGB(255, 0, 0);
		}
		break;
	}
	default:
		break;
	}
}
