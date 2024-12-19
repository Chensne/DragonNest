#pragma once

#include "DnCustomDlg.h"

#ifdef PRE_ADD_LEVELUP_GUIDE

#define _MAX_NEW_LEVELUP_SKILL_SLOT 4
#define _MAX_NEW_LEVELUP_SKILL_SLOT_EVEN (_MAX_NEW_LEVELUP_SKILL_SLOT - 1)
#define _LEVELUP_PRESENT_TIME 2.f
#define _LEVELUP_INTERVAL_TIME 1.f
#define _LEVELUP_SKILL_PRESENT_TIME	2.f

class CDnLevelUpGuideDlg : public CDnCustomDlg
{
public:
	CDnLevelUpGuideDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnLevelUpGuideDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );

	virtual void Process(float fElapsedTime);

	void ShowOnLevelUp(bool bShow, int nNewLevel);

private:
	void ShowNewLevel(bool bShow);
	void ShowNewSkill(bool bShow);

	enum eLevelUpGuideState
	{
		eNone,

		eShowNewLevel_Start,
		eShowNewLevel_Process,
		eShowNewLevel_End,

		eShowNewSkill_Start,
		eShowNewSkill_Process,
		eShowNewSkill_End
	};

	CEtUIStatic* m_pNewLevelBorder;
	CEtUIStatic* m_pNewLevelBackGround;
	CEtUIStatic* m_pNewLevelText;
	CEtUIStatic* m_pNewLevelCongratText;

	CEtUIStatic* m_pNewSkillBorder;
	CEtUIStatic* m_pNewSkillBorder2;
	CEtUIStatic* m_pNewSkillBackGround;
	CEtUIStatic* m_pNewSkillText;

	CEtUIStatic*		m_pNewSkillEvenCountPos[_MAX_NEW_LEVELUP_SKILL_SLOT_EVEN];
	CDnSkillSlotButton* m_pNewSkillSlotBtn[_MAX_NEW_LEVELUP_SKILL_SLOT];

	eLevelUpGuideState	m_State;
	float				m_Timer;

	int					m_NewLevelCache;
	std::vector<DnSkillHandle> m_SkillHandleList;
};

#endif // PRE_ADD_LEVELUP_GUIDE