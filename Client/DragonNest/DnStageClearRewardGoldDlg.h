#pragma once
#include "DnCustomDlg.h"
#include "DnDamageCount.h"

class CDnStageClearRewardGoldDlg : public CDnCustomDlg
{
public:
	CDnStageClearRewardGoldDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_TOP, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnStageClearRewardGoldDlg();

protected:
	int m_nRewardGold;

	CDnDamageCount *m_pGold;
	CDnDamageCount *m_pSilver;
	CDnDamageCount *m_pCopper;

	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticCopper;

	float m_fCountDelta;

protected:
	void RefreshValue( int nValue );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

	void Set( int nValue );
};