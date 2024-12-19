#pragma once

#include "DNCommonDef.h"

class CDNUserSession;
class CDNCommonVariable
{
private:
	CDNUserSession *m_pSession;
	std::map<CommonVariable::Type::eCode, CommonVariable::Data> m_mVariableData;

public:
	CDNCommonVariable(CDNUserSession *pSession);
	~CDNCommonVariable(void);

	bool InitializeData( const TAGetListVariableReset* pA );
	bool ModData( CommonVariable::Type::eCode Type, INT64 biValue, __time64_t tDate = 0 );
	bool GetDataValue( CommonVariable::Type::eCode Type, INT64& biValue );
	bool GetDataModDate( CommonVariable::Type::eCode Type, __time64_t& tDate );
	bool IncData( CommonVariable::Type::eCode Type, __time64_t tDate = 0 );

	void SendCommonVariableData(CommonVariable::Type::eCode Type);
};
