#pragma once

#ifdef PRE_ADD_CASHSHOP_RENEWAL

#include "DnCustomDlg.h"


class CDnCashShopMenuListDlg : public CDnCustomDlg
{
private:

	CDnMenuList * m_pMenuList;


public:
	CDnCashShopMenuListDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopMenuListDlg();
	
public:

	virtual void Initialize(bool bShow);
	//virtual void InitialUpdate();
	virtual void InitCustomControl(CEtUIControl *pControl);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void Process(float fElapsedTime);	
	virtual void Render( float fElapsedTime );
	virtual void Show(bool bShow);

public:
	
	void ResetMenuList();
	
	CDnMenuListItem * ShowSaleCouponTab( bool bShow, const std::wstring & strName, bool bSelect );
	bool ShowSaleCouponSubCategory( const std::wstring & strName, int tabID );
	void SelectMainTab();
	void Reset();

};

#endif // PRE_ADD_CASHSHOP_RENEWAL