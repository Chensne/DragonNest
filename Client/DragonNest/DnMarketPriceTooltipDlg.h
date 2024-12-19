#pragma once

class CDnItem;

class CDnMarketPriceTooltipDlg : public CEtUIDialog
{
public:
	CDnMarketPriceTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMarketPriceTooltipDlg();

public:
	void OnRecvMarketPrice( int nMarketDBID, std::vector< TMarketPrice > & vecPrice );
	void SetItemName( CDnItem * pItem );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
