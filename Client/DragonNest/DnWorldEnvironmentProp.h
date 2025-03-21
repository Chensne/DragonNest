#pragma once

#include "DnWorldActProp.h"


// 클라이언트에서만 이벤트에 따른 액션을 취한다.
class CDnWorldEnvironmentProp : public CDnWorldActProp
{
private:
	float m_fRange;
	string m_strOnRangeInAction;
	string m_strOnRangeOutAction;

	bool m_bActorIsInRange;

protected:
	void ReleasePostCustomParam( void ) 
	{
		if( m_pData && m_bPostCreateCustomParam )
		{
			EnvironmentPropStruct* pEnvironmentPropStruct = static_cast<EnvironmentPropStruct*>( m_pData );
			SAFE_DELETE( pEnvironmentPropStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

public:
	CDnWorldEnvironmentProp( void );
	virtual ~CDnWorldEnvironmentProp( void );

	virtual bool InitializeTable( int nTableID );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnFinishAction( const char* szPrevAction, const char *szNextAction, LOCAL_TIME time );
};