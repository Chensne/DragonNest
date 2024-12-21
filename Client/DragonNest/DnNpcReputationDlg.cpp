#include "StdAfx.h"

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "DnNpcReputationDlg.h"
#include "TaskManager.h"
#include "DnQuestTask.h"
#include "ReputationSystemRepository.h"
#include "DnTableDB.h"
#include "EtUIXML.h"
#include "DnReputationValueTooltipDlg.h"
#include "DnActor.h"
#include "DnCommonUtil.h"
#include "DnItemTask.h"
#include "DnSimpleTooltipDlg.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#endif
#ifdef PRE_ADD_REPUTATION_EXPOSURE
#include "DnLocalPlayerActor.h"
#endif


void CDnNpcReputationDlg::SFavorGiftUI::Show(bool bShow)
{
	pGiftTitle->Show(bShow);

	std::vector<CDnItemSlotButton*>::iterator iter = pGiftSlotBtn.begin();
	for (; iter != pGiftSlotBtn.end(); ++iter)
	{
		CDnItemSlotButton* pBtn = (*iter);
		if (pBtn)
			pBtn->Show(bShow);
	}
}

void CDnNpcReputationDlg::SFavorGiftUI::ResetSlot()
{
	std::vector<CDnItemSlotButton*>::iterator iter = pGiftSlotBtn.begin();
	for (; iter != pGiftSlotBtn.end(); ++iter)
	{
		CDnItemSlotButton* pBtn = (*iter);
		if (pBtn)
		{
			CDnItem *pItem = static_cast<CDnItem*>(pBtn->GetItem());
			SAFE_DELETE(pItem);
			pBtn->ResetSlot();
			pBtn->SetRegist(false);
		}
	}
}

void CDnNpcReputationDlg::SFavorGiftUI::SetItem(CDnItem* pItem)
{
	if (pItem == NULL)
		return;

	std::vector<CDnItemSlotButton*>::iterator iter = pGiftSlotBtn.begin();
	for (; iter != pGiftSlotBtn.end(); ++iter)
	{
		CDnItemSlotButton* pBtn = (*iter);
		if (pBtn && pBtn->IsEmptySlot())
		{
			std::vector<CDnItem *> invenItemList;
			int count = CDnItemTask::GetInstance().GetCharInventory().FindItemList(pItem->GetClassID(), -1, invenItemList);
			pBtn->SetItem(pItem, count > 0 ? count : pItem->GetOverlapCount());
			pBtn->SetRenderCount(count > 0);
			pBtn->SetRegist(count <= 0);
			return;
		}
	}
}

#ifdef PRE_ADD_REPUTATION_EXPOSURE
void CDnNpcReputationDlg::SFavorGiftUI::SetRewardItem(CDnItem* pItem, bool bIsUsable)
{
	if (pItem == NULL)
		return;

	std::vector<CDnItemSlotButton*>::iterator iter = pGiftSlotBtn.begin();
	for (; iter != pGiftSlotBtn.end(); ++iter)
	{
		CDnItemSlotButton* pBtn = (*iter);
		if (pBtn && pBtn->IsEmptySlot())
		{
			std::vector<CDnItem *> invenItemList;
			int count = CDnItemTask::GetInstance().GetCharInventory().FindItemList(pItem->GetClassID(), -1, invenItemList);
			pBtn->SetItem(pItem, count > 0 ? count : pItem->GetOverlapCount());
			pBtn->SetRenderCount(count > 0);
			pBtn->SetRegist(!bIsUsable);
			return;
		}
	}
}
#endif



const int NPC_UNION_ICON_TEXTURE_SIZE = 32;
const int NPC_FACE_TEXTURE_SIZE = 64;

CDnNpcReputationDlg::CDnNpcReputationDlg( UI_DIALOG_TYPE dialogType /* = UI_TYPE_FOCUS */, CEtUIDialog *pParentDialog /* = NULL */, 
										  int nID /* = -1 */, CEtUICallback *pCallback /* = NULL */ ) :
										  CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true ),
										  m_iNowPage( 0 ),
										  m_iNowSelectedNpcSlotIndex( 0 ),
										  m_pStaticPage( NULL ),
										  m_pBtnPrevPage( NULL ),
										  m_pBtnNextPage( NULL ),
										  m_pTextBoxNpcInfo( NULL ),
										  m_iMaxPage( 0 ),
										  m_pTooltipDlg( NULL ),
										  m_pComboSort( NULL ),
										  m_iNowSelectedSortMethod( NPC_SORT_DEFAULT )
										  , m_pUnAvailNpcSeal(NULL)
										  , m_pUnAvailNpcSealText(NULL)
{
	SecureZeroMemory( m_apStaticNpc, sizeof(m_apStaticNpc) );
	SecureZeroMemory( m_apStaticEventReceiver, sizeof(m_apStaticEventReceiver) );
	SecureZeroMemory( m_apTextureNpcFace, sizeof(m_apTextureNpcFace) );
	SecureZeroMemory( m_apStaticNpcSelected, sizeof(m_apStaticNpcSelected) );
	SecureZeroMemory( m_apStaticNpcFavor, sizeof(m_apStaticNpcFavor) );
	SecureZeroMemory( m_apStaticNpcMalice, sizeof(m_apStaticNpcMalice) );
	SecureZeroMemory( m_apProgressFavor, sizeof(m_apProgressFavor) );
#ifdef PRE_MOD_REPUTE_NOMALICE
#else
	SecureZeroMemory( m_apProgressMalice, sizeof(m_apProgressMalice) );
#endif
	SecureZeroMemory( m_pNpcAttrBtns, sizeof(m_pNpcAttrBtns) );
	SecureZeroMemory( m_apTextureNpcUnion, sizeof(m_apTextureNpcUnion) );
	SecureZeroMemory( m_TextureNpcUnionTableIDs, sizeof(m_TextureNpcUnionTableIDs) );
}

CDnNpcReputationDlg::~CDnNpcReputationDlg(void)
{
	SAFE_RELEASE_SPTR( m_hNpcFaceTexture );
	SAFE_RELEASE_SPTR( m_hHideNpcFaceTexture );
	SAFE_RELEASE_SPTR( m_hUnionIconTexture );
	SAFE_DELETE( m_pTooltipDlg );
}

void CDnNpcReputationDlg::Initialize( bool bShow )
{
	//CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "ReputeDlg.ui" ).c_str(), bShow );
#if defined(PRE_ADD_INTEGERATE_QUEST_REPUT)
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "Tab_ReputeListDlg.ui" ).c_str(), bShow );
#else
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "ReputeListDlg.ui" ).c_str(), bShow );
#endif // PRE_ADD_INTEGERATE_QUEST_REPUT
}

void CDnNpcReputationDlg::InitialUpdate( void )
{
	char acBuffer[ 256 ] = { 0 };
	for( int i = 0; i < NPC_REPUTATION_NPC_COUNT_PER_PAGE; ++i )
	{
		sprintf_s( acBuffer, "ID_TEXT_NPCNAME%d", i );
		m_apStaticNpc[ i ] = GetControl<CEtUIStatic>( acBuffer );
		m_apStaticNpc[ i ]->SetButton( true );

		sprintf_s( acBuffer, "ID_CLICK%d", i );;
		m_apStaticEventReceiver[ i ] = GetControl<CEtUIStatic>( acBuffer );
		m_apStaticEventReceiver[ i ]->SetButton( true );

		sprintf_s( acBuffer, "ID_TEXTUREL_NPC%d", i );
		m_apTextureNpcFace[ i ] = GetControl<CEtUITextureControl>( acBuffer );

		sprintf_s( acBuffer, "ID_TEXTUREL_MARK%d", i );
		m_apTextureNpcUnion[ i ] = GetControl<CEtUITextureControl>( acBuffer );

		sprintf_s( acBuffer, "ID_SELECT%d", i );
		m_apStaticNpcSelected[ i ] = GetControl<CEtUIStatic>( acBuffer );

		sprintf_s( acBuffer, "ID_TEXT_FAVORCOUNT%d", i );
		m_apStaticNpcFavor[ i ] = GetControl<CEtUIStatic>( acBuffer );

		sprintf_s( acBuffer, "ID_TEXT_MALICECOUNT%d", i );
		m_apStaticNpcMalice[ i ] = GetControl<CEtUIStatic>( acBuffer );

		sprintf_s( acBuffer, "ID_PROGRESSBAR_FAVOR%d", i );
		m_apProgressFavor[ i ] = GetControl<CEtUIProgressBar>( acBuffer );
		m_apProgressFavor[ i ]->SetSmooth( true );

#ifdef PRE_MOD_REPUTE_NOMALICE
#else
		sprintf_s( acBuffer, "ID_PROGRESSBAR_MALICE%d", i );
		m_apProgressMalice[ i ] = GetControl<CEtUIProgressBar>( acBuffer );
		m_apProgressMalice[ i ]->SetSmooth( true );
#endif
	}

	m_pComboSort = GetControl<CEtUIComboBox>( "ID_COMBOBOX_SORT" );
#ifdef PRE_MOD_REPUTE_NOMALICE2
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3209), NULL, NPC_SORT_DEFAULT );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3210), NULL, NPC_SORT_FAVOR );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3212), NULL, NPC_SORT_NAME );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3267), NULL, NPC_SORT_UNION );	// UISTRING : 그룹 순으로 정렬
#else
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3209), NULL, 0 );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3210), NULL, 1 );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3211), NULL, 2 ); // UISTRING : 불만 순으로 정렬
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3212), NULL, 3 );
	m_pComboSort->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3267), NULL, 4 );	// UISTRING : 그룹 순으로 정렬
#endif // PRE_MOD_REPUTE_NOMALICE2

	m_pStaticPage = GetControl<CEtUIStatic>( "ID_BT_PAGE" );
	m_pBtnPrevPage = GetControl<CEtUIButton>( "ID_BT_PRIOR" );
	m_pBtnNextPage = GetControl<CEtUIButton>( "ID_BT_NEXT" );

	m_pTextBoxNpcInfo = GetControl<CEtUITextBox>( "ID_TEXTBOX_NPCDETAIL" );

	SAFE_RELEASE_SPTR( m_hNpcFaceTexture );
	m_hNpcFaceTexture = LoadResource( "Repute_NPC.dds", RT_TEXTURE, true );

	SAFE_RELEASE_SPTR(m_hHideNpcFaceTexture);
	m_hHideNpcFaceTexture = LoadResource("ReputeGray_NPC.dds", RT_TEXTURE, true);

	SAFE_RELEASE_SPTR(m_hUnionIconTexture);
	m_hUnionIconTexture = LoadResource("Repute_SmallMark.dds", RT_TEXTURE, true);

	m_pUnAvailNpcSeal		= GetControl<CEtUIStatic>("ID_STATIC_SECRETBASE");
	m_pUnAvailNpcSealText	= GetControl<CEtUIStatic>("ID_TEXT_SECRET");

	int j = 0;
	std::string ctrlName;
	for (; j < eNPCATTRTAB_MAX; ++j)
	{
		ctrlName = FormatA("ID_RBT_TAB%d", j);
		m_pNpcAttrBtns[j] = GetControl<CEtUIRadioButton>(ctrlName.c_str());
	}

	m_FavorGiftUI.pGiftTitle = GetControl<CEtUIStatic>("ID_TEXT0");
	m_FavorGiftUI.pGiftSlotBtn.reserve(MAX_ACCEPT_NPC_PRESENT_VARIATION_COUNT);
	for (j = 0; j < MAX_ACCEPT_NPC_PRESENT_VARIATION_COUNT; ++j)
	{
		ctrlName = FormatA("ID_SLOT_ITEM%d", j);
		CDnItemSlotButton* pBtn = GetControl<CDnItemSlotButton>(ctrlName.c_str());
		pBtn->SetSlotType(ST_NPC_PRESENT);
		pBtn->ResetSlot();
		pBtn->SetRegist(false);
		m_FavorGiftUI.pGiftSlotBtn.push_back(pBtn);
	}

#ifdef PRE_MOD_CORRECT_UISOUND_PLAY
	m_DlgInfo.bSound = false;
#endif

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	m_RewardItemUI.pGiftTitle = GetControl<CEtUIStatic>("ID_TEXT1");
	m_RewardItemUI.pGiftSlotBtn.reserve(MAX_ACCEPT_NPC_PRESENT_VARIATION_COUNT);
	for (j = 0; j < MAX_ACCEPT_NPC_PRESENT_VARIATION_COUNT; ++j)
	{
		ctrlName = FormatA("ID_SLOT_ITEM%d", j);
		CDnItemSlotButton* pBtn = GetControl<CDnItemSlotButton>(ctrlName.c_str());
		pBtn->SetSlotType(ST_NPC_PRESENT);
		pBtn->ResetSlot();
		pBtn->SetRegist(false);
		m_RewardItemUI.pGiftSlotBtn.push_back(pBtn);
	}
#endif

	m_pTooltipDlg = new CDnReputationValueTooltipDlg( UI_TYPE_CHILD, this );
	m_pTooltipDlg->Initialize( false );
}

void CDnNpcReputationDlg::_UpdateReputeNpcList(int iSortMethod)
{
	m_vlAvailNpcReputeTableIDs.clear();
	m_vlUnAvailNpcReputeTableIDs.clear();

	// 현재 오픈된 npc 호감도의 정보를 모아둔다.
	CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
	if( !pQuestTask ) 
		return;

	DNTableFileFormat*  pReputeTable = GetDNTable( CDnTableDB::TREPUTE );
	CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
	if( pReputationRepos )
	{
		for( int iIndex = 0; iIndex < pReputeTable->GetItemCount(); ++iIndex )
		{
			// 퀘스트 클리어된 npc 들만 호감도 처리되므로 모아둔다.
			int iReputeTableItemID = pReputeTable->GetItemID( iIndex );

#ifdef PRE_ADD_REPUTATION_EXPOSURE
			// 미션 클리어 유무를 체크한다.
			bool bIsClearMission = false;

			int nMissionID = pReputeTable->GetFieldFromLablePtr(iReputeTableItemID, "_Mission")->GetInteger();
			if( nMissionID > 0 )
			{
				CDnMissionTask::MissionInfoStruct* pMissionInfo = GetMissionTask().GetMissionFromArrayIndex(nMissionID - 1);
				if(pMissionInfo) 
					bIsClearMission = pMissionInfo->bAchieve;
			}
			
			if(bIsClearMission)
			{
				m_vlAvailNpcReputeTableIDs.push_back( iReputeTableItemID );
			}
			else
			{
				m_vlUnAvailNpcReputeTableIDs.push_back( iReputeTableItemID );
			}
#else // PRE_ADD_REPUTATION_EXPOSURE
			// 퀘스트 클리어 유무를 체크한다.
			bool bCompleletedOpenQuest = false;
			std::vector<std::string> tokens;
			std::string szReputeClearQuestString = pReputeTable->GetFieldFromLablePtr(iReputeTableItemID, "_QuestID2")->GetString();
			TokenizeA(szReputeClearQuestString, tokens, ",");

			std::vector<std::string>::iterator iter = tokens.begin();
			for (; iter != tokens.end(); ++iter)
			{
				std::string& token = (*iter);
				int questId = atoi(token.c_str());

				if (pQuestTask->IsClearQuest(questId))
				{
					bCompleletedOpenQuest = true;
					break;
				}
			}

			if( bCompleletedOpenQuest )
			{
				m_vlAvailNpcReputeTableIDs.push_back( iReputeTableItemID );
			}
			else
			{
				m_vlUnAvailNpcReputeTableIDs.push_back( iReputeTableItemID );
			}
#endif // PRE_ADD_REPUTATION_EXPOSURE
		}
	}

	m_iMaxPage = ((int)m_vlAvailNpcReputeTableIDs.size() + (int)m_vlUnAvailNpcReputeTableIDs.size() - 1) / NPC_REPUTATION_NPC_COUNT_PER_PAGE;

	// 정렬 선택되어있다면 정렬처리.
	if( NPC_SORT_DEFAULT != iSortMethod )
	{
		switch( iSortMethod )
		{
			case NPC_SORT_FAVOR:
				{
					struct SortByFavor : public binary_function<const int, const int, bool>
					{
						CReputationSystemRepository* pReputationRepos;
						DNTableFileFormat*  pReputeTable;

						SortByFavor( CReputationSystemRepository* _pReputationRepos, DNTableFileFormat*  _pReputeTable ) : pReputationRepos( _pReputationRepos ),
																												 pReputeTable( _pReputeTable ) {};

						bool operator () ( const int iReputeTableIDA, const int iReputeTableIDB )
						{
							int iNpcTableIDA = pReputeTable->GetFieldFromLablePtr( iReputeTableIDA, "_NpcID" )->GetInteger();
							int iFavorPercentA = (int)pReputationRepos->GetNpcReputationPercent( iNpcTableIDA, IReputationSystem::NpcFavor );

							int iNpcTableIDB = pReputeTable->GetFieldFromLablePtr( iReputeTableIDB, "_NpcID" )->GetInteger();
							int iFavorPercentB = (int)pReputationRepos->GetNpcReputationPercent( iNpcTableIDB, IReputationSystem::NpcFavor );

							return iFavorPercentA > iFavorPercentB;
						}
					};

					std::sort( m_vlAvailNpcReputeTableIDs.begin(), m_vlAvailNpcReputeTableIDs.end(), SortByFavor(pReputationRepos, pReputeTable) );
				}
				break;

			case NPC_SORT_MALICE:
				{
					struct SortByMalice : public binary_function<const int, const int, bool>
					{
						CReputationSystemRepository* pReputationRepos;
						DNTableFileFormat*  pReputeTable;

						SortByMalice( CReputationSystemRepository* _pReputationRepos, DNTableFileFormat*  _pReputeTable ) : pReputationRepos( _pReputationRepos ),
																												 pReputeTable( _pReputeTable ) {};

						bool operator () ( const int iReputeTableIDA, const int iReputeTableIDB )
						{
							int iNpcTableIDA = pReputeTable->GetFieldFromLablePtr( iReputeTableIDA, "_NpcID" )->GetInteger();
							int iMalicePercentA = (int)pReputationRepos->GetNpcReputationPercent( iNpcTableIDA, IReputationSystem::NpcMalice );

							int iNpcTableIDB = pReputeTable->GetFieldFromLablePtr( iReputeTableIDB, "_NpcID" )->GetInteger();
							int iMalicePercentB = (int)pReputationRepos->GetNpcReputationPercent( iNpcTableIDB, IReputationSystem::NpcMalice );

							return iMalicePercentA > iMalicePercentB;
						}
					};

					std::sort( m_vlAvailNpcReputeTableIDs.begin(), m_vlAvailNpcReputeTableIDs.end(), SortByMalice(pReputationRepos, pReputeTable) );
				}
				break;

			case NPC_SORT_NAME:
				{
					struct SortByName : public binary_function<const int, const int, bool>
					{
						DNTableFileFormat*  pReputeTable;
						DNTableFileFormat*  pNpcTable;

						SortByName( DNTableFileFormat*  _pNpcTable, DNTableFileFormat*  _pReputeTable ) : pNpcTable(_pNpcTable), pReputeTable(_pReputeTable) {};

						bool operator () ( const int iReputeTableIDA, const int iReputeTableIDB )
						{
							int iNpcTableIDA = pReputeTable->GetFieldFromLablePtr( iReputeTableIDA, "_NpcID" )->GetInteger();
							int iNpcNameA = pNpcTable->GetFieldFromLablePtr( iNpcTableIDA, "_NameID" )->GetInteger();
							const wchar_t* pNpcNameA = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iNpcNameA );

							int iNpcTableIDB = pReputeTable->GetFieldFromLablePtr( iReputeTableIDB, "_NpcID" )->GetInteger();
							int iNpcNameB = pNpcTable->GetFieldFromLablePtr( iNpcTableIDB, "_NameID" )->GetInteger();
							const wchar_t* pNpcNameB = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iNpcNameB );

							return (0 > wcscmp( pNpcNameA, pNpcNameB ));
						}
					};

					DNTableFileFormat*  pNpcTable = GetDNTable( CDnTableDB::TNPC );
					std::sort( m_vlAvailNpcReputeTableIDs.begin(), m_vlAvailNpcReputeTableIDs.end(), SortByName(pNpcTable, pReputeTable) );
					std::sort( m_vlUnAvailNpcReputeTableIDs.begin(), m_vlUnAvailNpcReputeTableIDs.end(), SortByName(pNpcTable, pReputeTable) );
				}
				break;

			case NPC_SORT_UNION:
				{
					struct SortByUnion : public binary_function<const int, const int, bool>
					{
						DNTableFileFormat*  pReputeTable;

						SortByUnion( DNTableFileFormat*  _pReputeTable ) : pReputeTable(_pReputeTable) {};

						bool operator () ( const int iReputeTableIDA, const int iReputeTableIDB )
						{
							int unionA = pReputeTable->GetFieldFromLablePtr( iReputeTableIDA, "_UnionID" )->GetInteger();
							int unionB = pReputeTable->GetFieldFromLablePtr( iReputeTableIDB, "_UnionID" )->GetInteger();

							return (unionA > unionB);
						}
					};

					std::sort( m_vlAvailNpcReputeTableIDs.begin(), m_vlAvailNpcReputeTableIDs.end(), SortByUnion(pReputeTable) );
					std::sort( m_vlUnAvailNpcReputeTableIDs.begin(), m_vlUnAvailNpcReputeTableIDs.end(), SortByUnion(pReputeTable) );
				}
				break;
		}
	}
}


void CDnNpcReputationDlg::Show( bool bShow )
{
	CDnCustomDlg::Show( bShow );

	if( bShow )
	{
		m_iNowPage = 0;
		m_iNowSelectedNpcSlotIndex = -1;
		m_iNowSelectedSortMethod = NPC_SORT_DEFAULT;

#ifdef PRE_MOD_REPUTE_NOMALICE2
		m_pComboSort->SetSelectedByValue( m_iNowSelectedSortMethod );
#else
		m_pComboSort->SetSelectedByIndex( m_iNowSelectedSortMethod );
#endif

		// 아래 함수들은 SetSelectedByIndex() 함수를 호출하면 ProcessCommand() 함수 내부에서 호출되므로 
		// 여기서 호출하지 않는다.
		//_UpdateAvailNpcList( m_iNowSelectedSortMethod );
		//UpdatePage( m_iNowPage );
		//UpdateNpcSlotSelected( m_iNowSelectedNpcSlotIndex );

		// 한기씨~ 다이얼로그 닫혀있다가 열릴때 혹시 선택할 수 있는 npc 있을 경우 제일 첫번째거 선택해달라해서
		// 우선 이렇게 처리해뒀어요. 이상하시면 고쳐주세요~
		if( m_vlAvailNpcReputeTableIDs.size() )
		{
			ProcessCommand( EVENT_BUTTON_CLICKED, false, m_apStaticEventReceiver[0], 0 );
		}
	}
}

#define NO_UNION_ICON_IDX 3

void CDnNpcReputationDlg::UpdatePage( int iPage )
{
	Clear();

	// 페이지 단위로 npc 호감도, 악의 정보 출력.
	CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
	if( !pQuestTask ) 
		return;

	DNTableFileFormat*  pReputeTable = GetDNTable( CDnTableDB::TREPUTE );
	int iNumAvailNpcs = (int)m_vlAvailNpcReputeTableIDs.size();
	int iNumUnAvailNpcs = (int)m_vlUnAvailNpcReputeTableIDs.size();

	int iAvailIndex = iPage * NPC_REPUTATION_NPC_COUNT_PER_PAGE;
	int iUnAvailIndex = iPage * NPC_REPUTATION_NPC_COUNT_PER_PAGE - iNumAvailNpcs;
	CommonUtil::ClipNumber(iUnAvailIndex, 0, iUnAvailIndex);
	for (int i = 0; i < NPC_REPUTATION_NPC_COUNT_PER_PAGE; ++i)
	{
		int unionIconWidthCount = 1;
		if (m_hUnionIconTexture)
			unionIconWidthCount = m_hUnionIconTexture->Width() / NPC_UNION_ICON_TEXTURE_SIZE;

		if (iAvailIndex < iNumAvailNpcs)
		{
			CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();

			if (pReputationRepos)
			{
				int iReputeTableItemID = m_vlAvailNpcReputeTableIDs.at( iAvailIndex );

				int iNumFacePerHorizon = 1;
				if (m_hNpcFaceTexture)
					iNumFacePerHorizon = m_hNpcFaceTexture->Width() / NPC_FACE_TEXTURE_SIZE;

				int iNpcID = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, "_NpcID" )->GetInteger();
				REPUTATION_TYPE iFavorValue = pReputationRepos->GetNpcReputation( iNpcID, IReputationSystem::NpcFavor );
				REPUTATION_TYPE iFavorMax = pReputationRepos->GetNpcReputation( iNpcID, IReputationSystem::NpcFavor );
				REPUTATION_TYPE iMaliceValue = pReputationRepos->GetNpcReputation( iNpcID, IReputationSystem::NpcMalice );
				REPUTATION_TYPE iMaliceMax = pReputationRepos->GetNpcReputation( iNpcID, IReputationSystem::NpcMalice );
				int iFavorPercent = pReputationRepos->GetNpcReputationPercent( iNpcID, IReputationSystem::NpcFavor );
				int iMalicePercent = pReputationRepos->GetNpcReputationPercent( iNpcID, IReputationSystem::NpcMalice );

				// npc id  
				m_NpcIDs[i].npcId = iNpcID;
				m_NpcIDs[i].bAvail = true;

				// npc 이름.
				DNTableFileFormat*  pNpcTable = GetDNTable( CDnTableDB::TNPC );
				int iNameUIStringID = pNpcTable->GetFieldFromLablePtr( iNpcID, "_NameID" )->GetInteger();			
				m_apStaticNpc[ i ]->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iNameUIStringID) );

				// npc 얼굴.
				int iFaceIndex = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, "_NpcIcon" )->GetInteger();
				m_apTextureNpcFace[ i ]->SetTexture( m_hNpcFaceTexture, (iFaceIndex%iNumFacePerHorizon)*NPC_FACE_TEXTURE_SIZE, (iFaceIndex/iNumFacePerHorizon) * NPC_FACE_TEXTURE_SIZE,
					NPC_FACE_TEXTURE_SIZE, NPC_FACE_TEXTURE_SIZE );
				m_apTextureNpcFace[ i ]->Show( true );

				wchar_t wcBuffer[ 256 ] = { 0 };

				// 호감도, 악의 수치.
				wprintf_s( wcBuffer, L"%d/%d", iFavorValue, iFavorMax );
				m_apStaticNpcFavor[ i ]->SetText( wcBuffer );

				wprintf_s( wcBuffer, L"%d/%d", iMaliceValue, iMaliceMax );
				m_apStaticNpcMalice[ i ]->SetText( wcBuffer );

				// 호감도, 악의 비율 progress 셋팅.
				m_apProgressFavor[ i ]->SetProgress( (float)iFavorPercent );
#ifdef PRE_MOD_REPUTE_NOMALICE
#else
				m_apProgressMalice[ i ]->SetProgress( (float)iMalicePercent );
#endif

				//	union icon
				int unionId = pReputeTable->GetFieldFromLablePtr(iReputeTableItemID, "_UnionID")->GetInteger();
				int unionIconIdx = 0;

				int unionTableId = -1;
				if (unionId == NpcReputation::UnionType::Etc)
				{
					unionIconIdx = NO_UNION_ICON_IDX;
				}
				else
				{
					DNTableFileFormat*  pUnionTable = GetDNTable(CDnTableDB::TUNION);
					unionTableId = pUnionTable->GetItemIDFromField("_UnionID", unionId);

					unionIconIdx = pUnionTable->GetFieldFromLablePtr(unionTableId, "_UnionIconID")->GetInteger();
				}

				m_apTextureNpcUnion[i]->SetTexture(m_hUnionIconTexture, (unionIconIdx % unionIconWidthCount) * NPC_UNION_ICON_TEXTURE_SIZE, (unionIconIdx / unionIconWidthCount) * NPC_UNION_ICON_TEXTURE_SIZE,
					NPC_UNION_ICON_TEXTURE_SIZE, NPC_UNION_ICON_TEXTURE_SIZE);
				m_apTextureNpcUnion[i]->Show(true);

				m_TextureNpcUnionTableIDs[i] = unionTableId;
			}

			++iAvailIndex;
		}
		else
		{
			if (iUnAvailIndex < iNumUnAvailNpcs)
			{
				int iReputeTableItemID = m_vlUnAvailNpcReputeTableIDs.at( iUnAvailIndex );
				int iNumFacePerHorizon = 1;
				if (m_hHideNpcFaceTexture)
					iNumFacePerHorizon = m_hHideNpcFaceTexture->Width() / NPC_FACE_TEXTURE_SIZE;
				int iNpcID = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, "_NpcID" )->GetInteger();

				// npc id  
				m_NpcIDs[i].npcId = iNpcID;
				m_NpcIDs[i].bAvail = false;

				// npc 이름.
				DNTableFileFormat*  pNpcTable = GetDNTable( CDnTableDB::TNPC );
				int iNameUIStringID = pNpcTable->GetFieldFromLablePtr( iNpcID, "_NameID" )->GetInteger();
				m_apStaticNpc[ i ]->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iNameUIStringID) );

				// npc 얼굴.
				int iFaceIndex = pReputeTable->GetFieldFromLablePtr( iReputeTableItemID, "_NpcIcon" )->GetInteger();
				m_apTextureNpcFace[ i ]->SetTexture( m_hHideNpcFaceTexture, (iFaceIndex%iNumFacePerHorizon)*NPC_FACE_TEXTURE_SIZE, (iFaceIndex/iNumFacePerHorizon) * NPC_FACE_TEXTURE_SIZE,
					NPC_FACE_TEXTURE_SIZE, NPC_FACE_TEXTURE_SIZE );
				m_apTextureNpcFace[ i ]->Show( true );

				//	union icon
				int unionId = pReputeTable->GetFieldFromLablePtr(iReputeTableItemID, "_UnionID")->GetInteger();
				int unionIconIdx = 0;
				int unionTableId = -1;

				if (unionId == NpcReputation::UnionType::Etc)
				{
					unionIconIdx = NO_UNION_ICON_IDX;
				}
				else
				{
					DNTableFileFormat*  pUnionTable = GetDNTable(CDnTableDB::TUNION);
					unionTableId = pUnionTable->GetItemIDFromField("_UnionID", unionId);

					unionIconIdx = pUnionTable->GetFieldFromLablePtr(unionTableId, "_UnionIconID")->GetInteger();
				}

				m_apTextureNpcUnion[i]->SetTexture(m_hUnionIconTexture, (unionIconIdx % unionIconWidthCount) * NPC_UNION_ICON_TEXTURE_SIZE, (unionIconIdx / unionIconWidthCount) * NPC_UNION_ICON_TEXTURE_SIZE,
					NPC_UNION_ICON_TEXTURE_SIZE, NPC_UNION_ICON_TEXTURE_SIZE);
				m_apTextureNpcUnion[i]->Show(true);

				m_TextureNpcUnionTableIDs[i] = unionTableId;

				++iUnAvailIndex;
			}
		}
	}

	m_pStaticPage->SetIntToText( m_iNowPage+1 );
}

CDnNpcReputationDlg::eNPCAttrTabBtnType CDnNpcReputationDlg::GetCurrentNpcAttrTabType() const
{
	int i = 0;
	for (; i < eNPCATTRTAB_MAX; ++i)
	{
		if (m_pNpcAttrBtns[i]->IsChecked())
			return (eNPCAttrTabBtnType)i;
	}

	return eNPCATTRTAB_MAX;
}

int CDnNpcReputationDlg::GetCurrentSelectedUISlotIdx() const
{
	int i = 0;
	for (; i < NPC_REPUTATION_NPC_COUNT_PER_PAGE; ++i)
	{
		if (m_apStaticNpcSelected[i] && m_apStaticNpcSelected[i]->IsShow())
			return i;
	}

	return NPC_REPUTATION_NPC_COUNT_PER_PAGE;
}

void CDnNpcReputationDlg::UpdateNpcExplain(int uiSlotIdx)
{
	DNTableFileFormat*  pReputeTable = GetDNTable( CDnTableDB::TREPUTE );
	int iReputeTableID = m_vlAvailNpcReputeTableIDs.at(uiSlotIdx);

	int iNpcInfoUIStringID = pReputeTable->GetFieldFromLablePtr( iReputeTableID, "_NPCInfoID" )->GetInteger();
	m_pTextBoxNpcInfo->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iNpcInfoUIStringID), D3DCOLOR_ARGB(255, 60, 45, 25) );
}

void CDnNpcReputationDlg::UpdateNpcFavorGift(int uiSlotIdx)
{
	m_FavorGiftUI.ResetSlot();

	CDnItemTask* pItemTask = static_cast<CDnItemTask*>(CTaskManager::GetInstance().GetTask("ItemTask"));
	if( !pItemTask ) 
		return;

	// npc 호감도 테이블을 뒤져서 이 npc 가 받는 선물이 무엇인지 표시해준다.
	DNTableFileFormat*  pReputeTable = GetDNTable( CDnTableDB::TREPUTE );
	DNTableFileFormat*  pPresentTable = GetDNTable( CDnTableDB::TPRESENT );
	if (pReputeTable == NULL || pPresentTable == NULL)
		return;

	int iReputeTableID = m_vlAvailNpcReputeTableIDs.at(uiSlotIdx);

	// 현재 선물 최대 갯수는 6개임
	bool bItemExist = false;
	std::string giftColumnString;
	for( int i = 0; i < MAX_ACCEPT_NPC_PRESENT_VARIATION_COUNT; ++i )
	{
		giftColumnString = FormatA("_PresentID%d", i + 1);
		int iPresentTableItemID = pReputeTable->GetFieldFromLablePtr(iReputeTableID, giftColumnString.c_str())->GetInteger();
		if (0 == iPresentTableItemID)
			continue;

		int iAcceptCount = pPresentTable->GetFieldFromLablePtr( iPresentTableItemID, "_Count" )->GetInteger();
		int iPresentType = pPresentTable->GetFieldFromLablePtr( iPresentTableItemID, "_PresentType" )->GetInteger();
		if( 0 == iPresentType ||
			1 == iPresentType )
		{
			int iPresentItemID = pPresentTable->GetFieldFromLablePtr( iPresentTableItemID, "_PresentTypeID" )->GetInteger();

			TItemInfo ItemInfo;
			bool bSuccess = CDnItem::MakeItemInfo( iPresentItemID, iAcceptCount, ItemInfo );
			_ASSERT( bSuccess );
			CDnItem* pItem = pItemTask->CreateItem( ItemInfo );
			if( pItem )
				m_FavorGiftUI.SetItem(pItem);
		}
	}

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	m_FavorGiftUI.pGiftTitle->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1 , 3264));
#endif
	m_FavorGiftUI.Show(true);
}

#ifdef PRE_ADD_REPUTATION_EXPOSURE
void CDnNpcReputationDlg::UpdateNpcRewardGift(int uiSlotIdx)
{
	m_RewardItemUI.ResetSlot();

	CDnItemTask*	pItemTask	= static_cast<CDnItemTask*>(CTaskManager::GetInstance().GetTask("ItemTask"));
	CDnQuestTask*	pQuestTask	= static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask("QuestTask"));
	if( !pItemTask || !pQuestTask ) return;

	DNTableFileFormat* pReputeTable  = GetDNTable( CDnTableDB::TREPUTE );
	if(!pReputeTable) return;
	
	int iReputeTableID = m_vlAvailNpcReputeTableIDs.at(uiSlotIdx);
	CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
	if(pReputationRepos)
	{
		int nNpcID = pReputeTable->GetFieldFromLablePtr(iReputeTableID, "_NpcID")->GetInteger();
		REPUTATION_TYPE nCurrentRepute = pReputationRepos->GetNpcReputation(nNpcID, IReputationSystem::NpcFavor);

		int nReputeValue = -1;
		std::string szItemString;
		for(int i = 0 ; i < 2 ; ++i)
		{
			nReputeValue = pReputeTable->GetFieldFromLablePtr(iReputeTableID, FormatA("_NpcPresentRepute%d",i+1).c_str() )->GetInteger();
			szItemString = FormatA("_NpcPresentID%d",i+1);

			int nPresentItemID = pReputeTable->GetFieldFromLablePtr(iReputeTableID, szItemString.c_str())->GetInteger();

			TItemInfo ItemInfo;
			bool bSuccess = CDnItem::MakeItemInfo( nPresentItemID, 1, ItemInfo);
			_ASSERT(bSuccess);

			CDnItem* pItem = pItemTask->CreateItem( ItemInfo );
			if(pItem)
				m_RewardItemUI.SetRewardItem( pItem, (nReputeValue <= nCurrentRepute) );
		}
	}

	m_RewardItemUI.pGiftTitle->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1 , 1391)); // UIString: NPC가 주는 선물
	m_RewardItemUI.Show(true);
}
#endif

void CDnNpcReputationDlg::SetNpcPage(int npcId)
{
	DNTableFileFormat*  pReputeTable = GetDNTable( CDnTableDB::TREPUTE );
	if (pReputeTable == NULL)
		return;

	int slotIdx = 0;
	vector<int>::const_iterator availIter = m_vlAvailNpcReputeTableIDs.begin();
	for (; availIter != m_vlAvailNpcReputeTableIDs.end(); ++availIter, ++slotIdx)
	{
		const int& reputeTableId = (*availIter);
		int iNpcID = pReputeTable->GetFieldFromLablePtr( reputeTableId, "_NpcID" )->GetInteger();
		if (iNpcID == npcId)
		{
			m_iNowPage = slotIdx / NPC_REPUTATION_NPC_COUNT_PER_PAGE;
			m_iNowSelectedNpcSlotIndex = slotIdx;
			UpdatePage(m_iNowPage);
			UpdateNpcSlotSelected(slotIdx % NPC_REPUTATION_NPC_COUNT_PER_PAGE);
			return;
		}
	}

	if (availIter == m_vlAvailNpcReputeTableIDs.end())
	{
		vector<int>::const_iterator unAvailIter = m_vlUnAvailNpcReputeTableIDs.begin();
		for (; unAvailIter != m_vlUnAvailNpcReputeTableIDs.end(); ++unAvailIter, ++slotIdx)
		{
			const int& reputeTableId = (*unAvailIter);
			int iNpcID = pReputeTable->GetFieldFromLablePtr( reputeTableId, "_NpcID" )->GetInteger();
			if (iNpcID == npcId)
			{
				m_iNowPage = slotIdx / NPC_REPUTATION_NPC_COUNT_PER_PAGE;
				m_iNowSelectedNpcSlotIndex = slotIdx;
				UpdatePage(m_iNowPage);
				UpdateNpcSlotSelected(slotIdx % NPC_REPUTATION_NPC_COUNT_PER_PAGE);
				return;
			}
		}
	}
}

void CDnNpcReputationDlg::UpdateNpcSlotSelected( int iSelectedNpcSlotIndex )
{
	_ASSERT( iSelectedNpcSlotIndex < NPC_REPUTATION_NPC_COUNT_PER_PAGE );

	// 우선 전부 숨기고 npc 설명은 클리어.
	for( int i = 0; i < NPC_REPUTATION_NPC_COUNT_PER_PAGE; ++i )
		m_apStaticNpcSelected[ i ]->Show( false );
	m_pTextBoxNpcInfo->ClearText();

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	m_FavorGiftUI.pGiftTitle->ClearText();
	m_FavorGiftUI.pGiftTitle->Show(false);
	m_RewardItemUI.pGiftTitle->ClearText();
	m_RewardItemUI.pGiftTitle->Show(false);
#endif

	m_pUnAvailNpcSeal->Show(false);
	m_pUnAvailNpcSealText->Show(false);
	int j = 0;
	for (; j < eNPCATTRTAB_MAX; ++j)
		m_pNpcAttrBtns[j]->Enable(false);

	m_FavorGiftUI.Show(false);

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	m_RewardItemUI.Show(false);
#endif

	if( -1 == iSelectedNpcSlotIndex )
	{
		m_pUnAvailNpcSeal->Show(true);
		return;
	}

	int iArrayIndexFromUISlotIndex = m_iNowPage*NPC_REPUTATION_NPC_COUNT_PER_PAGE + iSelectedNpcSlotIndex;
	if( iArrayIndexFromUISlotIndex < (int)m_vlAvailNpcReputeTableIDs.size() )
	{
		m_apStaticNpcSelected[ iSelectedNpcSlotIndex ]->Show( true );
		int j = 0;
		for (; j < eNPCATTRTAB_MAX; ++j)
			m_pNpcAttrBtns[j]->Enable(true);

		eNPCAttrTabBtnType tabType = GetCurrentNpcAttrTabType();
		if (tabType == eNPCATTRTAB_EXPLAIN)
		{
			// NPC 설명 쪽 업데이트
			UpdateNpcExplain(iArrayIndexFromUISlotIndex);
		}
		else if (tabType == eNPCATTRTAB_GIFT)
		{
			UpdateNpcFavorGift(iArrayIndexFromUISlotIndex);
		}
#ifdef PRE_ADD_REPUTATION_EXPOSURE
		else if (tabType == eNPCATTRTAB_REWARD)
		{
			UpdateNpcRewardGift(iArrayIndexFromUISlotIndex);
		}
#endif
		else
		{
			m_pNpcAttrBtns[eNPCATTRTAB_EXPLAIN]->SetChecked(true);
			UpdateNpcExplain(iArrayIndexFromUISlotIndex);
		}
	}
	else
	{
		int unAvailNpcIdx = iArrayIndexFromUISlotIndex - (int)m_vlAvailNpcReputeTableIDs.size();
		if (unAvailNpcIdx >= 0 && unAvailNpcIdx < (int)m_vlUnAvailNpcReputeTableIDs.size())
		{
			m_apStaticNpcSelected[ iSelectedNpcSlotIndex ]->Show( true );

			m_pUnAvailNpcSeal->Show(true);

			std::wstring sealText;
			DNTableFileFormat*  pReputeTable = GetDNTable( CDnTableDB::TREPUTE );
			int iReputeTableID = m_vlUnAvailNpcReputeTableIDs.at( unAvailNpcIdx );

			int npcReputeOpenConditionStringID	= pReputeTable->GetFieldFromLablePtr(iReputeTableID, "_RibbonRewardInforID")->GetInteger();
#if !defined(PRE_ADD_REPUTATION_EXPOSURE)
			if (CDnActor::s_hLocalActor)
			{
				// PRE_ADD_REPUTATION_EXPOSURE일때는, 호감도 오픈 힌트를 통일 시켰으므로,(마을 입장) 아카데믹 전용 오픈 힌트를 사용하지 않는다.
				int curStringID = 0;
				std::string colName;
				int classId = CDnActor::s_hLocalActor->GetClassID();
				colName = FormatA("_%d_RibbonRewardInforID", classId);
				DNTableCell* pField = pReputeTable->GetFieldFromLablePtr(iReputeTableID, colName.c_str());
				if (pField)
					curStringID = pField->GetInteger();

				if (curStringID > 0)
					npcReputeOpenConditionStringID = curStringID;
			}
#endif // !defined(PRE_ADD_REPUTATION_EXPOSURE)
			int npcReputeOpenTypeStringID		= pReputeTable->GetFieldFromLablePtr(iReputeTableID, "_RibbonTypeID")->GetInteger();
			int mapID							= pReputeTable->GetFieldFromLablePtr(iReputeTableID, "_MapID")->GetInteger();
			
			std::wstring mapName;
			if (mapID <= 0)
			{
				mapName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3287);	// UISTRING : 모든 마을
			}
			else
			{
				DNTableFileFormat* pMapSox = GetDNTable( CDnTableDB::TMAP );
				if (pMapSox)
				{
					int stringNum = pMapSox->GetFieldFromLablePtr(mapID, "_MapNameID")->GetInteger();
					mapName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, stringNum);
				}
			}

			std::wstring wszStr0 = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3279);
			std::wstring wszStr1 = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9023);
			std::wstring wszStr2 = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, npcReputeOpenTypeStringID);
			std::wstring wszStr3 = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, npcReputeOpenConditionStringID);
			sealText = FormatW(L"%s%s\n\n%s\n%s\n%s", wszStr0.c_str(), mapName.c_str(), wszStr1.c_str(), wszStr2.c_str(), wszStr3.c_str());

			m_pUnAvailNpcSealText->SetText(sealText.c_str());
			m_pUnAvailNpcSealText->Show(true);
		}
	}
}


void CDnNpcReputationDlg::Clear( void )
{
	for( int i = 0; i < NPC_REPUTATION_NPC_COUNT_PER_PAGE; ++i )
	{
		// UI 내용 클리어
		m_apStaticNpc[ i ]->ClearText();
		m_apTextureNpcFace[ i ]->Show( false );
		m_apTextureNpcUnion[ i ]->Show(false);
		m_NpcIDs[ i ].bAvail = false;
		m_NpcIDs[ i ].npcId = -1;
		m_apStaticNpcSelected[ i ]->Show( false );
		m_apStaticNpcFavor[ i ]->ClearText();
		m_apStaticNpcMalice[ i ]->ClearText();
		m_apProgressFavor[ i ]->SetProgress( 0.0f );
#ifdef PRE_MOD_REPUTE_NOMALICE
#else
		m_apProgressMalice[ i ]->SetProgress( 0.0f );
#endif
	}
}


void CDnNpcReputationDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /*= 0*/ )
{
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_PRIOR" ) )
		{
			RefreshPage(false);
		}
		else
		if( IsCmdControl( "ID_BT_NEXT" ) )
		{
			RefreshPage(true);
		}
		else
		if( strstr( pControl->GetControlName(), "ID_CLICK" ) )
		{
			int iIndex = 0;
			for( int i = 0; i < NPC_REPUTATION_NPC_COUNT_PER_PAGE; ++i )
			{
				if( pControl == m_apStaticEventReceiver[ i ] )
				{
					UpdateNpcSlotSelected( i );
					break;
				}
			}
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_REPUTATION);
		}
#endif // PRE_ADD_SHORTCUT_HELP_DIALOG
#ifdef PRE_ADD_REPUTATION_EXPOSURE
		else if( strstr(pControl->GetControlName(), "ID_SLOT_ITEM") && uMsg == WM_RBUTTONUP )
		{
			CDnQuickSlotButton *pPressedButton = static_cast<CDnQuickSlotButton*>(pControl);
			if(pPressedButton && pPressedButton->GetItemState() == ITEMSLOT_DISABLE)
				return;

			if(pPressedButton->IsRegist())
				return;

			CDnItem* pPushItem = static_cast<CDnItem*>(pPressedButton->GetItem());
			if(!pPushItem)
				return;

			if(pPushItem->GetItemType() == ITEMTYPE_NPC_VOICEPLAYER)
			{
				CDnLocalPlayerActor* pLocalPlayerActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
				if(pLocalPlayerActor)
					pLocalPlayerActor->UseNpcVoicePlayer(pPushItem);
			}
		}
#endif // PRE_ADD_REPUTATION_EXPOSURE

		// 콤보 박스 아닌 곳 클릭시 콤보 박스 포커스를 없애야 할듯 [2010/11/16 semozz]
		m_pComboSort->Focus(false);
		PopFocusControl();
	}
	else if (nCommand == EVENT_RADIOBUTTON_CHANGED && bTriggeredByUser)
	{
		m_pTextBoxNpcInfo->ClearText();
		m_FavorGiftUI.Show(false);
#ifdef PRE_ADD_REPUTATION_EXPOSURE
		m_RewardItemUI.Show(false);
		m_FavorGiftUI.pGiftTitle->ClearText();
		m_FavorGiftUI.pGiftTitle->Show(false);
		m_RewardItemUI.pGiftTitle->ClearText();
		m_RewardItemUI.pGiftTitle->Show(false);
#endif

		int selectedIdx = GetCurrentSelectedUISlotIdx();
		if (selectedIdx >= NPC_REPUTATION_NPC_COUNT_PER_PAGE)
			return;

		if (IsCmdControl( "ID_RBT_TAB0" ))
			UpdateNpcExplain(selectedIdx + (m_iNowPage * NPC_REPUTATION_NPC_COUNT_PER_PAGE));
		else if (IsCmdControl("ID_RBT_TAB1"))
			UpdateNpcFavorGift(selectedIdx + (m_iNowPage * NPC_REPUTATION_NPC_COUNT_PER_PAGE));
#ifdef PRE_ADD_REPUTATION_EXPOSURE
		else if( IsCmdControl("ID_RBT_TAB2"))
			UpdateNpcRewardGift(selectedIdx + (m_iNowPage * NPC_REPUTATION_NPC_COUNT_PER_PAGE));
#endif
	}
	else
	if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		if( IsCmdControl( "ID_COMBOBOX_SORT" ) )
		{
			// 처음 뜰 때 이쪽으로 와서 npc 초상화 텍스쳐 로드되기 전까진 막는다.
			if( m_hNpcFaceTexture )
			{
				CEtUIComboBox* pComboSort = static_cast<CEtUIComboBox*>( pControl );
#ifdef PRE_MOD_REPUTE_NOMALICE2
				pComboSort->GetSelectedValue(m_iNowSelectedSortMethod);
#else
				m_iNowSelectedSortMethod = pComboSort->GetSelectedIndex();
#endif

				_UpdateReputeNpcList(m_iNowSelectedSortMethod);
				UpdatePage( m_iNowPage );

				// 선택된 내용이 없도록 처리.
				m_iNowSelectedNpcSlotIndex = -1;
				UpdateNpcSlotSelected( m_iNowSelectedNpcSlotIndex );
			}
		}
	}
}


bool CDnNpcReputationDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
		case WM_MOUSEMOVE:
			{
				CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
				if( !pQuestTask ) 
					return false;

				CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
				if( !pReputationRepos )
					return false;

				float fMouseX = 0;
				float fMouseY = 0;
				POINT MousePoint;
				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );
				PointToFloat( MousePoint, fMouseX, fMouseY );

				bool bMouseEnter = false;
				float fReputationValue = 0.0f;
				float fReputationValueMax = 100.0f;
				for( int i = 0; i < NPC_REPUTATION_NPC_COUNT_PER_PAGE; i++ )
				{
					if( m_apProgressFavor[ i ]->IsInside( fMouseX, fMouseY ) && m_NpcIDs[ i ].bAvail )
					{
						bMouseEnter = true;
						fReputationValue = pReputationRepos->GetNpcReputationPercentF( m_NpcIDs[i].npcId, IReputationSystem::NpcFavor );
						//fReputationValueMax = (int)pReputationRepos->GetNpcReputationMax( m_aiNpcId[ i ], IReputationSystem::NpcFavor );
						break;
					}
#ifdef PRE_MOD_REPUTE_NOMALICE
#else
					else
					if( m_apProgressMalice[ i ]->IsInside( fMouseX, fMouseY ) && m_NpcIDs[ i ].bAvail )
					{
						bMouseEnter = true;
						fReputationValue = pReputationRepos->GetNpcReputationPercentF( m_NpcIDs[i].npcId, IReputationSystem::NpcMalice );
						//fReputationValueMax = (int)pReputationRepos->GetNpcReputationMax( m_aiNpcId[ i ], IReputationSystem::NpcMalice );
						break;
					}
#endif // PRE_MOD_REPUTE_NOMALICE
					else if (m_apTextureNpcUnion[i]->IsInside(fMouseX, fMouseY))
					{
						std::wstring str;
						const int& unionTableId = m_TextureNpcUnionTableIDs[i];
						if (unionTableId < 0)
						{
							str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3286); // UISTRING : 소속없음
						}
						else
						{
							DNTableFileFormat*  pUnionTable = GetDNTable(CDnTableDB::TUNION);
							if (pUnionTable)
							{
								int nameStringNum = pUnionTable->GetFieldFromLablePtr(unionTableId, "_UnionName")->GetInteger();
								str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nameStringNum);
							}
						}

						if (str.empty() == false)
						{
							CDnSimpleTooltipDlg* tooltipDlg = GetInterface().GetSimpleTooltipDialog();
							if (tooltipDlg)
								tooltipDlg->ShowTooltipDlg(m_apTextureNpcUnion[i], true, str, 0xffffffff, true);
						}
					}
				}

				if( !CDnMouseCursor::GetInstance().IsShowCursor() )
					bMouseEnter = false;				

				//ShowExpDialog( bExpMouseEnter, fMouseX, fMouseY );
				if( bMouseEnter )
				{
					if (CDnActor::s_hLocalActor && !m_pComboSort->IsOpenedDropDownBox())
					{
						m_pTooltipDlg->SetReputationPercent( fReputationValue, fReputationValueMax );
						ShowChildDialog( m_pTooltipDlg, true );

						SUICoord dlgCoord;
						m_pTooltipDlg->GetDlgCoord( dlgCoord );
						dlgCoord.fX = fMouseX + GetXCoord() - (dlgCoord.fWidth / 2.0f);
						dlgCoord.fY = fMouseY + GetYCoord() - dlgCoord.fHeight - 0.004f;
						m_pTooltipDlg->SetDlgCoord( dlgCoord );
					}
				}
				else
				{
					ShowChildDialog( m_pTooltipDlg, false );
				}
			}
			break;
		// 마우스 휠 기능 추가 [2010/11/16 semozz]
		case WM_MOUSEWHEEL:
			{
				//다이얼로그 위에 있어야 하고, 정렬콤보 박스가 포커스가 아닐때
				if ( IsMouseInDlg() && !m_pComboSort->IsFocus() )
				{
					UINT uLines;
					SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
					int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
					
					//휠을 내릴때 - : 다음 페이지, 올릴때 + : 이전 페이지
					if ( nScrollAmount != 0 )
					{
						RefreshPage( nScrollAmount < 0 );
						UpdateNpcSlotSelected(-1);
					}
				}
			}
			break;
	}

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	if (m_pComboSort->IsOpenedDropDownBox() && GetInterface().GetSimpleTooltipDialog() && GetInterface().GetSimpleTooltipDialog()->IsShow())
		GetInterface().GetSimpleTooltipDialog()->Show(false);

	return bRet;
}

void CDnNpcReputationDlg::RefreshPage(bool bInc)
{
	int tempCurrentPage = m_iNowPage;

	if ( false == bInc )
	{
		tempCurrentPage--;
		if( tempCurrentPage < 0 )
			tempCurrentPage = 0;
	}
	else
	{
		tempCurrentPage++;
		if( m_iMaxPage < tempCurrentPage )
			tempCurrentPage = m_iMaxPage;
	}

	if (tempCurrentPage != m_iNowPage)
	{
		m_iNowPage = tempCurrentPage;
		RefreshToCurrentPage();
	}
}

#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM