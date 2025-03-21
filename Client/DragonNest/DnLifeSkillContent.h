#pragma once
#include "DnCustomDlg.h"
#include "DNTableFile.h"

class CDnSkillTreeDlg;
class CSecondarySkill;

struct stLifeSkillInfoData;

class CDnLifeSkillContent
{
public:
	CDnLifeSkillContent();
	virtual ~CDnLifeSkillContent();

public:
	void RefreshInfo();											// 보조스킬 정보 갱신
	void AddLifeSkill( CSecondarySkill* pAddSecondarySkill );	// 보조스킬 추가
	void SetSkillTreeDlg( CDnSkillTreeDlg* pSkillTreeDlg ) { m_pSkillTreeDlg = pSkillTreeDlg; };

private:
	void SetCustomSkillFunction( DNTableFileFormat* pSoxSkill, stLifeSkillInfoData* pLifeSkillInfoData );

private:
	CDnSkillTreeDlg* m_pSkillTreeDlg;
};
