// CLoginDlg.cpp : implementation file
//

#include "pch.h"
#include "Chat.h"
#include "CLoginDlg.h"
#include "afxdialogex.h"
#include "ChatDlg.h"


// CLoginDlg dialog

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
	, m_strUsername(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUsername);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CLoginDlg::OnBnClickedButtonLogin)
END_MESSAGE_MAP()


// CLoginDlg message handlers


void CLoginDlg::OnBnClickedButtonLogin()
{
	UpdateData(TRUE);

	EndDialog(0);
}
