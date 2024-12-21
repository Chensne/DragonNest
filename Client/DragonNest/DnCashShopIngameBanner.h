#pragma once

#ifdef PRE_ADD_CASHSHOP_RENEWAL

#include "DnCustomDlg.h"


class CDnCashShopIngameBanner : public CDnCustomDlg
{

private:

	CDnItemSlotButton * m_pItemSlot;
	CEtUICheckBox * m_pCheckBoxShow;
	CEtUIStatic * m_pStaticName;
	CEtUITextBox * m_pStaticPrice;
	CEtUIStatic * m_pStaticBoard;
	CEtUIStatic * m_pStaticShow;

	wchar_t m_strTemp[256];

	struct SBannerItem
	{
		bool bShow;
		CDnItem * pItem;
		float fShowtime;
		SBannerItem( CDnItem * _item, float time ) : bShow(true), pItem(_item), fShowtime(time){}		
		~SBannerItem(){
			if( pItem )
				delete pItem;
		}
	};
	//std::vector< SBannerItem > m_vItems;
	std::vector< SBannerItem * > m_vItems;	
	int m_crrShowItem;
	float m_AccumulateTime;

public:


public:
	
	CDnCashShopIngameBanner(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	~CDnCashShopIngameBanner();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float dTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	void UpdateBanner( float dTime );

public:

	// Close 아이템갱신하여 배너에 보이지 않게 한다.
	void UpdateIngameBannerItem();

};



#endif // PRE_ADD_CASHSHOP_RENEWAL