#pragma once

#if defined(_EU)

#include "../../DnLauncherDlg.h"


class CDnLauncherDlgEU : public CDnLauncherDlg
{
public:
	CDnLauncherDlgEU( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgEU() {}

protected:
	virtual void MakeFont();		// ��Ʈ ����
};

#endif // _EU