#pragma once
#include <map>
#include <set>
#include "CTCPServer.h"
#include "CSendEdit.h"
using namespace std;

class CChatDlg : public CDialogEx
{
public:
	CChatDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

private:
	void OnSetUserList(char* pData);
	void OnAddUser(char* pData);
	void OnPrivateChat(char* pData);
	void OnBroadcast(char* pData);
	void OnDeleteUser(char* pData);

	// 追加消息
	void AppendMsg(int nFromId, char* szMsg);

protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTCPServer m_TCPServer;			// TCP连接
	int m_nCurId;					// 当前用户ID
	CString m_strCurName;			// 当前用户姓名
	map<int, CString> m_mapMessage;	// 每个用户信息映射表
	int m_nSelId;					// 当前选中的用户ID
	set<int> m_set;					// 有未查看消息的用户ID集合

	CListCtrl m_ContactList;
	CString m_strSend;
	CSendEdit m_SendEdit;
	CString m_strMessage;
	CString m_strSelName;
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnClickListContact(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIdok();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnClose();
	afx_msg void OnCustomdrawListContact(NMHDR* pNMHDR, LRESULT* pResult);
};