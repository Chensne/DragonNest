#include "StdAfx.h"
#include "DnSkillTreeSystem.h"
#include "DnTableDB.h"
#if defined(_VILLAGESERVER) || defined(_GAMESERVER)
#include "DNGameDataManager.h"
#else
#include "DNTableFile.h"
#endif
#include "DnCommonUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnSkillTreeSystem::CDnSkillTreeSystem(void)
{
}

CDnSkillTreeSystem::~CDnSkillTreeSystem(void)
{
	SAFE_DELETE_PVEC( m_vlpJobSkillTreeList );
}


bool CDnSkillTreeSystem::InitializeTable( void )
{
	bool bResult = true;

	// 스킬 테이블로부터 필요 직업 정보/ 락/언락 정보 및 필요한 스킬북 ItemID 를 얻어옴.
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
	DNTableFileFormat* pSkillTreeTable = GetDNTable( CDnTableDB::TSKILLTREE );

	int iNumSkill = pSkillTable->GetItemCount();
	for( int iIndex = 0; iIndex < iNumSkill; ++iIndex )
	{
		int iSkillID = pSkillTable->GetItemID( iIndex );

		// 트리테이블에서 연관된 테이블 ID 얻어옴.
		vector<int> vlSkillTreeTableItemID;
		pSkillTreeTable->GetItemIDListFromField( "_SkillTableID", iSkillID, vlSkillTreeTableItemID );

		if( (int)vlSkillTreeTableItemID.empty() )
			continue;
		
		_ASSERT( (int)vlSkillTreeTableItemID.size() == 1 );
		int iTreeTableID = vlSkillTreeTableItemID.front();

		// 스킬 최초 획득시 필요한 SP 는 스킬 레벨테이블에 있는 필요 SP 1렙짜리에서 얻어온다.
		vector<int> vlSkillLevelList;
		// pve 기준으로 처리하면 된다.
		pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillID, vlSkillLevelList );
		
		// pve, pvp 대상인지 확인하여 걸러냄.
		vector<int>::iterator iterLevelList = vlSkillLevelList.begin();
		for( iterLevelList; iterLevelList != vlSkillLevelList.end(); )
		{
			int iSkillLevelTableID = *iterLevelList;
			int iApplyType = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_ApplyType" )->GetInteger();
			if( iApplyType != 0 )
				iterLevelList = vlSkillLevelList.erase( iterLevelList );
			else
				++iterLevelList;
		}

		_ASSERT( false == vlSkillLevelList.empty() );
		//
		if (vlSkillLevelList.empty() == true)
			continue;

		int iSkillLevelTableID = vlSkillLevelList.front();
		int iNeedJobID = pSkillTable->GetFieldFromLablePtr( iSkillID, "_NeedJob" )->GetInteger();

		map<int, S_SKILLTREE_INFO*>::iterator iter = m_mapSkillTreeByJobID.find( iNeedJobID );
		if( m_mapSkillTreeByJobID.end() == iter )
		{
			S_SKILLTREE_INFO* pNewTree = new S_SKILLTREE_INFO;
			pNewTree->iJobID = iNeedJobID;
			m_mapSkillTreeByJobID.insert( make_pair(iNeedJobID, pNewTree) );
			m_vlpJobSkillTreeList.push_back( pNewTree );

			pNewTree->vlNodeRenderInfo.assign( MAX_SKILL_SLOT_COUNT, S_NODE_RENDER_INFO() );
		}

		S_SKILLTREE_INFO* pTree = m_mapSkillTreeByJobID[ iNeedJobID ];

		S_NODE_INFO* pNodeInfo = new S_NODE_INFO;
		pNodeInfo->iSkillID = iSkillID;
		pNodeInfo->iJobID = iNeedJobID;
		pNodeInfo->iNeedSkillPointToAcquire = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
		pNodeInfo->iTreeSlotIndex = pSkillTreeTable->GetFieldFromLablePtr( iTreeTableID, "_TreeSlotIndex" )->GetInteger();
		pNodeInfo->bDefaultLocked = (pSkillTable->GetFieldFromLablePtr( iSkillID, "_Lock" )->GetInteger() == 1) ? true : false;
		
#if defined( PRE_ADD_PRESET_SKILLTREE )
		char szLabel[256] = {0,};
		for( int itr = 0; itr < E_ADVICE_SKILL_LEVEL_COUNT; ++itr )
		{
			sprintf_s( szLabel, _countof(szLabel), "_AdviceSkillLevel%d", itr + 1 );
			int nAdviceSkillLevel = pSkillTreeTable->GetFieldFromLablePtr( iTreeTableID, szLabel )->GetInteger();
			pNodeInfo->vecAdviceSkillLevel.push_back( nAdviceSkillLevel );
		}
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )

#if defined( PRE_ADD_ONLY_SKILLBOOK )
		pNodeInfo->bNeedSkillBook = (pSkillTreeTable->GetFieldFromLablePtr( iTreeTableID, "_NeedSkillBook" )->GetInteger() == 1) ? true : false;
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )

		// 획득조건 정보
		char caLabel[ 32 ];
		pNodeInfo->UnLockCondition.iUnlockSkillBookItemID = pSkillTable->GetFieldFromLablePtr( iSkillID, "_UnlockSkillBookItemID" )->GetInteger();
		pNodeInfo->AcquireCondition.iNeedLevel = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_LevelLimit" )->GetInteger();

		for( int i = 0; i < MAX_PARENT_SKILL_COUNT; ++i )
		{
			// 정의되지 않은 값은 0 일 수도 있다.
			sprintf_s( caLabel, "_NeedParentSkillLevel%d", i + 1 );
			int iNeedLevel = pSkillTreeTable->GetFieldFromLablePtr( iTreeTableID, caLabel )->GetInteger();

			if( 0 != iNeedLevel )
				pNodeInfo->AcquireCondition.vlParentSkillNeedLevel.push_back( iNeedLevel );
		}

		// 최대 슬롯 크기가 넘어갔음. 프로그램을 종료시키는 것 이외에는 답이 없다!
		_ASSERT( pNodeInfo->iTreeSlotIndex < (int)pTree->vlNodeRenderInfo.size() );

		pTree->vlNodeRenderInfo.at( pNodeInfo->iTreeSlotIndex ) = S_NODE_RENDER_INFO( iSkillID, pNodeInfo->iTreeSlotIndex, iNeedJobID, 
																					  &pNodeInfo->UnLockCondition, &pNodeInfo->AcquireCondition );
		pNodeInfo->pNodeRenderInfo = &(pTree->vlNodeRenderInfo[pNodeInfo->iTreeSlotIndex]);
		pTree->vlpNodeList.push_back( pNodeInfo );
		pTree->mapNodeBySkillID.insert( make_pair(iSkillID, pNodeInfo) );
		m_mapNodeListBySkillBookID.insert( make_pair(pNodeInfo->UnLockCondition.iUnlockSkillBookItemID, pNodeInfo) );
	}

	// 부모자식 구성 정보 및 트리 구성에 대한 데이터를 구성한다.
	_MakeupParentSkillLinks( pSkillTreeTable, pSkillTable );
	
	return bResult;
}


#if defined(_VILLAGESERVER) || defined(_GAMESERVER)
bool CDnSkillTreeSystem::InitializeTableUsingDataManager( CDNGameDataManager* pDataManager )
{
	bool bResult = true;

	// 스킬 테이블로부터 필요 직업 정보/ 락/언락 정보 및 필요한 스킬북 ItemID 를 얻어옴.
#ifdef _WORK
	DNTableFileFormat* pSkillTable;
	if (pDataManager->GetLoaded())
		pSkillTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSKILL );
	else
		pSkillTable = GetDNTable( CDnTableDB::TSKILL );
#else
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
#endif

	int iNumSkill = pSkillTable->GetItemCount();
	for( int iIndex = 0; iIndex < iNumSkill; ++iIndex )
	{
		int iSkillID = pSkillTable->GetItemID( iIndex );

		// 트리테이블에서 연관된 테이블 ID 얻어옴.
		TSkillTreeData* pSkillTreeData = pDataManager->GetSkillTreeData( iSkillID );
		if( NULL == pSkillTreeData )
			continue;

		// 스킬 최초 획득시 필요한 SP 는 스킬 레벨테이블에 있는 필요 SP 1렙짜리에서 얻어온다.
		const TSkillData* pSkillData = pDataManager->GetSkillData( iSkillID );
		int iNeedJobID = pSkillData->nNeedJobID;
		
		map<int, S_SKILLTREE_INFO*>::iterator iter = m_mapSkillTreeByJobID.find( iNeedJobID );
		if( m_mapSkillTreeByJobID.end() == iter )
		{
			S_SKILLTREE_INFO* pNewTree = new S_SKILLTREE_INFO;
			pNewTree->iJobID = iNeedJobID;
			m_mapSkillTreeByJobID.insert( make_pair(iNeedJobID, pNewTree) );
			m_vlpJobSkillTreeList.push_back( pNewTree );

			pNewTree->vlNodeRenderInfo.assign( MAX_SKILL_SLOT_COUNT, S_NODE_RENDER_INFO() );
		}
		
		S_SKILLTREE_INFO* pTree = m_mapSkillTreeByJobID[ iNeedJobID ];

		S_NODE_INFO* pNodeInfo = new S_NODE_INFO;
		pNodeInfo->iSkillID = iSkillID;
		pNodeInfo->iJobID = iNeedJobID;

		// 스킬 레벨 데이터가 해당 스킬 id 로 존재하지 않는 경우 로그 남기고 넘김.
		if( pSkillData->vLevelDataList.empty() )
		{
			g_Log.Log( LogType::_ERROR, L"[CSkillTreeSystem::InitializeTableUsingDataManager] Not Exist SkillLevelTable Data!!    SkillID: %d \r\n", iSkillID );
			SAFE_DELETE( pNodeInfo );
			continue;
		}

		pNodeInfo->iNeedSkillPointToAcquire = pSkillData->vLevelDataList.front().nNeedSkillPoint;
		pNodeInfo->iTreeSlotIndex = pSkillTreeData->nTreeSlotIndex;
		pNodeInfo->bDefaultLocked = pSkillData->bDefaultLocked;

#if defined( PRE_ADD_ONLY_SKILLBOOK )
		pNodeInfo->bNeedSkillBook = pSkillTreeData->bNeedSkillBook;
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )

		// 획득조건 정보
		pNodeInfo->UnLockCondition.iUnlockSkillBookItemID = pSkillData->nUnlockSkillBookItemID;
		pNodeInfo->AcquireCondition.iNeedLevel = pSkillData->vLevelDataList.front().cLevelLimit;

		for( int i = 0; i < MAX_PARENT_SKILL_COUNT; ++i )
		{
			// 정의되지 않은 값은 0 일 수도 있음
			int iNeedLevel = pSkillTreeData->nNeedParentSkillLevel[ i ];

			if( 0 != iNeedLevel )
				pNodeInfo->AcquireCondition.vlParentSkillNeedLevel.push_back( iNeedLevel );
		}

		// 최대 슬롯 크기가 넘어갔음. 프로그램을 종료시키는 것 이외에는 답이 없다!
		_ASSERT( pNodeInfo->iTreeSlotIndex < (int)pTree->vlNodeRenderInfo.size() );

		pTree->vlNodeRenderInfo.at( pNodeInfo->iTreeSlotIndex ) = S_NODE_RENDER_INFO( iSkillID, pNodeInfo->iTreeSlotIndex, iNeedJobID, 
			&pNodeInfo->UnLockCondition, &pNodeInfo->AcquireCondition );

		pNodeInfo->pNodeRenderInfo = &(pTree->vlNodeRenderInfo[pNodeInfo->iTreeSlotIndex]);
		pTree->vlpNodeList.push_back( pNodeInfo );
		pTree->mapNodeBySkillID.insert( make_pair(iSkillID, pNodeInfo) );
		m_mapNodeListBySkillBookID.insert( make_pair(pNodeInfo->UnLockCondition.iUnlockSkillBookItemID, pNodeInfo) );
	}

	// 부모자식 구성 정보 및 트리 구성에 대한 데이터를 구성한다.
	int iNumTreeNode = pDataManager->GetNumSkillTreeData();
	for( int iIndex = 0; iIndex < iNumTreeNode; ++iIndex )
	{
		const TSkillTreeData* pSkillTreeData = pDataManager->GetSkillTreeDataByIndex( iIndex );
		//const TSkillData* pSkillData = pDataManager->GetSkillData( pSkillTreeData->nSkillID );

		//int iJobID = pSkillData->nNeedJobID;

		S_NODE_INFO* pNodeInfo = _FindNode( pSkillTreeData->nSkillID );
		if( NULL == pNodeInfo )
		{
			OutputDebug( "[%d] 스킬 트리 테이블엔 있으나 스킬 테이블에 없는 스킬 ID\n", pSkillTreeData->nSkillID );
			_ASSERT( !"스킬 트리 테이블엔 있으나 스킬 테이블에 없는 스킬 ID" );
		}
		else
		{
#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
			pNodeInfo->pNodeRenderInfo->vlNeedJobSP.push_back(pSkillTreeData->nNeedBasicSP);
			pNodeInfo->pNodeRenderInfo->vlNeedJobSP.push_back(pSkillTreeData->nNeedFirstSP);
			pNodeInfo->pNodeRenderInfo->vlNeedJobSP.push_back(pSkillTreeData->nNeedSecondSP);
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP
		}

		// 부모 스킬 찾아서 자기 자신을 등록시킨다.
		for( int i = 0; i < MAX_PARENT_SKILL_COUNT; ++i )
		{
			int iParentSkillID = pSkillTreeData->nParentSkillID[ i ];
			if( 0 != iParentSkillID )
			{
				S_NODE_INFO* pParentNodeInfo = _FindNode( iParentSkillID );
				if( NULL == pParentNodeInfo )
				{
					OutputDebug( "[%d 의 부모로 설정된 %d] 스킬 트리 테이블엔 있으나 스킬 테이블에 없는 스킬 ID\n", pSkillTreeData->nSkillID, iParentSkillID );
					_ASSERT( !"스킬 트리 테이블엔 있으나 스킬 테이블에 없는 스킬 ID" );
				}

				if( pNodeInfo )
					pNodeInfo->vlpParentNodeInfo.push_back( pParentNodeInfo );
				if( pParentNodeInfo )
					pParentNodeInfo->vlpChildNodeInfo.push_back( pNodeInfo );
			}
		}
	}

	return bResult;
}
#endif

CDnSkillTreeSystem::S_NODE_INFO* CDnSkillTreeSystem::_FindNode( int iSkillID )
{
	S_NODE_INFO* pNodeInfo = NULL;

	vector<S_SKILLTREE_INFO*>::iterator iter = m_vlpJobSkillTreeList.begin();
	for( iter; iter != m_vlpJobSkillTreeList.end(); ++iter )
	{
		map<int, S_NODE_INFO*>& mapNodeBySkillID = (*iter)->mapNodeBySkillID;
		map<int, S_NODE_INFO*>::iterator iterNode = mapNodeBySkillID.find( iSkillID );
		if( mapNodeBySkillID.end() != iterNode )
		{
			pNodeInfo = iterNode->second;
		}
	}

	return pNodeInfo;
}

#ifdef PRE_ADD_ONLY_SKILLBOOK
bool CDnSkillTreeSystem::IsNeedSkillBook( int iSkillID, bool& bExistSkill )
{
	S_NODE_INFO* pNodeInfo = NULL;

	vector<S_SKILLTREE_INFO*>::iterator iter = m_vlpJobSkillTreeList.begin();
	for( iter; iter != m_vlpJobSkillTreeList.end(); ++iter )
	{
		map<int, S_NODE_INFO*>& mapNodeBySkillID = (*iter)->mapNodeBySkillID;
		map<int, S_NODE_INFO*>::iterator iterNode = mapNodeBySkillID.find( iSkillID );
		if( mapNodeBySkillID.end() != iterNode )
		{
			pNodeInfo = iterNode->second;
		}
	}

	if( pNodeInfo )
	{
		bExistSkill = true;
		return pNodeInfo->bNeedSkillBook;
	}

	bExistSkill = false;
	return false;
}
#endif // PRE_ADD_ONLY_SKILLBOOK

void CDnSkillTreeSystem::_MakeupParentSkillLinks( DNTableFileFormat* pSkillTreeTable, DNTableFileFormat* pSkillTable )
{
	// 부모/자식 스킬 노드 정보를 이어준다.
	int iNumTreeNode = pSkillTreeTable->GetItemCount();
	for( int iIndex = 0; iIndex < iNumTreeNode; ++iIndex )
	{
		int iTreeTableID = pSkillTreeTable->GetItemID( iIndex );

		int iSkillID = pSkillTreeTable->GetFieldFromLablePtr( iTreeTableID, "_SkillTableID" )->GetInteger();
		
		if( 0 == iSkillID )
			continue;

		int iJobID = pSkillTable->GetFieldFromLablePtr( iSkillID, "_NeedJob" )->GetInteger();

		S_NODE_INFO* pNodeInfo = _FindNode( iSkillID );
		_ASSERT( pNodeInfo && "스킬 트리 테이블엔 없는 스킬 ID" );
		if( pNodeInfo ) 
		{
			// 부모 스킬 찾아서 자기 자신을 등록시킨다.
			char caLabel[ 32 ];
			for( int i = 0; i < MAX_PARENT_SKILL_COUNT; ++i )
			{
				sprintf_s( caLabel, "_ParentSkillID%d", i + 1 );
				int iParentSkillID = pSkillTreeTable->GetFieldFromLablePtr( iTreeTableID, caLabel )->GetInteger();
				if( 0 != iParentSkillID )
				{
					S_NODE_INFO* pParentNodeInfo = _FindNode( iParentSkillID );
					if (pParentNodeInfo == NULL)
						continue;

					pNodeInfo->vlpParentNodeInfo.push_back( pParentNodeInfo );
					pNodeInfo->pNodeRenderInfo->vlParentSkillJobIDs.push_back( pParentNodeInfo->iJobID );
					pNodeInfo->pNodeRenderInfo->vlParentSkillIDs.push_back( pParentNodeInfo->iSkillID );

					_ASSERT( pNodeInfo->iSkillID == pNodeInfo->pNodeRenderInfo->iSkillID && "스킬 테이블에서 같은 직업군의 스킬 슬롯이 중복되었을 가능성이 있습니다." );
					if( pNodeInfo->iSkillID != pNodeInfo->pNodeRenderInfo->iSkillID )
					{
						OutputDebug( "[SkillID: %d] 스킬 테이블에서 같은 직업군의 스킬 슬롯(%d)이 중복되었을 가능성이 있습니다.\n", 
									  pNodeInfo->iSkillID, pNodeInfo->pNodeRenderInfo->iSlotIndex );
					}

					// 같은 직업 트리 안에서의 자식 슬롯 인덱스.
					if( pNodeInfo->iJobID == pParentNodeInfo->iJobID )
						pParentNodeInfo->pNodeRenderInfo->vlChildSlotIndexInJob.push_back( pNodeInfo->iTreeSlotIndex );

					pParentNodeInfo->vlpChildNodeInfo.push_back( pNodeInfo );
				}
			}

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
			int nLimitSPValue = pSkillTreeTable->GetFieldFromLablePtr( iTreeTableID, "_NeedBasicSP1" )->GetInteger();
			pNodeInfo->pNodeRenderInfo->vlNeedJobSP.push_back(nLimitSPValue);
			nLimitSPValue = pSkillTreeTable->GetFieldFromLablePtr( iTreeTableID, "_NeedFirstSP1" )->GetInteger();
			pNodeInfo->pNodeRenderInfo->vlNeedJobSP.push_back(nLimitSPValue);
			nLimitSPValue = pSkillTreeTable->GetFieldFromLablePtr( iTreeTableID, "_NeedSecondSP1" )->GetInteger();
			pNodeInfo->pNodeRenderInfo->vlNeedJobSP.push_back(nLimitSPValue);
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP
		}
	}
}


void CDnSkillTreeSystem::TryUnLockSkill( const S_TRY_UNLOCK& PresentInfo, S_OUTPUT* pResult )
{
	const S_NODE_INFO* pSkillNodeInfo = _FindNode( PresentInfo.iTryUnlockSkillID );

	// 주어진 스킬 정보에 해당하는 스킬 노드가 존재하는가.
	if( NULL == pSkillNodeInfo )
	{
		pResult->eResult = R_ERROR;
		pResult->eErrorCode = E_CANT_FIND_SKILLNODE;
		return;
	}

	//// 직업이 일치하는가. 히스토리 뒤져서 하위 직업이면 가능하다.
	//if( pSkillNodeInfo->iJobID != PresentInfo.iJobID )
	//{
	//	pResult->eResult = R_ERROR;
	//	pResult->eErrorCode = E_MISMATCH_JOB;
	//}

	//스킬북 더이상 사용하지 않음?
#if defined(PRE_FIX_51727)
#else
	// 스킬북이 맞는지.
	if( PresentInfo.iSkillBookItemID != pSkillNodeInfo->UnLockCondition.iUnlockSkillBookItemID )
	{
		pResult->eResult = R_INVALID_SKILLBOOK_ITEMID;
		return;
	}
#endif // PRE_FIX_51727
	
#if defined(_CLIENT) && defined( PRE_ADD_ONLY_SKILLBOOK )
	if( true == pSkillNodeInfo->bNeedSkillBook )
	{
		pResult->eResult = R_ONLY_SKILL_BOOK;
		return;
	}
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )

	pResult->eResult = R_SUCCESS;
}


void CDnSkillTreeSystem::TryAcquireSkill( const S_TRY_ACQUIRE& TryAcquire, S_OUTPUT* pResult )
{
	// 이미 배운 스킬인지, 직업이 일치하는지, 필요레벨이 충족되는지.
	const S_NODE_INFO* pSkillNodeInfo = _FindNode( TryAcquire.iTryAcquireSkillID );

	// 선행(부모)스킬 획득 상태.

	// 주어진 스킬 정보에 해당하는 스킬 노드가 존재하는가.
	if( NULL == pSkillNodeInfo )
	{
		pResult->eResult = R_ERROR;
		pResult->eErrorCode = E_CANT_FIND_SKILLNODE; 
		return;
	}

	// 선행 스킬이 존재한다면 획득한 상태인가.
	// SkillInfo 구조체에 맞는 함수 객체.
	struct FindSkill : unary_function<const S_POSSESSED_SKILL_INFO, bool>
	{
		int m_iSkillIDToFind;
		FindSkill( int iSkillID ) : m_iSkillIDToFind( iSkillID ) {};

		bool operator () ( const S_POSSESSED_SKILL_INFO& SkillInfo )
		{
			return SkillInfo.iSkillID == m_iSkillIDToFind;
		}
	};

	//// 직업이 일치하는가. 히스토리 뒤져서 하위 직업이면 가능하다.
	//if( pSkillNodeInfo->iJobID != TryAcquire.iJobID )
	//{
	//	pResult->eResult = R_ERROR;
	//	pResult->eErrorCode = E_MISMATCH_JOB;
	//}

	// 필요 레벨이 충족되는지... 근데 언락할 때는 레벨 체크 안하는 건지 물어봐야겠군.
	if( TryAcquire.iCurrentCharLevel < pSkillNodeInfo->AcquireCondition.iNeedLevel )
	{
		pResult->eResult = R_NOT_ENOUGH_CHAR_LEVEL;
		return;
	}

	// 보유한 스킬 포인트가 요구 포인트보다 많은지 체크.
	if( TryAcquire.iHasSkillPoint < pSkillNodeInfo->iNeedSkillPointToAcquire )
	{
		pResult->eResult = R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE;
		return;
	}

	// 부모 스킬이 충족되는지. 부모스킬 레벨까지 충족되어야 함.
	if( false == pSkillNodeInfo->vlpParentNodeInfo.empty() )
	{
		size_t NumParentSkill = pSkillNodeInfo->vlpParentNodeInfo.size();
		for( size_t i = 0; i < NumParentSkill; ++i )
		{
			const S_NODE_INFO* pParentNodeInfo = pSkillNodeInfo->vlpParentNodeInfo.at( i );

			vector<S_POSSESSED_SKILL_INFO>::const_iterator iter = find_if( TryAcquire.vlPossessedSkillInfo.begin(), TryAcquire.vlPossessedSkillInfo.end(), 
																		   FindSkill(pParentNodeInfo->iSkillID) );
			if( TryAcquire.vlPossessedSkillInfo.end() == iter )
			{
				pResult->eResult = R_DONT_HAVE_PARENT_SKILL;
				return;
			}

			// 부모 스킬이 언락 상태이어야 함.
			if( true == iter->bCurrentLock )
			{
				pResult->eResult = R_LOCKED_PARENTSKILL;
				return;
			}

			// 부모 스킬의 레벨을 조건 이상 올려주어야 함.
			int iParentSkillNeedLevel = pSkillNodeInfo->AcquireCondition.vlParentSkillNeedLevel.at( i );
			if( iter->iSkillLevel < iParentSkillNeedLevel )
			{
				pResult->eResult = R_NOT_ENOUGH_PARENT_SKILL_LEVEL;
				return;
			}
		}
	}

	pResult->eResult = R_SUCCESS;
}


int CDnSkillTreeSystem::GetNodeCount( int iJobID ) const
{
	int iResult = 0;

	map<int, S_SKILLTREE_INFO*>::const_iterator iter = m_mapSkillTreeByJobID.find( iJobID );
	if( m_mapSkillTreeByJobID.end() != iter )
		iResult = (int)iter->second->vlpNodeList.size();

	return iResult;
}

int CDnSkillTreeSystem::GetSlotCount( int iJobID ) const
{
	int iResult = 0;

	map<int, S_SKILLTREE_INFO*>::const_iterator iter = m_mapSkillTreeByJobID.find( iJobID );
	if( m_mapSkillTreeByJobID.end() != iter )
	{
		// 왼쪽에서 오른쪽으로 슬롯 인덱스가 늘어나므로 가장 끝에 있는 슬롯인덱스가 가장 크다.
		//iResult = (int)iter->second->vlNodeRenderInfo.back().iSlotIndex + 1;
		iResult = MAX_SKILL_SLOT_COUNT;
	}

	return iResult;
}

void CDnSkillTreeSystem::GetNodeRenderInfo( int iSlotIndex, int iJobID, /*OUT*/S_NODE_RENDER_INFO* pNodeRenderInfo ) const
{
	_ASSERT( pNodeRenderInfo );
	if( NULL == pNodeRenderInfo )
		return;

	map<int, S_SKILLTREE_INFO*>::const_iterator iter = m_mapSkillTreeByJobID.find( iJobID );
	if( m_mapSkillTreeByJobID.end() != iter )
	{
		const S_SKILLTREE_INFO* pSkillTree = iter->second;

		if( NULL == pSkillTree )
			return;

		_ASSERT( iSlotIndex < (int)pSkillTree->vlNodeRenderInfo.size() );
		if( iSlotIndex < (int)pSkillTree->vlNodeRenderInfo.size() )
			*pNodeRenderInfo = pSkillTree->vlNodeRenderInfo.at( iSlotIndex );
	}
}

int CDnSkillTreeSystem::FindSkillBySkillBook( int iSkillBookItemID )
{
	int iResult = 0;

	map<int, S_NODE_INFO*>::iterator iter = m_mapNodeListBySkillBookID.find( iSkillBookItemID );
	if( m_mapNodeListBySkillBookID.end() != iter )
	{
		iResult = iter->second->iSkillID;
	}

	return iResult;
}

#ifdef PRE_ADD_LEVELUP_GUIDE
void CDnSkillTreeSystem::GetLevelUpSkillInfo(std::vector<int>& newSkillIdList, int newLevel, int jobId)
{
	std::map<int, S_SKILLTREE_INFO*>::const_iterator iter = m_mapSkillTreeByJobID.find(jobId);
	if (m_mapSkillTreeByJobID.end() != iter)
	{
		const S_SKILLTREE_INFO* pInfo = (*iter).second;
		if (pInfo)
		{
			const std::vector<S_NODE_INFO*>& nodeInfoList = pInfo->vlpNodeList;
			std::vector<S_NODE_INFO*>::const_iterator nodeInfoIter = nodeInfoList.begin();
			for (; nodeInfoIter != nodeInfoList.end(); ++nodeInfoIter)
			{
				const S_NODE_INFO* pNodeInfo = (*nodeInfoIter);
				if (pNodeInfo && (pNodeInfo->AcquireCondition.iNeedLevel == newLevel))
					newSkillIdList.push_back(pNodeInfo->iSkillID);
			}
		}
	}
}

void CDnSkillTreeSystem::GetLevelUpSkillInfo(std::vector<int>& newSkillIdList, int newLevel, const std::vector<int>& jobHistoryVec)
{
	std::vector<int>::const_iterator& iter = jobHistoryVec.begin();
	for (; iter != jobHistoryVec.end(); ++iter)
	{
		const int& jobId = *iter;
		if (CommonUtil::IsValidCharacterJobId(jobId) == false)
			continue;

		GetLevelUpSkillInfo(newSkillIdList, newLevel, jobId);
	}
}
#endif

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
void CDnSkillTreeSystem::GetNeedSPValuesByJob(int nSkillID, std::vector<int>& nNeedSPValues)
{
	CDnSkillTreeSystem::S_NODE_INFO* pInfo = _FindNode(nSkillID);
	if (pInfo && pInfo->pNodeRenderInfo)
		nNeedSPValues = pInfo->pNodeRenderInfo->vlNeedJobSP;
}
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

int CDnSkillTreeSystem::IsParentSkill( const int nParentSkillID, const int nChildSkillID )
{
	S_NODE_INFO * pData = _FindNode( nChildSkillID );

	if( NULL == pData )
		return 0;

	for( DWORD itr = 0; itr < pData->vlpParentNodeInfo.size(); ++itr )
	{
		if( nParentSkillID == pData->vlpParentNodeInfo[itr]->iSkillID )
			return pData->AcquireCondition.vlParentSkillNeedLevel[itr];
	}

	return 0;
}

void CDnSkillTreeSystem::GetParentSkillData( const int nChildSkillID, std::vector< std::pair<int, int> > & vecParentSkillData )
{
	S_NODE_INFO * pData = _FindNode( nChildSkillID );

	if( NULL == pData )
		return;

	for( DWORD itr = 0; itr < pData->vlpParentNodeInfo.size(); ++itr )
		vecParentSkillData.push_back( std::make_pair( pData->vlpParentNodeInfo[itr]->iSkillID, pData->AcquireCondition.vlParentSkillNeedLevel[itr] ) );
}

#if defined( PRE_ADD_PRESET_SKILLTREE )
void CDnSkillTreeSystem::GetAdviceSkillTree( const int nAdviceSkill_Index, const std::vector<int> & vecJobList, std::vector< std::pair<int, BYTE> > & vecData )
{
	if( E_ADVICE_SKILL_LEVEL_COUNT <= nAdviceSkill_Index )
		return;

	vecData.clear();
	for( DWORD itr = 0; itr < vecJobList.size(); ++itr )
	{
		if( NULL == m_mapSkillTreeByJobID[ vecJobList[itr] ] )
			continue;

		std::vector<S_NODE_INFO*> & vecNodeList = m_mapSkillTreeByJobID[ vecJobList[itr] ]->vlpNodeList;
        std::vector<S_NODE_INFO*>::iterator Itor = vecNodeList.begin();
		for( ; Itor != vecNodeList.end(); ++Itor )
		{
			BYTE cAdviceSkillLevel = (*Itor)->vecAdviceSkillLevel[ nAdviceSkill_Index ];
			if( 0 != cAdviceSkillLevel )
				vecData.push_back( std::make_pair( (*Itor)->iSkillID, cAdviceSkillLevel ) );
		}
	}
}
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )