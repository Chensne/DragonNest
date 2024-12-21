#include "StdAfx.h"
#include "DnWorldHitMoveDamageBrokenProp.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DNUserSession.h"
#include "DnWorldSector.h"
#include "NavigationMesh.h"


CDnWorldHitMoveDamageBrokenProp::CDnWorldHitMoveDamageBrokenProp( CMultiRoom* pRoom ) : CDnWorldBrokenDamageProp( pRoom ),
																						m_bHitMove( false ),
																						m_fMoveDistance( 0.0f ),
																						m_fWholeMoveTime( 0.0f ),
																						m_fMovedTime( 0.0f ),
																						m_fVelocity( 0.0f ),
																						m_fResist( 0.0f ),
																						m_pCurCell( NULL ),
																						m_iMovingAxis( MOVE_AXIS_NONE ),
																						m_fAxisMovingSpeed( 0.0f ),
																						m_fAxisMaxMoveDistance( 0.0f ),
																						m_bArrived( false ),
																						m_fDelta( 0.0f ),
																						m_iActivateElementIndex( -1 ),
																						m_bActivate( false )
{
}

CDnWorldHitMoveDamageBrokenProp::~CDnWorldHitMoveDamageBrokenProp(void)
{
	ReleasePostCustomParam();
}


bool CDnWorldHitMoveDamageBrokenProp::InitializeTable(int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	if( GetData() ) {
		HitMoveDamageBrokenStruct*	pStruct = (HitMoveDamageBrokenStruct *)GetData();
		bool						bResult = false;

//#61146 Trap타입 프랍의 공격력을 난이도에 따라 다르게 세팅할 수 있는 구조.
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID, pStruct->MonsterWeight );
#else
		bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID );
#endif	

		if( bResult )
		{
//			m_hMonster->SetUniqueID( STATIC_INSTANCE(CDnActor::s_dwUniqueCount)++ );

			m_nDurability = pStruct->nDurability;

			if( -1 != m_nDurability )
			{
				if( 0 == m_nDurability ) m_nDurability = 1;
				m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				if( m_nItemDropGroupTableID > 0 ) 
				{
					CDnDropItem::CalcDropItemList( GetRoom(), Dungeon::Difficulty::Max, m_nItemDropGroupTableID, m_VecDropItemList );

					if (m_VecDropItemList.empty())
					{
						// 프랍로그
						// g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, 0, nTableID);	// 스테이지 로그 090226
					}
					else
					{
						for( DWORD i=0; i<m_VecDropItemList.size(); i++ )
						{
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
							if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList[i].nItemID, m_VecDropItemList[i].nEnchantID ) == false )
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
							if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList[i].nItemID ) == false )
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
							{
								m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
								i--;
								continue;
							}

							// 프랍로그
							// g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, m_VecDropItemList[i].nItemID, nTableID);	// 스테이지 로그 090226
						}
					}
				}

				m_iMovingAxis = pStruct->MovingAxis;
				m_fAxisMovingSpeed = pStruct->fDefaultAxisMovingSpeed;
				m_fAxisMaxMoveDistance = pStruct->fMaxAxisMoveDistance;
			}

			m_HitComponent.Initialize( m_hMonster, &m_Cross, GetMySmartPtr(), this );
		}
		else
			return false;
	}

	return true;
}


bool CDnWorldHitMoveDamageBrokenProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldBrokenDamageProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );

	EtVector3 vDir = m_Cross.m_vZAxis;
	EtVec3Normalize( &vDir, &vDir );
	m_vInitialPos = m_Cross.m_vPosition;
	m_vDestPosition = m_Cross.m_vPosition + (vDir * m_fAxisMaxMoveDistance);

	if( m_Handle )
		m_Handle->SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) | COLLISION_GROUP_DYNAMIC( 2 ) );

	return bResult;
}


bool CDnWorldHitMoveDamageBrokenProp::CreateObject( void )
{
	bool bResult = CDnWorldBrokenDamageProp::CreateObject();

	if( bResult )
	{
		m_iActivateElementIndex = GetElementIndex( "Activate" );
	}

	return bResult;
}



void CDnWorldHitMoveDamageBrokenProp::_GetPosProperly( EtVector3 &vPrevPos )
{
	NavigationMesh *pNavMesh = INSTANCE(CDnWorld).GetNavMesh( m_Cross.m_vPosition );
	if( !pNavMesh )
	{
		//EtVector3 vPrevPos = m_Cross.m_vPosition;
		if( CDnWorld::GetInstance(GetRoom()).GetAttribute( m_Cross.m_vPosition ) != 0 )
		{
			m_Cross.m_vPosition = vPrevPos;
		}
		m_Cross.m_vPosition.y = CDnWorld::GetInstance(GetRoom()).GetHeight( m_Cross.m_vPosition );
	}
	else 
	{
		if( m_pCurCell == NULL ) 
		{
			m_pCurCell = pNavMesh->FindClosestCell( m_Cross.m_vPosition );
			if( ( m_pCurCell == NULL ) || ( !m_pCurCell->IsPointInCellCollumn( m_Cross.m_vPosition ) ) ) 
			{
				m_pCurCell = NULL;

				//EtVector3 vPrevPos = m_Cross.m_vPosition;
				if( CDnWorld::GetInstance(GetRoom()).GetAttribute( m_Cross.m_vPosition ) != 0 )
				{
					m_Cross.m_vPosition = vPrevPos;
				}
				m_Cross.m_vPosition.y = CDnWorld::GetInstance(GetRoom()).GetHeight( m_Cross.m_vPosition );
			}
		}
		if( m_pCurCell )
		{
			//EtVector3 vPrevPos = m_Cross.m_vPosition;
			//m_Cross.MoveFrontBack( fValue );
			int nSide = -1;
			NavigationCell *pLastCell = NULL;
			m_pCurCell->FindLastCollision( vPrevPos, m_Cross.m_vPosition, &pLastCell, nSide );
			if( nSide != -1 )
			{
				if( pLastCell->Link( nSide ) == NULL )
				{
					EtVector2 vMoveDir2D( m_Cross.m_vPosition.x - vPrevPos.x, m_Cross.m_vPosition.z - vPrevPos.z );
					float fMoveLength = EtVec2Length( &vMoveDir2D );
					vMoveDir2D /= fMoveLength;
					EtVector2 vWallDir2D = pLastCell->Side( nSide )->EndPointB() - pLastCell->Side( nSide )->EndPointA();
					EtVec2Normalize( &vWallDir2D, &vWallDir2D );
					fMoveLength *= EtVec2Dot( &vWallDir2D, &vMoveDir2D );
					m_Cross.m_vPosition.x = vPrevPos.x + fMoveLength * vWallDir2D.x;
					m_Cross.m_vPosition.z = vPrevPos.z + fMoveLength * vWallDir2D.y;

					int nNewSide = -1;
					m_pCurCell->FindLastCollision( vPrevPos, m_Cross.m_vPosition, &pLastCell, nNewSide );
					if( nNewSide != -1 )
					{
						if( pLastCell->Link( nNewSide ) )
						{
							NavigationCell *pNewCell;
							pNewCell = pLastCell->Link( nNewSide );
							if( pNewCell->IsPointInCellCollumn( m_Cross.m_vPosition ) )
							{
								m_pCurCell = pNewCell;
							}
							else
							{
								m_Cross.m_vPosition.x = vPrevPos.x;
								m_Cross.m_vPosition.z = vPrevPos.z;
							}
						}
						else if( !m_pCurCell->IsPointInCellCollumn( m_Cross.m_vPosition ) )
						{
							m_Cross.m_vPosition.x = vPrevPos.x;
							m_Cross.m_vPosition.z = vPrevPos.z;
						}
					}
				}
				else
				{
					m_pCurCell = pLastCell->Link( nSide );
				}
			}

			if( m_pCurCell->GetType() == NavigationCell::CT_PROP )
				m_Cross.m_vPosition.y = m_pCurCell->GetPlane()->SolveForY( m_Cross.m_vPosition.x, m_Cross.m_vPosition.z );
			else
				m_Cross.m_vPosition.y = CDnWorld::GetInstance(GetRoom()).GetHeight( m_Cross.m_vPosition );
		}
	}
}



void CDnWorldHitMoveDamageBrokenProp::_UpdatePropPos()
{
	// 역으로 포지션 값도 보정
	EtVector3 vOffset = *m_pParentSector->GetOffset();
	vOffset.x -= ( m_pParentSector->GetTileWidthCount() * m_pParentSector->GetTileSize() ) / 2.f;
	vOffset.z -= ( m_pParentSector->GetTileHeightCount() * m_pParentSector->GetTileSize() ) / 2.f;
	vOffset.y = 0.f;
	m_vPosition = m_Cross.m_vPosition - vOffset;

	// 스케일 값도 있는 경우가 있으므로 고려.
	// m_vCross 크로스 벡터에는 스케일 값을 적용하면 이동에 영향을 미치므로 마지막에 객체를 업데이트 할 때만 사용한다.
	// hit signal 처리할 때는 prop 객체에서 직접 스케일 값을 가져와서 사용하게 된다. MatrixEx 엔 이동과 회전만.
	EtMatrix matScale;
	m_matWorld = m_Cross;
	EtMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	EtMatrixMultiply( &m_matWorld, &matScale, &m_matWorld );

	m_Handle->Update( &m_matWorld );

	// 이동되면 프랍은 원래 옥트리 업데이트 안하므로 이 녀석만 따로 업데이트 해주도록 한다.
	SSphere Sphere;
	GetBoundingSphere( Sphere );
	m_pCurOctreeNode = static_cast<CDnWorldSector*>(m_pParentSector)->GetOctree()->Update( GetMySmartPtr(), Sphere, m_pCurOctreeNode );
}


void CDnWorldHitMoveDamageBrokenProp::Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta )
{
	// 로컬 타임 기준으로 해야 함.
	// delta 값이 프랍 쪽에 3번에 한번씩 process 돌기 때문에 정확한 값이 오는 게 아님.
	if( 0 == m_PrevLocalTime )
		m_fDelta = fDelta;
	else
		m_fDelta = float(LocalTime - m_PrevLocalTime) * 0.001f;
		
	m_PrevLocalTime = LocalTime;

	if( 0.0f == m_fDelta )
		return;

	CDnWorldBrokenProp::Process( pmatWorld, LocalTime, m_fDelta );

	m_bActivate = (GetElementIndex( GetCurrentAction() ) == m_iActivateElementIndex);
	
	// 일단 걍 이렇게 하고 서버랑 클라랑 프랍 위치가 틀어지면 다른 방법을 강구해보자.
	if( m_bHitMove )
	{
		m_fMovedTime += m_fDelta;

		EtVector3 vPrevPos = m_Cross.m_vPosition;
		EtVector3 vTargetPos;

		if( m_fMovedTime < m_fWholeMoveTime )
		{
			vTargetPos = m_vHitPosition + m_vVelocity*m_fMovedTime + 0.5f*m_vResist*m_fMovedTime*m_fMovedTime;
		}
		else
		{
			// 목적지로 그냥 셋팅
			vTargetPos = m_vHitPosition + m_vVelocity*m_fWholeMoveTime + 0.5f*m_vResist*m_fWholeMoveTime*m_fWholeMoveTime;
			m_bHitMove = false;
			m_fMovedTime = 0.0f;
		}

		MatrixEx m_matTarget = m_Cross;
		m_matTarget.m_vPosition = vTargetPos;

		DNVector(SCollisionResponse) vecResult;
		m_Handle->FindCollision( m_matTarget, vecResult );

		bool bCollision = false;
		if( vecResult.size() > 1 )
		{
			for( int i=0; i<vecResult.size(); i++ )
			{
				if( vecResult[i].pCollisionPrimitive->Type != CT_CAPSULE )
				{
					bCollision = true;
				}
				else
				{
					SCollisionCapsule* pCollisionCapsule = dynamic_cast<SCollisionCapsule*>( vecResult[i].pCollisionPrimitive );
					if( pCollisionCapsule && pCollisionCapsule->fRadius != 18.0f ) // hit한 액터 구분이 안되기 때문에 radius 18.0f 사이즈로 구분 함
					{
						bCollision = true;
						m_bHitMove = false;
						m_fMovedTime = 0.0f;
					}
				}
			}
		}

		if( !bCollision )
			m_Cross.m_vPosition = vTargetPos;

		_GetPosProperly( vPrevPos );
		_UpdatePropPos();
	}
	else if( m_bActivate &&	false == m_bArrived && MOVE_AXIS_NONE != m_iMovingAxis && 0.0f != m_fAxisMovingSpeed )
	{
		EtVector3 vPrevPos = m_Cross.m_vPosition;

		// 정해진 축으로 기본 이동 상태가 정의되어있다면 움직여 준다.
		m_Cross.MoveLocalZAxis( m_fAxisMovingSpeed * m_fDelta );

		// 목적 위치를 초과했다면 도착한 것으로 처리한다.
		EtVector3 vMovableLength = m_vInitialPos - m_vDestPosition;
		float fMovableLengthSQ = EtVec3LengthSq( &vMovableLength );
		EtVector3 vMovedLength = m_vInitialPos - m_Cross.m_vPosition;
		float fCurrentMovedLength = EtVec3LengthSq( &vMovedLength );
		if( fMovableLengthSQ < fCurrentMovedLength )
		{
			m_Cross.m_vPosition = m_vDestPosition;
			m_bArrived = true;
		}

		_GetPosProperly( vPrevPos );
		_UpdatePropPos();
	}
}


void CDnWorldHitMoveDamageBrokenProp::OnDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam )
{
	// #29483 너무 큰 내구도 소모 값은 클리핑 해준다.
	float fOrignalDurability = 0.0f;
	if( 5.0f < HitParam.fDurability )
	{
		fOrignalDurability = HitParam.fDurability;
		HitParam.fDurability = 5.0f;
	}
	//////////////////////////////////////////////////////////////////////////

	// 축 이동되는 타입은 내구도 다 되고 시작위치까지 밀릴 예정이 아니라면
	// 끝까지 도달하고 내구도 다 되었을 때 break 액션이 나와야 하므로 내구도를 다시 회복시켜 줌.
	if( MOVE_AXIS_NONE != m_iMovingAxis &&
		0.0f != m_fAxisMovingSpeed )
	{
		// 목적지까지 날아갈 시간을 구함.
		float fVelocity = EtVec3Length( &HitParam.vVelocity );
		float fResist = EtVec3Length( &HitParam.vResistance );

		float fWholeMoveTime = 0.0f;
		if( 0.0f == fVelocity && 0.0f == fResist )
			fWholeMoveTime = 0.0f;
		else
			fWholeMoveTime = fVelocity / fResist;

		if( fWholeMoveTime > 0.2f )
			fWholeMoveTime = 0.2f;

		EtVector3 vDir;
		switch( m_iMovingAxis )
		{
			case MOVE_PROPLOCAL_AXIS_X:
				vDir = -m_Cross.m_vXAxis;
				break;

			case MOVE_PROPLOCAL_AXIS_Y:
				vDir = -m_Cross.m_vYAxis;
				break;

			case MOVE_PROPLOCAL_AXIS_Z:
				vDir = -m_Cross.m_vZAxis;
				break;
		}
	
		EtVec3Normalize( &vDir, &vDir );

		// 맞은 방향으로 일정 힘을 가한다.
		EtVector3 vVelocity = vDir * 400.0f * HitParam.fDurability;			// 초기 속도
		EtVector3 vResist = -vDir * 80.0f;			// 저항, 마찰력
	 	EtVector3 vHitPosition = m_Cross.m_vPosition;

		EtVector3 vFinalPos = vHitPosition + vVelocity*fWholeMoveTime + 0.5f*vResist*fWholeMoveTime*fWholeMoveTime;
		
		// 어느곳을 때리던 밀리는 방향이 정해져 있으므로 간단하게 거리로 비교가 가능하다.
		// 초기위치보다 더 길다면 초기위치보다 더 뒤로 밀렸다는 이야기가 된다.
		EtVector3 vMovableLength = m_vDestPosition - m_vInitialPos;
		float fMovableLengthSQ = EtVec3LengthSq( &vMovableLength );
		EtVector3 vFinalPosLength = vFinalPos - m_vDestPosition;
		float fFinalPosLengthSQ = EtVec3LengthSq( &vFinalPosLength );
		if( fFinalPosLengthSQ < fMovableLengthSQ )
		{
			// 끝까지 밀리지 않았으므로 CDnWorldBrokenProp::OnDamage() 에서 
			// 내구도가 0이 되어 break 처리 되지 않도록 한다.
			m_nDurability = (int)(HitParam.fDurability * 100.f) + 1;
		}
	}

	CDnWorldBrokenProp::OnDamage( pHitter, HitParam );

	if( false == m_bBroken )
	{
		// 역으로 포지션 값도 보정
		EtVector3 vOffset = *m_pParentSector->GetOffset();
		vOffset.x -= ( m_pParentSector->GetTileWidthCount() * m_pParentSector->GetTileSize() ) / 2.f;
		vOffset.z -= ( m_pParentSector->GetTileHeightCount() * m_pParentSector->GetTileSize() ) / 2.f;
		vOffset.y = 0.f;
		m_vPosition = m_Cross.m_vPosition - vOffset;

		// 목적지까지 날아갈 시간을 구함.
		m_fVelocity = EtVec3Length( &HitParam.vVelocity );
		m_fResist = EtVec3Length( &HitParam.vResistance );

		if( 0.0f == m_fVelocity && 0.0f == m_fResist )
			m_fWholeMoveTime = 0.0f;
		else
			m_fWholeMoveTime = m_fVelocity / m_fResist;
		
		if( m_fWholeMoveTime > 0.2f )
			m_fWholeMoveTime = 0.2f;

		// 움직이는 축이 고정되어있다면 방향 고정으로 처리.
		if( MOVE_AXIS_NONE != m_iMovingAxis &&
			0.0f != m_fAxisMovingSpeed )
		{
			switch( m_iMovingAxis )
			{
				case MOVE_PROPLOCAL_AXIS_X:
					m_vDir = -m_Cross.m_vXAxis;
					break;

				case MOVE_PROPLOCAL_AXIS_Y:
					m_vDir = -m_Cross.m_vYAxis;
					break;

				case MOVE_PROPLOCAL_AXIS_Z:
					m_vDir = -m_Cross.m_vZAxis;
					break;
			}

			EtVec3Normalize( &m_vDir, &m_vDir );

			// 끝까지 도달했을지라도 맞았으면 다시 축 이동 되도록 플래그 켜줌.
			m_bArrived = false;
		}
		else
		{
			m_vDir.y = 0.0f;
			EtVec3Normalize( &m_vDir, &HitParam.vViewVec );
			m_vDir = -m_vDir;
		}

		// 맞은 방향으로 일정 힘을 가한다.
		m_vVelocity = m_vDir * 200.0f * HitParam.fDurability;			// 초기 속도
		m_vResist = -m_vDir * 80.0f;			// 저항, 마찰력
		m_vHitPosition = m_Cross.m_vPosition;

		m_bHitMove = true;
	}
	else
	{
		// 축 이동하는 녀석인데 broken 된 경우엔 시작 위치로 셋팅해 줌.
		if( false == m_bArrived &&
			MOVE_AXIS_NONE != m_iMovingAxis &&
			0.0f != m_fAxisMovingSpeed )
		{
			m_Cross.m_vPosition = m_vInitialPos;
			m_bArrived = true;
			_UpdatePropPos();
		}
	}

	// 내구도 조절했던것 다시 원래대로 해 놓음.
	if( 0.0f < fOrignalDurability )
		HitParam.fDurability = fOrignalDurability;
}

void CDnWorldHitMoveDamageBrokenProp::OnDamageWriteAdditionalPacket( CDnDamageBase::SHitParam &HitParam, CPacketCompressStream *pStream )
{
	pStream->Write( &m_Cross.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
//	pStream->Write( &HitParam.vViewVec, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
	pStream->Write( &HitParam.vVelocity, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	pStream->Write( &HitParam.vResistance, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	pStream->Write( &HitParam.fDurability, sizeof(float) );
}