#include "Packet.h"
#include <string.h>

CPacket::CPacket(): m_Length(0), m_pBuf(NULL)
{
}

CPacket::~CPacket()
{
	if (m_pBuf)
	{
		delete m_pBuf;
		m_pBuf = NULL;
	}
}

int	CPacket::GetLength()
{
	return m_Length;
}

unsigned char *	CPacket::GetBuf(long size)
{
	if ( size <= 0 && m_pBuf == NULL)
		return NULL;

	if (size > 0 && size != m_Length)
	{
		unsigned char * p;
		p = new unsigned char[size + sizeof(long)];
		if (m_pBuf)
		{
			memcpy(p, m_pBuf, (size > m_Length ? m_Length : size) + sizeof(long));
			delete[] m_pBuf;
		}

		m_pBuf = p;
		m_Length = size;
		*(long*)m_pBuf = ntohl(m_Length);
	}

	return m_pBuf + sizeof(long);
}
	
int CPacket::GetWholeLength()
{
	return m_Length + sizeof(long);
}

unsigned char * CPacket::GetWholeBuf()
{
	return m_pBuf;
}

void CPacket::CopyBuf(const char * buf)
{
	if (buf==NULL)
		return;

	long Length = ntohl( *(long*)buf);
	if (Length <=0)
		return ;

	memcpy( GetBuf(Length), buf + sizeof(long), Length);
}

bool CPacket::CheckEnough(const char * buf, int length)
{
	if (length<4)
		return false;

	return length >= (int)ntohl( *(long*)buf) + 4;
}

long CPacket::ntohl(long a)
{
	long r = 0;
	for (int i=0; i<sizeof(a);++i)
	{
		r<<=8;
		r|= (a>>(i<<3))&0x0ff;
	}
	return r;
}

short CPacket::ntohs (short a)
{
	short r = 0;
	for (int i=0; i<sizeof(a);++i)
	{
		r<<=8;
		r|= (a>>(i<<3))&0x0ff;
	}
	return r;
}