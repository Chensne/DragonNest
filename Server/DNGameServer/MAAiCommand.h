
#pragma once

namespace AICommand
{
	enum Command
	{
		CMD_USESKILL = 0,				// 스킬 사용
		CMD_REMOVEBLOW_BYSKILLINDEX,	// 해당 스킬로 발생된 BLOW 제거
		CMD_MAX,
	};

	typedef struct tagUseSkill
	{
		int iSkillIndex;
	}SUseSkill;

	typedef SUseSkill	SRemoveBlowBySkillIndex;
};

class MAAiScript;

class MAAiCommand:public TBoostMemoryPool<MAAiCommand>
{
public:

	void MsgProc( MAAiScript* pAIScript, const UINT uiMsg, void* pParam );
};
