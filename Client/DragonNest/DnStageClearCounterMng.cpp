#include "StdAfx.h"
#include "DnStageClearCounterMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define _MIN_ROLL_COUNT		1
#define _NORMAL_ROLL_COUNT	3

CDnStageClearCounterMng::CDnStageClearCounterMng(void)
	: m_CounterTime(0.2f)
	, m_RollDigitConst(5)
{
	m_CurrentDlg	= NULL;
	m_CounterTimer	= 0.f;
	m_CurrentPoint	= 0;
	m_TargetPoint	= 0;

	m_CurDigitNumber	= 0;
	m_DigitNumber		= 0;
	m_RollCounter		= 0;
	m_RollCount			= _NORMAL_ROLL_COUNT;

	m_bProcessing		= false;
	m_CurCounterOffset	= 1;
#ifdef _COUNT_RENEW
	m_CurEmptyCounterIdx = 0;
	ZeroMemory(m_CounterDlgs, sizeof(m_CounterDlgs));
#endif
}

CDnStageClearCounterMng::~CDnStageClearCounterMng(void)
{
#ifdef _COUNT_RENEW
	Clear();
#endif
}

#ifdef _COUNT_RENEW
void CDnStageClearCounterMng::Clear()
{
	int i = 0;
	for (; i < _COUNTER_ARRAY_COUNT; ++i)
		SAFE_DELETE(m_CounterDlgs[i]);
}

void CDnStageClearCounterMng::Reset()
{
	m_CurEmptyCounterIdx = 0;
	
	int i = 0;
	for (; i < _COUNTER_ARRAY_COUNT; ++i)
		m_CounterDlgs[i]->Show(false);
}

bool CDnStageClearCounterMng::Initialize()
{
	Clear();

	int i = 0;
	for (; i < _COUNTER_ARRAY_COUNT; ++i)
	{
		m_CounterDlgs[i] = new CDnStageClearCounterDlg;
		m_CounterDlgs[i]->Initialize(false);
	}

	Reset();

	return true;
}

int CDnStageClearCounterMng::GetCounterEmptyIdx()
{
	int ret = m_CurEmptyCounterIdx;
	(++m_CurEmptyCounterIdx) %= _COUNTER_ARRAY_COUNT;
	return ret;
}
#endif // _COUNT_RENEW

void CDnStageClearCounterMng::ShowCountDlg(bool bShowPlus)
{
#ifdef _COUNT_RENEW
	CDnStageClearCounterDlg *pDlg = m_CounterDlgs[GetCounterEmptyIdx()];
	if (pDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	pDlg->ShowPlusSign(bShowPlus);
	pDlg->Set(m_CurrentPoint, m_DlgInfo.color);
	pDlg->Show( true );
#else
	CDnStageClearCounterDlg *pDlg = m_CountDlgMemPool.Allocate();
	pDlg->Initialize( true );
	m_listCountDlg.push_back( pDlg );
	pDlg->ShowPlusSign(bShowPlus);
	pDlg->Set(m_CurrentPoint, m_DlgInfo.color);
	pDlg->Show( true );
#endif

	m_CurrentDlg = pDlg;
}

#define _TOO_SLOW_MACHINE_DELTA_LIMIT 0.05f

void CDnStageClearCounterMng::Process(float fDelta)
{
	if (m_CurrentDlg == NULL)
		return;

	if (fDelta >= _TOO_SLOW_MACHINE_DELTA_LIMIT)
		m_CurrentPoint = m_TargetPoint;

	if (m_TargetPoint != m_CurrentPoint)
	{
		int offset = int(pow(10, m_CurDigitNumber + 1));
		int currentRest = m_CurrentPoint % offset;
		int targetRest = m_TargetPoint % offset;
		if (currentRest == targetRest)
		{
			m_RollCount = (m_DigitNumber > m_RollDigitConst && m_CurDigitNumber < (m_DigitNumber - m_RollDigitConst)) ? _MIN_ROLL_COUNT : _NORMAL_ROLL_COUNT;
			OutputDebug("digit num : %d / cur digit : %E/ roll cnt : %d m_CounterTimer >= m_CounterTime : %s\n", m_DigitNumber, m_CurDigitNumber, m_RollCount,
				(m_CounterTimer >= m_CounterTime) ? "T" : "F");

			if (m_RollCounter == m_RollCount)
			{
				++m_CurDigitNumber;
				m_RollCounter = 0;
// 				OutputDebug("counter timer %f -> 0\n", m_CounterTimer);
				m_CounterTimer = 0.f;
				m_CurCounterOffset = (m_CurDigitNumber != 0) ? int(pow(10, m_CurDigitNumber)) : 1;
			}
			else
			{
				m_RollCounter++;
			}
		}
		else
		{
			if (m_CounterTimer >= m_CounterTime)
			{
// 				OutputDebug("counter timer overs the limit : %f\n", m_CounterTimer);
				m_CurrentPoint = m_CurrentPoint - currentRest + targetRest;

				++m_CurDigitNumber;
				m_RollCounter = 0;
				m_CounterTimer = 0.f;
				m_CurCounterOffset = (m_CurDigitNumber != 0) ? int(pow(10, m_CurDigitNumber)) : 1;
			}
		}

		m_CurrentPoint = (m_TargetPoint > m_CurrentPoint) ? m_CurrentPoint + m_CurCounterOffset : m_CurrentPoint - m_CurCounterOffset;
		m_CurrentDlg->Hide();
		m_CurrentDlg->Show(false);

		ShowCountDlg(false);

		m_CounterTimer += fDelta;
// 		OutputDebug("counter timer : %f , %f\n", m_CounterTimer, fDelta);
	}
	else if (m_TargetPoint == m_CurrentPoint)
	{
		m_CurrentDlg->Hide();
		m_CurrentDlg->Show(false);
		ShowCountDlg(m_DlgInfo.withSign);
		m_bProcessing = false;
	}

#ifdef _COUNT_RENEW
#else
	CDnCountMng<CDnStageClearCounterDlg>::Process(fDelta);
#endif
}

void CDnStageClearCounterMng::Set(int point, COLOR_TYPE colorType, bool withSign, bool bCounting, int initPoint)
{
	m_CurrentPoint = bCounting ? initPoint : point;
	m_TargetPoint = point;

	m_CurDigitNumber = 0;
	wchar_t wszBuffer[256] = {0};
	_itow_s(m_TargetPoint, wszBuffer, _countof(wszBuffer), 10);
	m_DigitNumber = (int)wcslen(wszBuffer);	
	m_RollCounter = 0;
	m_RollCount = _NORMAL_ROLL_COUNT;
	m_CounterTimer = 0.f;
	m_CurCounterOffset = 1;

	m_DlgInfo.Set(colorType, withSign);

	ShowCountDlg(false);

	m_bProcessing = true;
}

void CDnStageClearCounterMng::Hide()
{
	if (m_CurrentDlg)
	{
		m_CurrentDlg->Hide();
		m_CurrentDlg->Show(false);
		m_CurrentDlg = NULL;
	}
}