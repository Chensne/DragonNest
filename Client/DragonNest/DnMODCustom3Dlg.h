#pragma once

#ifdef PRE_ADD_CRAZYDUC_UI

#include "DnMODDlgBase.h"

class CDnDamageCount;
class CDnMODCustom3Dlg : public CDnMODDlgBase
{
public:
	CDnMODCustom3Dlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnMODCustom3Dlg();

protected:
	
	float m_fExtraTimeSec;
	float m_fOriginTimeSec;
	float m_fCurTimeSec;
	float m_fMilliSecond;

	__time64_t m_tStandardTimeSec;

	bool		m_bPlay;
	
	CEtUIStatic* m_pStaticSecondTimer;
	CEtUIStatic* m_pStaticMilliSecondTimer;	
	std::vector<CDnDamageCount *> m_pVecStaticCounter;

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

public:

	void Play( bool bPlay ) { m_bPlay = bPlay; }
	void SetTimer( int nSecond, int nMilliSecond );
	void RefreshTimer();
	void ResetTimer();
	
	void TimeChecker();
};

#endif 
