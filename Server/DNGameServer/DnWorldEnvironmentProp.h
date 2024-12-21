#pragma once
#include "dnworldactprop.h"

// 이 친구는 서버에서 하는 일이 없음. 
// 클라에서 오로지 꾸미기 용.
class CDnWorldEnvironmentProp : public CDnWorldActProp, public TBoostMemoryPool< CDnWorldEnvironmentProp >
{
public:
	CDnWorldEnvironmentProp( CMultiRoom* pRoom );
	virtual ~CDnWorldEnvironmentProp(void);

	void ReleasePostCustomParam( void ) 
	{ 
		if( m_pData && m_bPostCreateCustomParam )
		{
			EnvironmentPropStruct* pEnvironmentPropStruct = static_cast<EnvironmentPropStruct*>( m_pData );
			SAFE_DELETE( pEnvironmentPropStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};
};


