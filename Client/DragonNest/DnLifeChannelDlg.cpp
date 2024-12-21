#include "Stdafx.h"
#include "DnLifeChannelDlg.h"
#include "DnTableDB.h"
#include "TaskManager.h"
#include "DnLoginTask.h"
#include "DnInterface.h"
#include "DnTradeTask.h"
#include "PartySendPacket.h"
#include "DnWorld.h"
#include "EtWorldEventArea.h"
#include "EtWorldEventControl.h"
#include "EtWorldSector.h"
#include "DnWorldData.h"
#include "DnLocalPlayerActor.h"
#include "DnPartyTask.h"
#include "DnUIString.h"
#include "DnLifeSkillPlantTask.h"
#include "DnLifeChannelListDlg.h"
#include "DnItemTask.h"
#include "DnCashInventory.h"
#include "DnVIPDataMgr.h"
#include "SyncTimer.h"
#include "DnBridgeTask.h"
#include "FarmSendPacket.h"
#include "DnCommonTask.h"


const float CHANNEL_REFRESH_TIME = 60.0f;

CDnLifeChannelDlg::CDnLifeChannelDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pBaseCheck( NULL )
, m_pStaticCheck( NULL )
, m_pHarvestBtn( NULL )
, m_pGrowingBtn( NULL )
, m_pEmptyBtn( NULL )
, m_pEmptyBtn2( NULL )
, m_pZoneList( NULL )
, m_pPlantList( NULL )
, m_pZoneMap( NULL )
, m_pButtonOK( NULL )
, m_pStaticMapName( NULL )
, m_pStaticEmptyText( NULL )
, m_fElapsedTime(CHANNEL_REFRESH_TIME)
, m_nFarmDBID( 0 )
, m_bRefreshGate(true)
#if defined(_WORK)	
, m_bQAZone( false )
#endif
{

}

CDnLifeChannelDlg::~CDnLifeChannelDlg()
{
	RemoveList();
	SAFE_RELEASE_SPTR( m_hMinimap );	
}

void CDnLifeChannelDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeChannelDlg.ui" ).c_str(), bShow );
}

void CDnLifeChannelDlg::InitialUpdate()
{
	m_pBaseCheck = GetControl<CEtUIButton>( "ID_STATIC_CHECK" );
	m_pHarvestBtn = GetControl<CEtUIButton>( "ID_BUTTON_HARVEST" );
	m_pGrowingBtn = GetControl<CEtUIButton>( "ID_BUTTON_GROWING" );
	m_pEmptyBtn = GetControl<CEtUIButton>( "ID_STATIC_CHECK" );
	m_pEmptyBtn2 = GetControl<CEtUIButton>( "ID_BUTTON_EMPTY" );
	m_pZoneList = GetControl<CEtUIListBox>( "ID_LISTBOX_NAME" );
	m_pPlantList = GetControl<CEtUIListBoxEx>( "ID_LISTBOXEX_CONDITION" );
	m_pZoneMap = GetControl<CEtUITextureControl>( "ID_TEXTUREL_MAP" );

	m_pButtonOK = GetControl<CEtUIButton>( "ID_BT_OK" );
	m_pStaticMapName = GetControl<CEtUIStatic>( "ID_TITLE" );

	m_pStaticEmptyText = GetControl<CEtUIStatic>( "ID_TEXT4" );

	m_pBaseCheck->Show( false );
	m_pHarvestBtn->Show( false );
	m_pGrowingBtn->Show( false );
	m_pButtonOK->Enable( false );
	m_pEmptyBtn->Show( false );
	m_pEmptyBtn2->Show( false );
	m_pStaticEmptyText->Show( false );
}

void CDnLifeChannelDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_fElapsedTime = CHANNEL_REFRESH_TIME;
		RequestFocus( m_pZoneList );
	}
	else
	{
		RemoveList();
		m_pButtonOK->Enable( false );
		GetInterface().CloseFarmChannel();
		GetInterface().CloseBlind();
	}

	CDnCustomDlg::Show( bShow );
}

void CDnLifeChannelDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		if( m_fElapsedTime <= 0.0f )
		{
			RemoveList();
			if (m_bRefreshGate)
			{
				EtVector3 vPos = *CDnActor::s_hLocalActor->GetStaticPosition();
				SendRefreshGateInfo( false, vPos );
				SendRefreshGateInfo( true, vPos );
			}
			else
			{
				Farm::Send::SendReqFarmInfo();
			}
			m_fElapsedTime = CHANNEL_REFRESH_TIME;
		}
		else
		{
			m_fElapsedTime -= fElapsedTime;
		}
	}
}

void CDnLifeChannelDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_OK" ) )
		{
			GetPartyTask().SetEnterPotal(false); //rlkt_dirty_fix
			SelectChannel();
		}
		else if( IsCmdControl( "ID_BT_CANCEL" ) || IsCmdControl( "ID_BUTTON_CLOSE" ) )
		{
			GetPartyTask().OnCancelStage();
			PositionCancel();
			Show( false );
		}
	}
	else if( nCommand == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		if( IsCmdControl("ID_LISTBOX_NAME" ) )
		{
			SelectChannel();
			return;
		}
	}
	else if( nCommand == EVENT_LISTBOX_SELECTION )
	{
		if( IsCmdControl( "ID_LISTBOX_NAME" ) )
		{
			SListBoxItem *pItem = m_pZoneList->GetSelectedItem();
			if( pItem )
			{
				SFarmInfo *pInfo = (SFarmInfo*)(pItem->pData);
				if( CGlobalInfo::GetInstance().m_cLocalAccountLevel == 0 && pInfo->m_nCongestion == HIGH_CONGESTION )
				{
					m_pButtonOK->Enable( false );
				}
				else
				{
					SFarmInfo * pFarmInfo = (SFarmInfo *)pItem->pData;

					m_pButtonOK->Enable( true );
					RefreshPlantedText( pFarmInfo );
					RefreshPlantIcon( pFarmInfo->m_nFarmDBID, pFarmInfo->m_nMapID );
				}
			}
		}
		if( IsCmdControl( "ID_LISTBOXEX_CONDITION" ) )
		{
			int nSelect = m_pPlantList->GetSelectedIndex();

			if( -1 != nSelect )
			{
				CDnLifeChannelListDlg * pItem = m_pPlantList->GetItem<CDnLifeChannelListDlg>( nSelect );

				int nID = pItem->GetID();

				if( -1 != nID )
				{
					m_pStaticCheck->SetPosition( m_vPlantedInfo[nID].m_vButtonCrood.fX, m_vPlantedInfo[nID].m_vButtonCrood.fY );
					m_pStaticCheck->Show( true );
					return;
				}
			}

			m_pStaticCheck->Show( false );
		}
	}
}

bool CDnLifeChannelDlg::CompareFarmChannelInfo( const std::pair<TFarmItem, int> &s1, const std::pair<TFarmItem, int> &s2 )
{
#if defined( PRE_ADD_FARM_DOWNSCALE )
	if( s1.first.iAttr != s2.first.iAttr )
	{
		if( s2.first.iAttr&Farm::Attr::DownScale )
			return true;
		if( s1.first.iAttr <= s2.first.iAttr)
			return false;
		else
			return true;
	}
#elif defined(PRE_ADD_VIP_FARM)
	if( s1.first.Attr != s2.first.Attr )
	{
		if( s1.first.Attr <= s2.first.Attr)
			return false;
		else
			return true;
	}
#endif

	float fUserRatio1 = (float)s1.first.iFarmCurUser / s1.first.iFarmMaxUser;
	float fUserRatio2 = (float)s2.first.iFarmCurUser / s2.first.iFarmMaxUser;

	if( s1.second >= s2.second )	
		return true;
	else
	{
		if( fUserRatio1 > fUserRatio2 ) 
			return true;
	}

	return false;
}

void CDnLifeChannelDlg::AddSortChannelList()
{
	int nCongestion(NO_CONGESTION);
	std::wstring strCongestion;

	wchar_t wszColor[255]={0,};

	for( int itr = 0; itr < (int)m_vecChannelInfo.size(); ++itr )
	{
		TFarmItem * pFarm = &(m_vecChannelInfo[itr].first);

		if( !pFarm )
		{
			ASSERT( pFarm && "Farm Information Error" );
			continue;
		}

		float fCongestion = (float)pFarm->iFarmCurUser / pFarm->iFarmMaxUser;
		if( fCongestion >= 0.95f )
		{
			strCongestion = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100039 );
			nCongestion = HIGH_CONGESTION;
			_wcscpy(wszColor, _countof(wszColor), L"@RGB(204,51,0)", (int)wcslen(L"@RGB(204,51,0)"));
		}
		else if( fCongestion >= 0.40f )
		{
			strCongestion = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100040 );
			nCongestion = MID_CONGESTION;
			_wcscpy(wszColor, _countof(wszColor), L"@RGB(255,204,0)", (int)wcslen(L"@RGB(255,204,0)"));
		}
		else if( fCongestion >= 0.10f )
		{
			strCongestion = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100041 );
			nCongestion = LOW_CONGESTION;		
			_wcscpy(wszColor, _countof(wszColor), L"@RGB(102,204,255)", (int)wcslen(L"@RGB(102,204,255)"));
		}
		else
		{
			strCongestion = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100042 );
			nCongestion = NO_CONGESTION;
			_wcscpy(wszColor, _countof(wszColor), L"@RGB(153,204,0)", (int)wcslen(L"@RGB(153,204,0)"));
		}

		wchar_t strTemp[128] = {0}, strTemp1[128];
		wchar_t szServerFullName[255] = {0,};
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
		if( pSox )
		{
			int nMapNameID = pSox->GetFieldFromLablePtr( pFarm->iFarmMapID, "_MapNameID" )->GetInteger();

#if defined( PRE_ADD_FARM_DOWNSCALE )
			if( Farm::Attr::Vip & pFarm->iAttr )
			{
				swprintf_s( strTemp, _countof(strTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7530 ), pFarm->iFarmDBID );
				if( 0 < m_vecChannelInfo[itr].second )
				{
					if( Farm::Attr::DownScale & pFarm->iAttr )
						swprintf_s( strTemp, _countof(strTemp), L"%s@RGB(204,51,0)%s@/RGB(%d)", strTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ), m_vecChannelInfo[itr].second );
					else
						swprintf_s( strTemp, _countof(strTemp), L"%s(%d)", strTemp, m_vecChannelInfo[itr].second );
				}
				else if( Farm::Attr::DownScale & pFarm->iAttr ) 
					swprintf_s( strTemp, _countof(strTemp), L"%s@RGB(204,51,0)%s@/RGB", strTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ) );

				swprintf_s( strTemp1, _countof(strTemp1), L"@RGB(220,220,50)%s@/RGB", strTemp );	// VIP 구역
			}
			else if( Farm::Attr::GuildChampion & pFarm->iAttr )
			{
				swprintf_s( strTemp, _countof(strTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7533 ) );
				if( 0 < m_vecChannelInfo[itr].second )
				{
					if( Farm::Attr::DownScale & pFarm->iAttr )
						swprintf_s( strTemp, _countof(strTemp), L"%s@RGB(204,51,0)%s@/RGB(%d)", strTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ), m_vecChannelInfo[itr].second );
					else
						swprintf_s( strTemp, _countof(strTemp), L"%s(%d)", strTemp, m_vecChannelInfo[itr].second );
				}
				else if( Farm::Attr::DownScale & pFarm->iAttr )
					swprintf_s( strTemp, _countof(strTemp), L"%s@RGB(204,51,0)%s@/RGB", strTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ) );

				swprintf_s( strTemp1, _countof(strTemp1), L"@RGB(50,250,50)%s@/RGB", strTemp );	// 우승길드 구역
			}
			else
#elif defined(PRE_ADD_VIP_FARM)
			if( Farm::Attr::Vip & pFarm->Attr )
			{
				swprintf_s( strTemp, _countof(strTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7530 ), pFarm->iFarmDBID );
				if( 0 < m_vecChannelInfo[itr].second )
				{
					if( Farm::Attr::DownScale & pFarm->Attr )
						swprintf_s( strTemp, _countof(strTemp), L"%s@RGB(204,51,0)%s@/RGB(%d)", strTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ), m_vecChannelInfo[itr].second );
					else
						swprintf_s( strTemp, _countof(strTemp), L"%s(%d)", strTemp, m_vecChannelInfo[itr].second );
				}
				else if( Farm::Attr::DownScale & pFarm->Attr ) 
					swprintf_s( strTemp, _countof(strTemp), L"%s@RGB(204,51,0)%s@/RGB", strTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ) );

				swprintf_s( strTemp1, _countof(strTemp1), L"@RGB(220,220,50)%s@/RGB", strTemp );	// VIP 구역
			}
			else if( Farm::Attr::GuildChampion & pFarm->Attr )
			{
				swprintf_s( strTemp, _countof(strTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7533 ) );
				if( 0 < m_vecChannelInfo[itr].second )
				{
					if( Farm::Attr::DownScale & pFarm->Attr )
						swprintf_s( strTemp, _countof(strTemp), L"%s@RGB(204,51,0)%s@/RGB(%d)", strTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ), m_vecChannelInfo[itr].second );
					else
						swprintf_s( strTemp, _countof(strTemp), L"%s(%d)", strTemp, m_vecChannelInfo[itr].second );
				}
				else if( Farm::Attr::DownScale & pFarm->Attr )
					swprintf_s( strTemp, _countof(strTemp), L"%s@RGB(204,51,0)%s@/RGB", strTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ) );

				swprintf_s( strTemp1, _countof(strTemp1), L"@RGB(50,250,50)%s@/RGB", strTemp );	// 우승길드 구역
			}
			else
#endif	//#if defined( PRE_ADD_FARM_DOWNSCALE )
			{
				swprintf_s( strTemp1, _countof(strTemp1), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7422 ), pFarm->iFarmDBID );
				if( 0 < m_vecChannelInfo[itr].second )
				{
#if defined(PRE_ADD_FARM_DOWNSCALE)
					if( Farm::Attr::DownScale & pFarm->iAttr )
						swprintf_s( strTemp1, _countof(strTemp1), L"%s@RGB(204,51,0)%s@/RGB(%d)", strTemp1, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ), m_vecChannelInfo[itr].second );
					else
#endif // #if defined(PRE_ADD_FARM_DOWNSCALE) 
						swprintf_s( strTemp1, _countof(strTemp1), L"%s(%d)", strTemp1, m_vecChannelInfo[itr].second );
				}
#if defined(PRE_ADD_FARM_DOWNSCALE)
				else if( Farm::Attr::DownScale & pFarm->iAttr )
					swprintf_s( strTemp1, _countof(strTemp1), L"%s@RGB(204,51,0)%s@/RGB", strTemp1, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7542 ) );
#endif // #if defined(PRE_ADD_FARM_DOWNSCALE)
			 
			} 

			_wcscpy(szServerFullName, _countof(szServerFullName), strTemp1, (int)wcslen(strTemp1));
			//rlkt 2016
			//online players
			//wcscat_s(szServerFullName, FormatW(L"(%d/%d) ",pFarm->iFarmCurUser , pFarm->iFarmMaxUser).c_str());
			
			//
			wcscat_s(szServerFullName, L"@MOVE(0.07f)[ ");
			wcscat_s(szServerFullName, wszColor );
			//wcscat_s(szServerFullName, strCongestion.c_str() );
			wcscat_s(szServerFullName, FormatW(L"(%d/%d) ",pFarm->iFarmCurUser , pFarm->iFarmMaxUser).c_str());
			wcscat_s(szServerFullName, L"@/RGB ]");
		}

#if defined( PRE_ADD_FARM_DOWNSCALE )
		SFarmInfo * farmInfo = new SFarmInfo( pFarm->iFarmDBID, pFarm->iFarmMapID, nCongestion, pFarm->iAttr );
#elif defined(PRE_ADD_VIP_FARM)
		SFarmInfo * farmInfo = new SFarmInfo( pFarm->iFarmDBID, pFarm->iFarmMapID, nCongestion, pFarm->Attr );
#else	//#if defined(PRE_ADD_VIP_FARM)
		SFarmInfo * farmInfo = new SFarmInfo( pFarm->iFarmDBID, pFarm->iFarmMapID, nCongestion );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )


		m_pZoneList->AddItem( szServerFullName, farmInfo, itr );
	}
}

void CDnLifeChannelDlg::AddChannelList( SCFarmInfo * pFarmInfo )
{
	RemoveList();

	m_bRefreshGate = pFarmInfo->bRefreshGate;

	for( int i=0; i<pFarmInfo->cCount; i++ )
		m_vecChannelInfo.push_back( std::make_pair(pFarmInfo->Farms[i], 0) );
}

void CDnLifeChannelDlg::UpdateFarmPlantedInfo( SCFarmPlantedInfo * pPlantedInfo )
{
	DNTableFileFormat* pFarmSox = GetDNTable(CDnTableDB::TFARMCULTIVATE);
	DNTableFileFormat* pItemSox = GetDNTable(CDnTableDB::TITEM);

	for( BYTE itr = 0; itr < pPlantedInfo->cCount; ++itr )
	{
		SPlantInfo sPlantInfo;
		sPlantInfo.m_nFarmDBID = pPlantedInfo->FarmFields[itr].iFarmDBID;
		sPlantInfo.m_nFieldIndex = pPlantedInfo->FarmFields[itr].nFieldIndex;
		sPlantInfo.m_nItemID = pPlantedInfo->FarmFields[itr].iItemID;
		
		int nMaxTime = pFarmSox->GetFieldFromLablePtr( sPlantInfo.m_nItemID, "_CultivateMaxTime" )->GetInteger();

		int nDecreaseTime = 0;
		for( int count = 0; count < Farm::Max::ATTACHITEM_KIND; ++count )
		{
			int nItemID = pPlantedInfo->FarmFields[itr].AttachItems[count].iItemID;

			if( 0 == nItemID )
				break;

			if( CDnLifeSkillPlantTask::GROWING_BOOST_ITEM == nItemID )
				nDecreaseTime += pItemSox->GetFieldFromLablePtr( nItemID, "_TypeParam1" )->GetInteger() * pPlantedInfo->FarmFields[itr].AttachItems[count].iCount;
		}

		sPlantInfo.m_nRemainTime = nMaxTime - nDecreaseTime - pPlantedInfo->FarmFields[itr].iElapsedTimeSec;

		if( 0 >= sPlantInfo.m_nRemainTime )
			sPlantInfo.m_bHarvest = true;
		else
			sPlantInfo.m_bHarvest = false;

		m_vPlantedInfo.push_back( sPlantInfo );

		for( int count = 0; count < (int)m_vecChannelInfo.size(); ++count )
		{
			if( sPlantInfo.m_nFarmDBID == m_vecChannelInfo[count].first.iFarmDBID )
			{
				++m_vecChannelInfo[count].second;
				break;
			}
			else if( 0 == sPlantInfo.m_nFarmDBID )
				++m_vecChannelInfo[count].second;
		}
	}

#if defined(_WORK)	
	if( !m_bQAZone )
	{
		if( !m_vecChannelInfo.empty() )
		{
#if defined( PRE_ADD_FARM_DOWNSCALE )
			std::vector< std::pair<TFarmItem,int> >::iterator itor = m_vecChannelInfo.begin();
			std::vector< std::pair<TFarmItem,int> >::iterator eraseItor = m_vecChannelInfo.begin();
			for( ; itor != m_vecChannelInfo.end(); ++itor )
			{
				if( Farm::Attr::None == itor->first.iAttr )
				{
					if( eraseItor->first.iFarmDBID < itor->first.iFarmDBID )
						eraseItor = itor;
				}
			}

			if( eraseItor != m_vecChannelInfo.end() )
				m_vecChannelInfo.erase( eraseItor );
#elif defined(PRE_ADD_VIP_FARM)
			std::vector< std::pair<TFarmItem,int> >::iterator itor = m_vecChannelInfo.begin();
			std::vector< std::pair<TFarmItem,int> >::iterator eraseItor = m_vecChannelInfo.begin();
			for( ; itor != m_vecChannelInfo.end(); ++itor )
			{
				if( Farm::Attr::None == itor->first.Attr )
				{
					if( eraseItor->first.iFarmDBID < itor->first.iFarmDBID )
						eraseItor = itor;
				}
			}
			
			if( eraseItor != m_vecChannelInfo.end() )
				m_vecChannelInfo.erase( eraseItor );
#else
			m_vecChannelInfo.erase( m_vecChannelInfo.end() - 1 );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
		}
	}
#endif


	if( !m_vecChannelInfo.empty() )
	{
		InitializeMinimap( m_vecChannelInfo[0].first.iFarmMapID );

		std::stable_sort( m_vecChannelInfo.begin(), m_vecChannelInfo.end(), CompareFarmChannelInfo );
		AddSortChannelList();
	}
}

void CDnLifeChannelDlg::SelectChannel()
{
	SListBoxItem *pItem = m_pZoneList->GetSelectedItem();
	if( !pItem ) return;

	SFarmInfo *pInfo = (SFarmInfo*)(pItem->pData);
	if( CGlobalInfo::GetInstance().m_cLocalAccountLevel == 0 && pInfo->m_nCongestion == HIGH_CONGESTION )
	{
		GetInterface().MessageBox( 97, MB_OK );
		return;
	}

#if defined(PRE_ADD_FARM_DOWNSCALE)
	CDnBridgeTask::GetInstance().InitAttr();

	if( Farm::Attr::Vip & pInfo->iAttr )
#elif defined(PRE_ADD_VIP_FARM) 
	if( Farm::Attr::Vip & pInfo->Attr )
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
#if defined(PRE_ADD_FARM_DOWNSCALE) || defined(PRE_ADD_VIP_FARM) 
	{
		CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
		bool bVip = pLocalActor->IsVipFarm();

		if( !bVip )
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7531 ) );	//농장구역 입장조건을 만족하지 못했습니다. 다시 한번 입장조건을 확인해주세요.
			return;
		}
		else
		{
#if defined(PRE_ADD_FARM_DOWNSCALE)
			CDnBridgeTask::GetInstance().AddAttr( Farm::Attr::Vip );
#elif defined(PRE_ADD_VIP_FARM) 
			CDnBridgeTask::GetInstance().SetAttr( Farm::Attr::Vip );
#endif
		}
	}
	else
	{
		
#if defined(PRE_ADD_FARM_DOWNSCALE)
		CDnBridgeTask::GetInstance().AddAttr( Farm::Attr::None );
#elif defined(PRE_ADD_VIP_FARM) 
		CDnBridgeTask::GetInstance().SetAttr( Farm::Attr::None );
#endif
	}
#endif	// defined(PRE_ADD_FARM_DOWNSCALE) || defined(PRE_ADD_VIP_FARM) 

	// 혹시 요청중인 거래가 있다면 취소하고 이동패킷을 요청한다.
	GetTradeTask().GetTradePrivateMarket().ClearTradeUserInfoList();
	GetInterface().DisableCashShopMenuDlg(true);

	m_nFarmDBID = pInfo->m_nFarmDBID;

	CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
	if( pTask )			
		pTask->SendWindowState(WINDOW_NONE);

#ifdef PRE_PARTY_DB
	SendStartStage( Dungeon::Difficulty::Easy, -1, false, m_nFarmDBID );
#else
	SendStartStage( 0, -1, false, m_nFarmDBID );
#endif

	Show( false );
}

void CDnLifeChannelDlg::RemoveList()
{
	SListBoxItem *pItem(NULL);
	int nSize = m_pZoneList->GetSize();
	for( int i=0; i<nSize; i++ )
	{
		pItem = m_pZoneList->GetItem( i );
		if( pItem ) SAFE_DELETE( pItem->pData );
	}

	m_vecChannelInfo.clear();
	m_pZoneList->RemoveAllItems();
	m_pPlantList->RemoveAllItems();
	m_vPlantedInfo.clear();
	m_pButtonOK->Enable( false );
	RemoveIcon();
}

void CDnLifeChannelDlg::RemoveIcon()
{
	for( std::vector<CEtUIButton *>::iterator itor = m_vPlantIcon.begin(); itor != m_vPlantIcon.end(); ++itor )
		DeleteControl( (*itor) );

	m_vPlantIcon.clear();
	DeleteControl( m_pStaticCheck );
}

void CDnLifeChannelDlg::RefreshPlantedText(SFarmInfo *pFarmInfo)
{
	m_pPlantList->RemoveAllItems();

	wchar_t wszColor[255]={0,};
	wchar_t wszInfo[255] = {0,};
	wchar_t wszTime[255] = {0,};

	bool bIsExist = false;

	tstring szName;
	CDnLifeChannelListDlg * pItem;
	for( int itr = 0; itr < (int)m_vPlantedInfo.size(); ++itr )
	{
		if( pFarmInfo->m_nFarmDBID == m_vPlantedInfo[itr].m_nFarmDBID ||
			0 == m_vPlantedInfo[itr].m_nFarmDBID )
		{
			szName = CDnItem::GetItemFullName( m_vPlantedInfo[itr].m_nItemID );

			pItem = m_pPlantList->InsertItem<CDnLifeChannelListDlg>(0);
			pItem->SetInfo( szName.c_str(), m_vPlantedInfo[itr].m_bHarvest, m_vPlantedInfo[itr].m_nRemainTime, itr );

			bIsExist = true;
		}
	}

	if( !bIsExist )
	{
		pItem = m_pPlantList->InsertItem<CDnLifeChannelListDlg>(0);
		pItem->SetEmpty();
	}
}

void CDnLifeChannelDlg::RefreshPlantIcon( int iFarmDBID, int iMapIndex )
{
	if( !CDnWorld::IsActive() || !CDnWorld::GetInstance().GetGrid() ) return;
	RemoveIcon();

	CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( iMapIndex );
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
	if( !pWorldData || !pSox )
		return;

	SUICoord textureCoord, UVCoord, uiCoord, uiCheckCoord;
	m_pZoneMap->GetUICoord(textureCoord);
	m_pZoneMap->GetUVCoord(UVCoord);
	uiCheckCoord = m_pBaseCheck->GetUICoord();

	float fMapWidth = pWorldData->GetGridWidth() * 50.f;
	float fMapHeight = pWorldData->GetGridHeight() * 50.f;

	tstring szName;
	CDnWorldData::PlantData *pPlantData;
	CEtUIButton *pButton(NULL);
	for( int itr = 0; itr < (int)m_vPlantedInfo.size(); ++itr )
	{
		if( iFarmDBID == m_vPlantedInfo[itr].m_nFarmDBID ||
			0 == m_vPlantedInfo[itr].m_nFarmDBID )
		{
			SUIControlProperty sUIProperty;

			pPlantData = pWorldData->GetPlantData( m_vPlantedInfo[itr].m_nFieldIndex );
			if( NULL == pPlantData ) continue;

			if( m_vPlantedInfo[itr].m_bHarvest )
				m_pHarvestBtn->GetProperty( sUIProperty );
			else
				m_pGrowingBtn->GetProperty( sUIProperty );

			pButton = (CEtUIButton*)CEtUIDialog::CreateControl( &sUIProperty );
			pButton->Show( true );

			float fNpcX = pPlantData->vPos.x + fMapWidth;
			float fNpcY = fabs(pPlantData->vPos.y - fMapHeight);		

			fNpcX = (((fNpcX / fMapWidth )*0.5f - UVCoord.fX) / UVCoord.fWidth) * textureCoord.fWidth;
			fNpcY = (((fNpcY / fMapHeight )*0.5f - UVCoord.fY) / UVCoord.fHeight) * textureCoord.fHeight;

			pButton->GetUICoord(uiCoord);
			pButton->SetPosition( textureCoord.fX + fNpcX - (uiCoord.fWidth/2.0f), textureCoord.fY + fNpcY - (uiCoord.fHeight/2.0f) );

			int nNameID = pSox->GetFieldFromLablePtr( m_vPlantedInfo[itr].m_nItemID, "_NameID" )->GetInteger();
			char *szParam = pSox->GetFieldFromLablePtr( m_vPlantedInfo[itr].m_nItemID, "_NameIDParam" )->GetString();
			MakeUIStringUseVariableParam( szName, nNameID, szParam );

			pButton->SetTooltipText( szName.c_str() );

			m_vPlantedInfo[itr].m_vButtonCrood = pButton->GetUICoord();
			m_vPlantedInfo[itr].m_vButtonCrood.fX -= uiCheckCoord.fWidth/4;
			m_vPlantedInfo[itr].m_vButtonCrood.fY -= uiCheckCoord.fHeight/2;

			m_vPlantIcon.push_back( pButton );
		}
	}

	SUIControlProperty sUIProperty;
	m_pBaseCheck->GetProperty( sUIProperty );

	m_pStaticCheck = (CEtUIButton*)CEtUIDialog::CreateControl( &sUIProperty );
	m_pStaticCheck->Show( false );
}

void CDnLifeChannelDlg::InitializeMinimap( int nMapID )
{
	CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( nMapID );
	if( pWorldData )
	{
		std::string strMiniMapName;
		pWorldData->GetMiniMapName( strMiniMapName );

		char szTemp[_MAX_PATH] = {0};
		sprintf_s( szTemp, _MAX_PATH, "%s\\Grid\\%s\\%s_Map.dds", CEtResourceMng::GetInstance().GetFullPath( "MapData" ).c_str(), strMiniMapName.c_str(), strMiniMapName.c_str() );
		SAFE_RELEASE_SPTR( m_hMinimap );
		m_hMinimap = LoadResource( szTemp, RT_TEXTURE );
		if( !m_hMinimap ) {
			sprintf_s( szTemp, _MAX_PATH, "%s\\Grid\\%s\\%s.dds", CEtResourceMng::GetInstance().GetFullPath( "MapData" ).c_str(), strMiniMapName.c_str(), strMiniMapName.c_str() );
			m_hMinimap = LoadResource( szTemp, RT_TEXTURE );
		}

		int nX = 0, nY = 0, nWidth = 1024, nHeight = 1024;
		if( m_hMinimap ) {
			m_hMinimap->CalcFittingArea( 10, &nX, &nY, &nWidth, &nHeight);

			if( nWidth > nHeight ) {
				nY -= (nWidth-nHeight)/2;
				nHeight = nWidth;
			}
			if( nHeight > nWidth ) {
				nX -= (nHeight-nWidth)/2;
				nWidth = nHeight;
			}

			const int GabSize = 10;

			nX -= GabSize;
			nWidth += GabSize*2;
			nY -= GabSize;
			nHeight += GabSize*2;
		}
		m_pZoneMap->SetTexture( m_hMinimap, nX, nY, nWidth, nHeight );

		std::wstring strMapName;
		pWorldData->GetMapName( strMapName );
		m_pStaticMapName->SetText( strMapName );
	}
}

void CDnLifeChannelDlg::PositionCancel()
{
	CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
	if( CDnPartyTask::IsActive() ) 
	{
		int reqGateIdx = CDnPartyTask::GetInstance().GetRequestEnteredGateIndex();
		if (reqGateIdx != -1)
		{
			const CDnWorld::GateStruct *pGate = CDnWorld::GetInstance().GetGateStruct(reqGateIdx);
			CDnWorld * pWorld = CDnWorld::GetInstancePtr();
			if( !pWorld || !pWorld->GetGrid() ) return;
			if (pGate)
			{
				//	Get StartPosition Center
				EtVector3 startPosCenter;
				char szStartName[32];
				sprintf_s( szStartName, "StartPosition %d", reqGateIdx );
				for( DWORD i=0 ; i<pWorld->GetGrid()->GetActiveSectorCount() ; i++ ) 
				{
					CEtWorldSector* pSector	= pWorld->GetGrid()->GetActiveSector(i);
					CEtWorldEventControl* pControl	= pSector->GetControlFromUniqueID( ETE_EventArea );
					if (pControl == NULL)
						continue;

					CEtWorldEventArea* pArea = pControl->GetAreaFromName( szStartName );
					if (pArea) 
					{
						SOBB *pBox = pArea->GetOBB();
						if (pBox)
							startPosCenter = pBox->Center;
						break;
					}
				}

				SOBB* pOBB = pGate->pGateArea->GetOBB();
				if( pOBB != NULL)
				{
					EtVector3 vGatePos = pOBB->Center;
					EtVector3 gateOriented = startPosCenter - vGatePos;
					EtVec3Normalize(&gateOriented, &gateOriented);
					if (pLocalActor)
						pLocalActor->OnCancelStage(gateOriented);

					//rlky_dirty_fix farm
					GetPartyTask().SetEnterPotal(false);
				}
			}
			else
				pLocalActor->LockInput( false );
		}
		else
			pLocalActor->LockInput( false );
	}
	else
		pLocalActor->LockInput( false );
}


#if defined(_WORK)
void CDnLifeChannelDlg::ToggleQAZone()
{
	m_bQAZone = !m_bQAZone;

	m_fElapsedTime = -1.f;
}
#endif

