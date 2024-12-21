
#pragma once

class MAAiScript;

class CDNAggroRange
{
public:

	CDNAggroRange();

	void	Initialize( CDnMonsterActor* pMonsterActor, DNTableFileFormat* pSox );
	float	GetThreatRange();
	float	GetCognizanceThreatRange();
	float	GetCognizanceThreatRangeSq();
	float	GetCognizanceGentleRangeSq();

private:

	MAAiScript* m_pAI;

	float m_fThreatRange;
	float m_fCognizanceThreatRange;
	float m_fCognizanceGentleRange;
};
