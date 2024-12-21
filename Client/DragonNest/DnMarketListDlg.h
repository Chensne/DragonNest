#pragma once
#include "DnCustomDlg.h"

class CDnItem;
class CDnMarketListDlg :
	public CDnCustomDlg
{
public:
	CDnMarketListDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnMarketListDlg(void);

public:
	void	Initialize(bool bShow) override;
	virtual void	Show(bool bShow) override;

protected:
	void	InitialUpdate() override;
	void	InitCustomControl(CEtUIControl *pControl) override;
	void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 ) override;
	void  Process( float fElapsedTime ) override;
	
protected:
	CDnItem		*m_pItem;
	int			m_nMarketDBID;
	MONEY_TYPE		m_nMoney;
	int				m_nOnePrice;
	bool			m_bPetalTrade;
	bool			m_bRemainTime;
	CDnItemSlotButton		*m_pItemSlotButton;

	typedef	CDnCustomDlg		BaseClass;

	static std::string m_sUIFileName;

public:	
	void ProcessAlpha( float fElapsedTime );
	void SetProperty( TMyMarketInfo &Info, float fAlpha );
	void SetProperty( TMarketCalculationInfo &Info, float fAlpha );
	int GetMarketDBID() { return m_nMarketDBID;}	
	MONEY_TYPE GetMoney() { return m_nMoney; }
	wchar_t* GetItemName();
	int GetItemSound();
	void SetPetalUI();
	bool GetRemainTime()	{ return m_bRemainTime; }	// true : 시간 남음, false : 기간 만료
	
	static void SetUIFileName( std::string sUIFileName )	{ m_sUIFileName = sUIFileName; }
};
