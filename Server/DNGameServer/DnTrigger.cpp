#include "StdAfx.h"
#include "DnTrigger.h"
#include "DnTriggerObject.h"
#include "DNGameRoom.h"
#include "DnWorldSector.h"
#include "DnWorldGrid.h"
#include "DnWorld.h"
#include "DNUserSession.h"
#include "DnTriggerElement.h"

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
}

CEtTriggerObject *CDnTrigger::AllocTriggerObject()
{
	return new CDnTriggerObject( this );
}


void CDnTrigger::OnCallTriggerAction( CEtTriggerObject *pObject )
{
	int nObjectIndex = pObject->GetCreateIndex();

	CDnWorld *pWorld = (CDnWorld*)m_pSector->GetParentGrid()->GetWorld();
	CDNGameRoom *pRoom = pWorld->GetCurrentTask();
	SectorIndex Index = m_pSector->GetIndex();
	int nIndex = Index.nY * m_pSector->GetParentGrid()->GetGridX() + Index.nX;
	for( DWORD i=0; i<pRoom->GetUserCount(); i++ ) {
		pRoom->GetUserData(i)->SendTriggerCallAction( nIndex, nObjectIndex, pObject->GetRandomSeed() );
	}
}

int CDnTrigger::Random()
{
	CDnWorld *pWorld = (CDnWorld*)m_pSector->GetParentGrid()->GetWorld();
	CDNGameRoom *pRoom = pWorld->GetCurrentTask();

	return _rand( pRoom );
}

void CDnTrigger::RandomSeed( int nValue )
{
	CDnWorld *pWorld = (CDnWorld*)m_pSector->GetParentGrid()->GetWorld();
	CDNGameRoom *pRoom = pWorld->GetCurrentTask();

	_srand( pRoom, nValue );
}

#if defined( PRE_TRIGGER_TEST )

void CDnTestTrigger::OnEventCallback( const char *szFuncName, LOCAL_TIME LocalTime, float fDelta, bool bProcessQueue )
{
	if( !m_bEnable ) 
		return;
	
	for( DWORD i=0; i<m_VecEventTrigger.size(); i++ ) 
	{
		EventCallbackStruct *pStruct = &m_VecEventTrigger[i];
		if( strcmp( szFuncName, pStruct->szFuncName.c_str() ) == NULL )  
		{
			for( DWORD j=0; j<pStruct->pVecObject.size(); j++  ) 
			{
				CEtTriggerObject *pObject = pStruct->pVecObject[j];

				//if( !pObject->IsCanProcess( LocalTime, fDelta ) ) 
				//	continue;
				bool bProcess = pObject->Process( LocalTime, fDelta );
				if( bProcess ) 
				{
					if( pObject->ProcessQueue( LocalTime, fDelta ) == true ) 
					{
						std::vector<CEtTriggerObject *>::iterator it = std::find( m_pVecActionTriggerQueue.begin(), m_pVecActionTriggerQueue.end(), pObject );
						if( it != m_pVecActionTriggerQueue.end() ) m_pVecActionTriggerQueue.erase( it );
						if( !pObject->IsRepeat() ) 
						{
							FindAndRemoveEventCallbackObject( pObject );
							j--;
							continue;
						}
					}
				}
			}
		}
	}
}

void CDnTestTrigger::FindAndRemoveEventCallbackObject( CEtTriggerObject *pObject )
{
	CDnTestTriggerGameRoom* pGameRoom = dynamic_cast<CDnTestTriggerGameRoom*>(((CDnWorld*)pObject->GetTrigger()->GetSector()->GetParentGrid()->GetWorld())->GetCurrentTask());
	pGameRoom->FinishTrigger( pObject );

	CEtTrigger::FindAndRemoveEventCallbackObject( pObject );

	if( m_pVecTriggerList.empty() )
	{
		//std::cout << "트리거 테스트 완료" << std::endl;
		pGameRoom->DestroyGameRoom();
	}
}

#endif // #if defined( PRE_TRIGGER_TEST )
