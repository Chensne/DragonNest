#pragma once

#ifdef PRE_ADD_CRAZYDUC_UI

#include "DnCountMng.h"
#include "DnScoreDlg.h"

class CDnScoreMng : public CDnCountMng<CDnScoreDlg>
{
public:

	enum EnumScoreColor
	{
		DARK_GRAY = 1,
		YELLOW,
		BLUE,
		RED,
		BLACK,
	};

	CDnScoreMng(void);
	virtual ~CDnScoreMng(void);

protected:
	EtAniHandle m_hUIAni;

public:
	EtAniHandle GetScoreAni() { return m_hUIAni; }

public:
	void SetScore( EtVector3 vPos, int nScore, int nColorType );
};


#endif 