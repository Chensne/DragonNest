#include "StdAfx.h"
#include "DnLifeSkillCookingTask.h"
#include "DnInterface.h"
#include "DnLifeSkillCookingDlg.h"
#include "DnPlayerActor.h"
#include "SecondarySkillFactory.h"
#include "ManufactureSkill.h"
#include "SecondarySkillRecipe.h"
#include "DnSkillTask.h"
#include "DnMainMenuDlg.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "DnLocalPlayerActor.h"
#include "DnLifeSkillFishingTask.h"


#ifdef PRE_ADD_COOKING_SYSTEM

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnLifeSkillCookingTask::CDnLifeSkillCookingTask()
: CTaskListener( false )
, m_pCookingSkill( NULL )
, m_pCookRecipe( NULL )
, m_bStartManufacture ( false )
, m_bRequestCooking( false )
, m_bManufactureLoop( false )
, m_bRecieveResult( false )
, m_bCooking( false )
, m_fTimer( 0.0f )
, m_pRequestRecipe( NULL )
{
}

CDnLifeSkillCookingTask::~CDnLifeSkillCookingTask()
{
}

bool CDnLifeSkillCookingTask::Initialize()
{
	return true;
}

void CDnLifeSkillCookingTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	if( nMainCmd == SC_SECONDARYSKILL )
	{
		switch( nSubCmd )
		{
			case eSecondarySkill::SC_RECIPE_LIST:		OnRecvRecipeList( (SecondarySkill::SCRecipeList*)pData ); break;
			case eSecondarySkill::SC_ADD_RECIPE:		OnRecvAddRecipe( (SecondarySkill::SCAddRecipe*)pData ); break;
			case eSecondarySkill::SC_UPDATE_RECIPE_EXP:	OnRecvUpdateRecipeExp( (SecondarySkill::SCUpdateRecipeExp*)pData ); break;
			case eSecondarySkill::SC_DELETE_RECIPE:		OnRecvDeleteRecipe( (SecondarySkill::SCDeleteRecipe*)pData ); break;
			case eSecondarySkill::SC_EXTRACT_RECIPE:	OnRecvExtractRecipe( (SecondarySkill::SCExtractRecipe*)pData ); break;
			case eSecondarySkill::SC_MANUFACTURE:		OnRecvManufacture( (SecondarySkill::SCManufacture*)pData ); break;
			case eSecondarySkill::SC_CANCEL_MANUFACTURE:	OnRecvCancelManufacture( (SecondarySkill::SCCancelManufacture*)pData ); break;
		}
	}
}

void CDnLifeSkillCookingTask::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if( nID == MANUFACTURE_PROGRESS )
	{
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			// 안보이는 버튼. 프로세스 처리가 끝났음을 알린다.
			if( strcmp( pControl->GetControlName(), "ID_PROCESS_COMPLETE" ) == 0 )
			{
				if( m_pCookRecipe )
					SendManufacture( m_pCookRecipe->GetSkillID(), m_pCookRecipe->GetItemID(), false );
			}
			else if( strcmp( pControl->GetControlName(), "ID_BUTTON_CANCEL" ) == 0 )
			{
				CancelManufacture();
				GetInterface().OpenCookingDialog( false );
			}
		}
	}
	else if( nID == MSGBOX_REQUEST_ADD_RECIPE )
	{
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( strcmp( pControl->GetControlName(), "ID_YES" ) == 0 && m_pRequestRecipe )
				SendAddRecipe( m_pRequestRecipe );
		}

		m_pRequestRecipe = NULL;
	}
}

void CDnLifeSkillCookingTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bDestroyTask ) return;

	if( m_bRecieveResult )
	{
		m_fTimer -= fDelta;
		if( m_fTimer <= 0.0f )
			CheckRecieveResult();
	}
}

void CDnLifeSkillCookingTask::CheckRecieveResult()
{
	if( m_bManufactureLoop && m_pCookRecipe )	// 연속 제작
	{
		m_pCookingSkill = GetSkillTask().GetLifeSkillRepository().GetManufactureSkill( m_pCookRecipe->GetSkillID() );
		if( m_pCookingSkill )
		{
			m_pCookRecipe = m_pCookingSkill->GetRecipe( m_pCookRecipe->GetItemID() );
			if( !Manufacture( m_pCookRecipe ) )
			{
				CDnLocalPlayerActor* plocalActor = dynamic_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
				if( plocalActor )
					plocalActor->LockInput( false );

				GetInterface().OpenCookingDialog( false );
				m_bRequestCooking = false;
				m_bCooking = false;
				GetInterface().DisableCashShopMenuDlg( false );
			}
		}
	}
	else
	{
		GetInterface().OpenCookingDialog( false );
		m_bRequestCooking = false;
		m_bCooking = false;
		GetInterface().DisableCashShopMenuDlg( false );

		CDnLocalPlayerActor* plocalActor = dynamic_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
		if( plocalActor )
			plocalActor->LockInput( false );
	}

	m_bRecieveResult = false;
}

void CDnLifeSkillCookingTask::StartManufacture( CSecondarySkillRecipe* pCookRecipe, bool bManufactureLoop )
{
	if( pCookRecipe == NULL )
		return;

	if( GetItemTask().IsRequestWait() || 
		GetLifeSkillFishingTask().IsRequestFishingReady() ||
		GetLifeSkillFishingTask().IsNowFishing() )			// 다른거 사용 요청 중이면 낚시 시작 안함
		return;

	if( CDnPartyTask::GetInstance().IsRequestEnteredGate() || CDnPartyTask::GetInstance().GetEnteredGateIndex() != INVALID_GATE_INDEX )
		return;

	if( CDnActor::s_hLocalActor == NULL )
		return;

	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );

	if( pPlayer )
	{
		if( pPlayer->IsBattleMode() )
			pPlayer->CmdToggleBattle( false );
		
#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
		GetInterface().ClearAcceptReqDlg(true);
#endif
		
		if( pPlayer->IsCanVehicleMode() && ( pPlayer->IsVehicleMode() || pPlayer->GetVehicleInfo().Vehicle[0].nItemID != 0 ) )
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7477 ) );
		}
		else if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG, ITEM_SLOT_TYPE::ST_INVENTORY ) )
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7482 ) );
		}
		else
		{
			m_bManufactureLoop = bManufactureLoop;
			Manufacture( pCookRecipe );
		}
	}
}

bool CDnLifeSkillCookingTask::Manufacture( CSecondarySkillRecipe* pCookRecipe )
{
	if( pCookRecipe == NULL )
		return false;

	// 제작 도구 및 재료 확인
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
	if( pSox == NULL )
	{
		DN_ASSERT( 0, "Invalid SecondarySkillRecipe" );
		return false;
	}

	char szStr[64];
	bool bCheckManufacture = true;
	for( int i=0; i<MAX_REQUIRED_NUM; i++ )
	{
		sprintf_s( szStr, "_ToolItem%dID", i+1 );
		int nTooltemID = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();
		if( nTooltemID > 0 )
		{
			int nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( nTooltemID );
			if( nCurItemCount == 0 )
			{
				bCheckManufacture = false;
				break;
			}
		}

		sprintf_s( szStr, "_MaterialItem%dID", i+1 );
		int nIngredientID = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();
		if( nIngredientID > 0 )
		{
			int nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( nIngredientID );
			sprintf_s( szStr, "_Count%d", i+1 );
			int nNeedItemCount = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();
			if( nCurItemCount < nNeedItemCount )
			{
				bCheckManufacture = false;
				break;
			}
		}
	}

	if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG, ITEM_SLOT_TYPE::ST_INVENTORY ) )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7482 ) );
		bCheckManufacture = false;
	}

	if( !m_bStartManufacture && bCheckManufacture )
	{
		m_pCookRecipe = pCookRecipe;
		SendManufacture( pCookRecipe->GetSkillID(), pCookRecipe->GetItemID(), true );
		m_bStartManufacture = true;
		m_bRequestCooking = true;
	}

	return bCheckManufacture;
}

void CDnLifeSkillCookingTask::OnRecvRecipeList( SecondarySkill::SCRecipeList* pPacket )
{
	if( pPacket == NULL || pPacket->RecipeList == NULL )
		return;

	if( pPacket->cCount > SecondarySkill::Max::LearnManufactureRecipe )
		return;

	for( int i=0; i<pPacket->cCount; i++ )
	{
		CSecondarySkillRecipe* pCookRecipe = NULL;
		pCookRecipe = CSecondarySkillFactory::CreateRecipe( pPacket->RecipeList[i].iSkillID, pPacket->RecipeList[i].iItemID, 
			pPacket->RecipeList[i].biItemSerial, pPacket->RecipeList[i].nExp );
		m_pCookingSkill = GetSkillTask().GetLifeSkillRepository().GetManufactureSkill( pPacket->RecipeList[i].iSkillID );
		if( m_pCookingSkill && pCookRecipe )
			m_pCookingSkill->AddRecipe( pCookRecipe );
	}
}

void CDnLifeSkillCookingTask::SendAddRecipe( CDnItem* pRecipeItem )
{
	if( pRecipeItem == NULL )
		return;

	// 이미 추가된 레시피인지 확인
	if( m_pCookingSkill && m_pCookingSkill->GetRecipe( pRecipeItem->GetClassID() ) )
		return;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
	if( pSox == NULL )
	{
		DN_ASSERT( 0, "Invalid SecondarySkillRecipe" );
		return;
	}
	int nSecondarySkillID = pSox->GetFieldFromLablePtr( pRecipeItem->GetClassID(), "_SecondarySkillID" )->GetInteger();

	SecondarySkill::CSAddRecipe AddRecipe;
	AddRecipe.iSkillID = nSecondarySkillID;
	AddRecipe.cInvenType = pRecipeItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;;
	AddRecipe.cInvenIndex = pRecipeItem->GetSlotIndex();
	AddRecipe.biInvenSerial = pRecipeItem->GetSerialID();
	CClientSessionManager::GetInstance().SendPacket( CS_SECONDARYSKILL, eSecondarySkill::CS_ADD_RECIPE, (char*)&AddRecipe, int(sizeof(AddRecipe)) );
}

void CDnLifeSkillCookingTask::SendDeleteRecipe( int nSkillID, int nItemID )
{
	SecondarySkill::CSDeleteRecipe DeleteRecipe;
	DeleteRecipe.iSkillID = nSkillID;
	DeleteRecipe.iItemID = nItemID;

	CClientSessionManager::GetInstance().SendPacket( CS_SECONDARYSKILL, eSecondarySkill::CS_DELETE_RECIPE, (char*)&DeleteRecipe, int(sizeof(DeleteRecipe)) );
}

void CDnLifeSkillCookingTask::SendExtractRecipe( int nSkillID, int nItemID )
{
	SecondarySkill::CSExtractRecipe ExtractRecipe;
	ExtractRecipe.iSkillID = nSkillID;
	ExtractRecipe.iItemID = nItemID;

	CClientSessionManager::GetInstance().SendPacket( CS_SECONDARYSKILL, eSecondarySkill::CS_EXTRACT_RECIPE, (char*)&ExtractRecipe, int(sizeof(ExtractRecipe)) );
}

void CDnLifeSkillCookingTask::SendManufacture( int nSkillID, int nItemID, bool bStart )
{
	SecondarySkill::CSManufacture Manufacture;
	Manufacture.iSkillID = nSkillID;
	Manufacture.iItemID = nItemID;
	Manufacture.bIsStart = bStart;

	CClientSessionManager::GetInstance().SendPacket( CS_SECONDARYSKILL, eSecondarySkill::CS_MANUFACTURE, (char*)&Manufacture, int(sizeof(Manufacture)) );
}

void CDnLifeSkillCookingTask::OnRecvAddRecipe( SecondarySkill::SCAddRecipe* pPacket )
{
	if( pPacket == NULL )
		return;

	if( pPacket->iRet == 0 )
	{
		CSecondarySkillRecipe* pCookRecipe = NULL;
		pCookRecipe = CSecondarySkillFactory::CreateRecipe( pPacket->iSkillID, pPacket->iItemID, pPacket->biItemSerial, pPacket->nExp );

		m_pCookingSkill = GetSkillTask().GetLifeSkillRepository().GetManufactureSkill( pPacket->iSkillID );
		if( m_pCookingSkill && pCookRecipe )
			m_pCookingSkill->AddRecipe( pCookRecipe );

		CDnLifeSkillCookingDlg* pSkillCookingDlg = static_cast<CDnLifeSkillCookingDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COOKING_DIALOG ) );
		if( pSkillCookingDlg && pSkillCookingDlg->IsShow() )
			pSkillCookingDlg->UpdateCookingList();

		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7449 ), false );
	}
	else if( pPacket->iRet == ERROR_SECONDARYSKILL_ADDRECIPE_FAILED )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7487 ) );
	}
	else if( pPacket->iRet == ERROR_SECONDARYSKILL_ADDRECIPE_MAX )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7513 ) );
	}
}

void CDnLifeSkillCookingTask::OnRecvUpdateRecipeExp( SecondarySkill::SCUpdateRecipeExp* pPacket )
{
	if( pPacket == NULL )
		return;

	m_pCookingSkill = GetSkillTask().GetLifeSkillRepository().GetManufactureSkill( pPacket->iSkillID );
	if( m_pCookingSkill )
	{
		CSecondarySkillRecipe* pCookRecipe = m_pCookingSkill->GetRecipe( pPacket->iItemID );
		if( pCookRecipe )
			pCookRecipe->SetExp( pPacket->iExp );
	}
}

void CDnLifeSkillCookingTask::OnRecvDeleteRecipe( SecondarySkill::SCDeleteRecipe* pPacket )
{
	if( pPacket == NULL )
		return;

	if( pPacket->iRet == 0 )
	{
		m_pCookingSkill = GetSkillTask().GetLifeSkillRepository().GetManufactureSkill( pPacket->iSkillID );
		if( m_pCookingSkill )
		{
			CSecondarySkillRecipe* pCookRecipe = m_pCookingSkill->GetRecipe( pPacket->iItemID );
			if( pCookRecipe )
				m_pCookingSkill->DeleteRecipe( pCookRecipe );
		}

		CDnLifeSkillCookingDlg* pSkillCookingDlg = static_cast<CDnLifeSkillCookingDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COOKING_DIALOG ) );
		if( pSkillCookingDlg && pSkillCookingDlg->IsShow() )
		{
			pSkillCookingDlg->ResetSelectedRecipeInfo();
			pSkillCookingDlg->UpdateCookingList();
		}

		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7450 ), false );
	}
	else if( pPacket->iRet == ERROR_SECONDARYSKILL_DELETERECIPE_FAILED )
	{
		// 레시피 삭제 실패함
	}
}

void CDnLifeSkillCookingTask::OnRecvExtractRecipe( SecondarySkill::SCExtractRecipe* pPacket )
{
	if( pPacket == NULL )
		return;

	if( pPacket->iRet == 0 )
	{
		GetInterface().CloseCookingDialog();	
		GetInterface().OpenMovieAlarmDlg( NULL, 2.0f, pPacket->iItemID );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7451 ), false );
	}
	else if( pPacket->iRet == ERROR_SECONDARYSKILL_EXTRACTRECIPE_FAILED )
	{
		// 레시피 추출 실패함
	}
}

void CDnLifeSkillCookingTask::OnRecvManufacture( SecondarySkill::SCManufacture* pPacket )
{
	switch( pPacket->iRet )
	{
		case 0:
			{
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
				if( pSox == NULL )
				{
					DN_ASSERT( 0, "Invalid SecondarySkillRecipe" );
					return;
				}

				if( GetInterface().IsGateQuestionDlgShow() )
				{
					CancelManufacture( false );
					return;
				}

				if( pPacket->bIsStart )	// 제작 시작
				{
					float fManufactureTime = static_cast<float>( pSox->GetFieldFromLablePtr( pPacket->iItemID, "_MaxTime" )->GetInteger() );
					GetInterface().CloseCookingDialog();
					GetInterface().OpenFocusTypeMovieProcessDlg( NULL, fManufactureTime / 1000.0f, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7444 ), MANUFACTURE_PROGRESS, this );
					m_bCooking = true;

					CDnLocalPlayerActor* plocalActor =  dynamic_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
					if( plocalActor )
					{
						plocalActor->ResetAutoRun();
						plocalActor->CmdStop( "Stand" );
						plocalActor->LockInput( true );
					}

					GetInterface().DisableCashShopMenuDlg( true );
				}
				else // 제작 완료
				{
					if( m_pCookRecipe )
					{
						char szStr[64];
						// 레시피 현재 달성율을 구함
						int fAchievementRate = static_cast<int>(( ( m_pCookRecipe->GetExp() ) / static_cast<float>( m_pCookRecipe->GetMaxExp() ) ) * 100);
						int nMakeItemCount = pSox->GetFieldFromLablePtr( m_pCookRecipe->GetItemID(), "_MakeItemCount" )->GetInteger();

						int nMakeItem = 1;
						for( int i=0; i<nMakeItemCount; i++ )
						{
							sprintf_s( szStr, "_MinExp%d", i+1 );
							int nMinExp = pSox->GetFieldFromLablePtr( m_pCookRecipe->GetItemID(), szStr )->GetInteger();
							sprintf_s( szStr, "_MaxExp%d", i+1 );
							int nMaxExp = pSox->GetFieldFromLablePtr( m_pCookRecipe->GetItemID(), szStr )->GetInteger();
							if( fAchievementRate >= nMinExp && fAchievementRate <= nMaxExp )
							{
								nMakeItem = i+1;
								break;
							}
						}

						sprintf_s( szStr, "_MakeItem%dID", nMakeItem );
						int nItemID = pSox->GetFieldFromLablePtr( m_pCookRecipe->GetItemID(), szStr )->GetInteger();

						// 알람 다이얼로그
						GetInterface().OpenMovieAlarmDlg( NULL, 2.0f, nItemID );
						// 시스템 메세지.
						GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7448 ), false );
						m_bStartManufacture = false;
						m_bRecieveResult = true;
						m_fTimer = 2.0f;
					}
				}
			}
			break;
		case ERROR_SECONDARYSKILL_MANUFACTURE_PROBABILITY_FAILED:
			{
				GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7011 ), false );
				GetInterface().OpenMovieAlarmDlg( NULL, 2.0f, 0, 0, 0, 7011 );
				m_bStartManufacture = false;
				m_bRecieveResult = false;
				m_bRequestCooking = false;
				m_fTimer = 2.0f;
			}
			break;
		case ERROR_SECONDARYSKILL_MANUFACTURE_DONTALLOWMAP:
			{
				GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7512 ), false );
				GetInterface().OpenMovieAlarmDlg( NULL, 2.0f, 0, 0, 0, 7512 );
				m_bStartManufacture = false;
				m_bRecieveResult = false;
				m_bRequestCooking = false;
				m_fTimer = 2.0f;
			}
			break;
	}
}

void CDnLifeSkillCookingTask::OnRecvCancelManufacture( SecondarySkill::SCCancelManufacture* pPacket )
{
	if( pPacket->iRet == 0 )
	{
		m_pCookRecipe = NULL;
		GetInterface().CloseFocusTypeMovieProcessDlg( true );

		CDnLocalPlayerActor* plocalActor = dynamic_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
		if( plocalActor )
			plocalActor->LockInput( false );

		m_bStartManufacture = false;
		m_bRequestCooking = false;
		m_bCooking = false;
		GetInterface().DisableCashShopMenuDlg( false );
	}
}

void CDnLifeSkillCookingTask::CancelManufacture( bool bReleaseLockInput )
{
	CClientSessionManager::GetInstance().SendPacket( CS_SECONDARYSKILL, eSecondarySkill::CS_CANCEL_MANUFACTURE, NULL, 0 );
	m_pCookRecipe = NULL;
	GetInterface().CloseFocusTypeMovieProcessDlg( true );
	
	m_bStartManufacture = false;
	m_bRequestCooking = false;
	m_bCooking = false;

	if( bReleaseLockInput )
	{
		CDnLocalPlayerActor* plocalActor = dynamic_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
		if( plocalActor )
			plocalActor->LockInput( false );
	}

	GetInterface().DisableCashShopMenuDlg( false );
}

void CDnLifeSkillCookingTask::RequestAddRecipe( CDnItem* pRecipeItem )
{
	if( pRecipeItem == NULL )
		return;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
	if( pSox == NULL )
	{
		DN_ASSERT( 0, "Invalid SecondarySkillRecipe" );
		return;
	}

	SecondarySkill::Grade::eType eRequiredGrade = (SecondarySkill::Grade::eType)pSox->GetFieldFromLablePtr( pRecipeItem->GetClassID(), "_SecondarySkillClass" )->GetInteger();
	int nRequiredSkillLevel = pSox->GetFieldFromLablePtr( pRecipeItem->GetClassID(), "_SecondarySkillLevel" )->GetInteger();
	int nSecondarySkillID = pSox->GetFieldFromLablePtr( pRecipeItem->GetClassID(), "_SecondarySkillID" )->GetInteger();

	CManufactureSkill* pCookingSkill = GetSkillTask().GetLifeSkillRepository().GetManufactureSkill( nSecondarySkillID );
	if( pCookingSkill )
	{
		if( ( eRequiredGrade < pCookingSkill->GetGrade() ) || ( eRequiredGrade == pCookingSkill->GetGrade() && nRequiredSkillLevel <= pCookingSkill->GetLevel() ) )
		{
			m_pRequestRecipe = pRecipeItem;
			WCHAR wszMessage[128];
			wsprintf( wszMessage, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7447 ), pRecipeItem->GetName() );
			GetInterface().MessageBox( wszMessage, MB_YESNO, MSGBOX_REQUEST_ADD_RECIPE, this );
		}
		else
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7466 ) );
	}
}

#endif // PRE_ADD_COOKING_SYSTEM