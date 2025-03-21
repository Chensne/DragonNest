#pragma once
#include "DNDefine.h"

#include "DnServiceSetup.h"
#include "DnPacket.h"


#if defined(_RU) && defined(_AUTH)

class DnRusService : public IServiceSetup
{
public:
	DnRusService() {}
	virtual ~DnRusService() {}

	virtual int	Initialize( void* pCustomData );
	virtual int	PreInitialize( void* pCustomData );

	void HShieldSetMyID( std::wstring strID );

private:
	std::wstring m_strId;
	std::wstring m_strPassword;
};

#endif // _RU, _AUTH