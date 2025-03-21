#include "StdAfx.h"
#include "DnWorldHitMoveDamageBrokenProp.h"
#include "DnWorld.h"
#include "DnTableDB.h"
#include "DnWorldSector.h"
#include "NavigationMesh.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnWorldHitMoveDamageBrokenProp::CDnWorldHitMoveDamageBrokenProp(void) : m_bHitMove( false ), 
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
																		 m_fHitDurability( 0.0f ),
																		 m_iActivateElementIndex( -1 ),
																		 m_bActivate( false )

{
}

CDnWorldHitMoveDamageBrokenProp::~CDnWorldHitMoveDamageBrokenProp(void)
{
	ReleasePostCustomParam();
}


// 브로큰 프랍과 프랍 헤더 구조체가 다르므로 여기서 새로 해줘야 함
bool CDnWorldHitMoveDamageBrokenProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPROP );

	LoadHitEffectData( nTableID );
	m_bIsStaticCollision = false;

	if( GetData() ) 
	{
		HitMoveDamageBrokenStruct* pStruct = (HitMoveDamageBrokenStruct *)GetData();
		bool					   bResult = false;

//#61146 Trap타입 프랍의 공격력을 난이도에 따라 다르게 세팅할 수 있는 구조.
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID, pStruct->MonsterWeight );
#else
		bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID );
#endif	

		_ASSERT( bResult );
		if( bResult )
		{
			m_nDurability = pStruct->nDurability;

			if( -1 != m_nDurability )
			{
				if( 0 == m_nDurability ) m_nDurability = 1;
				m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				if( m_nItemDropGroupTableID > 0 )
				{
					CDnDropItem::CalcDropItemList( -1, m_nItemDropGroupTableID, m_VecDropItemList );

					for( DWORD i=0; i<m_VecDropItemList.size(); i++ )
					{
						if( CDnDropItem::PreInitializeItem( m_VecDropItemList[i].nItemID ) == false ) 
						{
							m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
							i--;
						}
					}
				}

				m_iMovingAxis = pStruct->MovingAxis;
				m_fAxisMovingSpeed = pStruct->fDefaultAxisMovingSpeed;
				m_fAxisMaxMoveDistance = pStruct->fMaxAxisMoveDistance;
			}
		}
		else
			return false;
	}

	return true;
}


bool CDnWorldHitMoveDamageBrokenProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldBrokenDamageProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );

	EtVector3 vDir = m_matExWorld.m_vZAxis;
	EtVec3Normalize( &vDir, &vDir );
	m_vInitialPos = m_matExWorld.m_vPosition;
	m_vDestPosition = m_matExWorld.m_vPosition + (vDir * m_fAxisMaxMoveDistance);

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
	NavigationMesh *pNavMesh = INSTANCE(CDnWorld).GetNavMesh( m_matExWorld.m_vPosition );
	if( !pNavMesh )
	{
		//EtVector3 vPrevPos = m_matExWorld.m_vPosition;
		if( CDnWorld::GetInstance().GetAttribute( m_matExWorld.m_vPosition ) != 0 )
		{
			m_matExWorld.m_vPosition = vPrevPos;
		}
		m_matExWorld.m_vPosition.y = CDnWorld::GetInstance().GetHeight( m_matExWorld.m_vPosition );
	}
	else 
	{
		if( m_pCurCell == NULL ) 
		{
			m_pCurCell = pNavMesh->FindClosestCell( m_matExWorld.m_vPosition );
			if( ( m_pCurCell == NULL ) || ( !m_pCurCell->IsPointInCellCollumn( m_matExWorld.m_vPosition ) ) ) 
			{
				m_pCurCell = NULL;

				//EtVector3 vPrevPos = m_matExWorld.m_vPosition;
				if( CDnWorld::GetInstance().GetAttribute( m_matExWorld.m_vPosition ) != 0 )
				{
					m_matExWorld.m_vPosition = vPrevPos;
				}
				m_matExWorld.m_vPosition.y = CDnWorld::GetInstance().GetHeight( m_matExWorld.m_vPosition );
			}
		}
		if( m_pCurCell )
		{
			//EtVector3 vPrevPos = m_matExWorld.m_vPosition;
			//m_matExWorld.MoveLocalZAxis( fValue );
			int nSide = -1;
			NavigationCell *pLastCell = NULL;
			m_pCurCell->FindLastCollision( vPrevPos, m_matExWorld.m_vPosition, &pLastCell, nSide );
			if( nSide != -1 )
			{
				if( pLastCell->Link( nSide ) == NULL )
				{
					EtVector2 vMoveDir2D( m_matExWorld.m_vPosition.x - vPrevPos.x, m_matExWorld.m_vPosition.z - vPrevPos.z );
					float fMoveLength = EtVec2Length( &vMoveDir2D );
					vMoveDir2D /= fMoveLength;
					EtVector2 vWallDir2D = pLastCell->Side( nSide )->EndPointB() - pLastCell->Side( nSide )->EndPointA();
					EtVec2Normalize( &vWallDir2D, &vWallDir2D );
					fMoveLength *= EtVec2Dot( &vWallDir2D, &vMoveDir2D );
					m_matExWorld.m_vPosition.x = vPrevPos.x + fMoveLength * vWallDir2D.x;
					m_matExWorld.m_vPosition.z = vPrevPos.z + fMoveLength * vWallDir2D.y;

					int nNewSide = -1;
					m_pCurCell->FindLastCollision( vPrevPos, m_matExWorld.m_vPosition, &pLastCell, nNewSide );
					if( nNewSide != -1 )
					{
						if( pLastCell->Link( nNewSide ) )
						{
							NavigationCell *pNewCell;
							pNewCell = pLastCell->Link( nNewSide );
							if( pNewCell->IsPointInCellCollumn( m_matExWorld.m_vPosition ) )
							{
								m_pCurCell = pNewCell;
							}
							else
							{
								m_matExWorld.m_vPosition.x = vPrevPos.x;
								m_matExWorld.m_vPosition.z = vPrevPos.z;
							}
						}
						else if( !m_pCurCell->IsPointInCellCollumn( m_matExWorld.m_vPosition ) )
						{
							m_matExWorld.m_vPosition.x = vPrevPos.x;
							m_matExWorld.m_vPosition.z = vPrevPos.z;
						}
					}
				}
				else
				{
					m_pCurCell = pLastCell->Link( nSide );
				}
			}

			if( m_pCurCell->GetType() == NavigationCell::CT_PROP )
				m_matExWorld.m_vPosition.y = m_pCurCell->GetPlane()->SolveForY( m_matExWorld.m_vPosition.x, m_matExWorld.m_vPosition.z );
			else
				m_matExWorld.m_vPosition.y = CDnWorld::GetInstance().GetHeight( m_matExWorld.m_vPosition );
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
	m_vPosition = m_matExWorld.m_vPosition - vOffset;

	// 스케일 값도 있는 경우가 있으므로 고려.
	// m_vCross 크로스 벡터에는 스케일 값을 적용하면 이동에 영향을 미치므로 마지막에 객체를 업데이트 할 때만 사용한다.
	// hit signal 처리할 때는 prop 객체에서 직접 스케일 값을 가져와서 사용하게 된다. EtMatrixEx 엔 이동과 회전만.
	EtMatrix matScale;
	m_matWorld = m_matExWorld;
	EtMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	EtMatrixMultiply( &m_matWorld, &matScale, &m_matWorld );

	m_Handle->Update( &m_matWorld );

	// 이동되면 프랍은 원래 옥트리 업데이트 안하므로 이 녀석만 따로 업데이트 해주도록 한다.
	SSphere Sphere;
	GetBoundingSphere( Sphere );
	m_pCurOctreeNode = static_cast<CDnWorldSector*>(m_pParentSector)->GetOctree()->Update( GetMySmartPtr(), Sphere, m_pCurOctreeNode );
}


// 클라는 밀리면서 움직이고 서버는 한번에 포지션 셋팅 시키자.
void CDnWorldHitMoveDamageBrokenProp::Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta )
{
	CDnWorldActProp::Process( pmatWorld, LocalTime, fDelta );
	if( m_hMonster )
		*m_hMonster->GetMatEx() = *GetMatEx();

	m_bActivate = (GetElementIndex( GetCurrentAction() ) == m_iActivateElementIndex );

	// 일단 걍 이렇게 하고 서버랑 클라랑 프랍 위치가 틀어지면 다른 방법을 강구해보자.
	// 네비게이션 메시를 활용한다!
	if( m_bHitMove )
	{
		m_fMovedTime += fDelta;

		EtVector3 vPrevPos = m_matExWorld.m_vPosition;
		EtVector3 vTargetPos;
		EtVector3* pNowPos = this->GetPosition();

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

		MatrixEx m_matTarget = m_matExWorld;
		m_matTarget.m_vPosition = vTargetPos;

		DNVector(SCollisionResponse) vecResult;
		m_Handle->FindCollision( m_matTarget, vecResult );

		bool bCollision = false;
		if( vecResult.size() > 1 )
		{
			for( int i=0; i<(int)vecResult.size(); i++ )
			{
				if( vecResult[i].pCollisionPrimitive->Type != CT_CAPSULE )
				{
					bCollision = true;
				}
				else
				{
					SCollisionCapsule* pCollisionCapsule = dynamic_cast<SCollisionCapsule*>( vecResult[i].pCollisionPrimitive );
					if( pCollisionCapsule && pCollisionCapsule->fRadius != 18.0f )	// hit한 액터 구분이 안되기 때문에 radius 18.0f 사이즈로 구분 함
					{
						bCollision = true;
						m_bHitMove = false;
						m_fMovedTime = 0.0f;
					}
				}
			}
		}

		if( !bCollision )
			m_matExWorld.m_vPosition = vTargetPos;

		_GetPosProperly( vPrevPos );
		_UpdatePropPos();
	}
	else
	if( m_bActivate &&
		false == m_bArrived &&
		MOVE_AXIS_NONE != m_iMovingAxis &&
		0.0f != m_fAxisMovingSpeed )
	{
		EtVector3 vPrevPos = m_matExWorld.m_vPosition;

		// 정해진 축으로 기본 이동 상태가 정의되어있다면 움직여 준다.
		m_matExWorld.MoveLocalZAxis( m_fAxisMovingSpeed * fDelta );

		// 목적 위치를 초과했다면 도착한 것으로 처리한다.
		EtVector3 vMovableLength = m_vInitialPos - m_vDestPosition;
		float fMovableLengthSQ = EtVec3LengthSq( &vMovableLength );
		EtVector3 vMovedLength = m_vInitialPos - m_matExWorld.m_vPosition;
		float fCurrentMovedLength = EtVec3LengthSq( &vMovedLength );
		if( fMovableLengthSQ < fCurrentMovedLength )
		{
			m_matExWorld.m_vPosition = m_vDestPosition;
			m_bArrived = true;
		}

		_GetPosProperly( vPrevPos );
		_UpdatePropPos();
	}
}

// 브로큰 프랍과 거의 같다. 다른 건 맞으면 이동하고 내구도 다 되면 폭발하여 데미지를 주는 것.
// 데미지 주느 것은 서버에서 처리 됨..
void CDnWorldHitMoveDamageBrokenProp::OnDamage( CDnDamageBase *pHitter, CDnActor::SHitParam &HitParam )
{
	CDnWorldBrokenProp::OnDamage( pHitter, HitParam );

	if( false == m_bBroken )
	{
		// 일단 서버에서 날아온 위치를 셋팅해 주고,
//		m_matExWorld.SetPosition( HitParam.vPosition );
		m_matWorld = m_matExWorld;
		m_Handle->Update( &m_matWorld );

		// 역으로 포지션 값도 보정
		EtVector3 vOffset = *m_pParentSector->GetOffset();
		vOffset.x -= ( m_pParentSector->GetTileWidthCount() * m_pParentSector->GetTileSize() ) / 2.f;
		vOffset.z -= ( m_pParentSector->GetTileHeightCount() * m_pParentSector->GetTileSize() ) / 2.f;
		vOffset.y = 0.f;
		m_vPosition = m_matExWorld.m_vPosition - vOffset;

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
					m_vDir = -m_matExWorld.m_vXAxis;
					break;

				case MOVE_PROPLOCAL_AXIS_Y:
					m_vDir = -m_matExWorld.m_vYAxis;
					break;

				case MOVE_PROPLOCAL_AXIS_Z:
					m_vDir = -m_matExWorld.m_vZAxis;
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
		// 깍이는 내구도에 비례해서 밀리도록 처리하자.
		m_vVelocity = m_vDir * 200.0f * m_fHitDurability;			// 초기 속도
		m_vResist = -m_vDir * 80.0f;			// 저항, 마찰력
		m_vHitPosition = m_matExWorld.m_vPosition;

		m_bHitMove = true;
	}
	else
	{
		// 축 이동하는 녀석인데 broken 된 경우엔 시작 위치로 셋팅해 줌.
		if( false == m_bArrived &&
			MOVE_AXIS_NONE != m_iMovingAxis &&
			0.0f != m_fAxisMovingSpeed )
		{
			m_matExWorld.m_vPosition = m_vInitialPos;
			m_bArrived = true;
			_UpdatePropPos();
		}
	}
}

void CDnWorldHitMoveDamageBrokenProp::OnDamageReadAdditionalPacket( CDnDamageBase::SHitParam &HitParam, CPacketCompressStream *pStream )
{
	pStream->Read( &m_matExWorld.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
//	pStream->Read( &HitParam.vViewVec, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
	pStream->Read( &HitParam.vVelocity, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	pStream->Read( &HitParam.vResistance, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	pStream->Read( &m_fHitDurability, sizeof(float) );
}
