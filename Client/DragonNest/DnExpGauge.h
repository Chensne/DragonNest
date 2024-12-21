#pragma once
#include "EtUIProgressBar.h"
#include "DnCustomControlCommon.h"

class CDnExpGauge : public CEtUIProgressBar
{
public:
	CDnExpGauge(CEtUIDialog *pParent);
	virtual ~CDnExpGauge(void);

protected:
	bool m_bFlipH;	// ÁÂ¿ì¹ÝÀü

public:
	virtual void Render( float fElapsedTime );

	void SetFlipH( bool bFlip = true ) { m_bFlipH = bFlip; }
};
