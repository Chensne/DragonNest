#pragma once

#if defined(_JPN)

#include "../../DnLauncherDlg.h"


class CDnLauncherDlgJPN : public CDnLauncherDlg
{
public:
	CDnLauncherDlgJPN( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgJPN() {}

public:
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

protected:
	virtual void MakeFont();			// 폰트 생성
	virtual void NavigateHtml();		// 웹페이지 이동

};

#endif // _JPN