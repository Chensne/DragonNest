#pragma once
#include "idnskillusablechecker.h"


// 특정 토글 스킬이 실행 중인지를 체크함
class CDnToggleChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnToggleChecker >
{
private:
	int					m_iRequireToggleSkillID;	

public:
	CDnToggleChecker( DnActorHandle hActor, int iToggleSkillID );
	virtual ~CDnToggleChecker(void);

	// from IDnSkillUsableChecker
	bool CanUse( void );
	int GetNumArgument( void ) { return 1; };

	virtual IDnSkillUsableChecker* Clone();
};
