
#pragma once

#include "SimpleServerFrame.h"

class CGSMServer : public CSimpleServerFrame
{
	enum eResponseType
	{
		_ARGUMENT_INSUFFICIENCY = 0,
		_UNKNONW_MESSAGE,
		_MESSAGE_OK,
		_MESSGEE_END,
	};
public:
	CGSMServer();
	virtual ~CGSMServer();

	bool Open(USHORT nAcceptPort);

	void Recv(SOCKET * pSocket, const char * pData, int nLen);

	char m_szGsmResponse[_MESSGEE_END][256];
};