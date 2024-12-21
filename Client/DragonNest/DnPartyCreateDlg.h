#pragma once

#ifdef PRE_PARTY_DB
#include "DnCustomDlg.h"
#else
#include "EtUIDialog.h"
#endif
#include "DnSmartMoveCursor.h"
#include "DnPartyData.h"
#ifdef PRE_PARTY_DB
#include "DnTextAutoCompleteUIMgr.h"
#include "DnRotateToggleButton.h"
#endif



#ifdef PRE_PARTY_DB
class CDnPartyCreateDlg : public CDnCustomDlg, public CEtUICallback
#else
class CDnPartyCreateDlg : public CEtUIDialog, public CEtUICallback
#endif
{
public:
	enum ePartyReqType
	{
		PARTYREQ_NONE,
		PARTYREQ_CREATE,
		PARTYREQ_MODIFY
	};

#ifdef PRE_PARTY_DB
#else
	enum eStageDifficultyLevel
	{
		eDIFFCLT_EASY = 1,
		eDIFFCLT_NORMAL,
		eDIFFCLT_HARD,
		eDIFFCLT_MASTER,
		eDIFFCLT_ABYSS
	};
#endif

	enum ePartyCreateType
	{
		ePTYPE_NORMAL = 0,
		ePTYPE_RAID	= 1,
		ePTYPE_MAX = 2
	};

	CDnPartyCreateDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPartyCreateDlg(void);

protected:
	CEtUIIMEEditBox *m_pEditBoxPartyName;
	CEtUIEditBox *m_pEditBoxPassword;
	CEtUIEditBox *m_pEditBoxLevelMin;
#ifdef PRE_PARTY_DB
#else
	CEtUIEditBox *m_pEditBoxLevelMax;
#endif

	CEtUIComboBox *m_pComboBoxCount;

	CEtUICheckBox *m_pCheckBoxSecret;
#ifdef PRE_PARTY_DB
#else
	CEtUICheckBox *m_pCheckBoxUserLevel;
#endif
	CEtUIRadioButton*	m_pRadioBtnPartyType[ePTYPE_MAX];
	ePartyType			m_CurPartyType;
	int					m_PartyCountMin;
	int					m_PartyCountMax;

	CEtUIButton *m_pButtonCreate;
	CEtUIButton *m_pButtonClose;

	CEtUIStatic *m_pStaticTitle;

	std::vector<CEtUIStatic*>	m_pSecretWarnStaticList;
	ePartyReqType				m_Type;

	bool						m_bDefaultPartyName;
	CDnSmartMoveCursor			m_SmartMove;

#ifdef PRE_PARTY_DB
	CEtUIIMEEditBox*		m_pTargetStageEditBox;
	CDnRotateToggleButton* m_pTargetStageFilterButton;

	CDnTextAutoCompleteUIMgr m_AutoCompleteMgr;

	std::wstring m_AutoCompleteTextCache;
	eTargetStageFilterType m_PreviousTargetStageFilter;
	eTargetStageFilterType m_CurrentTargetStageFilter;
#ifdef PRE_ADD_MTRANDOM_CLIENT
	CMtRandomLocalUsable m_Random;
#else
	CMtRandom m_Random;
#endif // PRE_ADD_MTRANDOM_CLIENT
#endif
	CEtUIComboBox*				m_pTargetStageComboBox;


	CEtUIComboBox*				m_pDifficultyComboBox;
	int							m_nCurrentMapIdx;
#ifdef PRE_PARTY_DB
	int m_nCurrentLocalPlayerLevel;
	std::vector<int> m_AbyssMapListForDefaultTemplate;
#endif

	std::map<int, int>			m_nMapLimitLevelList;	// mapid / limitLevel
	bool						m_bIsUserChangeLimitLevel;

	std::vector<SPartyTargetMapInfo> m_MapAreaNameListForSortCache;

#ifdef PRE_ADD_PREDICTIVE_TEXT
	std::multimap< wchar_t, SPartyTargetMapInfo > m_MapAreaNameListbyKey;	
	bool						m_bOpenStageWithMouse;
#endif

protected:
	void	Init_Single();
	void	Init_Master();
	void	Init_Member();

	void	SetComboBoxCountRange(int min, int max);
	void	SetComboBoxCount( int nCount );
	void	OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

	void	OnSetSecretCheckbox(bool bSet);

public:
	LPCWSTR GetPartyName() const;
#ifdef PRE_PARTY_DB
	int GetPartyPassword() const;
	int GetMapLevelLimit() const;
#else
	LPCWSTR GetPartyPassword() const;
#endif

	bool	IsPartySecret();
#ifdef PRE_PARTY_DB
#else
	bool	IsPartyLevelLimit();
#endif

	int		GetPartyCount() const;
#ifdef PRE_MOD_PARTY_CREATE_SORT
	int GetPartyMinCountByMode() const;
	int GetPartyMaxCountByMode() const;
#endif
#ifdef PRE_PARTY_DB
	bool	GetUserLevel( int &nMin );
#else
	bool	GetUserLevel( int &nMin, int &nMax );
#endif

	bool	DoCreateParty(ePartyReqType type);
	void	SetMode(ePartyReqType type);

	void	SetDifficulty();
#ifdef PRE_PARTY_DB
	void	GetTargetStageIdx(int& mapIdx, TDUNGEONDIFFICULTY& mapDifficulty);
#else
	void	GetTargetStageIdx(int& mapIdx, int& mapDifficulty);
#endif

#ifdef PRE_ADD_PREDICTIVE_TEXT
	bool	OnAddComboBoxTargetStageList( int nCount, const std::wstring strName );
#endif 

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	void		SetPartyMinLevel(int mapId, int difficultyLevel);
#ifdef PRE_PARTY_DB
	int			GetStageMinLevel( int mapId, int difficultyLevel ) const;
#else
	int			GetStageMinLevel( int mapId, int difficultyLevel );
#endif
	void		OnChangePartyType();
	void		SetPartyMinLevelFixed(int difficultyLevel);
	int			ConvertCreateTypeToPartyType(ePartyCreateType createType);
	int			ConvertPartyTypeToCreateType(ePartyType partyType);
#ifdef PRE_PARTY_DB
	void OnAddEditBoxString(const std::wstring& strName);
	void OnChangeTargetStage(TDUNGEONDIFFICULTY difficultyLevelSet = Dungeon::Difficulty::Max);
	void OnReleaseTargetStage();

	void SetTargetStage(int nMapIndex);
	void SetCurrentStageFilter();
	eTargetStageFilterType GetCurrentStageFilter() const { return m_CurrentTargetStageFilter; }

#ifdef PRE_MOD_PARTY_CREATE_SORT
	void ReArrangeTargetStageCombo(int curSelectedMapIdx);
#else
	void ReArrangeTargetStageCombo(int nPartyCount, int curSelectedMapIdx);
#endif

	void CheckAndSetMapLevelLimit();
	void SetDefaultPartyTemplate(bool bMakeAbyssMapList);
#else
	void ReArrangeTargetStageComboByMemberCount(int nCount);
#endif
};
