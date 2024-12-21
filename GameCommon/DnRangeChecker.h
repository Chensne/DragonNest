#pragma once
#include "idnskillusablechecker.h"


// ���� + ��ų ��Ÿ��� ���� �ȿ� ���� �ִ��� üũ���ִ� ��ü. 
// Ŭ�󿡼��� ���Դϴ�.
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
