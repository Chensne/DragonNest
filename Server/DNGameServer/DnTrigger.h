#pragma once

#include "EtTrigger.h"

class CDnTrigger : public CEtTrigger, public TBoostMemoryPool< CDnTrigger >
{
public:
	CDnTrigger( CEtWorldSector *pSector );
	virtual ~CDnTrigger();

protected:
	lua_State *m_pLuaState;
	bool m_bRegisterLuaAPI;

protected:

public:
	virtual CEtTriggerObject *AllocTriggerObject();

	virtual void OnCallTriggerAction( CEtTriggerObject *pObject );

	lua_State *GetLuaState() { return m_pLuaState; }
	bool IsRegisterLuaAPI() { return m_bRegisterLuaAPI; }
	void SetRegisterLuaAPI( bool bEnable ) { m_bRegisterLuaAPI = bEnable; }

	virtual int Random();
	virtual void RandomSeed( int nValue );
};

#if defined( PRE_TRIGGER_TEST )

class CDnTestTrigger : public CDnTrigger
{
public:
	CDnTestTrigger( CEtWorldSector *pSector ):CDnTrigger(pSector){}
	virtual void OnEventCallback( const char *szFuncName, LOCAL_TIME LocalTime, float fDelta, bool bProcessQueue );
	virtual void FindAndRemoveEventCallbackObject( CEtTriggerObject *pObject );
};

#endif // #if defined( PRE_TRIGGER_TEST )
