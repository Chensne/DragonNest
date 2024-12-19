#include "StdAfx.h"
#include "DnTriggerObject.h"
#include "DnTriggerElement.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTriggerObject::CDnTriggerObject( CEtTrigger *pTrigger )
: CEtTriggerObject( pTrigger )
{
}

CDnTriggerObject::~CDnTriggerObject()
{
}

CEtTriggerElement *CDnTriggerObject::AllocTriggerElement()
{
	return new CDnTriggerElement( this );
}
