
#pragma once

namespace AICommand
{
	enum Command
	{
		CMD_USESKILL = 0,				// ��ų ���
		CMD_REMOVEBLOW_BYSKILLINDEX,	// �ش� ��ų�� �߻��� BLOW ����
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
