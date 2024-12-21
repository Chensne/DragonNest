#pragma once

#include "Timer.h"
#include "MAMovementBase.h"
#include "EtMatrixEx.h"

class CDnActorState;

class MAStaticMovement : public MAMovementBase{
public:
	MAStaticMovement();
	virtual ~MAStaticMovement();

protected:

	EtVector3 m_vIdentity;

public:

	virtual bool bIsStaticMovement(){ return true; }

	virtual EtVector3 *GetMoveVectorX() { return &m_vIdentity; }
	virtual EtVector3 *GetMoveVectorZ() { return &m_vIdentity; }
	virtual EtVector3 *GetMovePos() { return &m_vIdentity; }
	virtual EtVector3 *GetLookDir() { return &m_pMatExWorld->m_vZAxis; }

	virtual EtVector3 *GetVelocity() { return &m_vIdentity; }
	virtual EtVector3 *GetResistance() { return &m_vIdentity; }
	virtual EtVector3 *GetLastVelocity() { return &m_vIdentity; }
	virtual EtVector3 *GetVelocityValue() { return &m_vIdentity; }

	virtual void ForceLook( EtVector2 &vVec );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

};

class IBoostPoolMAStaticMovement : public MAStaticMovement, public TBoostMemoryPool<IBoostPoolMAStaticMovement>
{
public :
	IBoostPoolMAStaticMovement()
		:MAStaticMovement(){}
	virtual ~IBoostPoolMAStaticMovement(){}
};
