#include "StdAfx.h"
#include "DnPartyCreateDlg.h"
#include "DnPlayerActor.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DnTableDB.h"
#include "DnCommonUtil.h"
#include "DnGameTask.h"
#ifdef PRE_PARTY_DB
#include "DnMainMenuDlg.h"
#include <MMSystem.h>
#endif

#ifdef PRE_ADD_PREDICTIVE_TEXT
#include "DnLocalPlayerActor.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define MIN_PARTY_LEVEL 1
#define MAX_PARTY_LEVEL 100
#define NO_TARGET_STAGE_INDEX 0
#define NO_LIMIT_DIFFICULTY_INDEX 0

CDnPartyCreateDlg::CDnPartyCreateDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
#ifdef PRE_PARTY_DB
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
#else
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
#endif
	, m_pEditBoxPartyName(NULL)
	, m_pEditBoxPassword(NULL)
	, m_pEditBoxLevelMin(NULL)
	, m_pComboBoxCount(NULL)
	, m_pCheckBoxSecret(NULL)
#ifdef PRE_PARTY_DB
#else
	, m_pEditBoxLevelMax(NULL)
	, m_pCheckBoxUserLevel(NULL)
#endif
	, m_pButtonCreate(NULL)
	, m_pButtonClose(NULL)
	, m_pStaticTitle(NULL)
	, m_Type(PARTYREQ_NONE)
	, m_bDefaultPartyName(true)
	, m_pTargetStageComboBox(NULL)
	, m_pDifficultyComboBox(NULL)
	, m_nCurrentMapIdx(-1)
#ifdef PRE_PARTY_DB
	, m_nCurrentLocalPlayerLevel(0)
#endif
	, m_bIsUserChangeLimitLevel(false)
	, m_CurPartyType(_NORMAL_PARTY)
	, m_PartyCountMin(0)
	, m_PartyCountMax(0)
#ifdef PRE_PARTY_DB
	, m_pTargetStageEditBox(NULL)
	, m_pTargetStageFilterButton(NULL)
	, m_CurrentTargetStageFilter(eTARGETFILTER_ALL)
	, m_PreviousTargetStageFilter(eTARGETFILTER_ALL)
#endif
#ifdef PRE_ADD_PREDICTIVE_TEXT
	, m_bOpenStageWithMouse(false)
#endif
{
	int i = 0;
	for (; i < ePTYPE_MAX; ++i)
		m_pRadioBtnPartyType[i] = NULL;
}

CDnPartyCreateDlg::~CDnPartyCreateDlg(void)
{
}

void CDnPartyCreateDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PartyCreateDlg.ui" ).c_str(), bShow );

#ifdef PRE_PARTY_DB
	m_Random.srand(timeGetTime());
#endif
}

int CDnPartyCreateDlg::ConvertCreateTypeToPartyType(ePartyCreateType createType)
{
	switch(createType)
	{
	case ePTYPE_NORMAL: return _NORMAL_PARTY;
	case ePTYPE_RAID:	return _RAID_PARTY_8;
	default: return -1;
	}
	return -1;
}

int CDnPartyCreateDlg::ConvertPartyTypeToCreateType(ePartyType partyType)
{
	switch(partyType)
	{
	case _NORMAL_PARTY: return ePTYPE_NORMAL;
	case _RAID_PARTY_8:	return ePTYPE_RAID;
	default: return -1;
	}
	return -1;
}

void CDnPartyCreateDlg::InitialUpdate()
{
	m_pEditBoxPartyName = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_PARTYNAME");
	m_pEditBoxPassword = GetControl<CEtUIEditBox>("ID_EDITBOX_PASSWORD");

	m_pComboBoxCount = GetControl<CEtUIComboBox>("ID_COMBOBOX_COUNT");

	m_pCheckBoxSecret = GetControl<CEtUICheckBox>("ID_CHECKBOX_SECRET");
#ifdef PRE_PARTY_DB
#else
	m_pCheckBoxUserLevel = GetControl<CEtUICheckBox>("ID_CHECKBOX_USERLEVEL");
#endif
	int i = 0;
	for (; i < ePTYPE_MAX; ++i)
	{
		std::string ctrlName;
		ctrlName = FormatA("ID_RBT_%d", i);
		m_pRadioBtnPartyType[i] = GetControl<CEtUIRadioButton>(ctrlName.c_str());
		m_pRadioBtnPartyType[i]->SetButtonID(ConvertCreateTypeToPartyType((ePartyCreateType)i));
#ifdef PRE_FIX_PARTY_NO_RAID2
		if (i != ePTYPE_NORMAL)
			m_pRadioBtnPartyType[i]->Enable(false);
#endif
	}

#ifdef PRE_REMOVE_EU_CBTUI_1206
	CEtUIStatic* pText = GetControl<CEtUIStatic>("ID_TEXT1");
	if(pText) pText->Show(false);
	m_pRadioBtnPartyType[1]->Show(false);
#endif // PRE_REMOVE_EU_CBTUI_1206

#ifdef PRE_PARTY_DB
#else
	m_pEditBoxLevelMax = GetControl<CEtUIEditBox>("ID_EDITBOX_LV_MAX");
#endif
	m_pEditBoxLevelMin = GetControl<CEtUIEditBox>("ID_EDITBOX_LV_MIN");
#ifdef PRE_PARTY_DB
	m_pEditBoxLevelMin->Enable(true);
#endif

	m_pButtonCreate = GetControl<CEtUIButton>("ID_BUTTON_CREATE");
	m_pButtonClose = GetControl<CEtUIButton>("ID_BUTTON_CLOSE");

	m_pStaticTitle = GetControl<CEtUIStatic>("ID_STATIC_TITLE");

	CEtUIStatic* pWarnIcon	= GetControl<CEtUIStatic>("ID_STATIC12");
	CEtUIStatic* pWarnState = GetControl<CEtUIStatic>("ID_STATIC13");
	if (pWarnState && pWarnIcon)
	{
		m_pSecretWarnStaticList.push_back(pWarnIcon);
		m_pSecretWarnStaticList.push_back(pWarnState);
	}
	else
		_ASSERT(0);

	m_pTargetStageComboBox = GetControl<CEtUIComboBox>("ID_COMBOBOX_TSTAGE");

	

#ifdef PRE_PARTY_DB
	m_pTargetStageComboBox->SetEditMode(true);
	m_pTargetStageEditBox = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_TSTAGE");

	m_pTargetStageFilterButton = GetControl<CDnRotateToggleButton>("ID_BT_MODETOGGLE");
	m_pTargetStageFilterButton->RegisterRotate(eTARGETFILTER_ALL, 0, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 79)); // UISTRING : 모두
	m_pTargetStageFilterButton->RegisterRotate(eTARGETFILTER_DUNGEON, 0, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7110)); // UISTRING : 스테이지
	m_pTargetStageFilterButton->RegisterRotate(eTARGETFILTER_NEST, 0, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1372)); // UISTRING : 네스트
	#ifdef PRE_ADD_PARTYTARGETMAPTOGGLE
	m_pTargetStageFilterButton->RegisterRotate(eTARGETFILTER_ETC, 0, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2292)); // UISTRING : 기타
	#endif
	m_pTargetStageFilterButton->SetState(eTARGETFILTER_ALL, false, false, false);

	m_PreviousTargetStageFilter = eTARGETFILTER_ALL;
	m_CurrentTargetStageFilter = eTARGETFILTER_ALL;
#endif
	m_pDifficultyComboBox = GetControl<CEtUIComboBox>("ID_COMBOBOX_DIFFICULT");

	m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_BUTTON_CREATE") );
}

void CDnPartyCreateDlg::SetMode(ePartyReqType type)
{
	m_Type = type;
}

void CDnPartyCreateDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if (bShow)
		m_SmartMove.MoveCursor();
	else
		m_SmartMove.ReturnCursor();

	if( bShow )
	{
		CDnPartyTask *pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
		if( pPartyTask )
		{
			switch( pPartyTask->GetPartyRole() )
			{
			case CDnPartyTask::LEADER:	Init_Master();	break;
			case CDnPartyTask::SINGLE:	Init_Single();	break;
			case CDnPartyTask::MEMBER:	Init_Member();	break;
			}

			m_bIsUserChangeLimitLevel = false;
#ifdef PRE_PARTY_DB
			m_pTargetStageEditBox->ClearText();
			m_AutoCompleteTextCache.clear();
			bool bCurrentMapIndexChanged = false;
#endif

			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
			if (pSox != NULL && CGlobalInfo::GetInstance().m_nCurrentMapIndex != m_nCurrentMapIdx)
			{
				std::string areaString = pSox->GetFieldFromLablePtr(CGlobalInfo::GetInstance().m_nCurrentMapIndex, "_MapArea")->GetString();
				if (areaString.empty() == false)
				{
					m_pTargetStageComboBox->RemoveAllItems();
					m_pDifficultyComboBox->RemoveAllItems();
					m_pTargetStageComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3599), NULL, NO_TARGET_STAGE_INDEX, true);	// UISTRING : 제한없음
					m_pDifficultyComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3599), NULL, NO_LIMIT_DIFFICULTY_INDEX);	// UISTRING : 제한없음

					m_pTargetStageComboBox->Enable(true);
					m_pDifficultyComboBox->Enable(false);

					m_nCurrentMapIdx = CGlobalInfo::GetInstance().m_nCurrentMapIndex;

#ifdef PRE_PARTY_DB
					bCurrentMapIndexChanged = true;
#endif
				}

				m_MapAreaNameListForSortCache.clear();

#ifdef PRE_ADD_PREDICTIVE_TEXT
				m_MapAreaNameListbyKey.clear();
#endif

#ifdef PRE_PARTY_DB
				CDnPartyTask::GetInstance().MakePartyTargetStageList(CGlobalInfo::GetInstance().m_nCurrentMapIndex, m_MapAreaNameListForSortCache, true);
				CDnPartyTask::GetInstance().MakePartyTargetStageLevelLimitList(m_nMapLimitLevelList, m_MapAreaNameListForSortCache, true);
				
#ifdef PRE_ADD_PREDICTIVE_TEXT
				m_AutoCompleteMgr.Clear();
				m_AutoCompleteMgr.AddTextUnit(m_MapAreaNameListForSortCache);
				for( int i =0; i < (int)m_MapAreaNameListForSortCache.size(); i++ )
				{
					const SPartyTargetMapInfo &MapInfo =  m_MapAreaNameListForSortCache[i];
					m_MapAreaNameListbyKey.insert( make_pair( MapInfo.mapName[0], MapInfo ) );	
				}
#else
				m_AutoCompleteMgr.AddTextUnit(m_MapAreaNameListForSortCache);
#endif
			
#else
				CDnPartyTask::GetInstance().MakePartyTargetStageList(CGlobalInfo::GetInstance().m_nCurrentMapIndex, m_MapAreaNameListForSortCache, m_nMapLimitLevelList);
#endif
			}

			if (m_Type == PARTYREQ_CREATE)
			{
				SetDifficulty();

				GetPartyTask().SetPartyTargetStageIdx(0);
				GetPartyTask().SetPartyTargetStageDifficulty(0);

				m_pRadioBtnPartyType[ePTYPE_NORMAL]->SetChecked(true);
	#ifdef PRE_FIX_PARTY_NO_RAID2
				int i = 0;
				for (; i < ePTYPE_MAX; ++i)
				{
					if (i != ePTYPE_NORMAL)
						m_pRadioBtnPartyType[i]->Enable(false);
				}
	#else
				int i = 0;
				for (; i < ePTYPE_MAX; ++i)
					m_pRadioBtnPartyType[i]->Enable(true);

				int curChannel = CDnBridgeTask::GetInstance().GetChannelAttribute();
	#ifdef PRE_MOD_ENABLE_DARKLAIR_RAID
				if (curChannel & (CHANNEL_ATT_PVP|CHANNEL_ATT_PVPLOBBY|CHANNEL_ATT_GM|CHANNEL_ATT_FARMTOWN))
	#else
				if (curChannel & (CHANNEL_ATT_PVP|CHANNEL_ATT_PVPLOBBY|CHANNEL_ATT_GM|CHANNEL_ATT_DARKLAIR|CHANNEL_ATT_FARMTOWN))
	#endif
	#endif // PRE_FIX_PARTY_NO_RAID2
					m_pRadioBtnPartyType[ePTYPE_RAID]->Enable(false);
			}
			else
			if (m_Type == PARTYREQ_MODIFY)
			{
				ePartyType curType = CDnPartyTask::GetInstance().GetPartyType();
				int createType = ConvertPartyTypeToCreateType(curType);
				if (createType >= 0 && createType < ePTYPE_MAX)
					m_pRadioBtnPartyType[createType]->SetChecked(true);

				int i = 0;
				for (; i < ePTYPE_MAX; ++i)
					m_pRadioBtnPartyType[i]->Enable(false);

				if (pPartyTask->GetPartyRole() == CDnPartyTask::LEADER)
				{
					m_pDifficultyComboBox->Enable(GetPartyTask().GetPartyTargetStageIdx() > 0);
				}

				SetDifficulty();

#ifdef PRE_PARTY_DB
				SetTargetStage(GetPartyTask().GetPartyTargetStageIdx());
#else
				m_pTargetStageComboBox->SetSelectedByValue(GetPartyTask().GetPartyTargetStageIdx());
#endif
				m_pDifficultyComboBox->SetSelectedByValue(GetPartyTask().GetPartyTargetStageDifficulty());
			}

#ifdef PRE_MOD_PARTY_CREATE_SORT
			ReArrangeTargetStageCombo(GetPartyTask().GetPartyTargetStageIdx());
#else
	#ifdef PRE_PARTY_DB
			ReArrangeTargetStageCombo(GetPartyCount(), GetPartyTask().GetPartyTargetStageIdx());
	#else
			ReArrangeTargetStageComboByMemberCount(GetPartyCount());
	#endif
#endif
		}
	}

	CEtUIDialog::Show(bShow);
}

#ifdef PRE_PARTY_DB
void CDnPartyCreateDlg::CheckAndSetMapLevelLimit()
{
	DNTableFileFormat* pGateSox = GetDNTable( CDnTableDB::TMAPGATE );
	if (pGateSox == NULL)
		return;

	int mapLimitLevel = GetMapLevelLimit();
	if (mapLimitLevel > MIN_PARTY_LEVEL)
	{
		m_pEditBoxLevelMin->SetIntToText(mapLimitLevel);
	}
	else
	{
		m_pEditBoxLevelMin->SetIntToText(MIN_PARTY_LEVEL);
	}
}

void CDnPartyCreateDlg::SetDefaultPartyTemplate(bool bMakeAbyssMapList)
{
	if (m_MapAreaNameListForSortCache.empty() || m_nMapLimitLevelList.empty())
		return;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	DNTableFileFormat* pDungeonSox = GetDNTable( CDnTableDB::TDUNGEONENTER );
	if (pSox == NULL || pDungeonSox == NULL)
		return;

	if (bMakeAbyssMapList)
	{
		if (!CDnActor::s_hLocalActor)
			return;
		DnActorHandle hActor = CDnActor::s_hLocalActor;
		CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(static_cast<CDnActor*>(hActor));
		if (pPlayerActor == NULL)
			return;

		int userLevel = pPlayerActor->GetLevel();
		m_AbyssMapListForDefaultTemplate.clear();
		int minLevelGap = INT_MAX;
		std::map<int, int>::const_iterator iter = m_nMapLimitLevelList.begin();
		for (; iter != m_nMapLimitLevelList.end(); ++iter)
		{
			int mapId = (*iter).first;

			int nDungeonEnterTableID = pSox->GetFieldFromLablePtr(mapId, "_EnterConditionTableID")->GetInteger();
			if (nDungeonEnterTableID > 0)
			{
				if (pDungeonSox->IsExistItem(nDungeonEnterTableID))
				{
					int abyssMinLevel = pDungeonSox->GetFieldFromLablePtr( nDungeonEnterTableID, "_AbyssLvlMin" )->GetInteger();
					if (userLevel >= abyssMinLevel)
					{
						int currentLevelGap = userLevel - abyssMinLevel;
						if (currentLevelGap < minLevelGap)
						{
							m_AbyssMapListForDefaultTemplate.clear();
							minLevelGap = currentLevelGap;
							m_AbyssMapListForDefaultTemplate.push_back(mapId);
						}
						else if (currentLevelGap == minLevelGap)
						{
							m_AbyssMapListForDefaultTemplate.push_back(mapId);
						}
					}
				}
			}
		}

		m_nCurrentLocalPlayerLevel = userLevel;
	}

	if (m_AbyssMapListForDefaultTemplate.empty())
		return;

	int randMapIndex = m_Random.rand(0, (int)m_AbyssMapListForDefaultTemplate.size() - 1);
	int nMapIndexChosen = m_AbyssMapListForDefaultTemplate[randMapIndex];

	SetTargetStage(nMapIndexChosen);
	OnChangeTargetStage(Dungeon::Difficulty::Abyss);
	m_pDifficultyComboBox->SetSelectedByValue(Dungeon::Difficulty::Abyss);
}
#endif // PRE_PARTY_DB

#ifdef PRE_PARTY_DB
void CDnPartyCreateDlg::GetTargetStageIdx(int& mapIdx, TDUNGEONDIFFICULTY& mapDifficulty)
#else
void CDnPartyCreateDlg::GetTargetStageIdx(int& mapIdx, int& mapDifficulty)
#endif
{
	SComboBoxItem* pItem = m_pTargetStageComboBox->GetSelectedItem();
	if (pItem && pItem->nValue != 0)
		mapIdx = pItem->nValue;

	pItem = m_pDifficultyComboBox->GetSelectedItem();
	if (pItem && pItem->nValue != 0)
#ifdef PRE_PARTY_DB
		mapDifficulty = (TDUNGEONDIFFICULTY)pItem->nValue;
#else
		mapDifficulty = pItem->nValue;
#endif
}

#ifdef PRE_PARTY_DB
int CDnPartyCreateDlg::GetMapLevelLimit() const
{
	int mapLimitLevel = -1;
	DNTableFileFormat* pGateSox = GetDNTable( CDnTableDB::TMAPGATE );
	if (pGateSox == NULL)
		return -1;

	int itemId = pGateSox->GetItemIDFromField("_MapIndex", CGlobalInfo::GetInstance().m_nCurrentMapIndex);
	if (itemId != -1)
	{
		DNTableFileFormat::Cell* pCell = pGateSox->GetFieldFromLablePtr(itemId, "_PermitClassLevel");
		if (pCell== NULL)
			return -1;
		mapLimitLevel = pCell->GetInteger();
		CommonUtil::ClipNumber(mapLimitLevel, MIN_PARTY_LEVEL, mapLimitLevel);
	}

	return mapLimitLevel;
}
#endif

bool CDnPartyCreateDlg::DoCreateParty(ePartyReqType type)
{
#ifdef PRE_PARTY_DB
	std::wstring wszPartyName;
	int partyPassword = Party::Constants::INVALID_PASSWORD;
#else
	std::wstring wszPartyName, wszPartyPassword;
#endif

	wszPartyName = GetPartyName();
	if( wszPartyName.empty() )
	{
		GetInterface().MessageBox( MESSAGEBOX_15, MB_OK );
		return false;
	}

	if( DN_INTERFACE::UTIL::CheckChat( wszPartyName ) )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3534 ), MB_OK );
		return false;
	}

#ifdef PRE_PARTY_DB
	int mapLevelMin = GetMapLevelLimit();
	SComboBoxItem* pItem = m_pTargetStageComboBox->GetSelectedItem();
	if (pItem && pItem->nValue != 0)
	{
		int difficultyLevel = -1;
		m_pDifficultyComboBox->GetSelectedValue( difficultyLevel );
		mapLevelMin = GetStageMinLevel( pItem->nValue, difficultyLevel );
	}
	//if( pItem->strText )
	
	int curLevelMin = m_pEditBoxLevelMin->GetTextToInt();
	if (mapLevelMin != -1 && curLevelMin < mapLevelMin)
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3473 ), MB_OK); // UISTRING : 제한 레벨이 스테이지 입장 레벨보다 낮습니다.
		m_pEditBoxLevelMin->SetIntToText(mapLevelMin);
		return false;
	}
#endif

	if( IsPartySecret() )
	{
#ifdef PRE_PARTY_DB
		partyPassword = GetPartyPassword();
#else
		wszPartyPassword = GetPartyPassword();
#endif
		CDnPartyTask *pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
		if (pPartyTask == NULL)
		{
			GetInterface().MessageBox( MESSAGEBOX_16, MB_OK );
			return false;
		}

#ifdef PRE_PARTY_DB
		if( partyPassword == Party::Constants::INVALID_PASSWORD || (CommonUtil::GetCipherDecimal(partyPassword) < pPartyTask->GetMaxPasswordLength()) )
#else
		if( wszPartyPassword.empty() || ((int)wszPartyPassword.size() < pPartyTask->GetMaxPasswordLength()) )
#endif
		{
			GetInterface().MessageBox( MESSAGEBOX_35, MB_OK );
			return false;
		}
	}

#ifdef PRE_PARTY_DB
	int nMinLv = MIN_PARTY_LEVEL;
	if (GetUserLevel(nMinLv))
#else
	int nMinLv, nMaxLv;
	nMinLv = MIN_PARTY_LEVEL;
	nMaxLv = MAX_PARTY_LEVEL;
	if (GetUserLevel(nMinLv, nMaxLv))
#endif
	{
		if (type == PARTYREQ_CREATE)
		{
#ifdef PRE_PARTY_DB
			SPartyCreateParam param;
			GetTargetStageIdx(param.nMapIdx, param.Difficulty);
			param.bAllowWorldZonePartyList = (DEFAULT_PARTY_LIST_ALLOW_WORLDZONE != 0);
			param.PartyType = m_CurPartyType;
			param.cPartyMemberMax = (BYTE)GetPartyCount();
			param.nUserLvLimitMin = nMinLv;
			param.wszPartyName = wszPartyName;
			param.iPassword = IsPartySecret() ? partyPassword : Party::Constants::INVALID_PASSWORD;
			param.cIsPartyDice = 1;

			GetPartyTask().ReqCreateParty(param);
#else
			int mapIdx, difficulty;
			mapIdx = difficulty = 0;

			GetTargetStageIdx(mapIdx, difficulty);

			GetPartyTask().ReqCreateParty((BYTE)m_CurPartyType, (BYTE)GetPartyCount(),
				nMinLv, nMaxLv, wszPartyName.c_str(),
				IsPartySecret() ? wszPartyPassword.c_str() : NULL,
				mapIdx, difficulty, 1
				);
#endif
		}
		else if (type == PARTYREQ_MODIFY)
		{
#ifdef PRE_PARTY_DB
			int mapIdx = 0;
			TDUNGEONDIFFICULTY difficulty = Dungeon::Difficulty::Easy;
			GetTargetStageIdx(mapIdx, difficulty);

			if (GetPartyTask().IsPartyInfoModified(GetPartyCount(), nMinLv, GetPartyName(), GetPartyPassword(), mapIdx, difficulty))
				GetPartyTask().ReqPartyInfoModify( GetPartyCount(), GetPartyTask().GetPartyLootRule(), GetPartyTask().GetPartyLootItemRank(), nMinLv, GetPartyName(), GetPartyPassword(), mapIdx, difficulty, GetPartyTask().IsPartyAllowWorldZoneMapList() );
#else
			int mapIdx, difficulty;
			mapIdx = difficulty = 0;
			GetTargetStageIdx(mapIdx, difficulty);

			if (GetPartyTask().IsPartyInfoModified(GetPartyCount(), nMinLv, nMaxLv, GetPartyName(), GetPartyPassword(), mapIdx, difficulty))
				GetPartyTask().ReqPartyInfoModify( GetPartyCount(), GetPartyTask().GetPartyLootRule(), GetPartyTask().GetPartyLootItemRank(), nMinLv, nMaxLv, GetPartyName(), GetPartyPassword(), mapIdx, difficulty );
#endif
		}
		else
		{
			_ASSERT(0);
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void CDnPartyCreateDlg::SetPartyMinLevelFixed(int difficultyLevel)
{
#ifdef PRE_PARTY_DB
	int levelComplemented = difficultyLevel;
	int mapLevelLimit = GetMapLevelLimit();
	if (mapLevelLimit != -1 && difficultyLevel < mapLevelLimit)
		levelComplemented = mapLevelLimit;

	m_pEditBoxLevelMin->SetIntToText(levelComplemented);
#else
	if (IsPartyLevelLimit() == false)
	{
		m_pEditBoxLevelMin->Enable( true );
		m_pEditBoxLevelMax->Enable( true );
	}

	if (m_bIsUserChangeLimitLevel == false)
	{
		m_pEditBoxLevelMin->SetIntToText(difficultyLevel);
	}
	else
	{
		if (m_pEditBoxLevelMin->GetTextToInt() < difficultyLevel)
			m_pEditBoxLevelMin->SetIntToText(difficultyLevel);
	}
#endif
}

void CDnPartyCreateDlg::SetPartyMinLevel(int mapId, int difficultyLevel)
{
	if (mapId != -1)
	{
		int limitLevel = GetStageMinLevel( mapId, difficultyLevel );
#ifdef PRE_PARTY_DB
		SetPartyMinLevelFixed(limitLevel);
#else
		if( 0 < limitLevel )
			SetPartyMinLevelFixed(limitLevel);
#endif
	}
}

#ifdef PRE_PARTY_DB
int CDnPartyCreateDlg::GetStageMinLevel( int mapId, int difficultyLevel ) const
#else
int CDnPartyCreateDlg::GetStageMinLevel( int mapId, int difficultyLevel )
#endif
{
	int iResult = 0;

	std::map<int, int>::const_iterator iter = m_nMapLimitLevelList.find(mapId);
	if (iter != m_nMapLimitLevelList.end())
	{
#ifdef PRE_PARTY_DB
		if (difficultyLevel == Dungeon::Difficulty::Abyss)
#else
		if (difficultyLevel == eDIFFCLT_ABYSS)
#endif
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
			int nDungeonEnterTableID = pSox->GetFieldFromLablePtr(mapId, "_EnterConditionTableID")->GetInteger();
			if (nDungeonEnterTableID > 0)
			{
				DNTableFileFormat* pDungeonSox = GetDNTable( CDnTableDB::TDUNGEONENTER );
				if (pDungeonSox->IsExistItem( nDungeonEnterTableID ))
				{
					int abyssMinLevel = pDungeonSox->GetFieldFromLablePtr( nDungeonEnterTableID, "_AbyssLvlMin" )->GetInteger();
#ifdef PRE_PARTY_DB
#else
					SetPartyMinLevelFixed(abyssMinLevel);
#endif
					return abyssMinLevel;
				}
			}
		}

		iResult = (*iter).second;
	}

	return iResult;
}


void CDnPartyCreateDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

#ifdef PRE_PARTY_DB
	if (IsCmdControl("ID_IMEEDITBOX_TSTAGE"))
	{
		if( nCommand == EVENT_EDITBOX_IME_CHANGE )
		{
			std::wstring curText = m_pTargetStageEditBox->GetText();
			int curEditBoxCaretPos = m_pTargetStageEditBox->GetCaretPos();
			curText = curText.substr(0, curEditBoxCaretPos );	

			std::wstring composeStr;
			m_pTargetStageEditBox->GetIMECompositionString(composeStr);
			std::wstring temp = FormatW(L"[AutoText] EVENT_EDITBOX_IME_CHANGE : curText:%s / composeStr:%s\n", curText.c_str(), composeStr.c_str());
			OutputDebugStringW(temp.c_str());
			curText += composeStr;	

#ifdef PRE_ADD_PREDICTIVE_TEXT
			OnAddEditBoxString( m_pTargetStageEditBox->GetText() );
#else
			OnAddEditBoxString( curText );	
#endif 
			return;
		} 
		else if (nCommand == EVENT_EDITBOX_CHANGE)
		{
			WCHAR ch = (WCHAR)uMsg;
			if (CommonUtil::IsCtrlChar(ch) == false)
			{
				std::wstring curText = m_pTargetStageEditBox->GetText();
				std::wstring temp = FormatW(L"[AutoText] EVENT_EDITBOX_CHANGE : curText:%s\n", curText.c_str());
				OutputDebugStringW(temp.c_str());

#ifdef PRE_ADD_PREDICTIVE_TEXT
				// EditBox -> ComboBox 의 강제 호출이라 Flag를 하나 두어 Edit -> Combo 통신이란걸 식별합니다. 
				if( OnAddComboBoxTargetStageList( GetPartyCount(), curText ) ) {
					// m_bEditBoxToComboBox 위치 유의 하자. ToggleDropDownList 가 먼저 호출되면 ProcessCommand를 호출하기 때문에 
					// m_bEditBoxToComboBox = true 의 대입이 이루어 지지 않음 
					m_bOpenStageWithMouse = true;
					m_pTargetStageComboBox->ToggleDropDownList();
				}
				//OnAddEditBoxString(curText);

#else 
				OnAddEditBoxString( curText );
#endif 

			}
			return;
		}
		else if (nCommand == EVENT_EDITBOX_ESCAPE)
		{
			CDnMainMenuDlg* pDlg = GetInterface().GetMainMenuDialog();
			if (pDlg)
				pDlg->CloseMenuDialog();
			return;
		}
		else if (nCommand != EVENT_EDITBOX_CHANGE)
		{
			if (nCommand == EVENT_EDITBOX_FOCUS || nCommand == EVENT_EDITBOX_RELEASEFOCUS)
			{				
				if (m_pTargetStageEditBox->IsFocusEditBox())
					m_pTargetStageEditBox->ClearText();

				CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
				return;
			}

			if (m_AutoCompleteTextCache != L"")
			{
				m_pTargetStageEditBox->ClearText();
				m_pTargetStageEditBox->SetText(m_AutoCompleteTextCache.c_str());
				m_AutoCompleteTextCache = L"";
			}
			return;
		}
	}
#endif

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_CREATE" ) )
		{
			if( GetPartyTask().GetPartyRole() == CDnPartyTask::LEADER )
			{
				if (DoCreateParty(PARTYREQ_MODIFY) == false)
					return;
			}
		}

#ifdef PRE_PARTY_DB
		if (IsCmdControl("ID_BT_MODETOGGLE"))
		{
			m_CurrentTargetStageFilter = (eTargetStageFilterType)uMsg;

#ifdef PRE_MOD_PARTY_TOGGLE_OPT
			m_pTargetStageComboBox->RemoveAllItems();
			//m_pTargetStageComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3599), NULL, NO_TARGET_STAGE_INDEX);	// UISTRING : 제한없음

			if (m_PreviousTargetStageFilter != m_CurrentTargetStageFilter)
			{
				int nMapId = 0;
				m_pTargetStageComboBox->GetSelectedValue(nMapId);
#ifdef PRE_MOD_PARTY_CREATE_SORT
				ReArrangeTargetStageCombo(nMapId);
#else
				ReArrangeTargetStageCombo(GetPartyCount(), nMapId);
#endif
				m_PreviousTargetStageFilter = m_CurrentTargetStageFilter;
			}
#endif  // PRE_MOD_PARTY_TOGGLE_OPT 

			SetTargetStage(NO_TARGET_STAGE_INDEX);
			OnChangeTargetStage();
			return;
		}
#endif
	}
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl("ID_CHECKBOX_SECRET" ) )
		{
			bool bCheck = m_pCheckBoxSecret->IsChecked();
			OnSetSecretCheckbox(bCheck);
			return;
		}

#ifdef PRE_PARTY_DB
#else
		if( IsCmdControl("ID_CHECKBOX_USERLEVEL" ) )
		{
			bool bCheck = m_pCheckBoxUserLevel->IsChecked();
			m_pEditBoxLevelMin->Enable( bCheck );
			m_pEditBoxLevelMax->Enable( bCheck );
			RequestFocus(m_pEditBoxLevelMin);
			return;
		}
#endif
	}
#ifdef PRE_PARTY_DB
	else if (nCommand == EVENT_COMBOBOX_DROPDOWN_OPENED && bTriggeredByUser)
	{
		if (IsCmdControl("ID_COMBOBOX_TSTAGE"))
		{
#ifdef PRE_ADD_PREDICTIVE_TEXT		
			CDnLocalPlayerActor::LockInput( true );		

			if ( ! m_bOpenStageWithMouse  )
			{
				int nMapId = 0;
				m_pTargetStageComboBox->GetSelectedValue(nMapId);
#ifdef PRE_MOD_PARTY_CREATE_SORT
				ReArrangeTargetStageCombo(nMapId);
#else
				ReArrangeTargetStageCombo(GetPartyCount(), nMapId);
#endif
				m_PreviousTargetStageFilter = m_CurrentTargetStageFilter;

			}
#else	
			if (m_PreviousTargetStageFilter != m_CurrentTargetStageFilter)
			{
				int nMapId = 0;
				m_pTargetStageComboBox->GetSelectedValue(nMapId);
#ifdef PRE_MOD_PARTY_CREATE_SORT
				ReArrangeTargetStageCombo(nMapId);
#else
				ReArrangeTargetStageCombo(GetPartyCount(), nMapId);
#endif
				m_PreviousTargetStageFilter = m_CurrentTargetStageFilter;
			}

#endif // PRE_ADD_PREDICTIVE_TEXT


#ifdef PRE_ADD_PREDICTIVE_TEXT
			m_bOpenStageWithMouse = false;
#endif
		}
	}
#endif
	else if (nCommand == EVENT_COMBOBOX_SELECTION_CHANGED && bTriggeredByUser)
	{
		if (IsCmdControl("ID_COMBOBOX_TSTAGE"))
		{
#ifdef PRE_PARTY_DB

			OnChangeTargetStage();

			if (m_pTargetStageComboBox->IsOpenedDropDownBox() == false)
			{
				SComboBoxItem* pItem = m_pTargetStageComboBox->GetSelectedItem();
				if (pItem)
				{
					m_pTargetStageEditBox->ClearText();
					m_pTargetStageEditBox->SetTextWithEllipsis(pItem->strText, L"...");
				}
			}
			
#ifdef PRE_ADD_PREDICTIVE_TEXT
			CDnLocalPlayerActor::LockInput( false );
#endif

#else
			SComboBoxItem* pItem = m_pTargetStageComboBox->GetSelectedItem();
			if (pItem && pItem->nValue != 0)
			{
				m_pDifficultyComboBox->Enable(true);
				int difficultyLevel = -1;
				m_pDifficultyComboBox->GetSelectedValue(difficultyLevel);
				SetPartyMinLevel(pItem->nValue, difficultyLevel);
			}
			else
			{
				m_pDifficultyComboBox->SetSelectedByValue(0);
				m_pDifficultyComboBox->Enable(false);
			}
#endif
		}
		else if (IsCmdControl("ID_COMBOBOX_DIFFICULT"))
		{
			SComboBoxItem* pItem = m_pDifficultyComboBox->GetSelectedItem();
			if (pItem)
			{
				int mapId = -1;
				m_pTargetStageComboBox->GetSelectedValue(mapId);
				SetPartyMinLevel(mapId, pItem->nValue);
			}
		}
 
	}
#ifdef PRE_ADD_PREDICTIVE_TEXT
	else if( nCommand == EVENT_COMBOBOX_DROPDOWN_CLOSED )
	{
		CDnLocalPlayerActor::LockInput( false );
	}
#endif
	else if (nCommand == EVENT_EDITBOX_CHANGE )
	{
		if( IsCmdControl("ID_EDITBOX_LV_MIN" ) )
		{
			int nMinLv = m_pEditBoxLevelMin->GetTextToInt();
			int nPlayerLv = CDnActor::s_hLocalActor->GetLevel();

			if( nMinLv > nPlayerLv )
			{
				nMinLv = nPlayerLv;
				m_pEditBoxLevelMin->SetIntToText( nMinLv );
			}
			else if( nMinLv >= 0 && nMinLv < MIN_PARTY_LEVEL)
			{
				nMinLv = MIN_PARTY_LEVEL;
				m_pEditBoxLevelMin->SetIntToText( nMinLv );
			}

			if (bTriggeredByUser)
				m_bIsUserChangeLimitLevel = true;

			return;
		}

#ifdef PRE_PARTY_DB
#else
		if( IsCmdControl("ID_EDITBOX_LV_MAX" ) )
		{
			int nMaxLv = m_pEditBoxLevelMax->GetTextToInt();
			int nPlayerLv = CDnActor::s_hLocalActor->GetLevel();

			if( nMaxLv > MAX_PARTY_LEVEL )
			{
				nMaxLv = MAX_PARTY_LEVEL;
				m_pEditBoxLevelMax->SetIntToText( nMaxLv );
			}
			else if( nMaxLv >= 0 && nMaxLv < MIN_PARTY_LEVEL)
			{
				nMaxLv = MIN_PARTY_LEVEL;
				m_pEditBoxLevelMin->SetIntToText( nMaxLv );
			}

			return;
		}
#endif

		if (IsCmdControl("ID_IMEEDITBOX_PARTYNAME"))
		{
			if (m_bDefaultPartyName)
				m_bDefaultPartyName = false;
		}
	}
	else if (nCommand == EVENT_EDITBOX_IME_CHANGE)
	{
		if (IsCmdControl("ID_IMEEDITBOX_PARTYNAME"))
		{
			if (m_bDefaultPartyName)
			{
				m_pEditBoxPartyName->ClearText();
				m_bDefaultPartyName = false;
			}
		}
	}
	else
		if (nCommand == EVENT_RADIOBUTTON_CHANGED)
		{
			if (strstr(pControl->GetControlName(), "ID_RBT_"))
			{
				int type = static_cast<CEtUIRadioButton*>(pControl)->GetButtonID();
				if (type != (int)m_CurPartyType)
				{
					m_CurPartyType = (ePartyType)type;
					OnChangePartyType();
				}
			}
		}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

#define DEFAULT_RAID_MIN_LEVEL 40
void CDnPartyCreateDlg::OnChangePartyType()
{
#ifdef PRE_MOD_PARTY_CREATE_SORT
	SetComboBoxCountRange(GetPartyMinCountByMode(), GetPartyMaxCountByMode());
	SetComboBoxCount(GetPartyMaxCountByMode());

	SetDifficulty();
	GetPartyTask().SetPartyTargetStageIdx(0);
	SetTargetStage(GetPartyTask().GetPartyTargetStageIdx());
#endif
	if (m_CurPartyType == _NORMAL_PARTY)
	{
#ifdef PRE_MOD_PARTY_CREATE_SORT
#else
		SetComboBoxCountRange(NORMPARTYCOUNTMIN, NORMPARTYCOUNTMAX);
		SetComboBoxCount(NORMPARTYCOUNTMAX);

		SetDifficulty();
#endif

#ifdef PRE_MOD_PARTY_CREATE_SORT
#else
	#ifdef PRE_PARTY_DB
		SetTargetStage(GetPartyTask().GetPartyTargetStageIdx());
	#else
		m_pTargetStageComboBox->SetSelectedByValue(GetPartyTask().GetPartyTargetStageIdx());
	#endif
#endif
		m_pTargetStageComboBox->Enable(true);
		m_pDifficultyComboBox->Enable(GetPartyTask().GetPartyTargetStageIdx() > 0);

		SetPartyMinLevelFixed(1);
#ifdef PRE_PARTY_DB
#else
		m_pCheckBoxUserLevel->SetChecked(false);
#endif
	}
	else if (m_CurPartyType == _RAID_PARTY_8)
	{
#ifdef PRE_MOD_PARTY_CREATE_SORT
#else
		SetComboBoxCountRange(RAIDPARTYCOUNTMIN, RAIDPARTYCOUNTMAX);
		SetComboBoxCount(RAIDPARTYCOUNTMAX);

		SetDifficulty();
#endif

		SetPartyMinLevelFixed(DEFAULT_RAID_MIN_LEVEL);
#ifdef PRE_PARTY_DB
#else
		m_pCheckBoxUserLevel->SetChecked(true);
#endif
	}
}

void CDnPartyCreateDlg::SetComboBoxCountRange(int min, int max)
{
	m_PartyCountMin = min;
	m_PartyCountMax = max;
}

#ifdef PRE_MOD_PARTY_CREATE_SORT
void CDnPartyCreateDlg::ReArrangeTargetStageCombo(int curSelectedMapIdx)
#else
void CDnPartyCreateDlg::ReArrangeTargetStageCombo(int nCount, int curSelectedMapIdx)
#endif
{
// 	int curSelectedMapIdx = GetPartyTask().GetPartyTargetStageIdx();
	bool bCurSelectedMapIdx = false;
	m_pTargetStageComboBox->RemoveAllItems();

	m_pTargetStageComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3599), NULL, NO_TARGET_STAGE_INDEX, true);	// UISTRING : 제한없음

	std::vector<SPartyTargetMapInfo>::const_iterator iter = m_MapAreaNameListForSortCache.begin();
	for (; iter != m_MapAreaNameListForSortCache.end(); ++iter)
	{
		const SPartyTargetMapInfo& info = (*iter);

		bool bAddItem = false;
		if (info.IsValid())
		{
			if (info.minPartyNum < 0 && info.maxPartyNum < 0)
			{
				bAddItem = true;
			}
			else
			{
#ifdef PRE_MOD_PARTY_CREATE_SORT
				bAddItem = (info.minPartyNum < 0 || (info.minPartyNum >= GetPartyMinCountByMode() && info.minPartyNum <= GetPartyMaxCountByMode()));
				if (bAddItem)
					bAddItem = (info.maxPartyNum < 0 || (info.maxPartyNum >= GetPartyMinCountByMode() && info.maxPartyNum <= GetPartyMaxCountByMode()));
#else
				bAddItem = (info.minPartyNum < 0 || (info.minPartyNum >= 0 && info.minPartyNum <= nCount));
				if (bAddItem)
					bAddItem = (info.maxPartyNum < 0 || (info.maxPartyNum >= 0 && info.maxPartyNum >= nCount));
#endif
			}

			if (GetCurrentStageFilter() != eTARGETFILTER_ALL && GetCurrentStageFilter() != info.filterType)
				bAddItem = false;

			if (bAddItem)
			{
				m_pTargetStageComboBox->AddItem(info.mapName.c_str(), NULL, info.mapId, true);
				if (curSelectedMapIdx == info.mapId)
					bCurSelectedMapIdx = true;
			}
		}
	}

	if (m_MapAreaNameListForSortCache.empty() == false)
	{
		int nSelectedMapIndex = (bCurSelectedMapIdx) ? curSelectedMapIdx : 0;
		SetTargetStage(nSelectedMapIndex);
	}
}

// Note : 파티인원을 설정한다.
//		어떤 조건이나 상황에 따라 4명이하 또는 8명 이하로 설정가능
//
void CDnPartyCreateDlg::SetComboBoxCount( int nCount )
{
	m_pComboBoxCount->RemoveAllItems();
	std::wstring str;

	CommonUtil::ClipNumber(nCount, m_PartyCountMin, m_PartyCountMax);
	int i = m_PartyCountMin;
	for (; i <= m_PartyCountMax; i++)
	{
		str = FormatW(L"%d", i);
		m_pComboBoxCount->AddItem(str.c_str(), NULL, i);
	}

	m_pComboBoxCount->SetSelectedByValue(nCount);

#ifdef PRE_MOD_PARTY_CREATE_SORT
#else
	#ifdef PRE_PARTY_DB
	ReArrangeTargetStageCombo(nCount, 0);
	#else
	ReArrangeTargetStageComboByMemberCount(nCount);
	#endif
#endif // PRE_MOD_PARTY_CREATE_SORT
}

// Note : 파티의 월드 레벨은 1~파티장의 월드레벨 사이의 값이 될 수 있다.
//		설정한 값보다 낮은 유저는 입장할 수 없다.
//

LPCWSTR CDnPartyCreateDlg::GetPartyName() const
{
	return m_pEditBoxPartyName->GetText();
}

#ifdef PRE_PARTY_DB
int CDnPartyCreateDlg::GetPartyPassword() const
{
	return _wtoi(m_pEditBoxPassword->GetText());
}
#else
LPCWSTR CDnPartyCreateDlg::GetPartyPassword() const
{
	return m_pEditBoxPassword->GetText();
}
#endif

bool CDnPartyCreateDlg::IsPartySecret()
{
	return m_pCheckBoxSecret->IsChecked();
}

#ifdef PRE_PARTY_DB
#else
bool CDnPartyCreateDlg::IsPartyLevelLimit()
{
	return m_pCheckBoxUserLevel->IsChecked();
}
#endif

int CDnPartyCreateDlg::GetPartyCount() const
{
	int nCount;
	if( m_pComboBoxCount->GetSelectedValue( nCount ) )
	{
		return nCount;
	}

	return 0;
}

#ifdef PRE_MOD_PARTY_CREATE_SORT
int CDnPartyCreateDlg::GetPartyMinCountByMode() const
{
	if (m_CurPartyType == _NORMAL_PARTY)
		return NORMPARTYCOUNTMIN;
	else if (m_CurPartyType == _RAID_PARTY_8)
		return RAIDPARTYCOUNTMIN;

	_ASSERT(0);
	return NORMPARTYCOUNTMIN;
}

int CDnPartyCreateDlg::GetPartyMaxCountByMode() const
{
	if (m_CurPartyType == _NORMAL_PARTY)
		return NORMPARTYCOUNTMAX;
	else if (m_CurPartyType == _RAID_PARTY_8)
		return RAIDPARTYCOUNTMAX;

	_ASSERT(0);
	return NORMPARTYCOUNTMAX;
}
#endif

#ifdef PRE_PARTY_DB
bool CDnPartyCreateDlg::GetUserLevel( int &nMin )
#else
bool CDnPartyCreateDlg::GetUserLevel( int &nMin, int &nMax )
#endif
{
#ifdef PRE_PARTY_DB
#else
	if (!m_pCheckBoxUserLevel->IsChecked())
	{
		nMin = MIN_PARTY_LEVEL;
		nMax = MAX_PARTY_LEVEL;
		return true;
	}
#endif

	int nMinLv = m_pEditBoxLevelMin->GetTextToInt();
	if (nMinLv < 0)
		nMinLv = MIN_PARTY_LEVEL;
#ifdef PRE_PARTY_DB
#else
	int nMaxLv = m_pEditBoxLevelMax->GetTextToInt();
	if (nMaxLv < 0)
		nMaxLv = MAX_PARTY_LEVEL;
#endif

	int nPlayerLv = CDnActor::s_hLocalActor->GetLevel();

#ifdef PRE_PARTY_DB
#else
	if (nMinLv > nMaxLv)
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3549 ), MB_OK, MESSAGEBOX_MIN_MAX_LEVEL_ERROR, this); // UISTRING : 최저 레벨은 최대 레벨보다 클 수 없습니다
		return false;
	}

	if (nMaxLv < nPlayerLv)
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3550 ), MB_OK, MESSAGEBOX_PARTY_CREATE_FAIL_MAX_LEVEL, this); // UISTRING : 최대 레벨은 자신의 레벨보다 작을 수 없습니다
		return false;
	}
#endif

	if (nMinLv > nPlayerLv)
		nMinLv = nPlayerLv;

	if (nMinLv < MIN_PARTY_LEVEL)
		nMinLv = MIN_PARTY_LEVEL;

	nMin = nMinLv;

#ifdef PRE_PARTY_DB
#else
	if (nMaxLv > MAX_PARTY_LEVEL)
		nMaxLv = MAX_PARTY_LEVEL;

	nMax = nMaxLv;
#endif

	return true;
}

void CDnPartyCreateDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (nID == MESSAGEBOX_MIN_MAX_LEVEL_ERROR)
		{
			if (IsCmdControl("ID_OK"))
			{
				RequestFocus(m_pEditBoxLevelMin);
				return;
			}
		}
	}
}

void CDnPartyCreateDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		CDnPartyTask *pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
		if( !pPartyTask ) return;

		if( pPartyTask->GetPartyRole() == CDnPartyTask::SINGLE || pPartyTask->GetPartyRole() == CDnPartyTask::LEADER )
		{
			std::wstring strTemp;

			strTemp = m_pEditBoxPartyName->GetText();
			strTemp = boost::algorithm::trim_copy(strTemp);
			if( strTemp.empty() )
			{
				m_pButtonCreate->Enable( false );
				return;
			}

#ifdef PRE_PARTY_DB
#else
			if (m_pCheckBoxUserLevel->IsChecked() == false)
			{
				m_pEditBoxLevelMin->SetIntToText(1);
				m_pEditBoxLevelMax->SetIntToText(100);
			}
			m_pEditBoxLevelMin->Enable(m_pCheckBoxUserLevel->IsChecked());
			m_pEditBoxLevelMax->Enable(m_pCheckBoxUserLevel->IsChecked());
#endif

			if( m_pCheckBoxSecret->IsChecked() )
			{
				strTemp = m_pEditBoxPassword->GetText();
				
				if( strTemp.empty() || (int)strTemp.size() < pPartyTask->GetMaxPasswordLength())
				{
					m_pButtonCreate->Enable( false );
					return;
				}
			}

			m_pButtonCreate->Enable( true );
#ifdef PRE_PARTY_DB
			m_pTargetStageEditBox->Enable(true);
			m_pTargetStageFilterButton->Enable(true);
#endif
		}
		else
		{
			m_pTargetStageComboBox->Enable(false);
			m_pDifficultyComboBox->Enable(false);

#ifdef PRE_PARTY_DB
			m_pTargetStageEditBox->Enable(false);
			m_pTargetStageFilterButton->Enable(false);
#endif

		}
	}
}

void CDnPartyCreateDlg::Init_Single()
{
#ifdef PRE_PARTY_DB
	m_pTargetStageFilterButton->SetState(eTARGETFILTER_ALL, true, true, false);
#else
	if( !CDnActor::s_hLocalActor ) return;
	DnActorHandle hActor = CDnActor::s_hLocalActor;

	CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(static_cast<CDnActor*>(hActor));
	if( !pPlayerActor ) return;
#endif

	if (m_CurPartyType == _NORMAL_PARTY)
	{
		SetComboBoxCountRange(NORMPARTYCOUNTMIN, NORMPARTYCOUNTMAX);
		SetComboBoxCount(NORMPARTYCOUNTMAX);
	}
	else if (m_CurPartyType == _RAID_PARTY_8)
	{
		SetComboBoxCountRange(RAIDPARTYCOUNTMIN, RAIDPARTYCOUNTMAX);
		SetComboBoxCount(RAIDPARTYCOUNTMAX);
	}

	m_pButtonCreate->Show( true );
	m_pButtonCreate->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3532 ) );
	m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3512 ) );

	WCHAR wszPartyName[512];
#if defined(_EU)
	swprintf_s( wszPartyName, _countof(wszPartyName), L"%s%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3535 ), CDnActor::s_hLocalActor->GetName() );	//	UISTRING : 님의 파티  
#elif !defined(_EU)
	swprintf_s( wszPartyName, _countof(wszPartyName), L"%s%s", CDnActor::s_hLocalActor->GetName(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3535 ) );	//	UISTRING : 님의 파티
#endif 

	m_pCheckBoxSecret->SetChecked( false );
	OnSetSecretCheckbox(false);

	m_pEditBoxPassword->ClearText();
	m_pEditBoxPassword->Enable( false );

#ifdef PRE_PARTY_DB
#else
	m_pCheckBoxUserLevel->SetChecked( false );
#endif

#ifdef PRE_PARTY_DB
	SetPartyMinLevel(CGlobalInfo::GetInstance().m_nCurrentMapIndex, Dungeon::Difficulty::Max);
#else
	m_pEditBoxLevelMin->Enable( false );
	m_pEditBoxLevelMin->SetIntToText( 1 );

	m_pEditBoxLevelMax->Enable( false );
	m_pEditBoxLevelMax->SetIntToText( 100 );
#endif

	m_pEditBoxPartyName->ClearText();
	const int maxPartyNameLen = m_pEditBoxPartyName->GetMaxChar();
	std::wstring safePartyNameString(wszPartyName);
	safePartyNameString = safePartyNameString.substr(0, maxPartyNameLen);

	m_pEditBoxPartyName->SetText(safePartyNameString.c_str(), true);
	m_bDefaultPartyName = true;
	RequestFocus(m_pEditBoxPartyName);
}

void CDnPartyCreateDlg::Init_Master()
{
	CDnPartyTask *pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( !pPartyTask ) return;

	m_pEditBoxPartyName->Enable( true );
	m_pEditBoxPassword->Enable( true );
	m_pComboBoxCount->Enable( true );
	m_pCheckBoxSecret->Enable( true );
#ifdef PRE_PARTY_DB
#else
	m_pCheckBoxUserLevel->Enable( true );
	m_pEditBoxLevelMax->Enable( true );
#endif
	m_pEditBoxLevelMin->Enable( true );
	m_pButtonCreate->Enable( true );
	m_pTargetStageComboBox->Enable(true);
	m_pDifficultyComboBox->Enable(true);

	m_pButtonCreate->Show( true );
	m_pButtonCreate->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3533 ) );
	m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3515 ) );

	m_pEditBoxPartyName->SetText( pPartyTask->GetPartyName() );
	m_bDefaultPartyName = true;

	ePartyType type = pPartyTask->GetPartyType();
	if (type == _NORMAL_PARTY)
		SetComboBoxCountRange(NORMPARTYCOUNTMIN, NORMPARTYCOUNTMAX);
	else if (type == _RAID_PARTY_8)
		SetComboBoxCountRange(RAIDPARTYCOUNTMIN, RAIDPARTYCOUNTMAX);
	m_CurPartyType = type;

	m_pTargetStageFilterButton->SetState(eTARGETFILTER_ALL, true, true, false);
	SetComboBoxCount( pPartyTask->GetMaxPartyCount() );
	//SetComboBoxWorldLevel( pPartyTask->GetWorldLevelLimit() );

	m_pCheckBoxSecret->SetChecked( pPartyTask->IsSecretParty() );
	m_pEditBoxPassword->Enable( pPartyTask->IsSecretParty() );
#ifdef PRE_PARTY_DB
	if (pPartyTask->GetPartyPassword() == Party::Constants::INVALID_PASSWORD)
		m_pEditBoxPassword->ClearText();
	else
		m_pEditBoxPassword->SetIntToText(pPartyTask->GetPartyPassword());
#else
	m_pEditBoxPassword->SetText( pPartyTask->GetPartyPassword() );
	m_pCheckBoxUserLevel->SetChecked( pPartyTask->IsUserLevelLimit() == false );
	m_pEditBoxLevelMax->Enable( m_pCheckBoxUserLevel->IsChecked() );
	m_pEditBoxLevelMin->Enable( m_pCheckBoxUserLevel->IsChecked() );
#endif

#if defined( PRE_PARTY_DB )
#else
	m_pEditBoxLevelMax->SetIntToText( pPartyTask->GetMaxUserLevel() );
#endif // #if defined( PRE_PARTY_DB )
	m_pEditBoxLevelMin->SetIntToText( pPartyTask->GetMinUserLevel() );
}

//	todo : 멤버가 파티 정보창이나 생성창을 호출할 수 있나? 확인 필요 by kalliste
void CDnPartyCreateDlg::Init_Member()
{
	CDnPartyTask *pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( !pPartyTask ) return;

	m_pButtonCreate->Show( false );
	m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3515 ) );

	m_pEditBoxPartyName->SetText( pPartyTask->GetPartyName() );
	m_bDefaultPartyName = true;

	ePartyType type = pPartyTask->GetPartyType();
	if (type == _NORMAL_PARTY)
		SetComboBoxCountRange(NORMPARTYCOUNTMIN, NORMPARTYCOUNTMAX);
	else if (type == _RAID_PARTY_8)
		SetComboBoxCountRange(RAIDPARTYCOUNTMIN, RAIDPARTYCOUNTMAX);
	m_CurPartyType = type;

	m_pTargetStageFilterButton->SetState(eTARGETFILTER_ALL, true, true, false);
	SetComboBoxCount( pPartyTask->GetMaxPartyCount() );

	m_pCheckBoxSecret->SetChecked( pPartyTask->IsSecretParty() );
#ifdef PRE_PARTY_DB
	if (pPartyTask->GetPartyPassword() == Party::Constants::INVALID_PASSWORD)
		m_pEditBoxPassword->ClearText();
	else
		m_pEditBoxPassword->SetIntToText(pPartyTask->GetPartyPassword());
#else
	m_pEditBoxPassword->SetText( pPartyTask->GetPartyPassword() );
	m_pCheckBoxUserLevel->SetChecked( false );
#endif

#if defined( PRE_PARTY_DB )
#else
	m_pEditBoxLevelMax->SetIntToText( pPartyTask->GetMaxUserLevel() );
#endif // #if defined( PRE_PARTY_DB )
	m_pEditBoxLevelMin->SetIntToText( pPartyTask->GetMinUserLevel() );

	m_pEditBoxPartyName->Enable( false );
	m_pEditBoxPassword->Enable( false );
	m_pComboBoxCount->Enable( false );
	m_pCheckBoxSecret->Enable( false );
#ifdef PRE_PARTY_DB
#else
	m_pCheckBoxUserLevel->Enable( false );
	m_pEditBoxLevelMax->Enable( false );
#endif
	m_pEditBoxLevelMin->Enable( false );
	m_pButtonCreate->Enable( false );
	m_pTargetStageComboBox->Enable(false);
	m_pDifficultyComboBox->Enable(false);
}

bool CDnPartyCreateDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show(false);
			focus::ReleaseControl();	// todo : remove this and set ESC hotkey at ui tool next time - by kalliste'
			return true;
		}
		else if (wParam == VK_RETURN)
		{
#ifdef PRE_PARTY_DB
			if (m_pTargetStageEditBox->IsFocus())
			{
				SComboBoxItem* pItem = m_pTargetStageComboBox->GetSelectedItem();
				if (pItem)
				{
					std::wstring str(pItem->strText);
					if (str.compare(m_pTargetStageEditBox->GetText()))
					{
						OnReleaseTargetStage();
						return true;
					}
				}
			}
#endif
#ifdef PRE_ADD_PREDICTIVE_TEXT
			if( m_pTargetStageComboBox->IsFocus() )
			{
				// 여기에 m_pTargetStateEditBox 문자열 입력 

				int nMapId = 0;

				int nSelectedMapIndex = (nMapId) ? nMapId : 0;
				SetTargetStage(nSelectedMapIndex);
				OnChangeTargetStage();
				CDnLocalPlayerActor::LockInput( false );

				focus::ReleaseControl();
				return true;
			}
#endif
			if (m_pButtonCreate->IsEnable())
			{
				if (DoCreateParty(m_Type))
				{
					Show(false);
					focus::ReleaseControl();	// todo : remove this and set ESC hotkey at ui tool next time - by kalliste
				}
				return true;
			}
		}
		// #61563 - elkain03
#ifdef PRE_ADD_PREDICTIVE_TEXT

		else if( wParam == VK_BACK )
		{
			if (m_pTargetStageComboBox->IsFocus() )
			{	
				focus::ReleaseControl();
				focus::SetFocus( m_pTargetStageEditBox );
				CDnLocalPlayerActor::LockInput( false );
				return true;
			}
		}

#endif
	}
	else if (uMsg == WM_LBUTTONDOWN)
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		PointToFloat( MousePoint, fMouseX, fMouseY );

		if (m_pEditBoxPartyName->IsInside(fMouseX, fMouseY) && m_bDefaultPartyName && 
			CDnPartyTask::IsActive() && GetPartyTask().GetPartyRole() != CDnPartyTask::MEMBER)
		{
			m_pEditBoxPartyName->ClearText();
			m_bDefaultPartyName = false;
			return true;
		}
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnPartyCreateDlg::OnSetSecretCheckbox(bool bSet)
{
	m_pEditBoxPassword->Enable(bSet);
	RequestFocus(m_pEditBoxPassword);

	DWORD i = 0;
	for (; i < m_pSecretWarnStaticList.size(); ++i)
		m_pSecretWarnStaticList[i]->Show(bSet);

	if (bSet == false)
	{
		m_pEditBoxPassword->ClearText();
	}
}

void CDnPartyCreateDlg::SetDifficulty()
{
	m_pDifficultyComboBox->RemoveAllItems();

#ifdef PRE_PARTY_DB
	m_pDifficultyComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3599), NULL, Dungeon::Difficulty::Max);	// UISTRING : 제한없음

	int i = Dungeon::Difficulty::Easy;
	std::wstring difficultyText;
	for(; i < Dungeon::Difficulty::Max; ++i)
#else
	m_pDifficultyComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3599), NULL, 0);	// UISTRING : 제한없음

	int i = 1;
	std::wstring difficultyText;
	for(; i <= Dungeon::Difficulty::Max; ++i)
#endif
	{
		DN_INTERFACE::STRING::GetStageDifficultyText(difficultyText, i);
		m_pDifficultyComboBox->AddItem(difficultyText.c_str(), NULL, i);
	}
}

#ifdef PRE_ADD_PREDICTIVE_TEXT
bool CDnPartyCreateDlg::OnAddComboBoxTargetStageList( int nCount, const std::wstring strName )
{
	if (strName.size() < 1) {
		return false;
	}
	
	if( m_MapAreaNameListbyKey.empty() )
		return false ;

	m_pTargetStageComboBox->RemoveAllItems();

	const std::pair< std::multimap< wchar_t, SPartyTargetMapInfo >::const_iterator, 
		std::multimap< wchar_t, SPartyTargetMapInfo >::const_iterator >range = m_MapAreaNameListbyKey.equal_range( strName[0]);

	std::multimap< wchar_t, SPartyTargetMapInfo >::const_iterator itor = range.first;

	bool bOpenMapNameList = false;

	for( ; itor != range.second; ++itor )
	{			
		const SPartyTargetMapInfo& pPartyTagetMapInfo = (*itor).second;

		bool bAddItem = false;

		if (pPartyTagetMapInfo.IsValid())
		{
			if (pPartyTagetMapInfo.minPartyNum < 0 && pPartyTagetMapInfo.maxPartyNum < 0)
			{
				bAddItem = true;
			}
			else
			{
				bAddItem = (pPartyTagetMapInfo.minPartyNum < 0 || (pPartyTagetMapInfo.minPartyNum >= 0 && pPartyTagetMapInfo.minPartyNum <= nCount));
				if (bAddItem)
					bAddItem = (pPartyTagetMapInfo.maxPartyNum < 0 || (pPartyTagetMapInfo.maxPartyNum >= 0 && pPartyTagetMapInfo.maxPartyNum >= nCount));
			}

			if (GetCurrentStageFilter() != eTARGETFILTER_ALL && GetCurrentStageFilter() != pPartyTagetMapInfo.filterType)
			{
				bAddItem = false;
			}
			if ( bAddItem )
			{
				bOpenMapNameList = true;
				m_pTargetStageComboBox->AddItem(pPartyTagetMapInfo.mapName.c_str(), NULL, pPartyTagetMapInfo.mapId);
			}
		}
	
	}

	return bOpenMapNameList;
}
#endif  // PRE_ADD_PREDICTIVE_TEXT

#ifdef PRE_PARTY_DB
// Command가 발생해야 아래의 함수가 호출되는 구조로 되어있다.
void CDnPartyCreateDlg::OnAddEditBoxString(const std::wstring& strName)
{
	int curEditBoxCaretPos = m_pTargetStageEditBox->GetCaretPos();
	
	if (strName.size() < 1 ) {
		return;
	}

	std::wstring almost = L"";
	
	
#ifdef PRE_ADD_PREDICTIVE_TEXT
	
	const std::pair< std::multimap< wchar_t, SPartyTargetMapInfo >::const_iterator, 
		std::multimap< wchar_t, SPartyTargetMapInfo >::const_iterator >range = m_MapAreaNameListbyKey.equal_range( strName[0]);

	std::multimap< wchar_t, SPartyTargetMapInfo >::const_iterator itor = range.first;

	for( ; itor != range.second; ++itor )
	{
		const SPartyTargetMapInfo& info = (*itor).second;

		bool bAddItem = false;

		if (info.IsValid())
		{
			if (info.minPartyNum < 0 && info.maxPartyNum < 0)
			{
				bAddItem = true;
			}
			else
			{
#ifdef PRE_MOD_PARTY_CREATE_SORT
				bAddItem = (info.minPartyNum < 0 || (info.minPartyNum >= 0 && info.minPartyNum <= GetPartyMaxCountByMode() ));
				if (bAddItem)
					bAddItem = (info.maxPartyNum < 0 || (info.maxPartyNum >= 0 && info.maxPartyNum >= GetPartyMaxCountByMode() ));
#else
				bAddItem = (info.minPartyNum < 0 || (info.minPartyNum >= 0 && info.minPartyNum <= GetPartyCount() ));
				if (bAddItem)
					bAddItem = (info.maxPartyNum < 0 || (info.maxPartyNum >= 0 && info.maxPartyNum >= GetPartyCount() ));
#endif
			}

			if (GetCurrentStageFilter() != eTARGETFILTER_ALL && GetCurrentStageFilter() != info.filterType)
				bAddItem = false;

			if (bAddItem)
			{
				almost = info.mapName;
				break;
			}
		}
	}
		
#else
		m_AutoCompleteMgr.GetAlmostMatchingString(almost, strName);
#endif 
	
	if (almost != L"")
	{ 
 		//std::wstring temp;
		//temp = FormatW(L"[AutoText] OnAddEditBoxString : current EditBox:%s\n ", m_pTargetStageEditBox->GetText() );

		m_pTargetStageEditBox->ClearText();
		m_pTargetStageEditBox->SetText( almost.c_str() );
		
		m_pTargetStageEditBox->SetSelection( 0, m_pTargetStageEditBox->GetTextLength() );
		//m_pTargetStageEditBox->SetSelection( m_pTargetStageEditBox->GetTextLength(), curEditBoxCaretPos );
		m_AutoCompleteTextCache = almost;
		//temp += FormatW(L"almost:%s\n", almost.c_str());
		//OutputDebugStringW(temp.c_str());
	}
	else
	{
		m_AutoCompleteTextCache = L"";
	}
}

void CDnPartyCreateDlg::SetTargetStage(int nMapIndex)
{
	m_pTargetStageComboBox->SetSelectedByValue(nMapIndex);

	SComboBoxItem* pItem = m_pTargetStageComboBox->GetSelectedItem();
	if (pItem)
	{
		m_pTargetStageEditBox->ClearText();
		m_pTargetStageEditBox->SetText(pItem->strText);
	}
}

void CDnPartyCreateDlg::OnChangeTargetStage(TDUNGEONDIFFICULTY difficultyLevelSet)
{
	SComboBoxItem* pItem = m_pTargetStageComboBox->GetSelectedItem();
	if (pItem && pItem->nValue != 0)
	{
		m_pDifficultyComboBox->Enable(true);

		int difficultyForComboBox = (int)difficultyLevelSet;
		if (difficultyLevelSet == Dungeon::Difficulty::Max)
			difficultyForComboBox = -1;
		m_pDifficultyComboBox->GetSelectedValue(difficultyForComboBox);
		SetPartyMinLevel(pItem->nValue, difficultyForComboBox);
#ifdef PRE_MOD_PARTY_CREATE_SORT
		std::vector<SPartyTargetMapInfo>::const_iterator iter = m_MapAreaNameListForSortCache.begin();
		for (; iter != m_MapAreaNameListForSortCache.end(); ++iter)
		{
			const SPartyTargetMapInfo& info = (*iter);
			if (info.mapId == pItem->nValue)
			{
				int minNum = (info.minPartyNum < 0) ? GetPartyMinCountByMode() : info.minPartyNum;
				int maxNum = (info.maxPartyNum > GetPartyMaxCountByMode()) ? GetPartyMaxCountByMode() : info.maxPartyNum;
				SetComboBoxCountRange(minNum, maxNum);
				SetComboBoxCount(maxNum);
				break;
			}
		}
#endif
	}
	else
	{
		m_pDifficultyComboBox->SetSelectedByValue(Dungeon::Difficulty::Max);
		m_pDifficultyComboBox->Enable(false);

		SetPartyMinLevel(CGlobalInfo::GetInstance().m_nCurrentMapIndex, Dungeon::Difficulty::Max);
#ifdef PRE_MOD_PARTY_CREATE_SORT
		SetComboBoxCountRange(GetPartyMinCountByMode(), GetPartyMaxCountByMode());
		SetComboBoxCount(GetPartyMaxCountByMode());
#endif
	}
}


void CDnPartyCreateDlg::OnReleaseTargetStage()
{
	if (m_pTargetStageEditBox->GetTextLength() <= 0)
		SetTargetStage(NO_TARGET_STAGE_INDEX);

	m_pTargetStageComboBox->SetSelectedByText(m_pTargetStageEditBox->GetText());
	m_pTargetStageEditBox->SetSelection(m_pTargetStageEditBox->GetTextLength(), m_pTargetStageEditBox->GetTextLength());

	OnChangeTargetStage();
}
#endif // PRE_PARTY_DB