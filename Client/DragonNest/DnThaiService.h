#pragma once
#include "DNDefine.h"

#include "DnServiceSetup.h"
#include "DnPacket.h"


#if defined(_TH) && defined(_AUTH)

class DnThaiService : public IServiceSetup
{
public:
	DnThaiService() {}
	virtual ~DnThaiService() {}

	virtual int	Initialize( void* pCustomData );
	virtual int	PreInitialize( void* pCustomData );

	void HShieldSetMyID( std::wstring strID );

private:
	std::wstring m_strId;
};

#endif // _TW