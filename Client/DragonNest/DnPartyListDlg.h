#pragma once
#ifdef PRE_PARTY_DB
#include "DnCustomDlg.h"
#else
#include "EtUIDialog.h"
#endif
#include "VillageClientSession.h"
#include "DnPartyData.h"
#include "EtUIDialogGroup.h"

class CEtUIButton;
class CDnPartyCreateDlg;
class CDnPassWordInputDlg;
class CDnPartyEnterPasswordDlg;
class CDnPartyEnterDlg;
class CDnPartySearchDlg;
class CDnVillageTask;

#ifdef PRE_PARTY_DB
class CDnPartyListDlg : public CDnCustomDlg, public CEtUICallback
#else
class CDnPartyListDlg : public CEtUIDialog, public CEtUICallback
#endif
{
	enum
	{
		PARTYCREATE_DIALOG,
		PARTYJOINPASS_DIALOG,
		PARTYJOIN_DIALOG,
		PARTYSEARCH_DIALOG,
	};

#ifdef PRE_PARTY_DB
	enum ePartyListSortBtnType
	{
		eLevelAvailable,
		eUserCount,
		eTargetStage,
		eDifficulty,
		eSortMax
	};

	enum ePartyListSortType
	{
		eINCREASE,
		eDECREASE,
	};
#endif

public:
	CDnPartyListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPartyListDlg(void);

protected:
	PARTYINFO_LIST_VEC m_vecPartyInfoList;

	CEtUIStatic *m_pPartyPage;
	CEtUIStatic *m_pSelectBar;

	CEtUIButton *m_pButtonPagePrev;
	CEtUIButton *m_pButtonPageNext;

	CEtUIButton *m_pButtonCreate;
	CEtUIButton *m_pButtonJoin;
	//CEtUIButton *m_pButtonDirect;
	CEtUIButton *m_pButtonRefresh;

	int m_nCurrentPage;
	int m_nCurSelect;
	float m_fRefreshTime;

	CDnPartyCreateDlg *m_pPartyCreateDlg;
	CDnPartyEnterPasswordDlg*	m_pPartyEnterPasswordDlg;
	CDnPartyEnterDlg*			m_pPartyEnterDlg;
	CDnPartyEnterDlg*			m_pRaidEnterDlg;
	CDnPartyEnterPasswordDlg*	m_pRaidEnterPasswordDlg;
	CDnPassWordInputDlg*		m_pPassWordInputDlg;
	CDnPartySearchDlg*			m_pPartySearchDlg;

	CEtUIComboBox*				m_pMapSortComboBox;
	CEtUIComboBox*				m_pDifficultySortComboBox;
	CEtUIButton*				m_pSearchBtn;
	int							m_nCurrentMapIdx;

#ifdef PRE_PARTY_DB
	CEtUIComboBox* m_pSearchHistoryComboBox;
	CEtUIIMEEditBox* m_pSearchEditBox;
	CEtUICheckBox* m_pOnlySameVillageCheckBox;
	CDnRotateToggleButton* m_pSortBtns[eSortMax];
	CDnRotateToggleButton* m_pTargetStageFilterButton;
	Party::OrderType::eCode m_CurrentOrderType;
	Party::StageOrderType::eCode m_CurrentStageOrderType;

	bool m_bOnlySameVillageCoolTimer;
	float m_fOnlySameVillageCoolTimeChecker;
#endif

	bool						m_bSearchCoolTimer;
	float						m_fSearchCoolTime;

	CEtUIDialogGroup			m_PartyDlgGroup;
	bool						m_bResetSortForAdvSearch;

protected:
	void PriorPage();
	void NextPage();

	int FindInsideItem( float fX, float fY );

	void UpdatePage();
#ifdef PRE_PARTY_DB
	bool UpdatePartyInfoList(int nPageOffset);
#else
	void UpdatePartyInfoList();
#endif
#ifdef PRE_WORLDCOMBINE_PARTY
	void SetWorldCombineParty(SPartyInfoList& partyInfo, const CVillageClientSession::PartyListStruct& partyListInfo);

#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
	void SetPartyEnterDlgByRaidOrNormal(bool bRaid, const SPartyInfoList& partyInfoList, const std::wstring& title, ePartyType type);
#endif
	void SetPartyEnterDlgByRaidOrNormal(bool bRaid, const SPartyInfoList& partyInfoList, const std::wstring& title);
#endif

	void UpdateSelectBar();
	void UpdatePageButton();

	void ClearLists();

	int GetCurrentPage() { return m_nCurrentPage; }
	int GetMaxPage();
	int GetCurSel();


	void SetPartyList();
	int	 GetPartyListSize() const;

	const CDnVillageTask* GetVillageTask() const;
	const CVillageClientSession::PARTYLISTINFO_MAP& GetPartyListFromVCS() const;
	void InitPartyInfoList();

#ifdef PRE_PARTY_DB
	void JoinParty( TPARTYID nPartyIndex, int iPassword );
	void DoSearch();
	void LockSearchBtn(bool bLock);
	bool IsSearchBtnLocked() const;
	void ResetAllSortBtn(ePartyListSortBtnType excepBtnType);
	void OnTurnOverPage();

	void MakeSearchHistoryWordsComboBox();
	void MakeHighLightSearchWord(CEtUIStatic* pStatic);
#else
	void JoinParty( int nPartyIndex, const WCHAR * pPartyPassword = NULL );
#endif
	const CVillageClientSession::PartyListStruct* GetPartyListInfo( TPARTYID PartyIndex ) const;

public:
	int GetStartIndex();
	void RefreshList();
	void RefreshPartyMemberList();
	void SetPartyInfo(const std::wstring& partyName, const std::wstring& targetStageName);
	void SetDifficultyToPartyCreateDlg(const BYTE* pDifficulties);
	void OnRecvPartyMemberInfo(ePartyType type);

	void OnPartyJoinFailed();
	void ResetSort(bool bUIOnly = false);

#ifdef PRE_PARTY_DB
	const WCHAR* GetCurrentSearchWord() const;
	Party::OrderType::eCode GetCurrentOrderType() const;
	Party::StageOrderType::eCode GetCurrentStageOrderType() const;

	void MakeSearch(const std::wstring& searchName);
	void EnableSortButtons(bool bEnable);

	void ProcessCoolTimer(float fElapsedTime, float& coolTimer, bool& bCoolTimeChecker);
	bool IsOnlySameVillage() const;
	void ResetPartyList();
#endif

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process( float fElapsedTime );
};
