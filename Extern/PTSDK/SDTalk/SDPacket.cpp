#include ".\sdpacket.h"

CSDPacket::CSDPacket(void)
{
}

CSDPacket::~CSDPacket(void)
{
}

void CSDPacket::PutSDMessage(CSDMessage & msg )
{
	msg.ToBuf( GetBuf(msg.GetSize()));
}

void CSDPacket::PutSDMessage(CSDMessage & msg, unsigned char * buf)
{	
	long Length = msg.GetSize();
	*(long*)buf = ntohl(Length);
	msg.ToBuf( buf + sizeof(long));
}


CSDMessage * CSDPacket::GetSDMessage()
{
	return GetSDMessage((const char *)GetWholeBuf());
}

bool CSDPacket::GetSDMessage(CSDMessage &msg)
{
	return GetSDMessage((const char *)GetWholeBuf(), msg);
}

CSDMessage * CSDPacket::GetSDMessage( const char * packetAddr )
{
    CSDMessage * pMsg = new CSDMessage;

	if (GetSDMessage(packetAddr, *pMsg))
		return pMsg;

	delete pMsg;
	return NULL;
}

bool CSDPacket::GetSDMessage( const char * packetAddr, CSDMessage &msg )
{
	if (packetAddr==NULL)
		return false;

	long Length = ntohl( *(long*)packetAddr);
	if (Length <=0)
		return false;

	msg.FromBuf(packetAddr+ sizeof(long), Length);

	return true;		
}
