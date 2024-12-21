#include "StdAfx.h"

#if 0

#include "DnMessageList.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMessageList::CDnMessageList(void)
{
}

CDnMessageList::~CDnMessageList(void)
{
}

bool CDnMessageList::AddMessage( int nMainCmd, int nSubCmd )
{
	//CDebugSet::ToLogFile( "CDnMessageList::AddMessage, 추가되는 커맨드(Main:%d, Sub:%d)", nMainCmd, nSubCmd );

	SMessageInfo messageInfo( nMainCmd, nSubCmd );
	m_listMessage.push_back( messageInfo );
	return true;
}

bool CDnMessageList::RemoveMessage( int nMainCmd, int nSubCmd )
{
	SMessageInfo messageInfo( nMainCmd, nSubCmd );

	std::list<SMessageInfo>::iterator iter = std::find_if( m_listMessage.begin(), m_listMessage.end(), CCompareMessage( &messageInfo ) );
	if( iter != m_listMessage.end() )
	{
		//CDebugSet::ToLogFile( "CDnMessageList::RemoveMessage, 제거되는 커맨드(Main:%d, Sub:%d)", nMainCmd, nSubCmd );

		m_listMessage.erase( iter );
		return true;
	}

	return false;
}

bool CDnMessageList::FindMessage( int nMainCmd, int nSubCmd )
{
	SMessageInfo messageInfo( nMainCmd, nSubCmd );

	std::list<SMessageInfo>::iterator iter = std::find_if( m_listMessage.begin(), m_listMessage.end(), CCompareMessage( &messageInfo ) );
	if( iter != m_listMessage.end() )
	{
		//CDebugSet::ToLogFile( "CDnMessageList::FindMessage, 발견된 커맨드(Main:%d, Sub:%d)", nMainCmd, nSubCmd );
		return true;
	}

	return false;
}

#endif