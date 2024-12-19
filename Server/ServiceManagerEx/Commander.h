#pragma once

class CCommander : public CEdit
{
	DECLARE_DYNAMIC(CCommander)

public:
	CCommander();
	virtual ~CCommander();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	void SetCommand(const wstring& command);
	void OnCommand();
	void OnPrevCommand();
	void OnNextCommand();

private:
	vector<wstring> history;
	size_t index;
};


