#include "StdAfx.h"

#if 0

#include "DnMessageManager.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnMessageManager::CDnMessageManager(void)
{
}

CDnMessageManager::~CDnMessageManager(void)
{
}

CDnMessageManager& CDnMessageManager::GetInstance()
{
	static CDnMessageManager Instance;
	return Instance;
}

bool CDnMessageManager::RemoveMessage( int nMainCmd, int nSubCmd )
{
	if( m_MessageData.EnableCommand( CDnMessageData::typeLocal, CDnMessageData::SCommandSet( nMainCmd, nSubCmd ) ) )
	{
		std::vector<int> vecAddCommand;
		m_MessageData.GetRemoveCommand( CDnMessageData::typeLocal, CDnMessageData::SCommandSet( nMainCmd, nSubCmd ), vecAddCommand );

		for( int i=0; i<(int)vecAddCommand.size(); i++ )
		{
			m_TaskMessage.RemoveMessage( nMainCmd, vecAddCommand[i] );
		}

		return true;
	}

	//CDebugSet::ToLogFile( "CDnMessageManager::RemoveMessage, 존재하지 않는 커맨드(Main:%d, Sub:%d)", nMainCmd, nSubCmd );
	return false;
}

bool CDnMessageManager::AddMessage( int nMainCmd, int nSubCmd, int nStringIndex )
{
	if( m_MessageData.EnableCommand( CDnMessageData::typeLocal, CDnMessageData::SCommandSet( nMainCmd, nSubCmd ) ) )
	{
		std::vector<int> vecAddCommand;
		m_MessageData.GetAddCommand( CDnMessageData::typeLocal, CDnMessageData::SCommandSet( nMainCmd, nSubCmd ), vecAddCommand );

		for( int i=0; i<(int)vecAddCommand.size(); i++ )
		{
			if( m_TaskMessage.FindMessage( nMainCmd, vecAddCommand[i] ) )
			{
				GetInterface().MessageBox( nStringIndex );
				return false;
			}
		}

		for( int i=0; i<(int)vecAddCommand.size(); i++ )
		{
			m_TaskMessage.AddMessage( nMainCmd, vecAddCommand[i] );
		}

		return true;
	}

	//CDebugSet::ToLogFile( "CDnMessageManager::AddMessage, 존재하지 않는 커맨드(Main:%d, Sub:%d)", nMainCmd, nSubCmd );
	return false;
}

#endif