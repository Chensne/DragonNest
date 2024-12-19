#include "stdAfx.h"
#include "VelocityFunc.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

float CalcMovementToTargetTime( float &fCurSpeed, float fElapsedTime, float fTargetSpeed, float fAccel )
{
	float fFinalSpeed, fMovement, fDiffTime;

	fElapsedTime *= 1.f;

	fFinalSpeed = fCurSpeed + fElapsedTime * fAccel;
	if( ( fFinalSpeed > fTargetSpeed ) && ( fCurSpeed <= fTargetSpeed ) )
	{
		fDiffTime =  ( fTargetSpeed - fCurSpeed ) / ( fFinalSpeed - fCurSpeed ) * fElapsedTime;
		fMovement = fCurSpeed * fDiffTime + fAccel * fDiffTime * fDiffTime * 0.5f;
		fCurSpeed = fTargetSpeed;
	}
	else if( ( fFinalSpeed < fTargetSpeed ) && ( fCurSpeed >= fTargetSpeed ) )
	{
		fDiffTime =  ( fCurSpeed - fTargetSpeed ) / ( fCurSpeed - fFinalSpeed ) * fElapsedTime;
		fMovement = fCurSpeed * fDiffTime + fAccel * fDiffTime * fDiffTime * 0.5f;
		fCurSpeed = fTargetSpeed;
	}
	else
	{
		fMovement = fCurSpeed * fElapsedTime + fAccel * fElapsedTime * fElapsedTime * 0.5f;
		fCurSpeed = fFinalSpeed;
	}

	return fMovement * 100.f;
}

float CalcMovement( float &fCurSpeed, float fElapsedTime, float fMaxSpeed, float fMinSpeed, float fAccel )
{
	float fFinalSpeed, fMovement, fDiffTime;

	fElapsedTime *= 1.f;

	fFinalSpeed = fCurSpeed + fElapsedTime * fAccel;
	if( ( fFinalSpeed > fMaxSpeed ) && ( fCurSpeed <= fMaxSpeed ) )
	{
		fDiffTime =  ( fMaxSpeed - fCurSpeed ) / ( fFinalSpeed - fCurSpeed ) * fElapsedTime;
		fMovement = fCurSpeed * fDiffTime + fAccel * fDiffTime * fDiffTime * 0.5f;
		fMovement += fMaxSpeed * ( fElapsedTime - fDiffTime );
		fCurSpeed = fMaxSpeed;
	}
	else if( ( fFinalSpeed < fMinSpeed ) && ( fCurSpeed >= fMinSpeed ) )
	{
		fDiffTime =  ( fCurSpeed - fMinSpeed ) / ( fCurSpeed - fFinalSpeed ) * fElapsedTime;
		fMovement = fCurSpeed * fDiffTime + fAccel * fDiffTime * fDiffTime * 0.5f;
		fMovement += fMinSpeed * ( fElapsedTime - fDiffTime );
		fCurSpeed = fMinSpeed;
	}
	else
	{
		fMovement = fCurSpeed * fElapsedTime + fAccel * fElapsedTime * fElapsedTime * 0.5f;
		fCurSpeed = fFinalSpeed;
	}

	return fMovement * 100.f;
}

float CalcResistance( float fCurSpeed, float fElapsedTime, float fMaxSpeed, float fMinSpeed, float fElapsedDist )
{
	float fResist, fFinalSpeed, fDiffTime, fDiffDist;

	fResist = 2.0f * fElapsedDist / ( fElapsedTime * fElapsedTime ) - 2.0f * fCurSpeed / fElapsedTime;
	fFinalSpeed = fCurSpeed + fElapsedTime * fResist;
	if( ( fFinalSpeed > fMaxSpeed ) && ( fCurSpeed <= fMaxSpeed ) )
	{
		fDiffTime =  ( fMaxSpeed - fCurSpeed ) / ( fFinalSpeed - fCurSpeed ) * fElapsedTime;
		fDiffDist = fElapsedDist - fMaxSpeed * ( fElapsedTime - fDiffTime );
		fResist = 2.0f * fDiffDist / ( fDiffTime * fDiffTime ) - 2.0f * fCurSpeed / fDiffTime;
	}
	else if( ( fFinalSpeed < fMinSpeed ) && ( fCurSpeed >= fMinSpeed ) )
	{
		fDiffTime =  ( fCurSpeed - fMinSpeed ) / ( fCurSpeed - fFinalSpeed ) * fElapsedTime;
		fDiffDist = fElapsedDist - fMinSpeed * ( fElapsedTime - fDiffTime );
		fResist = 2.0f * fDiffDist / ( fDiffTime * fDiffTime ) - 2.0f * fCurSpeed / fDiffTime;
	}
	return fResist;
}

EtVector3 CalcHoming( DWORD dwTotalTime, DWORD dwElapsedTime, EtVector3 &vStartPos, EtVector3 &vTargetPos, EtVector3 &vViewVec )
{
	EtVector3 vResult;
	EtVector3 Accel = ( 2.f / ( (float)( dwTotalTime * dwTotalTime ) ) ) * ( vTargetPos - vStartPos - ( (float)dwTotalTime * vViewVec ) );
	vResult = vStartPos + ( (float)dwElapsedTime * vViewVec ) + ( ( 0.5f * ( (float)( dwElapsedTime * dwElapsedTime ) ) ) * Accel );
	return vResult;

}