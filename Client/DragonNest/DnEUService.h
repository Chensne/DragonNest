#pragma once
#include "DNDefine.h"

#include "DnServiceSetup.h"
#include "DnPacket.h"


#if defined(_EU) && defined(_AUTH)

class DnEUService : public IServiceSetup
{
public:
	DnEUService() {}
	virtual ~DnEUService() {}

	virtual int	Initialize( void* pCustomData );
	virtual int	PreInitialize( void* pCustomData );

	void HShieldSetMyID( std::wstring strID );

private:
	std::wstring m_strId;
};

#endif // _EU & _AUTH