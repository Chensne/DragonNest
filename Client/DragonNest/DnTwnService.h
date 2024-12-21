#pragma once
#include "DNDefine.h"

#include "DnServiceSetup.h"
#include "DnPacket.h"


#if defined(_TW) && defined(_AUTH)

class DnTwnService : public IServiceSetup
{
public:
	DnTwnService() : m_bWebLogin( FALSE ) {}
	virtual ~DnTwnService() {}

	virtual int	Initialize( void* pCustomData );
	virtual int	PreInitialize( void* pCustomData );

public:
	BOOL IsWebLogin() { return m_bWebLogin; }

private:
	std::wstring m_strId;
	std::wstring m_strPassword;
	BOOL m_bWebLogin;
};

#endif // _TW