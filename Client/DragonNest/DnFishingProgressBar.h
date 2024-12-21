#pragma once
#include "EtUIProgressBar.h"
#include "DnCustomControlCommon.h"

class CDnFishingProgressBar : public CEtUIProgressBar
{
public:
	CDnFishingProgressBar( CEtUIDialog* pParent );
	virtual ~CDnFishingProgressBar();

public:
	virtual void Render( float fElapsedTime );
	void SetSpeedUpSection( float fSpeedUpStart, float fSpeedUpEnd );

private:
	float m_fSpeedUpStart;
	float m_fSpeedUpEnd;
};

