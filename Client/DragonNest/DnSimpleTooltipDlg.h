#pragma once
#include "EtUIDialog.h"

class CEtUITextBox;

class CDnSimpleTooltipDlg : public CEtUIDialog
{
public:
	CDnSimpleTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnSimpleTooltipDlg(void);

protected:
	CEtUIControl *m_pTooltipCtl;
	CEtUITextBox *m_pTextBox;
	float	m_fDelayTime;
	bool	m_bTopPosition;
	bool	m_bBottomPosition;
	bool    m_bRefreshControlTooltip;

	float	m_fDeltaPositionX;
	float	m_fDeltaPositionY;
	float   m_fOriginalWidth;
	float   m_fRefreshTime;

	// TooltipInfo
	DWORD	m_TextColor;
	bool	m_bPermanent;

protected:
	void UpdateTooltipDlg();
	void UpdateTooltipText();

public:
	virtual void SetTooltipControl( CEtUIControl *pTooltipCtl ) { m_pTooltipCtl = pTooltipCtl; }
	void SetTooltipInfo( DWORD color = textcolor::WHITE, bool bPermanent = true );
	void ClearTooltipInfo();
	CEtUIControl* GetTooltipCtrl() const;
	void SetTopPosition( bool bSetTopPosition ) { m_bTopPosition = bSetTopPosition; }
	void SetBottomPosition( bool bSetBottomPosition ) { m_bBottomPosition = bSetBottomPosition; }
	void SetDeltaPosition( const float fX, const float fY );

	void SetPermanent( bool bPermanent );
	void SetDelayTime( float fDelayTime );

	void ApplyTooltipInfo();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};
