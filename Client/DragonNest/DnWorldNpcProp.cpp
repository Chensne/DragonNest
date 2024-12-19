#include "StdAfx.h"
#include "DnWorldNpcProp.h"
#include "DNProtocol.h"
#include "DnWorld.h"
#include "DnWorldSector.h"
#include "DnWorldGrid.h"
#include "DnNpcActor.h"
#include "DnMinimap.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldNpcProp::CDnWorldNpcProp()
{
}

CDnWorldNpcProp::~CDnWorldNpcProp()
{
}

bool CDnWorldNpcProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	bool bResult = false;

	if( GetData() ) {
		NpcStruct *pStruct = (NpcStruct *)GetData();
		int nNpcTableID = pStruct->nNpcID;
		bResult = InitializeNpcActorProp( nNpcTableID );
	}

	return bResult;
}

bool CDnWorldNpcProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( !bResult ) return false;

	if( m_hMonster ) {
		*m_hMonster->GetMatEx() = *GetMatEx();
		if( m_hMonster->GetObjectHandle() ) m_hMonster->GetObjectHandle()->Update( *m_hMonster->GetMatEx() );
		m_hMonster->SetActionSignalScale( *GetActionSignalScale() );

		// 마을일 경우에 CanTalk 를 꺼놔야한다. EnterPropNpc 패킷을 받기 전까지는.. 
		if( CDnWorld::IsActive() && ((CDnWorld*)m_pParentSector->GetParentGrid()->GetWorld())->GetMapType() == CDnWorld::MapTypeVillage ) {
			CDnNPCActor *pNpc = (CDnNPCActor*)m_hMonster.GetPointer();
			pNpc->EnableOperator( false );
		}

		SAABox Box;
		GetBoundingBox( Box );
		float fHeight = Box.Max.y - Box.Min.y;
		m_hMonster->GetBoundingBox( Box );
		fHeight -= Box.Max.y - Box.Min.y;
		m_hMonster->SetIncreaseHeight( fHeight );
	}
	return true;
}

void CDnWorldNpcProp::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnWorldActProp::Process( LocalTime, fDelta );

	// Npc 생성해놓으면 그놈은 Process 돌면서 MAMovementbase Process 에서 강제로 위치를 셋팅해버립니다.
	// 원래는 PropNpc 로 생성해서 StaticMovement 로 생성해야하지만 NpcActor 를 베이스로 잡아버리는바람에
	// 그렇게 하기가 애매해서 일단 이렇게 이쪽에서 강재로 셋팅해주도록 하고 나중에 NPCActor 쪽 베이스로
	// 내리는 작업 할 예정입니다.
	if( m_hMonster ) {
		*m_hMonster->GetMatEx() = *GetMatEx();
	}
}

void CDnWorldNpcProp::Show( bool bShow )
{
	CDnWorldActProp::Show( bShow );
	if( m_hMonster ) {
		m_hMonster->Show( bShow );
		CDnNPCActor *pNpc = (CDnNPCActor*)m_hMonster.GetPointer();
		GetMiniMap().ShowNpcQuestInfo( pNpc->GetNpcData().nNpcID, bShow );
	}
}