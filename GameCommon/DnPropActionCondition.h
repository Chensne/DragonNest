#pragma once
#include "DnPropCondition.h"


// 프랍의 현재 액션을 체크해 전이 시킴
class CDnPropActionCondition : public CDnPropCondition, 
							   public TBoostMemoryPool<CDnPropActionCondition>
{
private:
	string m_strActionName;

public:
	CDnPropActionCondition( DnPropHandle hEntity );
	virtual ~CDnPropActionCondition(void);

	void SetActionName( const char* pActionName ) 
	{  
		if( pActionName )
			m_strActionName.assign( pActionName );
	};

	virtual bool IsSatisfy( void );
};
