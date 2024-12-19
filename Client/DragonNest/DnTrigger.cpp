#include "StdAfx.h"
#include "DnTrigger.h"
#include "DnTriggerObject.h"
#include "DnTriggerElement.h"
#include "DnClientScriptAPI.h"
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTrigger::CDnTrigger( CEtWorldSector *pSector )
: CEtTrigger( pSector )
{
	m_bRegisterLuaAPI = false;
	m_pLuaState = lua_open();
	luaL_openlibs( m_pLuaState );
}

CDnTrigger::~CDnTrigger()
{
	if( m_pLuaState ) lua_close( m_pLuaState );
	ClearScriptLoadResource();
}

CEtTriggerObject *CDnTrigger::AllocTriggerObject()
{
	return new CDnTriggerObject( this );
}


void CDnTrigger::CallActionTrigger( int nObjectIndex, int nRandomSeed )
{
//	if( nObjectIndex < 0 || nObjectIndex >= (int)m_pVecTriggerList.size() ) return;

	CEtTriggerObject *pObject = NULL;
	for( DWORD i=0; i<m_pVecTriggerList.size(); i++ ) {
		if( m_pVecTriggerList[i]->GetCreateIndex() == nObjectIndex ) {
			pObject = m_pVecTriggerList[i];
			break;
		}
	}
	if( pObject == NULL ) return;

	if( pObject->GetActionCount() ) pObject->GetActionFromIndex(0)->SetLastExcuteTime( m_LocalTime );

	pObject->SetRandomSeed( nRandomSeed );
	InsertActionQueue( pObject );
	/*
	for( DWORD i=0; i<pObject->GetActionCount(); i++ ) {
		CDnTriggerElement *pElement = (CDnTriggerElement *)pObject->GetActionFromIndex(i);
		pElement->CallLuaFunction();
	}
	if( !pObject->IsRepeat() ) {
		FindAndRemoveEventCallbackObject( pObject );
	}
	*/
}

void CDnTrigger::OnEventCallback( const char *szFuncName, LOCAL_TIME LocalTime, float fDelta, bool bProcessQueue )
{
}

void CDnTrigger::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( CTaskManager::GetInstance().GetTask( "CutSceneTask" ) ) return;

	CEtTrigger::Process( LocalTime, fDelta );
}

int CDnTrigger::Random()
{
	return _rand();
}

void CDnTrigger::RandomSeed( int nValue )
{
	_srand( nValue );
}
