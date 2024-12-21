#include "StdAfx.h"

#if 0

#include "DnMessageData.h"
#include "DNProtocol.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMessageData::CDnMessageData(void)
{
	SCommandList sCommandList;
	sCommandList.InsertAddCmd( eParty::SC_CREATEPARTY );
	sCommandList.InsertAddCmd( eParty::SC_JOINPARTY );
	sCommandList.InsertRemoveCmd( eParty::SC_DESTROYPARTY );
	sCommandList.InsertRemoveCmd( eParty::SC_PARTYOUT );
	m_mapLocalCommand[SCommandSet(SC_PARTY,eParty::SC_CREATEPARTY)] = sCommandList;

	sCommandList.Clear();
	sCommandList.InsertAddCmd( eParty::SC_DESTROYPARTY );
	sCommandList.InsertAddCmd( eParty::SC_PARTYOUT );
	sCommandList.InsertRemoveCmd( eParty::SC_CREATEPARTY );
	sCommandList.InsertRemoveCmd( eParty::SC_JOINPARTY );
	m_mapLocalCommand[SCommandSet(SC_PARTY,eParty::SC_DESTROYPARTY)] = sCommandList;
}

CDnMessageData::~CDnMessageData(void)
{
}

bool CDnMessageData::FindCommand( SCommandSet &commandSet, COMMAND_MAP &commandMap )
{
	COMMAND_MAP::iterator iter = commandMap.find( commandSet );
	if( iter != commandMap.end() )
	{
		return true;
	}

	return false;
}

bool CDnMessageData::EnableCommand( emCommandType commandType, SCommandSet &commandSet )
{
	switch( commandType )
	{
	case typeGlobal:	return FindCommand( commandSet, m_mapGlobalCommand );
	case typeLocal:		return FindCommand( commandSet, m_mapLocalCommand );
	default:
		CDebugSet::ToLogFile( "CDnMessageData::EnableCommand, Invalid Command Type!" );
	    break;
	}

	return false;
}

void CDnMessageData::GetAddCommand( SCommandSet &commandSet, COMMAND_MAP &commandMap, std::vector<int> &vecCommand )
{
	COMMAND_MAP::iterator iter = commandMap.find( commandSet );
	if( iter != commandMap.end() )
	{
		for( int i=0; i<(int)iter->second.m_vecAddCommand.size(); i++ )
		{
			vecCommand.push_back( iter->second.m_vecAddCommand[i] );
		}
	}
}

void CDnMessageData::GetRemoveCommand( SCommandSet &commandSet, COMMAND_MAP &commandMap, std::vector<int> &vecCommand )
{
	COMMAND_MAP::iterator iter = commandMap.find( commandSet );
	if( iter != commandMap.end() )
	{
		for( int i=0; i<(int)iter->second.m_vecRemoveCommand.size(); i++ )
		{
			vecCommand.push_back( iter->second.m_vecRemoveCommand[i] );
		}
	}
}

void CDnMessageData::GetAddCommand( emCommandType commandType, SCommandSet &commandSet, std::vector<int> &vecAddCommand )
{
	switch( commandType )
	{
	case typeGlobal:	GetAddCommand( commandSet, m_mapGlobalCommand, vecAddCommand );	break;
	case typeLocal:		GetAddCommand( commandSet, m_mapLocalCommand, vecAddCommand );	break;
	default:
		CDebugSet::ToLogFile( "CDnMessageData::GetAddCommand, Invalid Command Type!" );
		break;
	}
}

void CDnMessageData::GetRemoveCommand( emCommandType commandType, SCommandSet &commandSet, std::vector<int> &vecRemoveCommand )
{
	switch( commandType )
	{
	case typeGlobal:	GetRemoveCommand( commandSet, m_mapGlobalCommand, vecRemoveCommand );	break;
	case typeLocal:		GetRemoveCommand( commandSet, m_mapLocalCommand, vecRemoveCommand );	break;
	default:
		CDebugSet::ToLogFile( "CDnMessageData::GetRemoveCommand, Invalid Command Type!" );
		break;
	}
}

#endif
