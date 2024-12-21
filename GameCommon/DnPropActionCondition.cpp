#include "StdAfx.h"
#include "DnPropActionCondition.h"
#include "DnWorldActProp.h"

CDnPropActionCondition::CDnPropActionCondition( DnPropHandle hEntity ) : CDnPropCondition( hEntity )
{

}

CDnPropActionCondition::~CDnPropActionCondition(void)
{
}

bool CDnPropActionCondition::IsSatisfy( void )
{
	_ASSERT( dynamic_cast<CDnWorldActProp*>(m_hEntity.GetPointer()) );
	if( !m_hEntity )
		return false;

	const char* pCurrentActionName = static_cast<CDnWorldActProp*>(m_hEntity.GetPointer())->GetCurrentAction();
	
	return pCurrentActionName ? (m_strActionName == pCurrentActionName) : false;
}