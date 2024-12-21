#pragma once

#include "Task.h"

class CDnGameTipTask : public CTask, public CSingleton<CDnGameTipTask>
{
	enum
	{
		EXCEPTION_DELAY_TIME = 5,	// 뭔가 이상하면 5초뒤에 다시 처리.
		DELAY_TIME = 40,			// 기본 딜레이는 40초마다 한번.

		MIN_NO_BATTLE = 1301000,	// 비전투 메세지 범위 <= 로 검사.
		MAX_NO_BATTLE = 1303000,
	};

	enum
	{
		GRADE_ALL = 0,
		GRADE_NEW,
		GRADE_BEGINNER,
		GRADE_INTERMEDIATE,
	};

	enum
	{
		LEVEL_ALL = 0,
		LEVEL_NEW = 10,
		LEVEL_BEGINNER = 23
	};

	enum
	{
		OutputType_ALL = 0,
		OutputType_Custom = 1,
	};

	enum{
		MAP_COUNT = 5,
	};

	struct STipData
	{
		int  nStringID;
		std::vector<int> vMapIDs;

		STipData() : nStringID( -1 )
		{}
	};

public:
	CDnGameTipTask();
	virtual ~CDnGameTipTask();

	bool Initialize();
	void Finalize();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void ResetGameTip( float fCurDelayTime = 2.0f );	// 들어가자마자 바로 보이는것보다 낫다.

	int GetTodayTipStringID( int nMapID, int nLevel );
	int GetCustomStringID( int nMapID );

protected:
	std::vector<int> m_vecNoBattleStringID;
	std::vector<int> m_vecBattleStringID;

	std::vector<STipData> m_vecAll;				// 전체 레벨
	std::vector<STipData> m_vecNew;				// 입문
	std::vector<STipData> m_vecBeginner;		// 초급
	std::vector<STipData> m_vecIntermediate;	// 중급
	std::vector<STipData> m_vecCustom;			// 커스텀 맵

	float m_fDelayTime;
};

#define GetGameTipTask()	CDnGameTipTask::GetInstance()