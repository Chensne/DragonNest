#include "StdAfx.h"
#include "DnAppellationRewardCoinTooltipDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL

CDnAppellationRewardCoinTooltipDlg::CDnAppellationRewardCoinTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticGold(NULL)
, m_pStaticSilver(NULL)
, m_pStaticBronze(NULL)
{
}

CDnAppellationRewardCoinTooltipDlg::~CDnAppellationRewardCoinTooltipDlg(void)
{
}

void CDnAppellationRewardCoinTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AppellationCoinDlg.ui" ).c_str(), bShow );
}

void CDnAppellationRewardCoinTooltipDlg::InitialUpdate()
{
	m_pStaticGold = GetControl<CEtUIStatic>( "ID_GOLD" );
	m_pStaticSilver = GetControl<CEtUIStatic>( "ID_SILVER" );
	m_pStaticBronze = GetControl<CEtUIStatic>( "ID_BRONZE" );
}

void CDnAppellationRewardCoinTooltipDlg::SetRewardCoin( int nRewardCoin )
{
	int nGold = nRewardCoin/10000;
	int nSilver = (nRewardCoin%10000)/100;
	int nBronze = nRewardCoin%100;
	m_pStaticGold->SetIntToText( nGold );
	m_pStaticSilver->SetIntToText( nSilver );
	m_pStaticBronze->SetIntToText( nBronze );
}

#endif