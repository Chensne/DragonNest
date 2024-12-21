#pragma once
#include "DnWorldProp.h"


class CDnWorldCameraProp : public CDnWorldProp
{
public:
	CDnWorldCameraProp();
	virtual ~CDnWorldCameraProp();

protected:
	virtual bool CreateObject();

public:
};
