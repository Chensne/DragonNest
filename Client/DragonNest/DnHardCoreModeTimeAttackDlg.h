#pragma once
#include "DnCustomDlg.h"


class CDnItem;

class CDnHardCoreModeTimeAttackDlg : public CDnCustomDlg
{
public:
	CDnHardCoreModeTimeAttackDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnHardCoreModeTimeAttackDlg();

protected:
	CEtUIStatic*		m_pStaticMinute;
	CEtUIStatic*		m_pStaticSecond;
	CEtUIStatic*		m_pStaticMilliSecond;
	CEtUITextureControl*	m_pBaseTexture;
	CEtUITextureControl*	m_pScaleTexture;

	// Timer Setting Value
	int			m_nMin;	
	int			m_nSec;
	__time64_t	m_tAxisTime;	// 시간
	float		m_tMSec;		// 밀리세컨드
	float		m_fRemainSec;
	float		m_fOriginSec;
	float		m_fTotalSec;
	float		m_fHurryUpSec;
	bool		m_bStartTimer;

	// Effect Setting Value
	bool	m_bContinue;
	bool	m_bScaleProcess;
	float	m_fScaleAfterShowTime;
	float	m_fScaleAfterShowElapsedTime;
	float	m_fScaleShowTime;
	float	m_fScaleTime;
	float	m_fScaleAccel;
	float	m_fScaleInit;
	float	m_fHalfTime;
	float	m_fHalfShowTime;

protected:
	void UpdateTimer( bool bShowTimer );
	void ProcessTimerEffect( float fElapsedTime );
	void GetUICtrlCenter( EtVector2& result, CEtUIControl* ctrl, float scale );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );

public:
	void SetRemainTime( int nOriginTime, int nRemainTime );
	void SetHurryTime( int nTime );
	void ResetTimer();
};

