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

		// ������ ��쿡 CanTalk �� �������Ѵ�. EnterPropNpc ��Ŷ�� �ޱ� ��������.. 
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

	// Npc �����س����� �׳��� Process ���鼭 MAMovementbase Process ���� ������ ��ġ�� �����ع����ϴ�.
	// ������ PropNpc �� �����ؼ� StaticMovement �� �����ؾ������� NpcActor �� ���̽��� ��ƹ����¹ٶ���
	// �׷��� �ϱⰡ �ָ��ؼ� �ϴ� �̷��� ���ʿ��� ����� �������ֵ��� �ϰ� ���߿� NPCActor �� ���̽���
	// ������ �۾� �� �����Դϴ�.
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