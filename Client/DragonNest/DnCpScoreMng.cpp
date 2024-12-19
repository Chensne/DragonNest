#include "StdAfx.h"
#include "DnCpScoreMng.h"
#include "DnCpScoreDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCpScoreMng::CDnCpScoreMng(void) :
// 	m_StaticPosXRatioConst(0.8f),
// 	m_StaticPosYRatioConst(0.5f),
	m_CpCache(0),
	m_CounterOffsetConst(1),
	m_CounterTimer(0.f),
	m_ShakeTimer(0.f),
	m_ShakeDecayTime(0.5f),
	m_CounterTimerOffsetConst(0.001f)
{
	m_pCurScoreCountDlg	= NULL;
	m_pScoreDlg			= NULL;
	m_bShowDlg			= false;
	m_bShakeTotalDlg	= false;
	m_CurScoreCountIndex = 0;

	int i = 0;
	for (; i < MAX_COUNT_DLG_BUFFER; ++i)
	{
		m_pScoreCountDlgBuffer[i] = NULL;
	}
}

CDnCpScoreMng::~CDnCpScoreMng(void)
{
	SAFE_DELETE(m_pScoreDlg);

	int i = 0;
	for (; i < MAX_COUNT_DLG_BUFFER; ++i)
	{
		SAFE_DELETE(m_pScoreCountDlgBuffer[i]);
	}
}

void CDnCpScoreMng::Initialize()
{
	m_pScoreDlg = new CDnCpScoreDlg;
	m_pScoreDlg->Initialize(false);

	int i = 0;
	for (; i < MAX_COUNT_DLG_BUFFER; ++i)
	{
		m_pScoreCountDlgBuffer[i] = new CDnCpScoreCountDlg;
		m_pScoreCountDlgBuffer[i]->Initialize(false);
	}
	m_CurScoreCountIndex = 0;
}

CDnCpScoreCountDlg* CDnCpScoreMng::GetCurScoreCountDlg()
{
	int curIdx = m_CurScoreCountIndex;
	m_CurScoreCountIndex = (m_CurScoreCountIndex + 1) % MAX_COUNT_DLG_BUFFER;
	return m_pScoreCountDlgBuffer[curIdx];
}

void CDnCpScoreMng::ShowCountDlg(bool bShowAction, bool bNewAllocate)
{
	if (bNewAllocate)
	{
		if (m_pCurScoreCountDlg)
		{
			//m_pCurScoreCountDlg->Hide();
			m_pCurScoreCountDlg->Show(false);
			m_pCurScoreCountDlg = NULL;
		}

		CDnCpScoreCountDlg *pDlg = GetCurScoreCountDlg();//m_CountDlgMemPool.Allocate();
		if (pDlg)
		{
			pDlg->Initialize(true);
			pDlg->Show(true);
			m_pCurScoreCountDlg = pDlg;
		}
		else
		{
			_ASSERT(0);
		}
	}

	if (m_pCurScoreCountDlg != NULL)
	{
		m_pCurScoreCountDlg->ShowCpScoreCount(m_CpCache, 0.95f, bShowAction);
		if (m_pScoreDlg)
		{
			SUICoord countDlgCoord;
			m_pCurScoreCountDlg->GetDlgCoord(countDlgCoord);

			if (m_CurScoreCountDlgCoordCache.Bottom() == 0.f && m_CurScoreCountDlgCoordCache.Right() == 0.f)
				m_CurScoreCountDlgCoordCache = countDlgCoord;
			m_pScoreDlg->ShowDlg(countDlgCoord);
		}
	}
}

void CDnCpScoreMng::SetShakeTotalScore()
{
	m_bShakeTotalDlg	= true;
	m_ShakeTimer		= 0.f;
	ShowCountDlg(false, false);
}

void CDnCpScoreMng::Reset()
{
	m_bShowDlg = false;
}

void CDnCpScoreMng::Process(float fDelta)
{
	if (CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeDungeon)
	{
		if (m_pCurScoreCountDlg && m_pCurScoreCountDlg->IsShow())
			m_pCurScoreCountDlg->Show(false);

		if (m_pScoreDlg && m_pScoreDlg->IsShow())
			m_pScoreDlg->Show(false);

		//if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage)
		GetInterface().ResetCp();
		m_CpCache = 0;
		m_bShowDlg = false;
		return;
	}

	if (GetInterface().IsOpenBlind())
	{
		m_bShowDlg = false;
		return;
	}

	if (m_bShowDlg == false)
	{
		ShowCpScore();
		if (m_pScoreDlg)
			m_pScoreDlg->Show(true);
	}

	if (m_bShakeTotalDlg)
	{
		m_ShakeTimer += fDelta;

		if (m_ShakeTimer < m_ShakeDecayTime)
		{
			const float freq = 18.f;
			const float amp = 2.f;
			float offset = sin(2.f * ET_PI * m_ShakeTimer * freq) * amp * (m_ShakeDecayTime - m_ShakeTimer) / m_ShakeDecayTime;
			offset = offset / m_pCurScoreCountDlg->GetScreenWidth();
			m_pCurScoreCountDlg->Shake(offset);
		}
		else
		{
			m_bShakeTotalDlg = false;
			m_pCurScoreCountDlg->SetDlgCoord(m_CurScoreCountDlgCoordCache);
			ShowCountDlg(false, false);
		}
	}

	const int& currentCp = CDnInterface::GetInstance().GetCp();

	if (currentCp > m_CpCache)
	{
		if (m_CounterTimer >= m_CounterTimerOffsetConst)
		{
			int offset = 1;
			wchar_t wszBuffer[256] = {0};
			_itow_s(currentCp - m_CpCache, wszBuffer, _countof(wszBuffer), 10);
			int nValueLen = (int)wcslen(wszBuffer);
			int i = 2, unit = 10;
			for (; i < nValueLen; ++i)
				offset += int(pow(unit, double(i-1)));

			m_CpCache = (currentCp > m_CpCache) ? m_CpCache + offset : m_CpCache - offset;
			//m_pCurScoreCountDlg->Hide();
			m_pCurScoreCountDlg->Show(false);
			m_pCurScoreCountDlg = NULL;

			ShowCountDlg(false, true);
			m_CounterTimer = 0.f;
		}

		m_CounterTimer += fDelta;
	}

	CDnCountMng<CDnCpScoreCountDlg>::Process(fDelta);
}

void CDnCpScoreMng::ShowCpScore()
{
	ShowCountDlg(false, true);

	m_bShowDlg = true;
	//m_CpCache = 0;
	m_bShakeTotalDlg = false;
}

void CDnCpScoreMng::OnCpDown()
{
	m_CpCache = CDnInterface::GetInstance().GetCp();

	SetShakeTotalScore();
}

SUICoord CDnCpScoreMng::GetCpScoreDlgCoord() const
{
	SUICoord coord;
	m_pScoreDlg->GetDlgCoord(coord);
	return coord;
}

SUICoord CDnCpScoreMng::GetCpScoreDlgBaseCoord() const
{
	SUICoord coord;
	coord = m_pScoreDlg->GetBaseBGCoord();
	return coord;
}

float CDnCpScoreMng::GetCpScorePointStaticWidth() const
{
	if (m_pScoreCountDlgBuffer[0] == NULL)
		return -1;

	return m_pScoreCountDlgBuffer[0]->GetCpPointStaticWidth();
}