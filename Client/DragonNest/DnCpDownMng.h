
#pragma once

#include "DnCountMng.h"
#include "DnCpDownDlg.h"

class CDnCpDownMng : public CDnCountMng<CDnCpDownDlg>
{
public:
	CDnCpDownMng(void);
	virtual ~CDnCpDownMng(void);

 	void				SetCpDown(int point);

private:
	CDnCpDownDlg*		m_CurCountDlg;
};