#pragma once
#include "idnskillusablechecker.h"


// 무기 + 스킬 사거리로 범위 안에 적이 있는지 체크해주는 객체. 
// 클라에서만 쓰입니다.
class CDnRangeChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnRangeChecker >
{
private:
	int	m_iRange;

public:
	CDnRangeChecker( DnActorHandle hActor );
	virtual ~CDnRangeChecker(void);

	void SetRange( int iRange ) { m_iRange = iRange; };
	int GetRange( void ) { return m_iRange; };

	virtual bool CanUse( void );
	virtual int GetNumArgument( void ) { return 0; };

	virtual IDnSkillUsableChecker* Clone();
};
