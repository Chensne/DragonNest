#pragma once

class CDnWorldProp;


struct SPlantInfo
{
	float m_fHarvestTime;
	float m_fWaterTime;
	CDnWorldProp * m_pProp;
	Farm::AreaState::eState	m_eState;
	time_t m_tTime;
};

