#pragma once
#include "EtUIDialog.h"
#include "DnPartyData.h"
#include "DnItem.h"

class CEtUIComboBox;
class CEtUIButton;
class CDnPartyCreateDlg;
class CDnPartyKickDlg;
class CDnPartyInviteDlg;
class CDnPartyMemberDlg;
class CDnPartyRaidReGroupDlg;
class CDnPartyInfoListDlg : public CEtUIDialog, public CEtUICallback
{
	enum
	{
		PARTYINFO_DIALOG,
		PARTYKICK_DIALOG,
		PARTYINVITE_DIALOG,
		PARTY_NORMAL_MEMBER_DIALOG,
		PARTY_RAID_MEMBER_DIALOG,
		PARTY_RAID_REGROUP_DIALOG,
#if defined (PRE_WORLDCOMBINE_PARTY)
		PARTY_NORMAL_COMBINE_DIALOG,
#endif
	};

public:
	CDnPartyInfoListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPartyInfoListDlg(void);

protected:
	CEtUIStatic *m_pStaticTitle;
	CEtUIComboBox*	m_pComboBoxRoulette;
	CEtUIComboBox*	m_pComboBoxRandRoulette;

	CEtUIButton *m_pButtonInfo;
	CEtUIButton *m_pButtonChange;
	CEtUIButton *m_pButtonQuit;
	CEtUIButton *m_pButtonInvite;
	CEtUIButton *m_pButtonExile;
	CEtUIButton* m_pButtonRequestReady;
	float m_fRequestReadyCoolTimer;

	CEtUIStatic*	m_pTargetMapStatic;
	CEtUIStatic*	m_pDifficultyStatic;
	CEtUICheckBox*	m_pJobDiceCheckBox;
	CEtUIStatic*	m_pJobDiceStatic;
#ifdef PRE_PARTY_DB
	CEtUICheckBox* m_pAllowWorldZoneMapListCheckBox;
	CEtUIStatic* m_pAllowWorldZoneMapListStatic;
#endif

	float			m_SetJobDiceCoolTimer;
	bool			m_bIsReqJobDiceState;

	CEtUIButton*	m_pButtonEditIndex;
	CDnPartyRaidReGroupDlg* m_pReGroupDlg;
	CDnPartyCreateDlg*	m_pPartyInfoDlg;
	CDnPartyKickDlg*	m_pPartyKickDlg;
	CDnPartyInviteDlg*	m_pPartyInviteDlg;
	ePartyItemLootRule	m_PreReqChangeRuleCache;
	std::vector<CDnPartyMemberDlg*> m_pPartyMemberDlgList;

protected:
	ePartyItemLootRule	ConvertLootRule(const WCHAR* ruleName) const;
	const WCHAR*		ConvertLootRule(ePartyItemLootRule rule) const;
	eItemRank ConvertLootRank(const WCHAR* rankName) const;

	void				OnChangeItemLootRule();
	void				OnChangeItemLootRank();
	bool				IsInvalidItemLootRule(ePartyItemLootRule rule) const;
	bool				IsInvalidItemLootRank(eItemRank rank) const;

public:
	void RefreshList();
	void SetPartyInfoLeader( int nLeaderIndex );
	void OnPartyJoin();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Process( float fElapsedTime );
	void EnableLeaderChangeBtn(bool bEnable);
	void EnableExileBtn(bool bEnable);

	CDnPartyMemberDlg* GetCurrentPartyMemberDlg() const;
	CDnPartyMemberDlg* GetPartyMemberDlg(ePartyType type) const;
	void ShowOffPartyMemberDlg();

#ifdef PRE_ADD_NEWCOMEBACK
	void SetComebackAppellation( UINT sessionID, bool bComeback );
#endif // PRE_ADD_NEWCOMEBACK

};
