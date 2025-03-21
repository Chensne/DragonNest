#pragma once

#include "EtUIDialog.h"

class CDnClimaxEffect
{
public :
	CDnClimaxEffect();
	virtual ~CDnClimaxEffect();

	void Init( float fScaleShowTime, float fScaleAccel, float fScaleInit, float fScaleAfterShowTime, float fHalfShowTime, bool bContinue );
	void Process( float fElapsedTime );
	bool IsShow();
	void Start();

protected :
	CEtUITextureControl * m_pBaseTexture;
	CEtUITextureControl * m_pScaleTexture;

	EtTextureHandle m_hClimaxTexture;

	bool m_bContinue;
	bool  m_bScaleProcess;
	float m_fScaleAfterShowTime;
	float m_fScaleAfterShowElapsedTime;
	float m_fScaleShowTime;
	float m_fScaleTime;
	float m_fScaleAccel;
	float m_fScaleInit;
	float m_fHalfTime;
	float m_fHalfShowTime;

	void GetUICtrlCenter(EtVector2& result, CEtUIControl* ctrl, float scale);
};