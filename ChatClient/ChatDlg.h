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

	// ׷����Ϣ
	void AppendMsg(int nFromId, char* szMsg);

protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTCPServer m_TCPServer;			// TCP����
	int m_nCurId;					// ��ǰ�û�ID
	CString m_strCurName;			// ��ǰ�û�����
	map<int, CString> m_mapMessage;	// ÿ���û���Ϣӳ���
	int m_nSelId;					// ��ǰѡ�е��û�ID
	set<int> m_set;					// ��δ�鿴��Ϣ���û�ID����

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