#pragma once

#include "Timer.h"

namespace TimeImp {
	static float m_fCriterionTime = 16.66666666f;
	float GetTimeVelocity( float fVelocity, LOCAL_TIME LocalTime, LOCAL_TIME PrevTime );
	float GetTimeVelocity( float fVelocity, float fDelta );
};