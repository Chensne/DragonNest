
#pragma once

// 신청기간 -> 예선 -> 보상(본선포함) -> 끝
class CDNGuildWar
{

public:
	CDNGuildWar();
	virtual ~CDNGuildWar();

	virtual void Reset();
	virtual void Process();
	virtual void OnStartEvent();
	virtual void OnEndEvent();
	
	bool UpdateTime(short wScheduleID, time_t tStart, time_t tEnd, short wWinersWeightRate);
	bool IsValidPeriod();
	bool IsFinishPeriod();
	
	time_t GetEventTime(char cType);
	bool IsEventComplete(char cType);
	void SetEventComplete(char cType, bool bComplete);
	inline void SetDBJobSend(bool bSend) { m_bDBJobSend = bSend; };	
	inline void SetDBJobSeq(int nJobSeq) { m_nDBJobSeq = nJobSeq; };
	inline void SetDBJobSuccess(bool bSuccess) { m_bDBJobSuccess = bSuccess; };	
	inline bool GetDBJobSuccess() { return m_bDBJobSuccess; };

private:
	bool CheckEventType(char cType);

protected:
	time_t m_tEventMoment[GUILDWAR_EVENT_MAX];
	time_t m_tCurrent;

	bool m_bEventComplete[GUILDWAR_EVENT_MAX];

	char m_cEventStep;
	short m_wScheduleID;
	short m_wWinersWeightRate;

	int m_nDBJobSeq;												// DBJob Seq
	DWORD m_nDBJobSendTick;											// DBJob Tick
	bool m_bDBJobSend;
	bool m_bDBJobSuccess;
};

// 신청기간
class CDNGuildWarPreparation : public CDNGuildWar
{
public:
	CDNGuildWarPreparation();
	~CDNGuildWarPreparation();

	void Process();
};

// 예선기간
class CDNGuildWarTrial : public CDNGuildWar
{
public:
	CDNGuildWarTrial();
	~CDNGuildWarTrial();

	void Process();	

	DWORD m_nTrialPointSendTick;									// 예선전 순위 갱신 Tick
};

// 본선, 보상기간
class CDNGuildWarReward : public CDNGuildWar
{
private:
	time_t m_tFinalsStartMoment[GUILDWAR_FINALPART_MAX];			// 각 16강, 8강, 4강, 결승 시작시간(0은 버림)
	time_t m_tFinalsEndMoment[GUILDWAR_FINALPART_MAX];				// 각 16강, 8강, 4강, 결승 종료시간
	bool m_bFinalsComplete[GUILDWAR_FINALPART_MAX];					// 각 차수별 완료 여부
	char m_cCurrentFinals;											// 현재 차수	
	bool m_bFinalSchedule;											// 결승 스케쥴 셋팅 여부		
public:
	CDNGuildWarReward();
	~CDNGuildWarReward();

	bool m_bTrialStatsRequest;										// 예선결과 집계

	virtual void OnStartEvent();
	virtual void Reset();

	void Process();
	void SetFinalScheduleInfo(MASetGuildWarFinalSchedule* pData);
	char GetCurFinalPart();
	time_t GetCurFinalPartBeginTime();	
};
