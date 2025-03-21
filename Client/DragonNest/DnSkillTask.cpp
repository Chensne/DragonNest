#pragma once
#include "StdAfx.h"
#include "DnSkillTask.h"
#include "DnActor.h"
#include "DnSkill.h"
#include "DnInterface.h"
#include "ItemSendPacket.h"
#include "SkillSendPacket.h"
#include "DnMainDlg.h"
#include "DnStateBlow.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DnPartyTask.h"
#include "DnSkillTabDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnSkillTreeContentDlg.h"
#include "DnTableDB.h"
#include "DnIncodeResource.h"
#include "DnItemTask.h"
#include "DnSkillResetDlg.h"
#include "DnSkillTreeDlg.h"
#include "SecondarySkill.h"
#include "DnLifeSkillFishingTask.h"
#include "DnLifeSkillCookingTask.h"
#include "ManufactureSkill.h"
#include "DnTradeTask.h"
#include "DnTradeMail.h"
#include "DnTooltipDlg.h"
#include "DnGuildWarTask.h"
#include "DnGuildWarSkillSlotDlg.h"

//#undef PRE_ADD_SKILL_LEVELUP_RESERVATION

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#include "TotalLevelSkillSystem.h"
#endif // PRE_ADD_TOTAL_LEVEL_SKILL


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSkillTask::CDnSkillTask() : CTaskListener(true), m_iSkillResetStartJobDegree( 0 ), m_iSkillResetEndJobDegree( 0 ), m_bAddSkillCheatEnabled( false ), m_bRequestWait( false )
{

	m_iSkillTreePage = DualSkill::Type::Primary;
	m_iSkillPageCount = DualSkill::Type::None;
	m_iNeedRefreshSkillPage = DualSkill::Type::None;

	for(int i=0 ;i<DualSkill::Type::MAX; i++)
		m_iSkillPoint[i] = 0;
	m_bTempSkillAdded = false;

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	Clear_SkillLevelUp();
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
}

CDnSkillTask::~CDnSkillTask()
{

	// 직업을 바꾸거나 하는 경우 밑의 내 직업의 스킬 트리에도 존재하지 않는
	// 스킬 객체가 되어서 아래의 스킬 트리에 있는 스킬 객체 release 에서도 제거되지 않는다.
	// 따라서 여기서 먼저 ZeroLevelSkill 들을 해제시켜준다. 
	// 상당수는 아래의 트리 노드에 있는 핸들과 중복된다.
	// m_vlhUnlockZeroLevelSkills[m_iSkillTreePage] 는 서버로부터 받은 정보를 기준으로 스킬객체를 생성해서 갖고 있고,
	// 유저가 새롭게 획득한다면 m_vlhUnlockZeroLevelSkills[m_iSkillTreePage] 에서 핸들이 빠지고 그 핸들이 
	// m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage] 로 추가되는 구조이다.

	for(int i=0; i<DualSkill::Type::MAX ; i++)
	{
		SAFE_RELEASE_SPTRVEC( m_vlhLocalPlayerOpenedSkillList[i] );
		SAFE_RELEASE_SPTRVEC( m_vlhUnlockZeroLevelSkills[i] );
	}

	// 바로 위의 구문.. 로컬 플레이어 오픈 스킬 리스트에서 삭제되었을 수도 있고, 
	// 원래 비어있는 슬롯일 수도 있으므로 valid 체크를 하고 제거.
	map<int, S_SKILLTREE_INFO>::iterator iterSkillTreeMap = m_mapSkillTreeInfoByJobID.begin();
	for( iterSkillTreeMap; iterSkillTreeMap != m_mapSkillTreeInfoByJobID.end(); ++iterSkillTreeMap )
	{
		S_SKILLTREE_INFO& SkillTreeInfo = iterSkillTreeMap->second;
		for( int iSlot = 0; iSlot < MAX_SKILL_SLOT_COUNT; ++iSlot )
		{
			if( SkillTreeInfo.aSkillTreeSlotInfo[ iSlot ].hSkill )
				SAFE_RELEASE_SPTR( SkillTreeInfo.aSkillTreeSlotInfo[ iSlot ].hSkill );
		}
	}

	list<S_SKILL_LEVELUP_INFO*>::iterator iterLevelUpInfo = m_listWaitSkillLevelup.begin();
	for( iterLevelUpInfo; iterLevelUpInfo != m_listWaitSkillLevelup.end(); ++iterLevelUpInfo )
		delete (*iterLevelUpInfo);

	SAFE_RELEASE_SPTRVEC( m_vlhGuildWarSkillList );
}

bool CDnSkillTask::InitializeSkillMovieTable()
{
	m_SkillMovieDataList.clear();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSKILLVIDEO );
	if( !pSox ) return false;

	int iItemID = 0;
	for (; iItemID < pSox->GetItemCount(); ++iItemID)
	{
		int nTableID		= pSox->GetItemID(iItemID);
		int skillId			= pSox->GetFieldFromLablePtr(nTableID, "_SkillID")->GetInteger();
		char* szSkillVideo	= pSox->GetFieldFromLablePtr(nTableID, "_SkillVideoFile")->GetString();

		m_SkillMovieDataList.insert(std::make_pair(skillId, szSkillVideo));
	}

	return true;
}

const char* CDnSkillTask::GetSkillMovieName(int skillId) const
{
	std::map<int, std::string>::const_iterator iter = m_SkillMovieDataList.find(skillId);
	if (iter != m_SkillMovieDataList.end())
	{
		const std::string& str = (*iter).second;
		return str.c_str();
	}

	return NULL;
}

bool CDnSkillTask::Initialize()
{
	m_bTempSkillAdded = false;
	// 스킬 트리 시스템 초기화.
	m_SkillTreeSystem.InitializeTable();
	InitializeSkillMovieTable();

	for(int i=0 ;i<DualSkill::Type::MAX; i++)
		m_iSkillPoint[i] = 0;

#if defined(PRE_FIX_NEXTSKILLINFO)
	InitSkillLevelTableIDList();
#endif // PRE_FIX_NEXTSKILLINFO

	return true;
}

void CDnSkillTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	bool bProcessDispatch = false;

	switch( nMainCmd )
	{
		case SC_SKILL:	_OnRecvSkillMessage( nSubCmd, pData, nSize ); bProcessDispatch = true;	break;
		case SC_SECONDARYSKILL:	_OnRecvLifeSkillMessage( nSubCmd, pData, nSize ); bProcessDispatch = true;	break;
		case SC_PVP:	_OnRecvPVPSkillMessage( nSubCmd, pData, nSize ); bProcessDispatch = true;	break;
	}

	if( bProcessDispatch ) 
	{
		WriteLog( 1, ", Info, CDnSkillTask::OnDispatchMessage End : nMainCmd(%d), nSubCmd(%d), nSize(%d)\n", nMainCmd, nSubCmd, nSize );
	}
}

void CDnSkillTask::_OnRecvSkillMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd )
	{
		case eSkill::SC_SKILLLIST:		OnRecvSkillList((SCSkillList*)pData);		break;
		case eSkill::SC_ADDSKILL:		OnRecvSkillAdd((SCAddSkill*)pData);			break;
		case eSkill::SC_DELSKILL:		OnRecvSkillDel((SCDelSkill*)pData);			break;
		case eSkill::SC_SKILLLEVELUP:	OnRecvSkillLevelUp((SCSkillLevelUp*)pData); break;
		case eSkill::SC_OTHERPLAYERSKILLLEVELUP: OnRecvOtherPlayerSkillLevelUp((SCOtherPlayerSkillLevelUp*)pData); break;
		case eSkill::SC_PUSHSKILLPOINT: OnRecvPushSkillPoint( (SCPushSkillPoint*)pData ); break;
		case eSkill::SC_UNLOCKSKILL:	OnRecvUnlockSkill( (SCUnlockSkill*)pData ); break;
		case eSkill::SC_ACQUIRESKILL:	OnRecvAcquireSkill( (SCAcquireSkill*)pData ); break;
		case eSkill::SC_SKILL_RESET:	OnRecvSkillReset( (SCSkillReset*)pData ); break;
		case eSkill::SC_CANUSE_SKILLRESET_CASH_ITEM:	OnRecvCanUseSkillResetCashItem( (SCCanUseSkillResetCashItem*)pData ); break;
		case eSkill::SC_USE_SKILLRESET_CASHITEM_RES: OnRecvUseSkillResetCashItemRes( (SCUseSkillResetCashItemRes*)pData ); break;
		case eSkill::SC_CHANGE_SKILLPAGE_RES: OnRecvChangeSkillPageRes( (SCChangeSkillPage*)pData); break;
		case eSkill::SC_SKILLPAGE_COUNT: OnRecvSkillPageCount( (SCSkillPageCount*)pData ); break;
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		case eSkill::SC_RESERVATION_SKILL_LIST_ACK: OnRecvReservationSkillList( (SCReservationSkillListAck *)pData ); break;
#endif	// #if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
#ifdef PRE_ADD_PRESET_SKILLTREE
		case eSkill::SC_LIST_SKILLSET: OnRecvSkillSetList((SCSKillSetList*)pData); break;
		case eSkill::SC_SAVE_SKILLSET: OnRecvSkillSetSaveResult((SCSKillSaveResult*)pData); break;
		case eSkill::SC_DELETE_SKILLSET: OnRecvSkillSetDeleteResult((SCSKillDeleteResult*)pData); break;
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE
	}
}

void CDnSkillTask::_OnRecvLifeSkillMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd )
	{
		case eSecondarySkill::SC_CREATE:			OnRecvLifeSkillAdd( (SecondarySkill::SCCreate*)pData );				break;
		case eSecondarySkill::SC_DELETE:			OnRecvLifeSkillDelete( (SecondarySkill::SCDelete*)pData );			break;
		case eSecondarySkill::SC_UPDATE_EXP:		OnRecvLifeSkillUpdateExp( (SecondarySkill::SCUpdateExp*)pData );	break;
		case eSecondarySkill::SC_LIST:				OnRecvLifeSkillList( (SecondarySkill::SCList*)pData );				break;
	}
}

void CDnSkillTask::OnRecvSkillList( SCSkillList *pPacket )
{
	// 스킬 리스트 또 오면 새로 채우지 않는다.
	// 장비아이템 스킬은 장비리스트 받고 미리 채워지기 때문에 실제 서버가 보내주는 스킬리스트에 포함되어있지 않음.
	bool bAllowFillSkillList = true;

	if( pPacket->cSkillPage >= DualSkill::Type::MAX || pPacket->cSkillPage < DualSkill::Type::Primary )
		return;

	if( m_vlhLocalPlayerOpenedSkillList[pPacket->cSkillPage].empty() )
	{
		int iNumSkill = (int)m_vlhLocalPlayerOpenedSkillList[pPacket->cSkillPage].size();
		for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
		{
			DnSkillHandle hSkill = m_vlhLocalPlayerOpenedSkillList[pPacket->cSkillPage].at( iSkill );
			if( false == hSkill->IsEquipItemSkill() )
				bAllowFillSkillList = false;
		}
	}

	if( bAllowFillSkillList )
	{
		DnSkillHandle hSkill;
		for( int i=0; i<pPacket->cSkillCount; i++ ) 
		{
			// TODO: 서버로부터 Lock/Unlock 스킬인지 여부도 확인..
			// Unlock만 된 스킬은 open 스킬 리스트에 추가하지 않고 따로 보관합니다. 서버에서 날아오는 것은 레벨 0으로 되어있음.
			
			int nSkillID = pPacket->Skill[i].nSkillID;
			int nSkillLevel = pPacket->Skill[i].cSkillLevel;
			int nLevelUpValue = 0;
			if( CDnActor::s_hLocalActor )
				nLevelUpValue = CDnActor::s_hLocalActor->GetSkillLevelUpValue(nSkillID);

			// #41639 nSkillLevel 이 0 이면 초기화된 스킬. 미획득 상태.
			if( nSkillLevel < 1 )
				nLevelUpValue = 0;

			nSkillLevel += nLevelUpValue;

			hSkill = CDnSkill::CreateSkill( CDnActor::Identity(), nSkillID, nSkillLevel );

			if (!hSkill && nLevelUpValue != 0)
			{
				OutputDebug("스킬 (%d) 레벨 (%d) 생성 실패.. 스킬 레벨 테이블 확인 요망!!!!!!\n", nSkillID, nSkillLevel);
				_ASSERT( hSkill && "스킬 레벨업 시도 실패.. 레벨 테이블 확인" );

				//레벨값 다시 원래로 내리고
				nSkillLevel -= nLevelUpValue;
				hSkill = CDnSkill::CreateSkill(CDnActor::Identity(), nSkillID, nSkillLevel);

				//레벨업값 초기화
				nLevelUpValue = 0;
			}

			if ( hSkill )
			{
				// 획득상태인지 미획득상태인지
				if( hSkill->IsAcquired() )
				{
					hSkill->SetElapsedDelayTime( pPacket->Skill[i].nCoolTime / 1000.f );
					m_vlhLocalPlayerOpenedSkillList[pPacket->cSkillPage].push_back( hSkill );
				}
				else
					m_vlhUnlockZeroLevelSkills[pPacket->cSkillPage].push_back( hSkill );

				if( hSkill->IsAcquired() )
					hSkill->SetLevelUpValue(nLevelUpValue);
			}
		}

		// 스킬 트리에 들어있는 모든 스킬들을 읽어들인다.
		// 일단 잡 id 는 1로.. 워리어로 체크한다.
		if( CDnActor::s_hLocalActor )
		{
			// TODO: 추후에 직업 히스토리 별로 루프를 돌도록 한다.
			InitializeSkillTree();
		}

		InitializeLocalPlayerSkills();
		GetInterface().RefreshSkillDialog();

		DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
		if( !hLocalActor ) return;
		CDnLocalPlayerActor *pActor = (CDnLocalPlayerActor *)hLocalActor.GetPointer();

		for( int itr = GLYPH_SKILL1; itr <= GLYPH_SKILL4; ++itr )
		{
			CDnItem *pItem = CDnItemTask::GetInstance().GetGlyphItem(itr);
			if( !pItem ) continue;
			if( dynamic_cast<CDnGlyph*>(pItem) && pItem->GetItemType() == ITEMTYPE_GLYPH ) {
				DnGlyphHandle hGlyph = ((CDnGlyph*)pItem)->GetMySmartPtr();

				if( pPacket->cSkillPage > DualSkill::Type::Primary )
					pActor->ApplyGlyphToExpendedSkillPage( hGlyph , pPacket->cSkillPage );
				else
					pActor->AttachGlyph( hGlyph, (CDnGlyph::GlyphSlotEnum)itr );
			}
		}
	}
}

void CDnSkillTask::AddJobSkillTree( int iJobID )
{
	int iNumSkillTreeSlot = m_SkillTreeSystem.GetSlotCount( iJobID );
	_ASSERT( iNumSkillTreeSlot <= MAX_SKILL_SLOT_COUNT );
	for( int iSlot = 0; iSlot < iNumSkillTreeSlot; ++iSlot )
	{
		CDnSkillTreeSystem::S_NODE_RENDER_INFO NodeRenderInfo;
		m_SkillTreeSystem.GetNodeRenderInfo( iSlot, iJobID, &NodeRenderInfo );

		m_mapSkillTreeInfoByJobID[ iJobID ].aSkillTreeSlotInfo[ iSlot ].NodeRenderInfo = NodeRenderInfo;
		S_SKILLTREE_SLOT_INFO& TreeSlotInfo = m_mapSkillTreeInfoByJobID[ iJobID ].aSkillTreeSlotInfo[ iSlot ];

		if( 0 != NodeRenderInfo.iSkillID )
		{
			// 갖고 있는 스킬이라면 해당 스킬 객체로 셋팅.
			DnSkillHandle hOpenedSkill = FindSkill( NodeRenderInfo.iSkillID );
			if( hOpenedSkill )
			{
				TreeSlotInfo.hSkill = hOpenedSkill;
			}
			else
			{
				// 언락만 시켜놓은 미획득 스킬을 셋팅.
				DnSkillHandle hZeroLevelSkill = FindZeroLevelSkill( NodeRenderInfo.iSkillID );
				if( hZeroLevelSkill )
				{
					TreeSlotInfo.hSkill = hZeroLevelSkill;
				}
				else
				{
					// 레벨 1로 생성하고 락 상태로 해둠. 추후에 스킬북으로 스킬을 열고 SP를 투자하여 레벨업을 해야 획득으로 처리함.
					// 스킬 슬롯에 이미 존재하고 있는 스킬이 있다면 삭제 후 새로 생성해서 넣는다. 
					// 여기서 생성하는 건 툴팁을 보여주기 위해 생성하는 임시객체임. 유저가 얻은 적이 없는 스킬이기 때문에 사용되지 않는다.
					// 최종적으로 CDnSkillTask 객체가 파괴될 때 배열에 있는 유효한 스킬핸들은 전부 초기화된다.
					if( TreeSlotInfo.hSkill )
						SAFE_RELEASE_SPTR( TreeSlotInfo.hSkill );

					TreeSlotInfo.hSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, NodeRenderInfo.iSkillID, 1 );
					TreeSlotInfo.hSkill->Lock();
				}
			}

			TreeSlotInfo.hSkill->SetNodeRenderInfo( TreeSlotInfo.NodeRenderInfo );

			if( CDnActor::s_hLocalActor )
			{
				TreeSlotInfo.hSkill->SetHasActor( CDnActor::s_hLocalActor );
				TreeSlotInfo.hSkill->RefreshDecreaseMP();		// SetHasActor 내부에서 해줌.
			}
		}
	}
}

void CDnSkillTask::RemoveJobSkillTree( int iJobID )
{
	map<int, S_SKILLTREE_INFO>::iterator iter = m_mapSkillTreeInfoByJobID.find( iJobID );
	if( m_mapSkillTreeInfoByJobID.end() != iter )
	{
		// 아예 획득한 적이 없는 스킬은 AddJobSkillTree() 함수에서 생성되어 셋팅해준 스킬 객체이기 때문에 여기서 삭제한다.
		// 나머지 스킬 객체들은 모두 관리되는 곳이 있으므로 직접 생성해준 스킬 객체만 날리고 트리구조체 자체를 맵에서 삭제하면 끝.
		S_SKILLTREE_INFO& SkillTreeInfo = iter->second;
		for( int i = 0; i < MAX_SKILL_SLOT_COUNT; ++i )
		{
			S_SKILLTREE_SLOT_INFO& SlotInfo = SkillTreeInfo.aSkillTreeSlotInfo[ i ];
			if( SlotInfo.hSkill && SlotInfo.hSkill->IsLocked() )
			{
				SAFE_RELEASE_SPTR( SlotInfo.hSkill );
			}
		}

		m_mapSkillTreeInfoByJobID.erase( iter );
	}
}

void CDnSkillTask::InitializeSkillTree()
{
	// 직업 히스토리 별로 루프를 돌도록 한다.
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	vector<int> vlJobHistory;
	pPlayerActor->GetJobHistory( vlJobHistory );

	for( int i = 0; i < (int)vlJobHistory.size(); ++i )
	{
		int iJobID = vlJobHistory.at( i );

		AddJobSkillTree(iJobID);
	}

	pPlayerActor->InitializedSkillTree();
}

void CDnSkillTask::RefreshSkillTree()
{
	m_mapSkillTreeInfoByJobID.clear();
	InitializeSkillTree();
}

void CDnSkillTask::GetSkillTreeSlotInfo( int iJobID, int iSkillSlotIndex, S_SKILLTREE_SLOT_INFO* pSkillSlotInfo )
{
	// TODO: jobid 는 나중에 전직 들어가면 처리.
	_ASSERT( pSkillSlotInfo );
	_ASSERT( iSkillSlotIndex < MAX_SKILL_SLOT_COUNT );
	if( pSkillSlotInfo && iSkillSlotIndex < MAX_SKILL_SLOT_COUNT )
		*pSkillSlotInfo = m_mapSkillTreeInfoByJobID[ iJobID ].aSkillTreeSlotInfo[ iSkillSlotIndex ];
}

// 이제 이 함수는 치트키로 addskill 사용했을 때만 패킷이 와서 호출된다.
void CDnSkillTask::OnRecvSkillAdd( SCAddSkill *pPacket )
{
	// Note : 리턴값 체크
	//		pPacket->cRet
	//
	if (pPacket->nRet == ERROR_NONE)
	{
		// 해당 스킬을 찾아서 상태를 언락으로 변경시킨다.
		DnSkillHandle hCheatAcquiredSkill = FindLockedSkill( pPacket->nSkillID );
		if( !hCheatAcquiredSkill )
			return;

		if( hCheatAcquiredSkill )
		{
			SCUnlockSkill UnlockSkill;
			UnlockSkill.nSkillID = pPacket->nSkillID;
			UnlockSkill.nRetCode = ERROR_NONE;
			this->OnRecvUnlockSkill( &UnlockSkill );
		}

		// 스킬 획득! 보유 스킬리스트에 넣어줌.
		vector<DnSkillHandle>::iterator iter = find( m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].begin(), m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].end(), hCheatAcquiredSkill );
		_ASSERT( m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].end() != iter );
		if( m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].end() != iter )
			m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].erase( iter );

		m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].push_back( hCheatAcquiredSkill );

		
		hCheatAcquiredSkill->SetHasActor( CDnActor::s_hLocalActor );
		hCheatAcquiredSkill->Acquire();

		CDnActor::s_hLocalActor->AddSkill( hCheatAcquiredSkill );

		GetInterface().RefreshSkillDialog();
		m_bAddSkillCheatEnabled = true;

		if( hCheatAcquiredSkill->GetLevel() != 1 )
			return;

		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
		if( !pSox ) return;

		for( int itr = GLYPH_SKILL1; itr <= GLYPH_SKILL4; ++itr )
		{
			CDnItem *pItem = CDnItemTask::GetInstance().GetGlyphItem(itr);
			if( !pItem ) continue;
			if( dynamic_cast<CDnGlyph*>(pItem) && pItem->GetItemType() == ITEMTYPE_GLYPH ) {
				DnGlyphHandle hGlyph = ((CDnGlyph*)pItem)->GetMySmartPtr();

				int nSkillID = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_SkillID" )->GetInteger();

				if( hCheatAcquiredSkill->GetClassID() == nSkillID )
					hCheatAcquiredSkill->AddGlyphStateEffect( hGlyph->GetClassID() );
			}
		}
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRet);
}

void CDnSkillTask::OnRecvSkillDel( SCDelSkill *pPacket )
{
	// Note : 리턴값 체크
	//		pPacket->cRet
	//
	if (pPacket->nRet == ERROR_NONE)
		DelSkill( pPacket->nSkillID );
	else
		GetInterface().ServerMessageBox(pPacket->nRet);
}

void CDnSkillTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_listWaitSkillLevelup.empty() )
	{
		list<S_SKILL_LEVELUP_INFO*>::iterator iter = m_listWaitSkillLevelup.begin();
		for( iter; iter != m_listWaitSkillLevelup.end(); )
		{
			S_SKILL_LEVELUP_INFO* pInfo = *iter;
			if( pInfo->hActor )
			{
				if( pInfo->hActor->GetProcessSkill() != pInfo->hSkill )
				{
					OnRecvSkillLevelUp( &(pInfo->Packet) );
					delete (*iter);
					iter = m_listWaitSkillLevelup.erase( iter );
					continue;
				}
			}
			else
			{
				// 사라진 액터 객체는 리스트에서 제거.
				delete (*iter);
				iter = m_listWaitSkillLevelup.erase( iter );
				continue;
			}

			++iter;
		}
	}
	if( GetGuildWarTask().GetGuildWarState() == PvPCommon::OccupationSystemState::Play )
	{
		vector<DnSkillHandle>::iterator iter = m_vlhGuildWarSkillList.begin();

		for( ; iter != m_vlhGuildWarSkillList.end(); iter++ )
		{
			DnSkillHandle hSkill = (*iter);
			if( hSkill && hSkill->GetActor() == CDnActor::s_hLocalActor )
				hSkill->Process( LocalTime, fDelta );
		}
	}
}

void CDnSkillTask::OnRecvSkillLevelUp( SCSkillLevelUp *pPacket )
{
	_ASSERT( pPacket );

	if( CDnActor::s_hLocalActor == NULL )
		return;

	m_bRequestWait = false;
	DnSkillHandle hSkill = CDnActor::s_hLocalActor->FindSkill( pPacket->nSkillID );

	// 게임 중일 땐 게임서버에서 알아서 패킷이 스킬 끝난 후에 오기 때문에 상관 없지만,
	// 마을에서 휠윈드 같은 중간 액션 있는 거 쓰면서 레벨업하면 이 루틴으로 처리된다.
	// 레벨업은 빌리지 서버에서 곧바로 되고 클라에서는 연출만 늦게 보여준다.
	// 게임 나갔을 때 다른 유저 스킬 레벨업은 겜 서버에서 먼저 해당 유저의 패킷을 받았을 때 부터 
	// 큐잉되었다가 나중에 쏴주기 때문에 자동으로 처리 됨.
	if( CDnActor::s_hLocalActor->GetProcessSkill() == hSkill )
	{
		S_SKILL_LEVELUP_INFO* pSkillLevelupInfo = new S_SKILL_LEVELUP_INFO;
		pSkillLevelupInfo->hActor = CDnActor::s_hLocalActor;
		pSkillLevelupInfo->hSkill = hSkill;
		pSkillLevelupInfo->Packet = *pPacket;

		m_listWaitSkillLevelup.push_back( pSkillLevelupInfo );
		return;
	}

	//CDnSkillTabDlg *pSkillDlg = (CDnSkillTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG );
	//CDnSkillTreeContentDlg* pSkillTreeContentDlg = (CDnSkillTreeContentDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_TREE_CONTENT_DIALOG );

	if (pPacket->nRetCode == ERROR_NONE)
	{
		LevelUpSkill( pPacket->nSkillID, pPacket->cLevel );

		/*
		if( pSkillTreeContentDlg )
			pSkillTreeContentDlg->OnSkillLevelUp( true );
		*/
	}
	else
	{
		//GetInterface().ServerMessageBox(pPacket->nRetCode);
		// Note: 현재 스킬 레벨을 1씩 올리게 되어있음.

		// 서버에서 스킬레벨업 성공했을 때만 스킬 레벨업 포인트 계산하도록 수정. (#22216)
		//if( hSkill )
		//	hSkill->ResetLevelUpUsePoint( 1 );

		/*
		if( pSkillTreeContentDlg )
			pSkillTreeContentDlg->OnSkillLevelUp( false );
		*/
	}
}

int CDnSkillTask::QueryPVEOrPVP( void )
{
	int iSkillLevelDataType = CDnSkill::PVE;
	CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	bool bIsPVPGame = ((pGameTask) && (pGameTask->GetGameTaskType() == GameTaskType::PvP));
	if( bIsPVPGame )
	{
		iSkillLevelDataType = CDnSkill::PVP;
	}

	return iSkillLevelDataType;
}

void CDnSkillTask::InitializeLocalPlayerSkills()
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor ) return;

	// 스킬 레벨 데이터 디폴트는 무조건 PVE 
	int iSkillLevelDataType = QueryPVEOrPVP();

	DnSkillHandle hSkill;

	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) {
		hSkill = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i];
		// SetHasActor 함수 내에서 업데이트 되는 정보들이 있으므로 반드시 미리 스킬레벨데이터 타입을 업데이트 해준다.
		hSkill->SelectLevelDataType( iSkillLevelDataType , true );
		hSkill->SetHasActor( hLocalActor );

		if( hSkill->IsToggleOn() )
			hSkill->EnableToggle( false );

		if( hSkill->IsAuraOn() )
			hSkill->EnableAura( false );

		if( hLocalActor->CanAddSkill( hSkill->GetClassID() ) == false )
			continue;
		hLocalActor->AddSkill( hSkill );

#if defined(PRE_FIX_CLIENT_MEMOPTIMIZE)
		if( hSkill->IsUseInVillage() ) {
			const set<string> &setActionList = hSkill->GetUseActionSet();
			for( set<string>::const_iterator iter = setActionList.begin(); iter != setActionList.end(); iter++ ) {
				hLocalActor->RefreshUsingElement( (*iter).c_str() );
			}
		}
#endif
	}

	// 전부 돌면서 강화 패시브 스킬이 있다면 관련 정보를 업데이트 해준다.
	InitializeEnchantPassiveSkills();

#if defined(PRE_FIX_51999)
	bool bAddTempSkill = GetSkillTask().IsTempSkillAdded();
#endif // PRE_FIX_51999

	// 겜서버에서 겜서버 이동하는 것은 서버에서 패킷을 다 쏴주지만 마을에 오는 경우엔 
	// 게임서버와 커넥션이 끊기므로 직접 임시로 생성한 스킬은 초기화한다.
	// 마을로 들어오면 캐릭터 정보는 다시 새로 받으므로 임시 직업 넣었던 것은 초기화 된다.
	// 여기서 임시 스킬만 날려주면 된다.
	GetSkillTask().RemoveAllTempSkill();

#if defined(PRE_FIX_51999)
	//전직 퀘스트 완료후 정상적인 처리를 하지 않고, 바로 마을로 이동해 왔을 경우
	//SkillTree정보가 비정상적인 경우가 발생됨.
	//tempSkill이 추가가 되어 있다면 skillTree를 갱신해준다.
	if (bAddTempSkill == true)
		GetSkillTask().InitializeSkillTree();
#endif // PRE_FIX_51999

	// 패시브 버프 스킬들을 적용시키기 위해 한 번 루프를 돌려줘야 함.
	hLocalActor->GetStateBlow()->Process( 0, 0.0f, true );

	if( m_iNeedRefreshSkillPage != DualSkill::Type::None )
	{
		ApplySkillPage(m_iNeedRefreshSkillPage);
		m_iNeedRefreshSkillPage = DualSkill::Type::None;
		// 패시브 버프 스킬들을 적용시키기 위해 한 번 루프를 돌려줘야 함.
		hLocalActor->GetStateBlow()->Process( 0, 0.0f, true );
	}

	hLocalActor->SetHP( hLocalActor->GetMaxHP() );
	hLocalActor->SetSP( hLocalActor->GetMaxSP() );
}

void CDnSkillTask::AddTempSkill( int iSkillID )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor ) return;

	// 임시로 만들려고 하는 스킬이 강화 패시브 스킬인 경우 
	// 베이스가 되는 스킬을 갖고 있는지 찾아서 없다면 역시 임시 스킬로 베이스 스킬로 추가한다.
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	int iNeedBaseSkillID = 0;
	if( pSkillTable->IsExistItem( iSkillID ) )
		iNeedBaseSkillID = pSkillTable->GetFieldFromLablePtr( iSkillID, "_BaseSkillID" )->GetInteger();

	DNVector(int) vlSkillsToAdd;
	if( 0 < iNeedBaseSkillID )
	{
		// 강화 대상 스킬을 갖고 있지 않은 상태라면 이것도 새로 생성하도록 벡터에 넣어줌.
		if( false == hLocalActor->IsExistSkill( iNeedBaseSkillID ) )
			vlSkillsToAdd.push_back( iNeedBaseSkillID );
	}
	vlSkillsToAdd.push_back( iSkillID );

	for( int i = 0; i < (int)vlSkillsToAdd.size(); ++i )
	{
		int iSkillIDToAdd = vlSkillsToAdd.at( i );

		for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ )
		{
			if( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->GetClassID() == iSkillIDToAdd ) 
				return;
		}

		DnSkillHandle hSkill = CDnSkill::CreateSkill( hLocalActor, iSkillIDToAdd, 1 );
		if( !hSkill )
			return;
		m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].push_back( hSkill );
		// 스킬 레벨 데이터 디폴트는 무조건 PVE 
		CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
		bool bIsPVPGame = ((pGameTask) && (pGameTask->GetGameTaskType() == GameTaskType::PvP));
		int iSkillLevelDataType = CDnSkill::PVE;
		if( bIsPVPGame )
		{
			iSkillLevelDataType = CDnSkill::PVP;
		}
		hSkill->SelectLevelDataType( iSkillLevelDataType );

		hLocalActor->AddSkill( hSkill );

		hSkill->AsTempSkill();

		// 2차 전직스킬이라서 현재 사용할 수 없는 스킬이라면,
		// 현재 캐릭터 레벨 및 직업에 맞게 객체 값을 바꿔준다.
		int iLevel = hLocalActor->GetLevel();
		if( iLevel < hSkill->GetLevelLimit() )
			hSkill->SetLevelLimit( iLevel );

		CDnLocalPlayerActor* pLocalPlayer = static_cast<CDnLocalPlayerActor*>(hLocalActor.GetPointer());
		if( false == pLocalPlayer->IsPassJob( hSkill->GetNeedJobClassID() ) )
			hSkill->SetNeedJobClassID( pLocalPlayer->GetJobClassID() );
	}

	m_bTempSkillAdded = true;
}

void CDnSkillTask::RemoveTempSkill( int iSkillID )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor ) return;

	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
	{
		if( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->GetClassID() == iSkillID ) 
		{
			_ASSERT( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i] && m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->IsTempSkill() );
			hLocalActor->RemoveSkill( iSkillID );
			SAFE_RELEASE_SPTR( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i] );
			m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].erase( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].begin() + i );
			break;
		}
	}
}

void CDnSkillTask::RemoveAllTempSkill( void )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor ) return;

	vector<DnSkillHandle>::iterator iter = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].begin();
	for( iter; iter != m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].end(); )
	{
		DnSkillHandle hSkill = *iter;
		if( hSkill->IsTempSkill() )
		{
			hLocalActor->RemoveSkill( hSkill->GetClassID() );
			SAFE_RELEASE_SPTR( hSkill );
			iter = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].erase( iter );
		}
		else
			++iter;
	}

	m_bTempSkillAdded = false;
}

void CDnSkillTask::InitializeEnchantPassiveSkills( void )
{
	// 강화 패시브 타입의 스킬을 찾아서 베이스 스킬에 수치를 적용하도록 한다.
	for( DWORD i = 0; i < m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
	{
		if( CDnSkill::EnchantPassive == m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][ i ]->GetSkillType() &&
			0 < m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][ i ]->GetBaseSkillID() )
		{
			DnSkillHandle hEnchantPassiveSkill = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][ i ];
			int iBaseSkillID = hEnchantPassiveSkill->GetBaseSkillID();
			DnSkillHandle hBaseSkill = FindSkill( iBaseSkillID );

			// 예외처리 [2011/03/17 semozz]
			if (hBaseSkill)
				hBaseSkill->ApplyEnchantSkill( hEnchantPassiveSkill );
		}
	}
}

// 문장 스킬을 찼거나 할 때 이쪽으로 스킬이 추가된다.

void CDnSkillTask::AddSkill( int nSkillID, int iLevel /*= 1*/ , int nSkillPage /* = DualSkill::Type::None */ )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor ) return;

	if( nSkillPage == DualSkill::Type::None )
		nSkillPage = m_iSkillTreePage;

	if(nSkillPage < DualSkill::Type::Primary || nSkillPage >= DualSkill::Type::MAX )
		return;

	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[nSkillPage].size(); i++ )
	{
		if( m_vlhLocalPlayerOpenedSkillList[nSkillPage][i]->GetClassID() == nSkillID ) 
			return;
	}

	//스킬 레벨업 값 적용..
	int nLevelUpValue = hLocalActor->GetSkillLevelUpValue(nSkillID);
	iLevel += nLevelUpValue;

	DnSkillHandle hSkill = CDnSkill::CreateSkill( hLocalActor, nSkillID, iLevel );

	if (!hSkill && nLevelUpValue != 0)
	{
		OutputDebug("스킬 (%d) 레벨 (%d) 생성 실패.. 스킬 레벨 테이블 확인 요망!!!!!!\n", nSkillID, iLevel);
		_ASSERT( hSkill && "스킬 레벨업 시도 실패.. 레벨 테이블 확인" );

		//레벨값 다시 원래로 내리고
		iLevel -= nLevelUpValue;
		hSkill = CDnSkill::CreateSkill(hLocalActor, nSkillID, iLevel);

		//레벨업값 초기화
		nLevelUpValue = 0;
	}

	if( !hSkill )
		return;

	hSkill->SetLevelUpValue(nLevelUpValue);

	m_vlhLocalPlayerOpenedSkillList[nSkillPage].push_back( hSkill );

	// 스킬 레벨 데이터 디폴트는 무조건 PVE 
	CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	bool bIsPVPGame = ((pGameTask) && (pGameTask->GetGameTaskType() == GameTaskType::PvP));
	int iSkillLevelDataType = CDnSkill::PVE;
	if( bIsPVPGame )
	{
		iSkillLevelDataType = CDnSkill::PVP;
	}
	hSkill->SelectLevelDataType( iSkillLevelDataType );

	hLocalActor->AddSkill( hSkill );

	GetInterface().RefreshSkillDialog();
}

void CDnSkillTask::DelSkill( int nSkillID , int nSkillPage /* = DualSkill::Type::None */ )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor ) return;

	
	if( nSkillPage == DualSkill::Type::None )
		nSkillPage = m_iSkillTreePage;

	if(nSkillPage < DualSkill::Type::Primary || nSkillPage >= DualSkill::Type::MAX )
		return;


	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[nSkillPage].size(); i++ ) 
	{
		if( m_vlhLocalPlayerOpenedSkillList[nSkillPage][i]->GetClassID() == nSkillID ) 
		{
			hLocalActor->RemoveSkill( nSkillID );
			SAFE_RELEASE_SPTR( m_vlhLocalPlayerOpenedSkillList[nSkillPage][i] );
			m_vlhLocalPlayerOpenedSkillList[nSkillPage].erase( m_vlhLocalPlayerOpenedSkillList[nSkillPage].begin() + i );
			break;
		}
	}
	GetInterface().RefreshSkillDialog();
}

void CDnSkillTask::ErrorMessage_ReservationSkill( const int nError )
{
	switch( nError )
	{
		// 언락 실패 관련. ////////////////////////////////////////////////////////////////
	case ERROR_SKILL_UNLOCK_FAIL:
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 757) );
		GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 757) );
		break;

	case ERROR_SKILL_UNLOCK_ALREADY_OPEN:
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 722) );
		GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 722) );
		break;

		// 돈 주고 스킬 언락 할 때.
	case ERROR_SKILL_UNLOCK_NOT_ENOUGH_MONEY:
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1744) );
		GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1744) );
		break;

		// 획득 실패 관련. ////////////////////////////////////////////////////////////////
		// 캐릭터 요구레벨이 모자람.
	case ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_CHAR_LEVEL:
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1145) );
		GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1145) );
		break;

		// 선행(부모) 스킬이 없음.
	case ERROR_SKILL_ACQUIRE_FAIL_DONT_HAVE_PARENT_SKILL:
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1817) );
		GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1817) );
		break;

		// 부모 스킬의 레벨이 충족되지 않음.
	case ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_PARENT_SKILL_LEVEL:
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1817) );
		GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1817) );
		break;

		// 스킬 포인트가 모자라서 스킬을 획득할 수 없음.
	case ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_SKILLPOINT:
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1147) );
		GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1147) );
		break;

		// 다른 궁극기 스킬을 배워서 못 배움. 테이블에 exclusive id 가 같은 경우에만 이렇게 응답이 온다.
	case ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE:
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1153) );
		GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1153) );
		break;
	}
}

void CDnSkillTask::Unlock_ReservationSkill( const int nSkillID )
{
	DnSkillHandle hSkillToUnlock = FindLockedSkill( nSkillID );

	if( !hSkillToUnlock )
		return;

	hSkillToUnlock->UnLock();

	// 언락 스킬 리스트에 추가.
	m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].push_back( hSkillToUnlock );
}

void CDnSkillTask::Acquire_ReservationSkill( const int nSkillID, const bool bRefreshDialog )
{
	// 스킬 획득! 보유 스킬리스트에 넣어줌.
	DnSkillHandle hAcquiredSkill = FindZeroLevelSkill( nSkillID );
	if( !hAcquiredSkill )
		return;

	hAcquiredSkill->SetHasActor( CDnActor::s_hLocalActor );
	hAcquiredSkill->Acquire();

	vector<DnSkillHandle>::iterator iter = find( m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].begin(), m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].end(), hAcquiredSkill );
	_ASSERT( m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].end() != iter );
	if( m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].end() != iter )
	{
		m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].erase( iter );

		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		pPlayerActor->AddSkill( hAcquiredSkill );
		hAcquiredSkill->SelectLevelDataType( QueryPVEOrPVP() );
		m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].push_back( hAcquiredSkill );
		m_iSkillPoint[m_iSkillTreePage] -= hAcquiredSkill->GetNeedAcquireSkillPoint();

		if( true == bRefreshDialog )
		{
			GetInterface().RefreshSkillDialog();
			GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 723), MB_OK );
			GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 723) );
		}

		// 스킬 트리 UI 그릴 때 퀵슬롯 버튼 그릴 때 new 표시 붙여주기 위해서.
		hAcquiredSkill->SetNewAcquire();

		// 스킬 단축키 등록
		if( hAcquiredSkill->GetSkillType() == CDnSkill::Active )
		{
			CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
			int nEmptySlotIndex = pMainDlg->GetEmptyQuickSlotIndex();
			if( nEmptySlotIndex != -1 )
			{
				pMainDlg->SetQuickSlot( nEmptySlotIndex, hAcquiredSkill.GetPointer() );
			}

			// PvP콜로세움에서 스킬 배우면 쿨타임 적용되도록
			// #27498 [2011/01/17 semozz]
			CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
			if(	pTask && pTask->GetGameTaskType() == GameTaskType::PvP ) 
			{
				hAcquiredSkill->OnBeginCoolTime();
			}
		}

		// 스킬 언락 이펙트 보여줌.
		DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
		if( hHandle ) {
			hHandle->SetPosition( *(CDnActor::s_hLocalActor->GetPosition()) );
			hHandle->SetActionQueue( "BuySkill" );
			EtcObjectSignalStruct *pResult = CDnActor::s_hLocalActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, hHandle );

			pResult->vOffset = EtVector3( 0.f, 0.f, 0.f );
			pResult->vRotate = EtVector3( 0, 0, 0);
			pResult->bLinkObject = true;
			pResult->bDefendenceParent = true;
		}

		if( hAcquiredSkill->GetLevel() != 1 )
			return;

		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
		if( !pSox ) return;

		for( int itr = GLYPH_SKILL1; itr <= GLYPH_SKILL4; ++itr )
		{
			CDnItem *pItem = CDnItemTask::GetInstance().GetGlyphItem(itr);
			if( !pItem ) continue;
			if( dynamic_cast<CDnGlyph*>(pItem) && pItem->GetItemType() == ITEMTYPE_GLYPH ) {
				DnGlyphHandle hGlyph = ((CDnGlyph*)pItem)->GetMySmartPtr();

				int nSkillID = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_SkillID" )->GetInteger();

				if( hAcquiredSkill->GetClassID() == nSkillID )
					hAcquiredSkill->AddGlyphStateEffect( hGlyph->GetClassID() );
			}
		}


		//처음 획득 하는 시킬이 레벨업 정보가 설정 되어 있으면 레벨업을 시켜 준다..
		if (pPlayerActor)
		{
			int nLevelUpValue = pPlayerActor->GetSkillLevelUpValue(hAcquiredSkill->GetClassID());
			int nCurLevel = hAcquiredSkill->GetLevel();
			int nSkillID = hAcquiredSkill->GetClassID();

			if (nLevelUpValue != 0)
			{
				ChangeSkillLevelUp(nSkillID, nCurLevel , true );
			}
		}
	}
}

void CDnSkillTask::LevelUpSkill( int nSkillID, int nLevel, const bool bRefreshDialog )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor ) return;

	DnSkillHandle hSkillToLevelUp;
	int iSkillListIndex = -1;

	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
	{
		if( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->GetClassID() == nSkillID ) 
		{
			hSkillToLevelUp = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][ i ];
			iSkillListIndex = i;
			break;
		}
	}

	bool bValidIndex = (-1 != iSkillListIndex);
	_ASSERT( bValidIndex );
	if( bValidIndex )
	{
		int iNowSkillLevel = 0;
		float fElapsedDelay = 0.f;

		hSkillToLevelUp->CalcLevelUpUsePoint( 1 );
		m_iSkillPoint[m_iSkillTreePage] -= hSkillToLevelUp->GetUsePoint();
		fElapsedDelay = hSkillToLevelUp->GetElapsedDelayTime();
		hSkillToLevelUp->SetUsePoint( 0 );

		int nLevelUp = hLocalActor->GetSkillLevelUpValue(nSkillID);
		int nCurSkillLevel = hSkillToLevelUp->GetLevel();
		nLevel += nLevelUp;

		DnSkillHandle hLevelUppedSkill = CDnSkill::CreateSkill( hLocalActor, nSkillID, nLevel );

		//레벨업 스킬 생성이 실패 했다면 레벨업 취소...
		if (!hLevelUppedSkill && nLevelUp > 0)
		{
			OutputDebug("스킬 (%d) 레벨 (%d) 생성 실패.. 스킬 레벨 테이블 확인 요망!!!!!!\n", nSkillID, nLevel);
			_ASSERT( hLevelUppedSkill && "스킬 레벨업 시도 실패.. 레벨 테이블 확인" );

			//레벨값 다시 원래로 내리고
			nLevel -= nLevelUp;
			hLevelUppedSkill = CDnSkill::CreateSkill(hLocalActor, nSkillID, nLevel);

			//레벨업값 초기화
			nLevelUp = 0;
		}

		if (hLevelUppedSkill)
			hLevelUppedSkill->SetLevelUpValue(nLevelUp);

		if( true == hSkillToLevelUp->IsNewAcquire() )
			hLevelUppedSkill->SetNewAcquire();

		for( int i = 0; i < MAX_SKILL_SLOT_COUNT; ++i )
		{
			if( m_mapSkillTreeInfoByJobID[ hLevelUppedSkill->GetNeedJobClassID() ].aSkillTreeSlotInfo[ i ].hSkill == m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].at(iSkillListIndex) )
			{
				m_mapSkillTreeInfoByJobID[ hLevelUppedSkill->GetNeedJobClassID() ].aSkillTreeSlotInfo[ i ].hSkill = hLevelUppedSkill;
				break;
			}
		}
		
		m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].at( iSkillListIndex ) = hLevelUppedSkill;
		hLevelUppedSkill->SetElapsedDelayTime( fElapsedDelay );
		hLevelUppedSkill->SelectLevelDataType( hSkillToLevelUp->GetSelectedLevelDataType() );
		bool bResult = hLocalActor->ReplacementSkill( hLevelUppedSkill );
		_ASSERT( bResult );
		CDnLocalPlayerActor * pLocalPlayerActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));
		pLocalPlayerActor->ReplacementGlyph( hLevelUppedSkill ); 
		
		// 트리정보도 인수인계 받도록 해둡니다.
		CDnSkillTreeSystem::S_NODE_RENDER_INFO sInfo = hSkillToLevelUp->GetNodeRenderInfo();
		hLevelUppedSkill->SetNodeRenderInfo( sInfo );

#ifdef PRE_FIX_SKILL_TOOLTIP_REFRESH
		CDnSkillTreeDlg* pSkillDlg  = (CDnSkillTreeDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG );
		if( pSkillDlg )
		{
			CDnSkillTreeContentDlg* pSkillContentDlg = pSkillDlg->GetContentPanelDlg();
			if( pSkillContentDlg )
			{
				pSkillContentDlg->RefreshToolTip( hSkillToLevelUp );
			}
		}
#endif

#if defined(PRE_FIX_61821)
		DnSkillHandle hNewSkill = hLocalActor->FindSkill(hLevelUppedSkill->GetClassID());

		//패시브 스킬인 경우 패시브 스킬 UI갱신..
		CDnSkill::SkillTypeEnum skillType = hNewSkill->GetSkillType();
		switch(skillType)
		{
		case CDnSkill::Passive:
		case CDnSkill::AutoPassive:
			{
				CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
				if( pMainDlg && hNewSkill && hNewSkill->HasCoolTime() ) 
				{
					pMainDlg->ReplacePassiveSkill( hNewSkill );
				}
			}
			break;
		}
#endif // PRE_FIX_61821

		if( hLevelUppedSkill->GetSkillType() == CDnSkill::Active )
		{
			CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
			int nSkillSlotIndex = pMainDlg->GetSkillQuickSlotIndex( hLevelUppedSkill->GetClassID() );
			if( nSkillSlotIndex != -1 )
				pMainDlg->SetQuickSlot( nSkillSlotIndex, hLevelUppedSkill.GetPointer() );
		}

		SAFE_RELEASE_SPTR( hSkillToLevelUp );

		if( true == bRefreshDialog )
			GetInterface().RefreshSkillDialog();
	}
}


void CDnSkillTask::OnRecvOtherPlayerSkillLevelUp( SCOtherPlayerSkillLevelUp* pPacket )
{
	CDnPartyTask* pPartyTask = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	DWORD dwNumParty = pPartyTask->GetPartyCount();
	if( pPacket->nPartyIndex < (int)dwNumParty )
	{
		CDnPartyTask::PartyStruct* pParty = pPartyTask->GetPartyData( pPacket->nPartyIndex );
		pParty->hActor->RemoveSkill( pPacket->nSkillID );

		//DnSkillHandle hSkill = CDnSkill::CreateSkill( pParty->hActor, pPacket->nSkillID, pPacket->cLevel );
		//bool bResult = hLocalActor->ReplacementSkill( hSkill );
		//_ASSERT( bResult );
		//pParty->hActor->AddSkill( hSkill );
		pParty->hActor->AddSkill( pPacket->nSkillID, pPacket->cLevel );

	}
}

void CDnSkillTask::OnRecvPushSkillPoint( SCPushSkillPoint* pPacket )
{
	for(int nPage=0; nPage<DualSkill::Type::MAX; nPage++)
		SetSkillPoint( pPacket->usSkillPoint , nPage );
	GetInterface().RefreshSkillDialog();
}

// 스킬북/마을에서 돈주고 언락 모두 이 패킷으로 결과가 옴.
// 돈 차감은 따로 빌리지 서버에서 패킷 보내준다.
void CDnSkillTask::OnRecvUnlockSkill( SCUnlockSkill* pPacket )
{
	m_bRequestWait = false;
	if( ERROR_NONE == pPacket->nRetCode )
	{
		// 스킬 언락
		_ASSERT( 0 != pPacket->nSkillID );

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		Unlock_ReservationSkill( pPacket->nSkillID );
#else	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		// 해당 스킬을 찾아서 상태를 언락으로 변경시킨다.
		DnSkillHandle hSkillToUnlock = FindLockedSkill( pPacket->nSkillID );
		
		if( !hSkillToUnlock )
			return;

		hSkillToUnlock->UnLock();

		// 언락 스킬 리스트에 추가.
		m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].push_back( hSkillToUnlock );

#endif	//	#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	}
}

void CDnSkillTask::OnRecvAcquireSkill( SCAcquireSkill* pPacket )
{
	m_bRequestWait = false;
	if( ERROR_NONE == pPacket->nRetCode )
	{
		Acquire_ReservationSkill( pPacket->nSkillID );
	}
	else
	{
		// 획득 실패.
		ErrorMessage_ReservationSkill( pPacket->nRetCode );

		CDnTradeMail& tradeMail = GetTradeTask().GetTradeMail();
		if (tradeMail.IsOnMailMode())
			tradeMail.LockMailDlg(false);
	}
}

void CDnSkillTask::OnRecvSkillReset( SCSkillReset* pPacket )
{
	if( m_bAddSkillCheatEnabled )
	{
		m_bAddSkillCheatEnabled = false;
		return;
	}

	DNTableFileFormat*  pDefaultCreateTable = GetDNTable( CDnTableDB::TDEFAULTCREATE );
	CDnLocalPlayerActor* pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	vector<int> vlJobHistory;
	pLocalPlayerActor->GetJobHistory( vlJobHistory );
	int iFirstJob = (int)vlJobHistory.front();

	char caScratchBuffer[ 64 ];
	SecureZeroMemory( caScratchBuffer, sizeof(caScratchBuffer) );

	// 기본 스킬만 레벨 1로, 나머지 보유중인 모든 스킬을 미 acquire 상태로 새로 만듦
	vector<DnSkillHandle> vlhDefaultSkills;


	int iResetSkillPage = pPacket->cSkillPage;
	if(iResetSkillPage < DualSkill::Type::Primary || iResetSkillPage >= DualSkill::Type::MAX )
		return;

	int iNumOpenedSkill = (int)m_vlhLocalPlayerOpenedSkillList[iResetSkillPage].size();
	for( int iSkill = 0; iSkill < iNumOpenedSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhLocalPlayerOpenedSkillList[iResetSkillPage].at( iSkill );
		int iSkillID = hSkill->GetClassID();

		
		bool bDefaultSkill = false;
		for( int i = 0; i < DEFAULTSKILLMAX; ++i )
		{
			sprintf_s( caScratchBuffer, "_DefaultSkill%d", i );
			int iDefaultSkilID = pDefaultCreateTable->GetFieldFromLablePtr( iFirstJob, caScratchBuffer )->GetInteger();
			if( iDefaultSkilID == iSkillID )
			{
				bDefaultSkill = true;
				break;
			}
		}

		if( bDefaultSkill )
		{
			int nLevelUpValue = CDnActor::s_hLocalActor->GetSkillLevelUpValue(iSkillID);
			int nSkillLevel = 1;
			nSkillLevel += nLevelUpValue;

			DnSkillHandle hResettedDefaultSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, iSkillID, nSkillLevel );
			if (!hResettedDefaultSkill && nLevelUpValue > 0)
			{
				OutputDebug("스킬 (%d) 레벨 (%d) 생성 실패.. 스킬 레벨 테이블 확인 요망!!!!!!\n", iSkillID, nSkillLevel);
				_ASSERT( hResettedDefaultSkill && "스킬 레벨업 시도 실패.. 레벨 테이블 확인" );

				//레벨값 다시 원래로 내리고
				nSkillLevel -= nLevelUpValue;
				hResettedDefaultSkill = CDnSkill::CreateSkill(CDnActor::s_hLocalActor, iSkillID, nSkillLevel);

				//레벨업값 초기화
				nLevelUpValue = 0;
			}

			if (hResettedDefaultSkill)
				hResettedDefaultSkill->SetLevelUpValue(nLevelUpValue);
			
			hResettedDefaultSkill->SetHasActor( CDnActor::s_hLocalActor );
			hResettedDefaultSkill->Acquire();
			vlhDefaultSkills.push_back( hResettedDefaultSkill );
			
			// 디폴트 스킬인 경우에 1렙짜리 디폴트 스킬로 교체.
			CDnActor::s_hLocalActor->ReplacementSkill( hResettedDefaultSkill );
		}
		else
		{
			DnSkillHandle hResettedZeroLevelSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, iSkillID, 0 );
			m_vlhUnlockZeroLevelSkills[iResetSkillPage].push_back( hResettedZeroLevelSkill );
			pLocalPlayerActor->RemoveSkill( hResettedZeroLevelSkill->GetClassID() );
		}

		// 교체되던 제거되던 기존 스킬객체는 삭제해야 함.
		SAFE_RELEASE_SPTR( hSkill );
	}

	m_vlhLocalPlayerOpenedSkillList[iResetSkillPage].swap( vlhDefaultSkills );
	// 스킬 트리 정보 다시 꾸민다.
	m_mapSkillTreeInfoByJobID.clear();
	InitializeSkillTree();

	// 스킬포인트 서버에서 알려준 결과로 셋팅.
	// 스킬 레벨업을 하나도 하지 않으면 서버로부터 0으로 날아오므로 스킬 포인트를 그대로 둔다.

	if( 0 < pPacket->nResultSkillPoint )
	{
		m_iSkillPoint[iResetSkillPage] = pPacket->nResultSkillPoint;
	}

	GetInterface().RefreshSkillDialog();
}

void CDnSkillTask::ResetSkillsByJob( int iStartJobDegree, int iEndJobDegree, int iResultSP , int nSkillPage)
{
	DNTableFileFormat*  pDefaultCreateTable = GetDNTable( CDnTableDB::TDEFAULTCREATE );
	CDnLocalPlayerActor* pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	vector<int> vlJobHistory;
	pLocalPlayerActor->GetJobHistory( vlJobHistory );
	int iFirstJob = (int)vlJobHistory.front();

	// 서버에서 날아온 직업차수는 테이블에 정의된 그대로이므로 -1씩 빼줘야 인덱싱이 맞는다.
	--iStartJobDegree;
	--iEndJobDegree;

	_ASSERT( 0 <= iStartJobDegree && iStartJobDegree < (int)vlJobHistory.size() );
	_ASSERT( 0 <= iEndJobDegree && iEndJobDegree < (int)vlJobHistory.size() );

	vector<int> vlJobsToReset;
	for( int i = iStartJobDegree; i <= iEndJobDegree; ++i )
	{
		// 현재 캐릭터에 맞지 않는 직업 차수. 아이템의 스킬 리셋 직업 차수 구간과 같다면 
		// 현재 직업수준에 맞지 않다는 에러코드 클라로 리턴.
		// 하나라도 현재 직업 차수에 맞다면 사용 허용.
		if( i < (int)vlJobHistory.size() )
			vlJobsToReset.push_back( vlJobHistory.at(i) );
	}

	char caScratchBuffer[ 64 ];
	SecureZeroMemory( caScratchBuffer, sizeof(caScratchBuffer) );

	// 기본 스킬만 레벨 1로, 나머지 보유중인 모든 스킬을 미 acquire 상태로 새로 만듦
	vector<DnSkillHandle> vlhResultOpenedSkillList;
	if(nSkillPage < DualSkill::Type::Primary || nSkillPage >= DualSkill::Type::MAX )
		return;

	int iNumOpenedSkill = (int)m_vlhLocalPlayerOpenedSkillList[nSkillPage].size();
	for( int iSkill = 0; iSkill < iNumOpenedSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhLocalPlayerOpenedSkillList[nSkillPage].at( iSkill );
		// 리셋 요청 받은 직업의 스킬들만 리셋한다.
		vector<int>::iterator iter = find( vlJobsToReset.begin(), vlJobsToReset.end(), hSkill->GetNeedJobClassID() );
		if( vlJobsToReset.end() == iter )
		{
			vlhResultOpenedSkillList.push_back( hSkill );
			continue;
		}

		int iSkillID = hSkill->GetClassID();

		bool bDefaultSkill = false;
		for( int i = 0; i < DEFAULTSKILLMAX; ++i )
		{
			sprintf_s( caScratchBuffer, "_DefaultSkill%d", i );
			int iDefaultSkilID = pDefaultCreateTable->GetFieldFromLablePtr( iFirstJob, caScratchBuffer )->GetInteger();
			if( iDefaultSkilID == iSkillID )
			{
				bDefaultSkill = true;
				break;
			}
		}

		if( bDefaultSkill )
		{
			int nLevelUpValue = CDnActor::s_hLocalActor->GetSkillLevelUpValue(iSkillID);
			int nSkillLevel = 1;
			nSkillLevel += nLevelUpValue;

			DnSkillHandle hResettedDefaultSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, iSkillID, nSkillLevel );
			if (!hResettedDefaultSkill && nLevelUpValue > 0)
			{
				OutputDebug("스킬 (%d) 레벨 (%d) 생성 실패.. 스킬 레벨 테이블 확인 요망!!!!!!\n", iSkillID, nSkillLevel);
				_ASSERT( hResettedDefaultSkill && "스킬 레벨업 시도 실패.. 레벨 테이블 확인" );

				//레벨값 다시 원래로 내리고
				nSkillLevel -= nLevelUpValue;
				hResettedDefaultSkill = CDnSkill::CreateSkill(CDnActor::s_hLocalActor, iSkillID, nSkillLevel);

				//레벨업값 초기화
				nLevelUpValue = 0;
			}

			if (hResettedDefaultSkill)
				hResettedDefaultSkill->SetLevelUpValue(nLevelUpValue);

			hResettedDefaultSkill->SetHasActor( CDnActor::s_hLocalActor );
			hResettedDefaultSkill->Acquire();
			vlhResultOpenedSkillList.push_back( hResettedDefaultSkill );

			// 디폴트 스킬인 경우에 1렙짜리 디폴트 스킬로 교체.
			CDnActor::s_hLocalActor->ReplacementSkill( hResettedDefaultSkill );
		}
		else
		{
			DnSkillHandle hResettedZeroLevelSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, iSkillID, 0 );
			m_vlhUnlockZeroLevelSkills[nSkillPage].push_back( hResettedZeroLevelSkill );
			pLocalPlayerActor->RemoveSkill( hResettedZeroLevelSkill->GetClassID() );
		}

		// 교체되던 제거되던 기존 스킬객체는 삭제해야 함.
		SAFE_RELEASE_SPTR( hSkill );
	}

	m_vlhLocalPlayerOpenedSkillList[nSkillPage].swap( vlhResultOpenedSkillList );

	// 스킬 트리 정보 다시 꾸민다.
	m_mapSkillTreeInfoByJobID.clear();
	InitializeSkillTree();

	// 스킬포인트 서버에서 알려준 결과로 셋팅.
	// 직업 변경 아이템을 사용해서 스킬 리셋을 할 경우,
	// 스킬 레벨업을 하나도 하지 않으면 서버로부터 0으로 날아오므로 스킬 포인트를 그대로 둔다.
	if( 0 < iResultSP )
	{
		m_iSkillPoint[nSkillPage] = iResultSP;
	}

	GetInterface().RefreshSkillDialog();

	// 이펙트 보여줌.
	DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
	if( hHandle )
	{
		hHandle->SetPosition( *(CDnActor::s_hLocalActor->GetPosition()) );
		hHandle->SetActionQueue( "BuySkill" );
		EtcObjectSignalStruct *pResult = CDnActor::s_hLocalActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, hHandle );

		pResult->vOffset = EtVector3( 0.f, 0.f, 0.f );
		pResult->vRotate = EtVector3( 0, 0, 0);
		pResult->bLinkObject = true;
		pResult->bDefendenceParent = true;
	}
}

void CDnSkillTask::OnRecvCanUseSkillResetCashItem( SCCanUseSkillResetCashItem* pPacket )
{
	if( ERROR_NONE == pPacket->nRet )
	{
#ifdef PRE_MOD_SKILLRESETITEM
		CDnItem* pItem = GetItemTask().GetCashInventory().FindItemFromSerialID( pPacket->biItemSerial );
		if (pItem == NULL)
			pItem = GetItemTask().GetCharInventory().FindItemFromSerialID( pPacket->biItemSerial );
		_ASSERT( pItem );
		if( pItem )
		{
			// 실제로 리셋할 것인지 물어본다.
			GetInterface().OpenSkillResetConfirmDlg(pItem, pPacket->nRewardSP);
		}
#else
		CDnItem* pItem = GetItemTask().GetCashInventory().FindItemFromSerialID( pPacket->biItemSerial );
		_ASSERT( pItem );
		if( pItem )
		{
			// 실제로 리셋할 것인지 물어본다.
			GetInterface().OpenSkillResetConfirmDlg( pPacket->biItemSerial, pItem->GetTypeParam( 0 ), pItem->GetTypeParam( 1 ), pPacket->nRewardSP );
		}
#endif
	}
	else
	{
		switch( pPacket->nRet )
		{
			case ERROR_ITEM_DONT_HAVE_SKILL_TO_RESET:
				GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 114028) );
				//GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 114028) );
				break;

			case ERROR_ITEM_ALREADY_HAVE_HIGHER_JOB_SKILL:
				GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 114027) );
				//GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 114027) );
				break;

			case ERROR_ITEM_MISMATCH_RESET_SKILL_JOB:
				GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 114025) );
				//GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 114025) );
				break;
		}
	}
}

void CDnSkillTask::OnRecvUseSkillResetCashItemRes( SCUseSkillResetCashItemRes* pPacket )
{
	m_bRequestWait = false;
	if( ERROR_NONE == pPacket->nRet )
	{
		ResetSkillsByJob( m_iSkillResetStartJobDegree, m_iSkillResetEndJobDegree, pPacket->nResultSP , pPacket->cSkillPage );
		m_iSkillResetStartJobDegree = 0;
		m_iSkillResetEndJobDegree = 0;
	}
};

#ifdef PRE_ADD_PRESET_SKILLTREE
void CDnSkillTask::OnRecvSkillSetList(SCSKillSetList * packet)
{
	m_vecPresetSkillTree.clear();

	int nSkillCount = 0;
	for( int itr = 0; itr < SKILLPRESETMAX; ++itr )
	{
		m_vecPresetSkillTree.push_back( SPresetSkillTree() );

		m_vecPresetSkillTree[itr].m_cIndex = packet->SKillIndex[itr].cIndex;
		m_vecPresetSkillTree[itr].m_wszTreeName = std::wstring( packet->SKillIndex[itr].wszSetName );

		for( BYTE cCount = 0; cCount < packet->SKillIndex[itr].cCount; ++cCount )
		{
			int nSkillID = packet->SKills[nSkillCount].nSkillID;
			BYTE cLevel = packet->SKills[nSkillCount].cLevel;
			m_vecPresetSkillTree[itr].m_Data.push_back( std::make_pair( nSkillID, cLevel ) );
			++nSkillCount;
		}
	}

	for( DWORD itr = 0; itr < m_vecPresetSkillTree.size(); ++itr )
	{
		if( true == m_vecPresetSkillTree[itr].m_Data.empty() )
			continue;

		std::vector< std::pair<int, BYTE> > vecData;
		vecData.clear();

		AdviceSkillTreeSort( m_vecPresetSkillTree[itr].m_Data, vecData );
		m_vecPresetSkillTree[itr].m_Data.clear();

		m_vecPresetSkillTree[itr].m_Data.resize( vecData.size() );
		std::copy( vecData.begin(), vecData.end(), m_vecPresetSkillTree[itr].m_Data.begin() );
	}

	SetPresetSkillTree();
}

void CDnSkillTask::OnRecvSkillSetSaveResult(SCSKillSaveResult * pPacket)
{
	if( ERROR_NONE == pPacket->nRetCode )
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 745 ) );	// UISTRING : 스킬 트리를 저장했습니다.
	else
		return;

	if( m_SavePresetSkillTree.m_Data.empty() )
		return;

	for( DWORD itr = 0; itr < m_vecPresetSkillTree.size(); ++itr )
	{
		if( m_SavePresetSkillTree.m_cIndex != m_vecPresetSkillTree[itr].m_cIndex )
			continue;

		m_vecPresetSkillTree[itr].m_wszTreeName = m_SavePresetSkillTree.m_wszTreeName;

		m_vecPresetSkillTree[itr].m_Data.clear();
		m_vecPresetSkillTree[itr].m_Data.resize( m_SavePresetSkillTree.m_Data.size() );
		std::copy( m_SavePresetSkillTree.m_Data.begin(), m_SavePresetSkillTree.m_Data.end(), m_vecPresetSkillTree[itr].m_Data.begin() );

		break;
	}
	m_SavePresetSkillTree.Clear();

	CDnSkillTreeDlg* pSkillDlg  = (CDnSkillTreeDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG );
	if( pSkillDlg )
		pSkillDlg->UpdateView();
}

void CDnSkillTask::OnRecvSkillSetDeleteResult(SCSKillDeleteResult * pPacket)
{
	if( ERROR_NONE == pPacket->nRetCode )
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 109 ) );	// UISTRING : 삭제완료
	else
		return;

	for( DWORD itr = 0; itr < m_vecPresetSkillTree.size(); ++itr )
	{
		if( m_vecPresetSkillTree[itr].m_cIndex != m_nPresetSkillDeleteIndex )
			continue;

		m_vecPresetSkillTree[itr].Clear();
		m_vecPresetSkillTree[itr].m_cIndex = m_nPresetSkillDeleteIndex;
		m_vecPresetSkillTree[itr].m_bAdvice = false;
		break;
	}

	CDnSkillTreeDlg* pSkillDlg  = (CDnSkillTreeDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG );
	if( pSkillDlg )
		pSkillDlg->UpdateView();
}
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE

// 서버에 스킬 획득 요청
void CDnSkillTask::SendAcquireSkillReq( int iSkillID )
{
	if( !m_bRequestWait )
	{
		::SendAcquireSkillReq( iSkillID );
		m_bRequestWait = true;
	}
}

// 서버에 스킬 레벨업 요청
void CDnSkillTask::SendSkillLevelUpReq( int iSkillID, int iUsePoint )
{
	if( !m_bRequestWait )
	{
		if( 0 < m_iSkillPoint[m_iSkillTreePage] && iUsePoint <= m_iSkillPoint[m_iSkillTreePage] )
		{
			::SendSkillLevelUpReq( iSkillID, iUsePoint );
		}
		m_bRequestWait = true;
	}
}

// 스킬트레이너와 대화 후 돈으로 스킬 언락 요청
void CDnSkillTask::SendUnlockSkillByMoneyReq( int iSkillID )
{
	if( !m_bRequestWait )
	{
		::SendUnlockSkillByMoneyReq( iSkillID );
		m_bRequestWait = true;
	}
}

// 최종적으로 스킬리셋 캐쉬템 사용하겠다고 서버로 요청
#ifdef PRE_MOD_SKILLRESETITEM
void CDnSkillTask::SendUseSkillResetCashItemReq(const INT64 biItemSerial, const int iStartJobDegree, const int iEndJobDegree, const int iInvenType, const int iInvenIndex)
#else
void CDnSkillTask::SendUseSkillResetCashItemReq( INT64 biItemSerial, int iStartJobDegree, int iEndJobDegree )
#endif
{
	// 먼저 보냈던 것이 응답이 없다면 응답 올 때 까지 다시 요청 불가.
	if ((0 == m_iSkillResetStartJobDegree) && ( 0 == m_iSkillResetEndJobDegree))
	{
#ifdef PRE_MOD_SKILLRESETITEM
		::SendUseSkillResetCashItemReq(biItemSerial, iInvenType, iInvenIndex);
#else
		::SendUseSkillResetCashItemReq( biItemSerial );
#endif

		m_iSkillResetStartJobDegree = iStartJobDegree;
		m_iSkillResetEndJobDegree = iEndJobDegree;

		m_bRequestWait = true;
	}
}


DnSkillHandle CDnSkillTask::FindSkill( int nSkillID )
{
	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) {
		if( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->GetClassID() == nSkillID ) return m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i];
	}
	return CDnSkill::Identity();
}

DnSkillHandle CDnSkillTask::FindZeroLevelSkill( int nSkillID )
{
	for( DWORD i=0; i<m_vlhUnlockZeroLevelSkills[m_iSkillTreePage].size(); i++ ) {
		if( m_vlhUnlockZeroLevelSkills[m_iSkillTreePage][i]->GetClassID() == nSkillID ) return m_vlhUnlockZeroLevelSkills[m_iSkillTreePage][i];
	}

	return CDnSkill::Identity();
}

DnSkillHandle CDnSkillTask::FindLockedSkill( int nSkillID )
{
	map<int, S_SKILLTREE_INFO>::iterator iter = m_mapSkillTreeInfoByJobID.begin();

	for( iter; m_mapSkillTreeInfoByJobID.end() != iter; ++iter )
	{
		for( int iSlot = 0; iSlot < MAX_SKILL_SLOT_COUNT; ++iSlot )
		{
			S_SKILLTREE_SLOT_INFO& SkillTreeSlotInfo = iter->second.aSkillTreeSlotInfo[ iSlot ];
			if( SkillTreeSlotInfo.hSkill && nSkillID == SkillTreeSlotInfo.hSkill->GetClassID() )
				return SkillTreeSlotInfo.hSkill;
		}
	}

	return CDnSkill::Identity();
}

DnSkillHandle CDnSkillTask::FindSkillBySkillBookItemID( int iSkillBookItemID )
{
	int iSkillID = m_SkillTreeSystem.FindSkillBySkillBook( iSkillBookItemID );
	
	return FindSkill( iSkillID );
}

DnSkillHandle CDnSkillTask::FindZeroLevelSkillBySkillBookItemID( int iSkillBookItemID )
{
	int iSkillID = m_SkillTreeSystem.FindSkillBySkillBook( iSkillBookItemID );

	return FindZeroLevelSkill( iSkillID );
}

#ifdef PRE_ADD_LEVELUP_GUIDE
void CDnSkillTask::GetLevelUpSkillInfo(std::vector<int>& newSkillIdList, int newLevel, int jobId)
{
	m_SkillTreeSystem.GetLevelUpSkillInfo(newSkillIdList, newLevel, jobId);
}

void CDnSkillTask::GetLevelUpSkillInfo(std::vector<int>& newSkillIdList, int newLevel, const std::vector<int>& jobHistoryVec)
{
	m_SkillTreeSystem.GetLevelUpSkillInfo(newSkillIdList, newLevel, jobHistoryVec);
}
#endif

DnSkillHandle CDnSkillTask::FindEmblemSkill( void )
{
	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
	{
		if( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->IsEquipItemSkill() )
			return m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i];
	}

	return CDnSkill::Identity();
}

void CDnSkillTask::ResetNewAcquireSkills( void )
{
	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
	{
		if( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->IsNewAcquire() )
			m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->ResetNewAcquire();
	}
}

int CDnSkillTask::_GetAccumlatedNeedSkillPoint( DnSkillHandle hSkill, int iSkillLevel )
{
	int iAccuemlatedUsedSkillPoint = 0;

	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	vector<int> vlSkillLevelList;
	//pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", hSkill->GetClassID(), vlSkillLevelList );
	GetSkillLevelList( hSkill->GetClassID(), vlSkillLevelList, CDnSkill::PVE );		// pve 기준으로 처리하면 된다.
	for( DWORD k = 0; k < vlSkillLevelList.size(); k++ ) 
	{
		int iSkillLevelTableID = vlSkillLevelList.at( k );
		int iSkillLevelInTable = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillLevel" )->GetInteger();
		if( iSkillLevel < iSkillLevelInTable )
			break;

		int iNeedSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
		iAccuemlatedUsedSkillPoint += iNeedSkillPoint;
	}	
	
	return iAccuemlatedUsedSkillPoint;
}

int CDnSkillTask::GetWholeUsedSkillPoint( void )
{
	int iWholeUsedSkillPoint = 0;

	for( DWORD i = 0; i < m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
	{
		DnSkillHandle hSkill = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][ i ];

		if( hSkill->IsAcquired() )
		{
			// 테이블에서 값을 직접 참조.
			int iSkillLevel = hSkill->GetLevel();
			iSkillLevel -= hSkill->GetLevelUpValue();
			iWholeUsedSkillPoint += _GetAccumlatedNeedSkillPoint( hSkill, iSkillLevel );
		}
	}

	return iWholeUsedSkillPoint;
}

int CDnSkillTask::GetUsedSkillPointInThisJob( int iJobID )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor )
		return 0;

	int iWholeUsedSPInThisJob = 0;

	for( DWORD i = 0; i < m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
	{
		DnSkillHandle hSkill = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][ i ];

		int nItemLevel = 0;

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		DnSkillHandle hReservationSkill = GetReservationSkill( hSkill->GetClassID() );
		if( hReservationSkill )
		{
			nItemLevel = hLocalActor->GetSkillLevelUpValue( hReservationSkill->GetClassID() );
			hSkill = hReservationSkill;
		}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

		if( hSkill->IsAcquired() && 
			hSkill->GetNeedJobClassID() == iJobID )
		{
			// 테이블에서 값을 직접 참조.
			int iSkillLevel = hSkill->GetLevel() - nItemLevel;
			iSkillLevel -= hSkill->GetLevelUpValue();
			iWholeUsedSPInThisJob += _GetAccumlatedNeedSkillPoint( hSkill, iSkillLevel );
		}
	}

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	iWholeUsedSPInThisJob += GetUsedReservationSkillPointInThisJob( iJobID );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

	return iWholeUsedSPInThisJob;
}

CDnSkillTreeSystem::RESULT_CODE CDnSkillTask::CanUnlockThis( DnSkillHandle hSkill )
{
	if( !hSkill )
		return CDnSkillTreeSystem::R_ERROR;

	if( !CDnActor::s_hLocalActor )
		return CDnSkillTreeSystem::R_ERROR;

	CDnSkillTreeSystem::RESULT_CODE eResult = CDnSkillTreeSystem::R_ERROR;

	// 스킬북 소유여부는 여기서 확인하지 않는다.
	CDnSkillTreeSystem::S_OUTPUT Output;

	CDnSkillTreeSystem::S_TRY_UNLOCK TryUnlockInfo;
	TryUnlockInfo.iTryUnlockSkillID = hSkill->GetClassID();
	TryUnlockInfo.iCurrentCharLevel = CDnActor::s_hLocalActor->GetLevel();
	TryUnlockInfo.iSkillBookItemID = hSkill->GetUnlockSkillBookItemID();

	m_SkillTreeSystem.TryUnLockSkill( TryUnlockInfo, &Output );

#if defined( PRE_ADD_ONLY_SKILLBOOK )
	if( CDnSkillTreeSystem::R_ONLY_SKILL_BOOK == Output.eResult )
		return Output.eResult;
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )

	bool bExclusiveSkill = GetSkillTask().IsExclusiveSkill( hSkill->GetClassID(), hSkill->GetExclusiveID() );
	if( bExclusiveSkill ) return CDnSkillTreeSystem::R_EXCLUSIVE_SKILL;

	if( CDnSkillTreeSystem::R_SUCCESS == Output.eResult )
	{
		vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> vlPossessedSkillInfos;

		for( int i = 0; i < (int)m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); ++i )
		{
			CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
			DnSkillHandle hSkill = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].at( i );

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
			DnSkillHandle hReservationSkill = GetReservationSkill( hSkill->GetClassID() );
			if( hReservationSkill )
				hSkill = hReservationSkill;
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

			SkillInfo.iSkillID = hSkill->GetClassID();
			SkillInfo.iSkillLevel = hSkill->GetLevel();

			//#37528 선행 스킬 레벨업 조건 확인때 스킬레벨업에 의한 증가 수치는 무시 되도록
			//스킬레벨업 수치 만큼 차감한다.
			SkillInfo.iSkillLevel -= hSkill->GetLevelUpValue();

			SkillInfo.bCurrentLock = false;

			vlPossessedSkillInfos.push_back( SkillInfo );
		}

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		GetPossessReservationSkill( vlPossessedSkillInfos );
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

		CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( vlPossessedSkillInfos );
		TryAcquire.iCurrentCharLevel = CDnActor::s_hLocalActor->GetLevel();
		TryAcquire.iTryAcquireSkillID = hSkill->GetClassID();

		int iWholeAvailSPByJob = GetAvailSkillPointForThisJob( hSkill->GetNeedJobClassID() );
		TryAcquire.iHasSkillPoint = iWholeAvailSPByJob;
		
		m_SkillTreeSystem.TryAcquireSkill( TryAcquire, &Output );

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
		std::vector<int> nNeedSPValues;
		GetNeedSPValuesByJob(hSkill->GetClassID(), nNeedSPValues);

		bool bAvailableSPByJob = IsAvailableSPByJob(CDnActor::s_hLocalActor, nNeedSPValues);
		if (bAvailableSPByJob == false)
			Output.eResult = CDnSkillTreeSystem::R_ERROR;
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP
		
		eResult = Output.eResult;
	}

	return eResult;
}

void CDnSkillTask::RefreshAllSkillsDecreaseMP( void )
{
	map<int, S_SKILLTREE_INFO>::iterator iter = m_mapSkillTreeInfoByJobID.begin();
	for( iter; m_mapSkillTreeInfoByJobID.end() != iter; ++iter )
	{
		S_SKILLTREE_INFO& SkillInfo = iter->second;
		for( int i = 0; i < MAX_SKILL_SLOT_COUNT; ++i )
		{
			if( SkillInfo.aSkillTreeSlotInfo[ i ].hSkill )
			{
				SkillInfo.aSkillTreeSlotInfo[ i ].hSkill->RefreshDecreaseMP();
			}
		}
	}
}

int CDnSkillTask::GetAvailSkillPointForThisJob( int iNeedJobID )
{
	// 현재 직업 차수
	CDnLocalPlayerActor* pLocalPlayerActor = static_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
	int iJobDegree = -1;

	vector<int> vlJobHistory;
	pLocalPlayerActor->GetJobHistory( vlJobHistory );
	for( int i = 0; i < (int)vlJobHistory.size(); ++i )
	{
		int iJobID = vlJobHistory.at( i );
		if( iJobID == iNeedJobID )
		{
			iJobDegree = i;
			break;
		}
	}

	_ASSERT( -1 != iJobDegree );

	int iWholeAvailSPByJob = pLocalPlayerActor->GetSPUsingLimitByJobArrayIndex( iJobDegree );
	int iAvailSkillPoint = iWholeAvailSPByJob - GetSkillTask().GetUsedSkillPointInThisJob( iNeedJobID );

	// 전체 사용가능 SP 보다 직업 SP 가 남은 것이 많으면 전체 사용가능 SP 가 진짜이므로 해당 포인트로 리턴.
#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	const int nSkillPoint = GetSkillPoint() - GetReservationSkillNeedSP();
	if( nSkillPoint < iAvailSkillPoint )
		iAvailSkillPoint = nSkillPoint;
#else	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	if( GetSkillPoint() < iAvailSkillPoint )
		iAvailSkillPoint = GetSkillPoint();
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

	return iAvailSkillPoint;
}

bool CDnSkillTask::IsExclusiveSkill( int iSkillID, int iExclusiveID )
{
	// 배운 스킬 중에 같은 스킬 못 배우게 하는 id 가 있으면 true.

	int iNumSkill = (int)m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size();
	for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].at( iSkill );
		
		// 자신의 스킬은 제외하고.
		if( hSkill->GetClassID() == iSkillID )
			continue;

		if( 0 == hSkill->GetExclusiveID() )
			continue;

		if( iExclusiveID == hSkill->GetExclusiveID() )
		{
			return true;
		}
	}

	return false;
}

void CDnSkillTask::GetSkillLevelList( int iSkillID, vector<int>& vlSkillLevelList, int iLevelDataType )
{
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillID, vlSkillLevelList );

	// pve, pvp 대상인지 확인하여 걸러냄.
	vector<int>::iterator iterLevelList = vlSkillLevelList.begin();
	for( iterLevelList; iterLevelList != vlSkillLevelList.end(); )
	{
		int iSkillLevelTableID = *iterLevelList;
		int iApplyType = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_ApplyType" )->GetInteger();
		if( iApplyType != iLevelDataType )
			iterLevelList = vlSkillLevelList.erase( iterLevelList );
		else
			++iterLevelList;
	}
}

CSecondarySkill * CDnSkillTask::GetSecondarySkill( SecondarySkill::SubType::eType eSecondaryType )
{
	std::map<int,CSecondarySkill*>::iterator iter = m_LifeSkillRepository.GetSecondarySkillMap().begin();
	for( ; iter != m_LifeSkillRepository.GetSecondarySkillMap().end(); iter++ )
	{
		if( eSecondaryType == ( (*iter).second )->GetSubType() )
		{
			return (*iter).second;
		}
	}
	return NULL;
}

void CDnSkillTask::SendLifeSkillDelete( int nSkillID )
{
	SecondarySkill::CSDelete LifeSkillDelete;
	LifeSkillDelete.iSkillID = nSkillID;

	CClientSessionManager::GetInstance().SendPacket( CS_SECONDARYSKILL, eSecondarySkill::CS_DELETE, (char*)&LifeSkillDelete, int(sizeof(LifeSkillDelete)) );
}

void CDnSkillTask::OnRecvLifeSkillAdd( SecondarySkill::SCCreate* pPacket )
{
	if( pPacket == NULL )
		return;

	m_LifeSkillRepository.Create( pPacket->iSkillID );

	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
	if( pSkillTreeDlg && pSkillTreeDlg->IsShow() )
		pSkillTreeDlg->LifeSkillAdd( m_LifeSkillRepository.Get( pPacket->iSkillID ) );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkill );
	SecondarySkill::SubType::eType SubType = (SecondarySkill::SubType::eType)pSox->GetFieldFromLablePtr( pPacket->iSkillID, "_SecondarySkillType" )->GetInteger();

	WCHAR wszString[256] = {0,};
	if( SecondarySkill::SubType::CultivationSkill == SubType )
		swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7316 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7301 ) );
	else if( SecondarySkill::SubType::CookingSkill == SubType )
		swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7316 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7300 ) );
	else if( SecondarySkill::SubType::FishingSkill == SubType )
		swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7316 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7302 ) );

	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, false );	//%S 를 습득하였습니다
}

void CDnSkillTask::OnRecvLifeSkillDelete( SecondarySkill::SCDelete* pPacket )
{
	if( pPacket == NULL )
		return;

	if( pPacket->iRet == 0 )
	{
		m_LifeSkillRepository.Delete( pPacket->iSkillID );

		CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
		if( pSkillTreeDlg && pSkillTreeDlg->IsShow() )
			pSkillTreeDlg->LifeSkillDelete( pPacket->iSkillID );

		GetInterface().GetMainBarDialog()->ResetSecondarySkillQuickSlot( pPacket->iSkillID );

		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkill );
		SecondarySkill::SubType::eType SubType = (SecondarySkill::SubType::eType)pSox->GetFieldFromLablePtr( pPacket->iSkillID, "_SecondarySkillType" )->GetInteger();

		WCHAR wszString[256] = {0,};
		if( SecondarySkill::SubType::CultivationSkill == SubType )
			swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7317 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7301 ) );
		else if( SecondarySkill::SubType::CookingSkill == SubType )
			swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7317 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7300 ) );
		else if( SecondarySkill::SubType::FishingSkill == SubType )
			swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7317 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7302 ) );
		
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, false );	//	%S를 삭제하였습니다
	}
	else if( pPacket->iRet == ERROR_SECONDARYSKILL_DELETE_FAILED )
	{
		// 보조스킬 삭제 실패
	}
	else if( pPacket->iRet == ERROR_SECONDARYSKILL_DELETE_FAILED_DONTALLOWMAP )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7514 ) );
	}
}

void CDnSkillTask::OnRecvLifeSkillUpdateExp( SecondarySkill::SCUpdateExp* pPacket )
{
	if( pPacket == NULL )
		return;

	CSecondarySkill* pLifeSkill = m_LifeSkillRepository.Get( pPacket->iSkillID );

	if( pLifeSkill )
	{
		WCHAR wszString[256];
		swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7474 ), pPacket->iExp - pLifeSkill->GetExp() );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString, false );	// 스킬 숙련도가 %d가 올랐습니다.

		pLifeSkill->SetGrade( pPacket->Grade );
		pLifeSkill->SetLevel( pPacket->iLevel );
		pLifeSkill->SetExp( pPacket->iExp, NULL );
	}

	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
	if( pSkillTreeDlg && pSkillTreeDlg->IsShow() )
		pSkillTreeDlg->LifeSkillUpdateExp( pPacket->iSkillID, pPacket->Grade, pPacket->iLevel, pPacket->iExp );
}

void CDnSkillTask::OnRecvLifeSkillList( SecondarySkill::SCList* pPacket )
{
	if( pPacket == NULL || pPacket->SkillList == NULL )
		return;

	if( pPacket->cCount > SecondarySkill::Max::LearnSecondarySkill )
		return;

	for( int i=0; i<pPacket->cCount; i++ )
	{
		m_LifeSkillRepository.Add( pPacket->SkillList[i].iSkillID, pPacket->SkillList[i].iExp );
		CSecondarySkill* pLifeSkill = m_LifeSkillRepository.Get( pPacket->SkillList[i].iSkillID );

		if( pLifeSkill )
		{
			pLifeSkill->SetGrade( pPacket->SkillList[i].Grade );
			pLifeSkill->SetLevel( pPacket->SkillList[i].iLevel );
		}
	}
}

bool CDnSkillTask::CheckAvailableDeleteLifeSkill( int nSkillID )
{
	DNTableFileFormat* pSoxSkill = GetDNTable( CDnTableDB::TSecondarySkill );
	if( pSoxSkill == NULL )
	{
		DN_ASSERT( 0, "Invalid SecondarySkillType" );
		return false;
	}

	bool bAvailableDelete = false;

	SecondarySkill::SubType::eType SubType;
	SubType = (SecondarySkill::SubType::eType)pSoxSkill->GetFieldFromLablePtr( nSkillID, "_SecondarySkillType" )->GetInteger();

	switch( SubType )
	{
	case SecondarySkill::SubType::CookingSkill:
		{
			CManufactureSkill* pCookingSkill = GetLifeSkillCookingTask().GetCookingSkill();
			if( pCookingSkill )
			{
				if( static_cast<int>( pCookingSkill->GetRecipeCount() ) == 0 )
					bAvailableDelete = true;
				else
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7314 ) );
			}
			else
				bAvailableDelete = true;
		}
		break;
	default:
		{
			bAvailableDelete = true;
		}
		break;
	}

	return bAvailableDelete;
}

void CDnSkillTask::DoLifeSkill( int nSkillID )
{
	DNTableFileFormat* pSoxSkill = GetDNTable( CDnTableDB::TSecondarySkill );
	if( pSoxSkill == NULL )
	{
		DN_ASSERT( 0, "Invalid SecondarySkillType" );
		return;
	}

	SecondarySkill::SubType::eType SubType;
	SubType = (SecondarySkill::SubType::eType)pSoxSkill->GetFieldFromLablePtr( nSkillID, "_SecondarySkillType" )->GetInteger();

	switch( SubType )
	{
	case SecondarySkill::SubType::CookingSkill:
		{
			DoSkillCooking();
		}
		break;
	case SecondarySkill::SubType::FishingSkill:
		{
			DoSkillFishing();
		}
		break;
	default:
		{
		}
		break;
	}
}

void CDnSkillTask::DoSkillFishing()
{
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
	if( pPlayer )
	{
		CDnLifeSkillFishingTask* pTask = dynamic_cast<CDnLifeSkillFishingTask*>( CTaskManager::GetInstance().GetTask( "LifeSkillFishingTask" ) );
		if( pTask )
			pTask->SendFishingReady();
	}
}

void CDnSkillTask::DoSkillCooking()
{
	GetInterface().OpenCookingDialog( true );
}

void CDnSkillTask::DoSkillNone()
{
}

void CDnSkillTask::LoadGuildWarSkillList( PvPCommon::Team::eTeam eTeam )
{
	SAFE_RELEASE_SPTRVEC( m_vlhGuildWarSkillList );

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLTREE );
	if( pSox == NULL )
	{
		DN_ASSERT( 0, "Invalid BattleGroundSkillTree" );
		return;
	}

	for( int i=0; i<=pSox->GetItemCount(); i++ )
	{
		int nItemID = pSox->GetItemID( i );
		int nTeamID = pSox->GetFieldFromLablePtr( nItemID, "_TeamID" )->GetInteger();

		if( nTeamID == 0 || nTeamID == eTeam )
		{
			int nSkillID = pSox->GetFieldFromLablePtr( nItemID, "_SkillTableID" )->GetInteger();
			DnSkillHandle hSkill;
			hSkill = CDnGuildWarSkill::CreateSkill( CDnActor::Identity(), nSkillID, 0 );
			if( hSkill )
				m_vlhGuildWarSkillList.push_back( hSkill );
		}
	}
}

DnSkillHandle CDnSkillTask::FindGuildWarSkill( int nSkillTableID )
{
	DnSkillHandle hRetSkill;
	vector<DnSkillHandle>::iterator iter = m_vlhGuildWarSkillList.begin();

	for( ; iter != m_vlhGuildWarSkillList.end(); iter++ )
	{
		DnSkillHandle hSkill = (*iter); 
		if( hSkill->GetClassID() == nSkillTableID )
		{
			hRetSkill = (*iter);
			break;
		}
	}

	return hRetSkill;
}

bool CDnSkillTask::IsGuildWarSkill( int nSkillTableID )
{
	DnSkillHandle hSkill = FindGuildWarSkill( nSkillTableID );
	if( hSkill )
		return true;

	return false;
}

void CDnSkillTask::_OnRecvPVPSkillMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd )
	{
		case ePvP::SC_PVP_TRYACQUIRESKILL:	OnRecvAcquireGuildWarSkillReq( (SCPvPTryAcquireSkill*)pData );	break;
		case ePvP::SC_PVP_INITSKILL:		OnRecvGuildWarSkillInit( (SCPvPInitSkill*)pData );				break;
		case ePvP::SC_PVP_USESKILL:			OnRecvUseGuildWarSkill( (SCPvPUseSkill*)pData );				break;
		case ePvP::SC_PVP_OCCUPATION_SKILLSTATE:	OnRecvGuildWarSkillState( (SCPvPOccupationSkillState*)pData );	break;
		case ePvP::SC_PVP_SWAPSKILLINDEX:			OnRecvGuildSkillSwapIndex( (SCPvPSwapSkillIndex*)pData ); break;
	}
}

void CDnSkillTask::SendAcquireGuildWarSkillReq( int nSkillID, int nSkillLevel, int nReservationIndex )
{
	CDnMainDlg* pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
	if( pMainDlg == NULL )
		return;

	CDnGuildWarSkillSlotDlg* pGuildWarSkillSlotDlg = pMainDlg->GetGuildWarSkillSlotDialog();
	if( pGuildWarSkillSlotDlg == NULL )
		return;

	int nEmptySlotIndex = pGuildWarSkillSlotDlg->FindEmptyQuickSlotIndex() + nReservationIndex;

	CSPvPTryAcquireSKill AcquireSkillReq;
	SecureZeroMemory( &AcquireSkillReq, sizeof(CSPvPTryAcquireSKill) );
	AcquireSkillReq.nSkillID = nSkillID;
	AcquireSkillReq.nLevel = nSkillLevel;
	AcquireSkillReq.cSlotIndex = nEmptySlotIndex;

	CClientSessionManager::GetInstance().SendPacket( CS_PVP, ePvP::CS_TRYACQUIRE_SKILL, (char*)&AcquireSkillReq, int(sizeof(AcquireSkillReq)) );
}

void CDnSkillTask::SendGuildWarSkillInit()
{
	CClientSessionManager::GetInstance().SendPacket( CS_PVP, ePvP::CS_INITSKILL, NULL, 0 );
}

void CDnSkillTask::SendUseGuildWarSkill( int nSkillID )
{
	CSPvPUseSkill UseSkill;
	UseSkill.nSkillID = nSkillID;

	CClientSessionManager::GetInstance().SendPacket( CS_PVP, ePvP::CS_USESKILL, (char*)&UseSkill, int(sizeof(UseSkill)) );
}

void CDnSkillTask::SendGuildSkillSwapIndex( int nFromIndex, int nToIndex )
{
	CSPvPSwapSkillIndex SwapIndex;
	SwapIndex.cFromIndex = nFromIndex;
	SwapIndex.cToIndex = nToIndex;

	CClientSessionManager::GetInstance().SendPacket( CS_PVP, ePvP::CS_SWAPSKILLINDEX, (char*)&SwapIndex, int(sizeof(SwapIndex)) );
}

void CDnSkillTask::OnRecvAcquireGuildWarSkillReq( SCPvPTryAcquireSkill* pPacket )
{
	_ASSERT( pPacket );

	if( pPacket->nRetCode == ERROR_NONE )
	{
		DnSkillHandle hSkill;
		vector<DnSkillHandle>::iterator iter = m_vlhGuildWarSkillList.begin();
		for( ; iter != m_vlhGuildWarSkillList.end(); iter++ )
		{
			hSkill = (*iter);
			if( hSkill && hSkill->GetClassID() == pPacket->nSkillID )
			{
				hSkill->SetHasActor( CDnActor::s_hLocalActor );
				hSkill->SelectLevelDataType( CDnSkill::PVP, true );
				hSkill->Acquire();
				break;
			}
		}

		if( hSkill != NULL && hSkill->GetSkillType() == CDnSkill::Active )
		{
			// 스킬 단축키 등록
			CDnMainDlg* pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
			if( pMainDlg )
			{
				CDnGuildWarSkillSlotDlg* pGuildWarSkillSlotDlg = pMainDlg->GetGuildWarSkillSlotDialog();
				int nEmptySlotIndex = pGuildWarSkillSlotDlg->FindEmptyQuickSlotIndex();
				if( nEmptySlotIndex > -1 )
					pGuildWarSkillSlotDlg->SetSlot( nEmptySlotIndex, hSkill.GetPointer() );

				GetInterface().RefreshSkillDialog();

				if( pGuildWarSkillSlotDlg->IsShow() )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 723), MB_OK );
					GetInterface().AddChatMessage( CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 723) );
				}
			}
		}

		GetGuildWarTask().UseGuildWarSkillPoint();
	}
	else
	{
//		GetInterface().ServerMessageBox( pPacket->nRetCode );
	}
}

void CDnSkillTask::OnRecvGuildWarSkillInit( SCPvPInitSkill* pPacket )
{
	_ASSERT( pPacket );

	if( pPacket->nRetCode == ERROR_NONE )
	{
	}
	else
	{
	}
}

void CDnSkillTask::OnRecvUseGuildWarSkill( SCPvPUseSkill* pPacket )
{
	_ASSERT( pPacket );

	if( pPacket->nRetCode == ERROR_NONE )
	{
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
		if( CDnLocalPlayerActor::s_hLocalActor == NULL ) return;
		if( hActor == NULL ) return;
		
		DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );
		if( pSkillLevelTable == NULL ) return;
		int nItemID = pSkillLevelTable->GetItemIDFromField( "_SkillIndex", pPacket->nSkillID );

		if( CDnLocalPlayerActor::s_hLocalActor->GetTeam() == hActor->GetTeam() )	// 같은팀인 경우 쿨타임, 알람 설정
		{
			// 쿨타임 돌리기
			DnSkillHandle hSkill = GetSkillTask().FindGuildWarSkill( pPacket->nSkillID );
			if( !hSkill ) return;
			if( hSkill->GetSkillType() != CDnSkill::Active ) return;
			hSkill->OnBeginCoolTime();

			// 출력 알람
			int nAlarmStaringIndex = pSkillLevelTable->GetFieldFromLablePtr( nItemID, "_GuildSkillAlarmID" )->GetInteger();
			if( nAlarmStaringIndex > 0 )
				GetInterface().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nAlarmStaringIndex ), textcolor::TOMATO, 3.0f );
		}

		// 액션!
		string strActionName = pSkillLevelTable->GetFieldFromLablePtr( nItemID, "_ProcessParam1" )->GetString();
		if( strActionName.length() > 0 )
		{
			CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor *>( hActor.GetPointer() );
			if( pActor == NULL ) return;

			if( !pActor->IsBattleMode() )
				pActor->CmdToggleBattle( true );

			hActor->CDnActor::CmdAction( strActionName.c_str(), 0, 3.0f, 0.0f, true, false );
		}
	}
	else
	{
//		GetInterface().ServerMessageBox( pPacket->nRetCode );
	}
}

void CDnSkillTask::OnRecvGuildWarSkillState( SCPvPOccupationSkillState* pPacket )
{
	_ASSERT( pPacket );
	if( !CDnGuildWarTask::IsActive() )
		return;

	GetGuildWarTask().ResetGuildWarSkillPoint();

	for( int i=0; i<QUICKSLOTMAX; i++ )
	{
		if( pPacket->Info.nSkillID[i] == 0 ) break;

		DnSkillHandle hSkill;
		vector<DnSkillHandle>::iterator iter = m_vlhGuildWarSkillList.begin();
		for( ; iter != m_vlhGuildWarSkillList.end(); iter++ )
		{
			hSkill = (*iter);
			if( hSkill && hSkill->GetClassID() == pPacket->Info.nSkillID[i] )
			{
				hSkill->SetHasActor( CDnActor::s_hLocalActor );
				hSkill->SelectLevelDataType( CDnSkill::PVP, true );
				hSkill->Acquire();

				GetGuildWarTask().UseGuildWarSkillPoint();
				break;
			}
		}

		if( hSkill != NULL && hSkill->GetSkillType() == CDnSkill::Active )
		{
			// 스킬 단축키 등록
			CDnMainDlg* pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
			if( pMainDlg )
			{
				CDnGuildWarSkillSlotDlg* pGuildWarSkillSlotDlg = pMainDlg->GetGuildWarSkillSlotDialog();
				pGuildWarSkillSlotDlg->SetSlot( pPacket->Info.cSlotIndex[i], hSkill.GetPointer() );

				if( pPacket->Info.nSKillUseElapsedTick[i] > 0 )
				{
					float fElapsedDelayTime = hSkill->GetDelayTime() - ( pPacket->Info.nSKillUseElapsedTick[i] / 1000.0f );
					if( fElapsedDelayTime >= 0.0f )
						hSkill->SetElapsedDelayTime( hSkill->GetDelayTime() - ( pPacket->Info.nSKillUseElapsedTick[i] / 1000.0f ) );
				}
			}
		}
	}

	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
	if( pSkillTreeDlg )
	{
		if( pSkillTreeDlg->IsShow() )
		{
			pSkillTreeDlg->UpdateView();
			pSkillTreeDlg->UpdateGuildWarSkillTreeContent();
		}
	}
}

void CDnSkillTask::OnRecvGuildSkillSwapIndex( SCPvPSwapSkillIndex* pPacket )
{
	_ASSERT( pPacket );

	if( pPacket->nRetCode == ERROR_NONE )
	{
		CDnMainDlg* pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
		if( pMainDlg )
		{
			CDnGuildWarSkillSlotDlg* pGuildWarSkillSlotDlg = pMainDlg->GetGuildWarSkillSlotDialog();
			if( pGuildWarSkillSlotDlg->IsShow() )
				pGuildWarSkillSlotDlg->SwapQuickSlot( pPacket->cFrom, pPacket->cTo );
		}
	}
	else
	{
//		GetInterface().ServerMessageBox( pPacket->nRetCode );
	}
}

void CDnSkillTask::AddSkill( DnSkillHandle hSkill )
{
	if (!hSkill) return;

	m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].push_back( hSkill );

	GetInterface().RefreshSkillDialog();
}

void CDnSkillTask::RemoveSkill( DnSkillHandle hSkill )
{
	if (!hSkill) return;

	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
	{
		if( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->GetClassID() == hSkill->GetClassID() ) 
		{
			SAFE_RELEASE_SPTR( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i] );
			m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].erase( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].begin() + i );
			break;
		}
	}

	GetInterface().RefreshSkillDialog();
}

void CDnSkillTask::ChangeSkillLevelUp( int nSkillID, int nOrigLevel , bool bCurrentPageOnly )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor ) return;

	DnSkillHandle hSkillToLevelUp;
	int iSkillListIndex = -1;

	
	int nChangingValue = 0; // 변동폭 -> 다른 페이지의 스킬도 변동폭만큼 스킬을 변경해준다.
	
	for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
	{
		if( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->GetClassID() == nSkillID ) 
		{
			int nLevelUp = hLocalActor->GetSkillLevelUpValue(nSkillID);
			int nLevel = nOrigLevel;
			nLevel += nLevelUp;

			nChangingValue = nLevel - m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][ i ]->GetLevel();
		}
	}

	for(int nSkillPage = DualSkill::Type::Primary; nSkillPage< DualSkill::Type::MAX; nSkillPage++ )
	{
		if( bCurrentPageOnly && m_iSkillTreePage != nSkillPage)
			continue;

		for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[nSkillPage].size(); i++ ) 
		{
			if( m_vlhLocalPlayerOpenedSkillList[nSkillPage][i]->GetClassID() == nSkillID ) 
			{
				hSkillToLevelUp = m_vlhLocalPlayerOpenedSkillList[nSkillPage][ i ];
				iSkillListIndex = i;
				break;
			}
		}

		bool bValidIndex = (-1 != iSkillListIndex);
		_ASSERT( bValidIndex );
		if( bValidIndex && hSkillToLevelUp)
		{
			int iNowSkillLevel = 0;
			float fElapsedDelay = 0.f;

			fElapsedDelay = hSkillToLevelUp->GetElapsedDelayTime();
			
			int nLevelUp = hLocalActor->GetSkillLevelUpValue(nSkillID);
			int nLevel = hSkillToLevelUp->GetLevel() + nChangingValue;


			DnSkillHandle hLevelUppedSkill = CDnSkill::CreateSkill( hLocalActor, nSkillID, nLevel );


			//레벨업 스킬 생성이 실패 했다면 레벨업 취소...
			if (!hLevelUppedSkill && nLevelUp > 0)
			{
				OutputDebug("스킬 (%d) 레벨 (%d) 생성 실패.. 스킬 레벨 테이블 확인 요망!!!!!!\n", nSkillID, nLevel);
				_ASSERT( hLevelUppedSkill && "스킬 레벨업 시도 실패.. 레벨 테이블 확인" );

				//레벨값 다시 원래로 내리고
				nLevel -= nLevelUp;
				hLevelUppedSkill = CDnSkill::CreateSkill(hLocalActor, nSkillID, nLevel);

				//레벨업값 초기화
				nLevelUp = 0;
			}

			if (hLevelUppedSkill)
				hLevelUppedSkill->SetLevelUpValue(nLevelUp);

			for( int i = 0; i < MAX_SKILL_SLOT_COUNT; ++i )
			{
				if(nSkillPage == m_iSkillTreePage ) // 지금 활성화된 페이지 스킬만 설정하도록 한다
				{
					if( m_mapSkillTreeInfoByJobID[ hLevelUppedSkill->GetNeedJobClassID() ].aSkillTreeSlotInfo[ i ].hSkill == m_vlhLocalPlayerOpenedSkillList[nSkillPage].at(iSkillListIndex) )
					{
						m_mapSkillTreeInfoByJobID[ hLevelUppedSkill->GetNeedJobClassID() ].aSkillTreeSlotInfo[ i ].hSkill = hLevelUppedSkill;
						break;
					}
				}
			}

			m_vlhLocalPlayerOpenedSkillList[nSkillPage].at( iSkillListIndex ) = hLevelUppedSkill;

			hLevelUppedSkill->SetElapsedDelayTime( fElapsedDelay );
			hLevelUppedSkill->SelectLevelDataType( hSkillToLevelUp->GetSelectedLevelDataType() );


			if(nSkillPage == m_iSkillTreePage ) // 지금 활성화된 페이지 스킬만 설정하도록 한다
			{
				bool bResult = hLocalActor->ReplacementSkill( hLevelUppedSkill );
				_ASSERT( bResult );
				CDnLocalPlayerActor * pLocalPlayerActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));
				pLocalPlayerActor->ReplacementGlyph( hLevelUppedSkill ); 
			}

			SAFE_RELEASE_SPTR( hSkillToLevelUp );

			GetInterface().RefreshSkillDialog();
		}
	}
}

bool CDnSkillTask::HasSameGlobalIDSkill( DnSkillHandle hSkill )
{
	bool bResult = false;

	if( 0 < hSkill->GetGlobalSkillGroupID() )
	{
		for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size(); i++ ) 
		{
			if( m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i]->GetGlobalSkillGroupID() == hSkill->GetGlobalSkillGroupID() )
			{
				bResult = true;
				break;
			}
		}

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
		std::map< int, DnSkillHandle >::iterator Itor = m_mapReservationSkillHandle.begin();
		for( ; Itor != m_mapReservationSkillHandle.end(); ++Itor )
		{
			DnSkillHandle hReservationSkill = Itor->second;

			if( hReservationSkill->GetGlobalSkillGroupID() == hSkill->GetGlobalSkillGroupID() )
				return true;
		}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
	}

	return bResult;
}



DnSkillHandle CDnSkillTask::FindSkillBySkillPage( int nSkillID , int nSkillPage , bool bEntirePage )
{
	if(nSkillPage < DualSkill::Type::Primary || nSkillPage >= DualSkill::Type::MAX )
		return CDnSkill::Identity();

	for( int nPage = DualSkill::Type::Primary; nPage < DualSkill::Type::MAX; nPage++ )
	{
		if( !bEntirePage && nPage != nSkillPage )
			continue;

		for( DWORD i=0; i<m_vlhLocalPlayerOpenedSkillList[nPage].size(); i++ ) 
		{
			if( m_vlhLocalPlayerOpenedSkillList[nPage][i] && m_vlhLocalPlayerOpenedSkillList[nPage][i]->GetClassID() == nSkillID ) 
			{
				return m_vlhLocalPlayerOpenedSkillList[nPage][i];
			}
		}
	}

	return CDnSkill::Identity();
}

void CDnSkillTask::SendChangeSkillPage(int nSkillPage , bool bForce )
{
	if( (!bForce && m_iSkillTreePage == nSkillPage) || nSkillPage < DualSkill::Type::Primary || nSkillPage >= DualSkill::Type::MAX )
		return;

	m_bRequestWait = true;
	::SendChangeSkillPage(nSkillPage);
}

void CDnSkillTask::OnRecvSkillPageCount( SCSkillPageCount *pPacket )
{
	if(pPacket->cSkillPageCount > DualSkill::Type::MAX || pPacket->cSkillPageCount < DualSkill::Type::Primary )
		return;

	m_iSkillPageCount = pPacket->cSkillPageCount;

	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
	if( pSkillTreeDlg )
	{
		for(int i=0; i<pPacket->cSkillPageCount; i++)
			pSkillTreeDlg->EnableSkillPage(i);
	}
}

void CDnSkillTask::OnRecvChangeSkillPageRes(SCChangeSkillPage* pPacket)
{
	int nSkillPage = (int)pPacket->cSkillPage;
	m_bRequestWait = false;

	std::wstring szCaption;

	if( nSkillPage == DualSkill::Type::Primary )
		szCaption = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 742 ) , GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 737 ) ) ;
	else
		szCaption = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 742 ) , GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 738 ) ) ;

	GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2 , szCaption.c_str() , textcolor::YELLOW);

	if( CTaskManager::GetInstance().GetTask( "PVPLobbyTask" ) )
	{
		m_iNeedRefreshSkillPage = nSkillPage;
		return;
	}

	if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeEnum::MapTypeVillage )
		return;
	
	ApplySkillPage(nSkillPage);
}

void CDnSkillTask::ApplySkillPage(int nSkillPage )
{
	if( m_iSkillTreePage == nSkillPage || nSkillPage < DualSkill::Type::Primary || nSkillPage >= DualSkill::Type::MAX )
		return;

	if(!CDnActor::s_hLocalActor)
		return;

	m_iSkillTreePage = nSkillPage;

	CDnLocalPlayerActor* pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	if(pLocalPlayerActor)
		pLocalPlayerActor->RemoveSkillAll(); // 현재스킬을 모두 제거해준다.

	DnSkillHandle hSkill;
	int nSkillSize = (int)m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage].size();
	for(int i=0;i<nSkillSize;i++)
	{
		hSkill = m_vlhLocalPlayerOpenedSkillList[m_iSkillTreePage][i];
		
		if( hSkill )
		{
			int iSkillLevelDataType = QueryPVEOrPVP();
			hSkill->SetHasActor(CDnActor::s_hLocalActor);
			hSkill->SelectLevelDataType( iSkillLevelDataType );

			if ( pLocalPlayerActor && pLocalPlayerActor->CanAddSkill(hSkill->GetClassID()) )
			{
				pLocalPlayerActor->MASkillUser::AddSkill( hSkill );
			}
		}
	}

	if( pLocalPlayerActor && pLocalPlayerActor->IsSummonPet() )
	{
		if( pLocalPlayerActor->GetPetInfo().nSkillID1 > 0 )
			AddSkill( pLocalPlayerActor->GetPetInfo().nSkillID1, 1 );
		if( pLocalPlayerActor->GetPetInfo().nSkillID2 > 0 )
			AddSkill( pLocalPlayerActor->GetPetInfo().nSkillID2, 1 );
	}

	m_mapSkillTreeInfoByJobID.clear();  // 스킬 트리 정보 다시 꾸민다.
	InitializeSkillTree();              // 스킬트리
	GetInterface().ChangeSkillPage(m_iSkillTreePage);
	GetInterface().RefreshSkillDialog();
}


#if defined(PRE_FIX_NEXTSKILLINFO)
SKILL_LEVEL_INFO CDnSkillTask::ms_SkillLevelTableInfo;
SKILL_LEVEL_INFO* CDnSkillTask::GetSkillLevelTableIDList(int nSkillID, int nApplyType)
{
	if (nApplyType >= 2 || nApplyType < 0)
		return &ms_SkillLevelTableInfo;

	SKILL_LEVEL_TABLEID_LIST::iterator findIter = m_SkillLevelTableIDList[nApplyType].find(nSkillID);
	if (findIter != m_SkillLevelTableIDList[nApplyType].end())
		return &findIter->second;

	return &ms_SkillLevelTableInfo;
}

void CDnSkillTask::InitSkillLevelTableIDList()
{
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	if (pSkillTable == NULL || pSkillLevelTable == NULL)
		return;

	int nSkillCount = pSkillTable->GetItemCount();
	for (int i = 0; i < nSkillCount; ++i)
	{
		int nSkillID = pSkillTable->GetItemID(i);

		//int nSkillID = pSkillTable->GetFieldFromLablePtr(nItemID, "id")->GetInteger();

		std::vector<int> vlSkillLevelList;
		pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", nSkillID, vlSkillLevelList );

		// pve, pvp 대상인지 확인하여 걸러냄.
		vector<int>::iterator iterLevelList = vlSkillLevelList.begin();
		for( iterLevelList; iterLevelList != vlSkillLevelList.end(); ++iterLevelList)
		{
			int iSkillLevelTableID = *iterLevelList;
			int iApplyType = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_ApplyType" )->GetInteger();
			int iSkillLevel = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillLevel" )->GetInteger();

			AddSkillLevelTableID(nSkillID, iSkillLevel, iSkillLevelTableID, iApplyType);
		}
	}
}

void CDnSkillTask::AddSkillLevelTableID(int nSkillID, int nSkillLevel, int nSkillLevelTableID, int nApplyType)
{
	if (nApplyType >= 2 || nApplyType < 0)
		return;

	SKILL_LEVEL_TABLEID_LIST::iterator findIter = m_SkillLevelTableIDList[nApplyType].find(nSkillID);
	if (findIter != m_SkillLevelTableIDList[nApplyType].end())
	{
		//기존 리스트가 존재 하면 그 리스트에 추가 한다..
		findIter->second.AddSkillLevelTableID(nSkillLevel, nSkillLevelTableID);
	}
	else
	{
		SKILL_LEVEL_INFO skillLevelInfo;
		skillLevelInfo.AddSkillLevelTableID(nSkillLevel, nSkillLevelTableID);

		m_SkillLevelTableIDList[nApplyType].insert(std::make_pair(nSkillID, skillLevelInfo));
	}
}
#endif // PRE_FIX_NEXTSKILLINFO

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
bool CDnSkillTask::IsAvailableSPByJob(DnActorHandle hActor, vector<int>& needSPValues)
{
	//return m_SkillTreeSystem.IsAvailableSPByJob(hActor, needSPValues);
	if (!hActor)
		return false;

	CDnPlayerActor* pPlayerActor = NULL;

	if (hActor->IsPlayerActor())
		pPlayerActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());

	if (pPlayerActor == NULL)
		return false;

	bool isAvailableSPByJob = true;

	if (pPlayerActor)
	{
		std::vector<int> jobHistory;
		pPlayerActor->GetJobHistory(jobHistory);
		int nJobCount = (int)jobHistory.size();
		int nJobID = -1;

		for (int i = 0; i < nJobCount; ++i)
		{
			if (IsAvailableSPByJob(jobHistory, needSPValues, i) == false)
			{
				isAvailableSPByJob = false;
				break;
			}
		}
	}

	return isAvailableSPByJob;
}

bool CDnSkillTask::IsAvailableSPByJob(std::vector<int>& jobHistory, vector<int>& needSPValues, int nIndex)
{
	bool isAvailableSPByJob = false;

	int nJobCount = (int)jobHistory.size();

	int nNeedSPValue = 0;
	int nSPValueCount = (int)needSPValues.size();
	if (nIndex >= 0 && nIndex < nSPValueCount)
		nNeedSPValue = needSPValues[nIndex];
	
	//전직이 되지 않은 경우??
	if (nIndex >= nJobCount)
	{
		if (nNeedSPValue <= 0)
			return true;
		else
			return false;
	}

	int nJobID = -1;

	nJobID = jobHistory[nIndex];
	int iUsedSkillPointInThisJob = GetUsedSkillPointInThisJob( nJobID );

	//해당 JobID에 사용한 SP값이 확인용 값보다 커야 사용 가능..
	if (iUsedSkillPointInThisJob >= nNeedSPValue)
		isAvailableSPByJob = true;

	return isAvailableSPByJob;
}

void CDnSkillTask::GetNeedSPValuesByJob(int nSkillID, std::vector<int>& nNeedSPValues)
{
	m_SkillTreeSystem.GetNeedSPValuesByJob(nSkillID, nNeedSPValues);
}
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP


#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
bool CDnSkillTask::IsTotalLevelSkill(int nSkillID)
{
	DNTableFileFormat*  pTotalLevelSkillDB = GetTableDB().GetTable( CDnTableDB::TTOTALLEVELSKILL );

	if (pTotalLevelSkillDB == NULL)
		return false;

	vector<int> vlTotalLevelSkills;
	pTotalLevelSkillDB->GetItemIDListFromField( "_SkillTableID", nSkillID, vlTotalLevelSkills );

	if (vlTotalLevelSkills.empty())
		return false;
	else
		return true;
}

bool CDnSkillTask::IsAvailableTotalSkill(DnActorHandle hActor, CDnSkill* pSkill)
{
	CDnTotalLevelSkillSystem* pTotalLevelSkillSystem = NULL;
	if (CDnActor::s_hLocalActor)
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		pTotalLevelSkillSystem = pPlayerActor ? pPlayerActor->GetTotalLevelSkillSystem() : NULL;
	}

	if (pTotalLevelSkillSystem == NULL || pSkill == NULL)
		return false;

	return pTotalLevelSkillSystem->IsUsableSkill(pSkill->GetMySmartPtr());
}

CDnTotalLevelSkillSystem* CDnSkillTask::GetTotalLevelSkillSystem()
{
	CDnTotalLevelSkillSystem* pTotalLevelSkillSystem = NULL;
	if (CDnActor::s_hLocalActor)
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		pTotalLevelSkillSystem = pPlayerActor ? pPlayerActor->GetTotalLevelSkillSystem() : NULL;
	}

	return pTotalLevelSkillSystem;
}

void CDnSkillTask::AddTotalLevelSkill(DnSkillHandle hSkill)
{
	CDnTotalLevelSkillSystem* pTotalLevelSkillSystem = NULL;
	CDnPlayerActor* pPlayerActor = NULL;
	if (CDnActor::s_hLocalActor)
	{
		pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		pTotalLevelSkillSystem = pPlayerActor ? pPlayerActor->GetTotalLevelSkillSystem() : NULL;
	}

	if (pTotalLevelSkillSystem == NULL || !hSkill)
		return;

	int nSkillID = hSkill->GetClassID();
	CDnTotalLevelSkillSystem::_TotalLevelSkillInfo* pSkillInfo = pTotalLevelSkillSystem->GetTotalLevelSkillInfo(nSkillID);
	
	//빈슬롯 위치 확인..
	int nSlotIndex = pTotalLevelSkillSystem->FindEmptySlot(nSkillID);
	//int nSlotIndex = pTotalLevelSkillSystem->FindEmptySlot();
	
	if (nSlotIndex == -1)
	{
		//추가할 슬롯을 찾지 못했다는 메시지??
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption1, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1829 ), textcolor::YELLOW, 4.0f );
		return;
	}
	
	//빈 슬롯에 추가 가능한지 확인 한다..
	int nResult = pTotalLevelSkillSystem->CanAddSkill(nSlotIndex, hSkill);
	switch(nResult)
	{
	case CDnTotalLevelSkillSystem::TotalLevelSystem_Error_None:
		{
			if (pPlayerActor)
				pPlayerActor->RequestAddTotalLevelSkill(nSlotIndex, hSkill->GetClassID());
		}
		break;
	case CDnTotalLevelSkillSystem::TotalLevelSystem_Error_SameSkillType:
		{
			//이미 같은 타입의 통합레벨 스킬이 장착되어 있을경우..
			GetInterface().ShowCaptionDialog( CDnInterface::typeCaption1, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1830 ), textcolor::YELLOW, 4.0f );
		}
		break;
	}
	
}

void CDnSkillTask::RequestAddTotalLevelSkill(int nSlotIndex, int nSkillID)
{
	CDnPlayerActor* pPlayerActor = NULL;
	if (CDnActor::s_hLocalActor)
		pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	
	if (pPlayerActor)
		pPlayerActor->RequestAddTotalLevelSkill(nSlotIndex, nSkillID);
}

void CDnSkillTask::RequestRemoveTotalLevelSkill(int nSlotIndex)
{
	CDnPlayerActor* pPlayerActor = NULL;
	if (CDnActor::s_hLocalActor)
		pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	if (pPlayerActor)
		pPlayerActor->RequestRemoveTotalLevelSkill(nSlotIndex);
}

int CDnSkillTask::GetTotalLevel()
{
	int nTotalLevel = 0;

	CDnPlayerActor* pPlayerActor = NULL;
	if (CDnActor::s_hLocalActor)
		pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	if (pPlayerActor)
	{
		CDnTotalLevelSkillSystem* pTotalLevelSystem = pPlayerActor->GetTotalLevelSkillSystem();

		if (pTotalLevelSystem)
			nTotalLevel = pTotalLevelSystem->GetTotalLevel();
	}

	return nTotalLevel;
}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )
void CDnSkillTask::OnRecvReservationSkillList( const SCReservationSkillListAck * pData )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor )
		return;

	for( int itr = 0; itr < pData->nCount; ++itr )
	{
		if( ERROR_NONE != pData->tReservationSkillAck[itr].nResult )
		{
			ErrorMessage_ReservationSkill( pData->tReservationSkillAck[itr].nResult );
			break;
		}

		const TReservationSkillReq & sData = pData->tReservationSkillAck[itr].tReservationSkill;

		if( ReservationSKillList::Type::UnLock == pData->tReservationSkillAck[itr].tReservationSkill.cType )
		{
			CDnSkillTreeSystem::S_OUTPUT Output;
			CDnSkillTreeSystem::S_TRY_UNLOCK TryUnlockInfo;
			TryUnlockInfo.iTryUnlockSkillID = sData.nSkillID;
			TryUnlockInfo.iCurrentCharLevel = sData.nLevel;

			m_SkillTreeSystem.TryUnLockSkill( TryUnlockInfo, &Output );

			if( CDnSkillTreeSystem::R_SUCCESS != Output.eResult )
				continue;

			Unlock_ReservationSkill( sData.nSkillID );
			Acquire_ReservationSkill( sData.nSkillID, false );
		}
		else if( ReservationSKillList::Type::Acquire == pData->tReservationSkillAck[itr].tReservationSkill.cType )
			Acquire_ReservationSkill( sData.nSkillID, false );
		else if( ReservationSKillList::Type::LevelUp == pData->tReservationSkillAck[itr].tReservationSkill.cType )
		{
			const int nLevelUp = hLocalActor->GetSkillLevelUpValue( sData.nSkillID );
			DnSkillHandle hSkill = FindSkill( sData.nSkillID );

			if( CDnSkill::Identity() == hSkill )
			{
				hSkill = GetReservationSkill( sData.nSkillID );
				if( CDnSkill::Identity() == hSkill )
					continue;
			}

			for( int itr = hSkill->GetLevel() + 1 - nLevelUp; itr <= sData.nLevel; ++itr )
				LevelUpSkill( sData.nSkillID, sData.nLevel, false );
		}
	}

	GetInterface().RefreshSkillDialog();
}

void CDnSkillTask::SendReservationSkillList()
{
	::SendReservationSkillList( m_vecReservationSkillData );

	int nNum = 0;
	std::map< int, DnSkillHandle >::iterator Itor = m_mapReservationGuildSkillHandle.begin();
	for( ; Itor != m_mapReservationGuildSkillHandle.end(); ++Itor )
	{
		SendAcquireGuildWarSkillReq( Itor->second->GetClassID(), Itor->second->GetLevel(), nNum );
		++nNum;
	}
}

int CDnSkillTask::GetUsedReservationSkillPointInThisJob( const int iJobID )
{
	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor )
		return 0;

	int iWholeUsedSPInThisJob = 0;

	for( DWORD itr = 0; itr < m_vecReservationSkillData.size(); ++itr )
	{
		if( ReservationSKillList::Type::LevelUp == m_vecReservationSkillData[itr].cType )
			continue;

		DnSkillHandle hSkill = GetReservationSkill( m_vecReservationSkillData[itr].nSkillID );

		if( !hSkill )
			continue;

		if( hSkill->IsAcquired()
			&& hSkill->GetNeedJobClassID() == iJobID )
		{
			int nItemLevel = hLocalActor->GetSkillLevelUpValue( hSkill->GetClassID() );

			int iSkillLevel = hSkill->GetLevel() - nItemLevel;
			iSkillLevel -= hSkill->GetLevelUpValue();
			iWholeUsedSPInThisJob += _GetAccumlatedNeedSkillPoint( hSkill, iSkillLevel );
		}
	}

	return iWholeUsedSPInThisJob;	
}

void CDnSkillTask::GetPossessReservationSkill( vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> & vlPossessedSkillInfos )
{
	for( DWORD itr = 0; itr < m_vecReservationSkillData.size(); ++itr )
	{
		if( ReservationSKillList::Type::LevelUp == m_vecReservationSkillData[itr].cType )
			continue;

		CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;

		DnSkillHandle hSkill = GetReservationSkill( m_vecReservationSkillData[itr].nSkillID );

		if( !hSkill )
			continue;

		SkillInfo.iSkillID = hSkill->GetClassID();
		SkillInfo.iSkillLevel = hSkill->GetLevel();

		//#37528 선행 스킬 레벨업 조건 확인때 스킬레벨업에 의한 증가 수치는 무시 되도록
		//스킬레벨업 수치 만큼 차감한다.
		SkillInfo.iSkillLevel -= hSkill->GetLevelUpValue();

		SkillInfo.bCurrentLock = false;

		vlPossessedSkillInfos.push_back( SkillInfo );
	}
}

bool CDnSkillTask::IsJobSP_SkillReservation( const int nJobID )
{
	std::map< int, DnSkillHandle >::iterator Itor = m_mapReservationSkillHandle.begin();
	for( ; Itor != m_mapReservationSkillHandle.end(); ++Itor )
	{
		DnSkillHandle hSkill = Itor->second;
		if( hSkill && hSkill->GetNeedJobClassID() == nJobID )
			return true;
	}
	return false;
}

bool CDnSkillTask::IsClear_SkillLevelUp()
{
	return m_vecReservationSkillData.empty() && m_mapReservationSkillHandle.empty() && m_mapReservationGuildSkillHandle.empty();
}

void CDnSkillTask::Clear_SkillLevelUp()
{
	m_vecReservationSkillData.clear();

	std::map< int, DnSkillHandle >::iterator Itor = m_mapReservationSkillHandle.begin();
	for( ; Itor != m_mapReservationSkillHandle.end(); ++Itor )
		SAFE_RELEASE_SPTR( Itor->second );
	m_mapReservationSkillHandle.clear();

	Itor = m_mapReservationGuildSkillHandle.begin();
	for( ; Itor != m_mapReservationGuildSkillHandle.end(); ++Itor )
		SAFE_RELEASE_SPTR( Itor->second );
	m_mapReservationGuildSkillHandle.clear();

	m_nReservationSkillNeedSP = 0;
	m_nReservationSkillNeedMoney = 0;
}

int CDnSkillTask::GetReservationSkillNeedSP()
{
	return m_nReservationSkillNeedSP;
}

INT64 CDnSkillTask::GetReservationSkillNeedMoney()
{
	return m_nReservationSkillNeedMoney;
}

DnSkillHandle CDnSkillTask::GetReservationSkill( const int nSkillID )
{
	if( m_mapReservationSkillHandle.empty() == false )
	{
		std::map< int, DnSkillHandle >::iterator Itor = m_mapReservationSkillHandle.find( nSkillID );
		if( m_mapReservationSkillHandle.end() != Itor )
			return Itor->second;
	}

	return CDnSkill::Identity();
}

void CDnSkillTask::GetReservationSkillLevelMax( SReservationSkillMax & sData )
{
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor * pLocalActor = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer());
	if( !pLocalActor )
		return;
	
	DnSkillHandle hSkill = GetReservationSkill( sData.m_nSkillID );
	if( !hSkill )
		hSkill = FindSkill( sData.m_nSkillID );

	bool bNeedAcquire = false;
	bool bNeedUnlock = false;
	if( !hSkill )
	{
		bNeedAcquire = true;
		hSkill = FindZeroLevelSkill( sData.m_nSkillID );

		if( !hSkill )
		{
			bNeedUnlock = true;
			hSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, sData.m_nSkillID, 1 );
		}
	}

	const int nJobID = hSkill->GetNeedJobClassID();
	const int nJobIndex = pLocalActor->GetJobArrayIndex( nJobID );

	const int nJobRemainSkillPoint = pLocalActor->GetSPUsingLimitByJobArrayIndex( nJobIndex ) - GetUsedSkillPointInThisJob( nJobID );
	const int nTotalRemainSkillPoint = GetSkillPoint() - GetReservationSkillNeedSP();
	const int nPlayerLevel = pLocalActor->GetLevel();

	int nTotalNeedSkillPoint = 0;
	if( true == bNeedAcquire )
	{
		const int nNeedAcquireSkillPoint = hSkill->GetNeedAcquireSkillPoint();

		if( nJobRemainSkillPoint < nNeedAcquireSkillPoint 
			|| nTotalRemainSkillPoint < nNeedAcquireSkillPoint 
			|| nPlayerLevel < hSkill->GetLevelLimit() )
		{
			if( bNeedUnlock )
				SAFE_RELEASE_SPTR( hSkill );
			
			return;
		}

		nTotalNeedSkillPoint += nNeedAcquireSkillPoint;
	}

	DNTableFileFormat * pSkillLevelTable = GetTableDB().GetTable( CDnTableDB::TSKILLLEVEL );
	SKILL_LEVEL_INFO * pLevelTableInfo = GetSkillLevelTableIDList( hSkill->GetClassID(), hSkill->GetSelectedLevelDataType());
	SKILL_LEVEL_TABLE_IDS::const_iterator findIter = pLevelTableInfo->_SkillLevelTableIDs.find( hSkill->GetLevel() );

	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	int nSkillMaxLevel = pSkillTable->GetFieldFromLablePtr( hSkill->GetClassID(), "_MaxLevel" )->GetInteger();

	const int nItemLevelUp = pLocalActor->GetSkillLevelUpValue( sData.m_nSkillID );
	int nNowSkillLevel = hSkill->GetLevel();

	if( true == bNeedAcquire )
		nNowSkillLevel += nItemLevelUp;

#if defined( PRE_MOD_68531_NEW )
	nNowSkillLevel -= nItemLevelUp;
#endif	// #if defined( PRE_MOD_68531_NEW )

	const int nNeedLevelUpSkillPoint = hSkill->GetNextLevelSkillPoint();
	int nLevelUpCount = 0, nNextSkillLevelTableID = -1;
	bool bEnable = false;
	while( nSkillMaxLevel >= nNowSkillLevel + nLevelUpCount )
	{
		if( nLevelUpCount > 100 )
			break;

		findIter = pLevelTableInfo->_SkillLevelTableIDs.find( nNowSkillLevel + nLevelUpCount );
		if( findIter != pLevelTableInfo->_SkillLevelTableIDs.end() )
			nNextSkillLevelTableID = findIter->second;

		const int nLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( nNextSkillLevelTableID, "_LevelLimit" )->GetInteger();
		const int nNeedSkillPoint = nTotalNeedSkillPoint + (nLevelUpCount * nNeedLevelUpSkillPoint);

		if( nJobRemainSkillPoint < nNeedSkillPoint 
			|| nTotalRemainSkillPoint < nNeedSkillPoint
			|| nPlayerLevel < nLevelLimit )
			break;

		++nLevelUpCount;
	}

	if( 0 != nLevelUpCount || true == bNeedAcquire )
		--nLevelUpCount;

	if( true == bNeedUnlock )
		sData.m_nNeedSkillMax = eNeed_SkillMax_Unlock;
	else if( true == bNeedAcquire )
		sData.m_nNeedSkillMax = eNeed_SkillMax_Acquire;

	if( 0 != nLevelUpCount )
		sData.m_nNeedSkillMax += eNeed_SkillMax_LevelUp;

	sData.m_nLevelUpCount = nLevelUpCount;

#if defined( PRE_MOD_68531_NEW )
	sData.m_nNowSkillLevel = nNowSkillLevel;
#else
	sData.m_nNowSkillLevel = nNowSkillLevel - nItemLevelUp;
#endif	// #if defined( PRE_MOD_68531_NEW )

	if( bNeedUnlock )
		SAFE_RELEASE_SPTR( hSkill );
}

void CDnSkillTask::Change_SkillLevelUpMax( const int nSkillID )
{
	SReservationSkillMax sData;
	sData.m_nSkillID = nSkillID;

	GetReservationSkillLevelMax( sData );

	if( sData.m_nNeedSkillMax & eNeed_SkillMax_Unlock )
		Change_SkillLevelUp( nSkillID, 1, ReservationSKillList::Type::UnLock );
	else if( sData.m_nNeedSkillMax & eNeed_SkillMax_Acquire )
		Change_SkillLevelUp( nSkillID, 1, ReservationSKillList::Type::Acquire );

	if( sData.m_nNeedSkillMax & eNeed_SkillMax_LevelUp )
	{
		for( int itr = 1; itr < sData.m_nLevelUpCount; ++itr )
			Change_SkillLevelUp( nSkillID, sData.m_nNowSkillLevel + itr, ReservationSKillList::Type::LevelUp );

		Change_SkillLevelUp( nSkillID, sData.m_nNowSkillLevel + sData.m_nLevelUpCount, ReservationSKillList::Type::LevelUp );
	}

	SetReservationSkill_Item( nSkillID );
}

void CDnSkillTask::Change_SkillLevelUp( const int nSkillID, const int nSkillLevel, const char cState )
{
	TReservationSkillReq sReservationSkillData;
	sReservationSkillData.nSkillID = nSkillID;
	sReservationSkillData.cType = cState;

	DnSkillHandle hLevelUpSkill;
	if( ReservationSKillList::Type::UnLock == cState
		|| ReservationSKillList::Type::Acquire == cState )
	{
		sReservationSkillData.nLevel = nSkillLevel;
		hLevelUpSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, nSkillID, sReservationSkillData.nLevel );
		m_nReservationSkillNeedSP += hLevelUpSkill->GetNeedAcquireSkillPoint();

		if( ReservationSKillList::Type::UnLock == cState )
			m_nReservationSkillNeedMoney += hLevelUpSkill->GetUnlockPrice();
	}
	else if( ReservationSKillList::Type::LevelUp == cState )
	{
		sReservationSkillData.nLevel = nSkillLevel;
		hLevelUpSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, nSkillID, sReservationSkillData.nLevel );
		if( CDnSkill::Identity() == hLevelUpSkill )
			return;

		DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
		const int nLevelTableID = hLevelUpSkill->GetSkillLevelID();
		const int nNextLevelUpSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( nLevelTableID, "_NeedSkillPoint" )->GetInteger();

		if( hLevelUpSkill )
			m_nReservationSkillNeedSP += nNextLevelUpSkillPoint;
	}

	if( m_mapReservationSkillHandle.empty() == false )
	{
		std::map< int, DnSkillHandle>::iterator Handle_Itor = m_mapReservationSkillHandle.find( nSkillID );
		if( m_mapReservationSkillHandle.end() != Handle_Itor )
		{
			SAFE_RELEASE_SPTR( Handle_Itor->second );
			m_mapReservationSkillHandle.erase( Handle_Itor );
		}
	}

	m_vecReservationSkillData.push_back( sReservationSkillData );

	m_mapReservationSkillHandle.insert( std::make_pair( nSkillID, hLevelUpSkill ) );
}

bool CDnSkillTask::IsGlobalSkillGroupDown( DnSkillHandle hSkill )
{
	bool bParentSkill = false;

	std::vector< std::pair<int, int> > vecParentSkillData;
	m_SkillTreeSystem.GetParentSkillData( hSkill->GetClassID(), vecParentSkillData );

	std::vector< std::pair<int, int> >::iterator parent_Itor = vecParentSkillData.begin();
	for( ; parent_Itor != vecParentSkillData.end(); ++parent_Itor )
	{
		bParentSkill = false;
		std::vector< TReservationSkillReq >::iterator reserve_Itor = m_vecReservationSkillData.begin();
		for( ; reserve_Itor != m_vecReservationSkillData.end(); ++reserve_Itor )
		{
			if( parent_Itor->first != reserve_Itor->nSkillID )
				continue;

			std::map< int, DnSkillHandle >::iterator skill_Itor = m_mapReservationSkillHandle.find( reserve_Itor->nSkillID );
			if( skill_Itor != m_mapReservationSkillHandle.end() && skill_Itor->second->GetLevel() >= parent_Itor->second )
				bParentSkill = true;
		}

		if( false == bParentSkill )
			break;
	}

	return bParentSkill;
}

int CDnSkillTask::IsReservationSkillDown( const int nSkillID )
{
	if( m_mapReservationSkillHandle.empty() == true )
		return eSkillMinus_Delete;

	std::map< int, DnSkillHandle>::iterator Handle_Itor = m_mapReservationSkillHandle.find( nSkillID );
	if( m_mapReservationSkillHandle.end() == Handle_Itor )
		return eSkillMinus_Delete;

	DnSkillHandle hEraseSkill = Handle_Itor->second;

	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	int nMaxLevel = pSkillTable->GetFieldFromLablePtr( hEraseSkill->GetClassID(), "_MaxLevel" )->GetInteger();

	int nNeedSkillPoint = 0;
	if( hEraseSkill->GetLevel() == 1 )
		nNeedSkillPoint = hEraseSkill->GetNeedAcquireSkillPoint();
	else if( hEraseSkill->GetLevel() == nMaxLevel )
	{
		DNTableFileFormat * pSkillLevelTable = GetTableDB().GetTable( CDnTableDB::TSKILLLEVEL );
		SKILL_LEVEL_INFO * pLevelTableInfo = GetSkillLevelTableIDList( hEraseSkill->GetClassID(), hEraseSkill->GetSelectedLevelDataType());
		SKILL_LEVEL_TABLE_IDS::const_iterator findIter = pLevelTableInfo->_SkillLevelTableIDs.find( hEraseSkill->GetLevel() );

		findIter = pLevelTableInfo->_SkillLevelTableIDs.find( hEraseSkill->GetLevel() - 1 );
		if( findIter != pLevelTableInfo->_SkillLevelTableIDs.end() )
		{
			int nNextSkillLevelTableID = findIter->second;

			nNeedSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( nNextSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
		}
	}
	else
		nNeedSkillPoint = hEraseSkill->GetNextLevelSkillPoint();

	if( !CDnActor::s_hLocalActor )
		return eSkillMinus_Disable;
	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( NULL == pPlayerActor )
		return eSkillMinus_Disable;

	vector<int> vlJobHistory;
	vector<int> vecJobUsedSkillPoint;
	pPlayerActor->GetJobHistory( vlJobHistory );
	for( DWORD itr = 0; itr < vlJobHistory.size(); ++itr )
	{
		const int nJobID = vlJobHistory[itr];
		int nUsedSkillPoint = GetUsedSkillPointInThisJob( nJobID );

		if( hEraseSkill->GetNeedJobClassID() == nJobID )
			nUsedSkillPoint -= nNeedSkillPoint;

		vecJobUsedSkillPoint.push_back( nUsedSkillPoint );
	}

	std::map< int, DnSkillHandle >::iterator Itor = m_mapReservationSkillHandle.begin();
	for( ; Itor != m_mapReservationSkillHandle.end(); ++Itor )
	{
		DnSkillHandle hSkill = Itor->second;
		if( !hSkill || hSkill->GetClassID() == nSkillID )
			continue;

		if( 0 < hSkill->GetGlobalSkillGroupID() 
			&& hSkill->GetGlobalSkillGroupID() == hEraseSkill->GetGlobalSkillGroupID() 
			&& hSkill->GetClassID() != hEraseSkill->GetClassID() )
		{
			if( true == IsGlobalSkillGroupDown( hEraseSkill ) && false == IsGlobalSkillGroupDown( hSkill ) && hEraseSkill->GetLevel() == 1 )
				return eSkillMinus_Disable;
		}

		std::vector<int> nNeedSPValues;
		m_SkillTreeSystem.GetNeedSPValuesByJob( hSkill->GetClassID(), nNeedSPValues );

		for( DWORD itr = 0 ; itr < nNeedSPValues.size(); ++itr )
		{
			if( vecJobUsedSkillPoint.size() <= itr )
				break;

			if( vecJobUsedSkillPoint[itr] < nNeedSPValues[itr] )
				return eSkillMinus_Disable;
		}
	}

	for( DWORD itr = 0; itr < m_vecReservationSkillData.size(); ++itr )
	{
		if( ReservationSKillList::Type::UnLock == m_vecReservationSkillData[itr].cType 
			|| ReservationSKillList::Type::Acquire == m_vecReservationSkillData[itr].cType )
		{
			const int nPrentLevel = m_SkillTreeSystem.IsParentSkill( nSkillID, m_vecReservationSkillData[itr].nSkillID );
			const int nLevel = hEraseSkill->GetLevel() - pPlayerActor->GetSkillLevelUpValue( nSkillID );

			if( 0 != nPrentLevel && nLevel <= nPrentLevel )
				return eSkillMinus_Disable;
		}
	}

	return eSkillMinus_Enable;
}

void CDnSkillTask::Change_SkillLevelDownMax( const int nSkillID )
{
	for( DWORD itr = 0; itr < 100; ++itr )
	{
		if( eSkillMinus_Enable != IsReservationSkillDown( nSkillID ) )
			return;

		DnSkillHandle hSkill = GetReservationSkill(  nSkillID );
		if( !hSkill )
			break;

		Change_SkillLevelDown( nSkillID, hSkill->GetLevel() );
	}

	SetReservationSkill_Item( nSkillID );
}

void CDnSkillTask::Change_SkillLevelDown( const int nSkillID, const int nSkillLevel )
{
	if( m_mapReservationSkillHandle.empty() == true )
		return;

	std::map< int, DnSkillHandle>::iterator Handle_Itor = m_mapReservationSkillHandle.find( nSkillID );
	if( m_mapReservationSkillHandle.end() == Handle_Itor )
		return;

	DnSkillHandle hReservationSkill = Handle_Itor->second;

	std::vector< TReservationSkillReq >::reverse_iterator Itor = m_vecReservationSkillData.rbegin();
	for( ; Itor != m_vecReservationSkillData.rend(); ++Itor )
	{
		if( nSkillID != Itor->nSkillID )
			continue;
		
		const int nNeedAcquireSkillPoint = hReservationSkill->GetNeedAcquireSkillPoint();
		const int nUnlockPrice = hReservationSkill->GetUnlockPrice();
		const int nReservationSkillLevel = hReservationSkill->GetLevel();

		DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
		const int nLevelTableID = hReservationSkill->GetSkillLevelID();
		const int nNextLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( nLevelTableID, "_NeedSkillPoint" )->GetInteger();
		
		SAFE_RELEASE_SPTR( Handle_Itor->second );
		m_mapReservationSkillHandle.erase( Handle_Itor );

		if( ReservationSKillList::Type::UnLock == Itor->cType
			|| ReservationSKillList::Type::Acquire == Itor->cType )
		{
			m_nReservationSkillNeedSP -= nNeedAcquireSkillPoint;

			if( ReservationSKillList::Type::UnLock == Itor->cType )
				m_nReservationSkillNeedMoney -= nUnlockPrice;
		}
		else if( ReservationSKillList::Type::LevelUp == Itor->cType )
		{
			m_nReservationSkillNeedSP -= nNextLevelSkillPoint;

			DnSkillHandle hLearnSkill = FindSkill( nSkillID );

			if( !hLearnSkill || hLearnSkill->GetLevel() != nReservationSkillLevel - 1 )
			{
				DnSkillHandle hLevelDownSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, nSkillID, nReservationSkillLevel - 1 );
				m_mapReservationSkillHandle.insert( std::make_pair( nSkillID, hLevelDownSkill ) );
			}
		}

		m_vecReservationSkillData.erase( (++Itor).base() );

		return;
	}
}

void CDnSkillTask::SetReservationSkill_Item( const int nSkillID )
{
	if( m_mapReservationSkillHandle.empty() == true )
		return;

	std::map< int, DnSkillHandle>::iterator Handle_Itor = m_mapReservationSkillHandle.find( nSkillID );
	if( Handle_Itor == m_mapReservationSkillHandle.end() )
		return;

	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor )
		return;

	const int nLevelUp = hLocalActor->GetSkillLevelUpValue( nSkillID );
	if( 0 == nLevelUp )
		return;

	int nLevel = 0;
	std::vector< TReservationSkillReq >::reverse_iterator Itor = m_vecReservationSkillData.rbegin();
	for( ; Itor != m_vecReservationSkillData.rend(); ++Itor )
	{
		if( Itor->nSkillID == nSkillID )
		{
			nLevel = Itor->nLevel;
			break;
		}
	}

	if( 0 == nLevel )
		return;

	DnSkillHandle hLevelUpSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, nSkillID, nLevel + nLevelUp );
	if( !hLevelUpSkill )
		return;

	SAFE_RELEASE_SPTR( Handle_Itor->second );
	m_mapReservationSkillHandle.erase( Handle_Itor );

	
	m_mapReservationSkillHandle.insert( std::make_pair( nSkillID, hLevelUpSkill ) );
}

DnSkillHandle CDnSkillTask::GetReservationGuildSkill( const int nSkillID )
{
	std::map< int, DnSkillHandle >::iterator Itor = m_mapReservationGuildSkillHandle.find( nSkillID );
	if( m_mapReservationGuildSkillHandle.end() != Itor )
		return Itor->second;

	return CDnSkill::Identity();
}

void CDnSkillTask::Change_GuildSkillLevelUp( const int nSkillID )
{
	std::map< int, DnSkillHandle >::iterator Itor = m_mapReservationGuildSkillHandle.find( nSkillID );
	if( m_mapReservationGuildSkillHandle.end() != Itor )
	{
		SAFE_RELEASE_SPTR( Itor->second );
		m_mapReservationGuildSkillHandle.erase( Itor );
	}

	DnSkillHandle hSkill = CDnGuildWarSkill::CreateSkill( CDnActor::s_hLocalActor, nSkillID, 1 );
	if( CDnSkill::Identity() == hSkill )
		return;

	m_mapReservationGuildSkillHandle.insert( make_pair< const int&, DnSkillHandle& >( nSkillID, hSkill ) );
}

void CDnSkillTask::Change_GuildSkillLevelDown( const int nSkillID )
{
	std::map< int, DnSkillHandle >::iterator Itor = m_mapReservationGuildSkillHandle.find( nSkillID );
	if( m_mapReservationGuildSkillHandle.end() != Itor )
	{
		SAFE_RELEASE_SPTR( Itor->second );
		m_mapReservationGuildSkillHandle.erase( Itor );
	}
}

bool CDnSkillTask::IsReservationGuildSkillDown( const int nSkillID )
{
	if( CDnSkill::Identity() == GetReservationGuildSkill( nSkillID ) )
		return false;

	return true;
}

int CDnSkillTask::GetReservationGuildSkillCount()
{
	return (int)m_mapReservationGuildSkillHandle.size();
}
#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

#if defined( PRE_ADD_PRESET_SKILLTREE )
void CDnSkillTask::SendPresetSkillTreeList()
{
	::SendSkillSetListReq();
}

void CDnSkillTask::SendPresetSkillTreeSave( std::wstring wszName )
{
	m_SavePresetSkillTree.Clear();

	m_SavePresetSkillTree.m_cIndex = GetEmptyEnablePresetIndex();
	if( -1 == m_SavePresetSkillTree.m_cIndex )
		return;

	m_SavePresetSkillTree.m_wszTreeName = wszName;

	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( NULL == pPlayerActor )	return;

	vector<int> vlJobHistory;
	pPlayerActor->GetJobHistory( vlJobHistory );

	for( DWORD itr = 0; itr < vlJobHistory.size(); ++itr )
	{
		int nJobID = vlJobHistory[itr];

		int nSlot = 0;

		// 트리구조가 역으로 될 경우에는 예외처리를 합니다.
		// 안그러면 모든 스킬트리를 뒤져야 하는데 너무 비효율적입니다.
		if( 41 == nJobID )	// 가디언
		{
			SavePresetSkill( nJobID, 1 );
			SavePresetSkill( nJobID, 0 );
			nSlot = 2;
		}

		for( ; nSlot < MAX_SKILL_SLOT_COUNT; ++nSlot )
			SavePresetSkill( nJobID, nSlot );
	}

	SendSkillSetSave( m_SavePresetSkillTree.m_cIndex, m_SavePresetSkillTree.m_wszTreeName.c_str(), m_SavePresetSkillTree.m_Data );
}

void CDnSkillTask::SendPresetSkillTreeDel( const BYTE cIndex )
{
	m_nPresetSkillDeleteIndex = cIndex;

	SendSKillSetDelete(cIndex);
}

void CDnSkillTask::SavePresetSkill( const int nJobID, const int nSlot )
{
	S_SKILLTREE_SLOT_INFO & sSlotInfo = m_mapSkillTreeInfoByJobID[ nJobID ].aSkillTreeSlotInfo[ nSlot ];

	if( CDnSkill::Identity() == sSlotInfo.hSkill )
		return;

	int nSkillID = sSlotInfo.hSkill->GetClassID();
	BYTE cLevel = sSlotInfo.hSkill->GetLevel();

	DnSkillHandle hReservationSkill = GetReservationSkill( nSkillID );
	if( ( true == sSlotInfo.hSkill->IsLocked() || false == sSlotInfo.hSkill->IsAcquired() )
		&& CDnSkill::Identity() == hReservationSkill )
		return;

	if( hReservationSkill )
		cLevel = hReservationSkill->GetLevel();

	DnActorHandle hLocalActor = CDnActor::s_hLocalActor;
	if( !hLocalActor )
		return;

	const int nLevelUp = hLocalActor->GetSkillLevelUpValue( nSkillID );

	cLevel -= nLevelUp;

	if( 0 >= cLevel )
		return;

	m_SavePresetSkillTree.m_Data.push_back( std::make_pair( nSkillID, cLevel ) );
}

bool CDnSkillTask::IsMySkillTreeSkill( const vector<int> & vlJobHistory, const int nSkillID )
{
	for( DWORD itr = 0; itr < vlJobHistory.size(); ++itr )
	{
		int nJobID = vlJobHistory[itr];

		for( int nSlot = 0; nSlot < MAX_SKILL_SLOT_COUNT; ++nSlot )
		{
			S_SKILLTREE_SLOT_INFO & sSlotInfo = m_mapSkillTreeInfoByJobID[ nJobID ].aSkillTreeSlotInfo[ nSlot ];
			if( CDnSkill::Identity() == sSlotInfo.hSkill )
				continue;

			if( sSlotInfo.hSkill->GetClassID() == nSkillID )
				return true;
		}
	}

	return false;
}

void CDnSkillTask::AdviceSkillTreeSort( std::vector< std::pair<int, BYTE> > & inData, std::vector< std::pair<int, BYTE> > & outData  )
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( NULL == pPlayerActor )	return;

	vector<int> vlJobHistory;
	pPlayerActor->GetJobHistory( vlJobHistory );

	for( DWORD itr = 0; itr < vlJobHistory.size(); ++itr )
	{
		int nJobID = vlJobHistory[itr];

		int nSlot = 0;

		// 트리구조가 역으로 될 경우에는 예외처리를 합니다.
		// 안그러면 모든 스킬트리를 뒤져야 하는데 너무 비효율적입니다.
		if( 41 == nJobID )	// 가디언
		{
			AdviceSkillTreePush( inData, outData, nJobID, 1 );
			AdviceSkillTreePush( inData, outData, nJobID, 0 );
			nSlot = 2;
		}

		for( ; nSlot < MAX_SKILL_SLOT_COUNT; ++nSlot )
			AdviceSkillTreePush( inData, outData, nJobID, nSlot );
	}
}

void CDnSkillTask::AdviceSkillTreePush( std::vector< std::pair<int, BYTE> > & inData, std::vector< std::pair<int, BYTE> > & outData, const int nJobID, const int nSlot )
{
	S_SKILLTREE_SLOT_INFO & sSlotInfo = m_mapSkillTreeInfoByJobID[ nJobID ].aSkillTreeSlotInfo[ nSlot ];

	if( CDnSkill::Identity() == sSlotInfo.hSkill )
		return;

	int nSkillID = sSlotInfo.hSkill->GetClassID();

	for( DWORD itr = 0; itr < inData.size(); ++itr )
	{
		if( nSkillID == inData[itr].first )
		{
			outData.push_back( inData[itr] );
			break;
		}
	}
}

void CDnSkillTask::InitAdviceSkillTreeTable()
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( NULL == pPlayerActor )	return;

	vector<int> vlJobHistory;
	pPlayerActor->GetJobHistory( vlJobHistory );

	DNTableFileFormat * pSkillTreeTable = GetDNTable( CDnTableDB::TSKILLTREE );
	DNTableFileFormat * pSkillAdviceTable = GetDNTable( CDnTableDB::TSKILLTREE_ADVICE );

	if( NULL == pSkillTreeTable || NULL == pSkillAdviceTable )
		return;

	const int nAdviceTableCount = pSkillAdviceTable->GetItemCount();

	for( int itr = 0; itr < nAdviceTableCount; ++itr )
	{
		int nItemID = pSkillAdviceTable->GetItemID( itr );

		SPresetSkillTree sData;

		int nAdviceIndex = pSkillAdviceTable->GetFieldFromLablePtr(nItemID, "_AdviceSkillIndex")->GetInteger();
		int nStringID = pSkillAdviceTable->GetFieldFromLablePtr(nItemID, "_NameID")->GetInteger();

		sData.m_nLevelLimit = pSkillAdviceTable->GetFieldFromLablePtr(nItemID, "_SkillTreeLevel")->GetInteger();
		sData.m_wszTreeName = std::wstring( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nStringID) );
		sData.m_bAdvice = true;

		std::vector< std::pair<int, BYTE> > vecData;
		m_SkillTreeSystem.GetAdviceSkillTree( nAdviceIndex - 1, vlJobHistory, vecData );

		AdviceSkillTreeSort( vecData, sData.m_Data );

		m_vecPresetSkillTree.push_back( sData );
	}
}

void CDnSkillTask::SetPresetSkillTree()
{
#if !defined(_JP)
	InitAdviceSkillTreeTable();
#endif

	for( DWORD itr = 0; itr < m_vecPresetSkillTree.size(); ++itr )
	{
		if( true == m_vecPresetSkillTree[itr].m_bAdvice )
			m_vecPresetSkillTree[itr].m_cIndex = (BYTE)(itr + E_ADVICE_INDEX_START);
	}

	CDnSkillTreeDlg* pSkillDlg  = (CDnSkillTreeDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG );
	if( pSkillDlg )
		pSkillDlg->UpdateView();
}

std::vector< CDnSkillTask::SPresetSkillTree > & CDnSkillTask::GetPresetSkillTree()
{
	return m_vecPresetSkillTree;
}

bool CDnSkillTask::IsPresetSkillTreeSave()
{
	const int nUsedSkillPoint = GetWholeUsedSkillPoint() + GetReservationSkillNeedSP();

	if( 0 == nUsedSkillPoint )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 743) );	// UISTRING : 사용한 스킬 포인트가 없습니다.
		return false;
	}

	if( -1 == GetEmptyEnablePresetIndex() )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 744) );	// UISTRING : 저장 공간이 부족합니다.
		return false;
	}

	return true;
}

int CDnSkillTask::GetEmptyEnablePresetIndex()
{
	for( DWORD itr = 0; itr < m_vecPresetSkillTree.size(); ++itr )
	{
		if( m_vecPresetSkillTree[itr].m_bAdvice == false && m_vecPresetSkillTree[itr].m_Data.empty() )
			return m_vecPresetSkillTree[itr].m_cIndex;
	}

	return -1;
}

void CDnSkillTask::PresetSkillLevelUp( const int nSkillID, const BYTE cLevel )
{
	for( int itr = 1; itr <= cLevel; ++itr )
	{
		DnSkillHandle hSkill = FindSkill( nSkillID );

		if( CDnSkill::Identity() != hSkill && hSkill->GetLevel() == cLevel && false == hSkill->IsLocked() )
			break;

		if( 1 == itr )
		{
			if( CDnSkill::Identity() == hSkill )
				hSkill = FindZeroLevelSkill( nSkillID );

			if( CDnSkill::Identity() == hSkill || true == hSkill->IsLocked() )
			{
				CDnSkillTreeSystem::S_OUTPUT Output;
				CDnSkillTreeSystem::S_TRY_UNLOCK TryUnlockInfo;
				TryUnlockInfo.iTryUnlockSkillID = nSkillID;
				TryUnlockInfo.iCurrentCharLevel = cLevel;

				m_SkillTreeSystem.TryUnLockSkill( TryUnlockInfo, &Output );
				if( CDnSkillTreeSystem::R_SUCCESS != Output.eResult )
					continue;

				Change_SkillLevelUp( nSkillID, 1, ReservationSKillList::Type::UnLock );
			}
			else if( hSkill && false == hSkill->IsAcquired() )
				Change_SkillLevelUp( nSkillID, 1, ReservationSKillList::Type::Acquire );
		}
		else
		{
			Change_SkillLevelUp( nSkillID, itr, ReservationSKillList::Type::LevelUp );
		}
	}

	SetReservationSkill_Item( nSkillID );
}
#endif	// #if defined( PRE_ADD_PRESET_SKILLTREE )