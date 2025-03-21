#include "Stdafx.h"
#include "MAWalkMovement.h"
#include "EtMatrixEx.h"
#include "DnWorld.h"
#include "DnActionBase.h"
#include "DnActor.h"
#include "VelocityFunc.h"
#include "MAActorRenderBase.h"
#include "PerfCheck.h"
#include "EtTestCollision.h"
#include "navigationcell.h"
#include "navigationmesh.h"
#include "navigationpath.h"
#ifndef _GAMESERVER
#include "EtDecal.h"
#endif

#if defined(_DEBUG) || defined(_RDEBUG)
#define _USE_COLLISION_DEBUG 0
#else
#define _USE_COLLISION_DEBUG 0
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MAWalkMovement::MAWalkMovement()
: m_vMovement( 0.f, 0.f )
, m_vJumpMovement( 0.f, 0.f )
, m_vJumpXVector( 0.f, 0.f, 0.f )
, m_vJumpZVector( 0.f, 0.f, 0.f )
, m_vVelocity( 0.f, 0.f, 0.f )
, m_vVelocityResist( -30.f, -30.f, -30.f )
, m_vLastVelocity( 0.f, 0.f, 0.f )
, m_vMoveVectorX( 0.f, 0.f, 0.f )
, m_vMoveVectorZ( 0.f, 0.f, 0.f )
, m_vMovePos( 0.f, 0.f, 0.f )
, m_vMagnetDir( 0.f, 0.f )
, m_vTargetLookVec( 0.f, 0.f )
, m_vLastVelocityValue( 0.f, 0.f, 0.f )
, m_fMoveYDistancePerSec( 0.0f )
, m_fLeftMoveYDistance( 0.0f )
, m_bAppliedYDistance( false )
, m_bMaintainYDistanceOnArriveDestPosition( false )
, m_bOnDrop(false)
, m_bOnFall(false)
, m_bOnStop(false)
, m_fDropVelocity(0.0f)
, m_fNaviTargetMinDistance(0.0f)
, m_nNaviType(0)
{
	m_fLimitDgrees = 90.f;
	m_PrevLocalTime = 0;
	m_fJumpMoveSpeed = 0.f;
	m_fTargetMinDistance = 0.f;
	m_bRefreshZVector = false;
	m_fMagnetLength = 0.f;
	m_fAngleAssist = 0.f;
	m_bEnableNaviMode = false;
	m_bFloorForceVelocity = false;
	m_bLastFloorForceVelocity = false;
	m_bFloorCollision = false;
	m_LastLookTargetTime = 0;
	m_bDebugRenderAttr = false;
//	m_fLastHeight = 0.f;

#ifndef _GAMESERVER
	m_fLastMoveDelta = 0.f;
	memset( m_fLastMoveLength, 0, sizeof(m_fLastMoveLength) );
	m_nLastMoveCount = 0;
#endif
#ifdef PRE_MOD_NAVIGATION_PATH
	m_bAutoMoving = false;
#endif // PRE_MOD_NAVIGATION_PATH
}

MAWalkMovement::~MAWalkMovement()
{
}


bool Compare_CollisionOrder( SCollisionInfo &a, SCollisionInfo &b )
{
	return a.CollisionNormal.y < b.CollisionNormal.y;
}

EtVector2 g_DirVector[4] =
{
	EtVector2( -0.707107f, 0.707107f ),
	EtVector2( 0.707107f, 0.707107f ),
	EtVector2( 0.707107f, -0.707107f ),
	EtVector2( -0.707107f, -0.707107f )
};

void GetCheckDirctionVector( const int nIndex, EtVector2 & etVector )
{
	if( 0 == nIndex )
	{
		etVector = EtVector2( -0.707107f,  0.707107f );
	}
	else if( 1 == nIndex )
	{
		etVector = EtVector2( 0.707107f,  0.707107f );
	}
	else if( 2 == nIndex )
	{
		etVector = EtVector2( 0.707107f,  -0.707107f );
	}
	else if( 3 == nIndex )
	{
		etVector = EtVector2( -0.707107f,  -0.707107f );
	}
}

bool MAWalkMovement::CheckDiagonalBlock( float fX, float fY )
{
	char cAttr = INSTANCE(CDnWorld).GetAttribute( fX, fY );
	int nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( fX, fY );
	if( nBlockSize == 0 ) return false;

	float fWorldX, fWorldY;
	INSTANCE(CDnWorld).CalcWorldBasePos( fX, fY, fWorldX, fWorldY );

	int nX = int(fWorldX) / nBlockSize;
	int nY = int(fWorldY) / nBlockSize;
	float fTempX = fWorldX - ( nX * nBlockSize );
	float fTempY = fWorldY - ( nY * nBlockSize );

	if( cAttr & 0x10 ) 
	{
		if( fTempX + fTempY > nBlockSize ) return false;
	}
	if( cAttr & 0x20 ) 
	{
		if( fTempX > fTempY ) return false;
	}
	if( cAttr & 0x40 ) 
	{
		if( fTempX + fTempY < nBlockSize ) return false;
	}
	if( cAttr & 0x80 ) 
	{
		if( fTempX < fTempY ) return false;
	}
	return true;
}

bool MAWalkMovement::CheckMovableBlock( char cAttr )
{
	if( cAttr == 0 ) return true;
	if( ( cAttr & 0xf0 ) != 0 ) return true;
	return false;
}

void MAWalkMovement::ProcessCommon( LOCAL_TIME LocalTime, float fDelta )
{
	m_bOnDrop = m_bOnFall = m_bOnStop = false;
	m_fDropVelocity = 0.f;

	m_pActor->SetPrevPosition( m_pMatExWorld->m_vPosition );
	EtVector3 vPrevPos = m_pMatExWorld->m_vPosition;
	EtVector3 vPrevVel = m_vVelocity;

	EtVector2 vAniDistance( 0.f, 0.f );
	EtVector3 *vDist = m_pActor->GetAniDistance();

	if( m_pActor->GetState() & CDnActorState::Move &&
		false == (m_pActor->GetState() & CDnActorState::IgnoreBackMoveSpeed) ) {
		EtVector2 vApplyMoveSpeed( 0.f, 0.f );
		if( m_pActor->GetMoveSpeed() == 0 || fDelta == 0.f ) {
			vApplyMoveSpeed.x = vDist->x;
			vApplyMoveSpeed.y = vDist->z;
		}
		else {
			float fMoveSpeed = m_pActor->GetMoveSpeed() * fDelta;
			vApplyMoveSpeed.x = ( ( 1.f / fDelta ) * vDist->x ) / 300.f;
			vApplyMoveSpeed.y = ( ( 1.f / fDelta ) * vDist->z ) / 300.f;
			vApplyMoveSpeed *= fMoveSpeed;
		}

		vAniDistance += EtVector2( m_vMoveVectorX.x, m_vMoveVectorX.z ) * vApplyMoveSpeed.x;
		vAniDistance += EtVector2( m_vMoveVectorZ.x, m_vMoveVectorZ.z ) * vApplyMoveSpeed.y;
	}
	else {
		vAniDistance += EtVector2( m_pMatExWorld->m_vXAxis.x, m_pMatExWorld->m_vXAxis.z ) * vDist->x;
		vAniDistance += EtVector2( m_pMatExWorld->m_vZAxis.x, m_pMatExWorld->m_vZAxis.z ) * vDist->z;
	}
	m_vMovement += vAniDistance;
 
	// Look 처리
	if( m_hLookTarget ) 
	{
		EtVector2 vLook;

#ifdef PRE_FIX_PARTSMONSTER_AI_TARGETTING
		EtVector3 vTargetPosition = m_hLookTarget->FindAutoTargetPos();
		vLook.x = vTargetPosition.x - m_pMatExWorld->m_vPosition.x;
		vLook.y = vTargetPosition.z - m_pMatExWorld->m_vPosition.z;
#else
		vLook.x = m_hLookTarget->GetPosition()->x - m_pMatExWorld->m_vPosition.x;
		vLook.y = m_hLookTarget->GetPosition()->z - m_pMatExWorld->m_vPosition.z;
#endif

		if( EtVec2LengthSq( &vLook ) > 0.f ) 
		{
			EtVec2Normalize( &vLook, &vLook );
			float fDot = EtVec2Dot( &vLook, &EtVec3toVec2( m_pMatExWorld->m_vZAxis ) );
			if( fDot >= 1.f ) fDot = 1.f;
			float fAngle = EtToDegree( EtAcos( fDot ) );
			bool bForceLook = true;

			if( fAngle > m_pActor->GetRotateAngleSpeed() * fDelta ) bForceLook = false;

			m_pActor->Look( vLook, bForceLook );

			if( bForceLook )
				OnEndLook();
			
			m_LastLookTargetTime = LocalTime;
		}
	}

	if ( m_bEnableNaviMode )
	{
		if( !m_WayPointList.empty() && m_WayPointId != m_WayPointList.end() )
		{
			m_vMovePos = (*m_WayPointId).Position;
		}
		else
		{
			m_bEnableNaviMode = false;
		}
	}
		// 타겟 이동 처리
	else if( m_hMoveTarget ) 
	{
#ifdef PRE_FIX_PARTSMONSTER_AI_TARGETTING
		m_vMovePos = m_hMoveTarget->FindAutoTargetPos();
#else
		m_vMovePos = *m_hMoveTarget->GetPosition();
#endif
	}

	// Look 처리
	if( EtVec2LengthSq( &m_vTargetLookVec ) > 0.f ) {
		float fDot = D3DXVec2Dot( &m_vTargetLookVec, &EtVec3toVec2( m_pMatExWorld->m_vZAxis ) );
		if( fDot >= 1.f ) fDot = 1.f;
		float fAngle = EtToDegree( EtAcos(fDot) );
		float fAngleSpeed = m_pActor->GetRotateAngleSpeed() * fDelta;
		if( fAngle > fAngleSpeed ) {
			EtVector3 vCrossVec;
			D3DXVec3Cross( &vCrossVec, &EtVec2toVec3( m_vTargetLookVec ), &m_pMatExWorld->m_vZAxis );
			if( vCrossVec.y > 0.f )
				m_pMatExWorld->RotateYaw( fAngleSpeed );
			else m_pMatExWorld->RotateYaw( -fAngleSpeed );

			fDot = D3DXVec2Dot( &m_vTargetLookVec, &EtVec3toVec2( m_pMatExWorld->m_vZAxis ) );
			if( fDot >= 1.f ) fDot = 1.f;
			fAngle = EtToDegree( EtAcos(fDot) );
			if( fAngle <= fAngleSpeed ) {
				m_pMatExWorld->m_vZAxis = EtVec2toVec3( m_vTargetLookVec );
				m_pMatExWorld->MakeUpCartesianByZAxis();
				m_vTargetLookVec = EtVector2( 0.f, 0.f );
			}
		}
		else {
			m_pMatExWorld->m_vZAxis = EtVec2toVec3( m_vTargetLookVec );
			m_pMatExWorld->MakeUpCartesianByZAxis();
			m_vTargetLookVec = EtVector2( 0.f, 0.f );
		}
	}

	// 포지션 이동 처리
	if( EtVec3LengthSq( &m_vMovePos ) > 0.f ) {
		EtVector3 vTemp = m_vMovePos - m_pMatExWorld->m_vPosition;
		vTemp.y = 0.f;
		float fLength = EtVec3Length( &vTemp );
		if( !m_hMoveTarget ) {
			if( m_pActor->GetMoveSpeed() > 0.f )
				m_fTargetMinDistance = m_pActor->GetMoveSpeed() * fDelta;
			else m_fTargetMinDistance = EtVec2Length( &m_vMovement );
			if( m_fTargetMinDistance == 0.f ) {
				m_fTargetMinDistance = EtVec3Length( m_pActor->GetAniDistance() );
			}
			m_fTargetMinDistance += 1.f;
		}
		if( fLength <= m_fTargetMinDistance || !m_pActor->IsMove() )
		{
			if( !m_hMoveTarget && fLength <= m_fTargetMinDistance ) {
				EtVec3Normalize( &vTemp, &vTemp );
				m_vMovement += EtVector2( vTemp.x, vTemp.z ) * fLength;
			}

			m_vMovePos = EtVector3( 0.f, 0.f, 0.f );
#ifdef PRE_MOD_NAVIGATION_PATH
			// 자동이동 모드 네비 모드에서는 다음 웨이포인트로 넘긴다.
			if( ( m_bAutoMoving ||m_bEnableNaviMode ) && !m_WayPointList.empty() )
			{
				WayPointID CurrentWayPointID = m_WayPointId;
				m_WayPointId++;
				if ( m_WayPointId == m_WayPointList.end() )
				{
					m_bOnStop = true;
					m_bEnableNaviMode = false;
					m_bAutoMoving = false;
				}
				else
				{
					if( m_bAutoMoving )
						OnAutoMoving( (EtVector3)(*m_WayPointId).Position, (EtVector3)(*CurrentWayPointID).Position );
					else if( m_bEnableNaviMode )
						OnMoveNavi( (EtVector3)(*m_WayPointId).Position );
				}
			}
			else
			{
				m_bOnStop = true;
				m_bEnableNaviMode = false;
				m_bAutoMoving = false;
			}
#else // PRE_MOD_NAVIGATION_PATH
			// 네비 모드에서는 다음 웨이포인트로 넘긴다.
			if( m_bEnableNaviMode && !m_WayPointList.empty() )
			{
				m_WayPointId++;
				if ( m_WayPointId == m_WayPointList.end() )
				{
					m_bOnStop = true;
					m_bEnableNaviMode = false;
				}
				else OnMoveNavi( (EtVector3)(*m_WayPointId).Position );
			}
			else
			{
				m_bOnStop = true;
				m_bEnableNaviMode = false;
			}
#endif // PRE_MOD_NAVIGATION_PATH
		}
		else {
			EtVec3Normalize( &vTemp, &vTemp );
			if( m_bRefreshZVector ) {
				m_vMoveVectorZ = vTemp;
				if( EtVec2LengthSq( &m_vTargetLookVec ) == 0.f ) {
					m_pMatExWorld->m_vZAxis = vTemp;
				}
				else m_vTargetLookVec = EtVec3toVec2( vTemp );
				m_pMatExWorld->MakeUpCartesianByZAxis();
			}
			m_vMovement += EtVector2( vTemp.x, vTemp.z ) * ( m_pActor->GetMoveSpeed() * fDelta );
#ifndef _GAMESERVER
			if( m_pActor->GetMoveSpeed() > 0 ) {
				m_fLastMoveDelta += fDelta;
				if( m_fLastMoveDelta >= FORCE_REVISION_DELTA ) {
					m_fLastMoveDelta -= FORCE_REVISION_DELTA;
					m_nLastMoveCount++;
					if( m_nLastMoveCount == FORCE_REVISION_HISTORY_COUNT ) {
						float fTemp = fLength;
						for( int n=0; n<FORCE_REVISION_HISTORY_COUNT; n++ ) fTemp += m_fLastMoveLength[n];
						fTemp /= ( FORCE_REVISION_HISTORY_COUNT + 1 );

						bool bValidDiff = false;
						float fOffsetSpeed = m_pActor->GetMoveSpeed() / 15.f;
						if( abs( fTemp - fLength ) < fOffsetSpeed ) {
							bValidDiff = true;
							for( int n=0; n<FORCE_REVISION_HISTORY_COUNT; n++ ) 
								if( abs( m_fLastMoveLength[n] - fTemp ) > fOffsetSpeed ) bValidDiff = false;
						}

						if( bValidDiff && !( m_pActor->IsPlayerActor() && (m_pActor->GetMySmartPtr() == CDnActor::s_hLocalActor ) ) ) {
							m_fLastMoveDelta = 0.f;
							m_nLastMoveCount = 0;

							m_vMovement += EtVector2( vTemp.x, vTemp.z ) * fLength;
							m_pMatExWorld->m_vPosition.y = vPrevPos.y = m_vMovePos.y;
							m_pActor->SetPrevPosition( vPrevPos );
							m_vMovePos = EtVector3( 0.f, 0.f, 0.f );

//							OnStop( m_pMatEx->m_vPosition );
							m_bOnStop = true;
							m_bEnableNaviMode = false;

						}
						else {
							m_fLastMoveDelta = 0.f;
							m_nLastMoveCount = 0;
						}
					}
					m_fLastMoveLength[m_nLastMoveCount] = fLength;
				}
			}
#endif
		}
	}
	// Magnet 처리
	if( m_fMagnetLength > 0.f ) {
		float fSpeed = (float)m_pActor->GetMoveSpeed();
		if( fSpeed == 0.f ) fSpeed = 300.f;
		float fMagnetMove = fSpeed * fDelta;
		if( m_fMagnetLength - fMagnetMove <= 0.f ) fMagnetMove = m_fMagnetLength;
		m_vMovement += m_vMagnetDir * fMagnetMove;

		m_fMagnetLength -= fMagnetMove;
		if( m_fMagnetLength <= 0.f ) {
			m_vMagnetDir = EtVector2( 0.f, 0.f );
			m_fMagnetLength = 0.f;
		}
	}

	// Velocity 처리
	EtVector3 vVelocity = EtVector3( 0.f, 0.f, 0.f );
	m_vLastVelocityValue = EtVector3( 0.f, 0.f, 0.f );

	if( m_vVelocity.z ) {
		float fTemp = m_vVelocity.z;
		float fMin = ( fTemp > 0.f ) ? 0.f : -FLT_MAX;
		float fMax = ( fTemp > 0.f ) ? FLT_MAX : 0.f;	

		float fTemp2 = m_vVelocity.z;
		m_vLastVelocityValue.z = CalcMovement( fTemp2, 1.f, fMax, fMin, m_vVelocityResist.z );

		vVelocity += m_vVelocityZVector * CalcMovement( m_vVelocity.z, fDelta, fMax, fMin, m_vVelocityResist.z );

		if( m_vVelocity.z * fTemp <= 0.f ) m_vVelocity.z = 0.f;
	}

	if( m_vVelocity.x ) {
		float fTemp = m_vVelocity.x;
		float fMin = ( fTemp > 0.f ) ? 0.f : -FLT_MAX;
		float fMax = ( fTemp > 0.f ) ? FLT_MAX : 0.f;

		float fTemp2 = m_vVelocity.x;
		m_vLastVelocityValue.x = CalcMovement( fTemp2, 1.f, fMax, fMin, m_vVelocityResist.x );

		vVelocity += m_vVelocityXVector * CalcMovement( m_vVelocity.x, fDelta, fMax, fMin, m_vVelocityResist.x );

		if( m_vVelocity.x * fTemp <= 0.f ) m_vVelocity.x = 0.f;
	}

	m_vMovement += EtVector2( vVelocity.x, vVelocity.z );
	m_fAngleAssist = 0.f;
	if( EtVec2Length( &m_vMovement ) > 0.f ) 
	{
		// 각도에 따른 속도 보정
		EtVector3 vTempPos = m_pMatExWorld->m_vPosition;
		vTempPos.x += m_vMovement.x;
		vTempPos.z += m_vMovement.y;
		vTempPos.y = INSTANCE(CDnWorld).GetHeight( vTempPos );
		if( ( m_pActor->GetAddHeight() == 0.f && m_pMatExWorld->m_vPosition.y == INSTANCE(CDnWorld).GetHeight( m_pMatExWorld->m_vPosition ) ) || m_pMatExWorld->m_vPosition.y <= vTempPos.y ) 
		{
			EtVector3 vMoveNor, vDirNor;
			EtVec3Normalize( &vMoveNor, &EtVector3( m_vMovement.x, vTempPos.y - m_pMatExWorld->m_vPosition.y, m_vMovement.y ) );
			EtVec3Normalize( &vDirNor, &EtVector3( m_vMovement.x, 0.f, m_vMovement.y ) );
			m_fAngleAssist = EtVec3Dot( &vMoveNor, &vDirNor );
			m_vMovement *= m_fAngleAssist;
		}
	}
	// 이동 처리
	EtVector3 vTemp = m_pMatExWorld->m_vPosition;
	vTemp.x += m_vMovement.x - ( vAniDistance.x * m_fAngleAssist );
	vTemp.z += m_vMovement.y - ( vAniDistance.y * m_fAngleAssist );
	vTemp.y += vVelocity.y;
	m_pActor->SetStaticPosition( vTemp );

	m_pMatExWorld->m_vPosition.x += m_vMovement.x;
	m_pMatExWorld->m_vPosition.z += m_vMovement.y;
	m_pMatExWorld->m_vPosition.y += vVelocity.y;

	// 점프 처리
	float fCurVelocity = 0.f;
	float fHeight = INSTANCE(CDnWorld).GetHeight( m_pMatExWorld->m_vPosition ) + m_pActor->GetAddHeight();
	if( m_vVelocity.y ) 
	{
		float fTemp2 = m_vVelocity.y;
		m_vLastVelocityValue.y = CalcMovement( fTemp2, 1.f, FLT_MAX, -FLT_MAX, m_vVelocityResist.y );
		m_pMatExWorld->m_vPosition.y += CalcMovement( m_vVelocity.y, fDelta, FLT_MAX, -FLT_MAX, m_vVelocityResist.y );
		
		if( m_vVelocity.y == 0.f ) 
			m_vVelocity.y += 0.0000001f;

		if( EtVec2LengthSq( &m_vJumpMovement ) > 0.f ) 
		{
			EtVector2 vVec = m_vJumpMovement * ( m_fJumpMoveSpeed * fDelta );

			m_pMatExWorld->m_vPosition += m_vJumpXVector * vVec.x;
			m_pMatExWorld->m_vPosition += m_vJumpZVector * vVec.y;
		}

		if( m_pMatExWorld->m_vPosition.y <= fHeight ) 
		{
			fCurVelocity = m_vVelocity.y - ( ( m_pMatExWorld->m_vPosition.y - fHeight ) * fDelta );
			m_vVelocity.y = 0.f;

			if( m_pActor->GetAddHeight() == 0.f ) 
				m_bFloorForceVelocity = false;

			m_bOnDrop = true;
			m_bLastFloorForceVelocity = m_bFloorForceVelocity;

			if( m_pActor->GetAddHeight() == 0.f ) 
				m_pMatExWorld->m_vPosition.y = fHeight;

			m_bFloorForceVelocity = false;
		}
		else 
		{
			m_bOnFall = true;
		}
	}

	m_fDropVelocity = fCurVelocity;

	// #48950 MoveY 시그널을 사용한 경우 Y 축 이동 처리
	if( m_fMoveYDistancePerSec != 0.0f )
	{
		float fMoveDistance = m_fMoveYDistancePerSec * fDelta;
		m_pMatExWorld->m_vPosition.y += fMoveDistance;

		if( 0.0f < fMoveDistance )
			m_fLeftMoveYDistance -= fMoveDistance;
		else
			m_fLeftMoveYDistance += fMoveDistance;

		// 남은 거리를 넘어설 경우 도착한 거임.
		// 위치를 셋팅해주고 끝냄.
		if( m_fLeftMoveYDistance < 0.0f )
		{
			if( 0.0f < fMoveDistance )
				m_pMatExWorld->m_vPosition.y -= (fMoveDistance + m_fLeftMoveYDistance);
			else
				m_pMatExWorld->m_vPosition.y += (fMoveDistance - m_fLeftMoveYDistance);

			ResetMoveYDistance();

			if( true == m_bMaintainYDistanceOnArriveDestPosition )
			{
				m_bAppliedYDistance = true;
				m_bMaintainYDistanceOnArriveDestPosition = false;
			}
		}
	}


#ifdef _WORK
	if( !( GetAsyncKeyState( VK_CONTROL ) < 0 ) )
#endif
	ProcessBlock( *m_pActor->GetPrevPosition() );

#ifdef _GAMESERVER
	if( m_pActor->IsPlayerActor() ) {
		EtVector2 vTemp2 = EtVector2( vPrevPos.x, vPrevPos.z ) - EtVector2( m_pMatExWorld->m_vPosition.x, m_pMatExWorld->m_vPosition.z );
		AniDistStruct Struct;
		Struct.Time = LocalTime;
		Struct.fDist = EtVec2Length( &vTemp2 ) * ( 1.f / fDelta );
		m_VecAniDistList.push_back( Struct );

		m_fAverageAniDist = FLT_MIN;
		for( DWORD i=0; i<m_VecAniDistList.size(); i++ ) {
			if( LocalTime - m_VecAniDistList[i].Time > 1000 ) {
				m_VecAniDistList.erase( m_VecAniDistList.begin() + i );
				i--;
				continue;
			}
			if( m_VecAniDistList[i].fDist > m_fAverageAniDist )
				m_fAverageAniDist = m_VecAniDistList[i].fDist;
		}
	}
#endif
}

void MAWalkMovement::PostProcess( LOCAL_TIME LocalTime )
{
	if( m_bOnStop ) {
#ifdef _SHADOW_TEST
		if( !m_pActor->IsShadowActor() ) OnStop( m_pMatExWorld->m_vPosition );
#else
		OnStop( m_pMatExWorld->m_vPosition );
#endif
	}
	if( m_bOnDrop ) {
#ifdef _SHADOW_TEST
		if( !m_pActor->IsShadowActor() ) OnDrop( m_fDropVelocity );
#else
		OnDrop( m_fDropVelocity );
#endif
	}
	if( m_bOnFall ) {
#ifdef _SHADOW_TEST
		if( !m_pActor->IsShadowActor() ) OnFall( m_vVelocity.y );
#else
		OnFall( m_vVelocity.y );
#endif
	}

	m_PrevLocalTime = LocalTime;
	m_vMovement = EtVector2( 0.f, 0.f );
}

bool MAWalkMovement::IsMovableBlock( CDnWorld* pWorld, EtVector3 &vPos )
{
	char cAttr = pWorld->GetAttribute( vPos.x, vPos.z );
	int nBlockSize = pWorld->GetAttributeBlockSize( vPos.x, vPos.z );
	float fWorldX, fWorldY;
	pWorld->CalcWorldBasePos( vPos.x, vPos.z, fWorldX, fWorldY );
	float fBlockX = fmodf( fWorldX, (float)nBlockSize );
	float fBlockY = fmodf( fWorldY, (float)nBlockSize );
		
	if(  (( cAttr & 0x0f ) != 1 && ( cAttr & 0x0f ) != 2) ) {					
		return true;
	}

	bool bMovable = false;
	char cHiAttr = ( cAttr & 0xf0 ) >> 4;
	switch( cHiAttr ) {
		case 1:				
			bMovable = fBlockX + fBlockY < nBlockSize;
			break;
		case 2: 				
			bMovable = fBlockX  < fBlockY;
			break;
		case 4: 
			bMovable = fBlockX + fBlockY > nBlockSize;
			break;
		case 8:
			bMovable = fBlockX > fBlockY;
			break;
	}
	return bMovable;
}

void MAWalkMovement::ProcessBlock( EtVector3 vPrevPos )
{	
	EtVector2 vMove;
	vMove.x = m_pMatExWorld->m_vPosition.x - vPrevPos.x;
	vMove.y = m_pMatExWorld->m_vPosition.z - vPrevPos.z;
	EtVector2 vPos(vPrevPos.x, vPrevPos.z);
	EtVector2 vAttrPos = vPos;
	char cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
	int nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
	float fWorldX, fWorldY;
	INSTANCE(CDnWorld).CalcWorldBasePos( vPos.x, vPos.y, fWorldX, fWorldY );
	float fBlockX = fmodf( fWorldX, (float)nBlockSize );
	float fBlockY = fmodf( fWorldY, (float)nBlockSize );

#if (_USE_COLLISION_DEBUG)
	std::vector< int > debugAttr;
	std::vector< EtVector2 > debugPos;
	std::vector< EtVector2 > debugBlock;
	std::vector< EtVector2 > debugBlockNext;
	std::vector< EtVector2 > debugMove;	
	std::vector< EtVector2 > debugAttrPos;	
	std::vector< int > debugCode;
#endif
	int nDebugCode = -1;
	int nStep = 0;
	int nSlideType = 0;

	if( EtVec2LengthSq(&vMove) > 2000.0f * 2000.0f ) {						// 한프레임에 20 미터 이상은 이동 못하도록.
		nStep = 101;
	}

	bool bBreakLoop = false;
	do
	{
		nStep++;
		if( nStep > 100 ) { // 이런 경우가 생기면 안된다.
			OutputDebug("Unknown Collision Error. Loop count exceed.\n");
			vPos = EtVector2(vPrevPos.x, vPrevPos.z);
			break;
		}

		if( EtVec2LengthSq(&vMove) <= 0.001f ) {
			break;
		}

		float fBlockNextX = fBlockX + vMove.x;
		float fBlockNextY = fBlockY + vMove.y;

#if (_USE_COLLISION_DEBUG)
		debugAttr.push_back( cAttr );
		debugPos.push_back( vPos);
		debugBlock.push_back( EtVector2(fBlockX, fBlockY));
		debugBlockNext.push_back( EtVector2(fBlockNextX, fBlockNextY));
		debugMove.push_back( vMove );
		debugAttrPos.push_back( EtVector2(vAttrPos.x/50.f, vAttrPos.y/50.f));
#endif

		// 대각선 방향 벡터와의 외적을 통해 다음에 충돌할 블럭의 방향을 미리 알아낸다.
		int DirIndices[2][2] = {{7,5},{1,3}};
		int BitX = (vMove.x >= 0 ? 1 : 0);
		int BitY = (vMove.y >= 0 ? 1 : 0);
		float fDirX =  BitX * nBlockSize - fBlockX;
		float fDirY =  BitY * nBlockSize - fBlockY;

		int nCollideDir = DirIndices[BitX][BitY] + ((fDirX * vMove.y - fDirY * vMove.x > 0 ) ? 1 : -1);
		nCollideDir = (nCollideDir>>1)&3; // 0 2 4 6 8 -> 0 1 2 3

		float fDa = 0.f;
		float fDb = 0.f;
		int nIndex = -1;
		bool bCollideBlock = (( cAttr & 0x0f ) == 1 || ( cAttr & 0x0f ) == 2);		// 0x01, 0x02 는 충돌체크
		char cHiAttr = ( cAttr & 0xf0 ) >> 4;
		if( bCollideBlock ) {					
			switch( cHiAttr ) {
				case 1:
					if( nCollideDir == 1 || nCollideDir == 2 )  {
						fDa = fBlockX + fBlockY - nBlockSize;
						fDb = fBlockNextX + fBlockNextY - nBlockSize;
						nIndex = 0;
					}
					break;
				case 2: 
					if( nCollideDir == 0 || nCollideDir == 1) {
						fDa = fBlockX - fBlockY;
						fDb = fBlockNextX - fBlockNextY;
						nIndex = 1; 
					}
					break;
				case 4: 
					if( nCollideDir == 0 || nCollideDir == 3 )  {
						fDa = nBlockSize - fBlockX - fBlockY;
						fDb = nBlockSize - fBlockNextX - fBlockNextY;
						nIndex = 2; 
					}
					break;
				case 8: 
					if( nCollideDir == 2 || nCollideDir == 3 ) {
						fDa = fBlockY - fBlockX;
						fDb = fBlockNextY - fBlockNextX;
						nIndex = 3; 
					}
					break;
			}			
		}

		float fDiagEpsilon = 0.01f;
		float fEpsilon = fDiagEpsilon * 1.4142f;
		if( fDa <= 0.f &&  fDb > 0) {			// 대각선 충돌해서 방향 바뀐 경우
			nDebugCode = 1;
			float fRatio = (-fDa-fDiagEpsilon) / (fDb-fDa);
			vPos.x += vMove.x * fRatio;
			vPos.y += vMove.y * fRatio;
			fBlockX += vMove.x * fRatio;
			fBlockY += vMove.y * fRatio;

			ASSERT( (vPos.x==vPos.x&&vPos.y==vPos.y) );
			vMove *= min(1,(1 - fRatio));

			// g_DirVector라는 전역 변수에 핵킹 툴이 접근하므로 이를 전역변수를 사용 하지 않고 상수값을 받아서 처리하도록 수정합니다.
			//vMove = g_DirVector[ nIndex ] * D3DXVec2Dot( &g_DirVector[ nIndex ], &vMove );

			EtVector2 vCheckDirVector;
			GetCheckDirctionVector( nIndex, vCheckDirVector );
			vMove = vCheckDirVector * D3DXVec2Dot( &vCheckDirVector, &vMove );

			if( nSlideType == 2 ) {	// 슬라이딩 처리 한번이상 하지 않는다. 모서리에 걸리는 경우 떨림 방지.
				vMove = EtVector2(0,0);
			}
			nSlideType = 1;
			// 블럭 넘어가는지 체크

			if( fBlockX+vMove.x < 0.f || fBlockX+vMove.x > nBlockSize || 
				fBlockY+vMove.y < 0.f || fBlockY+vMove.y > nBlockSize )
			{
				bool bDiagDirA = vMove.x+vMove.y > 0.f;
				bool bDiagDirB = vMove.x-vMove.y > 0.f;

				if( (cHiAttr == 2 && !bDiagDirB ) || (cHiAttr==1 && !bDiagDirA) ) {
					vAttrPos.x -= nBlockSize;
					fBlockX += nBlockSize;
				}
				else if( (cHiAttr ==4 && bDiagDirB ) ||  (cHiAttr==8 && bDiagDirA ) ) {
					vAttrPos.x += nBlockSize;
					fBlockX -= nBlockSize;
				}
				else if( (cHiAttr==1 && bDiagDirB ) || (cHiAttr==8 && !bDiagDirA) ) {
					vAttrPos.y -= nBlockSize;
					fBlockY += nBlockSize;
				}
				else if( (cHiAttr==2 && bDiagDirA ) || (cHiAttr==4 && !bDiagDirB ) ) {
					vAttrPos.y += nBlockSize;
					fBlockY -= nBlockSize;
				}
				else {
					ASSERT( false );
				}
				cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
				nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
			}
			else 
			{
				bBreakLoop = true;
			}
		}
		// 네모 블럭 충돌 한 경우 (밖에서 온 경우만) 미끄러져서 다음 블럭 까지 이동
		else if( bCollideBlock && cHiAttr == 0 && (fBlockX < 0 || fBlockX >= nBlockSize || fBlockY < 0 || fBlockY >= nBlockSize) ) {
			int OutDirIndices[2][2] = {{7,5},{1,3}};
			int OutBitX = (vMove.x > 0.f) ? 0 : 1;
			int OutBitY = (vMove.y > 0.f) ? 0 : 1;
			float fOutDirX =  OutBitX * nBlockSize - fBlockX;
			float fOutDirY =  OutBitY * nBlockSize - fBlockY;
			int nOutCollideDir = OutDirIndices[OutBitX][OutBitY] + ((fOutDirX * vMove.y - fOutDirY * vMove.x > 0 ) ? -1 : 1);
			nOutCollideDir = (nOutCollideDir>>1)&3; // 0 2 4 6 8 -> 0 1 2 3
			nDebugCode = nOutCollideDir;
			if( nOutCollideDir == 2 ) {
				float fLocalDa = fBlockY - nBlockSize;
				float fLocalDb = fBlockNextY - nBlockSize;				
				if( fLocalDa >= 0 && fLocalDb <= fEpsilon )
				{
					ASSERT( fLocalDa >= fLocalDb );
					nDebugCode |= 0x10;
					float fRatio = (fLocalDa-fEpsilon) / (fLocalDa - fLocalDb);					
					if(fLocalDa>fEpsilon) vPos.x += vMove.x * fRatio;					
					vPos.y += vMove.y * fRatio;
					if(fLocalDa>fEpsilon) fBlockX += vMove.x * fRatio;
					fBlockY += vMove.y * fRatio;
					ASSERT( (vPos.x==vPos.x&&vPos.y==vPos.y) );
					vMove *= min(1, (1.f - fRatio ));
					vMove.y = 0.f;	
					if( nSlideType == 1 ) {
						vMove = EtVector2(0,0);
						bBreakLoop = true;
					}
					nSlideType = 2;
					if( fBlockNextX >= nBlockSize ) {
						nDebugCode |= 0x20;
						vAttrPos.x += nBlockSize;
						vAttrPos.y += nBlockSize;
						fBlockX -= nBlockSize;
						fBlockY -= nBlockSize;
						cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
						nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
						// continue to next step
					}
					else if( fBlockNextX < 0 ) { 
						nDebugCode |= 0x40;
						vAttrPos.x -= nBlockSize;
						vAttrPos.y += nBlockSize;
						fBlockX += nBlockSize;
						fBlockY -= nBlockSize;
						cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
						nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
						// continue to next step
					}
					else {		// end of loop					
						bBreakLoop = true;
					}
				}
				else {
					vPos = EtVector2(vPrevPos.x, vPrevPos.z);		// Unknown Collision Error
					break;
				}
			}
			else if( nOutCollideDir == 0 ) {
				float fLocalDa = fBlockY;
				float fLocalDb = fBlockNextY;
				if( fLocalDa <= 0 && fLocalDb >= -fEpsilon)  
				{
					ASSERT( fLocalDa <= fLocalDb );
					nDebugCode |= 0x10;
					float fRatio = (-fLocalDa-fEpsilon) / (fLocalDb - fLocalDa);
					if(-fLocalDa>fEpsilon) vPos.x += vMove.x * fRatio;
					vPos.y += vMove.y * fRatio;
					if(-fLocalDa>fEpsilon) fBlockX += vMove.x * fRatio;
					fBlockY += vMove.y * fRatio;
					ASSERT( (vPos.x==vPos.x&&vPos.y==vPos.y) );
					vMove *= min(1, (1.f - fRatio ));
					vMove.y = 0.f;	
					if( nSlideType == 1 ) {
						vMove = EtVector2(0,0);
						bBreakLoop = true;
					}
					nSlideType = 2;
					if( fBlockNextX >= nBlockSize ) {
						nDebugCode |= 0x20;
						vAttrPos.x += nBlockSize;
						vAttrPos.y -= nBlockSize;
						fBlockX -= nBlockSize;
						fBlockY += nBlockSize;
						cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
						nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
						// continue to next step
					}
					else if( fBlockNextX < 0 ) { 
						nDebugCode |= 0x40;
						vAttrPos.x -= nBlockSize;
						vAttrPos.y -= nBlockSize;
						fBlockX += nBlockSize;
						fBlockY += nBlockSize;
						cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
						nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
						// continue to next step
					}
					else {		// end of loop
						bBreakLoop = true;
					}
				}
				else {
					vPos = EtVector2(vPrevPos.x, vPrevPos.z);		// Unknown Collision Error
					break;
				}
			}
			else if( nOutCollideDir == 3 ) {
				float fLocalDa = fBlockX;
				float fLocalDb = fBlockNextX;
				if( (fLocalDa <= 0 && fLocalDb >= -fEpsilon) )
				{
					ASSERT( fLocalDa <= fLocalDb );
					nDebugCode |= 0x10;
					float fRatio = (-fLocalDa-fEpsilon) / (fLocalDb - fLocalDa);
					vPos.x += vMove.x * fRatio;
					if(-fLocalDa>fEpsilon) vPos.y += vMove.y * fRatio;					
					fBlockX += vMove.x * fRatio;
					if(-fLocalDa>fEpsilon) fBlockY += vMove.y * fRatio;
					ASSERT( (vPos.x==vPos.x&&vPos.y==vPos.y) );
					vMove *= min( 1, (1.f - fRatio ));
					vMove.x = 0.f;	
					if( nSlideType == 1 ) {
						vMove = EtVector2(0,0);
						bBreakLoop = true;
					}
					nSlideType = 2;
					if( fBlockNextY >= nBlockSize ) {
						nDebugCode |= 0x20;
						vAttrPos.x -= nBlockSize;
						vAttrPos.y += nBlockSize;						
						fBlockX += nBlockSize;
						fBlockY -= nBlockSize;						
						cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
						nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
						// continue to next step
					}
					else if( fBlockNextY < 0 ) { 
						nDebugCode |= 0x40;
						vAttrPos.x -= nBlockSize;
						vAttrPos.y -= nBlockSize;
						fBlockX += nBlockSize;
						fBlockY += nBlockSize;
						cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
						nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
						// continue to next step
					}
					else {		// end of loop					
						bBreakLoop = true;
					}
				}
				else {
					vPos = EtVector2(vPrevPos.x, vPrevPos.z);		// Unknown Collision Error
					break;
				}
			}
			else if( nOutCollideDir == 1 ) {
				float fLocalDa = fBlockX - nBlockSize;
				float fLocalDb = fBlockNextX - nBlockSize ;
				if( fLocalDa >= 0 && fLocalDb <= fEpsilon )
				{
					ASSERT( fLocalDa >= fLocalDb );
					nDebugCode |= 0x10;
					float fRatio = (fLocalDa-fEpsilon) / (fLocalDa - fLocalDb);
					vPos.x += vMove.x * fRatio;
					if(fLocalDa>fEpsilon) vPos.y += vMove.y * fRatio;		
					fBlockX += vMove.x * fRatio;
					if(fLocalDa>fEpsilon) fBlockY += vMove.y * fRatio;
					ASSERT((vPos.x==vPos.x&&vPos.y==vPos.y));
					vMove *= min(1, (1.f - fRatio ));
					vMove.x = 0.f;	
					if( nSlideType == 1 ) {
						vMove = EtVector2(0,0);
						bBreakLoop = true;
					}
					nSlideType = 2;
					if( fBlockNextY >= nBlockSize ) {
						nDebugCode |= 0x20;
						vAttrPos.x += nBlockSize;
						vAttrPos.y += nBlockSize;
						fBlockX -= nBlockSize;
						fBlockY -= nBlockSize;
						cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
						nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
						// continue to next step
					}
					else if( fBlockNextY < 0 ) { 
						nDebugCode |= 0x40;
						vAttrPos.x += nBlockSize;
						vAttrPos.y -= nBlockSize;
						fBlockX -= nBlockSize;
						fBlockY += nBlockSize;
						cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
						nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
						// continue to next step
					}
					else {		// end of loop
						bBreakLoop = true;
					}
				}
				else {
					vPos = EtVector2(vPrevPos.x, vPrevPos.z);		// Unknown Collision Error
					break;
				}
			}
		}
		else { // 그외의 경우 다음 블럭까지 가는지 체크
			if( fBlockNextX < 0 || fBlockNextX >= nBlockSize || fBlockNextY < 0 || fBlockNextY >= nBlockSize ) {
				switch( nCollideDir )
				{
				case 0:
					vAttrPos.y -= nBlockSize;
					fBlockY += nBlockSize;
					break;
				case 1:
					vAttrPos.x += nBlockSize;
					fBlockX -= nBlockSize;
					break;
				case 2:
					vAttrPos.y += nBlockSize;
					fBlockY -= nBlockSize;					
					break;
				case 3:
					vAttrPos.x -= nBlockSize;
					fBlockX += nBlockSize;
					break;
				}
				cAttr = INSTANCE(CDnWorld).GetAttribute( vAttrPos.x, vAttrPos.y );
				nBlockSize = INSTANCE(CDnWorld).GetAttributeBlockSize( vAttrPos.x, vAttrPos.y );
				nDebugCode = 255;
				// continue to next step
			}
			else {
				bBreakLoop = true;
			}
		}


#if( _USE_COLLISION_DEBUG )
		debugCode.push_back( nDebugCode );
#endif

		if( bBreakLoop ) {
			vPos.x += vMove.x;
			vPos.y += vMove.y;
			ASSERT((vPos.x==vPos.x&&vPos.y==vPos.y));
		}
	}while( !bBreakLoop );	

	m_pMatExWorld->m_vPosition.x = vPos.x;
	m_pMatExWorld->m_vPosition.z = vPos.y;
	bool bClash = true;

	char cAttr2 = INSTANCE(CDnWorld).GetAttribute( m_pMatExWorld->m_vPosition );
	if( ( cAttr2 & 0x0f ) == 1 || ( cAttr2 & 0x0f ) == 2 ) {
		char cHiAttr = ( cAttr2 & 0xf0 ) >> 4;
		if( cHiAttr != 0 ) 
		{
			if( !CheckDiagonalBlock( m_pMatExWorld->m_vPosition.x, m_pMatExWorld->m_vPosition.z ) ) bClash = false;
		}
		else bClash = false;
	}
	if( bClash == false ) 
	{
		m_pMatExWorld->m_vPosition.x = vPrevPos.x;
		m_pMatExWorld->m_vPosition.z = vPrevPos.z;
	}
	else {
		if( ( cAttr & 0x0f ) == 2 && EtVec3LengthSq( &( m_pMatExWorld->m_vPosition - vPrevPos ) ) > 0.f ) {
	#ifdef _SHADOW_TEST
			if( !m_pActor->IsShadowActor() ) OnClash( m_pMatExWorld->m_vPosition, eCFCT_None ); // note by kalliste : 이 지점에서 바닥충돌 관련 검출이 필요 없어 None으로 셋팅. 이후 필요할 경우 NoFloor 셋팅할 것.
	#else
			OnClash( m_pMatExWorld->m_vPosition, eCFCT_None );
	#endif
		}
	}
}

void MAWalkMovement::ProcessProp( EtVector3 vPrevPos )
{
	m_bFloorCollision = false;
	if( m_pActor->GetObjectHandle() )
	{
#ifdef _GAMESERVER	// 서버에서는 정밀하게 할필요 없어서 크게 한다.
		const float fMinMove = 20.0f;
#else
		const float fMinMove = 5.0f;
#endif
		EtVector3 vDiffNormal;
		float fLength;

		vDiffNormal = m_pMatExWorld->m_vPosition - vPrevPos;
		fLength = EtVec3Length( &vDiffNormal );
		if( fLength <= fMinMove )
		{
			ProcessCollision( m_pActor, m_pActor->GetObjectHandle(), vPrevPos, m_bFloorCollision );
		}
		else
		{
			EtVector3 vTemp = m_pMatExWorld->m_vPosition;
			vDiffNormal /= fLength;
			int nCount = 0;
			EtVector3 vCurPrevPos = vPrevPos;
			while( fLength > 0.0f )
			{
				if( fLength > fMinMove )
				{
					m_pMatExWorld->m_vPosition = vCurPrevPos + fMinMove * vDiffNormal;
					fLength -= fMinMove;
				}
				else
				{
					m_pMatExWorld->m_vPosition = vCurPrevPos + fLength * vDiffNormal;
					fLength = 0.0f;
				}
				ProcessCollision( m_pActor, m_pActor->GetObjectHandle(), vCurPrevPos, m_bFloorCollision );
				m_pActor->GetObjectHandle()->Update( *m_pMatExWorld );
				vCurPrevPos = m_pMatExWorld->m_vPosition;
				nCount++;
#ifdef _GAMESERVER
				if( nCount == 50 )
#else
				if( nCount == 200 )
#endif
				{
					m_pMatExWorld->m_vPosition = vCurPrevPos;
					break;
				}
			}
		}
	}

	if( m_bFloorCollision ) {
		if( m_pActor->GetAddHeight() < 0.f ) m_bFloorCollision = false;
	}

	if( m_bFloorCollision == false ) 
	{
		if( m_pActor->GetAddHeight() != 0.f ) 
		{
			if( m_vVelocity.y == 0.f ) 
			{
				m_vVelocity.y = -1.f;
				m_vVelocityResist.y = -20.f;
				m_vJumpMovement = EtVector2( 0.f, 0.f );
				m_bFloorForceVelocity = true;
			}
		}
		m_pActor->SetAddHeight( 0.f );
	}

	if( m_bFloorCollision == false && m_vVelocity.y == 0.f )
	{
#ifdef PRE_FIX_FALL_PROP_COLLISION
		m_vVelocity.y = -1.f;
		m_vVelocityResist.y = -20.f;
		m_vJumpMovement = EtVector2( 0.f, 0.f );
		m_bFloorForceVelocity = true;
#else
		float fHeight = INSTANCE(CDnWorld).GetHeightWithProp( EtVector3( m_pMatExWorld->m_vPosition.x, m_pMatExWorld->m_vPosition.y + 100.f, m_pMatExWorld->m_vPosition.z ) );
		float fCurHeight = fHeight + m_pActor->GetAddHeight();
		m_pMatExWorld->m_vPosition.y = fCurHeight;
#endif
	}
}

void MAWalkMovement::Process( LOCAL_TIME LocalTime, float fDelta )
{
//	m_fLastHeight = m_pMatEx->m_vPosition.y;
	ProcessCommon( LocalTime, fDelta );

#ifndef _GAMESERVER
	DebugRenderAttr();
#endif	

	EtVector3 vPrevPos = *m_pActor->GetPrevPosition();	
	ProcessProp( vPrevPos );
 
	PostProcess( LocalTime );
}

void MAWalkMovement::ProcessCollision( CDnActor *pActor, EtObjectHandle hObject, const EtVector3 &vPrevPos, bool &bFloorCollision )
{
#ifdef _WORK
	if( GetAsyncKeyState( VK_CONTROL ) < 0 ) return;
#endif

	int nLoop, i;
	float fMoveDist;
	EtVector3 vMoveDir;	

	DNVector(SCollisionResponse) &vecResult = m_vecResult;
	vecResult.clear();

	vMoveDir = m_pMatExWorld->m_vPosition - vPrevPos;
	fMoveDist = EtVec3Length( &vMoveDir );
	if( fMoveDist == 0.0f ) {
		return;
	}
	vMoveDir /= fMoveDist;
	if( hObject->FindCollision( *m_pMatExWorld, vecResult ) )
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 올라갈 수 있나 체크. 
		bool bIsClimb = CheckClimb( hObject, vPrevPos, vecResult, bFloorCollision );
		if( bIsClimb )
		{
			float fLandHeight = INSTANCE(CDnWorld).GetHeight( m_pMatExWorld->m_vPosition );
			m_pActor->SetAddHeight( m_pMatExWorld->m_vPosition.y - fLandHeight );
			return;
		}

		if( fMoveDist <= 0.0f )
		{
			if( m_pActor->GetAddHeight() > 0.f )
				bFloorCollision = true;
			return;
		}

		for( nLoop = 0; nLoop < 5; nLoop++ )
		{
			SCollisionResponse CurResponse;
			CurResponse.fContactTime = FLT_MAX;
			for( i = 0; i < ( int )vecResult.size(); i++ )
			{
				EtVector3 vCurNormal = vecResult[ i ].vNormal;
				// 충돌은 났지만 노말의 크기가 1.0이 안되면 슬라이딩 후보에 들어가지 않음
/*				if( EtVec3LengthSq( &vCurNormal ) <= 0.5f )
				{
					continue;
				}*/
				if( EtVec3Dot( &vCurNormal, &vMoveDir ) > 0.0f )	// 벽의노말과 이동방향이 같으면 무시
				{
					continue;
				}
				if( vecResult[ i ].fContactTime < CurResponse.fContactTime )
				{
					CurResponse = vecResult[ i ];
				}
			}
			// 슬라이딩할만한 충돌면을 못찾았으면 예전 포지션으로 돌린다.
			if( CurResponse.fContactTime == FLT_MAX )
			{
//				m_pMatEx->m_vPosition = vPrevPos;
				break;
			}

			bool bStopSlide = false;
			bool bIsWall = true;
			if( EtVec3Dot( &CurResponse.vNormal, &EtVector3( 0.0f, 1.0f, 0.0f ) ) > 0.707106f )	// 45도 체크
			{
				if( vMoveDir.y < -0.9999f )	// 경사면에 가만히 서 있을때는 안미끄러져야 한다.
				{
					m_pMatExWorld->m_vPosition.y = vPrevPos.y;
					bStopSlide = true;
				}
 				bIsWall = false;
				bFloorCollision = true;
			}
			else if( EtVec3Dot( &CurResponse.vNormal, &EtVector3( 0.0f, -1.0f, 0.0f ) ) > 0.707106f )	// 45도 천장체크
			{
				bIsWall = false;
			}
			if( !bStopSlide )
			{
				EtVector3 vSlideDir;
				float fWeight;
				CalcSlide( vSlideDir, fWeight, CurResponse.vNormal, vMoveDir );
				if( ( bIsWall ) && ( vSlideDir.y > 0.0f ) && ( vMoveDir.y <= 0.0f ) )	// 벽면은 기어올라 갈수 없지만.. vMoveDir.y > 0.0f 커서 점프 일때는 슬라이딩 일어나야 한다.
				{
					EtVector3 vWallNormal;
					vWallNormal = CurResponse.vNormal;
					vWallNormal.y = 0.0f;
					EtVec3Normalize( &vWallNormal, &vWallNormal );
					CalcSlide( vSlideDir, fWeight, vWallNormal, vMoveDir );
				}
				else if( ( bIsWall ) && ( vSlideDir.y < 0.0f ) )	// 땅 위에 서있는채로 더 큰 물체의 아래에 파고드려하는 경우인데, 가게하면 안된다. 앞으로 가게 하면 컬리전을 파고들어버리게 된다.
				{
					if( m_pActor->GetState() & CDnActorState::Move && (m_pActor->GetState() & CDnActorState::Air) == false )
					{
						EtVector3 vWallNormal;
						vWallNormal = CurResponse.vNormal;
						vWallNormal.y = 0.0f;
						EtVec3Normalize( &vWallNormal, &vWallNormal );
						CalcSlide( vSlideDir, fWeight, vWallNormal, vMoveDir );
					}
				}
				vMoveDir = vSlideDir;
				fMoveDist *= fWeight;
				m_pMatExWorld->m_vPosition = vPrevPos + vMoveDir * fMoveDist;
			}

			vecResult.clear();
			if( hObject->FindCollision( *m_pMatExWorld, vecResult ) )
			{
				for( i = ( int )vecResult.size() - 1; i >= 0; i-- )
				{
					if( EtVec3Dot( &vecResult[ i ].vNormal, &CurResponse.vNormal ) > 0.996194f )	// 충돌 노멀이 5도 정도 밖에 차이 안나면 같은 면으로 생각해서 스킵
					{
						vecResult.erase( vecResult.begin() + i );
					}
				}
			}
			if( vecResult.empty() )
			{
				break;
			}
		}
		if( nLoop == 5 )
		{
			if( bFloorCollision )
			{
				float fLandHeight;
				fLandHeight = INSTANCE(CDnWorld).GetHeight( m_pMatExWorld->m_vPosition );
				if( m_pMatExWorld->m_vPosition.y > fLandHeight )
				{
					m_pActor->SetAddHeight( m_pMatExWorld->m_vPosition.y - fLandHeight );
				}
				m_pMatExWorld->m_vPosition = vPrevPos;
			}
			else	// 바닥과 충돌이 안났을때는 x,z만 Previous로 바꿔준다. 두벽사이의 모서리에 끼어서 점프 했을때 계속 slide 하면서 이동 못하고 여기 들어오는 경우 있다.
			{
				m_pMatExWorld->m_vPosition.x = vPrevPos.x;
				m_pMatExWorld->m_vPosition.z = vPrevPos.z;
				// 바닥과 충돌이 안나더라도 충돌 체크해보고 y도 못움직이면 y도 옛날 위치로 바꿔줌
				vecResult.clear();
				if( hObject->FindCollision( *m_pMatExWorld, vecResult ) )
				{
					for( i = 0; i < ( int )vecResult.size(); i++ )
					{
						// 벽사이에 비비다 보면 벽과 충돌이 날수도 있어서 벽은 제외한다. 
						if( EtVec3Dot( &vecResult[ i ].vNormal, &EtVector3( 0.0f, 1.0f, 0.0f ) ) > 0.087155f )
						{
							m_pMatExWorld->m_vPosition.y = vPrevPos.y;
							bFloorCollision = true;
							break;
						}
					}
				}
			}
		}
		else
		{
			float fLandHeight;
			fLandHeight = INSTANCE(CDnWorld).GetHeight( m_pMatExWorld->m_vPosition );
			if( fLandHeight > m_pMatExWorld->m_vPosition.y )
			{
				m_pMatExWorld->m_vPosition.y = fLandHeight;
			}
			else if( bFloorCollision )
			{
				// 바닥 충돌보다 기울어진 벽충돌이 먼저 일어 나게 되면 밑으로 슬라이딩 처리가 되서.. y값이 이미 낮아져 있다.
				// 그런 상태에서 바닥 충돌이 나게 되면 계속 아래쪽으로 꺼지는 현상이 있어서 이전 y값으로 돌려준다.
				if( m_pMatExWorld->m_vPosition.y < vPrevPos.y )
				{
					m_pMatExWorld->m_vPosition.y = vPrevPos.y;
				}
				m_pActor->SetAddHeight( m_pMatExWorld->m_vPosition.y - fLandHeight );
			}
		}

	#ifdef _SHADOW_TEST
			if( !m_pActor->IsShadowActor() ) OnClash( m_pMatExWorld->m_vPosition, bFloorCollision ? eCFCT_Floor : eCFCT_NoFloor );
	#else
			OnClash( m_pMatExWorld->m_vPosition, bFloorCollision ? eCFCT_Floor : eCFCT_NoFloor );
	#endif

		char cAttr = INSTANCE(CDnWorld).GetAttribute( m_pMatExWorld->m_vPosition );
		if( ( cAttr & 0x0f ) == 1 || ( cAttr & 0x0f ) == 2 ) // 0x01, 0x02 는 충돌체크
		{ 
			m_pMatExWorld->m_vPosition.x = vPrevPos.x;
			m_pMatExWorld->m_vPosition.z = vPrevPos.z;
		}
	}
}

bool MAWalkMovement::CheckClimb( EtObjectHandle hObject, const EtVector3 &vPrevPos, DNVector(SCollisionResponse) &vecResult, bool &bFloorCollision )
{
	int i;
	float fMinContactTime;;
	SCollisionResponse Response;
	const float fClimbBasis = MAX_CLIMB_HEIGHT;
	SCollisionPrimitive *pFloor;

	pFloor = NULL;
	fMinContactTime = 1.0f;
	for( i = 0; i < ( int )vecResult.size(); i++ )
	{
		if( EtVec3Dot( &vecResult[ i ].vNormal, &EtVector3( 0.0f, 1.0f, 0.0f ) ) > 0.707106f )
		{
			hObject->CEtCollisionEntity::FindCollision( *vecResult[ i ].pCollisionPrimitive, EtVector3( 0.0f, -fClimbBasis, 0.0f ), Response, true );
			if( ( Response.fContactTime >= 0.0f ) && ( fMinContactTime > Response.fContactTime ) )
			{
				fMinContactTime = Response.fContactTime;
				pFloor = vecResult[ i ].pCollisionPrimitive;
			}
		}
	}

	if( fMinContactTime != 1.0f )
	{
		float fMoveHeight;

		fMoveHeight = ( 1.0f - fMinContactTime ) * fClimbBasis;
		if( fMoveHeight - ( vPrevPos.y - m_pMatExWorld->m_vPosition.y ) <= MAX_CLIMB_HEIGHT )
		{
			DNVector(SCollisionResponse) &vecClimbResult = m_vecResult;
			vecClimbResult.clear();
			bFloorCollision = true;
			m_pMatExWorld->m_vPosition.y += fMoveHeight;
			hObject->FindCollision( *m_pMatExWorld, vecClimbResult );
			for( i = 0; i < ( int )vecClimbResult.size(); i++ )
			{
				if( vecClimbResult[ i ].pCollisionPrimitive != pFloor )
				{
					break;
				}
			}
			if( i < ( int )vecClimbResult.size() )
			{
				m_pMatExWorld->m_vPosition.y -= fMoveHeight;
				return false;
			}
			bFloorCollision = true;
			return true;
		}
	}

	return false;
}


void MAWalkMovement::CalcSlide( EtVector3 &SlideDir, float &fWeight, EtVector3 &CollisionNormal, EtVector3 &MoveDir )
{
	EtVector3 WallCross;

	EtVec3Cross( &WallCross, &CollisionNormal, &MoveDir );
	EtVec3Normalize( &WallCross, &WallCross );
	EtVec3Cross( &SlideDir, &WallCross, &CollisionNormal );
	EtVec3Normalize( &SlideDir, &SlideDir );
	fWeight = fabs( EtVec3Dot( &SlideDir, &MoveDir ) );
}

void MAWalkMovement::PushAndCollisionCheck( EtVector3 &vPush )
{
	EtVector3 vPrevPos;
	bool bFloorCollision;

	vPrevPos = m_pMatExWorld->m_vPosition;
	m_pMatExWorld->m_vPosition += vPush;

	ProcessCollision( m_pActor, m_pActor->GetObjectHandle(), vPrevPos, bFloorCollision );
}

void MAWalkMovement::SetLimitDgrees( float fDgrees )
{
	m_fLimitDgrees = fDgrees;
}

void MAWalkMovement::MoveZ( float fSpeed )
{
	m_vMovement += EtVector2( m_vMoveVectorZ.x, m_vMoveVectorZ.z ) * fSpeed;
}

void MAWalkMovement::MoveX( float fSpeed )
{
	m_vMovement += EtVector2( m_vMoveVectorX.x, m_vMoveVectorX.z ) * fSpeed;
}

void MAWalkMovement::SetMoveVectorX( EtVector3 &vVec )
{
	m_vMoveVectorX = vVec;
}

void MAWalkMovement::SetMoveVectorZ( EtVector3 &vVec )
{
	m_vMoveVectorZ = vVec;
}

void MAWalkMovement::Jump( float fPower, EtVector2 &vVec )
{
	m_vLastVelocity.y = fPower;
	m_vVelocity.y += fPower;
	m_vJumpMovement = vVec;
	m_vJumpXVector = m_vMoveVectorX;
	m_vJumpZVector = m_vMoveVectorZ;
	m_fJumpMoveSpeed = (float)m_pActor->GetMoveSpeed();
	m_bFloorForceVelocity = false;
}

void MAWalkMovement::MoveJumpX( float fSpeed )
{
	if( m_vJumpMovement.x > 0.f && fSpeed > 0.f ) return;
	if( m_vJumpMovement.x < 0.f && fSpeed < 0.f ) return;
	if( fSpeed > 0.f ) fSpeed = -fSpeed;
	m_fJumpMoveSpeed += fSpeed;
	if( m_fJumpMoveSpeed < 0.f ) m_fJumpMoveSpeed = 0.f;
}

void MAWalkMovement::MoveJumpZ( float fSpeed )
{
	if( m_vJumpMovement.y > 0.f && fSpeed > 0.f ) return;
	if( m_vJumpMovement.y < 0.f && fSpeed < 0.f ) return;
	if( fSpeed > 0.f ) fSpeed = -fSpeed;
	m_fJumpMoveSpeed += fSpeed;
	if( m_fJumpMoveSpeed < 0.f ) m_fJumpMoveSpeed = 0.f;
}

void MAWalkMovement::SetVelocityByMoveVector( EtVector3 &vVec )
{
#ifdef PRE_MOD_VELOCITYACCEL_SIGNAL
	// 컬리전 위에 올라가있는 상태에서 STE_VelocityAccel 시그널이 올때 문제가 발생할 가능성이 있다.
	// 현재 컬리전 위에 올라가있을땐 가만히 세워둔 상태임에도 불구하고,
	// 계속해서 OnDrop과 OnFall, m_bFloorCollision값 등이 수시로 바뀌면서 내부 코드가 돌게 되어있다.
	// (물론 보이기엔 컬리전 위에 가만 있는 상태다.)
	// 이 상태에서 STE_VelocityAccel 시그널이 와서 m_vVelocity의 y값을 0으로 초기화시켜버리면,
	// 바닥에 있던 컬리전을 뚫고 아래로 빠져버린다.
	// 그래서 컬리전 위에 올라가있는 상태에서 STE_VelocityAccel 시그널의 y가 0이라서 높이에 영향을 주지 않는거라면,
	// 기존의 m_vVelocity.y 값을 유지하도록 하겠다.
	// 이것과 더불이 항상 같이 쓰이는게 VelocityResist라서 이 값도 같은 처리를 해준다.
	// 아래 m_vLastVelocity는 쓰이는데가 없는거 같아서 제외시킴.
	bool bKeepVelocityY = false;
	float fKeepVelocityY = 0.0f;
	if( m_bLastFloorForceVelocity && vVec.y == 0.0f )
	{
		bKeepVelocityY = true;
		fKeepVelocityY = m_vVelocity.y;
	}
#endif

	m_vVelocity = vVec;
#ifdef PRE_MOD_VELOCITYACCEL_SIGNAL
	if( bKeepVelocityY ) m_vVelocity.y = fKeepVelocityY;
#endif

	m_vVelocityXVector = m_vMoveVectorX;
	m_vVelocityYVector = m_pMatExWorld->m_vYAxis;
	m_vVelocityZVector = m_vMoveVectorZ;

	m_vJumpMovement = EtVector2( 0.f, 0.f );

	m_vLastVelocity = vVec;
	m_bFloorForceVelocity = false;
}

void MAWalkMovement::SetVelocity( EtVector3 &vVec )
{
	m_vVelocity = vVec;

	m_vVelocityXVector = m_pMatExWorld->m_vXAxis;
	m_vVelocityYVector = m_pMatExWorld->m_vYAxis;
	m_vVelocityZVector = m_pMatExWorld->m_vZAxis;

	m_vJumpMovement = EtVector2( 0.f, 0.f );

	m_vLastVelocity = vVec;
	m_bFloorForceVelocity = false;
}


void MAWalkMovement::SetVelocityX( float fPower )
{
	m_vVelocity.x = fPower;
	m_vVelocityXVector = m_pMatExWorld->m_vXAxis;

	m_vLastVelocity.x = fPower;
}

void MAWalkMovement::SetVelocityY( float fPower )
{
	m_vVelocity.y = fPower;

	m_vVelocityYVector = m_pMatExWorld->m_vYAxis;

	m_vJumpMovement = EtVector2( 0.f, 0.f );

	m_vLastVelocity.y = fPower;
	m_bFloorForceVelocity = false;
}

void MAWalkMovement::SetVelocityZ( float fPower )
{
	m_vVelocity.z = fPower;
	m_vVelocityZVector = m_pMatExWorld->m_vZAxis;

	m_vLastVelocity.z = fPower;
}

void MAWalkMovement::SetResistance( EtVector3 &vVec )
{
#ifdef PRE_MOD_VELOCITYACCEL_SIGNAL
	bool bKeepVelocityResistY = false;
	float fKeepVelocityResistY = 0.0f;
	if( m_bLastFloorForceVelocity && vVec.y == 0.0f )
	{
		bKeepVelocityResistY = true;
		fKeepVelocityResistY = m_vVelocityResist.y;
	}
#endif

	m_vVelocityResist = vVec;
#ifdef PRE_MOD_VELOCITYACCEL_SIGNAL
	if( bKeepVelocityResistY ) m_vVelocityResist.y = fKeepVelocityResistY;
#endif
}

void MAWalkMovement::SetResistanceX( float fPower )
{
	m_vVelocityResist.x = fPower;
}

void MAWalkMovement::SetResistanceY( float fPower )
{
	m_vVelocityResist.y = fPower;
}

void MAWalkMovement::SetResistanceZ( float fPower )
{
	m_vVelocityResist.z = fPower;
}

void MAWalkMovement::MovePos( EtVector3 &vPos, bool bRefreshZVec )
{
	m_bEnableNaviMode = false;
	m_vMovePos = vPos;
	m_bRefreshZVector = bRefreshZVec;

	if( bRefreshZVec ) {
		m_vTargetLookVec.x = vPos.x - m_pMatExWorld->m_vPosition.x;
		m_vTargetLookVec.y = vPos.z - m_pMatExWorld->m_vPosition.z;
		EtVec2Normalize( &m_vTargetLookVec, &m_vTargetLookVec );
	}

#ifndef _GAMESERVER
	m_fLastMoveDelta = 0.f;
	m_nLastMoveCount = 0;
#endif 
}

void MAWalkMovement::MoveTarget( DnActorHandle &hActor, float fMinDistance )
{
	m_hMoveTarget = hActor;
	m_fTargetMinDistance = fMinDistance;
	m_bRefreshZVector = true;
	m_bEnableNaviMode = false;
}

DnActorHandle MAWalkMovement::GetMoveTarget()
{
	return m_hMoveTarget;
}

void MAWalkMovement::MoveTargetNavi( DnActorHandle &hActor, float fMinDistance, const char* szActionName )
{
	MoveTargetNavi( *hActor->GetPosition(), fMinDistance, szActionName );
	if( m_bEnableNaviMode )
	{
		m_hNaviTarget	= hActor;
		m_nNaviType		= NaviType::eTarget;
	}
}

void MAWalkMovement::MoveTargetNavi( EtVector3& vTargetPos, float fMinDistance, const char* szActionName )
{
	m_fNaviTargetMinDistance = fMinDistance;
	m_szNaviTargetActionName = szActionName;

	m_hLookTarget.Identity();
	m_WayPointList.clear();

	m_fTargetMinDistance	= fMinDistance;
	m_bEnableNaviMode		= false;
	m_nNaviType				= NaviType::eMax;
	m_bRefreshZVector		= true;

	EtVector3 vStartPos = this->m_pMatExWorld->m_vPosition;
	EtVector3 vEndPos	= vTargetPos;

	if( m_bRefreshZVector ) 
	{
		m_vTargetLookVec.x = vTargetPos.x - m_pMatExWorld->m_vPosition.x;
		m_vTargetLookVec.y = vTargetPos.z - m_pMatExWorld->m_vPosition.z;
		EtVec2Normalize( &m_vTargetLookVec, &m_vTargetLookVec );
	}

	CEtWorldGrid* pGrid = INSTANCE(CDnWorld).GetGrid();
	if( pGrid == NULL )
		return;
	NavigationMesh* pNaviMesh = pGrid->GetNavMesh(vStartPos);
	if( pNaviMesh == NULL )
		return;

	NavigationCell*	pStartCell	= pNaviMesh->FindClosestCell( vStartPos );
	NavigationCell*	pEndCell	= pNaviMesh->FindClosestCell( vEndPos );
	if ( !pStartCell || !pEndCell )
		return;

	NavigationPath naviPath;
	pNaviMesh->BuildNavigationPath( naviPath, pStartCell, vStartPos, pEndCell, vEndPos );
	WAYPOINT_LIST& fur_waypoint_list = naviPath.LineOfSightWayPointList();
	if( fur_waypoint_list.empty() )
		return;

	m_WayPointList	= fur_waypoint_list;
	m_WayPointId	= m_WayPointList.begin();
	m_WayPointId++;							// 맨첨위치는 현재 위치기 때문에 다음것으로 세팅
	OnMoveNavi( (EtVector3)(*m_WayPointId).Position );
	m_bEnableNaviMode	= true;
	m_nNaviType			= NaviType::ePosition;
}

#ifdef PRE_MOD_NAVIGATION_PATH
void MAWalkMovement::AutoMoving( EtVector3& vTargetPos, float fMinDistance, const char* szActionName, bool bGeneratePath )
{
	m_fNaviTargetMinDistance = fMinDistance;
	m_szNaviTargetActionName = szActionName;

	m_hLookTarget.Identity();
	m_WayPointList.clear();

	m_fTargetMinDistance	= fMinDistance;
	m_bEnableNaviMode		= false;
	m_nNaviType				= NaviType::eMax;
	m_bRefreshZVector		= true;

	EtVector3 vStartPos = this->m_pMatExWorld->m_vPosition;
	EtVector3 vEndPos	= vTargetPos;

	if( m_bRefreshZVector ) 
	{
		m_vTargetLookVec.x = vTargetPos.x - m_pMatExWorld->m_vPosition.x;
		m_vTargetLookVec.y = vTargetPos.z - m_pMatExWorld->m_vPosition.z;
		EtVec2Normalize( &m_vTargetLookVec, &m_vTargetLookVec );
	}

	CEtWorldGrid* pGrid = INSTANCE(CDnWorld).GetGrid();
	if( pGrid == NULL )
		return;
	NavigationMesh* pNaviMesh = pGrid->GetNavMesh(vStartPos);
	if( pNaviMesh == NULL )
		return;

	NavigationCell*	pStartCell	= pNaviMesh->FindClosestCell( vStartPos );
	NavigationCell*	pEndCell	= pNaviMesh->FindClosestCell( vEndPos );

	// 네비게이션 메쉬를 벗어나 생기는 정점에 대해 고려가 필요함

	if ( !pStartCell || !pEndCell )
		return;

	if( bGeneratePath )
		GenerateAutoMovingPath( pNaviMesh, pStartCell, pEndCell, vStartPos, vEndPos );

	m_bEnableNaviMode	= true;
	m_nNaviType			= NaviType::ePosition;
}

// Solve the Catmull-Rom parametric equation for a given time(t) and vector quadruple (p1,p2,p3,p4)
EtVector3 Eq(float t, const EtVector3& p1, const EtVector3& p2, const EtVector3& p3, const EtVector3& p4)
{
	float t2 = t * t;
	float t3 = t2 * t;

	float b1 = static_cast<float>( .5 * (  -t3 + 2*t2 - t) );
	float b2 = static_cast<float>( .5 * ( 3*t3 - 5*t2 + 2) );
	float b3 = static_cast<float>( .5 * (-3*t3 + 4*t2 + t) );
	float b4 = static_cast<float>( .5 * (   t3 -   t2    ) );

	return (p1*b1 + p2*b2 + p3*b3 + p4*b4); 
}

EtVector3 GetInterpolatedSplinePoint(std::vector<EtVector3>& vecList, float t)
{
	float delta_t = 1.0f / static_cast<float>(vecList.size() - 1);
	// Find out in which interval we are on the spline
	int p = (int)(t / delta_t);
	// Compute local control point indices
#define BOUNDS(pp) { if (pp < 0) pp = 0; else if (pp >= static_cast<int>( vecList.size() )) pp = static_cast<int>( vecList.size() ) - 1; }
	int p0 = p - 1;     BOUNDS(p0);
	int p1 = p;         BOUNDS(p1);
	int p2 = p + 1;     BOUNDS(p2);
	int p3 = p + 2;     BOUNDS(p3);
	// Relative (local) time 
	float lt = (t - delta_t*(float)p) / delta_t;
	// Interpolate
	return Eq(lt, vecList[p0], vecList[p1], vecList[p2], vecList[p3]);
}

void MAWalkMovement::GenerateAutoMovingPath( NavigationMesh* pNaviMesh, NavigationCell* pStartCell, NavigationCell* pEndCell, EtVector3& vStartPos, EtVector3& vEndPos )
{
	if( pNaviMesh == NULL || pStartCell == NULL || pEndCell == NULL )
		return;

	NavigationPath naviPath;
	pNaviMesh->BuildNavigationPath( naviPath, pStartCell, vStartPos, pEndCell, vEndPos );

	WAYPOINT_LIST& fur_waypoint_list = naviPath.LineOfSightWayPointList();

	m_LOSWayPointList.clear();
	std::list<WAYPOINT>::iterator iter1 = fur_waypoint_list.begin();
	for( ; iter1 != fur_waypoint_list.end(); iter1++ )
		m_LOSWayPointList.push_back( (*iter1) );

	if( fur_waypoint_list.size() < 2 )	// 직선으로 경로가 나올 경우 Pass
		return;

	// 꺽이는 부분만 쪼개서 곡선화
	WAYPOINT_LIST waypointlist;
	int nInnerNodeCount = static_cast<int>( fur_waypoint_list.size() ) - 2;	// 시작과 끝점을 제외한 중간 노드 갯 수

	std::list<WAYPOINT>::iterator iterCurrent = fur_waypoint_list.begin();

	// 갱신되는 노드 셋팅
	waypointlist.push_back( *iterCurrent );	// 시작 노드
	iterCurrent++;	// 시작 노드의 바로 다음 노드 부터 시작

	for( int i=0; i<nInnerNodeCount; i++, iterCurrent++ )
	{
		std::list<WAYPOINT>::iterator iterPrev = iterCurrent;
		iterPrev--;
		std::list<WAYPOINT>::iterator iterNext = iterCurrent;
		iterNext++;

		std::vector<EtVector3> tempvectorlist;

		EtVector3 vecTarget =  (*iterPrev).Position - (*iterCurrent).Position;	// 현재 노드에서 이전노드 방향벡터
		float fLength = EtVec3Length( &vecTarget );

		float fPointLength = 0.0f;
		if( fLength > 800.0f )		// 8m가 넘으면 앞뒤 4m를 5개 포인트로 쪼갬
			fPointLength = 400.0f;
		else						// 8m가 넘지 않는다면 중간지점 까지만 쪼갬
			fPointLength = fLength / 2.0f;

		EtVector3 vecNormal;
		EtVec3Normalize( &vecNormal, &vecTarget );
		tempvectorlist.push_back( (*iterCurrent).Position + ( vecNormal * fPointLength ) );
		tempvectorlist.push_back( (*iterCurrent).Position + ( vecNormal * ( fPointLength / 2.0f ) ) );

		tempvectorlist.push_back( (*iterCurrent).Position );

		vecTarget =  (*iterNext).Position - (*iterCurrent).Position;	// 현재 노드에서 다음노드 방향벡터
		fLength = EtVec3Length( &vecTarget );

		if( fLength > 800.0f )		// 8m가 넘으면 앞뒤 4m를 5개 포인트로 쪼갬
			fPointLength = 400.0f;
		else						// 8m가 넘지 않는다면 중간지점 까지만 쪼갬
			fPointLength = fLength / 2.0f;

		EtVec3Normalize( &vecNormal, &vecTarget );
		tempvectorlist.push_back( (*iterCurrent).Position + ( vecNormal * ( fPointLength / 2.0f ) ) );
		tempvectorlist.push_back( (*iterCurrent).Position + ( vecNormal * fPointLength ) );

		if( tempvectorlist.size() == 0 )
			continue;

		// 곡선 보간
		WAYPOINT_LIST interpolationlist;
		int nDivide = 40;
		for( int j=0; j<nDivide; j++ )
		{
			WAYPOINT waypoint;
			float fTime = static_cast<float>(j) / static_cast<float>(nDivide);
			waypoint.Position = GetInterpolatedSplinePoint( tempvectorlist, fTime );
			waypointlist.push_back( waypoint );
		}
	}

	std::list<WAYPOINT>::iterator iterEnd = fur_waypoint_list.end();
	iterEnd--;
	waypointlist.push_back( *iterEnd );	// 마지막 노드


	// 곡선 생성했으면 곡선의 정점이 네비게이션 메쉬안에 존재하는지 검사하여
	// 존재하지 않는 정점은 어떻게 해야할까??????


	m_WayPointList = waypointlist;
	m_WayPointId = m_WayPointList.begin();
}
#endif // PRE_MOD_NAVIGATION_PATH

void MAWalkMovement::MoveToWorld( EtVector2 &vVec )
{
	m_vMovement += vVec;
}

void MAWalkMovement::ResetMove()
{
	m_hMoveTarget.Identity();
	m_vMovePos = EtVector3( 0.f, 0.f, 0.f );
	m_vMagnetDir = EtVector2( 0.f, 0.f );
	m_fMagnetLength = 0.f;
	m_bEnableNaviMode = false;
}

void MAWalkMovement::Look( EtVector2 &vVec, bool bForceRotate )
{
	if( bForceRotate ) {
		if( EtVec2LengthSq( &vVec ) > 0.f ) {
			m_pMatExWorld->m_vZAxis = EtVector3( vVec.x, 0.f, vVec.y );
			EtVec3Normalize( &m_pMatExWorld->m_vZAxis, &m_pMatExWorld->m_vZAxis );
			m_pMatExWorld->MakeUpCartesianByZAxis();
		}
		m_vTargetLookVec = EtVector2( 0.f, 0.f );
	}
	else {
		m_vTargetLookVec = vVec;
	}
}

EtVector3 *MAWalkMovement::GetLookDir()
{
	return &m_pMatExWorld->m_vZAxis;
}

void MAWalkMovement::LookTarget( DnActorHandle &hActor )
{
	m_hLookTarget = hActor;

	if ( m_hLookTarget )
	{
		OnBeginLook();
	}
	else
	{
		OnEndLook();
	}
}

void MAWalkMovement::ResetLook()
{
	m_hLookTarget.Identity();
	m_vTargetLookVec = EtVector2( 0.f, 0.f );
}

DnActorHandle MAWalkMovement::GetLookTarget()
{
	return m_hLookTarget;
}

void MAWalkMovement::SetMagnetDir( EtVector2 &vPos )
{
	m_vMagnetDir = vPos;
}

void MAWalkMovement::SetMagnetLength( float fLength )
{
	m_fMagnetLength = fLength;
}

EtVector3 *MAWalkMovement::GetVelocity()
{
	return &m_vVelocity;
}

EtVector3 *MAWalkMovement::GetResistance()
{
	return &m_vVelocityResist;
}

EtVector3 *MAWalkMovement::GetLastVelocity()
{
	return &m_vLastVelocity;
}

EtVector3 *MAWalkMovement::GetVelocityValue()
{
	return &m_vLastVelocityValue;
}

void MAWalkMovement::SetMagnetPosition( EtVector3 &vPos )
{
	EtVector2 vMagnet;

	float fHeightDiff = vPos.y - m_pMatExWorld->m_vPosition.y;
	vMagnet.x = vPos.x - m_pMatExWorld->m_vPosition.x;
	vMagnet.y = vPos.z - m_pMatExWorld->m_vPosition.z;
	float fAberraionLength = EtVec2Length( &vMagnet );

	if( ( fAberraionLength > 1.f && fAberraionLength < 200.f ) && fHeightDiff < 200.f ) {
		SetMagnetLength( fAberraionLength );
		EtVec2Normalize( &vMagnet, &vMagnet );
		SetMagnetDir( vMagnet );
	}
	else {
		m_pActor->SetPosition( vPos );
	}
}

void MAWalkMovement::SetMoveYDistancePerSec( float fMoveYDistancePerSec, float fWholeMoveYDistance, bool bMaintainYDistanceOnArriveDestPosition )
{
	if( (0.0f < m_fMoveYDistancePerSec && 0.0f < fMoveYDistancePerSec) ||
		(0.0f > m_fMoveYDistancePerSec && 0.0f > fMoveYDistancePerSec) )
		return;
	
	// 이 값은 음수가 될 수도 있지만,
	m_fMoveYDistancePerSec = fMoveYDistancePerSec;

	// 이 값은 거리를 의미하기 때문에 무조건 양수이다.
	_ASSERT( 0.0f < fWholeMoveYDistance );
	m_fLeftMoveYDistance = fWholeMoveYDistance;

	m_bAppliedYDistance = true;

	// 목표로 되어있는 거리만큼 이동했을 때 Y 값을 더했다는 플래그 m_bAppliedYDistance 를 
	// 유지할 것인가. 
	// 이 플래그로 나중에 WalkMovementNavi 에서 바닥과의 높이를 맞추는 루틴을 할지 말지
	// 결정하기 때문에 시그널이 끝나고 유지하지 않으면 MoveY 시그널이 끝나고 곧바로 다시 바닥에 붙게 된다.
	m_bMaintainYDistanceOnArriveDestPosition = bMaintainYDistanceOnArriveDestPosition;
}

void MAWalkMovement::ResetMoveYDistance( void )
{
	m_fMoveYDistancePerSec = 0.0f;
	m_fLeftMoveYDistance = 0.0f;
	m_bAppliedYDistance = false;
}

#ifndef _GAMESERVER

void MAWalkMovement::DebugRenderAttr()
{
#if defined(_DEBUG) || defined(_RDEBUG)
	if( !m_bDebugRenderAttr  ) {
		return;
	}

	static EtTextureHandle hTexture, hTextureDiagonal;
	if(!hTexture) {
		hTexture = EternityEngine::LoadTexture(CEtResourceMng::GetInstance().GetFullName("Attr.dds").c_str() );
		hTextureDiagonal = EternityEngine::LoadTexture(CEtResourceMng::GetInstance().GetFullName("AttrDiagonal.dds").c_str() );
	}
	for( int y = -5; y <= 5; y++)
	for( int x = -5; x <= 5; x++)
	{
		float fX = ((int)(m_pMatExWorld->m_vPosition.x / 50.f)+x)*50.f+25.f;
		float fZ = ((int)(m_pMatExWorld->m_vPosition.z / 50.f)+y)*50.f+25.f;

		char cAttr = INSTANCE(CDnWorld).GetAttribute( EtVector3(fX,0,fZ) );
		if( cAttr & 0x3 ) {
			char cHiAttr = ( cAttr & 0xf0 ) >> 4;
			EtDecalHandle hDecal = (new CEtDecal)->GetMySmartPtr();
			float fAngle = 0.f;
			if( cHiAttr == 0 ) {
				hDecal->Initialize(hTexture, fX, fZ, 25.f, 0.f, 0.f, 0.f, EtColor(1,1,1,1) );				
			}
			else {
				if( cHiAttr == 1 ) fAngle = 0.f;
				if( cHiAttr == 2 ) fAngle = 90.f;
				if( cHiAttr == 4 ) fAngle = 180.f;
				if( cHiAttr == 8 ) fAngle = 270.f;
				hDecal->Initialize( hTextureDiagonal, fX, fZ, 25.f, 0.f, 0.f, fAngle, EtColor(1,1,1,1) );
			}
		}
	}
#endif
}

#endif

