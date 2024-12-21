#pragma once
#include "IDnSkillUsableChecker.h"


// 이동 가능한 상태인지 체크
class CDnMovableChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnMovableChecker >
{
private:

protected:
#ifndef _GAMESERVER
	static bool s_bUseSignalSkillCheck;
	static bool s_PrimUseSignalSkillCheck;

	static LOCAL_TIME s_EndTime;			// 시그널 스킬 체크가 유효한 시간.
#endif

public:
	CDnMovableChecker( DnActorHandle hActor );
	virtual ~CDnMovableChecker(void);

#ifndef _GAMESERVER
	static void SetUseSignalSkillCheck( bool bUseSignalSkillCheck, LOCAL_TIME SignalEndTime );
	static bool GetUseSignalSkillCheck( void ) { return s_bUseSignalSkillCheck; };
	static LOCAL_TIME GetUseSkillSignalEnd( void ) { return s_EndTime; };

	static void SetPrimUseSignalSkillCheck( bool bCheck ) { s_PrimUseSignalSkillCheck = bCheck; };
#endif

	// from IDnSkillUsableChecker
	bool CanUse( void );
	int GetNumArgument( void ) { return 0; };

	virtual IDnSkillUsableChecker* Clone();
};
