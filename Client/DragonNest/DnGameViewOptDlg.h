#pragma once
#include "DnOptionDlg.h"
#include "GameOption.h"

class CDnGameViewOptDlg : public CDnOptionDlg
{
public:
	CDnGameViewOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGameViewOptDlg(void);

protected:
	CEtUICheckBox *m_pCheckUserName;
	CEtUICheckBox *m_pCheckPartyUserName;
	CEtUICheckBox *m_pCheckMonsterName;
	CEtUICheckBox *m_pCheckStateBarInsideCount;
	CEtUICheckBox *m_pCheckMyHPBar;
	CEtUICheckBox *m_pCheckPartyHPBar;
	CEtUICheckBox *m_pCheckMonsterHPBar;
	CEtUICheckBox *m_pCheckMyCombatInfo;
	CEtUICheckBox *m_pCheckPartycombatinfo;
	CEtUICheckBox *m_pCheckDirectionDamageMelee;
	CEtUICheckBox *m_pCheckDirectionDamageRange;
	CEtUICheckBox *m_pCheckGuildName;
	CEtUICheckBox *m_pCheckGameTip;
	CEtUICheckBox *m_pCheckSimpleGuild;
#ifdef PRE_ADD_CASH_AMULET
	CEtUICheckBox *m_pCheckCashChatBalloon;
#endif
#ifdef PRE_ADD_VIEW_OPITION_PARTY_HEAL
	CEtUICheckBox *m_pCheckPartyHealInfo;
#endif
#ifdef PRE_ADD_SHOWOPTION_PVPMARK
	CEtUICheckBox *m_pCheckPVPLevelMark;
#endif
#ifdef PRE_ADD_SKILLCOOLTIME
	CEtUICheckBox* m_pCheckSkillCoolTimeConter;
#endif

	typedef CDnOptionDlg BaseClass;

public:
	void ExportSetting() override;
	void ImportSetting() override;
	bool IsChanged() override;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );	
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
