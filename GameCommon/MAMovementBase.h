#pragma once

class CDnActor;
class MatrixEx;
class CDnActor;

enum OnClashFloorCheckType
{
	eCFCT_None,
	eCFCT_Floor,
	eCFCT_NoFloor,
};

#ifdef _GAMESERVER
class MAMovementBase : public CMultiElement {
#else
class MAMovementBase {
#endif
public:
	MAMovementBase() 
	{
		m_pActor = NULL;
		m_pMatExWorld = NULL;
	}
	virtual ~MAMovementBase() {}

	enum NaviType
	{
		eTarget = 0,
		ePosition,
		eMax,
	};

protected:
	CDnActor *m_pActor;
	MatrixEx *m_pMatExWorld;

protected:

	virtual void OnDrop( float fCurVelocity );
	virtual void OnStop( EtVector3 &vPosition );
	virtual void OnBeginNaviMode();
	virtual void OnClash( EtVector3 &vPosition, OnClashFloorCheckType bFloorClash );
	virtual void OnBumpWall();
	virtual void OnFall( float fCurVelocity );
	virtual void OnBeginLook();
	virtual void OnEndLook();
	virtual void OnMoveNavi( EtVector3 &vPosition );
#ifdef PRE_MOD_NAVIGATION_PATH
	virtual void OnAutoMoving( EtVector3& vPosition, EtVector3& vCurrentPosition );
#endif // PRE_MOD_NAVIGATION_PATH
	virtual void OnLook( EtVector2 &vVec, bool bForceRotate = true ) {}

public:

	virtual bool bIsStaticMovement(){ return false; }

	virtual bool Initialize( CDnActor *pActor, MatrixEx *pCross )
	{
		m_pActor = pActor;
		m_pMatExWorld = pCross;
		return true;
	}
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}
	virtual void PushAndCollisionCheck( EtVector3 &vPush ) {}

	virtual void SetMoveVectorX( EtVector3 &vVec ) {}
	virtual void SetMoveVectorZ( EtVector3 &vVec ) {}
	virtual EtVector3 *GetMoveVectorX() { return NULL; }
	virtual EtVector3 *GetMoveVectorZ() { return NULL; }

	virtual void MoveZ( float fSpeed ) {}
	virtual void MoveX( float fSpeed ) {}
	virtual void MovePos( EtVector3 &vPos, bool bRefreshZVec = true ) {}
	virtual void MoveTarget( DnActorHandle &hActor, float fMinDistance ) {}
	virtual void MoveTargetNavi( DnActorHandle &hActor, float fMinDistance, const char* szActionName ) {}
	virtual void MoveTargetNavi( EtVector3& vTargetPos, float fMinDistance, const char* szActionName ) {}
	virtual int  GetNaviType(){ return NaviType::eMax; }
#ifdef PRE_MOD_NAVIGATION_PATH
	virtual void AutoMoving( EtVector3& vTargetPos, float fMinDistance, const char* szActionName, bool bGeneratePath ) {}
#endif // PRE_MOD_NAVIGATION_PATH
	virtual void MoveToWorld( EtVector2 &vVec ) {}
	virtual EtVector3 *GetMovePos() { return NULL; }
	virtual void ResetMove() {}
	virtual DnActorHandle GetMoveTarget();
	virtual DnActorHandle GetNaviTarget();

	virtual void Look( EtVector2 &vVec, bool bForceRotate = true ) {}
	virtual void ForceLook( EtVector2 &vVec ) {}
	virtual void LookTarget( DnActorHandle &hActor ) {}
	virtual EtVector3 *GetLookDir() { return NULL; }
	virtual EtVector2 *GetLookDirTarget() { return NULL; }
	virtual DnActorHandle GetLookTarget();
	virtual void ResetLook() {}

	virtual void SetVelocity( EtVector3 &vVec ) {}
	virtual void SetVelocityX( float fPower ) {}
	virtual void SetVelocityY( float fPower ) {}
	virtual void SetVelocityZ( float fPower ) {}

	virtual void SetResistance( EtVector3 &vVec ) {}
	virtual void SetResistanceX( float fPower ) {}
	virtual void SetResistanceY( float fPower ) {}
	virtual void SetResistanceZ( float fPower ) {}

	virtual EtVector3 *GetVelocity() { return NULL; }
	virtual EtVector3 *GetResistance() { return NULL; }
	virtual EtVector3 *GetLastVelocity() { return NULL; }
	virtual EtVector3 *GetVelocityValue() { return NULL; }

#ifdef PRE_ADD_GRAVITY_PROPERTY
	virtual void SetAppliedYDistance( bool bAppliedYDistance ) {}
#endif // PRE_ADD_GRAVITY_PROPERTY

	virtual void SetMagnetDir( EtVector2 &vPos ) {}
	virtual void SetMagnetLength( float fLength ) {}

	virtual void SetMoveYDistancePerSec( float fMoveYDistancePerSec, float fWholeMoveYDistance, bool bMaintainYDistance ) {};
	virtual void ResetMoveYDistance( void ) {};

	virtual bool	IsNaviMode() { return false; }
	virtual void			ResetNaviMode() { }
	virtual float			GetNaviTargetMinDistance(){ return 0.f; }
	virtual const char*		GetNaviTargetActionName(){ return NULL; }
	virtual size_t	GetWayPointSize(){ return 0; }

	virtual void Jump( float fPower, EtVector2 &vVec ){}
	virtual void MoveJumpX( float fSpeed ){}
	virtual void MoveJumpZ( float fSpeed ){}
	virtual void SetVelocityByMoveVector( EtVector3 &vVec ){}
	virtual bool IsFloorCollision() { return false; }
	virtual void SetMagnetPosition( EtVector3 &vPos ){}
	virtual EtVector2* GetJumpMovement() { return NULL; }
	virtual void SetJumpMovement( EtVector2& vVec ){ }
	virtual void EnableDebugDraw(bool bDebugRender ) {}
	
	virtual void			ValidateCurCell(){}
	virtual bool			GetFloorForceVelocity(){ return false; }
	virtual bool			GetLastFloorForceVelocity(){ return false; }
	virtual bool			CheckDiagonalBlock( float fX, float fY ){ return false; }

	MatrixEx*			GetMatEx(){ return m_pMatExWorld; }

};