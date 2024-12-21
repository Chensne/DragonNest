#include "StdAfx.h"
#include "DnActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DnActorHandle MAMovementBase::GetLookTarget()
{
	return CDnActor::Identity();
}

DnActorHandle MAMovementBase::GetMoveTarget()
{
	return CDnActor::Identity();
}

DnActorHandle MAMovementBase::GetNaviTarget()
{
	return CDnActor::Identity();
}

void MAMovementBase::OnDrop( float fCurVelocity ) 
{ 
	m_pActor->OnDrop(fCurVelocity); 
}

void MAMovementBase::OnStop( EtVector3 &vPosition ) 
{ 
	m_pActor->OnStop( vPosition ); 
}

void MAMovementBase::OnBeginNaviMode() 
{ 
	m_pActor->OnBeginNaviMode(); 
}

void MAMovementBase::OnClash( EtVector3 &vPosition, OnClashFloorCheckType bFloorClash  ) 
{ 
	m_pActor->OnClash( vPosition, bFloorClash ); 
}

void MAMovementBase::OnBumpWall()
{ 
	m_pActor->OnBumpWall(); 
}

void MAMovementBase::OnFall( float fCurVelocity ) 
{
	m_pActor->OnFall( fCurVelocity ); 
}

void MAMovementBase::OnBeginLook() 
{ 
	m_pActor->OnBeginLook(); 
}

void MAMovementBase::OnEndLook() 
{ 
	m_pActor->OnEndLook(); 
}

void MAMovementBase::OnMoveNavi( EtVector3 &vPosition ) 
{ 
	m_pActor->OnMoveNavi( vPosition ); 
}

#ifdef PRE_MOD_NAVIGATION_PATH
void MAMovementBase::OnAutoMoving( EtVector3& vPosition, EtVector3& vCurrentPosition ) 
{ 
	m_pActor->OnAutoMoving( vPosition, vCurrentPosition ); 
}
#endif // PRE_MOD_NAVIGATION_PATH
