#pragma once

#if defined(_SG)

#include "../../DnLauncherDlg.h"


class CDnLauncherDlgSG : public CDnLauncherDlg
{
public:
	CDnLauncherDlgSG( CWnd* pParent = NULL );
	virtual ~CDnLauncherDlgSG() {}

public:
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

protected:
	virtual void MakeFont();			// ��Ʈ ����
};

#endif // _SG