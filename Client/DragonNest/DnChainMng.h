#pragma once
#include "DnCountMng.h"
#include "DnChainDlg.h"

class CDnChainMng : public CDnCountMng<CDnChainDlg>
{
public:
	CDnChainMng(void);
	virtual ~CDnChainMng(void);

public:
	void SetChain( int nChain );
};