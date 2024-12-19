#pragma once

float CalcMovementToTargetTime( float &fCurSpeed, float fElapsedTime, float fTargetTime, float fAccel );
float CalcMovement( float &fCurSpeed, float fElapsedTime, float fMaxSpeed, float fMinSpeed, float fAccel );
float CalcResistance( float fCurSpeed, float fElapsedTime, float fMaxSpeed, float fMinSpeed, float fElapsedDist );
EtVector3 CalcHoming( DWORD dwTotalTime, DWORD dwElapsedTime, EtVector3 &vStartPos, EtVector3 &vTargetPos, EtVector3 &vViewVec );
