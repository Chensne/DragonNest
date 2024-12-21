#include "StdAfx.h"
#include "DnEmblemFactory.h"
#include "SundriesFunc.h"

CDnEmblemFactory::CDnEmblemFactory(void)
{
}

CDnEmblemFactory::~CDnEmblemFactory(void)
{
	SAFE_DELETE_PVEC( m_vlpCompoundInfo );
	SAFE_DELETE_PVEC( m_vlpPlateInfo );
	SAFE_DELETE_PVEC( m_vlpJewelInfo );
}


bool CDnEmblemFactory::InitializeTable( void )
{
	bool bResult = true;

	// 조합 테이블 데이터 퍼옴.
	//DNTableFileFormat* pCompoundTable = new DNTableFileFormat( "R:/GameRes/Resource/Ext/EmblemCompoundTable.ext" );
	DNTableFileFormat* pCompoundTable = GetDNTable( CDnTableDB::TEMBLEMCOMPOUND );

	char acBuf[ 256 ];
	int iNumItem = pCompoundTable->GetItemCount();
	m_vlpPlateInfo.reserve( iNumItem );
	for( int iItem = 1; iItem <= iNumItem; ++iItem )
	{
		S_EMBLEM_COMPOUND_INFO* pNewCompoundInfo = new S_EMBLEM_COMPOUND_INFO;

		for( DWORD dwHole = 1; dwHole <= NUM_MAX_PLATE_HOLE; ++dwHole )
		{
			ZeroMemory( acBuf, sizeof(acBuf) );
			sprintf_s( acBuf, "_Hole%d_ItemID", dwHole );
			int iJewelItemID = pCompoundTable->GetFieldFromLablePtr( iItem, acBuf )->GetInteger();
			if( iJewelItemID > 0 )		// 0 번 아이템 인덱스는 테이블에서 디폴트값, 존재하지 않는 테이블 인덱스이므로 넣지 않는다.
				pNewCompoundInfo->setHoleItemID.insert( iJewelItemID );

			ZeroMemory( acBuf, sizeof(acBuf) );
			sprintf_s( acBuf, "_AllowShowHoleInfo_%d", dwHole );
			pNewCompoundInfo->abShowHoleInfo[dwHole-1] = pCompoundTable->GetFieldFromLablePtr( iItem, acBuf )->GetInteger() ? true : false;
		}

		pNewCompoundInfo->iResultItemID = pCompoundTable->GetFieldFromLablePtr( iItem, "_ResultItemIndex" )->GetInteger();
		pNewCompoundInfo->bAllowShowResult = pCompoundTable->GetFieldFromLablePtr( iItem, "_AllowShowResult" )->GetInteger() ? true : false;

		m_vlpCompoundInfo.push_back( pNewCompoundInfo );
	}

	// 플레이트 정보 퍼옴
	DNTableFileFormat* pPlateTable = GetDNTable( CDnTableDB::TPLATE );
	//DNTableFileFormat* pPlateTable = new DNTableFileFormat( "R:/GameRes/Resource/Ext/PlateTable.ext" );

	iNumItem = pPlateTable->GetItemCount();
	m_vlpPlateInfo.reserve( iNumItem );
	for( int iItem = 1; iItem <= iNumItem; ++iItem )
	{
		S_PLATE_INFO* pNewPlateInfo = new S_PLATE_INFO;
		pNewPlateInfo->iItemID = pPlateTable->GetFieldFromLablePtr( iItem, "_ItemID" )->GetInteger();

		// 다른 플레이트에 같은 아이템 아이디가 있는지 검증한다.
#ifdef _DEBUG
		if( 0 != pNewPlateInfo->iItemID )
		{
			vector<S_PLATE_INFO*>::iterator iter = find_if( m_vlpPlateInfo.begin(), m_vlpPlateInfo.end(), 
															FindByItemID<S_PLATE_INFO>(pNewPlateInfo->iItemID) );
			if( iter != m_vlpPlateInfo.end() )
			{
				_ASSERT( "플레이트 테이블에 중복된 아이템 인덱스가 있습니다!" );

				// 메모리 해제도 안되고 리턴. 이런 경우는 있으면 안되기 때문에..
				return false;
			}
		}
#endif

		const char* pUIFileName = pPlateTable->GetFieldFromLablePtr( iItem, "_BackgroundTexture" )->GetString();
		//if( NULL == pUIFileName )
		//{
		//	//OutputDebug( "[Plate Table Error!!] ItemID: %d UIFile 항목이 비어 있습니다!\n", iItem );
		//	//_ASSERT( ! );
		//	//return false;
		//	//continue;
		//}
		//// 제대로 데이터 써 넣으면 else 빼내야 함..
		//else
		pNewPlateInfo->strBGTextureFileName.assign( pUIFileName );
		pNewPlateInfo->iSlotTypeIndex = pPlateTable->GetFieldFromLablePtr( iItem, "_SlotTypeIndex" )->GetInteger();
		pNewPlateInfo->iNumSlot = pPlateTable->GetFieldFromLablePtr( iItem, "_SlotNumber" )->GetInteger();

		char acBuf[ 256 ];
		ZeroMemory( acBuf, sizeof(acBuf) );
		for( DWORD i = 0; i < NUM_MAX_JEWEL_COMPOUND; ++i )
		{
			// 0 번이면 없는 것임
			sprintf_s( acBuf, "_CompoundTableIndex%d", i+1 );
			pNewPlateInfo->aiCompoundTableIndex[ i ] = pPlateTable->GetFieldFromLablePtr( iItem, acBuf )->GetInteger();

			if( pNewPlateInfo->aiCompoundTableIndex[ i ] > 0 )
			{
				int iNumCompoundJewelSlot = (int)m_vlpCompoundInfo.at( pNewPlateInfo->aiCompoundTableIndex[ i ] - 1 )->setHoleItemID.size();
				if( iNumCompoundJewelSlot != pNewPlateInfo->iNumSlot )
				{
					OutputDebug( "[EmblemFactory Error] 플레이트와 조합테이블에서 슬롯 갯수가 서로 다름 (Plate Table ID: %d, %d번째 조합, 슬롯:%d,  Compound Table ID: %d 슬롯:%d)\n", 
								 iItem, i+1, pNewPlateInfo->iNumSlot,
								 pNewPlateInfo->aiCompoundTableIndex[ i ],
								 iNumCompoundJewelSlot );
					bResult = false;
				}
			}
		}

		pNewPlateInfo->bLostJewelWhenFailed = pPlateTable->GetFieldFromLablePtr( iItem, "_LostJewelWhenFailed" )->GetInteger() ? true : false;
		pNewPlateInfo->bLostPlateWhenFailed = pPlateTable->GetFieldFromLablePtr( iItem, "_LostPlateWhenFailed" )->GetInteger() ? true : false;
		pNewPlateInfo->iCompositionFailProb = pPlateTable->GetFieldFromLablePtr( iItem, "_CompositionFailProb" )->GetInteger();
		pNewPlateInfo->bAllowShowProb = pPlateTable->GetFieldFromLablePtr( iItem, "_AllowShowProb" )->GetInteger() ? true : false;
		pNewPlateInfo->bLostJewelWhenFailedProb = pPlateTable->GetFieldFromLablePtr( iItem, "_LostJewelWhenFailedProb" )->GetInteger() ? true : false;
		pNewPlateInfo->bLostPlateWhenFailedProb = pPlateTable->GetFieldFromLablePtr( iItem, "_LostPlateWhenFailedProb" )->GetInteger() ? true : false;
		pNewPlateInfo->iCost = pPlateTable->GetFieldFromLablePtr( iItem, "_Cost" )->GetInteger();

		m_vlpPlateInfo.push_back( pNewPlateInfo );
	}

	// Jewel 테이블 정보 퍼옴
	//DNTableFileFormat* pJewelTable = new DNTableFileFormat( "R:/GameRes/Resource/Ext/JewelTable.ext" );
	DNTableFileFormat* pJewelTable = GetDNTable( CDnTableDB::TJEWEL );
	iNumItem = pJewelTable->GetItemCount();
	m_vlpJewelInfo.reserve( iNumItem );
	for( int iItem = 1; iItem <= iNumItem; ++iItem )
	{
		S_JEWEL_INFO* pNewJewelInfo = new S_JEWEL_INFO;

		pNewJewelInfo->iItemID = pJewelTable->GetFieldFromLablePtr( iItem, "_ItemID" )->GetInteger();
		pNewJewelInfo->iGrade = pJewelTable->GetFieldFromLablePtr( iItem, "_Grade" )->GetInteger();
		pNewJewelInfo->iColor = pJewelTable->GetFieldFromLablePtr( iItem, "_Color" )->GetInteger();
		pNewJewelInfo->iNeedUpgradeCount = pJewelTable->GetFieldFromLablePtr( iItem, "_NeedUpgradeCount" )->GetInteger();
		pNewJewelInfo->iNextGradeItemIndex = pJewelTable->GetFieldFromLablePtr( iItem, "_NextGradeItemIndex" )->GetInteger();
		pNewJewelInfo->iCost = pJewelTable->GetFieldFromLablePtr( iItem, "_Cost" )->GetInteger();

		m_vlpJewelInfo.push_back( pNewJewelInfo );
	}

	return bResult;
}


int CDnEmblemFactory::_ValidateUpgradeJewel( S_JEWEL_INFO* pJewelInfo, int iNumJewel )
{
	// TODO: 아이템 아이디 얻어서 가능한 등급인지 체크
	//pJewelInfo->iItemID
	int iResult = 0;

	if( pJewelInfo->iNeedUpgradeCount <= iNumJewel )
	{
		iResult = pJewelInfo->iNextGradeItemIndex;
	}

	return iResult;
}


int CDnEmblemFactory::_ValidateCompound( S_PLATE_INFO* pPlateInfo, const multiset<int>& setJewelItemID )
{
	int iResult = 0;

	// 이 플레이트에서 조합할 수 있는 경우의 수를 뽑아온다. (최대 10개
	for( int i = 0; i < NUM_MAX_PLATE_HOLE; ++i )
	{	
		int iCompoundIndex = pPlateInfo->aiCompoundTableIndex[ i ];
		if( iCompoundIndex <= 0 )
			continue;

		// 마찬가지로 테이블 인덱스는 1부터 시작하므로..
		S_EMBLEM_COMPOUND_INFO* pCompoundInfo = m_vlpCompoundInfo.at( iCompoundIndex-1 );
		if( pCompoundInfo->setHoleItemID == setJewelItemID )
		{
			iResult = iCompoundIndex;
			break;
		}
	}

	return iResult;
}


int CDnEmblemFactory::UpgradeJewel( int iJewelItemID, int iNumJewel, /*OUT*/ int* pCost, /*OUT*/ int* pNumUse )
{
	int iNewJewelItemID = 0;

	vector<S_JEWEL_INFO*>::iterator iter = find_if( m_vlpJewelInfo.begin(), m_vlpJewelInfo.end(),
													FindByItemID<S_JEWEL_INFO>(iJewelItemID) );
	if( m_vlpJewelInfo.end() == iter )
		return false;

	S_JEWEL_INFO* pJewelInfo = *iter;

	iNewJewelItemID = _ValidateUpgradeJewel( pJewelInfo, iNumJewel );
	if( iNewJewelItemID > 0 )
	{
		if( pCost )
			*pCost = pJewelInfo->iCost;

		if( pNumUse )
			*pNumUse = pJewelInfo->iNeedUpgradeCount;
	}

	return iNewJewelItemID;
}


int CDnEmblemFactory::Compound( int iPlateItemID, const multiset<int>& setJewelItemID, int* pCost )
{
	int iEmblemItemID = 0;

	// 테이블 인덱스는 1부터 시작하므로..
	vector<S_PLATE_INFO*>::iterator iter = find_if( m_vlpPlateInfo.begin(), m_vlpPlateInfo.end(), 
													FindByItemID<S_PLATE_INFO>(iPlateItemID) );
	if( m_vlpPlateInfo.end() == iter )
		return false;

	S_PLATE_INFO* pPlateInfo = *iter;

	int iResult = _ValidateCompound( pPlateInfo, setJewelItemID );
	if( iResult > 0 )
	{
		// 테이블의 인덱스는 1부터 시작한다.. 리스트 시작은 0 이므로,
		iEmblemItemID = m_vlpCompoundInfo.at( iResult-1 )->iResultItemID;

		_ASSERT( pCost && "비용은 반드시 리턴 받아야 함.." );
		if( pCost )
			*pCost = pPlateInfo->iCost;
	}

	return iEmblemItemID;
}




// 데이터 조회 함수들
const CDnEmblemFactory::S_EMBLEM_COMPOUND_INFO* CDnEmblemFactory::GetEmblemCompoundInfo( int iIndex ) const
{
	const S_EMBLEM_COMPOUND_INFO* pResult = NULL;
	
	if( iIndex < (int)m_vlpCompoundInfo.size() )
		pResult = m_vlpCompoundInfo.at( iIndex );

	return pResult;
}


const CDnEmblemFactory::S_PLATE_INFO* CDnEmblemFactory::GetPlateInfoByIndex( int iIndex ) const
{
	const S_PLATE_INFO* pResult = NULL;

	if( iIndex < (int)m_vlpPlateInfo.size() )
		pResult = m_vlpPlateInfo.at( iIndex );

	return pResult;
}


const CDnEmblemFactory::S_PLATE_INFO* CDnEmblemFactory::GetPlateInfoByItemID( int iItemID ) const
{
	const S_PLATE_INFO* pResult = NULL;

	vector<S_PLATE_INFO*>::const_iterator iter = find_if( m_vlpPlateInfo.begin(), m_vlpPlateInfo.end(), 
													FindByItemID<S_PLATE_INFO>(iItemID) );
	if( m_vlpPlateInfo.end() != iter )
		pResult = *iter;

	return pResult;
}



const CDnEmblemFactory::S_JEWEL_INFO* CDnEmblemFactory::GetJewelInfoByIndex( int iIndex ) const
{
	const S_JEWEL_INFO* pResult = NULL;

	if( iIndex < (int)m_vlpJewelInfo.size() )
		pResult = m_vlpJewelInfo.at(iIndex);

	return pResult;
}


const CDnEmblemFactory::S_JEWEL_INFO* CDnEmblemFactory::GetJewelInfoByItemID( int iItemID ) const
{
	const S_JEWEL_INFO* pResult = NULL;

	vector<S_JEWEL_INFO*>::const_iterator iter = find_if( m_vlpJewelInfo.begin(), m_vlpJewelInfo.end(), 
													FindByItemID<S_JEWEL_INFO>(iItemID) );

	if( m_vlpJewelInfo.end() != iter )
		pResult = *iter;

	return pResult;
}