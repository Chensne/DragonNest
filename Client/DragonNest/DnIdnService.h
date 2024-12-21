#pragma once
#include "DNDefine.h"

#include "DnServiceSetup.h"
#include "DnPacket.h"


#if defined(_ID) && defined(_AUTH)

class DnIdnService : public IServiceSetup
{
public:
	DnIdnService() {}
	virtual ~DnIdnService() {}

	virtual int	Initialize( void* pCustomData );
	virtual int	PreInitialize( void* pCustomData );

	void HShieldSetMyID( std::wstring strID );

private:
	std::wstring m_strId;
};

#endif // _ID