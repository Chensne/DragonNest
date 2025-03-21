#include "StdAfx.h"
#include "DnBeholderMonsterActor.h"
#include "DnActorClassDefine.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "MAAiBase.h"


CDnBeholderMonsterActor::CDnBeholderMonsterActor( CMultiRoom *pRoom, int nClassID )
:  CDnPartsMonsterActor( pRoom, nClassID )
{
	m_fShakeDelta = 0.f;
	m_HitCheckType = HitCheckTypeEnum::Collision;
/*
	m_DummyBeholderEye = CreateActor(GetRoom(),99);
	if( m_DummyBeholderEye ) {
		m_DummyBeholderEye->SetProcess( false );
		m_DummyBeholderEye->Show(false);
		m_DummyBeholderEye->Initialize();
	}
*/
}

CDnBeholderMonsterActor::~CDnBeholderMonsterActor()
{
}


bool CDnBeholderMonsterActor::Initialize()
{
	CDnPartsMonsterActor::Initialize();
	
	
	return true;
}


void CDnBeholderMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	float fValue = 1.f;
	if( m_fShakeDelta > 0.f ) {
		m_fShakeDelta -= fDelta;
		if( m_fShakeDelta <= 0.f ) {
			m_fShakeDelta = 0.f;
		}
		fValue = 1.f - ( ( _rand(m_hActor->GetRoom())%100 ) * 0.0005f );
	}
	MAActorRenderBase::SetScale( EtVector3( GetScale() * fValue, GetScale(), GetScale() * fValue ) );

	if( !IsDie() ) {
		DNVector(DnActorHandle) hVecList;
		EtVector3 vAniDist = *GetPosition();
		vAniDist.x -= GetAniDistance()->x;
		vAniDist.z -= GetAniDistance()->z;

		int nCount =  ScanActor( GetRoom(), vAniDist, (float)GetUnitSize(), hVecList );
		if( nCount > 0 ) {
			EtVector2 vDir;
			int nSumSize;
			for( int i=0; i<nCount; i++ ) {
				if( !hVecList[i] ) continue;
				if( hVecList[i] == GetMySmartPtr() ) continue;
				if( hVecList[i]->IsDie() ) continue;
				vDir = EtVec3toVec2( *hVecList[i]->GetPosition() - *GetPosition() );
				nSumSize = GetUnitSize() + hVecList[i]->GetUnitSize();
				float fLength = D3DXVec2Length( &vDir );
				if( fLength > (float)nSumSize ) continue;

				D3DXVec2Normalize( &vDir, &vDir );
				MAMovementBase *pMovement = hVecList[i]->GetMovement();
				if( pMovement ) pMovement->MoveToWorld( vDir * (float)( nSumSize - fLength ) );
			}
		}
	}

	CDnPartsMonsterActor::Process( LocalTime, fDelta );
	
}


void CDnBeholderMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct )
{
	std::string szActionName = HitParam.szActionName;
	HitParam.szActionName.clear();
	m_fShakeDelta = 0.3f;
	CDnPartsMonsterActor::OnDamage( pHitter, HitParam, pHitStruct );

#if 0
	OutputDebug("OnDamage: %d\n", HitParam.nBoneIndex);

	if ( HitParam.nBoneIndex > 0 )
	{
		
		CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance(GetRoom()).GetTask( "GameTask" );

		EtVector3 vVel;
		EtVector3 vResistance;

		vVel.x =0.0f;
		vVel.y = 2.0f;
		vVel.z = 2.0f;
		vResistance.x = 0.0f;
		vResistance.z = -20.0f;
		vResistance.y = -20.0f;


		DnActorHandle hMonster = pTask->RequestGenerationMonsterFromMonsterID(94, HitParam.vPosition, vVel, vResistance, NULL);
		OutputDebug( "UniqueID : %d \n", hMonster->GetUniqueID());

		//hMonster->CmdLook(GetMySmartPtr());
		hMonster->SetVelocityZ( vVel.z );
		hMonster->SetVelocityY( vVel.y );
		hMonster->SetResistanceZ( vResistance.z );
		hMonster->SetResistanceY( vResistance.y );
		
		hMonster->CmdAction("Hit_AirBounce");
		//hMonster->SetActionQueue("Hit_AirBounce");

		SCollisionPrimitive *pCollPrimitive = m_hObject->FindCollisionPrimitive( HitParam.nBoneIndex );
		if ( pCollPrimitive )
		{
			m_hObject->EnableCollision( pCollPrimitive, false );
		}
	}
#endif

	if ( IsDie() )
	{
		m_pAi->OnDie(CDnActionBase::m_LocalTime);
		SetDieDelta(10.0f);
	}

	HitParam.szActionName = szActionName;
}