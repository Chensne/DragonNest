#ifdef PRE_ADD_CASHSHOP_ACTOZ

#include "DnCustomDlg.h"


// #72888 액토즈이전 - 캐시샵UI 변경.
class CDnCashShopConfirmRefundDlg : public CDnCustomDlg
{

private:

	CDnItemSlotButton * m_pSlotBtn;
	CEtUIStatic * m_pStaticName;


public:


	CDnCashShopConfirmRefundDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnCashShopConfirmRefundDlg();


	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();		
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );


private:

	// 캐시인벤으로 이동요청.
	void MoveCashInventory();

public:

	// 캐시인벤으로 이동하기전 확인창.
	void SetItem( CDnItem * pItem );

};



#endif // PRE_ADD_CASHSHOP_ACTOZ