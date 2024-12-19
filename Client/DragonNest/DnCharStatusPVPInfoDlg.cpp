#include "StdAfx.h"
#include "DnCharStatusPVPInfoDlg.h"
#include "DnPlayerActor.h"
#include "DnCharStatusPVPInfoItemDlg.h"
#include "DnTableDB.h"
#include "DnPartyTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharStatusPVPInfoDlg::CDnCharStatusPVPInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pListBoxEx(NULL)
{
}

CDnCharStatusPVPInfoDlg::~CDnCharStatusPVPInfoDlg(void)
{
	m_pListBoxEx->RemoveAllItems();
}

void CDnCharStatusPVPInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharStatusPvpInfoDlg.ui" ).c_str(), bShow );
}

void CDnCharStatusPVPInfoDlg::InitialUpdate()
{
	m_pListBoxEx = GetControl<CEtUIListBoxEx>( "ID_LISTBOX_PVPINFO" );
	m_pListBoxEx->SetRenderSelectBar( false );
}

void CDnCharStatusPVPInfoDlg::Show( bool bShow ) 
{ 
	if( m_bShow == bShow )
		return;

	if( bShow )
	{		
		RefreshList();
	}
	else
	{
		m_pListBoxEx->RemoveAllItems();
	}

	CEtUIDialog::Show( bShow );
}

void CDnCharStatusPVPInfoDlg::RefreshList()
{
	m_pListBoxEx->RemoveAllItems();

	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor *pLocalPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	if( pLocalPlayer  )
	{
		TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();

		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
		int iUIString = 0;
		if ( pSox && pSox->IsExistItem( pPvPInfo->cLevel ) )
		{
			iUIString = pSox->GetFieldFromLablePtr( pPvPInfo->cLevel, "PvPRankUIString" )->GetInteger();
		}

		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, PVPLevel ) , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iUIString ));
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, PVPXP ) , pPvPInfo->uiXP );

		if( pPvPInfo->cLevel < PvPCommon::Common::MaxRank)
		{
			if ( pSox && pSox->IsExistItem( pPvPInfo->cLevel ) )
			{
				int nNextXP = pSox->GetFieldFromLablePtr( pPvPInfo->cLevel, "PvPRankEXP" )->GetInteger();
				InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, NEXTXP ) , nNextXP );
			}
		}

		InsertItem(L"" , -1 );

		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Win ) , pPvPInfo->uiWin);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Loose ) , pPvPInfo->uiLose);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Draw ) , pPvPInfo->uiDraw);

		InsertItem(L"" , -1 );

		int Total_KOCount = 0;
		for(int i=0; i<PvPCommon::Common::MaxClass; i++ )
			Total_KOCount += pPvPInfo->uiKOClassCount[i];

		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, KILL ) , Total_KOCount );


		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Warrior ) , pPvPInfo->uiKOClassCount[0]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Acher ) , pPvPInfo->uiKOClassCount[1]);		
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Socer ) , pPvPInfo->uiKOClassCount[2]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Cleric ) , pPvPInfo->uiKOClassCount[3]);
#if defined( PRE_ADD_ACADEMIC ) && !defined( PRE_REMOVE_ACADEMIC )
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Academic ) , pPvPInfo->uiKOClassCount[4]);
#endif
#if defined( PRE_ADD_KALI ) && !defined( PRE_REMOVE_KALI )
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Kali ) , pPvPInfo->uiKOClassCount[5]);
#endif
#if defined( PRE_ADD_ASSASSIN ) && !defined( PRE_REMOVE_ASSASSIN )
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Assassin ) , pPvPInfo->uiKOClassCount[6]);
#endif



		InsertItem(L"" , -1 );

		int Total_KOByCount = 0;
		for(int i=0; i<PvPCommon::Common::MaxClass; i++ )
			Total_KOByCount += pPvPInfo->uiKObyClassCount[i];
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, KILLED ) , Total_KOByCount );

		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Warrior ) , pPvPInfo->uiKObyClassCount[0] );
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Acher ) , pPvPInfo->uiKObyClassCount[1]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Socer ) , pPvPInfo->uiKObyClassCount[2]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Cleric ) , pPvPInfo->uiKObyClassCount[3]);
#if defined( PRE_ADD_ACADEMIC ) && !defined( PRE_REMOVE_ACADEMIC )
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Academic ) , pPvPInfo->uiKObyClassCount[4]);
#endif
#if defined( PRE_ADD_KALI ) && !defined( PRE_REMOVE_KALI )
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Kali ) , pPvPInfo->uiKObyClassCount[5]);
#endif
#if defined( PRE_ADD_ASSASSIN ) && !defined( PRE_REMOVE_ASSASSIN )
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Assassin ) , pPvPInfo->uiKObyClassCount[6]);
#endif

		InsertItem(L"" , -1 );

		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, KILLP ) , pPvPInfo->biTotalKillPoint);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AssiP ) , pPvPInfo->biTotalAssistPoint);	
#if !defined(_TH) && !defined(_ID)
		TPvPGhoulScores *pGhoulScoreInfo = CDnPartyTask::GetInstance().GetPvPGhoulInfo();
		
		InsertItem(L"" , -1 );
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Ghoul_PlayCount ) , pGhoulScoreInfo->nPoint[GhoulMode::PointType::PlayCount]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Ghoul_GhoulWin ) , pGhoulScoreInfo->nPoint[GhoulMode::PointType::GhoulWin]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Ghoul_HumanWin ) , pGhoulScoreInfo->nPoint[GhoulMode::PointType::HumanWin]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Ghoul_TimeOver ) , pGhoulScoreInfo->nPoint[GhoulMode::PointType::TimeOver]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Ghoul_GhoulKill ) , pGhoulScoreInfo->nPoint[GhoulMode::PointType::GhoulKill]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Ghoul_HumanKill ) , pGhoulScoreInfo->nPoint[GhoulMode::PointType::HumanKill]);
		InsertItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Ghoul_HolyWaterUse ) , pGhoulScoreInfo->nPoint[GhoulMode::PointType::HolyWaterUse]);
#endif
	}
}

void CDnCharStatusPVPInfoDlg::InsertItem(const  wchar_t * wszItemText , INT64 iData  )
{
	if( wszItemText && m_pListBoxEx )
	{
		CDnCharStatusPVPInfoItemDlg *pItem = m_pListBoxEx->AddItem<CDnCharStatusPVPInfoItemDlg>();
		pItem->SetInfo( wszItemText , iData );
	}	
}

void CDnCharStatusPVPInfoDlg::InsertItem(const wchar_t *wszItemText, const wchar_t *wszDataText)
{
	if( wszItemText && wszDataText && m_pListBoxEx )
	{
		CDnCharStatusPVPInfoItemDlg *pItem = m_pListBoxEx->AddItem<CDnCharStatusPVPInfoItemDlg>();
		pItem->SetInfo( wszItemText , wszDataText );
	}
}
