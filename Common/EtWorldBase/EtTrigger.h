#pragma once

class CEtTriggerObject;
class CEtWorldSector;
class CEtTriggerElement;
#include "Timer.h"

class CEtTrigger {
public:
	CEtTrigger( CEtWorldSector *pSector );
	virtual ~CEtTrigger();

	enum ParamTypeEnum {
		Integer,
		Float,
		String,
		Operator,
		Position,
		Prop,
		EventArea,
		Value,

		ParamTypeEnum_Amount,
	};

	struct DefineValue : public TBoostMemoryPool< DefineValue >
	{
		std::string szName;
		ParamTypeEnum ParamType;
		int nUniqueID;
		union {
			int nValue;
			float fValue;
			char *szValue;
			void *pValue;
		};

		DefineValue() {
			ParamType = Integer;
			pValue = NULL;
			nUniqueID = -1;
		};
		virtual ~DefineValue() {
			if( ParamType == CEtTrigger::String ) SAFE_DELETEA( szValue );
		};
	};

protected:
	CEtWorldSector *m_pSector;
	std::vector<CEtTriggerObject *> m_pVecTriggerList;
	bool m_bEnable;
	LOCAL_TIME m_LocalTime;

	struct EventCallbackStruct {
		std::string szFuncName;
		std::map<std::string,bool> szMapStoreList;
		std::vector<CEtTriggerObject *> pVecObject;
	};
	std::vector<EventCallbackStruct> m_VecEventTrigger;
	std::vector<DefineValue *> m_pVecDefineValueList;
	std::vector<CEtTriggerObject *> m_pVecActionTriggerQueue;

#ifdef PRE_FIX_62845
	std::vector<std::string> m_vecSzErrLuaFile;
#endif

protected:
	void CalcEventCallback( CEtTriggerObject *pObject );
	virtual void FindAndRemoveEventCallbackObject( CEtTriggerObject *pObject );
	void InsertEventCallback( const char *szFuncName, CEtTriggerObject *pObject );
	virtual void RegisterEventParam( const char *szFuncName, int nDefineID, CEtTriggerObject *pObject, CEtTriggerElement *pEvent );

public:
	virtual bool Load( const char *szFileName );
	virtual bool LoadDefine( const char *szFileName );
	virtual CEtTriggerObject *AllocTriggerObject();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEventCallback( const char *szFuncName, LOCAL_TIME LocalTime, float fDelta, bool bProcessQueue = true );

	DWORD GetTriggerCount() { return (DWORD)m_pVecTriggerList.size(); }
	CEtTriggerObject *GetTriggerFromName( const char *szTriggerName );
	CEtTriggerObject *GetTriggerFromIndex( DWORD dwIndex );

	CEtWorldSector *GetSector() { return m_pSector; }

	void EnableEventCallback( bool bEnable );

	void RegisterDefineValue( lua_State *pState );
	virtual void OnCallTriggerAction( CEtTriggerObject *pObject ) {}

	void InsertActionQueue( CEtTriggerObject *pObject );
	bool IsProcessActionQueue( CEtTriggerObject *pObject );

	void InsertTriggerEventStore( const char *szValueStr, int nValue );

	virtual int Random() { return rand(); }
	virtual void RandomSeed( int nValue ) { srand(nValue); }

#ifdef PRE_FIX_62845
	bool IsDetectLuaError() { return !m_vecSzErrLuaFile.empty(); }
	void SetErrLuaName( std::string szFileName );
	void ShowErrLuaName();
#endif
};