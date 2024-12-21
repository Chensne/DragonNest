#pragma once

#include "EtUIControl.h"

class CDnCpJudgeControl : public CEtUIControl
{
public:
	//	순서는 control 의 element 순
	enum eCpJudgeType
	{
		JUDGE_NONE = -1,
		JUDGE_AERIALCOMBO,
		JUDGE_SKILLCOMBO,
		JUDGE_CRITICAL,
		JUDGE_DOWNATTACK,
		JUDGE_FINISHATTACK,
		JUDGE_GENOCIDE,
		JUDGE_SHIELDBREAK,
		JUDGE_STUN,
		JUDGE_SUPERARMORBREAK,
		JUDGE_RESCUE,
		JUDGE_PROPBREAK,
		JUDGE_GETITEM,
		JUDGE_MAX
	};

	CDnCpJudgeControl(CEtUIDialog* pParent);
	virtual ~CDnCpJudgeControl(void);

protected:
	eCpJudgeType	m_JudgeType;
	DWORD			m_Color;

public:
	virtual void	Render( float fElapsedTime );

	void			SetJudge(eCpJudgeType type);
	void			SetColor(DWORD color)		{ m_Color = color; }
	DWORD			GetColor() const			{ return m_Color; }
};
