#pragma once
/*
	GPU의 성능을 대략적으로 짐작해본다.
	최초 실행시 그래픽 옵션 (상,중,하) 자동설정 용도.
*/
#include "Singleton.h"
class CEtBenchMark : public CSingleton<CEtBenchMark>
{
public:
	enum PerfGrade {
		PF_Unknown = -1,
		PF_Low,		
		PF_Middle,
		PF_High,
	};

public:
	CEtBenchMark();
	~CEtBenchMark();

private:
	PerfGrade		m_PerfGrade;
	DWORD			m_dwScore;

	bool				m_bRun;

public:
	void	Run();
	void Process();
	void Reset();
	PerfGrade GetResult( DWORD *pScore = NULL );
};
