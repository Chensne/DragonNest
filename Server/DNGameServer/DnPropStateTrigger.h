#pragma once
#include "DnPropState.h"

class CDnPropStateTrigger : public CDnPropState, public TBoostMemoryPool< CDnPropStateTrigger >
{
private:
	string m_strTriggerFuncName;

	struct ParamStruct {
		string szParamName;
		int nValue;
	};
	DNVector(ParamStruct) m_VecParamList;

public:
	CDnPropStateTrigger( DnPropHandle hEntity );
	virtual ~CDnPropStateTrigger(void);

	void SetFuncName( const char* pTriggerFuncName ) { m_strTriggerFuncName.assign(pTriggerFuncName); };
	void AddFuncParam( const char *pParamName, int nValue );

	virtual bool OnEntry( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnLeave( LOCAL_TIME LocalTime, float fDelta );
};
