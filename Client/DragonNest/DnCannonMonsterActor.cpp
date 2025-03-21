#include "StdAfx.h"
#include "DnCannonMonsterActor.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DnGravityOrbit.h"
#include "EtActionSignal.h"
#include "DnPlayAniProcess.h"
#include "DnCannonCamera.h"
#include "DnPlayerActor.h"
#include "DnLocalPlayerActor.h"
#include "DnInterface.h"
#include "DnMainDlg.h"


const float CANNON_CROSSHAIR_TARGET_DIST = 300.0f;

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCannonMonsterActor::CDnCannonMonsterActor( int nClassID, bool bProcess ) : TDnMonsterActor( nClassID, bProcess ),
																			  m_vNowShootDir( 0.0f, 0.0f, 0.0f )
{
	m_bHaveMaster = false;
}


CDnCannonMonsterActor::~CDnCannonMonsterActor()
{
}

void CDnCannonMonsterActor::SetDestroy()
{
	// 플레이어 액터에게 대포 몹이 죽었음을 알린다.
	if( m_hMasterPlayerActor )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hMasterPlayerActor.GetPointer());
		pPlayerActor->OnCannonMonsterDie();
	}

	CDnMonsterActor::SetDestroy();
}

bool CDnCannonMonsterActor::Initialize( void )
{
	bool bResult = CDnMonsterActor::Initialize();
	if( false == bResult )
		return false;

	DNTableFileFormat* pMonsterTable = GetDNTable( CDnTableDB::TMONSTER );
	int nSkillTableId = pMonsterTable->GetFieldFromLablePtr( GetMonsterClassID(), "_SkillTable")->GetInteger();

	DNTableFileFormat*  pMonsterSkillTable = GetDNTable( CDnTableDB::TMONSTERSKILL );
	for( int i = 0; i < 2; ++i )
	{
		char szLabel1[128]={0,};
		char szLabel2[128]={0,};
		sprintf_s( szLabel1, "_SkillIndex%d", i + 1);
		sprintf_s( szLabel2, "_SkillLevel%d", i + 1);
		DNTableCell* pField = pMonsterSkillTable->GetFieldFromLablePtr( nSkillTableId, szLabel1 );
		if ( !pField )
			continue;

		int nSkillId = pField->GetInteger();

		pField = pMonsterSkillTable->GetFieldFromLablePtr( nSkillTableId, szLabel2 );

		int nSkillLevel = pField->GetInteger();

		if ( nSkillId < 0 || nSkillLevel < 0 )
			continue;
		
		bool bResult = AddSkill( nSkillId, nSkillLevel );
		_ASSERT( bResult );
		DnSkillHandle hSkill = FindSkill( nSkillId );
		if( hSkill )
		{
			// 스킬에서 사용되는 액션에서 발사체 시그널을 뽑아내서 추후에 타겟팅 계산에서 쓰일 수 있도록 한다.
			//const set<string>& setUseActionSet = hSkill->GetUseActionSet();
			//// 사실 액션은 대포쏘는 액션이기 때문에 하나만 있어야 한다.
			//_ASSERT( 1 == setUseActionSet.size() );
			//set<string>::const_iterator iter = setUseActionSet.begin();
			//for( iter; iter != setUseActionSet.end(); ++iter )
			//{
			CDnPlayAniProcess* pProcessor = static_cast<CDnPlayAniProcess*>(hSkill->GetProcessor( IDnSkillProcessor::PLAY_ANI ));
			string strActionName = pProcessor->GetActionName();
			ActionElementStruct* pActionElement = GetElement( strActionName.c_str() );
			int iNumSignals = (int)pActionElement->pVecSignalList.size();
			for( int iSignal = 0; iSignal < iNumSignals; ++iSignal )
			{
				CEtActionSignal* pSignal = pActionElement->pVecSignalList.at( iSignal );
				int iSignalIndex = pSignal->GetSignalIndex();
				if( STE_Projectile == iSignalIndex )
				{
					// 현재는 대포의 각 액션별로 발사체 시그널이 하나씩만 있는 것으로 가정한다.
					_ASSERT( m_mapCannonProjectileSignalBySkillID.end() != m_mapCannonProjectileSignalBySkillID.find(nSkillId) );
					m_mapCannonProjectileSignalBySkillID[ nSkillId ] = (ProjectileStruct*)pSignal->GetData();
					break;
				}
			}
		}
	}

	m_vCannonLookDirection = GetMatEx()->m_vZAxis;
	return true;
}

void CDnCannonMonsterActor::Look( EtVector2& vVec, bool bForceRotate)
{
	if( bForceRotate ) 
	{
		if( EtVec2LengthSq( &vVec ) > 0.f ) {
			GetMatEx()->m_vZAxis = EtVector3( vVec.x, 0.f, vVec.y );
			EtVec3Normalize( &GetMatEx()->m_vZAxis, &GetMatEx()->m_vZAxis );
			GetMatEx()->MakeUpCartesianByZAxis();
		}
	}
}

void CDnCannonMonsterActor::SetMasterPlayerActor( DnActorHandle hMasterPlayerActor )
{
	m_hMasterPlayerActor = hMasterPlayerActor;
	m_vCannonLookDirection = GetMatEx()->m_vZAxis;
	// TODO: 대포 테스트 코드
	SetTeam( hMasterPlayerActor->GetTeam() );
	m_bHaveMaster = true;
}

void CDnCannonMonsterActor::ClearMasterPlayerActor( void )
{
	m_hMasterPlayerActor.Identity();
	m_vCannonLookDirection = GetMatEx()->m_vZAxis;
	m_bHaveMaster = false;
}

//float fCrosshairTargetDist = 300.0f;
bool CDnCannonMonsterActor::GetCannonGroundHitPosition( /*IN*/ DnCameraHandle hCamera,  /*OUT*/ EtVector3& vCannonGroundHitPos )
{
	bool bSuccess = false;

	// 대포 약공격이던 강공격이던 힘(속도) 수치가 같아야 미리 데칼 찍어준다는 것이 성립된다.
	// 만약 약공격과 강공격의 발사체 속도가 다르면 낙하지점도 달라지고 플레이어가 무엇을 쏠지 모르기 때문
	//// 결과적으로 데칼을 찍어줄 수 없다.
	//map<int, const ProjectileStruct*>::iterator iter = m_mapCannonProjectileSignalBySkillID.find( iSkillID );
	//if( iter != m_mapCannonProjectileSignalBySkillID.end() )
	if( false == m_mapCannonProjectileSignalBySkillID.empty() )
	{
		map<int, const ProjectileStruct*>::iterator iter = m_mapCannonProjectileSignalBySkillID.begin();
		{
			const ProjectileStruct* pProjectileInfo = iter->second;
			EtVector3 vShootPos = m_matExCannonRotated.m_vPosition;
			vShootPos += m_matExCannonRotated.m_vXAxis * (pProjectileInfo->vOffset->x * GetScale());
			vShootPos += m_matExCannonRotated.m_vYAxis * (pProjectileInfo->vOffset->y * GetScale());
			vShootPos += m_matExCannonRotated.m_vZAxis * (pProjectileInfo->vOffset->z * GetScale());
			
			// 테스트용. 발사체 구조체에 있는 로컬좌표 방향벡터 그대로 사용.
			//EtVector3 vShootDir = *pProjectileInfo->vDirection;

			int nWidth = CEtDevice::GetInstance().Width() / 2;
			int nHeight = (int)( CEtDevice::GetInstance().Height() * 0.46f/* * s_fCrossHairHeightRatio*/ );
			EtVector3 vOrig, vCrosshairDir;
			hCamera->GetCameraHandle()->CalcPositionAndDir( nWidth, nHeight, vOrig, vCrosshairDir );
			
			// 임의로 먼 위치로 셋팅후 방향만 얻어옴.
			vOrig += vCrosshairDir * static_cast<CDnCannonCamera*>(hCamera.GetPointer())->GetDistance();
			EtVector3 vDestination = vOrig + (vCrosshairDir*CANNON_CROSSHAIR_TARGET_DIST);
			EtVector3 vDirToCrosshairDest = vDestination - vShootPos;
			EtVec3Normalize( &vDirToCrosshairDest, &vDirToCrosshairDest );

			// 현재 대포 카메라의 Z 축 방향은 월드좌표 기준이므로 x 축 회전값만 뽑아서 사용한다.
			m_vNowShootDir = EtVector3( 0.0f, 0.0f, 1.0f );

			EtVector3 vDirToCrosshairDestZBasis( vDirToCrosshairDest.x, 0.0f, vDirToCrosshairDest.z );
			EtVec3Normalize( &vDirToCrosshairDestZBasis, &vDirToCrosshairDestZBasis );
			float fDot = EtVec3Dot( &vDirToCrosshairDest, &vDirToCrosshairDestZBasis );
			float fRotX = EtAcos( fDot );
			if( vDirToCrosshairDest.y < 0.0f )
				fRotX *= -1.0f;

			EtMatrix matRot;
			EtMatrixRotationX( &matRot, -fRotX );
			EtVec3TransformNormal( &m_vNowShootDir, &m_vNowShootDir, &matRot );

			CDnGravityOrbitCalculator::CalcHitGroundPos( m_matExCannonRotated, vShootPos, pProjectileInfo->fSpeed, m_vNowShootDir, vCannonGroundHitPos );

			bSuccess = true;
		}
	}

	return bSuccess;
}


void CDnCannonMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnMonsterActor::Process(LocalTime, fDelta);

	if(m_hMasterPlayerActor)
	{
		bool bIsLocalMaster = false;

		if(CDnActor::s_hLocalActor && m_hMasterPlayerActor == CDnActor::s_hLocalActor)
			bIsLocalMaster = true;

		ProcessCannonLook(bIsLocalMaster);
	}
}

void CDnCannonMonsterActor::ProcessCannonLook(bool bLocal)
{

	if(bLocal)
	{
		int BoneIndex = GetBoneIndex("~Dummy11");

#ifdef PRE_ADD_MODIFY_PLAYER_CANNON
		DNTableFileFormat* pTableCannon = GetDNTable( CDnTableDB::TCANNON );
		if( pTableCannon->IsExistItem(GetClassID()) )
		{
			const char* pCannonActionName = pTableCannon->GetFieldFromLablePtr( GetClassID(), "_Axis" )->GetString();
			BoneIndex = GetBoneIndex(pCannonActionName);
		}
#endif
		
		EtVector3 vec_Rotation = EtVector3(0.f,0.f,0.f);

		CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
		if( !hCamera || !pPlayer->IsCannonMode() ) return;
		if( hCamera->GetCameraType() != CDnCamera::CannonCamera ) return;

		CDnCannonCamera* pCamera = ((CDnCannonCamera*)hCamera.GetPointer());
		if(!pCamera) return;

		float fDot = EtVec3Dot( &EtVector3(0.f,1.f,0.f), &pCamera->GetMatEx()->m_vZAxis );
		vec_Rotation.x = (EtToDegree( EtAcos(fDot) ) - 90.f +10.f); 
		// 원래 -90도인데 크로스헤어가 약간 위에 표현되게 하기위해서 약간더 내립니다. 정확한 지점을 가르키는 것은 아니지만 퍼포먼스용

		vec_Rotation.y = pCamera->GetYawValue();

		m_hObject->SetBoneRotation( BoneIndex, vec_Rotation);
		m_vCannonLookDirection = pCamera->GetMatEx()->m_vZAxis; //  일단 정보는 가지고있자.
	}
	else
	{
		int BoneIndex = GetBoneIndex("~Dummy11");

#ifdef PRE_ADD_MODIFY_PLAYER_CANNON
		DNTableFileFormat* pTableCannon = GetDNTable( CDnTableDB::TCANNON );
		if( pTableCannon->IsExistItem(GetClassID()) )
		{
			const char* pCannonActionName = pTableCannon->GetFieldFromLablePtr( GetClassID(), "_Axis" )->GetString();
			BoneIndex = GetBoneIndex(pCannonActionName);
		}
#endif

		EtVector3 vec_Rotation = EtVector3(0.f,0.f,0.f);
		float fDot = EtVec3Dot( &EtVector3(0.f,1.f,0.f), &m_vCannonLookDirection );
		vec_Rotation.x = (EtToDegree( EtAcos(fDot) ) - 90.f +10.f); 

		float fCannonAngle = atan2f( GetMatEx()->m_vZAxis.x,GetMatEx()->m_vZAxis.z );
		float fCameraAngle = atan2f( m_vCannonLookDirection.x,m_vCannonLookDirection.z );

		vec_Rotation.y = EtToDegree(fCameraAngle) - EtToDegree(fCannonAngle);
		m_hObject->SetBoneRotation( BoneIndex, vec_Rotation);
	}

	// 대포 발사를 위한 기반이 될 변환 행렬
	// 발사체 발사시에 쏘는 액터의 Cross 를 기반으로 참조하므로 발사체에서 사용할 EtMatrixEx 를 따로 가지고 있도록 한다.
	m_matExCannonRotated = m_matexWorld;
	m_matExCannonRotated.m_vZAxis = m_vCannonLookDirection;		// m_vCannonLookDirection 은 카메라의 ZVector 와 동일.
	m_matExCannonRotated.m_vZAxis.y = 0.0f;
	EtVec3Normalize( &m_matExCannonRotated.m_vZAxis, &m_matExCannonRotated.m_vZAxis );
	m_matExCannonRotated.MakeUpCartesianByZAxis();

}
void CDnCannonMonsterActor::OnMasterPlayerActorDie( void )
{
	ClearMasterPlayerActor();
}


void CDnCannonMonsterActor::OnDie( DnActorHandle hHitter )
{
	// 플레이어 액터에게 대포 몹이 죽었음을 알린다.
	if( m_hMasterPlayerActor )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hMasterPlayerActor.GetPointer());
		pPlayerActor->OnCannonMonsterDie();
	}
}

void CDnCannonMonsterActor::OnSkillUse( int iSkillID, int iLevel )
{
	// 쿨타임 시작. 대포를 쏜 클라이언트에서만 표시되도록 처리.
	if( CDnActor::s_hLocalActor == m_hMasterPlayerActor )
	{
		DnSkillHandle hSkill = FindSkill( iSkillID );
		CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
		if( pMainDlg && hSkill && hSkill->HasCoolTime() ) 
		{
			pMainDlg->AddPassiveSkill( hSkill );
		}
	}
}

void CDnCannonMonsterActor::OnCreateProjectileBegin( MatrixEx& matExLocal )
{
	// 몬스터가 발사체 쏘는 가운데 CreateProjectile 내부에서 몬스터의 EtMatrixEx 를 참조하여 Target 을 정하는 
	// 루틴이 있어서 잠시 값을 여기서 바꿔줬다 복구 시켜줌.
	m_matExOriginal = m_matexWorld;
	m_matexWorld = matExLocal;
}

void CDnCannonMonsterActor::OnCreateProjectileEnd( void )
{
	m_matexWorld = m_matExOriginal;
}

void CDnCannonMonsterActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type )
	{
		case STE_Projectile:
			{
				// 서버에서 쏴주는 발사체로 패킷받아서 보여줄 것이다. 클라에서 따로 생성하지 않는다.
				return;
			}
			break;
	}

	CDnMonsterActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

void CDnCannonMonsterActor::OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor )
{
	if( m_hMasterPlayerActor && m_hMasterPlayerActor->IsPlayerActor() && CDnActor::s_hLocalActor == m_hMasterPlayerActor ) {
		m_hMasterPlayerActor->OnHitSuccess( LocalTime, hActor );
	}
	CDnMonsterActor::OnHitSuccess( LocalTime, hActor );
}

void CDnCannonMonsterActor::OnComboCount( int nValue, int nDelay )
{
	if( m_hMasterPlayerActor && m_hMasterPlayerActor->IsPlayerActor() && CDnActor::s_hLocalActor == m_hMasterPlayerActor ) 
	{
		m_hMasterPlayerActor->OnComboCount( nValue, nDelay );
	}
}
