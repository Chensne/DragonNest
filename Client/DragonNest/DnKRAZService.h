#pragma once
#include "DNDefine.h"

#include "DnServiceSetup.h"
#include "DnPacket.h"


#if defined(_KRAZ) && defined(_AUTH)

class DnKRAZService : public IServiceSetup
{
public:
	DnKRAZService();
	virtual ~DnKRAZService();

	virtual int	Initialize( void* pCustomData );
	virtual int	PreInitialize( void* pCustomData );
	void		HShieldSetMyID( std::wstring strID );
	bool		CmdLineParse( TCHAR *szString );

	std::wstring GetKeyID()			{ return m_keyID; }
	std::wstring GetRequestTime()	{ return m_RequestTime; }

private:
	std::wstring m_strId;
	std::wstring m_keyID;
	std::wstring m_RequestTime;
};

#endif // _KRAZ & _AUTH