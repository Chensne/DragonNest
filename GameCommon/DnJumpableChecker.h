#pragma once
#include "IDnSkillUsableChecker.h"



// �÷��̾ ������ �� �ִ� ��Ȳ����
class CDnJumpableChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnJumpableChecker >
{
protected:
#ifndef _GAMESERVER
	static bool s_bUseSignalSkillCheck;
	static bool s_PrimUseSignalSkillCheck;
	static LOCAL_TIME s_EndTime;			// �ñ׳� ��ų üũ�� ��ȿ�� �ð�.
#endif

public:
	CDnJumpableChecker( DnActorHandle hActor );
	virtual ~CDnJumpableChecker(void);

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
