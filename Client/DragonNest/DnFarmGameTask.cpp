
#include "StdAfx.h"
#include "DnFarmGameTask.h"
#include "EtWorldSector.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
#include "DnWorld.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "DnLifeChannelDlg.h"

#include "DnLifeSkillPlantTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 


CDnFarmGameTask::CDnFarmGameTask()
: CDnGameTask()
{
	m_GameTaskType = GameTaskType::Farm;
}

CDnFarmGameTask::~CDnFarmGameTask()
{
	CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( pPartyTask )
		pPartyTask->RequestInitParty();

	SAFE_DELETE_PVEC( m_vPreInitProp );
}

bool CDnFarmGameTask::Initialize()
{
	return CDnGameTask::Initialize();
}

bool CDnFarmGameTask::InitializeStage( int nCurrentMapIndex, int nGateIndex, int nStageConstructionLevel, int nRandomSeed, bool bContinueStage, bool bDirectConnect )
{
	bool bResult = CDnGameTask::InitializeStage( nCurrentMapIndex, nGateIndex, nStageConstructionLevel, nRandomSeed, bContinueStage, bDirectConnect );
	if( !bResult ) 
		return false;

	GetLifeSkillPlantTask().InitializeGrowingArea();
	GetLifeSkillPlantTask().InitializeProp();
	return true;
}

bool CDnFarmGameTask::OnPostInitializeStage()
{
	EtVector3 vPos = EtVector3( 0.f, 0.f, 0.f );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TFARMSKIN );
	CEtWorldSector * pSector = CDnWorld::GetInstance().GetSector( 0.f, 0.f );
	if( !pSox || !pSector )	return false;

	char * szPropName;
	for( int itr = 1; itr <= pSox->GetItemCount(); ++itr )
	{
		szPropName = pSox->GetFieldFromLablePtr( itr, "_SkinFileName" )->GetString();;

		CDnWorldOperationProp * pProp = new CDnWorldOperationProp;

		pProp->Initialize( pSector, szPropName, vPos, EtVector3(0.0f, 0.0f, 0.0f), EtVector3( 1.0f, 1.0f, 1.0f ) );
		pProp->Show( false );

		m_vPreInitProp.push_back( pProp );
	}

	return true;
}

void CDnFarmGameTask::OnInitializeStartPosition( CDnPartyTask::PartyStruct *pStruct, const int iPartyIndex, EtVector3& vStartPos, float& fRotate, const UINT uiSessionID )
{
	DnActorHandle hActor = pStruct->hActor;
	if( hActor )
		hActor->SetTeam( pStruct->usTeam );

	CEtWorldEventControl*	pControl	= NULL;
	CEtWorldSector*			pSector		= NULL;
	CEtWorldEventArea*		pArea		= NULL;

	std::vector<CEtWorldEventArea*> vRespawnArea;

	for( DWORD i=0; i<m_pWorld->GetGrid()->GetActiveSectorCount(); i++ ) 
	{
		pSector	 = m_pWorld->GetGrid()->GetActiveSector(i);
		pControl = pSector->GetControlFromUniqueID( ETE_EventArea );
		if( !pControl ) 
			continue;

		for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) 
		{
			pArea = pControl->GetAreaFromIndex(j);
			if( pArea )
			{
				if( strstr( pArea->GetName(), "StartPosition" ) == pArea->GetName() )
				{
					PvPRespawnAreaStruct* pStruct = reinterpret_cast<PvPRespawnAreaStruct*>(pArea->GetData());
					if( pStruct )
						vRespawnArea.push_back( pArea );
				}
			}
		}
	}

	if( !vRespawnArea.empty() )
	{
		_srand( uiSessionID );
		CEtWorldEventArea* pRespawnArea = vRespawnArea[_rand()%vRespawnArea.size()];

		EtVector3 vRandPos = GetGenerationRandomPosition( pRespawnArea->GetOBB() );

		vStartPos = vRandPos;
		vStartPos.y = CDnWorld::GetInstance().GetHeight( vStartPos.x, vStartPos.z ) + pRespawnArea->GetMin()->y;
		fRotate		= pRespawnArea->GetRotate();
	}
}

void CDnFarmGameTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnGameTask::Process( LocalTime, fDelta );
}

int CDnFarmGameTask::GetFarmZoneNumber()
{
	return GetInterface().GetLifeChannelDlg()->GetFarmDBID();
}

void CDnFarmGameTask::RequestMoveVillage()
{

}



