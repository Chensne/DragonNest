#pragma once
#include "DnCountMng.h"
#include "DnStageClearCounterDlg.h"

#define _COUNT_RENEW

#define _COUNTER_ARRAY_COUNT	5

#ifdef _COUNT_RENEW
class CDnStageClearCounterMng
#else
class CDnStageClearCounterMng : public CDnCountMng<CDnStageClearCounterDlg>
#endif
{
public:
	struct SCounterDlgInfo 
	{
		COLOR_TYPE	color;
		bool		withSign;

		SCounterDlgInfo() : color(COLOR_NONE), withSign(false) {}
		void Set(COLOR_TYPE _color, bool _sign)
		{
			color	 = _color;
			withSign = _sign;
		}
	};
	CDnStageClearCounterMng(void);
	virtual ~CDnStageClearCounterMng(void);

#ifdef _COUNT_RENEW
	bool						Initialize();
	void						Reset();
#endif

	void						Set(int point, COLOR_TYPE colorType, bool withSign = false, bool bCounting = true, int initPoint = 0);
	void						Process(float fDelta);
	bool						IsProcessing() const	{ return m_bProcessing; }
	void						Hide();

private:
	void						ShowCountDlg(bool bShowPlus);

#ifdef _COUNT_RENEW
	void						Clear();
	int							GetCounterEmptyIdx();

	CDnStageClearCounterDlg*	m_CounterDlgs[_COUNTER_ARRAY_COUNT];
	int							m_CurEmptyCounterIdx;
#endif

	int							m_TargetPoint;
	int							m_CurrentPoint;
	int							m_DigitNumber;
	double						m_CurDigitNumber;
	int							m_CurCounterOffset;
	int							m_RollCounter;
	SCounterDlgInfo				m_DlgInfo;
	CDnStageClearCounterDlg*	m_CurrentDlg;
	float						m_CounterTimer;

	const float					m_CounterTime;
	int							m_RollCount;
	const int					m_RollDigitConst;
	bool						m_bProcessing;
};