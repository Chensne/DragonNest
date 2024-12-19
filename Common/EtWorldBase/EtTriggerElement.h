#pragma once

#include "EtTrigger.h"
class CEtTriggerObject;
class CEtTriggerElement {
public:
	CEtTriggerElement( CEtTriggerObject *pObject, lua_State *pState );
	virtual ~CEtTriggerElement();

	enum TriggerElementTypeEnum {
		Condition,
		Action,
		Event,

		TriggerElementTypeEnum_Amount,
	};
	enum ScriptTypeEnum {
		ScriptFile,
		CustomScript,
	};

	enum OperatorTypeEnum {
		And,
		Or,
		Random,
	};
	struct ParamStruct {
		CEtTrigger::ParamTypeEnum Type;
		union {
			int nValue;
			char *szValue;
			float fValue;
			EtVector3 *vValue;
		};
	};

protected:
	bool m_bSelfOpenLua;
	bool m_bEnable;
	TriggerElementTypeEnum m_Type;
	ScriptTypeEnum m_ScriptType;
	OperatorTypeEnum m_OperatorType;
	int m_nOperatorIndex;
	int m_nOperatorValue[4];

	CEtTriggerObject *m_pTriggerObj;

	std::string m_szScriptName;
	std::string m_szCustomScript;
	std::string m_szFuncName;
	lua_State *m_pLuaState;
	std::vector<ParamStruct> m_VecParamList;

	static char *s_szFuncString[TriggerElementTypeEnum_Amount];

	LOCAL_TIME m_LastExecuteTime;
protected:
	void ReleaseParamList();

	virtual void RegisterLuaApi();

public:
	virtual bool Load( CStream *pStream );
	virtual bool Initialize();
	TriggerElementTypeEnum GetType() { return m_Type; }
	ScriptTypeEnum GetScriptType() { return m_ScriptType; }

	const char *GetFileName() { return m_szScriptName.c_str(); }
	const char *GetCustomScript() { return m_szCustomScript.c_str(); }

	DWORD GetParameterCount() { return (DWORD)m_VecParamList.size(); }
	ParamStruct *GetParameterStruct( DWORD dwIndex );

	CEtTriggerObject *GetTriggerObject() { return m_pTriggerObj; }

	void SetEnable( bool bValue ) { m_bEnable = bValue; }
	bool IsEnable() { return m_bEnable; }

	OperatorTypeEnum GetOperatorType() { return m_OperatorType; }
	int GetOperatorIndex() { return m_nOperatorIndex; }
	int GetOperatorValue( int nIndex ) { return m_nOperatorValue[nIndex]; }
	void SetOperatorType( OperatorTypeEnum Type ) { m_OperatorType = Type; }
	void SetOperatorIndex( int nValue ) { m_nOperatorIndex = nValue; }
	void SetOperatorValue( int nIndex, int nValue ) { m_nOperatorValue[nIndex] = nValue; }

	template < typename T >
	T GetLuaVariable( const char *szVariable ) {
		if( m_pLuaState == NULL ) return NULL;
		return lua_tinker::get<T>( m_pLuaState, szVariable );
	};

	virtual bool CallLuaFunction();

	void SetLastExcuteTime( LOCAL_TIME LocalTime ) { m_LastExecuteTime = LocalTime; }
	LOCAL_TIME GetLastExcuteTime() { return m_LastExecuteTime; }
};

class IBoostPoolEtTriggerElement :public CEtTriggerElement, public TBoostMemoryPool< IBoostPoolEtTriggerElement >
{
public:
	IBoostPoolEtTriggerElement( CEtTriggerObject *pObject, lua_State *pState ):CEtTriggerElement( pObject, pState ){}
	virtual ~IBoostPoolEtTriggerElement(){}
};
