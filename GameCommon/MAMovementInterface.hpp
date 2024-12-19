
#pragma once

#include "MAMovementBase.h"

class MAMovementInterface
{
public:

	MAMovementInterface()
	:m_pMovement(NULL)
	{
	}
	virtual ~MAMovementInterface()
	{
		SAFE_DELETE( m_pMovement );
	}

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new MAMovementBase();
		return pMovement;
	}

	void SetMovement( MAMovementBase* pMovement ){ m_pMovement = pMovement; }
	MAMovementBase* GetMovement(){ return m_pMovement; }

	//###################################################################################################
	virtual void OnDrop( float fCurVelocity ) {}
	virtual void OnStop( EtVector3 &vPosition ) {}
	virtual void OnBeginNaviMode() {}
	virtual void OnClash( EtVector3 &vPosition, OnClashFloorCheckType bFloorClash ) {}
	virtual void OnBumpWall(){}
	virtual void OnFall( float fCurVelocity ) {}
	virtual void OnBeginLook() { }
	virtual void OnEndLook() { }
	virtual void OnMoveNavi( EtVector3 &vPosition ) {}
#ifdef PRE_MOD_NAVIGATION_PATH
	virtual void OnAutoMoving( EtVector3& vPosition, EtVector3& vCurrentPosition ) {}
#endif // PRE_MOD_NAVIGATION_PATH
	//###################################################################################################

	void PushAndCollisionCheck( EtVector3 &vPush ) { m_pMovement->PushAndCollisionCheck( vPush ); }

	void SetMoveVectorX( EtVector3 &vVec ){ m_pMovement->SetMoveVectorX( vVec ); }
	void SetMoveVectorZ( EtVector3 &vVec ){ m_pMovement->SetMoveVectorZ( vVec ); }
	EtVector3 *GetMoveVectorX(){ return m_pMovement->GetMoveVectorX(); }
	EtVector3 *GetMoveVectorZ(){ return m_pMovement->GetMoveVectorZ(); }

	void MoveZ( float fSpeed ){ m_pMovement->MoveZ( fSpeed ); }
	void MoveX( float fSpeed ){ m_pMovement->MoveX( fSpeed ); }
	void MovePos( EtVector3 &vPos, bool bRefreshZVec = true ){ m_pMovement->MovePos( vPos, bRefreshZVec ); }
	void MoveTarget( DnActorHandle &hActor, float fMinDistance ){ m_pMovement->MoveTarget( hActor, fMinDistance ); }
	void MoveTargetNavi( DnActorHandle &hActor, float fMinDistance, const char* szActionName ){ m_pMovement->MoveTargetNavi( hActor, fMinDistance, szActionName ); }
	void MoveTargetNavi( EtVector3& vTargetPos, float fMinDistance, const char* szActionName ){ m_pMovement->MoveTargetNavi( vTargetPos, fMinDistance, szActionName ); }
	int  GetNaviType(){ return m_pMovement->GetNaviType(); }
	void MoveToWorld( EtVector2 &vVec ){ m_pMovement->MoveToWorld(vVec); }
	EtVector3 *GetMovePos() { return m_pMovement->GetMovePos(); }
	void ResetMove(){ m_pMovement->ResetMove(); }
	DnActorHandle GetMoveTarget(){ return m_pMovement->GetMoveTarget(); }
	DnActorHandle	GetNaviTarget(){ return m_pMovement->GetNaviTarget(); }

	void SetVelocity( EtVector3 &vVec ) { m_pMovement->SetVelocity(vVec); }
	void SetVelocityX( float fPower ) { m_pMovement->SetVelocityX(fPower); }
	void SetVelocityY( float fPower ) { m_pMovement->SetVelocityY(fPower); }
	void SetVelocityZ( float fPower ) { m_pMovement->SetVelocityZ(fPower); }

	EtVector3 *GetVelocity(){ return m_pMovement->GetVelocity(); }
	EtVector3 *GetResistance(){ return m_pMovement->GetResistance(); }
	EtVector3 *GetLastVelocity(){ return m_pMovement->GetLastVelocity(); }
	EtVector3 *GetVelocityValue(){ return m_pMovement->GetVelocityValue(); }

	void SetMagnetDir( EtVector2 &vPos ){ m_pMovement->SetMagnetDir(vPos); }
	void SetMagnetLength( float fLength ){ m_pMovement->SetMagnetLength(fLength); }
	bool IsNaviMode() { return m_pMovement->IsNaviMode(); }

	void SetMoveYDistancePerSec( float fMoveYDistancePerSec, float fWholeMoveYDistance, bool bMaintainYDistance ) { m_pMovement->SetMoveYDistancePerSec(fMoveYDistancePerSec, fWholeMoveYDistance, bMaintainYDistance); };
	void ResetMoveYDistance( void ) { m_pMovement->ResetMoveYDistance(); };

	void SetResistance( EtVector3 &vVec ) { m_pMovement->SetResistance(vVec); }
	void SetResistanceX( float fPower ) { m_pMovement->SetResistanceX(fPower); }
	void SetResistanceY( float fPower ) { m_pMovement->SetResistanceY(fPower); }
	void SetResistanceZ( float fPower ) { m_pMovement->SetResistanceZ(fPower); }

#ifdef PRE_ADD_GRAVITY_PROPERTY
	void SetAppliedYDistance( bool bAppliedYDistance ) { m_pMovement->SetAppliedYDistance( bAppliedYDistance ); }
#endif // PRE_ADD_GRAVITY_PROPERTY

	void Jump( float fPower, EtVector2 &vVec ){ m_pMovement->Jump( fPower, vVec); }
	void MoveJumpX( float fSpeed ){ m_pMovement->MoveJumpX(fSpeed); }
	void MoveJumpZ( float fSpeed ){ m_pMovement->MoveJumpZ(fSpeed); }
	void SetVelocityByMoveVector( EtVector3 &vVec ){ m_pMovement->SetVelocityByMoveVector(vVec); }
	void SetMagnetPosition( EtVector3 &vPos ){ m_pMovement->SetMagnetPosition(vPos); }

	bool IsFloorCollision() { return m_pMovement->IsFloorCollision(); }

	virtual void Look( EtVector2 &vVec, bool bForceRotate = true ){ m_pMovement->Look( vVec, bForceRotate ); }
	void LookTarget( DnActorHandle &hActor ){ m_pMovement->LookTarget( hActor ); }
	EtVector3 *GetLookDir(){ return m_pMovement->GetLookDir(); }
	EtVector2 *GetLookDirTarget(){ return m_pMovement->GetLookDirTarget(); }
	DnActorHandle GetLookTarget(){ return m_pMovement->GetLookTarget(); }
	void ResetLook(){ m_pMovement->ResetLook(); }

	void			ResetNaviMode() { m_pMovement->ResetNaviMode(); }
	float			GetNaviTargetMinDistance(){ return m_pMovement->GetNaviTargetMinDistance(); }
	const char*		GetNaviTargetActionName(){ return m_pMovement->GetNaviTargetActionName(); }

	size_t			GetWayPointSize(){ return m_pMovement->GetWayPointSize(); }

	void			SetJumpMovement( EtVector2& vVec ){ m_pMovement->SetJumpMovement(vVec); }

	void			EnableDebugDraw(bool bDebugRender ) { m_pMovement->EnableDebugDraw(bDebugRender); }

	bool			GetFloorForceVelocity(){ return m_pMovement->GetFloorForceVelocity(); }
	bool			GetLastFloorForceVelocity(){ return m_pMovement->GetLastFloorForceVelocity(); }

	MatrixEx*	GetMatEx(){ return m_pMovement->GetMatEx(); }
	bool			CheckDiagonalBlock( float fX, float fY ){ return m_pMovement->CheckDiagonalBlock(fX,fY); }
protected:

	MAMovementBase* m_pMovement;
};
