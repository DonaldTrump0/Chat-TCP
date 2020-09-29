#pragma once

class CSendEdit : public CEdit
{
	DECLARE_DYNAMIC(CSendEdit)

public:
	CSendEdit();
	virtual ~CSendEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


