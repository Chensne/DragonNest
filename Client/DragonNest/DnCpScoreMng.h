
#pragma once

#include "DnCountMng.h"
#include "DnCpScoreCountDlg.h"

class CDnCpScoreDlg;
class CDnCpDownDlg;
class CDnCpScoreMng : public CDnCountMng<CDnCpScoreCountDlg>
{
public:
	CDnCpScoreMng(void);
	virtual ~CDnCpScoreMng(void);

	void				Initialize();
	void				Process(float fDelta);
	void				ShowCpScore();
	void				OnCpDown();
	void				Reset();
	SUICoord			GetCpScoreDlgCoord() const;
	SUICoord			GetCpScoreDlgBaseCoord() const;
	float				GetCpScorePointStaticWidth() const;

private:
	void				ShowCountDlg(bool bShowAction, bool bNewAllocate);
	void				SetShakeTotalScore();
	CDnCpScoreCountDlg* GetCurScoreCountDlg();

	CDnCpScoreCountDlg*	m_pCurScoreCountDlg;
	CDnCpScoreDlg*		m_pScoreDlg;

#define MAX_COUNT_DLG_BUFFER 10
	CDnCpScoreCountDlg*	m_pScoreCountDlgBuffer[MAX_COUNT_DLG_BUFFER];
	int					m_CurScoreCountIndex;
	SUICoord			m_CurScoreCountDlgCoordCache;

	int					m_CpCache;
	float				m_ShakeTimer;
	float				m_CounterTimer;
	bool				m_bShowDlg;
	bool				m_bShakeTotalDlg;

//	float				m_MinusScoreDlgXRatio;
//	float				m_MinusScoreDlgYRatio;
// 	const float			m_StaticPosXRatioConst;
// 	const float			m_StaticPosYRatioConst;
	const int			m_CounterOffsetConst;
	const float			m_CounterTimerOffsetConst;
	const float			m_ShakeDecayTime;
};