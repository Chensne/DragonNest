#include "StdAfx.h"
#include "DnChangeActionStrProcessor.h"
#include "DnActor.h"
#include "DnProjectile.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif




CDnChangeActionStrProcessor::CDnChangeActionStrProcessor( DnActorHandle hActor, 
														 const char *szAdditionalStr, DNVector(std::string) &szActionStr) : IDnSkillProcessor( hActor ) 
{
	m_iType = CHANGE_ACTIONSTR;

//	m_szAdditionalStr = szAdditionalStr;
	char szStr[64];
	for( DWORD i=0; i<szActionStr.size(); i++ ) {
		sprintf_s( szStr, "%s%s", szActionStr[i].c_str(), szAdditionalStr );
		m_szMapMatchStr.insert( make_pair( szActionStr[i], szStr ) );
		m_setChangeActionNames.insert( szStr );
	}
}


CDnChangeActionStrProcessor::~CDnChangeActionStrProcessor(void)
{
}


void CDnChangeActionStrProcessor::SetHasActor( DnActorHandle hActor )
{
	_ASSERT( hActor && "void CDnChangeActionStrProcessor::SetHasActor( DnActorHandle hActor ), Actor is NULL!!" );
	IDnSkillProcessor::SetHasActor( hActor );
}



void CDnChangeActionStrProcessor::OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill )
{

}


void CDnChangeActionStrProcessor::Process( LOCAL_TIME LocalTime, float fDelta )
{

}


void CDnChangeActionStrProcessor::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

}


bool CDnChangeActionStrProcessor::IsFinished( void )
{
	return true;
}

const char *CDnChangeActionStrProcessor::GetChangeActionName( const string& strNowActionName, bool* pBChanged/* = NULL*/ )
{
	// 게임서버 덤프 확인용.
	if( strNowActionName.empty() )
		return NULL;

	std::map<std::string, std::string>::iterator it = m_szMapMatchStr.find( strNowActionName );
	if( it == m_szMapMatchStr.end() )
	{
		if( pBChanged )
			*pBChanged = false;

		//return strNowActionName.c_str();
		return NULL;
	}

	if( pBChanged )
		*pBChanged = true;

	return it->second.c_str();
}

bool CDnChangeActionStrProcessor::IsChangedActionName( const char* pActionName )
{
	if( m_setChangeActionNames.end() != m_setChangeActionNames.find( pActionName ) )
		return true;
	else
		return false;
}

void CDnChangeActionStrProcessor::CopyFrom( IDnSkillProcessor* pProcessor )
{
	if( NULL == pProcessor )
		return;

	if( GetType() != pProcessor->GetType() )
		return;

	CDnChangeActionStrProcessor* pSource = static_cast<CDnChangeActionStrProcessor*>( pProcessor );
	map<string, string>& matchList = pSource->GetMachStringList();
	set<string>& chageActionList = pSource->GetChangeActionNameList();

	m_szMapMatchStr.clear();
	m_setChangeActionNames.clear();

	{
		map<string, string>::iterator iter = matchList.begin();
		map<string, string>::iterator endIter = matchList.end();
		for (; iter != endIter; ++iter)
		{
			m_szMapMatchStr.insert(std::make_pair(iter->first, iter->second));
		}
	}

	{
		set<string>::iterator iter = chageActionList.begin();
		set<string>::iterator endIter = chageActionList.end();
		for (; iter != endIter; ++iter)
		{
			m_setChangeActionNames.insert((*iter));
		}
	}
}