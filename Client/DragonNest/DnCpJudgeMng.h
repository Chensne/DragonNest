#pragma once
#include "DnCountMng.h"
#include "DnCpJudgeDlg.h"

#define _MAX_CP_JUDGE_SLOT	4

class CDnCpJudgeMng : public CDnCountMng<CDnCpJudgeDlg>
{
private:
	struct SSlotInfo
	{
		EtVector2		pos;
		CDnCpJudgeDlg*	dlg;

		SSlotInfo()
		{
			pos.x	= 0.f;
			pos.y	= 0.f;
			dlg		= NULL;
		}
		bool IsEmpty() const	{ return (dlg == NULL); }
		void Clear()			{ dlg = NULL; }
	};

public:
	CDnCpJudgeMng(void);
	virtual ~CDnCpJudgeMng(void);

	void		Initialize();
	void		SetCpJudge(CDnInterface::eCpJudgeType type, int nValue);
	void		Process(float fElapsedTime);

private:
	void		StartCpAdd();
	void		FinishCpAdd();
	bool		IsCpAdding() const { return m_bCpAdding; }

	void		SetSlotPos(float initX, float initY);

	SSlotInfo*	GetEmptySlot();
	void		EmptySlots();
	void		ExtendTimeCpAddDlgs();

	//DWORD		m_TestColors[5];
	SSlotInfo	m_Slots[_MAX_CP_JUDGE_SLOT];
	float		m_CpAddWaitingTime;
	bool		m_bCpAdding;
	int 		m_CurAccumulateCp;
	float		m_DlgHideTargetY;

	const float m_DlgPosXRatioConst;
	const float m_DlgPosYRatioConst;
	const float	m_DlgOffsetYConst;
	const float m_DlgHideWaitingOffset;
	const float m_CpAddWaitingSecConst;
};