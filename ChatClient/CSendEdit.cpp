#include "pch.h"
#include "Chat.h"
#include "CSendEdit.h"

IMPLEMENT_DYNAMIC(CSendEdit, CEdit)

CSendEdit::CSendEdit()
{

}

CSendEdit::~CSendEdit()
{
}

BEGIN_MESSAGE_MAP(CSendEdit, CEdit)
END_MESSAGE_MAP()


// CSendEdit message handlers

BOOL CSendEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		::SendMessage(GetParent()->GetDlgItem(IDC_BUTTON_SEND)->m_hWnd, BM_CLICK, 0, 0);
		//::SendMessage(GetParent()->m_hWnd, WM_COMMAND, MAKELONG(IDC_BUTTON_SEND, BN_CLICKED), (LPARAM)GetParent()->GetDlgItem(IDC_BUTTON_SEND)->m_hWnd);
	}

	return CEdit::PreTranslateMessage(pMsg);
}
