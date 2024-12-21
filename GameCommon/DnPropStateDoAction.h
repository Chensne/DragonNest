#pragma once
#include "DnPropState.h"

class CDnPropStateDoAction : public CDnPropState, public TBoostMemoryPool< CDnPropStateDoAction >
{
private:
	vector<string> m_vlAvailActionList;
	bool m_bUseCmdAction;

public:
	CDnPropStateDoAction( DnPropHandle hEntity );
	virtual ~CDnPropStateDoAction(void);

	// 1개 이상이면 랜덤으로 돌림. 시드값은 프랍한테 받는다. 반드시 패킷으로 나간 클라랑 동일한 시드를 받아야 하기 땜시..
	void AddActionName( const char* pActionName ) { m_vlAvailActionList.push_back( string(pActionName) ); };
	void UseCmdAction( bool bUseCmdAction ) { m_bUseCmdAction = bUseCmdAction; };

	virtual bool OnEntry( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnLeave( LOCAL_TIME LocalTime, float fDelta );
};
