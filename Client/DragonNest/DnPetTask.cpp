#include "StdAfx.h"
#include "DnPetTask.h"
#include "DnLocalPlayerActor.h"
#include "DnItem.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnCharStatusDlg.h"
#include "SyncTimer.h"
#include "DnVehicleTask.h"
#include "DnPetNamingDlg.h"
#include "DnItemTask.h"
#include "DnCharInventory.h"
#include "DnCharPetDlg.h"
#include "DnPetActor.h"
#include "DnMainFrame.h"
#include "shlobj.h"
#include "direct.h"
#include "DnSkillTask.h"
#include "TaskManager.h"
#include "DnMainDlg.h"
#include "DnCommonTask.h"
#include "DnChatOption.h"
#include "DnPetExtendPeriodDlg.h"
#ifdef PRE_ADD_PET_EXTEND_PERIOD
#include "DnBuyPetExtendPeriodDlg.h"
#endif // PRE_ADD_PET_EXTEND_PERIOD
#include "TimeSet.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 


CDnPetTask::CDnPetTask() : CTaskListener( false )
, m_pItem( NULL )
, m_bIsNowSummonPet( false )
, m_nRemoveSlotIndex( 0 )
, m_bLoadPetConfig( false )
, m_bNotifySatietyHalf( false )
, m_bNotifySatietyEnd( false )
, m_bRequestUsePetFood( false )
, m_fCheckCoolTime( 0.0f )
, m_nChatID( 0 )
, m_nCurrentSatiety( 0 )
, m_nMaxSatiety( 0 )
, m_fSatietyPercent( 0.0f )
{
	memset( m_nSatietySelectionPickup, 0, sizeof( m_nSatietySelectionPickup ) );
	memset( m_nSatietySelectionApplyStatePercent, 0, sizeof( m_nSatietySelectionApplyStatePercent ) );
	memset( m_nSatietySelectionUseSkillNum, 0, sizeof( m_nSatietySelectionUseSkillNum ) );
}

CDnPetTask::~CDnPetTask()
{
	SAFE_DELETE( m_pItem );
	m_mapRemoveSkillQueue.clear();
	m_mapPetChat.clear();
}

bool CDnPetTask::Initialize()
{
	return true;
}

void CDnPetTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char* pData, int nSize )
{
	if( nMainCmd == SC_ITEMGOODS )
	{
		switch( nSubCmd )
		{
			case eItemGoods::SC_CHANGEPETNAME:	OnRecvPetNaming( (SCChangePetName*)pData ); break;
			case eItemGoods::SC_PETSKILLITEM:	OnRecvPetSkillItem( (SCPetSkillItem*)pData ); break;
			case eItemGoods::SC_PETSKILLEXPAND:	OnRecvPetSkillExpand( (SCPetSkillExpand*)pData ); break;
		}
	}
	else if( nMainCmd == SC_ITEM )
	{
		if( nSubCmd == eItem::SC_DELETE_PET_SKILL )
		{
			OnRecvRemovePetSkillItem( (SCPetSkillDelete*)pData );
		}
		else if( nSubCmd == eItem::SC_MODITEMEXPIREDATE )
		{
			OnRecvPetExtendPeriod( (SCModItemExpireDate*)pData );
		}
		else if( nSubCmd == eItem::SC_PET_CURRENT_SATIETY )
		{
			OnRecvCurrentSatiety( (SCPetCurrentSatiety*)pData );
		}
		else if( nSubCmd == eItem::SC_PET_FOOD_EAT )
		{
			OnRecvPetFoodEat( (SCPetFoodEat*)pData );
		}
	}
}

void CDnPetTask::OnRecvPetSummon( int nSessionID, int nPetItemID, LPCWSTR strPetName, bool bSummonAni )
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
	if( !hActor ) return;

	CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>( hActor.GetPointer() );
	if( !pPlayer ) return;

	pPlayer->SummonPet( nPetItemID, strPetName, bSummonAni );

	if( CDnLocalPlayerActor::s_hLocalActor && nSessionID == CDnLocalPlayerActor::s_hLocalActor->GetUniqueID() )
	{
		CDnPlayerActor* pLocalPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if( pLocalPlayer && pLocalPlayer->GetPetInfo().nType & Pet::Type::ePETTYPE_SATIETY )
		{
			SetPetSatietyInfo( pLocalPlayer->GetPetInfo().Vehicle[Pet::Slot::Body].nItemID, pLocalPlayer->GetPetInfo().nCurrentSatiety );
			CheckSatiety();
		}
	}

	LoadPetConfig();
}

void CDnPetTask::OnRecvPetSummonedOff( int nSessionID, bool bSummonAni )
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
	if( !hActor ) return;

	CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>( hActor.GetPointer() );
	if( !pPlayer ) return;

	TVehicleCompact& pPetCompact = pPlayer->GetPetInfo();
	if( pPetCompact.nSkillID1 > 0 )
	{
		RemovePetSkill( pPetCompact.nSkillID1 );
	}
	if( pPetCompact.nSkillID2 > 0 )
	{
		RemovePetSkill( pPetCompact.nSkillID2 );
	}

	if( CDnLocalPlayerActor::s_hLocalActor && nSessionID == CDnLocalPlayerActor::s_hLocalActor->GetUniqueID() )
	{
		ResetPetSatietyInfo();
		m_mapPetChat.clear();
	}

	pPlayer->SummonPet( -1, L"", bSummonAni );
}

void CDnPetTask::UsePetItem( CDnItem* pItem, bool bCallGestureQuickSlot )
{
	if( !GetPetTask().CheckCanMovePet( false ) ) return;
	CDnLocalPlayerActor* plocalActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	if( pItem == NULL || plocalActor == NULL )
		return;

	TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( pItem->GetSerialID() );
	if( pPetCompact == NULL ) return;
	const time_t pNowTime = CSyncTimer::GetInstance().GetCurTime();

	if( !pPetCompact->Vehicle[Pet::Slot::Body].bEternity && pNowTime >= pPetCompact->Vehicle[Pet::Slot::Body].tExpireDate )	// 만료기간 지났는지 체크
	{
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9221 ), false );
		return;
	}

	if( wcslen( pPetCompact->wszNickName) == 0 )	// 이름이 지정된 펫인지 체크
	{
		if( !plocalActor->IsInPetSummonableMap() )	// 소환 가능한 지역인지 체크
		{
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), false );
			CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
			return;
		}

		CDnPetNamingDlg* pPetNamingDlg = GetInterface().GetPetNamingDlg();
		if( pPetNamingDlg )
		{
			pPetNamingDlg->SetChangeItemSerial(0);
			pPetNamingDlg->SetPetSerial( pItem->GetSerialID() );
			pPetNamingDlg->Show( true );

			if( bCallGestureQuickSlot )	// GestureQuickSlot에서 콜된 경우 단축키가 Edit창에 먹어버리기 때문에 추가 처리 해야 함.
				pPetNamingDlg->SetCallGestureQuickSlot( true );
		}
		return;
	}

	if( m_bIsNowSummonPet )	// 이미 소환중이면 패스
		return;

	if( !plocalActor->IsInPetSummonableMap() )	// 소환 가능한 지역인지 체크
	{
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9244 ), false );
		return;
	}

	m_pItem = pItem;
	// 서버에 프로그레스 상태로 변환해줌.	
	CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
	if( pTask )			
		pTask->SendWindowState(WINDOW_PROGRESS);

	GetInterface().OpenMovieProcessDlg( NULL, 3.0f, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9217 ), PET_SUMMON_PROGRESS, this );
	m_bIsNowSummonPet = true;
}

void CDnPetTask::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if( nID == PET_SUMMON_PROGRESS )
	{
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( m_pItem == NULL )
				return;

			// 안보이는 버튼. 프로세스 처리가 끝났음을 알린다.
			if( strcmp( pControl->GetControlName(), "ID_PROCESS_COMPLETE" ) == 0 )
			{
				// 서버에 None상태로 변환해줌.	
				CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
				if( pTask )			
					pTask->SendWindowState(WINDOW_NONE);

				CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
				if( pCharStatusDlg )
				{
					bool bEquip = pCharStatusDlg->SetEquipItemFromInven( m_pItem->GetSlotIndex(), m_pItem, NULL );

					if( bEquip ) 
					{
						CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
						if( pMainMenuDlg )
							pMainMenuDlg->OpenCharStatusDialog();
					}
					pCharStatusDlg->ChangeEquipPage( (CDnCharStatusDlg::EquipPageType)m_pItem->GetClickedEquipPage() );
					m_bIsNowSummonPet = false;
				}
			}
			else if( strcmp( pControl->GetControlName(), "ID_BUTTON_CANCEL" ) == 0 )
			{
				GetInterface().CloseMovieProcessDlg( true );
				m_bIsNowSummonPet = false;
				// 서버에 None상태로 변환해줌.	
				CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
				if( pTask )			
					pTask->SendWindowState(WINDOW_NONE);
			}

			m_pItem = NULL;
		}
	}
	else if( nID == MSGBOX_REQUEST_REMOVE_PETSKILL )
	{
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( strcmp( pControl->GetControlName(), "ID_YES" ) == 0 && m_nRemoveSlotIndex > 0 )
			{
				CSPetSkillDelete petSkillDelete;
				petSkillDelete.cSlotNum = m_nRemoveSlotIndex;
				CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_DELETE_PET_SKILL, (char*)&petSkillDelete, int(sizeof(CSPetSkillDelete)) );
			}
		}

		m_nRemoveSlotIndex = 0;
	}
}

void CDnPetTask::LoadPetConfig()
{
	FILE *stream = NULL;
	char szString[256];
	char szFilename[MAX_PATH];

#ifndef _FINAL_BUILD
	sprintf_s( szFilename, 256, ".\\Config_Pet.ini" );
#else
	TCHAR pBuffer[MAX_PATH]={0};
	char szPath[_MAX_PATH] = { 0, };
	SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), pBuffer, CSIDL_PERSONAL, 0);
	WideCharToMultiByte( CP_ACP, 0, pBuffer, -1, szPath, sizeof(szPath), NULL, NULL );

	sprintf_s( szFilename, "%s\\DragonNest", szPath );
	_mkdir( szFilename );
	sprintf_s( szFilename, "%s\\DragonNest\\Config", szPath );
	_mkdir( szFilename );
	sprintf_s( szFilename, "%s\\DragonNest\\Config\\%s", szPath, "Config_Pet.ini" );
#endif

	if( fopen_s( &stream, szFilename, "r+" ) != 0 )
	{
		m_stPetOption.m_bGetItem = true;
		m_stPetOption.m_bGetNormalItem = true;
		m_stPetOption.m_bGetMagicItem = true;
		m_stPetOption.m_bGetRareItem = true;
		m_stPetOption.m_bGetEpicItem = true;
		m_stPetOption.m_bGetUniqueItem = true;
		m_stPetOption.m_bGetLegendItem = true;
//		m_stPetOption.m_bAutoUseHPPotion = true;
//		m_stPetOption.m_bAutoUseMPPotion = true;
//		m_stPetOption.m_nAutoUseHPPotionPercent = 50;
//		m_stPetOption.m_nAutoUseMPPotionPercent = 50;

		if( fopen_s( &stream, szFilename, "w+" ) != 0 )
			return;

		fseek( stream, 0L, SEEK_SET );

		fprintf( stream, "GetItem %d\n", m_stPetOption.m_bGetItem ? 1 : 0 );
		fprintf( stream, "GetNormalItem %d\n", m_stPetOption.m_bGetNormalItem ? 1 : 0 );
		fprintf( stream, "GetMagicItem %d\n", m_stPetOption.m_bGetMagicItem ? 1 : 0 );
		fprintf( stream, "GetRareItem %d\n", m_stPetOption.m_bGetRareItem ? 1 : 0 );
		fprintf( stream, "GetEpicItem %d\n", m_stPetOption.m_bGetEpicItem ? 1 : 0 );
		fprintf( stream, "GetUniqueItem %d\n", m_stPetOption.m_bGetUniqueItem ? 1 : 0 );
		fprintf( stream, "GetLegendItem %d\n", m_stPetOption.m_bGetLegendItem ? 1 : 0 );
//		fprintf( stream, "AutoUseHPPotion %d\n", m_stPetOption.m_bAutoUseHPPotion ? 1 : 0 );
//		fprintf( stream, "AutoUseMPPotion %d\n", m_stPetOption.m_bAutoUseMPPotion ? 1 : 0 );
//		fprintf( stream, "AutoUseHPPotionPercent %d\n", m_stPetOption.m_nAutoUseHPPotionPercent );
//		fprintf( stream, "AutoUseMPPotionPercent %d\n", m_stPetOption.m_nAutoUseMPPotionPercent );

		fclose( stream );
		return;
	}

	fseek( stream, 0L, SEEK_SET );

	for( int i=0; ;i++ )
	{
		int numRead = fscanf( stream, "%s", szString );
		if( numRead != 1 ) break;
		
		int iData = 0;
		if( strcmp( "GetItem", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bGetItem = iData ? true : false;
		}
		else if( strcmp( "GetNormalItem", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bGetNormalItem = iData ? true : false;
		}
		else if( strcmp( "GetMagicItem", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bGetMagicItem = iData ? true : false;
		}
		else if( strcmp( "GetRareItem", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bGetRareItem = iData ? true : false;
		}
		else if( strcmp( "GetEpicItem", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bGetEpicItem = iData ? true : false;
		}
		else if( strcmp( "GetUniqueItem", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bGetUniqueItem = iData ? true : false;
		}
		else if( strcmp( "GetLegendItem", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bGetLegendItem = iData ? true : false;
		}
/*		else if( strcmp( "AutoUseHPPotion", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bAutoUseHPPotion = iData ? true : false;
		}
		else if( strcmp( "AutoUseMPPotion", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bAutoUseMPPotion = iData ? true : false;
		}
		else if( strcmp( "AutoUseHPPotionPercent", szString ) == 0 )
		{
			fscanf( stream, "%d", &m_stPetOption.m_nAutoUseHPPotionPercent );
		}
		else if( strcmp( "AutoUseMPPotionPercent", szString ) == 0 )
		{
			fscanf( stream, "%d", &m_stPetOption.m_nAutoUseMPPotionPercent );
		}
*/
		else if( strcmp( "FoodItem", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_nFoodItemID = iData;
		}
		else if( strcmp( "AutoFeed", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_bAutoFeed = iData ? true : false;
		}
		else if( strcmp( "AutoFeedPercent", szString ) == 0 )
		{
			fscanf( stream, "%d", &iData );
			m_stPetOption.m_nAutoFeedPercent = iData;
		}
	}

	fclose( stream );
	m_bLoadPetConfig = true;
}

void CDnPetTask::SetPetOption( stPetOption stPetOptionData )
{
	m_stPetOption.m_bGetItem = stPetOptionData.m_bGetItem;
	m_stPetOption.m_bGetNormalItem = stPetOptionData.m_bGetNormalItem;
	m_stPetOption.m_bGetMagicItem = stPetOptionData.m_bGetMagicItem;
	m_stPetOption.m_bGetRareItem = stPetOptionData.m_bGetRareItem;
	m_stPetOption.m_bGetEpicItem = stPetOptionData.m_bGetEpicItem;
	m_stPetOption.m_bGetUniqueItem = stPetOptionData.m_bGetUniqueItem;
	m_stPetOption.m_bGetLegendItem = stPetOptionData.m_bGetLegendItem;
//	m_stPetOption.m_bAutoUseHPPotion = stPetOptionData.m_bAutoUseHPPotion;
//	m_stPetOption.m_bAutoUseMPPotion = stPetOptionData.m_bAutoUseMPPotion;
//	m_stPetOption.m_nAutoUseHPPotionPercent = stPetOptionData.m_nAutoUseHPPotionPercent;
//	m_stPetOption.m_nAutoUseMPPotionPercent = stPetOptionData.m_nAutoUseMPPotionPercent;
	m_stPetOption.m_nFoodItemID = stPetOptionData.m_nFoodItemID;
	m_stPetOption.m_bAutoFeed = stPetOptionData.m_bAutoFeed;
	m_stPetOption.m_nAutoFeedPercent = stPetOptionData.m_nAutoFeedPercent;

	FILE *stream = NULL;
	char szFilename[MAX_PATH];

#ifndef _FINAL_BUILD
	sprintf_s( szFilename, 256, ".\\Config_Pet.ini" );
#else
	TCHAR pBuffer[MAX_PATH]={0};
	char szPath[_MAX_PATH] = { 0, };
	SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), pBuffer, CSIDL_PERSONAL, 0);
	WideCharToMultiByte( CP_ACP, 0, pBuffer, -1, szPath, sizeof(szPath), NULL, NULL );

	sprintf_s( szFilename, "%s\\DragonNest", szPath );
	_mkdir( szFilename );
	sprintf_s( szFilename, "%s\\DragonNest\\Config", szPath );
	_mkdir( szFilename );
	sprintf_s( szFilename, "%s\\DragonNest\\Config\\%s", szPath, "Config_Pet.ini" );
#endif

	if( fopen_s( &stream, szFilename, "w+" ) != 0 )
		return;

	fseek( stream, 0L, SEEK_SET );

	fprintf( stream, "GetItem %d\n", m_stPetOption.m_bGetItem ? 1 : 0 );
	fprintf( stream, "GetNormalItem %d\n", m_stPetOption.m_bGetNormalItem ? 1 : 0 );
	fprintf( stream, "GetMagicItem %d\n", m_stPetOption.m_bGetMagicItem ? 1 : 0 );
	fprintf( stream, "GetRareItem %d\n", m_stPetOption.m_bGetRareItem ? 1 : 0 );
	fprintf( stream, "GetEpicItem %d\n", m_stPetOption.m_bGetEpicItem ? 1 : 0 );
	fprintf( stream, "GetUniqueItem %d\n", m_stPetOption.m_bGetUniqueItem ? 1 : 0 );
	fprintf( stream, "GetLegendItem %d\n", m_stPetOption.m_bGetLegendItem ? 1 : 0 );
//	fprintf( stream, "AutoUseHPPotion %d\n", m_stPetOption.m_bAutoUseHPPotion ? 1 : 0 );
//	fprintf( stream, "AutoUseMPPotion %d\n", m_stPetOption.m_bAutoUseMPPotion ? 1 : 0 );
//	fprintf( stream, "AutoUseHPPotionPercent %d\n", m_stPetOption.m_nAutoUseHPPotionPercent );
//	fprintf( stream, "AutoUseMPPotionPercent %d\n", m_stPetOption.m_nAutoUseMPPotionPercent );
	fprintf( stream, "FoodItem %d\n", m_stPetOption.m_nFoodItemID );
	fprintf( stream, "AutoFeed %d\n", m_stPetOption.m_bAutoFeed ? 1 : 0 );
	fprintf( stream, "AutoFeedPercent %d\n", m_stPetOption.m_nAutoFeedPercent );

	fclose( stream );
}

bool CDnPetTask::CheckCanMovePet( bool bSummonedOff )
{
	if( !CDnActor::s_hLocalActor ) return false;
	CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());
	if( !pActor ) return false;
	bool bCanMovePet = true;
	if( pActor->IsHit() || pActor->IsAir() || pActor->IsDown() || pActor->IsAttack() ) bCanMovePet = false;
	if( !pActor->IsCanToggleBattleMode() ) bCanMovePet = false;
	if( CTaskManager::IsActive() ) 
	{
		CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
		if( pTask && CDnPartyTask::IsActive() && !CDnPartyTask::GetInstance().IsSyncComplete() ) 
		{
			bCanMovePet = false;
		}
	}

	if( !bCanMovePet ) 
	{
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, ( bSummonedOff ) ? 9262 : 9261 ), false );
	}
	
	return bCanMovePet;
}

void CDnPetTask::RequestPetNaming(INT64 itemSerial, INT64 petSerial, const wchar_t* pName)
{
	CSChangePetName changePetName;
	changePetName.itemSerial = itemSerial;
	changePetName.petSerial = petSerial;
	_wcscpy(changePetName.name, _countof(changePetName.name), pName, (int)wcslen(pName));

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_CHANGEPETNAME, (char*)&changePetName, int(sizeof(CSChangePetName)));
}

void CDnPetTask::OnRecvPetNaming( SCChangePetName* pPacket )
{
	_ASSERT( pPacket );

	if( pPacket->nRet == ERROR_NONE )
	{
		if (CDnActor::s_hLocalActor)
		{
			CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if (!pPlayerActor)
				return;

			if (pPacket->nUserSessionID == pPlayerActor->GetUniqueID())
			{
				TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( pPacket->biItemSerial );
				if( pPetCompact )
					_wcscpy( pPetCompact->wszNickName, _countof(pPetCompact->wszNickName), pPacket->wszPetName, (int)wcslen(pPacket->wszPetName) );

				CDnPetActor* pPetActor = pPlayerActor->GetMyPetActor();
				if (pPetActor && pPetActor->GetPetInfo().Vehicle->nSerial == pPacket->biItemSerial)
				{
					pPetActor->SetName(pPacket->wszPetName);
					_wcscpy(pPetActor->GetPetInfo().wszNickName, _countof(pPetActor->GetPetInfo().wszNickName), pPacket->wszPetName, (int)wcslen(pPacket->wszPetName));
					_wcscpy(pPlayerActor->GetPetInfo().wszNickName, _countof(pPlayerActor->GetPetInfo().wszNickName), pPacket->wszPetName, (int)wcslen(pPacket->wszPetName));

					CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::CHARSTATUS_DIALOG);
					if (pCharStatusDlg)
					{
						CDnCharPetDlg* pCharPetDlg = pCharStatusDlg->GetPetDlg();
						if(pCharPetDlg)
							pCharPetDlg->SetPetInfoDetail();
					}
				}
				else
				{
					CDnItem* pPetItem = GetItemTask().GetPetInventory().FindItemFromSerialID( pPacket->biItemSerial );
					if( pPetItem )
						UsePetItem( pPetItem );
				}

				CDnPetNamingDlg* pPetNamingDlg = GetInterface().GetPetNamingDlg();
				if( pPetNamingDlg )
					pPetNamingDlg->Show( false );
			}
			else
			{
				DnActorHandle actor = CDnActor::FindActorFromUniqueID(pPacket->nUserSessionID);
				if (actor)
				{
					CDnPlayerActor* pOtherActor = dynamic_cast<CDnPlayerActor*>(actor.GetPointer());
					if (pOtherActor)
					{
						CDnPetActor* pPetActor = pOtherActor->GetMyPetActor();
						if (pPetActor)
							pPetActor->SetName(pPacket->wszPetName);
					}
				}
			}
		}
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}

void CDnPetTask::OnRecvPetEquipList( CDnPlayerActor* pPlayer, TVehicleCompact& stPetEquipInfo )
{
	if( pPlayer == NULL ) return;

	bool bLocalPlayer = false;
	if( CDnActor::s_hLocalActor )
	{
		CDnPlayerActor* pLocalPlayer = dynamic_cast<CDnPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
		if( pPlayer->GetUniqueID() == pLocalPlayer->GetUniqueID() ) 
			bLocalPlayer = true;
	}
	
//	const time_t pNowTime = CSyncTimer::GetInstance().GetCurTime();
//	if( pNowTime >= stPetEquipInfo.Vehicle[Vehicle::Slot::VehicleBody].tExpireDate )	// 만료기간 지났는지 체크
//	{
//		if( pPlayer->IsSummonPet() )
//			pPlayer->SummonPet( -1 );
//		return;
//	}

	pPlayer->SetPetInfo( stPetEquipInfo );

	bool bPetSummonEnable = true;
	if( !pPlayer->IsInPetSummonableMap() )	// 소환불가 지역에 들어왔는데 이미 소환한 펫이 있는 경우 Disable처리
	{
		bPetSummonEnable = false;
		if( bLocalPlayer )
		{
			if( stPetEquipInfo.nSkillID1 > 0 )
				RemovePetSkill( stPetEquipInfo.nSkillID1, true );
			if( stPetEquipInfo.nSkillID2 > 0 )
				RemovePetSkill( stPetEquipInfo.nSkillID2, true );
		}
		else
		{
			if( stPetEquipInfo.nSkillID1 > 0 )
				pPlayer->RemoveSkill( stPetEquipInfo.nSkillID1 );
			if( stPetEquipInfo.nSkillID2 > 0 )
				pPlayer->RemoveSkill( stPetEquipInfo.nSkillID2 );
		}
	}

	if( bPetSummonEnable && stPetEquipInfo.Vehicle[Pet::Slot::Body].nItemID > 0 )
	{
		int nPetLevelTableID = GetPetTask().GetPetLevelTableIndex( pPlayer->GetUniqueID() );

		pPlayer->SummonPet( stPetEquipInfo.Vehicle[Pet::Slot::Body].nItemID, stPetEquipInfo.wszNickName );
		if( bLocalPlayer )
			GetPetTask().LoadPetConfig();

		if( !pPlayer->IsSummonPet() )	// 펫 소환 실패시 파츠 장착은 패스
			return;

		for( int i=0; i<Pet::Slot::Max; i++ )
		{
			if( ( i == Pet::Slot::Accessory1 || i == Pet::Slot::Accessory2 ) && 
				stPetEquipInfo.Vehicle[i].nItemID > 0 )	// 장비 착용
			{
				pPlayer->GetMyPetActor()->EquipItem( stPetEquipInfo.Vehicle[i] );	// 장비 착용
				pPlayer->GetPetInfo().Vehicle[i] = stPetEquipInfo.Vehicle[i];		// 펫 정보 갱신
			}

			if( bLocalPlayer )
			{
				CDnItem *Item_Temp = GetItemTask().CreateItem( stPetEquipInfo.Vehicle[i] );
				if( Item_Temp )
				{
					Item_Temp->SetSlotIndex( i );
					GetItemTask().InsertPetItem( Item_Temp );
				}
			}
		}
	}

	CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( pCharStatusDlg )
	{
		CDnCharPetDlg* pCharPetDlg = pCharStatusDlg->GetPetDlg();
		if( pCharPetDlg )
		{
			pCharPetDlg->SetPetInfoDetail();
			pCharPetDlg->SetEnableMode( bPetSummonEnable );
		}
	}
}

void CDnPetTask::SendPetExtendPeriod( INT64 iExtendPeriodItemSerial, INT64 iItemSerial, int nExtendPeriod )
{
	CSModItemExpireDate stModItemExpireDate;
	stModItemExpireDate.biExpireDateItemSerial = iExtendPeriodItemSerial;
	stModItemExpireDate.biItemSerial = iItemSerial;	

	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_MODITEMEXPIREDATE, (char*)&stModItemExpireDate, int(sizeof(stModItemExpireDate)) );
}

int CDnPetTask::GetPetLevelTableIndex( int nSessionID )
{
	if( !CDnActor::s_hLocalActor ) return -1;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL ) return -1;

	CDnPlayerActor* pPlayer = NULL;
	if( pLocalPlayer->GetUniqueID() == nSessionID )
	{
		pPlayer = pLocalPlayer;
	}
	else
	{
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>( hActor.GetPointer() );
	}
	if( pPlayer == NULL ) return -1;

	TVehicleCompact* pPetCompact = &pPlayer->GetPetInfo();
	if( pPetCompact == NULL ) return -1;

	DNTableFileFormat* pSoxPet = GetDNTable( CDnTableDB::TVEHICLE );
	DNTableFileFormat* pSoxPetLevel = GetDNTable( CDnTableDB::TPETLEVEL );
	if( pSoxPet == NULL || pSoxPetLevel == NULL )
	{
		ErrorLog( "Pet table Not found!!" );
		return -1;
	}

	int nPetLevelTableID = -1;
	int nPetLevelTypeID = pSoxPet->GetFieldFromLablePtr( pPetCompact->Vehicle[Pet::Slot::Body].nItemID, "_PetLevelTypeID" )->GetInteger();
	for( int i=0; i<pSoxPetLevel->GetItemCount(); ++i )
	{
		int nTableID = pSoxPetLevel->GetItemID( i );
		if( nPetLevelTypeID == pSoxPetLevel->GetFieldFromLablePtr( nTableID, "_PetLevelTypeID" )->GetInteger() )
		{
			int nExp = pSoxPetLevel->GetFieldFromLablePtr( nTableID, "_PetExp" )->GetInteger();
			if( pPetCompact->nExp < nExp )
			{
				nPetLevelTableID = nTableID;
				break;
			}
			else
				nPetLevelTableID = nTableID;	// MAX레밸일 경우
		}
	}

	return nPetLevelTableID;
}

int CDnPetTask::GetPetLevel( INT64 nSerialID )
{
	TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( nSerialID );
	if( pPetCompact == NULL ) return -1;

	DNTableFileFormat* pSoxPet = GetDNTable( CDnTableDB::TVEHICLE );
	DNTableFileFormat* pSoxPetLevel = GetDNTable( CDnTableDB::TPETLEVEL );
	if( pSoxPet == NULL || pSoxPetLevel == NULL )
	{
		ErrorLog( "Pet table Not found!!" );
		return -1;
	}

	int nPetLevelTableID = -1;
	int nPetLevelTypeID = pSoxPet->GetFieldFromLablePtr( pPetCompact->Vehicle[Pet::Slot::Body].nItemID, "_PetLevelTypeID" )->GetInteger();
	for( int i=0; i<pSoxPetLevel->GetItemCount(); ++i )
	{
		int nTableID = pSoxPetLevel->GetItemID( i );
		if( nPetLevelTypeID == pSoxPetLevel->GetFieldFromLablePtr( nTableID, "_PetLevelTypeID" )->GetInteger() )
		{
			int nExp = pSoxPetLevel->GetFieldFromLablePtr( nTableID, "_PetExp" )->GetInteger();
			if( pPetCompact->nExp < nExp )
			{
				nPetLevelTableID = nTableID;
				break;
			}
			else
				nPetLevelTableID = nTableID;	// MAX레밸일 경우
		}
	}

	int nPetLevel = pSoxPetLevel->GetFieldFromLablePtr( nPetLevelTableID, "_PetLevel" )->GetInteger();

	return nPetLevel;
}

int CDnPetTask::GetPetMaxLevelExp( int nPetItemID )
{
	DNTableFileFormat* pSoxPet = GetDNTable( CDnTableDB::TVEHICLE );
	DNTableFileFormat* pSoxPetLevel = GetDNTable( CDnTableDB::TPETLEVEL );
	if( pSoxPet == NULL || pSoxPetLevel == NULL )
	{
		ErrorLog( "Pet table Not found!!" );
		return -1;
	}

	int nPetMaxLevelExp = 0;
	int nPetLevelTypeID = pSoxPet->GetFieldFromLablePtr( nPetItemID, "_PetLevelTypeID" )->GetInteger();
	for( int i=0; i<pSoxPetLevel->GetItemCount(); ++i )
	{
		int nTableID = pSoxPetLevel->GetItemID( i );
		if( nPetLevelTypeID == pSoxPetLevel->GetFieldFromLablePtr( nTableID, "_PetLevelTypeID" )->GetInteger() &&
			pSoxPetLevel->GetFieldFromLablePtr( nTableID, "_PetExp" )->GetInteger() != 0 )
			nPetMaxLevelExp = pSoxPetLevel->GetFieldFromLablePtr( nTableID, "_PetExp" )->GetInteger();
	}

	return nPetMaxLevelExp;
}

bool CDnPetTask::IsPetSkill( int nSkillTableID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPETSKILLLEVEL );
	if( pSox == NULL )
	{
		DN_ASSERT( 0, "Invalid PetSKillLevel" );
		return false;
	}

	bool bIsPetSkill = false;
	for( int i=0; i < pSox->GetItemCount(); i++ )
	{
		int nItemID = pSox->GetItemID( i );
		int nSkillIndex = pSox->GetFieldFromLablePtr( nItemID, "_SkillIndex" )->GetInteger();

		if( nSkillTableID == nSkillIndex )
		{
			bIsPetSkill = true;
			break;
		}
	}

	return bIsPetSkill;
}

void CDnPetTask::OnRecvPetSkillItem( SCPetSkillItem* pPacket )
{
	_ASSERT( pPacket );

	if( pPacket->nRet != ERROR_NONE )
		return;

	if( pPacket->cSlotNum != 1 && pPacket->cSlotNum != 2 )
		return;

	if( pPacket->nSkillID <= 0 )
		return;

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL ) return;

	TVehicleCompact* pPetCompact = &pLocalPlayer->GetPetInfo();
	if( pPetCompact == NULL || pPetCompact->Vehicle[Pet::Slot::Body].nItemID == 0 ) return;

	if( pPacket->cSlotNum == 1 )
		pPetCompact->nSkillID1 = pPacket->nSkillID;
	else if( pPacket->cSlotNum == 2 )
		pPetCompact->nSkillID2 = pPacket->nSkillID;

	pLocalPlayer->SetPetInfo( *pPetCompact );

	if( pLocalPlayer->IsInPetSummonableMap() )
	{
		if( pPetCompact->nType & Pet::Type::ePETTYPE_SATIETY )
		{
			if( ( m_nSatietySelectionUseSkillNum[GetSatietySelection()] == 1 && pPacket->cSlotNum == 1 ) 
				|| ( m_nSatietySelectionUseSkillNum[GetSatietySelection()] == 2 ) )
			{
				pLocalPlayer->AddSkill( pPacket->nSkillID, 1 );
			}
		}
		else
		{
			pLocalPlayer->AddSkill( pPacket->nSkillID, 1 );
		}
	}

	CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( pCharStatusDlg )
	{
		CDnCharPetDlg* pCharPetDlg = pCharStatusDlg->GetPetDlg();
		if( pCharPetDlg )
			pCharPetDlg->RefreshPetEquip();
	}
}

void CDnPetTask::OnRecvPetSkillExpand( SCPetSkillExpand* pPacket )
{
	_ASSERT( pPacket );

	if( pPacket->nRet != ERROR_NONE )
		return;

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL ) return;

	TVehicleCompact* pPetCompact = &pLocalPlayer->GetPetInfo();
	if( pPetCompact == NULL || pPetCompact->Vehicle[Pet::Slot::Body].nItemID == 0 ) return;

	pPetCompact->bSkillSlot = true;
	pLocalPlayer->SetPetInfo( *pPetCompact );

	CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( pCharStatusDlg )
	{
		CDnCharPetDlg* pCharPetDlg = pCharStatusDlg->GetPetDlg();
		if( pCharPetDlg )
			pCharPetDlg->RefreshPetEquip();
	}

	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9252 ), false );
}

void CDnPetTask::RequestRemovePetSkill( int nSlotIndex, CDnSkill* pSkillItem )
{
	if( pSkillItem == NULL )
		return;

	m_nRemoveSlotIndex = nSlotIndex;

	WCHAR wszMessage[256];
	swprintf_s( wszMessage, _countof(wszMessage), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9257 ), pSkillItem->GetName() );
	GetInterface().MessageBox( wszMessage, MB_YESNO, MSGBOX_REQUEST_REMOVE_PETSKILL, this, true, true );
}

void CDnPetTask::RemovePetSkill( int nSkillID, bool bForceRemove )
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL ) return;

	DnSkillHandle hSkill = pLocalPlayer->FindSkill( nSkillID );
	if( hSkill == NULL || hSkill->GetCoolTime() == 0.0f )
	{
		pLocalPlayer->RemoveSkill( nSkillID );
	}
	else
	{
		std::map<int, stRemoveSkillInfo>::iterator iter = m_mapRemoveSkillQueue.find( nSkillID );
		if( iter != m_mapRemoveSkillQueue.end() )
		{
			stRemoveSkillInfo& pstRemoveSkillInfo = (*iter).second;
			pstRemoveSkillInfo.bForceRemove = true;
			return;
		}

		stRemoveSkillInfo stSkillInfo;
		stSkillInfo.hSkillHandle = hSkill;
		stSkillInfo.nSessionID = pLocalPlayer->GetUniqueID();
		stSkillInfo.bForceRemove = bForceRemove;
		m_mapRemoveSkillQueue.insert( make_pair( hSkill->GetClassID(), stSkillInfo ) );
	}
}

void CDnPetTask::OnRecvRemovePetSkillItem( SCPetSkillDelete* pPacket )
{
	_ASSERT( pPacket );

	if( pPacket->nRet != ERROR_NONE )
		return;

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL ) return;

	TVehicleCompact* pPetCompact = &pLocalPlayer->GetPetInfo();
	if( pPetCompact == NULL || pPetCompact->Vehicle[Pet::Slot::Body].nItemID == 0 ) return;

	if( pPacket->cSlotNum == 1 )
	{
		RemovePetSkill( pPetCompact->nSkillID1 );
		pPetCompact->nSkillID1 = 0;
	}
	else if( pPacket->cSlotNum == 2 )
	{
		RemovePetSkill( pPetCompact->nSkillID2 );
		pPetCompact->nSkillID2 = 0;
	}

	GetInterface().GetMainBarDialog()->OnRefreshQuickSlot();

	pLocalPlayer->SetPetInfo( *pPetCompact );

	CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( pCharStatusDlg )
	{
		CDnCharPetDlg* pCharPetDlg = pCharStatusDlg->GetPetDlg();
		if( pCharPetDlg )
			pCharPetDlg->RefreshPetEquip();
	}
}

bool CDnPetTask::CheckRemovePetSkill( int nSkillID )
{
	bool bRemovePetSkill = false;
	std::map<int, stRemoveSkillInfo>::iterator iter = m_mapRemoveSkillQueue.begin();

	for( ; iter!=m_mapRemoveSkillQueue.end(); )
	{
		stRemoveSkillInfo stSkillInfo = iter->second;
		if( !stSkillInfo.bForceRemove )
		{
			DnActorHandle hActor = CDnActor::FindActorFromUniqueID( stSkillInfo.nSessionID );
			if( !hActor ) continue;
			CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor *>( hActor.GetPointer() );
			if( pPlayer && ( pPlayer->GetPetInfo().nSkillID1 == nSkillID || pPlayer->GetPetInfo().nSkillID2 == nSkillID ) )
			{
				iter = m_mapRemoveSkillQueue.erase( iter );
				continue;
			}
		}
		iter++;
	}

	for( iter = m_mapRemoveSkillQueue.begin(); iter!=m_mapRemoveSkillQueue.end(); )
	{
		stRemoveSkillInfo stSkillInfo = iter->second;
		if( !stSkillInfo.hSkillHandle )
		{
			iter = m_mapRemoveSkillQueue.erase( iter );
			continue;
		}
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( stSkillInfo.nSessionID );
		if( !hActor )
		{
			iter = m_mapRemoveSkillQueue.erase( iter );
			continue;
		}

		if( nSkillID == stSkillInfo.hSkillHandle->GetClassID() )
		{
			CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor *>( hActor.GetPointer() );
			if( pPlayer && stSkillInfo.hSkillHandle )
			{
				pPlayer->RemoveSkill( iter->first );
				iter = m_mapRemoveSkillQueue.erase( iter );
				bRemovePetSkill = true;
			}
			continue;
		}
		iter++;
	}

	return bRemovePetSkill;
}

void CDnPetTask::OnRecvPetExtendPeriod( SCModItemExpireDate* pPacket )
{
	_ASSERT( pPacket );

	CDnPetExtendPeriodDlg* pPetExtendPeriodDlg = GetInterface().GetPetExtendPeriodDlg();
	if( pPetExtendPeriodDlg && pPetExtendPeriodDlg->IsShow() )
		pPetExtendPeriodDlg->Show( false );

#ifdef PRE_ADD_PET_EXTEND_PERIOD
	CDnBuyPetExtendPeriodDlg* pBuyPetExtendPeriodDlg = GetInterface().GetBuyPetExtendPeriodDlg();
	if( pBuyPetExtendPeriodDlg && pBuyPetExtendPeriodDlg->IsShow() )
		pBuyPetExtendPeriodDlg->Show( false );
#endif // PRE_ADD_PET_EXTEND_PERIOD

	if( pPacket->nRet == ERROR_NONE )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9273 ), MB_OK );
	}
	else
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9274 ), MB_OK );
	}
}

void CDnPetTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( CDnActor::s_hLocalActor == NULL )
		return;

	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL || pLocalPlayer->GetMyPetActor() == NULL || !pLocalPlayer->IsSummonPet() )
		return;

	if( m_fCheckCoolTime > 0.0f )
	{
		m_fCheckCoolTime -= fDelta;
	}
	else
	{
		m_fCheckCoolTime = 0.2f;
		if( pLocalPlayer->GetPetInfo().nType & Pet::Type::ePETTYPE_SATIETY && m_nMaxSatiety > 0 )
			CheckSatiety();

		CheckPetChat( fDelta );
	}
}

void CDnPetTask::OnRecvCurrentSatiety( SCPetCurrentSatiety* pPacket )
{
	if( pPacket == NULL ) return;
	if(!CDnActor::s_hLocalActor) return;

	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL || pLocalPlayer->GetMyPetActor() == NULL || !pLocalPlayer->IsSummonPet() ) 
		return;

	TVehicleCompact* pPetCompact = &pLocalPlayer->GetPetInfo();
	if( pPetCompact == NULL || pPetCompact->Vehicle[Pet::Slot::Body].nSerial != pPacket->biPetSerial )
		return;

	pPetCompact->nCurrentSatiety = pPacket->nCurrentSatiety;
	SetPetSatietyInfo( pPetCompact->Vehicle[Pet::Slot::Body].nItemID, pPacket->nCurrentSatiety );
	CheckSatiety();

	pLocalPlayer->RefreshState( CDnActorState::RefreshEquip );
}

void CDnPetTask::OnRecvPetFoodEat( SCPetFoodEat* pPacket )
{
	if( pPacket == NULL ) return;

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL || pLocalPlayer->GetMyPetActor() == NULL || !pLocalPlayer->IsSummonPet() ) 
		return;

	if( pPacket->nResult == ERROR_NONE )
	{
		WCHAR wszStr[256];
		wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9268 ), pPacket->nIncSatiety );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
		DoPetChat( PET_CHAT_EAT );

		pLocalPlayer->SetPetEffectAction( "Give_PetFood" );
	}
	else
	{
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9269 ), false );
		DoPetChat( PET_CHAT_EATFAIL );
	}

	m_bRequestUsePetFood = false;
}

void CDnPetTask::SetPetSatietyInfo( int nPetItemID, int nCurrentSatiety )
{
	DNTableFileFormat* pPetTable = GetDNTable( CDnTableDB::TVEHICLE );
	if( pPetTable == NULL ) return;

	int nFoodID = pPetTable->GetFieldFromLablePtr( nPetItemID, "_FoodID" )->GetInteger();
	if( nFoodID == 0 )
		return;

	DNTableFileFormat* pPetFoodTable = GetDNTable( CDnTableDB::TPETFOOD );
	if( pPetFoodTable == NULL ) return;

	m_nMaxSatiety = pPetFoodTable->GetFieldFromLablePtr( nFoodID, "_FullMaxCount" )->GetInteger();

	if( nCurrentSatiety > m_nMaxSatiety )
		nCurrentSatiety = m_nMaxSatiety;

	m_nCurrentSatiety = nCurrentSatiety;

	if( m_nMaxSatiety != 0 )
		m_fSatietyPercent = static_cast<float>( m_nCurrentSatiety ) / static_cast<float>( m_nMaxSatiety ) * 100.0f;
	else
		m_fSatietyPercent = 0;

	if( m_fSatietyPercent < 0.0f )
		m_fSatietyPercent = 0.0f;
	else if( m_fSatietyPercent > 100.0f )
		m_fSatietyPercent = 100.0f;

	memset( m_nSatietySelectionPickup, 0, sizeof( m_nSatietySelectionPickup ) );
	memset( m_nSatietySelectionApplyStatePercent, 0, sizeof( m_nSatietySelectionApplyStatePercent ) );
	memset( m_nSatietySelectionUseSkillNum, 0, sizeof( m_nSatietySelectionUseSkillNum ) );

	char szLabel[32];
	for( int i=0; i<Pet::MAX_SATIETY_SELECTION; i++ )
	{
		sprintf_s( szLabel, "_Usepick%d", i+1 );
		m_nSatietySelectionPickup[i] = pPetFoodTable->GetFieldFromLablePtr( nFoodID, szLabel )->GetInteger();
		sprintf_s( szLabel, "_Usestate%d", i+1 );
		m_nSatietySelectionApplyStatePercent[i] = pPetFoodTable->GetFieldFromLablePtr( nFoodID, szLabel )->GetInteger();
		sprintf_s( szLabel, "_Useskill%d", i+1 );
		m_nSatietySelectionUseSkillNum[i] = pPetFoodTable->GetFieldFromLablePtr( nFoodID, szLabel )->GetInteger();
	}

	CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( pCharStatusDlg )
	{
		CDnCharPetDlg* pCharPetDlg = pCharStatusDlg->GetPetDlg();
		if( pCharPetDlg && pCharPetDlg->IsShow() ) pCharPetDlg->SetSatietyInfo();
	}
}

int CDnPetTask::GetSatietySelection()
{
	int nSelection = 0;
	if( 0 <= m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection0 )
	{
		nSelection = 0;
	}
	else if( Pet::Satiety::SatietySelection0 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection1 )
	{
		nSelection = 1;
	}
	else if( Pet::Satiety::SatietySelection1 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection2 )
	{
		nSelection = 2;
	}
	else if( Pet::Satiety::SatietySelection2 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection3 )
	{
		nSelection = 3;
	}
	else if( Pet::Satiety::SatietySelection3 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection4 )
	{
		nSelection = 4;
	}

	return nSelection;
}

float CDnPetTask::GetSatietyApplyStateRatio()
{
	float fRatio = 0.0f;
	if( m_nSatietySelectionApplyStatePercent[GetSatietySelection()] > 0 )
		fRatio = static_cast<float>( m_nSatietySelectionApplyStatePercent[GetSatietySelection()] ) / 100.0f;

	return fRatio;
}

bool CDnPetTask::EnableSatietyPickup()
{
	bool bEnablePickup = false;
	if( m_nSatietySelectionPickup[GetSatietySelection()] )
		bEnablePickup = true;

	return bEnablePickup;
}

void CDnPetTask::CheckSatiety()
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL )
		return;

	TVehicleCompact* pPetCompact = &pLocalPlayer->GetPetInfo();

	CDnCharPetDlg* pCharPetDlg = NULL;
	CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( pCharStatusDlg )
		pCharPetDlg = pCharStatusDlg->GetPetDlg();

	if( pCharStatusDlg == NULL ) return;

	if( m_nSatietySelectionUseSkillNum[GetSatietySelection()] == 0 )
	{
		// 펫 스킬 2개 모두 제거
		if( pPetCompact->nSkillID1 > 0 )
		{
			RemovePetSkill( pPetCompact->nSkillID1, true );
			pCharPetDlg->DisablePetSkill( 1 );
		}
		if( pPetCompact->nSkillID2 > 0 )
		{
			RemovePetSkill( pPetCompact->nSkillID2, true );
			pCharPetDlg->DisablePetSkill( 2 );
		}
	}
	else if( m_nSatietySelectionUseSkillNum[GetSatietySelection()] == 1 )
	{
		// 펫 스킬 1개만 사용
		if( pPetCompact->nSkillID1 > 0 )
		{
			pLocalPlayer->AddSkill( pPetCompact->nSkillID1, 1 );
			pCharPetDlg->EnablePetSkill( 1 );
		}
		if( pPetCompact->nSkillID2 > 0 )
		{
			RemovePetSkill( pPetCompact->nSkillID2, true );
			pCharPetDlg->DisablePetSkill( 2 );
		}
	}
	else if( m_nSatietySelectionUseSkillNum[GetSatietySelection()] == 2 )
	{
		// 펫 스킬 2개 모두 사용
		if( pPetCompact->nSkillID1 > 0 )
		{
			pLocalPlayer->AddSkill( pPetCompact->nSkillID1, 1 );
			pCharPetDlg->EnablePetSkill( 1 );
		}
		if( pPetCompact->nSkillID2 > 0 )
		{
			pLocalPlayer->AddSkill( pPetCompact->nSkillID2, 1 );
			pCharPetDlg->EnablePetSkill( 2 );
		}
	}

	if( !m_bNotifySatietyHalf && m_fSatietyPercent <= Pet::Satiety::SatietySelection2 )
	{
		m_bNotifySatietyHalf = true;
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9270 ), false );
	}

	if( m_bNotifySatietyHalf && m_fSatietyPercent > Pet::Satiety::SatietySelection2 )
		m_bNotifySatietyHalf = false;

	if( !m_bNotifySatietyEnd && m_fSatietyPercent <= Pet::Satiety::SatietySelection0 )
	{
		m_bNotifySatietyEnd = true;
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9271 ), false );
	}

	if( m_bNotifySatietyEnd && m_fSatietyPercent > Pet::Satiety::SatietySelection0 )
		m_bNotifySatietyEnd = false;

	if( m_fSatietyPercent == 0.0f )
	{
		DoPetChat( PET_CHAT_SATIETY_ZERO );
	}
	else
	{
		DoNotPetChat( PET_CHAT_SATIETY_ZERO );
		if( m_fSatietyPercent < 50.0f )
			DoPetChat( PET_CHAT_STARVE );
		else
			DoNotPetChat( PET_CHAT_STARVE );
	}

	CheckAutoFeeding();
}

void CDnPetTask::DoPetChat( ePetChatType chatType )
{
	if( CDnActor::s_hLocalActor == NULL )
		return;

	CDnPlayerActor* pLocalPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL || pLocalPlayer->GetMyPetActor() == NULL || !pLocalPlayer->IsSummonPet() ) 
		return;
	
	std::map<int, stPetChat>::iterator iter = m_mapPetChat.find( chatType );
	if( iter != m_mapPetChat.end() )
		return;

	TVehicleCompact* pPetCompact = &pLocalPlayer->GetPetInfo();

	DNTableFileFormat* pPetTable = GetDNTable( CDnTableDB::TVEHICLE );
	if( pPetTable == NULL ) 
		return;

	DNTableFileFormat* pPetChatTable = GetDNTable( CDnTableDB::TPETCHAT );
	if( pPetChatTable == NULL ) 
		return;

	int nPetChatID = pPetTable->GetFieldFromLablePtr( pPetCompact->Vehicle[Pet::Slot::Body].nItemID, "_PetChatID" )->GetInteger();
	if( nPetChatID == 0 )
		return;

	int nPetLevel = 0;
	int nPetLevelTableID = GetPetTask().GetPetLevelTableIndex( pLocalPlayer->GetUniqueID() );
	if( nPetLevelTableID != -1 )
	{
		DNTableFileFormat* pSoxPetLevel = GetDNTable( CDnTableDB::TPETLEVEL );
		if( pSoxPetLevel )
		{
			nPetLevel = pSoxPetLevel->GetFieldFromLablePtr( nPetLevelTableID, "_PetLevel" )->GetInteger();
		}
	}

	// 전체 Prob 합산 구함
	int nTotalProb = 0;
	for( int i=0; i<pPetChatTable->GetItemCount(); ++i )
	{
		int nTableID = pPetChatTable->GetItemID( i );
		int nChatID = pPetChatTable->GetFieldFromLablePtr( nTableID, "_PetChatID" )->GetInteger();
		int nEventType = pPetChatTable->GetFieldFromLablePtr( nTableID, "_EventType" )->GetInteger();
		int nMinLevel = pPetChatTable->GetFieldFromLablePtr( nTableID, "_MinLevel" )->GetInteger();
		int nMaxLevel = pPetChatTable->GetFieldFromLablePtr( nTableID, "_MaxLevel" )->GetInteger();

		if( nPetChatID == nChatID && nMinLevel <= nPetLevel && nPetLevel <= nMaxLevel && nEventType == chatType )
		{
			nTotalProb += pPetChatTable->GetFieldFromLablePtr( nTableID, "_Prob" )->GetInteger();
		}
	}

	if( nTotalProb == 0 ) return;

	for( int i=0; i<pPetChatTable->GetItemCount(); ++i )
	{
		int nTableID = pPetChatTable->GetItemID( i );
		int nChatID = pPetChatTable->GetFieldFromLablePtr( nTableID, "_PetChatID" )->GetInteger();
		int nEventType = pPetChatTable->GetFieldFromLablePtr( nTableID, "_EventType" )->GetInteger();
		int nMinLevel = pPetChatTable->GetFieldFromLablePtr( nTableID, "_MinLevel" )->GetInteger();
		int nMaxLevel = pPetChatTable->GetFieldFromLablePtr( nTableID, "_MaxLevel" )->GetInteger();

		if( nPetChatID == nChatID && nMinLevel <= nPetLevel && nPetLevel <= nMaxLevel && nEventType == chatType )
		{
			stPetChatProb petChatProb;
			petChatProb.nTableID = nTableID;
			int nProb = pPetChatTable->GetFieldFromLablePtr( nTableID, "_Prob" )->GetInteger();
			petChatProb.fProb = static_cast<float>( nProb ) / static_cast<float>( nTotalProb ) * 100.0f;

			std::map<int, stPetChat>::iterator iter = m_mapPetChat.find( nEventType );
			if( iter == m_mapPetChat.end() )
			{
				stPetChat petChat;
				petChat.fChatCoolTime = static_cast<float>( pPetChatTable->GetFieldFromLablePtr( nTableID, "_ChatTime" )->GetInteger() );
				petChat.tStartTime = CTimeSet().GetTimeT64_GM();
				petChat.vecPetChatProb.push_back( petChatProb );
				m_mapPetChat.insert( make_pair( nEventType, petChat ) );
			}
			else
			{
				stPetChat& petChat = (*iter).second;
				petChat.vecPetChatProb.push_back( petChatProb );
			}
		}
	}
}

void CDnPetTask::DoNotPetChat( ePetChatType chatType )
{
	std::map<int, stPetChat>::iterator iter = m_mapPetChat.find( chatType );
	if( iter != m_mapPetChat.end() )
	{
		m_mapPetChat.erase( iter );
	}
}

void CDnPetTask::CheckPetChat( float fDelta )
{
	CDnPlayerActor* pLocalPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer == NULL ) 
		return;

	DNTableFileFormat* pPetChatTable = GetDNTable( CDnTableDB::TPETCHAT );
	if( pPetChatTable == NULL ) 
		return;

	std::map<int, stPetChat>::iterator iter = m_mapPetChat.begin();
	for( ; iter != m_mapPetChat.end(); iter++ )
	{
		stPetChat& petChat = (*iter).second;
		float fTest = (float)((CTimeSet().GetTimeT64_GM() - (float)petChat.tStartTime));

		if( (float)((CTimeSet().GetTimeT64_GM() - (float)petChat.tStartTime)) >= petChat.fChatCoolTime )
		{
			petChat.tStartTime = CTimeSet().GetTimeT64_GM();

			int nProbTableID = 0;
			int nRandomOffset = _rand()%100000;
			float fRandomOffset = 0.0f;
			if( nRandomOffset > 0 )
				fRandomOffset = static_cast<float>( nRandomOffset ) / 1000.0f;

			std::vector<stPetChatProb>::iterator iter2 = petChat.vecPetChatProb.begin();
			for( ; iter2 != petChat.vecPetChatProb.end(); iter2++ )
			{
				if( (*iter2).fProb > fRandomOffset )
				{
					nProbTableID = (*iter2).nTableID;
					break;
				}
				else
				{
					fRandomOffset -= (*iter2).fProb;
				}
			}

			if( nProbTableID > 0 )
			{
				int nChatID = pPetChatTable->GetFieldFromLablePtr( nProbTableID, "_NameID" )->GetInteger();

				wchar_t wszChatString[256];
				wstring wszUIString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nChatID );
				if( StrStrW( wszUIString.c_str() , L"%s" ) )
					swprintf_s( wszChatString, wszUIString.c_str(), pLocalPlayer->GetName() );
				else
					swprintf_s( wszChatString, wszUIString.c_str() );

				pLocalPlayer->GetMyPetActor()->SetChatBalloonText( wszChatString, GetTickCount(), CHAT_NORMAL );
			}

			if( petChat.fChatCoolTime == 0.0f )
			{
				m_mapPetChat.erase( iter );
				break;
			}
		}
	}
}

void CDnPetTask::CheckAutoFeeding()
{
	if( m_fSatietyPercent == 100.0f )
		return;

	if( m_bRequestUsePetFood )	// 먹이 사용 이미 보낸 경우에는 응답 올 때 까지 먹이 먹지 않음
		return;

	if( m_stPetOption.m_bAutoFeed && m_stPetOption.m_nFoodItemID > 0 && static_cast<int>( m_fSatietyPercent ) <= m_stPetOption.m_nAutoFeedPercent )
	{
		// 자동먹이 먹기 기능
		bool bUseCashPetFood = false;
		int nPetFoodIndex = -1;

		CDnItem* pItem = NULL;
		CDnInventory::INVENTORY_MAP_ITER iter = GetItemTask().GetCharInventory().GetInventoryItemList().begin();
		for( ; iter != GetItemTask().GetCharInventory().GetInventoryItemList().end(); iter++ )
		{
			pItem = iter->second;
			if( !pItem )
				continue;

			if( pItem->GetItemType() == ITEMTYPE_PET_FOOD && pItem->GetClassID() == m_stPetOption.m_nFoodItemID )
			{
				if( nPetFoodIndex > -1 )
				{
					CDnItem* pPetFoodItem = GetItemTask().GetCharInventory().GetItem( nPetFoodIndex );
					if (pPetFoodItem != NULL)
					{
						int nPrevPetFoodCount = pPetFoodItem->GetOverlapCount();
						int nNextPetFoodCount = pItem->GetOverlapCount();

						if( nNextPetFoodCount <= nPrevPetFoodCount )
							nPetFoodIndex = pItem->GetSlotIndex();
					}
				}
				else
					nPetFoodIndex = pItem->GetSlotIndex();
			}
		}

		if( nPetFoodIndex == -1 )
		{
			iter = GetItemTask().GetCashInventory().GetInventoryItemList().begin();
			for( ; iter != GetItemTask().GetCashInventory().GetInventoryItemList().end(); iter++ )
			{
				pItem = iter->second;
				if( !pItem )
					continue;

				if( pItem->GetItemType() == ITEMTYPE_PET_FOOD && pItem->GetClassID() == m_stPetOption.m_nFoodItemID )
				{
					if( nPetFoodIndex > -1 )
					{
						CDnItem* pPetFoodItem = GetItemTask().GetCashInventory().GetItem( nPetFoodIndex );
						if (pPetFoodItem != NULL)
						{
							int nPrevPetFoodCount = pPetFoodItem->GetOverlapCount();
							int nNextPetFoodCount = pItem->GetOverlapCount();

							if( nNextPetFoodCount <= nPrevPetFoodCount )
								nPetFoodIndex = pItem->GetSlotIndex();
						}
					}
					else
						nPetFoodIndex = pItem->GetSlotIndex();

					bUseCashPetFood = true;
				}
			}
		}

		if( nPetFoodIndex > -1 )
		{
			if( bUseCashPetFood )
				GetItemTask().GetCashInventory().RequestUseItem( nPetFoodIndex, ITEMPOSITION_CASHINVEN );
			else
				GetItemTask().GetCharInventory().RequestUseItem( nPetFoodIndex, ITEMPOSITION_INVEN );

			m_bRequestUsePetFood = true;
		}
	}
}

void CDnPetTask::ResetPetSatietyInfo()
{
	m_nCurrentSatiety = 0;
	m_nMaxSatiety = 0;
	m_fSatietyPercent = 0.0f;
}

int CDnPetTask::GetPetFoodItemCount( int nPetFoodItemID )
{
	int nPetFoodCount = 0;
	CDnItem* pItem = NULL;

	CDnInventory::INVENTORY_MAP_ITER iter = GetItemTask().GetCharInventory().GetInventoryItemList().begin();
	for( ; iter != GetItemTask().GetCharInventory().GetInventoryItemList().end(); iter++ )
	{
		pItem = iter->second;
		if( !pItem )
			continue;

		if( pItem->GetItemType() == ITEMTYPE_PET_FOOD && pItem->GetClassID() == nPetFoodItemID )
		{
			nPetFoodCount += pItem->GetOverlapCount();
		}
	}

	if( nPetFoodCount == 0 )
	{
		iter = GetItemTask().GetCashInventory().GetInventoryItemList().begin();
		for( ; iter != GetItemTask().GetCashInventory().GetInventoryItemList().end(); iter++ )
		{
			pItem = iter->second;
			if( !pItem )
				continue;

			if( pItem->GetItemType() == ITEMTYPE_PET_FOOD && pItem->GetClassID() == nPetFoodItemID )
			{
				nPetFoodCount += pItem->GetOverlapCount();
			}
		}
	}

	return nPetFoodCount;
}


