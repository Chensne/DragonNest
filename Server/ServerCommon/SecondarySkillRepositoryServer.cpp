
#include "Stdafx.h"
#include "SecondarySkillRepositoryServer.h"
#include "SecondarySkillEventHandlerServer.h"
#include "SecondarySkill.h"
#include "DNUserSession.h"
#include "ManufactureSkill.h"
#include "SecondarySkillFactory.h"
#include "SecondarySkillRecipe.h"
#include "DNGameDataManager.h"

#if defined( PRE_ADD_SECONDARY_SKILL )

CSecondarySkillRepositoryServer::CSecondarySkillRepositoryServer( CDNUserSession* pSession )
:m_pSession(pSession),m_bManufacturing(false)
{
	m_pEventHandler = new CSecondarySkillEventHandlerServer( pSession );
}

void CSecondarySkillRepositoryServer::SendList()
{
	SecondarySkill::SCList TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	_ASSERT( m_mSecondarySkill.size() <= _countof(TxPacket.SkillList) );
	if( m_mSecondarySkill.size() > _countof(TxPacket.SkillList) )
		return;

	for( std::map<int,CSecondarySkill*>::iterator itor=m_mSecondarySkill.begin() ; itor!=m_mSecondarySkill.end() ; ++itor )
	{
		CSecondarySkill* pSkill = (*itor).second;

		TxPacket.SkillList[TxPacket.cCount].iSkillID	= pSkill->GetSkillID();
		TxPacket.SkillList[TxPacket.cCount].iExp		= pSkill->GetExp();
		TxPacket.SkillList[TxPacket.cCount].Grade		= pSkill->GetGrade();
		TxPacket.SkillList[TxPacket.cCount].iLevel		= pSkill->GetLevel();

		++TxPacket.cCount;
	}
	
	int iSize = sizeof(TxPacket)-sizeof(TxPacket.SkillList)+(TxPacket.cCount*sizeof(TxPacket.SkillList[0]));
	m_pSession->AddSendData( SC_SECONDARYSKILL, eSecondarySkill::SC_LIST, reinterpret_cast<char*>(&TxPacket), iSize );

#if defined( _WORK )
	for( int i=0 ; i<TxPacket.cCount ; ++i )
	{
		WCHAR wszBuf[MAX_PATH];
		wsprintf( wszBuf, L"[보조스킬] SC_LIST SkillID:%d Exp:%d", TxPacket.SkillList[i].iSkillID, TxPacket.SkillList[i].iExp );
		m_pSession->SendDebugChat( wszBuf );
	}
#endif // #if defined( _WORK )

	_SendRecipeList();
}

void CSecondarySkillRepositoryServer::_SendRecipeList()
{
	for( std::map<int,CSecondarySkill*>::iterator itor=m_mSecondarySkill.begin() ; itor!=m_mSecondarySkill.end() ; ++itor )
	{
		CSecondarySkill* pSkill = (*itor).second;

		if( pSkill->GetType() != SecondarySkill::Type::ManufactureSkill )
			continue;

		CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);
		if( pManufactureSkill->GetRecipeCount() <= 0 )
			continue;

		std::vector<CSecondarySkillRecipe*> vList;
		pManufactureSkill->CopyList( vList );

		// Send Recipe
		SecondarySkill::SCRecipeList TxPacket;
		memset( &TxPacket, 0, sizeof(TxPacket) );

		TxPacket.cCount = static_cast<BYTE>(vList.size());
		for( UINT i=0 ; i<TxPacket.cCount ; ++i )
		{
			TxPacket.RecipeList[i].iSkillID		= vList[i]->GetSkillID();
			TxPacket.RecipeList[i].iItemID		= vList[i]->GetItemID();
			TxPacket.RecipeList[i].biItemSerial	= vList[i]->GetItemSerial();
			TxPacket.RecipeList[i].nExp			= vList[i]->GetExp();

#if defined( _WORK )
			WCHAR wszBuf[MAX_PATH];
			wsprintf( wszBuf, L"[보조스킬] SC_RECIPE_LIST SkillID:%d ItemID:%d ItemSerial:%I64d Exp:%d", TxPacket.RecipeList[i].iSkillID, TxPacket.RecipeList[i].iItemID, TxPacket.RecipeList[i].biItemSerial, TxPacket.RecipeList[i].nExp );
			m_pSession->SendDebugChat( wszBuf );
#endif // #if defined( _WORK )
		}
		
		int iSize = sizeof(TxPacket)-sizeof(TxPacket.RecipeList)+(TxPacket.cCount*sizeof(TxPacket.RecipeList[0]));
		m_pSession->AddSendData( SC_SECONDARYSKILL, eSecondarySkill::SC_RECIPE_LIST, reinterpret_cast<char*>(&TxPacket), iSize );
	}
}

// ERROR_NONE	: OK
// 그외			: 에러값
int CSecondarySkillRepositoryServer::CanCreate( CSecondarySkill* pCreateSkill )
{
	int iMaxCount = 0;
	switch( pCreateSkill->GetType() )
	{
		case SecondarySkill::Type::ManufactureSkill:
		{
			iMaxCount = SecondarySkill::Max::LearnManufactureSkill;
			break;
		}
		case SecondarySkill::Type::ProductionSkill:
		{
			iMaxCount = SecondarySkill::Max::LearnProductionSkill;
			break;
		}
		case SecondarySkill::Type::CommonSkill:
		{
			iMaxCount = SecondarySkill::Max::LearnCommonSkill;
			break;
		}
	}

	// 중복 및 최대 수 검사
	if( Get( pCreateSkill->GetSkillID() ) || GetCount( pCreateSkill->GetType() ) >= iMaxCount )
		return ERROR_GENERIC_INVALIDREQUEST;

	return ERROR_NONE;
}

// ERROR_NONE	: OK
// 그외			: 에러값
int CSecondarySkillRepositoryServer::CanAddRecipe( CSecondarySkill* pSkill, CSecondarySkillRecipe* pRecipe )
{
	// 제작 스킬 검사
	if( pSkill->GetType() != SecondarySkill::Type::ManufactureSkill )
		return ERROR_GENERIC_INVALIDREQUEST;

	// 최대 Recipe 개수 검사
	CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);
	if( pManufactureSkill->GetRecipeCount() >= pManufactureSkill->GetMaxRecipeCount() )
		return ERROR_SECONDARYSKILL_ADDRECIPE_MAX;

	// 스킬ID 검사
	if( pSkill->GetSkillID() != pRecipe->GetSkillID() )
		return ERROR_GENERIC_INVALIDREQUEST;

	// Grade 검사
	if( pSkill->GetGrade() < pRecipe->GetRequiredGrade() )
		return ERROR_GENERIC_INVALIDREQUEST;

	// 레벨검사
	if( pSkill->GetGrade() == pRecipe->GetRequiredGrade() )
	{
		if( pSkill->GetLevel() < pRecipe->GetRequiredSkillLevel() )
			return ERROR_GENERIC_INVALIDREQUEST;
	}

	return ERROR_NONE;
}

// ERROR_NONE	: OK 일때 CSecondarySkillRecipe 포인터 참조값으로 반환
// 그외			: 에러값
int CSecondarySkillRepositoryServer::CanDeleteRecipe( CSecondarySkill* pSkill, int iItemID, CSecondarySkillRecipe*& pRecipe )
{
	// Type 검사
	if( pSkill->GetType() != SecondarySkill::Type::ManufactureSkill )
		return ERROR_GENERIC_INVALIDREQUEST;

	// 레시피 얻기
	CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);
	
	pRecipe = pManufactureSkill->GetRecipe( iItemID );
	if( pRecipe == NULL )
		return ERROR_GENERIC_INVALIDREQUEST;

	return ERROR_NONE;
}

// ERROR_NONE	: OK 일때 CSecondarySkillRecipe 포인터 참조값으로 반환
// 그외			: 에러값
int CSecondarySkillRepositoryServer::CanExtractRecipe( CSecondarySkill* pSkill, int iItemID, CSecondarySkillRecipe*& pRecipe )
{
	// Type 검사
	if( pSkill->GetType() != SecondarySkill::Type::ManufactureSkill )
		return ERROR_GENERIC_INVALIDREQUEST;

	// 레시피 얻기
	CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);

	pRecipe = pManufactureSkill->GetRecipe( iItemID );
	if( pRecipe == NULL )
		return ERROR_GENERIC_INVALIDREQUEST;

	// 100% Full상태인지 검사
	if( pRecipe->GetMaxExp() <= 0 )
		return ERROR_GENERIC_INVALIDREQUEST;

	if( pRecipe->GetExp() != pRecipe->GetMaxExp() )
		return ERROR_GENERIC_INVALIDREQUEST;

	return ERROR_NONE;
}

// ERROR_NONE	: OK 일때 CSecondarySkillRecipe 포인터 참조값으로 반환
// 그외			: 에러값
int	CSecondarySkillRepositoryServer::CanManufacture( CSecondarySkill* pSkill, bool bIsStart, int iItemID, CSecondarySkillRecipe*& pRecipe )
{
	// Type 검사
	if( pSkill->GetType() != SecondarySkill::Type::ManufactureSkill )
		return ERROR_GENERIC_INVALIDREQUEST;

	// 레시피 얻기
	CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);

	pRecipe = pManufactureSkill->GetRecipe( iItemID );
	if( pRecipe == NULL )
		return ERROR_GENERIC_INVALIDREQUEST;

	TSecondarySkillRecipeTableData* pRecipeData = g_pDataManager->GetSecondarySkillRecipeTableData( pRecipe->GetItemID() );
	if( pRecipeData == NULL )
		return ERROR_GENERIC_INVALIDREQUEST;

	// 도구템 검사
	for( int i=0 ; i<_countof(pRecipeData->iToolItemID) ; ++i )
	{
		if( pRecipeData->iToolItemID[i] <= 0 )
			continue;

		if( m_pSession->GetItem()->GetInventoryItemCount( pRecipeData->iToolItemID[i] ) <= 0 )
			return ERROR_GENERIC_INVALIDREQUEST;
	}

	// 재료템 검사
	for( int i=0 ; i<_countof(pRecipeData->iMaterialItemCount) ; ++i )
	{
		if( pRecipeData->iMaterialItemID[i] <= 0 )
			continue;

		if( m_pSession->GetItem()->GetInventoryItemCount( pRecipeData->iMaterialItemID[i] ) < pRecipeData->iMaterialItemCount[i] )
			return ERROR_GENERIC_INVALIDREQUEST;
	}

	// 인벤토리 빈 자리 검사
	int iCreateItemID = pRecipeData->GetManufactureItemID( pRecipe->GetExp(), pRecipe->GetMaxExp() );
	TItemData* pItemData = g_pDataManager->GetItemData( iCreateItemID );
	if( pItemData == NULL )
	{
		_ASSERT(0);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	if( pItemData->nType != ITEMTYPE_INSTANT )
	{
		if( m_pSession->GetItem()->FindBlankInventorySlotCount() <= 0 )
			return ERROR_GENERIC_INVALIDREQUEST;
	}
	else
	{
		// 빌리지서버는 ITEMTYPE_INSTANT 제작이 불가능한다.
#if defined( _VILLAGESERVER )
		return ERROR_SECONDARYSKILL_MANUFACTURE_DONTALLOWMAP;
#endif // #if defined( _VILLAGESERVER )
	}
	
	if( bIsStart == true )
	{
		// 결과템 검사
		int iResultItemID = pRecipeData->GetManufactureItemID( pRecipe->GetExp(), pRecipe->GetMaxExp() );
		TItemData* pResultItemData = g_pDataManager->GetItemData( iResultItemID );
		if( pResultItemData == NULL )
		{
			_ASSERT(0);
			return ERROR_GENERIC_INVALIDREQUEST;
		}

		if( !g_pDataManager->IsUseItemAllowMapTypeCheck( iResultItemID, m_pSession->GetMapIndex() ) )
			return ERROR_SECONDARYSKILL_MANUFACTURE_DONTALLOWMAP;

		// 현재 제작중인 스킬이 있는지 검사
		if( GetManufacturingSkill() )
			return ERROR_GENERIC_INVALIDREQUEST;

		// 이미 제작중인지 검사
		if( pManufactureSkill->bIsManufacturing() == true )
			return ERROR_GENERIC_INVALIDREQUEST;
	}
	else
	{
		// 제작중인지 검사
		if( pManufactureSkill->bIsManufacturing() == false )
			return ERROR_GENERIC_INVALIDREQUEST;

		// 제작시간 검사
		if( timeGetTime()-pManufactureSkill->GetManufactureTick() + 900 < static_cast<DWORD>(pRecipeData->iMaxTime) )
			return ERROR_GENERIC_INVALIDREQUEST;
	}

	return ERROR_NONE;
}

// ERROR_NONE	: OK 일때 CManufactureSkill 포인터 참조값으로 반환
// 그외			: 에러값
int	CSecondarySkillRepositoryServer::CanCancelManufacture( CManufactureSkill*& pManufacturingSkill )
{
#if defined( _GAMESERVER )
	// 게임서버는 호출 빈도수가 높아 Flag 로 먼저 검사한다.
	if( bIsManufacturing() == false )
		return ERROR_GENERIC_INVALIDREQUEST;
#endif // #if defined( _GAMESERVER )

	// 현재 제작중인 스킬 검사
	pManufacturingSkill = static_cast<CManufactureSkill*>(GetManufacturingSkill());
	if( pManufacturingSkill == NULL )
		return ERROR_GENERIC_INVALIDREQUEST;

	return ERROR_NONE;
}

// 클라이언트에 받은 레시피 추가
int CSecondarySkillRepositoryServer::AddRecipe( SecondarySkill::CSAddRecipe* pPacket )
{
	CSecondarySkill* pSkill = Get( pPacket->iSkillID );
	if( pSkill == NULL )
		return ERROR_SECONDARYSKILL_ADDRECIPE_FAILED;

	const TItem* pItem = NULL;

	switch( pPacket->cInvenType )
	{
		case ITEMPOSITION_INVEN:
		{
			pItem = m_pSession->GetItem()->GetInventory(pPacket->cInvenIndex);
			
			if( pItem == NULL )
				return ERROR_SECONDARYSKILL_ADDRECIPE_FAILED;

			if( pItem->nSerial != pPacket->biInvenSerial) 
				return ERROR_SECONDARYSKILL_ADDRECIPE_FAILED;
			break;
		}
		case ITEMPOSITION_CASHINVEN:
		{
			pItem = m_pSession->GetItem()->GetCashInventory( pPacket->biInvenSerial );
			if( pItem == NULL )
				return ERROR_SECONDARYSKILL_ADDRECIPE_FAILED;
			break;
		}
		default:
		{
			return ERROR_SECONDARYSKILL_ADDRECIPE_FAILED;
		}
	}

	// 아이템 타입 검사
	TItemData* pItemData = g_pDataManager->GetItemData( pItem->nItemID );
	if( pItemData == NULL )
		return ERROR_SECONDARYSKILL_ADDRECIPE_FAILED;
	if( pItemData->nType != ITEMTYPE_SECONDARYSKILL_RECIPE )
		return ERROR_SECONDARYSKILL_ADDRECIPE_FAILED;

	// 레시피 생성
	int iRet = ERROR_SECONDARYSKILL_ADDRECIPE_FAILED;
	CSecondarySkillRecipe* pRecipe = NULL;
	do 
	{
		pRecipe = CSecondarySkillFactory::CreateRecipe( pPacket->iSkillID, pItem->nItemID, pPacket->biInvenSerial, pItem->wDur );
		if( pRecipe == NULL || pRecipe->IsValid() == false )
			break;

		iRet = CanAddRecipe( pSkill, pRecipe );
		if( iRet != ERROR_NONE )
			break;

		// 아이템 제거
		bool bDeleteRet = false;
		if( pPacket->cInvenType == ITEMPOSITION_INVEN )
			bDeleteRet = m_pSession->GetItem()->DeleteInventoryBySlot( pPacket->cInvenIndex, 1, pPacket->biInvenSerial, 0 );
		else
			bDeleteRet = m_pSession->GetItem()->DeleteCashInventoryBySerial( pPacket->biInvenSerial, 1, false );
		_ASSERT( bDeleteRet );

		// 레시피 추가
		CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);
		iRet = pManufactureSkill->AddRecipe( pRecipe );
		if( iRet != ERROR_NONE )
			break;

		// EventHandler 호출
		m_pEventHandler->OnAddRecipe( pRecipe, pItemData->IsCash );
		return ERROR_NONE;
	}while( false );

	SAFE_DELETE( pRecipe );
	return iRet;
}

// DB에서 받은 레시피 추가
bool CSecondarySkillRepositoryServer::AddRecipe( TSecondarySkillRecipe* pRecipeData )
{
	CSecondarySkill* pSkill = Get( pRecipeData->iSkillID );
	if( pSkill == NULL )
		return false;

	CSecondarySkillRecipe* pRecipe = CSecondarySkillFactory::CreateRecipe( pRecipeData->iSkillID, pRecipeData->iItemID, pRecipeData->biItemSerial, pRecipeData->nExp );
	do 
	{
		if( pRecipe == NULL || pRecipe->IsValid() == false )
			break;

#if defined( _FINAL_BUILD )
		if( pSkill->GetType() != SecondarySkill::Type::ManufactureSkill )
			break;
#else
		// 개발버젼에서만 해당 에러에 대한 에러 처리하고 기본적으로 DB에서 받은 데이터는 신뢰한다.
		if( CanAddRecipe( pSkill, pRecipe ) != ERROR_NONE )
		{
			_ASSERT(0);
			break;
		}
#endif // #if defined( _FINAL_BUILD )

		CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);
		if( pManufactureSkill->AddRecipe( pRecipe ) != ERROR_NONE )
			break;
		return true;
	}while( false );
	
	SAFE_DELETE( pRecipe );
	return false;
}

// 클라이언트에서 받은 레시피 제거
bool CSecondarySkillRepositoryServer::DeleteRecipe( SecondarySkill::CSDeleteRecipe* pPacket )
{
	CSecondarySkill* pSkill = Get( pPacket->iSkillID );
	if( pSkill == NULL )
		return false;

	CSecondarySkillRecipe* pRecipe = NULL;
	if( CanDeleteRecipe( pSkill, pPacket->iItemID, pRecipe ) != ERROR_NONE )
		return false;

	CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);
	if( pManufactureSkill->DeleteRecipe( pRecipe ) == false )
		return false;

	bool bRet = m_pEventHandler->OnDeleteRecipe( pRecipe );
	_ASSERT( bRet );

	SAFE_DELETE( pRecipe );
	return true;
}

// 클라이언트에서 받은 레시피 추출
bool CSecondarySkillRepositoryServer::ExtractRecipe( SecondarySkill::CSExtractRecipe* pPacket )
{
	CSecondarySkill* pSkill = Get( pPacket->iSkillID );
	if( pSkill == NULL )
		return false;

	CSecondarySkillRecipe* pRecipe = NULL;
	if( CanExtractRecipe( pSkill, pPacket->iItemID, pRecipe ) != ERROR_NONE )
		return false;

	CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);

	// 아이템 여분 공간 체크
	TItemData* pItemData = g_pDataManager->GetItemData( pRecipe->GetItemID() );
	if( pItemData == NULL )
		return false;

	// 레시피 제거
	if( pManufactureSkill->DeleteRecipe( pRecipe ) == false )
		return false;

	// 캐시아이템은 슬롯 제한이 없다.
	int iEmptyInvenIndex = -1;
	if( pItemData->IsCash != true)
	{
		iEmptyInvenIndex = m_pSession->GetItem()->FindBlankInventorySlot();
		if( iEmptyInvenIndex < 0 )
			return false;

		TItem ExtractItem;
		if( CDNUserItem::MakeItemStruct( pRecipe->GetItemID(), ExtractItem ) == false )
			return false;

		ExtractItem.nSerial = pRecipe->GetItemSerial();
		ExtractItem.wDur = pRecipe->GetExp();

		if( m_pSession->GetItem()->CreateInvenWholeItemByIndex( iEmptyInvenIndex, ExtractItem ) != ERROR_NONE )
			return false;
	}
	else
	{
		TItem ExtractItem;
		if( m_pSession->GetItem()->MakeCashItemStruct( 0, pRecipe->GetItemID(), ExtractItem ) == false )
			return false;
		ExtractItem.nSerial = pRecipe->GetItemSerial();
		ExtractItem.wDur = pRecipe->GetExp();

		if( m_pSession->GetItem()->CreateCashInvenWholeItem( ExtractItem ) != ERROR_NONE )
			return false;
	}

	bool bRet = m_pEventHandler->OnExtractRecipe( pRecipe, pItemData->IsCash );
	_ASSERT( bRet );

	SAFE_DELETE( pRecipe );
	return true;
}

int CSecondarySkillRepositoryServer::DoManufacture( SecondarySkill::CSManufacture* pPacket )
{
	CSecondarySkill* pSkill = Get( pPacket->iSkillID );
	if( pSkill == NULL )
		return ERROR_SECONDARYSKILL_MANUFACTURE_FAILED;

	CSecondarySkillRecipe* pRecipe = NULL;
	int iRet = CanManufacture( pSkill, pPacket->bIsStart, pPacket->iItemID, pRecipe );
	if( iRet != ERROR_NONE )
		return iRet;

	bool bRet = m_pEventHandler->OnManufacture( pSkill, pRecipe, pPacket->bIsStart );
	_ASSERT( bRet );

	return ERROR_NONE;
}

bool CSecondarySkillRepositoryServer::CancelManufacture( bool bSend/*=true*/ )
{
	CManufactureSkill* pManufacturingSkill = NULL;
	if( CanCancelManufacture( pManufacturingSkill ) != ERROR_NONE )
	{
		_ASSERT( bIsManufacturing() == false );
		return false;
	}

	_ASSERT( bIsManufacturing() );
	_ASSERT( pManufacturingSkill );
	m_pEventHandler->OnCancelManufacture( pManufacturingSkill, bSend );
	return true;
}

// 현재 제작중인 스킬 포인터 얻기
CSecondarySkill* CSecondarySkillRepositoryServer::GetManufacturingSkill()
{
	for( std::map<int,CSecondarySkill*>::iterator itor=m_mSecondarySkill.begin() ; itor!=m_mSecondarySkill.end() ; ++itor )
	{
		CSecondarySkill* pSkill = (*itor).second;

		if( pSkill->GetType() != SecondarySkill::Type::ManufactureSkill )
			continue;

		CManufactureSkill* pManufactureSkill = static_cast<CManufactureSkill*>(pSkill);

		if( pManufactureSkill->bIsManufacturing() )
			return pSkill;
	}

	return NULL;
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
