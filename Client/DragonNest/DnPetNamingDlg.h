#pragma once

#include "DnCustomDlg.h"


class CDnPetNamingDlg : public CDnCustomDlg
{
public:
	CDnPetNamingDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPetNamingDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void SetChangeItemSerial(INT64 serial) { m_ChangeItemSerial = serial; }
	void SetPetSerial( INT64 iPetSerial ) { m_iPetSerial = iPetSerial; }
	void SetCallGestureQuickSlot( bool bSetGestureQuickSlot ) { m_bSetGestureQuickSlot = bSetGestureQuickSlot; }

protected:
	CEtUIIMEEditBox*	m_pEditBoxPetName;
	CEtUIButton*		m_pButtonOK;
	INT64				m_ChangeItemSerial;
	INT64				m_iPetSerial;
	bool				m_bSetGestureQuickSlot;
};

