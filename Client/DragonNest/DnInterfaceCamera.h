#pragma once

#include "DnCamera.h"

class CDnInterfaceCamera : public CDnCamera
{
public:
	CDnInterfaceCamera();
	virtual ~CDnInterfaceCamera();

protected:

public:
	virtual bool Activate();
	virtual bool DeActivate();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};