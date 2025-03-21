#include "StdAfx.h"
#include "DnCpJudgeMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCpJudgeMng::CDnCpJudgeMng(void) : 
	m_DlgPosXRatioConst(0.80f),
	m_DlgPosYRatioConst(0.45f),
	m_DlgOffsetYConst(-0.03f), 
	m_CpAddWaitingSecConst(2.f),
	m_DlgHideWaitingOffset(0.075f)
{
// 	m_TestColors[0] = textcolor::WHITE;
// 	m_TestColors[1] = textcolor::LIGHTSKYBLUE;
// 	m_TestColors[2] = textcolor::RED;
// 	m_TestColors[3] = textcolor::YELLOW;
// 	m_TestColors[4] = textcolor::PALEGREEN;

	SetSlotPos(m_DlgPosXRatioConst, m_DlgPosYRatioConst);

	m_CpAddWaitingTime	= 0.f;
	m_bCpAdding			= false;
	m_CurAccumulateCp	= 0;
	m_DlgHideTargetY	= 0.2f;//m_DlgPosYRatioConst - m_DlgOffsetYConst;
}

CDnCpJudgeMng::~CDnCpJudgeMng(void)
{
}

void CDnCpJudgeMng::Initialize()
{
	SUICoord standard = GetInterface().GetCpScoreDlgCoord();

	SetSlotPos(standard.fX, standard.fY - (standard.fHeight + 0.02f));
}

CDnCpJudgeMng::SSlotInfo* CDnCpJudgeMng::GetEmptySlot()
{
	int i = 0;
	for (; i < _MAX_CP_JUDGE_SLOT; ++i)
	{
		if (m_Slots[i].IsEmpty())
			return &(m_Slots[i]);
	}

	return NULL;
}

void CDnCpJudgeMng::SetSlotPos(float initX, float initY)
{
	m_Slots[0].pos.x = initX;	// temp
	m_Slots[0].pos.y = initY;
	int i = 1;
	for (; i < _MAX_CP_JUDGE_SLOT; ++i)
	{
		m_Slots[i].pos.x = m_Slots[0].pos.x;
		m_Slots[i].pos.y = m_Slots[i-1].pos.y + m_DlgOffsetYConst;
	}
}

void CDnCpJudgeMng::Process(float fElapsedTime)
{
	if (IsCpAdding())
	{
		m_CpAddWaitingTime -= fElapsedTime;

		if (m_CpAddWaitingTime <= 0.f)
			FinishCpAdd();
	}

	CDnCountMng<CDnCpJudgeDlg>::Process(fElapsedTime);
}

void CDnCpJudgeMng::ExtendTimeCpAddDlgs()
{
	m_CpAddWaitingTime = m_CpAddWaitingSecConst;
}

void CDnCpJudgeMng::EmptySlots()
{
	int i = 0;
	for (; i < _MAX_CP_JUDGE_SLOT; ++i)
	{
		if (m_Slots[i].dlg)
		{
			m_Slots[i].dlg->Hide(m_DlgHideWaitingOffset * i);
			m_Slots[i].Clear();
		}
	}
}

void CDnCpJudgeMng::SetCpJudge(CDnInterface::eCpJudgeType type, int nValue)
{
	static int i = 0;

	CDnCpJudgeDlg *pDlg = m_CountDlgMemPool.Allocate();
	pDlg->Initialize( true );
	m_listCountDlg.push_back( pDlg );

	SSlotInfo* emptySlot = GetEmptySlot();
	if (emptySlot == NULL)
	{
		FinishCpAdd();
		emptySlot = GetEmptySlot();
		if (emptySlot == NULL)
		{
			assert(0);
			return;
		}
	}

	if (IsCpAdding())
	{
		m_CurAccumulateCp += nValue;
		ExtendTimeCpAddDlgs();
	}
	else
	{
		m_CurAccumulateCp = nValue;
		StartCpAdd();
	}

	SSlotInfo& info = *emptySlot;
	info.dlg = pDlg;

	pDlg->SetCpJudge(type, info.pos.x, info.pos.y, m_DlgHideTargetY, m_CpAddWaitingSecConst * 1000.f);//m_TestColors[i]
	pDlg->ShowCount( true );
	pDlg->Show( true );

	i++;
	i = i % 5;
}

void CDnCpJudgeMng::StartCpAdd()
{
	m_bCpAdding = true;
	m_CpAddWaitingTime = m_CpAddWaitingSecConst;
}

void CDnCpJudgeMng::FinishCpAdd()
{
	m_bCpAdding = false;
	EmptySlots();

	//	test by kalliste
	GetInterface().SetCpUp(m_CurAccumulateCp);
}
