#pragma once

#if defined(_EU)

#include "../../DnLauncherDlg.h"


class CDnLauncherDlgEU : public CDnLauncherDlg
{
public:
	CDnLauncherDlgEU( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgEU() {}

protected:
	virtual void MakeFont();		// 폰트 생성
};

#endif // _EU