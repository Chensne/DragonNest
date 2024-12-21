#pragma once

#include "EtUIDialog.h"
#include "DnPartyData.h"

class CDnPartySearchDlg : public CEtUIDialog
{
public:
	enum eSearchType
	{
		eSEARCH_DIFFICULTY,
		eSEARCH_QUEST,
		eSEARCH_DAILYMISSION,
		eSEARCH_MAX
	};

	// note by kalliste : Server 측 DNMissionSystem 에 event type enum이 정의되어 있고 사용되고 있어, 
	//					  commondef로 옮기지 않는 이상 c/s 따로 가져가야 하는 상황.
	//					  c/s 통합 type 정의되면 삭제하고 그에 따를 것.
	enum eEventValueTypeEnum
	{
		None,
		PlayerLevel,		// 플레이어 레벨
		PartyCount,			// 파티원수
		MapID,				// 맵 ID
	};

	enum ePartySearchError
	{
		eERRPS_FAIL_NO_STAGE,
	};

	CDnPartySearchDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPartySearchDlg(void);

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
#ifdef PRE_PARTY_DB
	const std::map<int, int>& GetPartyTargetMapLevelLimitList() const;
	const std::vector<SPartyTargetMapInfo>& GetPartyTargetMapList() const;
#else
	void SetCurrentPartyListPage(int page) { m_nCurPartyListPage = page; }
#endif

protected:
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ );

private:
	void SetDifficultyComboBox();
	void SetQuestComboBox();
	void SetMissionComboBox();
	bool OnSetMapSelect(int nMapId, bool bOn);

	bool DoAddListBox(const SPartyTargetMapInfo& info, int nMapStringId);
	void DoAddQuestComboBox(TQuest* pQuest);
	void DoAddMissionComboBox(const std::wstring& missionName, int mapId);

	bool IsMapInSearchArea(int mapIdx, int mapNameId) const;
	bool IsMapStringInMapAreaNameList(const std::wstring& mapName) const;
#ifdef PRE_MOD_PARTY_SEARCH_MINLEV
	bool IsPlayerEnterMap(int mapId) const;
#endif

	void RefreshListBox();
	void RefreshQuestDescription(int nMapId);
	void RefreshMissionDescription(int nMapId);
	void RefreshSearchCondition();

	void HandlePartySearchError(ePartySearchError code);

	CEtUIStatic*			m_pCurConditionStatic;
	CEtUIComboBox*			m_pSearchComboBox[eSEARCH_MAX];
	CEtUITextBox*			m_pDescriptions[eSEARCH_MAX];
	CEtUIListBoxEx*			m_pListBoxEx;
	CEtUIButton*			m_pSearch;
	CEtUIButton*			m_pClose;
#ifdef PRE_PARTY_DB
	CEtUICheckBox* m_pCurVillageDungeonCheckBox;
#endif
	int						m_nCurrentMapIdx;
	std::map<int, int>		m_nMapLimitLevelList;	// mapid / limitLevel
	std::vector<SPartyTargetMapInfo> m_MapAreaNameList;

	int						m_nCurSelectDifficulty;
	int						m_nCurSelectQuestMapStringId;
	int						m_nCurSelectMisssionMapStringId;
#ifdef PRE_PARTY_DB
#else
	int						m_nCurPartyListPage;
#endif
	//std::vector<int>		m_nMapSelected;
};
