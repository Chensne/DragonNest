#pragma once

#include "DnCamera.h"

class CDnStaticCamera : public CDnCamera {
public:
	CDnStaticCamera();

protected:

public:
	virtual bool Activate();
	virtual bool DeActivate();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};