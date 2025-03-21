#pragma once

#include "EtUIType.h"

class CEtUIControl;
class CEtUIDialog;

class CEtUIControlCreator
{
public:
	CEtUIControlCreator(void);
	~CEtUIControlCreator(void);

public:
	CEtUIControl* CreateControl( UI_CONTROL_TYPE Type, CEtUIDialog *pParent );
};
