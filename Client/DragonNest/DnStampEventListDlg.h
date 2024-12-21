#pragma once

#ifdef PRE_ADD_STAMPSYSTEM

#include "DnCustomDlg.h"

struct STableStampData;

class CDnStampEventListItem : public CDnCustomDlg
{
public:
	CDnStampEventListItem( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnStampEventListItem();

protected:
	//int m_nEventID;
#define AWEEK 7
	CEtUIStatic *m_pStaticCheck[AWEEK];
	CEtUIStatic *m_pStaticDayCheck[AWEEK];
	CDnItemSlotButton *m_pItemSlotReward[2];
	CEtUICheckBox *m_pCheckBox;
	
	STableStampData * m_pStampData;
	
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	//virtual void Process( float fElapsedTime );


private:
	void InputContent();

public:
	
	void SetStamp( int idx, bool b );
	void SetListItem( STableStampData * pData );


	void ChangeWeekDay( int wDay );

};


#endif // PRE_ADD_STAMPSYSTEM