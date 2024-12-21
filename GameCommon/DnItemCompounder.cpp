#include "StdAfx.h"
#include "DnItemCompounder.h"
#include "SundriesFunc.h"
#include <mmsystem.h>

#ifdef _UNIT_TEST
#include "DNTableFile.h"
#else
#include "DnTableDB.h"
#endif

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnItemCompounder::CDnItemCompounder(void) : m_iLastError( 0 ), m_bCheckPossibility( true )
{

}

CDnItemCompounder::~CDnItemCompounder(void)
{
	SAFE_DELETE_PMAP( TMapItemCompoundInfo, m_mapCompoundInfo );
	SAFE_DELETE_PVEC( m_vlpPlateInfo );
}


#if defined (_WORK) && defined (_SERVER)
bool CDnItemCompounder::InitializeTable( bool bReLoad )
#else		//#if defined (_WORK) && defined (_SERVER)
bool CDnItemCompounder::InitializeTable( void )
#endif		//#if defined (_WORK) && defined (_SERVER)
{
	// TODO: 데이터 무결성도 검사하자. 결과물 아이템과 아이템 갯수 쌍이 안맞는다든지 등등.

	bool bResult = true;

	// 조합 테이블에서 데이터를 퍼옴.
#if defined (_WORK) && defined (_SERVER)

#ifdef _UNIT_TEST
	DNTableFileFormat* pCompoundTable = new DNTableFileFormat( "R:/GameRes/Resource/Ext/ItemCompoundTable.ext" );
#else				//#ifdef _UNIT_TEST
	DNTableFileFormat* pCompoundTable;
	if (bReLoad)
		pCompoundTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TITEMCOMPOUND );
	else
		pCompoundTable = GetDNTable( CDnTableDB::TITEMCOMPOUND );	
#endif				//#ifdef _UNIT_TEST

#else		//#if defined (_WORK) && defined (_SERVER)

#ifdef _UNIT_TEST
	DNTableFileFormat* pCompoundTable = new DNTableFileFormat( "R:/GameRes/Resource/Ext/ItemCompoundTable.ext" );
#else				//#ifdef _UNIT_TEST
	DNTableFileFormat* pCompoundTable = GetDNTable( CDnTableDB::TITEMCOMPOUND );
#endif				//#ifdef _UNIT_TEST

#endif		//#if defined (_WORK) && defined (_SERVER)

	char acBuf[ 128 ];
	ZeroMemory( acBuf, sizeof(acBuf) );

	int iNumItem = pCompoundTable->GetItemCount();
	for( int iItem = 0; iItem < iNumItem; ++iItem )
	{
		S_ITEM_COMPOUND_INFO* pNewCompoundInfo = new S_ITEM_COMPOUND_INFO;

		int iItemID = pCompoundTable->GetItemID( iItem );

		// 기본 정보
		pNewCompoundInfo->iCompoundTableID = iItemID;
		pNewCompoundInfo->iNameStringID = pCompoundTable->GetFieldFromLablePtr( iItemID, "_NameID" )->GetInteger();
		pNewCompoundInfo->iDescStringID = pCompoundTable->GetFieldFromLablePtr( iItemID, "_DescriptionID" )->GetInteger();
		pNewCompoundInfo->fSuccessPossibility = pCompoundTable->GetFieldFromLablePtr( iItemID, "_Possibility" )->GetFloat();
		pNewCompoundInfo->iCost = pCompoundTable->GetFieldFromLablePtr( iItemID, "_Cost" )->GetInteger();
		pNewCompoundInfo->fTimeRequired = pCompoundTable->GetFieldFromLablePtr( iItemID, "_TimeRequired" )->GetFloat();
		pNewCompoundInfo->cSuccessItemOptionIndex = (char)pCompoundTable->GetFieldFromLablePtr( iItemID, "_CompoundPotentialID" )->GetInteger();

		pNewCompoundInfo->iCompoundPreliminaryID = pCompoundTable->GetFieldFromLablePtr(iItemID, "_CompoundPreliminaryID")->GetInteger();
		// 확률이 0% 이면 실 데이터가 아님
		if( 0.0f == pNewCompoundInfo->fSuccessPossibility )
		{
//#ifndef _VILLAGESERVER
//			if( 0.0f != pNewCompoundInfo->fTimeRequired )
//				OutputDebug( "[CDnItemCompounder::InitializeTable()] %d 조합정보 로딩 실패. 확률 0%임.\n", iItemID );
//#endif
			SAFE_DELETE( pNewCompoundInfo );
			continue;
		}

		// 조합 성공 결과물 타입 및 결과물 아이템.. 성공과 실패시가 나눠져 있음.
		for( int i = 0;  i < NUM_MAX_RESULT_ITEM; ++i )
		{
			sprintf_s( acBuf, "_SuccessType%d", i+1 );
			pNewCompoundInfo->aiSuccessTypeOrCount[ i ] = pCompoundTable->GetFieldFromLablePtr( iItemID, acBuf )->GetInteger();

			sprintf_s( acBuf, "_SuccessItemID%d", i+1 );
			pNewCompoundInfo->aiSuccessItemID[ i ] = pCompoundTable->GetFieldFromLablePtr( iItemID, acBuf )->GetInteger();

#ifdef _CLIENT
	#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
			if (i == 0 && pNewCompoundInfo->aiSuccessItemID[i] > 0)
				m_SuccessIDsAsCompoundResultItem.insert(pNewCompoundInfo->aiSuccessItemID[i]);
	#endif
#endif

			sprintf_s( acBuf, "_FailType%d", i+1 );
			pNewCompoundInfo->aiFailTypeOrCount[ i ] = pCompoundTable->GetFieldFromLablePtr( iItemID, acBuf )->GetInteger();

			sprintf_s( acBuf, "_FailItemID%d", i+1 );
			pNewCompoundInfo->aiFailItemID[ i ] = pCompoundTable->GetFieldFromLablePtr( iItemID, acBuf )->GetInteger();
		}

		// 슬롯 정보
		for( int i = 0; i < NUM_MAX_NEED_ITEM; ++i )
		{
			sprintf_s( acBuf, "_Slot%dId", i+1 );
			pNewCompoundInfo->aiSlotItemID[ i ] = pCompoundTable->GetFieldFromLablePtr( iItemID, acBuf )->GetInteger();

			sprintf_s( acBuf, "_Slot%dNum", i+1 );
			pNewCompoundInfo->aiSlotItemCount[ i ] = pCompoundTable->GetFieldFromLablePtr( iItemID, acBuf )->GetInteger();
		}

		pNewCompoundInfo->bResultItemIsNeedItem = false;
		for( int i=0; i<NUM_MAX_RESULT_ITEM; i++ ) {
			for( int j=0; j<NUM_MAX_NEED_ITEM; j++ ) {
				if( pNewCompoundInfo->aiSuccessItemID[i] == pNewCompoundInfo->aiSlotItemID[j] && pNewCompoundInfo->aiSuccessItemID[i] != 0 ) {
					pNewCompoundInfo->bResultItemIsNeedItem = true;
					pNewCompoundInfo->abResultItemIsNeedItem[j] = true;
					break;
				}
			}
//			if( pNewCompoundInfo->bResultItemIsNeedItem ) break;
		}

		//m_vlpCompoundInfo.push_back( pNewCompoundInfo );
		m_mapCompoundInfo.insert( make_pair(pNewCompoundInfo->iCompoundTableID, pNewCompoundInfo) );
	}

#ifdef _UNIT_TEST
	delete pCompoundTable;
#endif

	// 플레이트 테이블에서 데이터 퍼옴.
#if defined (_WORK) && defined (_SERVER)

#ifdef _UNIT_TEST
	DNTableFileFormat* pPlateTable = new DNTableFileFormat( "R:/GameRes/Resource/Ext/PlateTable.ext" );
#else
	DNTableFileFormat* pPlateTable;
	if (bReLoad)
		pPlateTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPLATE );
	else
		pPlateTable = GetDNTable( CDnTableDB::TPLATE );	
#endif

#else		//#if defined (_WORK) && defined (_SERVER)

#ifdef _UNIT_TEST
	DNTableFileFormat* pPlateTable = new DNTableFileFormat( "R:/GameRes/Resource/Ext/PlateTable.ext" );
#else
	DNTableFileFormat* pPlateTable = GetDNTable( CDnTableDB::TPLATE );
#endif

#endif		//#if defined (_WORK) && defined (_SERVER)

	ZeroMemory( acBuf, sizeof(acBuf) );
	iNumItem = pPlateTable->GetItemCount();
	for( int iItem = 0; iItem < iNumItem; ++iItem )
	{
		S_PLATE_INFO* pNewPlateInfo = new S_PLATE_INFO;

		int iItemID = pPlateTable->GetItemID( iItem );

		pNewPlateInfo->iPlateItemID = pPlateTable->GetFieldFromLablePtr( iItemID, "_ItemID" )->GetInteger();

		// 아이템 아이디가 0인 것은 유효하지 않은 데이터
		// Note: 아이템 테이블 참조해서 실존하는 아이템인지 여부와 플레이트 타입 아이템인지 확인 필요.
		if( 0 == pNewPlateInfo->iPlateItemID ) {
			SAFE_DELETE( pNewPlateInfo );
			continue;
		}

		for( int i = 0; i < NUM_MAX_PLATE_COMPOUND_VARI; ++i )
		{
			sprintf_s( acBuf, "_CompoundTableIndex%d", i+1 );
			pNewPlateInfo->aiCompoundTableID[ i ] = pPlateTable->GetFieldFromLablePtr( iItemID, acBuf )->GetInteger();
		}

		m_vlpPlateInfo.push_back( pNewPlateInfo );
		m_mapPlateInfoByItemID.insert( make_pair(pNewPlateInfo->iPlateItemID, pNewPlateInfo) );
	}

#ifdef _UNIT_TEST
	delete pPlateTable;
#endif

	return bResult;
}


// UI 에 표시할 때 필요한 정보들.
void CDnItemCompounder::GetCompoundInfo( int iCompoundTableID, /*OUT*/S_COMPOUND_INFO_EXTERN* pInfo )
{
	if( pInfo == NULL || iCompoundTableID < 0 )
		return;

	map<int, S_ITEM_COMPOUND_INFO*>::iterator iterCompoundInfo = m_mapCompoundInfo.find( iCompoundTableID );
	if( m_mapCompoundInfo.end() == iterCompoundInfo )
		return;

	const S_ITEM_COMPOUND_INFO* pCompoundInfo = iterCompoundInfo->second;//m_vlpCompoundInfo.at( iCompoundTableID-1 );
	pInfo->iCompoundTableID = pCompoundInfo->iCompoundTableID;
	memcpy_s( pInfo->aiSuccessTypeOrCount, sizeof(pInfo->aiSuccessTypeOrCount), pCompoundInfo->aiSuccessTypeOrCount, sizeof(pCompoundInfo->aiSuccessTypeOrCount) );
	memcpy_s( pInfo->aiSuccessItemID, sizeof(pInfo->aiSuccessItemID), pCompoundInfo->aiSuccessItemID, sizeof(pCompoundInfo->aiSuccessItemID) );
	//pInfo->iNameStringID = pCompoundInfo->iNameStringID;
	//pInfo->iDescStringID = pCompoundInfo->iDescStringID;
	memcpy_s( pInfo->aiItemID, sizeof(pInfo->aiItemID), pCompoundInfo->aiSlotItemID, sizeof(pCompoundInfo->aiSlotItemID) );
	memcpy_s( pInfo->aiItemCount, sizeof(pInfo->aiItemCount), pCompoundInfo->aiSlotItemCount, sizeof(pCompoundInfo->aiSlotItemCount) );
	pInfo->iCost= pCompoundInfo->iCost;
	pInfo->fSuccessPossibility = pCompoundInfo->fSuccessPossibility;
	pInfo->fTimeRequired = pCompoundInfo->fTimeRequired;
	pInfo->bResultItemIsNeedItem = pCompoundInfo->bResultItemIsNeedItem;
	memcpy_s( pInfo->abResultItemIsNeedItem, sizeof(pInfo->abResultItemIsNeedItem), pCompoundInfo->abResultItemIsNeedItem, sizeof(pCompoundInfo->abResultItemIsNeedItem) );

	pInfo->iCompoundPreliminaryID = pCompoundInfo->iCompoundPreliminaryID;
}


int CDnItemCompounder::GetEmblemCompoundInfoCountByPlate( int iPlateItemID )
{
	if( iPlateItemID < 0 || iPlateItemID-1 >= (int)m_vlpPlateInfo.size() )
		return -1;

	return (int)m_vlpPlateInfo.size();
}


void CDnItemCompounder::GetEmblemCompoundInfo( int iPlateItemID, int iCompoundTableID, /*OUT*/S_COMPOUND_INFO_EXTERN* pInfo )
{
	if( pInfo == NULL || iPlateItemID < 0 || iPlateItemID-1 >= (int)m_vlpPlateInfo.size() ||
		iCompoundTableID < 0 )
		return;

	// 맵에서 뒤져봄.
	map<int, S_ITEM_COMPOUND_INFO*>::iterator iterCompoundInfo = m_mapCompoundInfo.find( iCompoundTableID );
	if( m_mapCompoundInfo.end() == iterCompoundInfo )
		return;

	// 플레이트 정보를 얻어옴.
	bool bValidEmblemCompound = false;
	int iSelectedCompoundTableID = -1;
	map<int, S_PLATE_INFO*>::iterator iter = m_mapPlateInfoByItemID.find( iPlateItemID );
	if( m_mapPlateInfoByItemID.end() != iter )
	{
		S_PLATE_INFO* pPlateInfo = iter->second;

		for( int i = 0; i < NUM_MAX_PLATE_COMPOUND_VARI; ++i )
		{
			if( iCompoundTableID == pPlateInfo->aiCompoundTableID[ i ] )
			{
				iSelectedCompoundTableID = iCompoundTableID;
				bValidEmblemCompound = true;
				break;
			}
		}
	}

	if( bValidEmblemCompound )
		GetCompoundInfo( iSelectedCompoundTableID, pInfo );
}


void CDnItemCompounder::GetEmblemCompoundInfoByIndex( int iPlateItemID, int iIndex, /*OUT*/S_COMPOUND_INFO_EXTERN* pInfo )
{
	if( pInfo == NULL || iPlateItemID < 0 || iPlateItemID-1 >= (int)m_vlpPlateInfo.size() ||
		iIndex < 0 || iIndex >= NUM_MAX_PLATE_COMPOUND_VARI )
		return;

	// 플레이트 정보를 얻어옴.
	bool bValidEmblemCompound = false;
	int iSelectedCompoundTableID = -1;
	map<int, S_PLATE_INFO*>::iterator iter = m_mapPlateInfoByItemID.find( iPlateItemID );
	if( m_mapPlateInfoByItemID.end() != iter )
	{
		S_PLATE_INFO* pPlateInfo = iter->second;
		int iCompoundTableID = pPlateInfo->aiCompoundTableID[ iIndex ];

		GetCompoundInfo( iCompoundTableID, pInfo );
	}
}


const CDnItemCompounder::S_PLATE_INFO* CDnItemCompounder::GetPlateInfoByItemID( int iPlateTableID )
{
	map<int, S_PLATE_INFO*>::iterator iter = m_mapPlateInfoByItemID.find( iPlateTableID );
	if( m_mapPlateInfoByItemID.end() != iter )
	{
		return iter->second;	
	}

	return NULL;
}


void CDnItemCompounder::_ResetOutputInfo( S_OUTPUT* pOutput )
{
	if( NULL == pOutput )
		return;

	pOutput->vlItemID.clear();
	pOutput->vlItemCount.clear();
	pOutput->cItemOptionIndex = 0;

	pOutput->iCost = 0;
	pOutput->eResultCode = R_SUCCESS;
	pOutput->eErrorCode = E_NONE;
	pOutput->fTimeRequired = 0.0f;
}



void CDnItemCompounder::ValidateCompound( S_ITEM_SETTING_INFO &Input, S_OUTPUT* pResult )
{
	_ASSERT( 0 < Input.iDiscountedCost );

	_ResetOutputInfo( pResult );

	// 맵에서 뒤져봄.
	map<int, S_ITEM_COMPOUND_INFO*>::iterator iterCompoundInfo = m_mapCompoundInfo.find( Input.iCompoundTableID );
	if( m_mapCompoundInfo.end() == iterCompoundInfo )
		return;

	if( NULL == pResult )
	{
		OutputDebug( "[EmblemCompound Error] 잘못된 입력 값입니다.\n" );
		return;
	}

	// 조합 정보를 갖고 옴
	S_ITEM_COMPOUND_INFO* pCompoundInfo = iterCompoundInfo->second;//m_vlpCompoundInfo.at( Input.iCompoundTableID-1 );

	// 돈은 충분한가.
	INT64 iCost = pCompoundInfo->iCost;
	if( 0 < Input.iDiscountedCost )
		iCost = Input.iDiscountedCost;

	if( Input.iHasMoney < iCost )
	{
		pResult->eErrorCode = E_NOT_ENOUGH_MONEY;
		pResult->eResultCode = R_ERROR;
		return;
	}

	// 슬롯에 맞게 들어가 있는지. 슬롯엔 맞으나 갯수가 부족한지 확인.
	for( int i  = 0; i < NUM_MAX_NEED_ITEM; ++i )
	{
		if( 0 == pCompoundInfo->aiSlotItemID[ i ] )
			break;

		if( 0 == Input.aiItemID[ i ] )
		{
			pResult->eErrorCode = E_NOT_ENOUGH_ITEM;
			pResult->eResultCode = R_ERROR;
		}
		else
		if( pCompoundInfo->aiSlotItemID[ i ] != Input.aiItemID[ i ] )
		{
			pResult->eErrorCode = E_NOT_MATCH_SLOT;
			pResult->eResultCode = R_ERROR;
			return;
		}
		else
		if( pCompoundInfo->aiSlotItemCount[ i ] != Input.aiItemCount[ i ] )
		{
			pResult->eErrorCode = E_NOT_ENOUGH_ITEM_COUNT;
			pResult->eResultCode = R_ERROR;
			return;
		}
	}
}


void CDnItemCompounder::ValidateEmblemCompound( int iPlateItemID, S_ITEM_SETTING_INFO &Input, S_OUTPUT* pResult )
{
	ValidateCompound( Input, pResult );

	if( pResult && R_ERROR != pResult->eResultCode )
	{
		if( iPlateItemID < 0 )
		{	
			pResult->eErrorCode = E_INVALID_CALL;
			pResult->eResultCode = R_ERROR;
			return;
		}

		bool bValidEmblemCompound = false;

		// 플레이트 정보를 얻어옴.
		map<int, S_PLATE_INFO*>::iterator iter = m_mapPlateInfoByItemID.find( iPlateItemID );
		if( m_mapPlateInfoByItemID.end() != iter )
		{
			S_PLATE_INFO* pPlateInfo = iter->second;

			for( int i = 0; i < NUM_MAX_PLATE_COMPOUND_VARI; ++i )
			{
				if( Input.iCompoundTableID == pPlateInfo->aiCompoundTableID[ i ] )
				{
					bValidEmblemCompound = true;
					break;
				}
			}
		}

		if( false == bValidEmblemCompound )
		{
			pResult->eErrorCode = E_NOT_MATCH_PLATE_WITH_ITEM_COMPOUND;
			pResult->eResultCode = R_ERROR;
			return;
		}
	}
}

#if defined( _GAMESERVER )
void CDnItemCompounder::_Compound( CMultiRoom* pRoom, S_ITEM_SETTING_INFO& Input, S_OUTPUT* pResult )
#else
void CDnItemCompounder::_Compound( S_ITEM_SETTING_INFO& Input, S_OUTPUT* pResult )
#endif // #if defined( _GAMESERVER )
{
	static long s_CompoundInterlocked = 0;
	CMtRandom Random;
	Random.srand( timeGetTime()+InterlockedIncrement(&s_CompoundInterlocked) );

	// 모두 다 제대로 들어왔다면 만들어준다.
	if( pResult && R_ERROR != pResult->eResultCode )
	{
		// 맵에서 뒤져봄.
		map<int, S_ITEM_COMPOUND_INFO*>::iterator iterCompoundInfo = m_mapCompoundInfo.find( Input.iCompoundTableID );
		if( m_mapCompoundInfo.end() == iterCompoundInfo )
		{

#ifndef _VILLAGESERVER
			_ASSERT( !"말도 안됨. 위에서 다 검증했는데 여기서 조합테이블 id가 잘못됐다고? -_-" );
#else
			g_Log.Log( LogType::_ERROR, L"[아이템 조합] 조합테이블: %d,  받아놓은 아이템 조합 정보 없음. \n", Input.iCompoundTableID );
#endif
			pResult->eResultCode = R_ERROR;
			pResult->eErrorCode = E_INVALID_CALL;
			return;
		}

		S_ITEM_COMPOUND_INFO* pCompoundInfo = iterCompoundInfo->second;//m_vlpCompoundInfo.at( Input.iCompoundTableID-1 );

		//float r = (float)_rand() / ((float)(RAND_MAX) + 1.0f );
		//if( r <= pCompoundInfo->fSuccessPossibility || !m_bCheckPossibility )
		int iRandom = Random.rand() % 10000;
		if( iRandom <= int(pCompoundInfo->fSuccessPossibility * 10000.0f) || !m_bCheckPossibility )
		{
			// 조합 성공.
			for( int i = 0; i < NUM_MAX_RESULT_ITEM; ++i )
			{
				if( 0 == pCompoundInfo->aiSuccessItemID[ i ] )
					break;

				pResult->vlItemCount.push_back( pCompoundInfo->aiSuccessTypeOrCount[ i ] );
				pResult->vlItemID.push_back( pCompoundInfo->aiSuccessItemID[ i ] );
			}

			pResult->cItemOptionIndex = pCompoundInfo->cSuccessItemOptionIndex;
			pResult->iCost = pCompoundInfo->iCost;
			pResult->fTimeRequired = pCompoundInfo->fTimeRequired;
			pResult->eResultCode = R_SUCCESS;
		}
		else
		{
			// 조합 실패. 
			for( int i = 0; i < NUM_MAX_RESULT_ITEM; ++i )
			{
				if( 0 == pCompoundInfo->aiFailItemID[ i ] )
					break;

				pResult->vlItemCount.push_back( pCompoundInfo->aiFailTypeOrCount[ i ] ); 
				pResult->vlItemID.push_back( pCompoundInfo->aiFailItemID[ i ] );
			}

			pResult->iCost = pCompoundInfo->iCost;
			pResult->fTimeRequired = pCompoundInfo->fTimeRequired;
			pResult->eResultCode = R_POSSIBILITY_FAIL;
		}

		if( 0 < Input.iDiscountedCost )
			pResult->iCost = Input.iDiscountedCost;
	}
}


// 문장 보옥 조합
#if defined( _GAMESERVER )
void CDnItemCompounder::EmblemCompound( CMultiRoom* pRoom, int iPlateTableID, S_ITEM_SETTING_INFO& Input, /*OUT*/S_OUTPUT* pResult )
#else
void CDnItemCompounder::EmblemCompound( int iPlateTableID, S_ITEM_SETTING_INFO& Input, /*OUT*/S_OUTPUT* pResult )
#endif // #if defined( _GAMESERVER )
{
	ValidateEmblemCompound( iPlateTableID, Input, pResult );

#if defined( _GAMESERVER )
	_Compound(pRoom,Input, pResult);
#else
	_Compound(Input, pResult);
#endif // #if defined( _GAMESERVER )
}

#if defined( _GAMESERVER )
void CDnItemCompounder::Compound( CMultiRoom* pRoom, S_ITEM_SETTING_INFO& Input, /*OUT*/S_OUTPUT* pResult )
#else
void CDnItemCompounder::Compound( S_ITEM_SETTING_INFO& Input, /*OUT*/S_OUTPUT* pResult )
#endif // #if defined( _GAMESERVER )
{
	// 조합 정보를 갖고 옴
	ValidateCompound( Input, pResult );
#if defined( _GAMESERVER )
	_Compound( pRoom, Input, pResult );
#else
	_Compound( Input, pResult );
#endif // #if defined( _GAMESERVER )
}

#ifdef _CLIENT
	#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
bool CDnItemCompounder::IsEnableCompoundItem(int itemId) const
{
	std::set<int>::const_iterator iter = m_SuccessIDsAsCompoundResultItem.find(itemId);
	return (iter != m_SuccessIDsAsCompoundResultItem.end());
}
	#endif
#endif