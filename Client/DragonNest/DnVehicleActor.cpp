#include "stdafx.h"
#include <mmsystem.h>
#include "TaskManager.h"
#include "DnTableDB.h"
#include "DnDataManager.h"
#include "DnWorld.h"
#include "navigationmesh.h"
#include "DnVehicleActor.h"
#include "DnLocalPlayerActor.h"
#include "DnActorClassDefine.h"
#include "DnInterface.h"
#include "DnMainDlg.h"

#include "DnPartsVehicleEffect.h"


CDnVehicleActor::CDnVehicleActor(int nClassID, bool bProcess )
: CDnActor( nClassID, bProcess )
{
	CDnActionBase::Initialize( this );
	CDnVehicleState::Initialize(nClassID);

	m_bAttachToPlayer = false; 
	m_bProcessTruning = false;
	m_bIsLocalVehicle = false;
	
	m_nItemID = 0;
	m_dwHairColor = 0;
	m_cMovePushKeyFlag = 0;

	for(int i=0;i<2;i++)
		m_hSimpleParts[i].Identity();

#ifdef PRE_ADD_VEHICLE_ACTION_STRING
	m_strVehicleActionString = "Vehicle_"; // As Default
#endif

	SetHP(10);  // 탈것의 체력은 현재 따로 테이블이나 데이터를 적용하지않고있습니다. 맞게되면 바로 내리게됩니다.
	SetMaxHP(10);
}


CDnVehicleActor::~CDnVehicleActor()
{
	for(int i=0;i<2;i++)
		SAFE_RELEASE_SPTR( m_hSimpleParts[i] );
}


bool CDnVehicleActor::Initialize()
{
	CDnActor::Initialize();

	if( m_hObject ) 
	{
		m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 1 ) );
		m_hObject->SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) | COLLISION_GROUP_DYNAMIC( 2 ) | COLLISION_GROUP_DYNAMIC( 3 ) ); // 충돌 그룹을 일단 플레이어 엑터와 동일한 수준으로 설정 해 놓음.*/
		m_hObject->SetCalcPositionFlag(CALC_POSITION_Y); // 이렇게 넣어주어야 합니다 < AniPosition 관련 >
	} 

	// SetPartsColor(m_hObject,"g_MaterialAmbient",EtInterface::textcolor::SILVER);
	// g_MaterialAmbient
	// g_MaterialDiffuse  탈것 몸통에 적용되는 값 !  

	return true;
}

void CDnVehicleActor::Show( bool bShow )
{
	CDnActor::Show( bShow );
	if( bShow == false )
	{
		ReleaseSignalImp();
	}
}


CDnPlayerActor* CDnVehicleActor::GetMyPlayerActor()
{
	if(m_hMyActor)
	{
		return static_cast<CDnPlayerActor*>(m_hMyActor.GetPointer());
	}

	return NULL;
}
bool CDnVehicleActor::LinkPlayerToVehicle()
{
	if(GetObjectHandle() && m_hMyActor && !m_hMyActor->IsDestroy() && m_hMyActor->GetObjectHandle() && m_hObject )
	{
		m_hMyActor->GetObjectHandle()->SetParent(this->GetObjectHandle(),"~Dummy_Seat");
		return true;
	}

	return false;
}

void CDnVehicleActor::Process(LOCAL_TIME LocalTime, float fDelta )
{
	CDnActor::ProcessActor( LocalTime, fDelta );
	PreProcess( LocalTime, fDelta);

	if( 0 < GetCantXZMoveSEReferenceCount() )
	{
		m_vAniDistance.x = m_vAniDistance.z = 0.0f;
	}

	GetMovement()->Process( LocalTime, fDelta );
	MASingleBody::Process( m_matexWorld, LocalTime, fDelta );
	
	ProcessVehicle(LocalTime , fDelta);
	ProcessTurning(LocalTime,fDelta);
	ProcessHideVehicle(LocalTime,fDelta);
	ProcessVisual( LocalTime, fDelta );
}


void CDnVehicleActor::ProcessHideVehicle( LOCAL_TIME LocalTime, float fDelta )
{
	if( !CDnPlayerActor::s_bEnableAnotherPlayer ) 
		return;

	if( IsLocalVehicle() ) 
		return;

	bool bShow = true;
	bool bCastShadow = true;
	float fAlpha = 1.f;
	if ( CDnPlayerActor::s_fHideAnotherPlayerDelta > 0.0f )
	{
		CDnPlayerActor::s_fHideAnotherPlayerDelta -= fDelta;

		if( CDnPlayerActor::s_fHideAnotherPlayerDelta < 0.0f ) 
			CDnPlayerActor::s_fHideAnotherPlayerDelta = 0.0f;

		if( CDnPlayerActor::s_bHideAnotherPlayer ) 
		{
			bCastShadow = false;
			fAlpha = CDnPlayerActor::s_fHideAnotherPlayerDelta;
		}
		else 
		{
			bShow = true;
			fAlpha = 1.f - CDnPlayerActor::s_fHideAnotherPlayerDelta;
		}

		if( CDnPlayerActor::s_fHideAnotherPlayerDelta == 0.f ) 
		{
			if( CDnPlayerActor::s_bHideAnotherPlayer ) 
				bShow = false;
			else 
				bCastShadow = true;
		}
	}
	else 
	{
		if( CDnPlayerActor::s_bHideAnotherPlayer ) 
		{
			bShow = false;
			bCastShadow = false;
			fAlpha = 0.f;
		}
		else 
		{
			bShow = true;
			bCastShadow = true;
			fAlpha = 1.f;
		}
	}

	if( IsShow() != bShow ) 
		Show( bShow );

	EnableCastShadow( bCastShadow );
	SetAlphaBlend( fAlpha );

}


void CDnVehicleActor::ProcessVehicle(LOCAL_TIME LocalTime, float fDelta)
{

	if(m_hMyActor && GetMyPlayerActor()->IsVehicleMode() ) // 탈것 쓰레드 호출이 들어간이후에 내리는 메세지가 들어오면 이런 이상한 상황이 생깁니다.
	{                                              // 다행히 메세지가 들어올때 정보를 관리하기 때문에 비교를 해서 이상하면 내려줍니다.
		if(GetMyPlayerActor()->GetVehicleInfo().Vehicle[Vehicle::Slot::Body].nItemID == 0)
		{
			GetMyPlayerActor()->UnRideVehicle();
		}
	}

	if(m_hMyActor && !m_hMyActor->IsDestroy() && GetMyPlayerActor()->IsVehicleMode() && m_bAttachToPlayer )
	{
		EtMatrix matVehicle = this->GetBoneMatrix("~Dummy_Seat");
		EtMatrix matVehicleWorld = *GetObjectHandle()->GetWorldMat();
		EtMatrix matActorWorld = *m_hMyActor->GetObjectHandle()->GetWorldMat();
		
		float fHeightGap;
		fHeightGap =matActorWorld._42 - matVehicleWorld._42; 
		matActorWorld._42 -= fHeightGap/2; // 탈것에 탑승하는 순간 캐릭터 충돌박스의 높이갭이 생기므로 중간값을 보정해줍니다.

		MatrixEx Cross;
	
		memcpy( &Cross.m_vXAxis, &matVehicle._11, sizeof(EtVector3) );
		memcpy( &Cross.m_vYAxis, &matVehicle._21, sizeof(EtVector3) );
		memcpy( &Cross.m_vZAxis, &matVehicle._31, sizeof(EtVector3) );
		memcpy( &Cross.m_vPosition, &matVehicle._41, sizeof(EtVector3) );

		m_hMyActor->GetMatEx()->CopyRotationFromThis(&Cross);
		m_hMyActor->SetPosition(Cross.m_vPosition);
		m_hMyActor->GetObjectHandle()->UpdateCollisionPrimitive(matActorWorld,true);

		// UpdateCollisionPrivmitive() 함수는 자신의 오브젝트가 Child() 형태일때는 동작하지 않으므로 따로 돌려줍니다 //

		if( GetMyPlayerActor()->IsEnalbeLinkedPlayerEffect() && GetMyPlayerActor()->GetLinkedPlayerEffect() )
		{
			EtMatrix AxisMat = GetBoneMatrix("~Dummy_Seat");
			GetMyPlayerActor()->GetLinkedPlayerEffect()->SetPosition( (EtVector3)&AxisMat._41 );
		}
	}
}


void CDnVehicleActor::ProcessRotateVehicle()
{

	SSphere Sphere;
	GetBoundingSphere( Sphere );
	float Radius = 0;
	Radius = Sphere.fRadius;

	MatrixEx cFrontCross;
	MatrixEx cBackCross;
	
	memset(cFrontCross,0,sizeof(cFrontCross));
	memset(cBackCross,0,sizeof(cBackCross));

	cFrontCross.SetPosition(GetMatEx()->m_vPosition);
	cFrontCross.CopyRotationFromThis(GetMatEx());
	cFrontCross.MoveLocalZAxis(Radius/2);
	float fFrontHeight = INSTANCE(CDnWorld).GetHeightWithProp( cFrontCross.m_vPosition );
	cFrontCross.SetPosition(EtVector3(cFrontCross.GetPosition().x,fFrontHeight,cFrontCross.GetPosition().z));

	cBackCross.SetPosition(GetMatEx()->m_vPosition);
	cBackCross.CopyRotationFromThis(GetMatEx());
	cBackCross.MoveLocalZAxis(-Radius/2);
	float fBackHeight = INSTANCE(CDnWorld).GetHeightWithProp( cBackCross.m_vPosition );
	cBackCross.SetPosition(EtVector3(cBackCross.GetPosition().x,fBackHeight,cBackCross.GetPosition().z));

	float fRadian = tan( (fFrontHeight-fBackHeight) / Radius );
	float fAngle = EtToDegree(fRadian);

	// fAngle 전후방에 좌표에 비례한 경사면 각도 , 하지만 바로 로테이션에 적용해봐야 지금은 다른곳에서 zVec을 변경하는 경우가 생겨서
	// 정상작동 하지 않는다. 나중에 기울기 로테이션 적용하려고 할때 응용.
}

void CDnVehicleActor::ChangeHairColor(DWORD dwColor)
{
	if(!m_hSimpleParts[Vehicle::Parts::Hair])
		return;

	float fHairColor[3]={0,};
	CDnParts::ConvertR10G10B10ToFloat(fHairColor, dwColor);
	EtColor Color( fHairColor[0], fHairColor[1], fHairColor[2], 1.0f);

	SetPartsColor(m_hSimpleParts[Vehicle::Parts::Hair]->GetObjectHandle(),"g_CustomColor",Color);

	m_dwHairColor = dwColor;
}

void CDnVehicleActor::SetDefaultHairColor()
{
	DNTableFileFormat* pItemTable = GetDNTable( CDnTableDB::TITEM );
	DWORD nVehicleDefaultColor = (DWORD)pItemTable->GetFieldFromLablePtr( m_nItemID , "_TypeParam1" )->GetInteger();
	
	ChangeHairColor(nVehicleDefaultColor); 
	if(m_dwHairColor == 0)
		m_dwHairColor = nVehicleDefaultColor;
}

// 아이템 인덱스만 넣어서 아이템을 착용시키는 형태 , 일반적으로 쓰이지 않는다. < 캐쉬샾에서 쓰임 >
void CDnVehicleActor::EquipItem(int nPartIndex)
{
	if(!GetObjectHandle() || IsDestroy() ) 
		return;

	DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
	DNTableFileFormat* pItem = GetDNTable( CDnTableDB::TITEM );

	if(!pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(nPartIndex) || !pItem || !pItem->IsExistItem(nPartIndex))
		return;

	int nPartsType = pVehiclePartsTable->GetFieldFromLablePtr( nPartIndex, "_VehiclePartsType" )->GetInteger();
	int nItemType = pItem->GetFieldFromLablePtr( nPartIndex, "_Type" )->GetInteger();
	std::string szSkinName = pVehiclePartsTable->GetFieldFromLablePtr( nPartIndex, "_SkinName" )->GetString();

	if(nItemType == ITEMTYPE_VEHICLEHAIRCOLOR)
	{
		DWORD dwVehicleColor = (DWORD)pItem->GetFieldFromLablePtr( nPartIndex , "_TypeParam1" )->GetInteger();
		ChangeHairColor(dwVehicleColor);
		return;
	}

	if(nPartsType == Vehicle::Parts::Default || !((nPartsType == Vehicle::Parts::Hair) || (nPartsType == Vehicle::Parts::Saddle)))
		return;

	if(m_hSimpleParts[nPartsType])
	{
		m_hSimpleParts[nPartsType]->FreeObject();
		m_hSimpleParts[nPartsType]->LinkPartsToObject(szSkinName.c_str(),m_szAniFileName.c_str(),m_hObject); 

		if(nPartsType == Vehicle::Parts::Hair)
			ChangeHairColor(m_dwHairColor);
	}
	else if(m_hSimpleParts[nPartsType] == NULL)
	{
		m_hSimpleParts[nPartsType] =(new CDnSimpleParts)->GetMySmartPtr();
		m_hSimpleParts[nPartsType]->LinkPartsToObject(szSkinName.c_str(),m_szAniFileName.c_str(),m_hObject); 

		if(nPartsType == Vehicle::Parts::Hair)
			ChangeHairColor(m_dwHairColor);
	}
}


void CDnVehicleActor::EquipItem(TVehicleItem tInfo)
{
	if(!GetObjectHandle() || IsDestroy() ) 
		return;

	DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );

	if(!pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(tInfo.nItemID))
		return;

	int nPartsType = pVehiclePartsTable->GetFieldFromLablePtr( tInfo.nItemID, "_VehiclePartsType" )->GetInteger();
	std::string szSkinName = pVehiclePartsTable->GetFieldFromLablePtr( tInfo.nItemID, "_SkinName" )->GetString();


	if(nPartsType == Vehicle::Parts::Default || !((nPartsType == Vehicle::Parts::Hair) || (nPartsType == Vehicle::Parts::Saddle)))
		return;

	if(m_hSimpleParts[nPartsType])
	{
		m_hSimpleParts[nPartsType]->FreeObject();
		m_hSimpleParts[nPartsType]->LinkPartsToObject(szSkinName.c_str(),m_szAniFileName.c_str(),m_hObject); 
		
	             	// 아이템 정보 설정 //
		m_hSimpleParts[nPartsType]->InitializeItem(tInfo.nItemID,0);
		m_hSimpleParts[nPartsType]->SetExpireDate(tInfo.tExpireDate);
		m_hSimpleParts[nPartsType]->SetEternityItem(tInfo.bEternity);
		m_hSimpleParts[nPartsType]->SetSerialID(tInfo.nSerial);
		m_hSimpleParts[nPartsType]->SetOverlapCount( tInfo.wCount );
	            	//////////////////////

		if(nPartsType == Vehicle::Parts::Hair)
			ChangeHairColor(m_dwHairColor);

	}
	else if(m_hSimpleParts[nPartsType] == NULL)
	{
		m_hSimpleParts[nPartsType] =(new CDnSimpleParts)->GetMySmartPtr();
		m_hSimpleParts[nPartsType]->LinkPartsToObject(szSkinName.c_str(),m_szAniFileName.c_str(),m_hObject); 

		// 아이템 정보 설정 //
		m_hSimpleParts[nPartsType]->InitializeItem(tInfo.nItemID,0);
		m_hSimpleParts[nPartsType]->SetExpireDate(tInfo.tExpireDate);
		m_hSimpleParts[nPartsType]->SetEternityItem(tInfo.bEternity);
		m_hSimpleParts[nPartsType]->SetSerialID(tInfo.nSerial);
		m_hSimpleParts[nPartsType]->SetOverlapCount( tInfo.wCount );
		//////////////////////

		if(nPartsType == Vehicle::Parts::Hair)
			ChangeHairColor(m_dwHairColor);
	}
	
}

void CDnVehicleActor::UnEquipItem(Vehicle::Parts::eVehicleParts Type)
{
	if(Type == Vehicle::Parts::Default || !((Type == Vehicle::Parts::Hair) || (Type == Vehicle::Parts::Saddle)))
		return;

	if(m_hSimpleParts[Type])
	{
		m_hSimpleParts[Type]->FreeObject();
	}
}

void CDnVehicleActor::SetDefaultParts()
{
	DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
	if(!pVehicleTable || !pVehicleTable->IsExistItem(m_nItemID))
		return;

	int nVehicleACtorDefaultPARTS_A = pVehicleTable->GetFieldFromLablePtr( m_nItemID, "_DefaultPartsA" )->GetInteger();
	int nVehicleACtorDefaultPARTS_B = pVehicleTable->GetFieldFromLablePtr( m_nItemID, "_DefaultPartsB" )->GetInteger();

	// 기본 파츠는 아이템 아이디에 대한 정보 밖에없습니다. //

	UnEquipItem(Vehicle::Parts::Saddle);
	UnEquipItem(Vehicle::Parts::Hair);

	TVehicleItem tItem;
	memset(&tItem,0,sizeof(tItem));

	tItem.nItemID = nVehicleACtorDefaultPARTS_A;
 	EquipItem(tItem);
	
	tItem.nItemID = nVehicleACtorDefaultPARTS_B;
	EquipItem(tItem);

	SetDefaultHairColor(); // 색도 기본지정
}


void CDnVehicleActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	
	switch( Type ) 
	{

		case STE_ChangeAction:
		{
			ChangeActionStruct *pStruct = (ChangeActionStruct *)pPtr;
			if( _rand()%100 < pStruct->nRandom ) 
			{
				SetActionQueue( pStruct->szAction );
			}
		}
		return;

		case STE_Jump:
			{

				JumpStruct *pStruct = (JumpStruct *)pPtr;
				EtVector2 vVec( 0.f, 0.f );

				// 애니메이션 키 까지 이동 불가일때는 방향키 먹지 않도록 처리한다.
				if( 0 == GetCantXZMoveSEReferenceCount() )
				{
					if( !pStruct->bIgnoreJumpDir ) 
					{
// 						if( m_cMovePushKeyFlag & 0x01 ) vVec.x -= 1.f; 
// 						if( m_cMovePushKeyFlag & 0x02 ) vVec.x += 1.f; // 말은 점프 좌우연산 안먹음
						if( m_cMovePushKeyFlag & 0x04 ) vVec.y += 1.f;
						if( m_cMovePushKeyFlag & 0x08 ) vVec.y -= 1.f;

						EtVec2Normalize( &vVec, &vVec );
					}
				}

				if( pStruct->bResetPrevVelocity ) 
				{
					SetVelocityY( 0.f );
					SetResistanceY( 0.f );
				}

				Jump( pStruct->fJumpVelocity , vVec );
				SetResistanceY( pStruct->fJumpResistance );
			}
			return;
		case STE_VelocityAccel:
			{
				VelocityAccelStruct *pStruct = (VelocityAccelStruct *)pPtr;

				MAWalkMovement *pMovement = dynamic_cast<MAWalkMovement *>(GetMovement());

				// #24949 VelocityAccel 시그널 사용하는 부분도 막도록 처리.
				EtVector3 vVelocity = *pStruct->vVelocity;
				EtVector3 vResistance = *pStruct->vResistance;
				if( 0 < GetCantXZMoveSEReferenceCount() )
				{
					vVelocity.x = vVelocity.z = 0.0f;
					vResistance.x = vResistance.z = 0.0f;
				}

				if( pMovement )
				{
					pMovement->SetVelocityByMoveVector( vVelocity );
					pMovement->SetResistance( vResistance );
				}
			}
			return;
		
	}


	CDnActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
	
}

void CDnVehicleActor::CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	MovePos( vPos, false );
	SetActionQueue( szActionName, nLoopCount, fBlendFrame );
}



void CDnVehicleActor::CmdStop( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bResetStop, bool bForce )
{
	MAMovementBase *pMovement = GetMovement();
	if( !pMovement ) 
		return;

	pMovement->ResetMove();
	SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame );
}


void CDnVehicleActor::CmdAction( const char *szActionName, int nLoopCount, float fBlendFrame, bool bCheckOverlapAction, 
									bool bFromStateBlow/* = false*/, bool bSkillChain/* = false*/ )
{
	CDnActor::CmdAction( szActionName, nLoopCount, fBlendFrame, bCheckOverlapAction, bFromStateBlow, bSkillChain );
}


void CDnVehicleActor::CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, int nLoopCount, float fFrame, float fBlendFrame )
{
	CDnActor::CmdMixedAction( szActionBone, szMaintenanceBone, szActionName, nLoopCount, fFrame, fBlendFrame );
}

void CDnVehicleActor::RenderCustom( float fElapsedTime )
{
	CDnActor::RenderCustom( fElapsedTime );
}



bool CDnVehicleActor::LoadSkin( const char *szSkinName, const char *szAniName, bool bTwoPassAlpha)
{
	bool bResult = MASingleBody::LoadSkin( szSkinName, szAniName, bTwoPassAlpha );
	
	if( !bResult ) 
		return false;

	m_hObject->SetCalcPositionFlag(CALC_POSITION_Y);
	return true;
}


void CDnVehicleActor::OnDrop( float fCurVelocity )
{
	if( IsAir() ) 
	{
		if( !IsHit() ) 
		{

			std::string CurrnentAction = GetCurrentAction();

			// 탈것 액션줄때 캐릭터의 값 받아오는 부분 이펙트나오면 고쳐야함
			if( m_hMyActor && !m_hMyActor->IsDestroy())
			{
				if(GetMyPlayerActor() && GetMyPlayerActor()->GetPartsVehicleEffect() )
				{
					std::string szEffectName = GetMyPlayerActor()->GetPartsVehicleEffect()->GetChangeVehicleActionStr();
					char szStr[64];
					sprintf_s( szStr, "_%s", szEffectName.c_str() );
					szEffectName = szStr;
					RemoveStringA(CurrnentAction, szEffectName);
				}
			}
		
			char szStr[64];
			sprintf_s( szStr, "%s_Landing", CurrnentAction.c_str() );

			if( IsExistAction( szStr ))
			{
				SetActionQueue( szStr, 0, 6.f, 0.f, true, false );
			}
			else 
			{// 만약에 없을경우에 하늘에서 병신짓하구있어서 넣어놉니다. 일단은 마춰서 넣어주는거임
				if( GetVelocity()->y != 0.f )
					SetActionQueue( "Stand", 0, 0.f, 0.f, true, false );
			}

			if( !GetFloorForceVelocity() ) {
				TileTypeEnum TileType = CDnWorld::GetInstance().GetTileType( m_matexWorld.m_vPosition );

				CreateEnviEffectParticle( ConditionEnum::Landing, TileType, &m_matexWorld );
				CreateEnviEffectSound( ConditionEnum::Landing, TileType, m_matexWorld.m_vPosition );
			}

			SetMovable( false );
		}
	}

}

void CDnVehicleActor::OnFall( float fCurVelocity ) // 탈것의 경우는 애니의 크기가 틀리므로 적절한 설정이 필요합니다.
{

	if( ( IsStay() || IsMove() ) && !IsFloorCollision() ) 
	{
		// 움직여지는 각도도 체크해서 계단등을 내려올때 떨어져보이는것 보정해보아요.
		EtVector3 vDir = *GetPosition() - *GetPrevPosition();
		EtVec3Normalize( &vDir, &vDir );
		float fDot = EtVec3Dot( &EtVector3( 0.f, 1.f, 0.f ), &vDir );
		float fAngle = EtToDegree( EtAcos( fDot ) );

		if( fCurVelocity < -10.f && fAngle > 145.f ) 
		{ // 탈것은 점프 /착지모션자체가 크기때문에 아주 높이떨어질때 애니가 설정되도록합니다.
			ActionElementStruct *pStruct = GetElement( "Jump" );
			if( pStruct ) 
			{
				SetActionQueue( "Jump", 0, 10.f, (float)pStruct->dwLength * 1.f/3.8f ); // 이전 모션과 이어지는 다음프레임을 적절한 수준으로 맞추어야합니다.
				SetMovable( false );
			}
		}
	}

}

void CDnVehicleActor::SetDestroy()
{
	CDnActor::SetDestroy();

	m_bAttachToPlayer = false;

	if( CDnPlayerActor::s_bHideAnotherPlayer ) 
	{
		CDnUnknownRenderObject<CDnActor>::SetDestroy();
	}
}



int CDnVehicleActor::GetMoveSpeed()
{
	int nMoveSpeed = CDnActor::GetMoveSpeed();

	if( CDnWorld::IsActive() && ( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage || CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeWorldMap ) )
		nMoveSpeed += GetSafeZoneMoveSpeed();

	if( m_cMovePushKeyFlag & 0x08 ) 
		nMoveSpeed = 200;
	
	return nMoveSpeed;
}


void CDnVehicleActor::SetPartsColor(EtAniObjectHandle hObject,char* Type,EtColor Color)
{
	if( !hObject )
		return;

	int nCustomColor = hObject->AddCustomParam( Type );//"g_CustomColor" ); // 탈것 파츠 색지정

	if( nCustomColor != -1 )  
	{
		hObject->SetCustomParam( nCustomColor, &Color );
	}
}

bool CDnVehicleActor::SetActionQueue( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bCheck, bool bCheckStateEffect )
{
 	std::string VehicleAction;
	std::string strActionName = szActionName;
	std::string strCurAction = szActionName; 

#ifdef PRE_ADD_VEHICLE_ACTION_STRING
	VehicleAction = GetVehicleActionString() + strCurAction;
#else
  	VehicleAction = "Vehicle_" + strCurAction;
#endif

	if( m_hMyActor && !m_hMyActor->IsDestroy() && !m_bRTTRenderMode) m_hMyActor->SetActionQueue(VehicleAction.c_str(),nLoopCount,fBlendFrame,fStartFrame,bCheck,bCheckStateEffect); // 플레이어의 액션을 말액션을 기준으로 맞추어 줍니다.

	// 탈것 액션줄때 캐릭터의 값 받아오는 부분 이펙트나오면 고쳐야함
	if( m_hMyActor && !m_hMyActor->IsDestroy())
	{
		if(GetMyPlayerActor() && GetMyPlayerActor()->GetPartsVehicleEffect() )
		{
			std::string szEffectName = GetMyPlayerActor()->GetPartsVehicleEffect()->GetChangeVehicleActionStr();
			char szStr[64];
			sprintf_s( szStr, "_%s", szEffectName.c_str() );
			strActionName += szStr;
		}
	}

	return CDnActor::SetActionQueue( strActionName.c_str() , nLoopCount, fBlendFrame, fStartFrame, bCheck, bCheckStateEffect );
}

void CDnVehicleActor::ProcessTurning(LOCAL_TIME LocalTime, float fDelta)
{
	float fYawDelta;

	if(!m_bProcessTruning)
		return;

	if(!m_bIsLocalVehicle && m_bProcessTruning)
	{
		if(strstr(GetCurrentAction(),"Left"))
		{
			fYawDelta = -Vehicle::Common::ROTATE_YAWDELTA * Vehicle::Common::ROTATE_ANGLEPOWER * fDelta; 
			GetMatEx()->RotateYaw( Vehicle::Common::ROTATE_YAWDELTA  * Vehicle::Common::ROTATE_ANGLEPOWER * fDelta); 
		}
		else if(strstr(GetCurrentAction(),"Right"))
		{
			fYawDelta = Vehicle::Common::ROTATE_YAWDELTA * Vehicle::Common::ROTATE_ANGLEPOWER * fDelta;   
			GetMatEx()->RotateYaw( -Vehicle::Common::ROTATE_YAWDELTA  * Vehicle::Common::ROTATE_ANGLEPOWER * fDelta); 
		}
	}
	else
	{
		CmdStop("Stand",-1,6.f);
		m_bProcessTruning = false;
	}
}

void CDnVehicleActor::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
{
	switch( dwActorProtocol ) {
		case eActor::SC_CMDMOVE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int			nActionIndex;
				EtVector3	vPos, vXVec;
				EtVector2	vZVec, vLook;
				char		cFlag;
				DWORD		dwGap;
				int nMoveSpeed;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &cFlag, sizeof(char) );
				Stream.Read( &nMoveSpeed, sizeof(int) );
					
				Look( vZVec, false );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );
				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );
				SetMagnetPosition( vPos );


				m_cMovePushKeyFlag = cFlag;
				float fXSpeed = 0.f, fZSpeed = 0.f;
				
				if( cFlag & 0x01 ) 
					fXSpeed = -100000.f;

				if( cFlag & 0x02 ) 
					fXSpeed = 100000.f;

				if( cFlag & 0x04 ) 
					fZSpeed = 100000.f;

				if( cFlag & 0x08 )
					fZSpeed = -100000.f;

				vPos += ( vXVec * fXSpeed );
				vPos += ( EtVec2toVec3( vZVec ) * fZSpeed );

				if(!m_hMyActor || m_hMyActor->IsDestroy())
					break;

				ActionElementStruct *pStruct = m_hMyActor->GetElement( nActionIndex ); // !중요! 플레이어 액터의 인덱스로 검출해야합니다 
				if( pStruct == NULL ) 
					break;

				CTask *pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
				if( pTask ) 
				{
					if( nActionIndex == m_hMyActor->GetCurrentActionIndex() ) 
					{      
						MovePos( vPos, false );
						break;
					}
				}

				CmdMove( vPos, pStruct->szName.c_str(), -1, 8.f );
			}
			break;
		case eActor::SC_CMDMOVEPOS:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int			nActionIndex;
				EtVector3	vPos, vXVec, vTargetPos;
				EtVector2	vZVec;
				DWORD		dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vTargetPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				Look( vZVec, false );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				SetMagnetPosition( vPos );

				m_cMovePushKeyFlag = 0;

				if(!m_hMyActor || m_hMyActor->IsDestroy())
					break;
				
				ActionElementStruct *pStruct = m_hMyActor->GetElement( nActionIndex );
				if( pStruct == NULL ) 
					break;

				CTask *pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
				if( pTask ) 
				{
					if( nActionIndex == m_hMyActor->GetCurrentActionIndex() ) 
					{
						MovePos( vPos, false );
						break;
					}
				}
				CmdMove( vTargetPos, pStruct->szName.c_str(), -1, 8.f );
			}
			break;
		case eActor::SC_CMDSTOP:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3	vPos;
				bool		bResetStop;
				DWORD		dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &bResetStop, sizeof(bool) );

				std::string szActionName = GetCurrentAction();
				if( EtVec2Length( &( EtVector2( vPos.x, vPos.z ) - EtVector2( GetPosition()->x, GetPosition()->z ) ) ) > 100.f ) 
				{
					if( GetState() != ActorStateEnum::Move ) 
						szActionName = "Move_Front";
				}
				
				if(strstr(GetCurrentAction(),"Stand") == NULL)
				{
					CmdStop("Stand",-1,6.f);
				}
			
			}
			break;
		case eActor::SC_CMDACTION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount;
				float fBlendFrame;
				EtVector3 vPos, vXVec;
				EtVector2 vLook, vZVec;
				DWORD dwGap;
				bool bFromStateBlow = false;
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &m_cMovePushKeyFlag, sizeof(char) );
				Stream.Read( &bFromStateBlow, sizeof(bool) );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				ResetMove();

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );
				Look( vLook, true );
				SetMagnetPosition( vPos );

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) 
					break;

				CmdAction( pStruct->szName.c_str(), nLoopCount, fBlendFrame, false );

				if(strstr(pStruct->szName.c_str(),"Turn"))
				{
					EnableTurning(true);
				}
			}
			break;
		case eActor::SC_CMDLOOK:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				EtVector2 vLook, vZVec;
				EtVector3 vXVec;

				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );                         
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				CmdLook( vLook );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );
			}
			break;

		case eActor::SC_POSREV:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos;
				bool bMove;
				DWORD dwGap;

				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &bMove, sizeof(bool) );

				SetMagnetPosition( vPos );
				if( bMove ) 
				{
					EtVector3 vXVec;
					EtVector2 vZVec;

					Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

					EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

					SetMoveVectorX( vXVec );
					SetMoveVectorZ( EtVec2toVec3( vZVec ) );

					if( EtVec3LengthSq( GetMovePos() ) > 0.f ) 
					{
						float fXSpeed = 0.f, fZSpeed = 0.f;

						if( m_cMovePushKeyFlag & 0x01 ) 
							fXSpeed = -100000.f;

						if( m_cMovePushKeyFlag & 0x02 ) 
							fXSpeed = 100000.f;

						if( m_cMovePushKeyFlag & 0x04 ) 
							fZSpeed = 100000.f;

						if( m_cMovePushKeyFlag & 0x08 ) 
							fZSpeed = -100000.f;

						vPos += ( vXVec * fXSpeed );
						vPos += ( EtVec2toVec3( vZVec ) * fZSpeed );
						MovePos( vPos, false );
					}
				}

				OutputDebug( "PosRev\n" );
			}
			break;
		case eActor::SC_VIEWSYNC:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos, vXVec;
				EtVector2 vZVec, vLook;
				DWORD dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				Look( vLook, true );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				SetMagnetPosition( vPos );

				OutputDebug( "ViewSync\n" );

			}
			break;
		case eActor::SC_ONDROP:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				EtVector3 vPos;
				DWORD dwGap;

				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &dwGap, sizeof(DWORD) );

				SetMagnetPosition( vPos );
				SetJumpMovement( EtVector2( 0.f, 0.f ) );
			}
			break;
		case eActor::SC_CMDESCAPE:
			{
				CPacketCompressStream Stream( pPacket, 32 );

				EtVector3 vPos;
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

				SetPosition( vPos );
				SetPrevPosition( vPos );
				SetActionQueue( "Stand" );

				CmdWarp( vPos, EtVec3toVec2( *GetMoveVectorZ() ) );
			}
			break;

	}
	CDnActor::OnDispatchMessage( dwActorProtocol, pPacket );
}

void CDnVehicleActor::SetMagnetPosition( EtVector3 &vPos )
{
	EtVector2 vMagnet;

	float fHeightDiff = vPos.y - GetMatEx()->m_vPosition.y;
	vMagnet.x = vPos.x - GetMatEx()->m_vPosition.x;
	vMagnet.y = vPos.z - GetMatEx()->m_vPosition.z;
	float fAberraionLength = EtVec2Length( &vMagnet );

	if( ( fAberraionLength > 1.f && fAberraionLength < 300.f ) && fHeightDiff < 300.f ) 
	{
		SetMagnetLength( fAberraionLength );
		EtVec2Normalize( &vMagnet, &vMagnet );
		SetMagnetDir( vMagnet );
	}
	else 
	{
		SetPosition( vPos );
	}
}

