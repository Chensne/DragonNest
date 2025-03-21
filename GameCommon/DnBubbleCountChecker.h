#pragma once
#include "idnskillusablechecker.h"


// 특정 버블 ID 가 일정 갯수 이상이어야 스킬 사용 가능.
class CDnBubbleCountChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnBubbleCountChecker >
{
private:
	int m_iBubbleTypeID;
	int	m_iNeedBubbleCount;

public:
	CDnBubbleCountChecker( DnActorHandle hActor, int iBubbleTypeID, int iNeedBubbleCount );
	virtual ~CDnBubbleCountChecker(void);

	void SetBubbleTypeID( int iBubbleTypeID ) { m_iBubbleTypeID = iBubbleTypeID; }
	int GetBubbleTypeID( void ) { return m_iBubbleTypeID; };
	void SetNeedBubbleCount( int iBubbleCount ) { m_iNeedBubbleCount = iBubbleCount; };
	int GetNeedBubbleCount( void ) { return m_iNeedBubbleCount; };

	virtual bool CanUse( void );
	virtual int GetNumArgument( void ) { return 2; };

	virtual IDnSkillUsableChecker* Clone();
};
