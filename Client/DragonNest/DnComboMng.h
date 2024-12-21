#pragma once
#include "DnCountMng.h"
#include "DnComboDlg.h"

class CDnComboMng : public CDnCountMng<CDnComboDlg>
{
public:
	CDnComboMng(void);
	virtual ~CDnComboMng(void);

public:
	void SetCombo( int nCombo, int nDelay );
};