#include "StdAfx.h"
#include "DnCelberosMonsterActor.h"
#include "DnActorClassDefine.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "MAAiBase.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCelberosMonsterActor::CDnCelberosMonsterActor( int nClassID, bool bProcess )
:  CDnPartsMonsterActor( nClassID, bProcess )
{
	m_HitCheckType = HitCheckTypeEnum::Collision;
	memset( m_bBufDieArray, 0, sizeof( m_bBufDieArray ) );
}

CDnCelberosMonsterActor::~CDnCelberosMonsterActor()
{
}

bool CDnCelberosMonsterActor::Initialize()
{
	CDnPartsMonsterActor::Initialize();

	m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 3 ) );
	m_hObject->SetTargetCollisionGroup( 0 );

	SetUseDamageColor(true);

	return true;
}


void CDnCelberosMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	/*
	if( !IsDie() ) {
		std::vector<DnActorHandle> hVecList;
		EtVector3 vAniDist = *GetPosition();
		vAniDist.x -= GetAniDistance()->x;
		vAniDist.z -= GetAniDistance()->z;

		int nCount =  ScanActor( vAniDist, (float)GetUnitSize(), hVecList );
		if( nCount > 0 ) {
			EtVector2 vDir;
			int nSumSize;
			for( int i=0; i<nCount; i++ ) {
				if( hVecList[i] == GetMySmartPtr() ) continue;
				if( hVecList[i]->IsDie() ) continue;
				vDir = EtVec3toVec2( *hVecList[i]->GetPosition() - *GetPosition() );
				nSumSize = GetUnitSize() + hVecList[i]->GetUnitSize();
				float fLength = D3DXVec2Length( &vDir );
				if( fLength > (float)nSumSize ) continue;

				D3DXVec2Normalize( &vDir, &vDir );
				MAMovementBase *pMovement = dynamic_cast<MAMovementBase *>(hVecList[i].GetPointer());
				if( pMovement ) pMovement->MoveToWorld( vDir * (float)( nSumSize - fLength ) );
			}
		}
	}
	*/

	CDnPartsMonsterActor::Process( LocalTime, fDelta );
}

void CDnCelberosMonsterActor::OnDrop( float fCurVelocity )
{
	
}

void CDnCelberosMonsterActor::OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time)
{
	if( strstr( szPrevAction, "Dash_End" ) )
	{		
		EtVector2 vView = EtVec3toVec2( *GetLookDir() );
		vView *= -1.f;
		Look( vView, true );
	}

	CDnMonsterActor::OnFinishAction( szPrevAction, szNextAction, time );
}