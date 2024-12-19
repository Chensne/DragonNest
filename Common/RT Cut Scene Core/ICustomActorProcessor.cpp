#include "StdAfx.h"
#include "ICustomActorProcessor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


ICustomActorProcessor::ICustomActorProcessor(void) : m_pWorld( NULL )
{
}


ICustomActorProcessor::~ICustomActorProcessor(void)
{
	for_each( m_vlpActors.begin(), m_vlpActors.end(), DeleteData<S_CS_ACTOR_INFO*>() );
	for_each( m_vlpProps.begin(), m_vlpProps.end(), DeleteData<S_CS_PROP_INFO*>() );
}


void ICustomActorProcessor::AddObjectToPlay( S_CS_ACTOR_INFO* pNewObject )
{
	assert( pNewObject );
	if( NULL == pNewObject )
		return;

	map<string, S_CS_ACTOR_INFO*>::iterator iter = m_mapActors.find( pNewObject->strActorName );
	if( m_mapActors.end() == iter )
	{
		S_CS_ACTOR_INFO* pCopyNewObject = new S_CS_ACTOR_INFO;
		*pCopyNewObject = *pNewObject;

		m_mapActors.insert( make_pair(pCopyNewObject->strActorName, pCopyNewObject) );
		m_vlpActors.push_back( pCopyNewObject );

		pCopyNewObject->iActorIndexInProcessorVector = (int)m_vlpActors.size() - 1;
	}
}


void ICustomActorProcessor::AddPropToPlay( S_CS_PROP_INFO* pPropInfo )
{
	if( pPropInfo )
		m_vlpProps.push_back( new S_CS_PROP_INFO(*pPropInfo) );
}



S_CS_ACTOR_INFO* ICustomActorProcessor::GetActorInfoByName( const char* pActorName )
{
	S_CS_ACTOR_INFO* pResult = NULL;

	map<string, S_CS_ACTOR_INFO*>::iterator iter = m_mapActors.find( string(pActorName) );
	if( m_mapActors.end() != iter )
	{
		pResult = iter->second;
	}

	return pResult;
}