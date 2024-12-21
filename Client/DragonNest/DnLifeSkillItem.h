#pragma once

#include "DnCustomDlg.h"
#include "DnSkillTask.h"
#include "DnCommonDef.h"

typedef void (CDnSkillTask::*fpClickButton) ();

struct stLifeSkillInfoData
{
	int m_nSkillID;
	int m_nLevel;
	int m_nExp;
	SecondarySkill::Grade::eType m_eGrade;

	std::wstring m_strName;
	std::wstring m_strDescription;
	std::wstring m_strDoSkill;
	
	fpClickButton m_fpDoSkill;

	stLifeSkillInfoData() : m_fpDoSkill( NULL ), m_nSkillID( 0 ), m_nLevel( 0 ), m_nExp( 0 ), m_eGrade( SecondarySkill::Grade::Beginning ) {}
	~stLifeSkillInfoData() {}
};

class CDnLifeSkillItem : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnLifeSkillItem( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeSkillItem();

protected:
	CEtUIStatic* m_pName;
	CEtUIStatic* m_pDescription;
	CEtUIStatic* m_pLevel;
	CEtUIButton* m_pButtonDoSkill;
	CEtUIProgressBar* m_pProgress;
	CEtUIStatic* m_pPercent;
	CEtUIStatic* m_pCount;
	CDnLifeSkillButton* m_pSkillButton;

	stLifeSkillInfoData m_stLifeSkillInfoData;

public:
	stLifeSkillInfoData& GetLifeSkillInfoData() { return m_stLifeSkillInfoData; }
	void SetInfo( stLifeSkillInfoData& stInfoData );
	void SetUIInfo();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

private:
	float GetAchievementRate();
};

