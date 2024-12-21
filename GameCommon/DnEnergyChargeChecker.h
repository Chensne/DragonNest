#pragma once
#include "idnskillusablechecker.h"


// 클러릭의 에너지 차지 스킬을 사용한 상태인지 체크
class CDnEnergyChargeChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnEnergyChargeChecker >
{
private:


public:
	CDnEnergyChargeChecker( DnActorHandle hActor );
	virtual ~CDnEnergyChargeChecker(void);

	// from IDnSkillUsableChecker
	bool CanUse( void );
	int GetNumArgument( void ) { return 0; };

	virtual IDnSkillUsableChecker* Clone();
};
