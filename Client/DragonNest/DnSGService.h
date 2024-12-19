#pragma once

#ifdef _SG

#include "DnServiceSetup.h"

class DnSGService : public IServiceSetup
{
public:
	DnSGService();
	virtual ~DnSGService();

	virtual int	PreInitialize( void* pCustomData );
	virtual int Release();
	virtual void OnDispatchMessage( int iMainCmd, int iSubCmd, char * pData, int iLen );
};

#endif // _SG