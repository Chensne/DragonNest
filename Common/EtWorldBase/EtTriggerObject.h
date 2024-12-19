#pragma once

class CEtTrigger;
class CEtTriggerElement;
#include "Timer.h"

class CEtTriggerObject 
{
public:
	CEtTriggerObject( CEtTrigger *pTrigger );
	virtual ~CEtTriggerObject();

	static int s_nDummySize;
#if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )
	LARGE_INTEGER liFrequency;
	LARGE_INTEGER liStartTime;
	LARGE_INTEGER liCurTime;
	double			m_dTime;
#endif // #if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )

protected:
	CEtTrigger *m_pTrigger;
	std::string m_szCategory;
	std::string m_szTriggerName;
	std::string m_szDescription;
	std::vector<CEtTriggerElement *> m_pVecEvent;
	std::vector<CEtTriggerElement *> m_pVecCondition;
	std::vector<CEtTriggerElement *> m_pVecAction;
	int m_nTick;
	bool m_bRepeat;
	bool m_bEnable;
	int m_nCreateIndex;
	int m_nRandomSeed;

	LOCAL_TIME m_LastProcessTime;

	std::map<std::string,int> m_MapEventValueStore;

	std::vector<int> m_nVecAndConditionList;
	std::vector<std::vector<int>> m_nVecOrConditionList;
	std::vector<int> m_nVecAndActionList;
	std::vector<std::vector<int>> m_nVecRandomActionList;
protected:
	void ResetTriggerEventStore();
	void CalcOperatorList();

public:
	virtual bool Load( CStream *pStream );
	virtual CEtTriggerElement *AllocTriggerElement();
	virtual bool Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool ProcessQueue( LOCAL_TIME LocalTime, float fDelta );

	bool IsCanProcess( LOCAL_TIME LocalTime, float fDelta );
	const char *GetTriggerName() { return m_szTriggerName.c_str(); }
	const char *GetCategory() { return m_szCategory.c_str(); }
	CEtTrigger *GetTrigger() { return m_pTrigger; }

	void SetTick( int nValue ) { m_nTick = nValue; }

	void SetRepeat( bool bValue ) { m_bRepeat = bValue; }
	bool IsRepeat() { return m_bRepeat; }

	void SetEnable( bool bValue ) { m_bEnable = bValue; }
	bool IsEnable() { return m_bEnable; }

	void SetCreateIndex( int nValue ) { m_nCreateIndex = nValue; }
	int GetCreateIndex() { return m_nCreateIndex; }

	DWORD GetEventCount() { return (DWORD)m_pVecEvent.size(); }
	DWORD GetConditionCount() { return (DWORD)m_pVecCondition.size(); }
	DWORD GetActionCount() { return (DWORD)m_pVecAction.size(); }
	CEtTriggerElement *GetEventFromIndex( DWORD dwIndex );
	CEtTriggerElement *GetConditionFromIndex( DWORD dwIndex );
	CEtTriggerElement *GetActionFromIndex( DWORD dwIndex );

	void InsertEventValueStore( const char *szValueStr, int nValue );
	int GetEventValueStore( const char *szValueStr );

	int GetRandomSeed() { return m_nRandomSeed; }
	void SetRandomSeed( int nValue ) { m_nRandomSeed = nValue; }
};

class IBoostPoolEtTriggerObject : public CEtTriggerObject, public TBoostMemoryPool< IBoostPoolEtTriggerObject >
{
public:
	IBoostPoolEtTriggerObject( CEtTrigger *pTrigger ):CEtTriggerObject( pTrigger ){}
	virtual ~IBoostPoolEtTriggerObject(){}
};
