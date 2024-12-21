#pragma once

#if defined(_USA)

#include "../../DnLauncherDlg.h"


class CDnLauncherDlgUSA : public CDnLauncherDlg
{
public:
	CDnLauncherDlgUSA( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgUSA() {}

public:
	afx_msg void OnDestroy();
	afx_msg void OnClickButtonCancel();
	afx_msg void OnClickButtonGameStart();
	afx_msg void OnClickButtonClose();

	DECLARE_MESSAGE_MAP()

protected:
	virtual void MakeFont();			// 폰트 생성
	virtual BOOL InitWebpage();			// 가이드 웹페이지 초기화
};

#endif // _USA