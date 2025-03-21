#pragma once

#include "EtTrigger.h"

class CDnTrigger : public CEtTrigger
{
public:
	CDnTrigger( CEtWorldSector *pSector );
	virtual ~CDnTrigger();

protected:
	bool m_bRegisterLuaAPI;
	lua_State *m_pLuaState;

public:
	virtual CEtTriggerObject *AllocTriggerObject();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void CallActionTrigger( int nObjectIndex, int nRandomSeed );

	lua_State *GetLuaState() { return m_pLuaState; }
	bool IsRegisterLuaAPI() { return m_bRegisterLuaAPI; }
	void SetRegisterLuaAPI( bool bEnable ) { m_bRegisterLuaAPI = bEnable; }
	// 클라이언트는 자기가 체크해서 하는게 없기땜시 상속받아서 비워준다.
	virtual void OnEventCallback( const char *szFuncName, LOCAL_TIME LocalTime, float fDelta, bool bProcessQueue = true );

	int Random();
	void RandomSeed( int nValue );
};