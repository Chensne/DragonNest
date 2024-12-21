#include "Stdafx.h"
#include "EtComputeDist.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

// 한 직선과 한 점을 가지고, CCW인지 CW인지를 판단한다. 음수면 CCW. 같은 선상에 있으면 0리턴.
float CCW( EtVector2 &Line1, EtVector2 &Line2, EtVector2 &Point )
{
	return ((Line1.x - Point.x) * (Line2.y - Point.y)) - ((Line1.y - Point.y) * (Line2.x - Point.x));
}

// 교차할때만 true, 그 외 나머지 경우 false
bool IntrLine2DToLine2D( EtVector2 &Line1, EtVector2 &Line2, EtVector2 &Line3, EtVector2 &Line4 )
{
	if( (CCW(Line1, Line2, Line3) * CCW(Line1, Line2, Line4)) < 0 && (CCW(Line3, Line4, Line1) * CCW(Line3, Line4, Line2)) < 0 )
		return true;
	return false;
}

// 와일드매직 코드를 보면 DistSegmentToSegment에 대응되는 2D용 선분 거리 검사코드가 있긴 한데,
// 직관적인 코드로 짜보려고, 직접 만들어본다. PressDir의 경우 필요없으면 사용하지 않아도 무방함.
float DistLine2DToLine2D( EtVector2 &Line1, EtVector2 &Line2, EtVector2 &Line3, EtVector2 &Line4, EtVector2 &PressDir )
{
	// 두 선분이 교차하면 0.0f 리턴
	if( IntrLine2DToLine2D(Line1, Line2, Line3, Line4) )
		return 0.0f;
	else
	{
		// 두 선분 각각 선분과 양 끝점의 거리를 재서 가장 짧은 것을 리턴.
		EtVector2 vDir1, vDir2, vDir3, vDir4;
		float fDist1 = DistPointToLine2D( Line3, Line1, Line2, vDir1 );
		float fDist2 = DistPointToLine2D( Line4, Line1, Line2, vDir2 );
		// 거리가 짧은 것을 기억.
		if( fDist1 > fDist2 ) {
			vDir1 = vDir2;
			fDist1 = fDist2;
		}

		// 타겟선분에서 더 짧은게 나온다면 PressDir을 반대로 해준다.
		float fDist3 = DistPointToLine2D( Line1, Line3, Line4, vDir3 );
		float fDist4 = DistPointToLine2D( Line2, Line3, Line4, vDir4 );
		if( fDist3 > fDist4 ) {
			vDir3 = vDir4;
			fDist3 = fDist4;
		}
		vDir3 *= -1.0f;

		// 거리가 가장 짧은 것을 리턴.
		if( fDist1 > fDist3 ) {
			vDir1 = vDir3;
			fDist1 = fDist3;
		}
		PressDir = vDir1;
		return fDist1;
	}
}

float DistPointToLine2D( EtVector2 &Point, EtVector2 &Line1, EtVector2 &Line2, EtVector2 &PressDir )
{
	EtVector2 vDiff;
	EtVector2 vOrig = Line1;
	EtVector2 vDir = Line2 - Line1;
	float fDot;

	vDiff = Point - vOrig;
	fDot = EtVec2Dot( &vDir, &vDiff );

	if(fDot <= 0.0f)
	{
		fDot = 0.0f;
	}
	else
	{
		float fLengthSq;

		fLengthSq = EtVec2LengthSq( &vDir );
		if( fDot >= fLengthSq )
		{
			fDot = 1.0f;
			vDiff -= vDir;
		}
		else
		{
			fDot /= fLengthSq;
			vDiff -= fDot * vDir;
		}
	}
	EtVec2Normalize( &PressDir, &vDiff );
	return EtVec2Length( &vDiff );
}

float DistPointToSegment( EtVector3 &Point, SSegment &Segment, float &fSegParam )
{
	EtVector3 Diff;
		
	Diff = Point - Segment.vOrigin;
	fSegParam = EtVec3Dot( &Segment.vDirection, &Diff );

	if(fSegParam <= 0.0f)
	{
		fSegParam = 0.0f;
	}
	else
	{
		float fLengthSq;

		fLengthSq = EtVec3LengthSq( &Segment.vDirection );
		if( fSegParam >= fLengthSq )
		{
			fSegParam = 1.0f;
			Diff -= Segment.vDirection;
		}
		else
		{
			fSegParam /= fLengthSq;
			Diff -= fSegParam * Segment.vDirection;
		}
	}

	return EtVec3LengthSq( &Diff );
}

float DistPointToOBB( EtVector3 &Point, SCollisionBox &Box, float &fBoxParam0, float &fBoxParam1, float &fBoxParam2 )
{
	float fSqrDistance = 0.0f;

	// Compute the squared distance in the box's coordinate system.
	EtVector3 Diff = Point - Box.vCenter, Closest;
	for (int i = 0; i < 3; i++)
	{
		float fDelta;
		Closest[ i ] = EtVec3Dot( &Diff, Box.vAxis + i );
		if(Closest[ i ] < -Box.fExtent[ i ])
		{
			fDelta = Closest[ i ] + Box.fExtent[ i ];
			fSqrDistance += fDelta * fDelta;
			Closest[ i ] = -Box.fExtent[ i ];
		}
		else if(Closest[ i ] > Box.fExtent[ i ])
		{
			fDelta = Closest[ i ] - Box.fExtent[ i ];
			fSqrDistance += fDelta * fDelta;
			Closest[ i ] = Box.fExtent[ i ];
		}
	}

	fBoxParam0 = Closest[ 0 ];
	fBoxParam1 = Closest[ 1 ];
	fBoxParam2 = Closest[ 2 ];

	return fSqrDistance;
}

float DistSegmentToSegment( SSegment &Segment1, SSegment &Segment2, float &fSeg0Param, float &fSeg1Param )
{
	const float fTolerance = 1e-06f;
	float fTmp;
	float fA, fB, fC, fD, fE, fF;
	EtVector3 Diff;

	Diff = Segment1.vOrigin - Segment2.vOrigin;
	fA = EtVec3Dot( &Segment1.vDirection, &Segment1.vDirection );
	fB = -EtVec3Dot( &Segment1.vDirection, &Segment2.vDirection );
	fC = EtVec3Dot( &Segment2.vDirection, &Segment2.vDirection );
	fD = EtVec3Dot( &Segment1.vDirection, &Diff );
	fE = -EtVec3Dot( &Segment2.vDirection, &Diff );
	fF = EtVec3Dot( &Diff, &Diff );

	// fA*fC-fB*fB = |Cross(Dir0, Dir1)|^2 >= 0
	float fDet;
	EtVector3 Cross;

	fDet = fabs( fA * fC - fB * fB );
	EtVec3Cross( &Cross, &Segment1.vDirection, &Segment2.vDirection );
	if( EtVec3LengthSq( &Cross ) > fTolerance )
	{
		// line segments are not parallel
		fSeg0Param = fB * fE - fC * fD;
		fSeg1Param = fB * fD - fA * fE;

		if( fSeg0Param >= 0 )
		{
			if( fSeg0Param <= fDet )
			{
				if( fSeg1Param >= 0 )
				{
					if( fSeg1Param <= fDet )  // region 0 (interior)
					{
						// minimum at two interior points of 3D lines
						float fInvDet;

						fInvDet = 1.0f / fDet;
						fSeg0Param *= fInvDet;
						fSeg1Param *= fInvDet;
						return fabs( fSeg0Param * ( fA * fSeg0Param + fB * fSeg1Param + 2 * fD ) + 
							fSeg1Param * (fB * fSeg0Param + fC * fSeg1Param + 2 * fE ) + fF);
					}
					else  // region 3 (side)
					{
						fSeg1Param = 1;
						fTmp = fB + fD;
						if( fTmp >= 0 )
						{
							fSeg0Param = 0;
							return fabs( fC + 2 * fE + fF );
						}
						else if( -fTmp >= fA )
						{
							fSeg0Param = 1;
							return fabs( fA + fC + fF + 2 * ( fE + fTmp ) );
						}
						else
						{
							fSeg0Param = -fTmp / fA;
							return fabs( fTmp * fSeg0Param + fC + 2 * fE + fF );
						}
					}
				}
				else  // region 7 (side)
				{
					fSeg1Param = 0;
					if( fD >= 0 )
					{
						fSeg0Param = 0;
						return fabs( fF );
					}
					else if( -fD >= fA )
					{
						fSeg0Param = 1;
						return fabs( fA + 2 * fD + fF );
					}
					else
					{
						fSeg0Param = -fD / fA;
						return fabs( fD * fSeg0Param + fF );
					}
				}
			}
			else
			{
				if( fSeg1Param >= 0 )
				{
					if( fSeg1Param <= fDet )  // region 1 (side)
					{
						fSeg0Param = 1;
						fTmp = fB + fE;
						if( fTmp >= 0 )
						{
							fSeg1Param = 0;
							return fabs( fA + 2 * fD + fF );
						}
						else if( -fTmp >= fC )
						{
							fSeg1Param = 1;
							return fabs( fA + fC + fF + 2 * ( fD + fTmp ) );
						}
						else
						{
							fSeg1Param = -fTmp / fC;
							return fabs( fTmp * fSeg1Param + fA + 2 * fD + fF );
						}
					}
					else  // region 2 (corner)
					{
						fTmp = fB + fD;
						if( -fTmp <= fA )
						{
							fSeg1Param = 1;
							if( fTmp >= 0 )
							{
								fSeg0Param = 0;
								return fabs( fC + 2 * fE + fF );
							}
							else
							{
								fSeg0Param = -fTmp / fA;
								return fabs( fTmp * fSeg0Param + fC + 2 * fE + fF );
							}
						}
						else
						{
							fSeg0Param = 1;
							fTmp = fB + fE;
							if( -fTmp >= fC )
							{
								fSeg1Param = 1;
								return fabs( fA + fC + fF + 2 * ( fD + fTmp ) );
							}
							else if( fTmp > 0 )
							{
								fSeg1Param = 0;
								return fabs( fA + 2 * fD + fF );
							}
							else
							{
								fSeg1Param = -fTmp / fC;
								return fabs( fTmp * fSeg1Param + fA + 2 * fD + fF );
							}
						}
					}
				}
				else  // region 8 (corner)
				{
					if( -fD < fA )
					{
						fSeg1Param = 0;
						if( fD >= 0 )
						{
							fSeg0Param = 0;
							return fabs( fF );
						}
						else
						{
							fSeg0Param = -fD / fA;
							return fabs( fD * fSeg0Param + fF );
						}
					}
					else
					{
						fSeg0Param = 1;
						fTmp = fB + fE;
						if( fTmp >= 0 )
						{
							fSeg1Param = 0;
							return fabs( fA + 2 * fD + fF );
						}
						else if(-fTmp >= fC)
						{
							fSeg1Param = 1;
							return fabs( fA + fC + fF + 2 * ( fD + fTmp ) );
						}
						else
						{
							fSeg1Param = -fTmp / fC;
							return fabs( fTmp * fSeg1Param + fA + 2 * fD + fF );
						}
					}
				}
			}
		}
		else 
		{
			if( fSeg1Param >= 0 )
			{
				if( fSeg1Param <= fDet )  // region 5 (side)
				{
					fSeg0Param = 0;
					if( fE >= 0 )
					{
						fSeg1Param = 0;
						return fabs( fF );
					}
					else if( -fE >= fC )
					{
						fSeg1Param = 1;
						return fabs( fC + 2 * fE + fF );
					}
					else
					{
						fSeg1Param = -fE / fC;
						return fabs( fE * fSeg1Param + fF );
					}
				}
				else  // region 4 (corner)
				{
					fTmp = fB + fD;
					if( fTmp < 0 )
					{
						fSeg1Param = 1;
						if( -fTmp >= fA )
						{
							fSeg0Param = 1;
							return fabs( fA + fC + fF + 2 * ( fE + fTmp ) );
						}
						else
						{
							fSeg0Param = -fTmp / fA;
							return fabs( fTmp * fSeg0Param + fC + 2 * fE + fF );
						}
					}
					else
					{
						fSeg0Param = 0;
						fTmp = fC + fE;
						if( fTmp <= 0 )
						{
							fSeg1Param = 1;
							return fabs( fC + 2 * fE + fF );
						}
						else if( fE >= 0 )
						{
							fSeg1Param = 0;
							return fabs( fF );
						}
						else
						{
							fSeg1Param = -fE / fC;
							return fabs( fE * fSeg1Param + fF );
						}
					}
				}
			}
			else   // region 6 (corner)
			{
				if( fD < 0 )
				{
					fSeg1Param = 0;
					if( -fD >= fA )
					{
						fSeg0Param = 1;
						return fabs( fA + 2 * fD + fF );
					}
					else
					{
						fSeg0Param = -fD / fA;
						return fabs( fD * fSeg0Param + fF );
					}
				}
				else
				{
					fSeg0Param = 0;
					if( fE >= 0 )
					{
						fSeg1Param = 0;
						return fabs( fF );
					}
					else if( -fE >= fC )
					{
						fSeg1Param = 1;
						return fabs( fC + 2 * fE + fF );
					}
					else
					{
						fSeg1Param = -fE / fC;
						return fabs( fE * fSeg1Param + fF );
					}
				}
			}
		}
	}
	else
	{
		// line segments are parallel
		if( fB > 0 )
		{
			// direction vectors form an obtuse angle
			if( fD >= 0 )
			{
				fSeg0Param = 0;
				fSeg1Param = 0;
				return fabs( fF );
			}
			else if( -fD <= fA )
			{
				fSeg0Param = -fD / fA;
				fSeg1Param = 0;
				return fabs( fD * fSeg0Param + fF );
			}
			else
			{
				fSeg0Param = 1;
				fTmp = fA + fD;
				if( -fTmp >= fB )
				{
					fSeg1Param = 1;
					return fabs( fA + fC + fF + 2 * ( fB + fD + fE ) );
				}
				else
				{
					fSeg1Param = -fTmp / fB;
					return fabs( fA + 2 * fD + fF + fSeg1Param * ( fC * fSeg1Param + 2 * ( fB + fE ) ) );
				}
			}
		}
		else
		{
			// direction vectors form an acute angle
			if( -fD >= fA )
			{
				fSeg0Param = 1;
				fSeg1Param = 0;
				return fabs( fA + 2 * fD + fF );
			}
			else if(fD <= 0)
			{
				fSeg0Param = -fD / fA;
				fSeg1Param = 0;
				return fabs( fD * fSeg0Param + fF );
			}
			else
			{
				fSeg0Param = 0;
				if( fD >= -fB )
				{
					fSeg1Param = 1;
					return fabs( fC + 2 * fE + fF );
				}
				else
				{
					fSeg1Param = -fD / fB;
					return fabs( fF + fSeg1Param * ( 2 * fE + fC * fSeg1Param ) );
				}
			}
		}
	}
}

float DistSegToBox( SSegment &Segment, SCollisionBox &Box, float &fSegParam, 
				   float &fBoxParam0, float &fBoxParam1, float &fBoxParam2 )
{
	// compute coordinates of line in box coordinate system
	EtVector3 Diff = Segment.vOrigin - Box.vCenter;
	EtVector3 DiffDotAxis, Direction;

	DiffDotAxis.x = EtVec3Dot( &Diff, Box.vAxis );
	DiffDotAxis.y = EtVec3Dot( &Diff, Box.vAxis + 1 );
	DiffDotAxis.z = EtVec3Dot( &Diff, Box.vAxis + 2 );

	Direction.x = EtVec3Dot( &Segment.vDirection, Box.vAxis );
	Direction.y = EtVec3Dot( &Segment.vDirection, Box.vAxis + 1 );
	Direction.z = EtVec3Dot( &Segment.vDirection, Box.vAxis + 2 );

	// Apply reflections so that direction vector has nonnegative components.
	int i;
	bool bReflect[ 3 ];
	for ( i = 0; i < 3; i++ )
	{
		if( Direction[ i ] < 0.0f )
		{
			DiffDotAxis[ i ] = -DiffDotAxis[ i ];
			Direction[ i ] = -Direction[ i ];
			bReflect[ i ] = true;
		}
		else
		{
			bReflect[ i ] = false;
		}
	}

	float fSqrDistance = 0.0f;
	fSegParam = 0.0f;  // parameter for closest point on line

	if( Direction.x > 0.0f )
	{
		if( Direction.y > 0.0f )
		{
			if( Direction.z > 0.0f )  // (+,+,+)
			{
				CaseNoZeros( Box, DiffDotAxis, Direction, fSegParam, fSqrDistance );
			}
			else  // (+,+,0)
			{
				Case0( Box, 0, 1, 2, DiffDotAxis, Direction, fSegParam, fSqrDistance );
			}
		}
		else
		{
			if( Direction.z > 0.0f )  // (+,0,+)
			{
				Case0( Box, 0, 2, 1, DiffDotAxis, Direction, fSegParam, fSqrDistance );
			}
			else  // (+,0,0)
			{
				Case00( Box, 0, 1, 2, DiffDotAxis, Direction, fSegParam, fSqrDistance );
			}
		}
	}
	else
	{
		if( Direction.y > 0.0f )
		{
			if( Direction.z > 0.0f )  // (0,+,+)
			{
				Case0( Box, 1, 2, 0, DiffDotAxis, Direction, fSegParam, fSqrDistance );
			}
			else  // (0,+,0)
			{
				Case00( Box, 1, 0, 2, DiffDotAxis, Direction, fSegParam, fSqrDistance );
			}
		}
		else
		{
			if( Direction.z > 0.0f )  // (0,0,+)
			{
				Case00( Box, 2, 0, 1, DiffDotAxis, Direction, fSegParam, fSqrDistance );
			}
			else  // (0,0,0)
			{
				Case000( Box, DiffDotAxis, fSqrDistance );
			}
		}
	}

	if( fSegParam >= 0.0f )
	{
		if( fSegParam <= 1.0f )
		{
			for ( i = 0; i < 3; i++ )
			{
				// undo the reflections applied previously
				if( bReflect[ i ] )
				{
					DiffDotAxis[ i ] = -DiffDotAxis[ i ];
				}
			}
			fBoxParam0 = DiffDotAxis[ 0 ];
			fBoxParam1 = DiffDotAxis[ 1 ];
			fBoxParam2 = DiffDotAxis[ 2 ];
		}
		else
		{
			fSegParam = 1.0f;
			fSqrDistance = DistPointToOBB( Segment.vOrigin + Segment.vDirection, Box, fBoxParam0, fBoxParam1, fBoxParam2 );
		}
	}
	else
	{
		fSegParam = 0.0f;
		fSqrDistance = DistPointToOBB( Segment.vOrigin, Box, fBoxParam0, fBoxParam1, fBoxParam2 );
	}

	return fSqrDistance;
}

void Face( SCollisionBox &Box, int i0, int i1, int i2, EtVector3 &Point, EtVector3 &Direction, EtVector3 &PmE, 
		  float &fLParam, float &fSqrDistance )
{
	EtVector3 ExtentPoint;
	float fLSqr, fInv, fTmp, fParam, fT, fDelta;

	ExtentPoint[ i1 ] = Point[ i1 ] + Box.fExtent[ i1 ];
	ExtentPoint[ i2 ] = Point[ i2 ] + Box.fExtent[ i2 ];
	if( Direction[ i0 ] * ExtentPoint[ i1 ] >= Direction[ i1 ] * PmE[ i0 ] )
	{
		if( Direction[ i0 ] * ExtentPoint[ i2 ] >= Direction[ i2 ] * PmE[ i0 ] )
		{
			 //  v[ i1 ] >= -e[ i1 ], v[ i2 ] >= -e[ i2 ] (distance = 0)
			Point[ i0 ] = Box.fExtent[ i0 ];
			fInv = 1.0f / Direction[ i0 ];
			Point[ i1 ] -= Direction[ i1 ] * PmE[ i0 ] * fInv;
			Point[ i2 ] -= Direction[ i2 ] * PmE[ i0 ] * fInv;
			fLParam = -PmE[ i0 ] * fInv;
		}
		else
		{
			 //  v[ i1 ] >= -e[ i1 ], v[ i2 ] < -e[ i2 ]
			fLSqr = Direction[ i0 ] * Direction[ i0 ] + Direction[ i2 ] * Direction[ i2 ];
			fTmp = fLSqr * ExtentPoint[ i1 ] - Direction[ i1 ] * (Direction[ i0 ] * PmE[ i0 ] + Direction[ i2 ] * ExtentPoint[ i2 ]);
			if( fTmp <= 2.0f * fLSqr * Box.fExtent[ i1 ] )
			{
				fT = fTmp / fLSqr;
				fLSqr += Direction[ i1 ] * Direction[ i1 ];
				fTmp = ExtentPoint[ i1 ] - fT;
				fDelta = Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * fTmp + Direction[ i2 ] * ExtentPoint[ i2 ];
				fParam = -fDelta / fLSqr;
				fSqrDistance += PmE[ i0 ] * PmE[ i0 ] + fTmp * fTmp + ExtentPoint[ i2 ] * ExtentPoint[ i2 ] + fDelta * fParam;

				fLParam = fParam;
				Point[ i0 ] = Box.fExtent[ i0 ];
				Point[ i1 ] = fT - Box.fExtent[ i1 ];
				Point[ i2 ] = -Box.fExtent[ i2 ];
			}
			else
			{
				fLSqr += Direction[ i1 ] * Direction[ i1 ];
				fDelta = Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * PmE[ i1 ] + Direction[ i2 ] * ExtentPoint[ i2 ];
				fParam = -fDelta / fLSqr;
				fSqrDistance += PmE[ i0 ] * PmE[ i0 ] + PmE[ i1 ] * PmE[ i1 ] + ExtentPoint[ i2 ] * ExtentPoint[ i2 ] + fDelta * fParam;

				fLParam = fParam;
				Point[ i0 ] = Box.fExtent[ i0 ];
				Point[ i1 ] = Box.fExtent[ i1 ];
				Point[ i2 ] = -Box.fExtent[ i2 ];
			}
		}
	}
	else
	{
		if( Direction[ i0 ] * ExtentPoint[ i2 ] >= Direction[ i2 ] * PmE[ i0 ] )
		{
			 //  v[ i1 ] < -e[ i1 ], v[ i2 ] >= -e[ i2 ]
			fLSqr = Direction[ i0 ] * Direction[ i0 ] + Direction[ i1 ] * Direction[ i1 ];
			fTmp = fLSqr * ExtentPoint[ i2 ] - Direction[ i2 ] * (Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * ExtentPoint[ i1 ]);
			if( fTmp <= 2.0f * fLSqr * Box.fExtent[ i2 ] )
			{
				fT = fTmp / fLSqr;
				fLSqr += Direction[ i2 ] * Direction[ i2 ];
				fTmp = ExtentPoint[ i2 ] - fT;
				fDelta = Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * ExtentPoint[ i1 ] + Direction[ i2 ] * fTmp;
				fParam = -fDelta / fLSqr;
				fSqrDistance += PmE[ i0 ] * PmE[ i0 ] + ExtentPoint[ i1 ] * ExtentPoint[ i1 ] + fTmp * fTmp + fDelta * fParam;

				fLParam = fParam;
				Point[ i0 ] = Box.fExtent[ i0 ];
				Point[ i1 ] = -Box.fExtent[ i1 ];
				Point[ i2 ] = fT - Box.fExtent[ i2 ];
			}
			else
			{
				fLSqr += Direction[ i2 ] * Direction[ i2 ];
				fDelta = Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * ExtentPoint[ i1 ] + Direction[ i2 ] * PmE[ i2 ];
				fParam = -fDelta / fLSqr;
				fSqrDistance += PmE[ i0 ] * PmE[ i0 ] + ExtentPoint[ i1 ] * ExtentPoint[ i1 ] + PmE[ i2 ] * PmE[ i2 ] + fDelta * fParam;

				fLParam = fParam;
				Point[ i0 ] = Box.fExtent[ i0 ];
				Point[ i1 ] = -Box.fExtent[ i1 ];
				Point[ i2 ] = Box.fExtent[ i2 ];
			}
		}
		else
		{
			 //  v[ i1 ] < -e[ i1 ], v[ i2 ] < -e[ i2 ]
			fLSqr = Direction[ i0 ] * Direction[ i0 ]+Direction[ i2 ] * Direction[ i2 ];
			fTmp = fLSqr * ExtentPoint[ i1 ] - Direction[ i1 ] * (Direction[ i0 ] * PmE[ i0 ] + Direction[ i2 ] * ExtentPoint[ i2 ]);
			if( fTmp >= (float)0.0 )
			{
				 //  v[ i1 ]-edge is closest
				if( fTmp <= 2.0f * fLSqr * Box.fExtent[ i1 ] )
				{
					fT = fTmp / fLSqr;
					fLSqr += Direction[ i1 ] * Direction[ i1 ];
					fTmp = ExtentPoint[ i1 ] - fT;
					fDelta = Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * fTmp + Direction[ i2 ] * ExtentPoint[ i2 ];
					fParam = -fDelta / fLSqr;
					fSqrDistance += PmE[ i0 ] * PmE[ i0 ] + fTmp * fTmp + ExtentPoint[ i2 ] * ExtentPoint[ i2 ] + fDelta * fParam;

					fLParam = fParam;
					Point[ i0 ] = Box.fExtent[ i0 ];
					Point[ i1 ] = fT - Box.fExtent[ i1 ];
					Point[ i2 ] = -Box.fExtent[ i2 ];
				}
				else
				{
					fLSqr += Direction[ i1 ] * Direction[ i1 ];
					fDelta = Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * PmE[ i1 ] + Direction[ i2 ] * ExtentPoint[ i2 ];
					fParam = -fDelta / fLSqr;
					fSqrDistance += PmE[ i0 ] * PmE[ i0 ] + PmE[ i1 ] * PmE[ i1 ] + ExtentPoint[ i2 ] * ExtentPoint[ i2 ] + fDelta * fParam;

					fLParam = fParam;
					Point[ i0 ] = Box.fExtent[ i0 ];
					Point[ i1 ] = Box.fExtent[ i1 ];
					Point[ i2 ] = -Box.fExtent[ i2 ];
				}
				return;
			}

			fLSqr = Direction[ i0 ] * Direction[ i0 ] + Direction[ i1 ] * Direction[ i1 ];
			fTmp = fLSqr * ExtentPoint[ i2 ] - Direction[ i2 ] * (Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * ExtentPoint[ i1 ]);
			if( fTmp >= 0.0f )
			{
				 //  v[ i2 ]-edge is closest
				if( fTmp <= 2.0f * fLSqr * Box.fExtent[ i2 ] )
				{
					fT = fTmp / fLSqr;
					fLSqr += Direction[ i2 ] * Direction[ i2 ];
					fTmp = ExtentPoint[ i2 ] - fT;
					fDelta = Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * ExtentPoint[ i1 ] + Direction[ i2 ] * fTmp;
					fParam = -fDelta / fLSqr;
					fSqrDistance += PmE[ i0 ] * PmE[ i0 ] + ExtentPoint[ i1 ] * ExtentPoint[ i1 ] + fTmp * fTmp + fDelta * fParam;

					fLParam = fParam;
					Point[ i0 ] = Box.fExtent[ i0 ];
					Point[ i1 ] = -Box.fExtent[ i1 ];
					Point[ i2 ] = fT - Box.fExtent[ i2 ];
				}
				else
				{
					fLSqr += Direction[ i2 ] * Direction[ i2 ];
					fDelta = Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * ExtentPoint[ i1 ] + Direction[ i2 ] * PmE[ i2 ];
					fParam = -fDelta / fLSqr;
					fSqrDistance += PmE[ i0 ] * PmE[ i0 ] + ExtentPoint[ i1 ] * ExtentPoint[ i1 ] + PmE[ i2 ] * PmE[ i2 ] + fDelta * fParam;

					fLParam = fParam;
					Point[ i0 ] = Box.fExtent[ i0 ];
					Point[ i1 ] = -Box.fExtent[ i1 ];
					Point[ i2 ] = Box.fExtent[ i2 ];
				}
				return;
			}

			 //  (v[ i1 ],v[ i2 ])-corner is closest
			fLSqr += Direction[ i2 ] * Direction[ i2 ];
			fDelta = Direction[ i0 ] * PmE[ i0 ] + Direction[ i1 ] * ExtentPoint[ i1 ] + Direction[ i2 ] * ExtentPoint[ i2 ];
			fParam = -fDelta / fLSqr;
			fSqrDistance += PmE[ i0 ] * PmE[ i0 ] + ExtentPoint[ i1 ] * ExtentPoint[ i1 ] + ExtentPoint[ i2 ] * ExtentPoint[ i2 ] + fDelta * fParam;

			fLParam = fParam;
			Point[ i0 ] = Box.fExtent[ i0 ];
			Point[ i1 ] = -Box.fExtent[ i1 ];
			Point[ i2 ] = -Box.fExtent[ i2 ];
		}
	}
}

void CaseNoZeros( SCollisionBox &Box, EtVector3 &Point, EtVector3 &Direction, float &fLParam, float &fSqrDistance )
{
	EtVector3 PmE;

	PmE.x = Point.x - Box.fExtent[ 0 ];
	PmE.y = Point.y - Box.fExtent[ 1 ];
	PmE.z = Point.z - Box.fExtent[ 2 ];

	float fProdDxPy = Direction.x * PmE.y;
	float fProdDyPx = Direction.y * PmE.x;
	float fProdDzPx, fProdDxPz, fProdDzPy, fProdDyPz;

	if( fProdDyPx >= fProdDxPy )
	{
		fProdDzPx = Direction.z * PmE.x;
		fProdDxPz = Direction.x * PmE.z;
		if( fProdDzPx >= fProdDxPz )
		{
			// line intersects x = e0
			Face( Box, 0, 1, 2, Point, Direction, PmE, fLParam, fSqrDistance );
		}
		else
		{
			// line intersects z = e2
			Face( Box, 2, 0, 1, Point, Direction, PmE, fLParam, fSqrDistance );
		}
	}
	else
	{
		fProdDzPy = Direction.z * PmE.y;
		fProdDyPz = Direction.y * PmE.z;
		if( fProdDzPy >= fProdDyPz )
		{
			// line intersects y = e1
			Face( Box, 1, 2, 0, Point, Direction, PmE, fLParam, fSqrDistance );
		}
		else
		{
			// line intersects z = e2
			Face( Box, 2, 0, 1, Point, Direction, PmE, fLParam, fSqrDistance );
		}
	}
}

void Case0( SCollisionBox &Box, int i0, int i1, int i2, EtVector3 &Point, EtVector3 &Direction, float &fLParam, float &fSqrDistance )
{
	float fPmE0 = Point[ i0 ] - Box.fExtent[ i0 ];
	float fPmE1 = Point[ i1 ] - Box.fExtent[ i1 ];
	float fProd0 = Direction[ i1 ] * fPmE0;
	float fProd1 = Direction[ i0 ] * fPmE1;
	float fDelta, fInvLSqr, fInv;

	if( fProd0 >= fProd1 )
	{
		// line intersects P[ i0 ] = e[ i0 ]
		Point[ i0 ] = Box.fExtent[ i0 ];

		float fPpE1 = Point[ i1 ] + Box.fExtent[ i1 ];
		fDelta = fProd0 - Direction[ i0 ] * fPpE1;
		if( fDelta >= 0.0f )
		{
			fInvLSqr = 1.0f / ( Direction[ i0 ] * Direction[ i0 ]+Direction[ i1 ] * Direction[ i1 ] );
			fSqrDistance += fDelta * fDelta * fInvLSqr;
			Point[ i1 ] = -Box.fExtent[ i1 ];
			fLParam = -( Direction[ i0 ] * fPmE0+Direction[ i1 ] * fPpE1 ) * fInvLSqr;
		}
		else
		{
			fInv = 1.0f / Direction[ i0 ];
			Point[ i1 ] -= fProd0 * fInv;
			fLParam = -fPmE0 * fInv;
		}
	}
	else
	{
		// line intersects P[ i1 ] = e[ i1 ]
		Point[ i1 ] = Box.fExtent[ i1 ];

		float fPpE0 = Point[ i0 ] + Box.fExtent[ i0 ];
		fDelta = fProd1 - Direction[ i1 ] * fPpE0;
		if( fDelta >= 0.0f )
		{
			fInvLSqr = 1.0f / ( Direction[ i0 ] * Direction[ i0 ]+Direction[ i1 ] * Direction[ i1 ] );
			fSqrDistance += fDelta * fDelta * fInvLSqr;
			Point[ i0 ] = -Box.fExtent[ i0 ];
			fLParam = -( Direction[ i0 ] * fPpE0+Direction[ i1 ] * fPmE1 ) * fInvLSqr;
		}
		else
		{
			fInv = ( 1.0f ) / Direction[ i1 ];
			Point[ i0 ] -= fProd1 * fInv;
			fLParam = -fPmE1 * fInv;
		}
	}

	if( Point[ i2 ] < -Box.fExtent[ i2 ] )
	{
		fDelta = Point[ i2 ] + Box.fExtent[ i2 ];
		fSqrDistance += fDelta * fDelta;
		Point[ i2 ] = -Box.fExtent[ i2 ];
	}
	else if( Point[ i2 ] > Box.fExtent[ i2 ] )
	{
		fDelta = Point[ i2 ] - Box.fExtent[ i2 ];
		fSqrDistance += fDelta * fDelta;
		Point[ i2 ] = Box.fExtent[ i2 ];
	}
}

void Case00( SCollisionBox &Box, int i0, int i1, int i2, EtVector3 &Point, EtVector3 &Direction, float &fLParam, float &fSqrDistance )
{
	float fDelta;

	fLParam = ( Box.fExtent[ i0 ] - Point[ i0 ] ) / Direction[ i0 ];

	Point[ i0 ] = Box.fExtent[ i0 ];

	if( Point[ i1 ] < -Box.fExtent[ i1 ] )
	{
		fDelta = Point[ i1 ] + Box.fExtent[ i1 ];
		fSqrDistance += fDelta * fDelta;
		Point[ i1 ] = -Box.fExtent[ i1 ];
	}
	else if( Point[ i1 ] > Box.fExtent[ i1 ] )
	{
		fDelta = Point[ i1 ] - Box.fExtent[ i1 ];
		fSqrDistance += fDelta * fDelta;
		Point[ i1 ] = Box.fExtent[ i1 ];
	}

	if( Point[ i2 ] < -Box.fExtent[ i2 ] )
	{
		fDelta = Point[ i2 ] + Box.fExtent[ i2 ];
		fSqrDistance += fDelta * fDelta;
		Point[ i2 ] = -Box.fExtent[ i2 ];
	}
	else if( Point[ i2 ] > Box.fExtent[ i2 ] )
	{
		fDelta = Point[ i2 ] - Box.fExtent[ i2 ];
		fSqrDistance += fDelta * fDelta;
		Point[ i2 ] = Box.fExtent[ i2 ];
	}
}

void Case000( SCollisionBox &Box, EtVector3 &Point, float &fSqrDistance )
{
	float fDelta;

	if( Point.x < -Box.fExtent[ 0 ] )
	{
		fDelta = Point.x + Box.fExtent[ 0 ];
		fSqrDistance += fDelta * fDelta;
		Point.x = -Box.fExtent[ 0 ];
	}
	else if( Point.x > Box.fExtent[ 0 ] )
	{
		fDelta = Point.x - Box.fExtent[ 0 ];
		fSqrDistance += fDelta * fDelta;
		Point.x = Box.fExtent[ 0 ];
	}

	if( Point.y < -Box.fExtent[ 1 ] )
	{
		fDelta = Point.y + Box.fExtent[ 1 ];
		fSqrDistance += fDelta * fDelta;
		Point.y = -Box.fExtent[ 1 ];
	}
	else if( Point.y > Box.fExtent[ 1 ] )
	{
		fDelta = Point.y - Box.fExtent[ 1 ];
		fSqrDistance += fDelta * fDelta;
		Point.y = Box.fExtent[ 1 ];
	}

	if( Point.z < -Box.fExtent[ 2 ] )
	{
		fDelta = Point.z + Box.fExtent[ 2 ];
		fSqrDistance += fDelta * fDelta;
		Point.z = -Box.fExtent[ 2 ];
	}
	else if( Point.z > Box.fExtent[ 2 ] )
	{
		fDelta = Point.z - Box.fExtent[ 2 ];
		fSqrDistance += fDelta * fDelta;
		Point.z = Box.fExtent[ 2 ];
	}
}

#define COMPARE_MIN( fTmpMinD, fMinD, fTmpSeg, fSeg, fTmpTri0, fTri0, fTmpTri1, fTri1 ) \
{ \
	if( fTmpMinD < fMinD ) \
	{ \
		fMinD = fTmpMinD; \
		fSeg = fTmpSeg; \
		fTri0 = fTmpTri0; \
		fTri1 = fTmpTri1; \
	} \
}

float DistPointToTriangle( EtVector3 &vPoint, SCollisionTriangle &Triangle, float &fTriParam0, float &fTriParam1 )
{
	EtVector3 vDiff = Triangle.vOrigin - vPoint;
	float fA = EtVec3Dot( &Triangle.vEdge1, &Triangle.vEdge1 );
	float fB = EtVec3Dot( &Triangle.vEdge1, &Triangle.vEdge2 );
	float fC = EtVec3Dot( &Triangle.vEdge2, &Triangle.vEdge2 );
	float fD = EtVec3Dot( &vDiff, &Triangle.vEdge1 );
	float fE = EtVec3Dot( &vDiff, &Triangle.vEdge2 );
	float fF = EtVec3Dot( &vDiff, &vDiff );
	float fDet = fabs( fA * fC - fB * fB );

	fTriParam0 = fB * fE - fC * fD;
	fTriParam1 = fB * fD - fA * fE;

	if( fTriParam0 + fTriParam1 <= fDet )
	{
		if( fTriParam0 < 0.0f )
		{
			if( fTriParam1 < 0.0f )  // region 4
			{
				if( fD < 0.0f )
				{
					fTriParam1 = 0.0f;
					if( -fD >= fA )
					{
						fTriParam0 = 1.0f;
						return fabs( fA + 2.0f * fD + fF );
					}
					else
					{
						fTriParam0 = -fD / fA;
						return fabs( fD * fTriParam0 + fF );
					}
				}
				else
				{
					fTriParam0 = 0.0f;
					if( fE >= 0.0f )
					{
						fTriParam1 = 0.0f;
						return fabs(fF);
					}
					else if( -fE >= fC )
					{
						fTriParam1 = 1.0f;
						return fabs( fC + 2.0f * fE + fF );
					}
					else
					{
						fTriParam1 = -fE / fC;
						return fabs( fE * fTriParam1 + fF );
					}
				}
			}
			else  // region 3
			{
				fTriParam0 = 0.0f;
				if( fE >= 0.0f )
				{
					fTriParam1 = 0.0f;
					return fabs( fF );
				}
				else if( -fE >= fC )
				{
					fTriParam1 = 1.0f;
					return fabs( fC + 2.0f * fE + fF );
				}
				else
				{
					fTriParam1 = -fE / fC;
					return fabs( fE * fTriParam1 + fF );
				}
			}
		}
		else if( fTriParam1 < 0.0f )  // region 5
		{
			fTriParam1 = 0.0f;
			if( fD >= 0.0f )
			{
				fTriParam0 = 0.0f;
				return fabs( fF );
			}
			else if( -fD >= fA )
			{
				fTriParam0 = 1.0f;
				return fabs( fA + 2.0f * fD + fF );
			}
			else
			{
				fTriParam0 = -fD / fA;
				return fabs( fD * fTriParam0 + fF );
			}
		}
		else  // region 0
		{
			// minimum at interior point
			float fInvDet = 1.0f / fDet;
			fTriParam0 *= fInvDet;
			fTriParam1 *= fInvDet;
			return fabs( fTriParam0 * ( fA * fTriParam0 + fB * fTriParam1 + 2 * fD ) + fTriParam1 * ( fB * fTriParam0 + fC * fTriParam1	+ 2 * fE ) + fF );
		}
	}
	else
	{
		float fTmp0;
		float fTmp1;
		float fNumer;
		float fDenom;

		if( fTriParam0 < 0.0f )  // region 2
		{
			fTmp0 = fB + fD;
			fTmp1 = fC + fE;
			if( fTmp1 > fTmp0 )
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA - 2.0f * fB + fC;
				if( fNumer >= fDenom )
				{
					fTriParam0 = 1.0f;
					fTriParam1 = 0.0f;
					return fabs( fA + 2.0f * fD + fF );
				}
				else
				{
					fTriParam0 = fNumer / fDenom;
					fTriParam1 = 1.0f - fTriParam0;
					return fabs( fC + 2.0f * fE + fF - fNumer * fTriParam0 );
				}
			}
			else
			{
				fTriParam0 = 0.0f;
				if( fTmp1 <= 0.0f )
				{
					fTriParam1 = 1.0f;
					return fabs( fC + 2.0f * fE + fF );
				}
				else if( fE >= 0.0f )
				{
					fTriParam1 = 0.0f;
					return fabs( fF );
				}
				else
				{
					fTriParam1 = -fE / fC;
					return fabs( fE * fTriParam1 + fF );
				}
			}
		}
		else if( fTriParam1 < 0.0f )  // region 6
		{
			fTmp0 = fB + fE;
			fTmp1 = fA + fD;
			if( fTmp1 > fTmp0 )
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA - 2.0f * fB + fC;
				if( fNumer >= fDenom )
				{
					fTriParam1 = 1.0f;
					fTriParam0 = 0.0f;
					return fabs( fC + 2.0f * fE + fF );
				}
				else
				{
					fTriParam1 = fNumer / fDenom;
					fTriParam0 = 1.0f - fTriParam1;
					return fabs( fA + 2.0f * fD + fF - fNumer * fTriParam1 );
				}
			}
			else
			{
				fTriParam1 = 0.0f;
				if( fTmp1 <= 0.0f )
				{
					fTriParam0 = 1.0f;
					return fabs( fA + 2.0f * fD + fF );
				}
				else if( fD >= 0.0f )
				{
					fTriParam0 = 0.0f;
					return fabs( fF );
				}
				else
				{
					fTriParam0 = -fD / fA;
					return fabs( fD * fTriParam0 + fF );
				}
			}
		}
		else  // region 1
		{
			fNumer = ( fC + fE ) - ( fB + fD );
			if(fNumer <= 0)
			{
				fTriParam0 = 0.0f;
				fTriParam1 = 1.0f;
				return fabs( fC + 2.0f * fE + fF );
			}
			else
			{
				fDenom = fA - 2.0f * fB + fC;
				if( fNumer >= fDenom )
				{
					fTriParam0 = 1.0f;
					fTriParam1 = 0.0f;
					return fabs( fA + 2.0f * fD + fF );
				}
				else
				{
					fTriParam0 = fNumer / fDenom;
					fTriParam1 = 1.0f - fTriParam0;
					return fabs( fC + 2.0f * fE + fF - fNumer * fTriParam0 );
				}
			}
		}
	}
}

float DistSegmentToTriangle( SSegment &Segment, SCollisionTriangle &Triangle, float &fSegParm, float &fTriParm0, float &fTriParm1 )
{
	const float fTolerance = 1e-05f;
	SSegment TriangleSeg;
	EtVector3 vPoint;
	float fMin, fMin0, fTmpSegParm, fTmpTriParm0, fTmpTriParm1;

	EtVector3 vDiff = Triangle.vOrigin - Segment.vOrigin;
	float fA00 = EtVec3Dot( &Segment.vDirection, &Segment.vDirection );
	float fA01 = -EtVec3Dot( &Segment.vDirection, &Triangle.vEdge1 );
	float fA02 = -EtVec3Dot( &Segment.vDirection, &Triangle.vEdge2 );
	float fA11 = EtVec3Dot( &Triangle.vEdge1, &Triangle.vEdge1 );
	float fA12 = EtVec3Dot( &Triangle.vEdge1, &Triangle.vEdge2 );
	float fA22 = EtVec3Dot( &Triangle.vEdge2, &Triangle.vEdge2 );
	float fB0 = -EtVec3Dot( &vDiff, &Segment.vDirection );
	float fB1 = EtVec3Dot( &vDiff, &Triangle.vEdge1 );
	float fB2 = EtVec3Dot( &vDiff, &Triangle.vEdge2 );

	EtVector3 vPlaneNormal;
	EtVec3Cross( &vPlaneNormal, &Triangle.vEdge1, &Triangle.vEdge2 );

	if( fabs( EtVec3Dot( &vPlaneNormal, &Segment.vDirection ) ) > ( fTolerance * ( EtVec3Length( &vPlaneNormal ) * EtVec3Length( &Segment.vDirection ) ) ) )
	{
		float fCof00 = fA11 * fA22 - fA12 * fA12;
		float fCof01 = fA02 * fA12 - fA01 * fA22;
		float fCof02 = fA01 * fA12 - fA02 * fA11;
		float fCof11 = fA00 * fA22 - fA02 * fA02;
		float fCof12 = fA02 * fA01 - fA00 * fA12;
		float fCof22 = fA00 * fA11 - fA01 * fA01;

		float fDet = fA00 * fCof00 + fA01 * fCof01 + fA02 * fCof02;
		float fInvDet = 1.0f / fDet;

		float fRhs0 = -fB0 * fInvDet;
		float fRhs1 = -fB1 * fInvDet;
		float fRhs2 = -fB2 * fInvDet;

		fSegParm = fCof00 * fRhs0 + fCof01 * fRhs1 + fCof02 * fRhs2;
		fTriParm0 = fCof01 * fRhs0 + fCof11 * fRhs1 + fCof12 * fRhs2;
		fTriParm1 = fCof02 * fRhs0 + fCof12 * fRhs1 + fCof22 * fRhs2;

		if( fSegParm < 0.0f )
		{
			if( fTriParm0 + fTriParm1 <= 1.0f )
			{
				if( fTriParm0 < 0.0f )
				{
					if( fTriParm1 < 0.0f ) 
					{
						TriangleSeg.vOrigin = Triangle.vOrigin;
						TriangleSeg.vDirection = Triangle.vEdge2;
						fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
						fTriParm0 = 0.0f;
						TriangleSeg.vOrigin = Triangle.vOrigin;
						TriangleSeg.vDirection = Triangle.vEdge1;
						fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm0 );
						fTmpTriParm1 = 0.0f;
						COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
						fMin0 = DistPointToTriangle( Segment.vOrigin, Triangle, fTmpTriParm0, fTmpTriParm1 );
						fTmpSegParm = 0.0f;
						COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
					}
					else
					{
						TriangleSeg.vOrigin = Triangle.vOrigin;
						TriangleSeg.vDirection = Triangle.vEdge2;
						fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
						fTriParm0 = 0.0f;
						fMin0 = DistPointToTriangle( Segment.vOrigin, Triangle, fTmpTriParm0, fTmpTriParm1 );
						fTmpSegParm = 0.0f;
						COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
					}
				}
				else if( fTriParm1 < 0.0f )
				{
					// min on face TP1=0 or SP=0
					TriangleSeg.vOrigin = Triangle.vOrigin;
					TriangleSeg.vDirection = Triangle.vEdge1;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm0 );
					fTriParm1 = 0.0f;
					fMin0 = DistPointToTriangle( Segment.vOrigin, Triangle, fTmpTriParm0, fTmpTriParm1 );
					fTmpSegParm = 0.0f;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
				else    // region 0m, min face on SP=0
				{
					fMin = DistPointToTriangle( Segment.vOrigin, Triangle, fTriParm0, fTriParm1) ;
					fSegParm = 0.0f;
				}
			}
			else
			{
				if( fTriParm0 < 0.0f )   // region 2m
				{
					// min on face TP0=0 or TP0+TP1=1 or SP=0
					TriangleSeg.vOrigin = Triangle.vOrigin;
					TriangleSeg.vDirection = Triangle.vEdge2;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
					fTriParm0 = 0.0f;
					TriangleSeg.vOrigin = Triangle.vOrigin + Triangle.vEdge1;
					TriangleSeg.vDirection = Triangle.vEdge2-Triangle.vEdge1;
					fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm1);
					fTmpTriParm0 = 1.0f - fTmpTriParm1;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
					fMin0 = DistPointToTriangle( Segment.vOrigin, Triangle, fTmpTriParm0, fTmpTriParm1 );
					fTmpSegParm = 0.0f;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
				else if(fTriParm1 < 0.0f)  // region 6m
				{
					// min on face TP1=0 or TP0+TP1=1 or SP=0
					TriangleSeg.vOrigin = Triangle.vOrigin;
					TriangleSeg.vDirection = Triangle.vEdge1;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm0 );
					fTriParm1 = 0.0f;
					TriangleSeg.vOrigin = Triangle.vOrigin + Triangle.vEdge1;
					TriangleSeg.vDirection = Triangle.vEdge2-Triangle.vEdge1;
					fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm1 );
					fTmpTriParm0 = 1.0f - fTmpTriParm1;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
					fMin0 = DistPointToTriangle( Segment.vOrigin, Triangle, fTmpTriParm0, fTmpTriParm1 );
					fTmpSegParm = 0.0f;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
				else    // region 1m, min on face TP0+TP1=1 or SP=0
				{
					TriangleSeg.vOrigin = Triangle.vOrigin + Triangle.vEdge1;
					TriangleSeg.vDirection = Triangle.vEdge2-Triangle.vEdge1;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
					fTriParm0 = 1.0f - fTriParm1;
					fMin0 = DistPointToTriangle( Segment.vOrigin, Triangle, fTmpTriParm0, fTmpTriParm1 );
					fTmpSegParm = 0.0f;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
			}
		}
		else if( fSegParm <= 1.0f )
		{
			if( fTriParm0 + fTriParm1 <= 1.0f )
			{
				if( fTriParm0 < 0.0f )
				{
					if( fTriParm1 < 0.0f )   // region 4
					{
						// min on face TP0=0 or TP1=0
						TriangleSeg.vOrigin = Triangle.vOrigin;
						TriangleSeg.vDirection = Triangle.vEdge2;
						fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
						fTriParm0 = 0.0f;
						TriangleSeg.vOrigin = Triangle.vOrigin;
						TriangleSeg.vDirection = Triangle.vEdge1;
						fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm0) ;
						fTmpTriParm1 = 0.0f;
						COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
					}
					else    // region 3, min on face TP0=0
					{
						TriangleSeg.vOrigin = Triangle.vOrigin;
						TriangleSeg.vDirection = Triangle.vEdge2;
						fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
						fTriParm0 = 0.0f;
					}
				}
				else if( fTriParm1 < 0.0f )  // region 5, min on face TP1=0
				{
					TriangleSeg.vOrigin = Triangle.vOrigin;
					TriangleSeg.vDirection = Triangle.vEdge1;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm0 );
					fTriParm1 = 0.0f;
				}
				else    // region 0, global minimum is interior, so done
				{
					fMin = 0.0f;
				}
			}
			else
			{
				if( fTriParm0 < 0.0f )   // region 2
				{
					// min on face TP0=0 or TP0+TP1=1
					TriangleSeg.vOrigin = Triangle.vOrigin;
					TriangleSeg.vDirection = Triangle.vEdge2;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm,  fTriParm1 );
					fTriParm0 = 0.0f;
					TriangleSeg.vOrigin = Triangle.vOrigin + Triangle.vEdge1;
					TriangleSeg.vDirection = Triangle.vEdge2 - Triangle.vEdge1;
					fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm1 );
					fTmpTriParm0 = 1.0f - fTmpTriParm1;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
				else if(fTriParm1 < 0.0f)  // region 6
				{
					// min on face TP1=0 or TP0+TP1=1
					TriangleSeg.vOrigin = Triangle.vOrigin;
					TriangleSeg.vDirection = Triangle.vEdge1;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm0 );
					fTriParm1 = 0.0f;
					TriangleSeg.vOrigin = Triangle.vOrigin + Triangle.vEdge1;
					TriangleSeg.vDirection = Triangle.vEdge2 - Triangle.vEdge1;
					fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm1 );
					fTmpTriParm0 = 1.0f - fTmpTriParm1;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
				else    // region 1, min on face TP0+TP1=1
				{
					TriangleSeg.vOrigin = Triangle.vOrigin + Triangle.vEdge1;
					TriangleSeg.vDirection = Triangle.vEdge2 - Triangle.vEdge1;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
					fTriParm0 = 1.0f - fTriParm1;
				}
			}
		}
		else    // fSegParm > 1
		{
			if( fTriParm0 + fTriParm1 <= 1.0f )
			{
				if( fTriParm0 < 0.0f )
				{
					if( fTriParm1 < 0.0f )   // region 4p
					{
						// min on face TP0=0 or TP1=0 or SP=1
						TriangleSeg.vOrigin = Triangle.vOrigin;
						TriangleSeg.vDirection = Triangle.vEdge2;
						fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
						fTriParm0 = 0.0f;
						TriangleSeg.vOrigin = Triangle.vOrigin;
						TriangleSeg.vDirection = Triangle.vEdge1;
						fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm0 );
						fTmpTriParm1 = 0.0f;
						COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
						vPoint = Segment.vOrigin + Segment.vDirection;
						fMin0 = DistPointToTriangle( vPoint, Triangle, fTmpTriParm0, fTmpTriParm1 );
						fTmpSegParm = 1.0f;
						COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
					}
					else    // region 3p, min on face TP0=0 or SP=1
					{
						TriangleSeg.vOrigin = Triangle.vOrigin;
						TriangleSeg.vDirection = Triangle.vEdge2;
						fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
						fTriParm0 = 0.0f;
						vPoint = Segment.vOrigin + Segment.vDirection;
						fMin0 = DistPointToTriangle( vPoint, Triangle, fTmpTriParm0, fTmpTriParm1 );
						fTmpSegParm = 1.0f;
						COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
					}
				}
				else if( fTriParm1 < 0.0f )  // region 5p
				{
					// min on face TP1=0 or SP=1
					TriangleSeg.vOrigin = Triangle.vOrigin;
					TriangleSeg.vDirection = Triangle.vEdge1;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm0 );
					fTriParm1 = 0.0f;
					vPoint = Segment.vOrigin + Segment.vDirection;
					fMin0 = DistPointToTriangle( vPoint, Triangle, fTmpTriParm0, fTmpTriParm1 );
					fTmpSegParm = 1.0f;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
				else    // region 0p, min face on SP=1
				{
					vPoint = Segment.vOrigin + Segment.vDirection;
					fMin = DistPointToTriangle( vPoint, Triangle, fTriParm0, fTriParm1 );
					fSegParm = 1.0f;
				}
			}
			else
			{
				if( fTriParm0 < 0.0f )   // region 2p
				{
					// min on face TP0=0 or TP0+TP1=1 or SP=1
					TriangleSeg.vOrigin = Triangle.vOrigin;
					TriangleSeg.vDirection = Triangle.vEdge2;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
					fTriParm0 = 0.0f;
					TriangleSeg.vOrigin = Triangle.vOrigin + Triangle.vEdge1;
					TriangleSeg.vDirection = Triangle.vEdge2 - Triangle.vEdge1;
					fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm1 );
					fTmpTriParm0 = 1.0f - fTmpTriParm1;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
					vPoint = Segment.vOrigin + Segment.vDirection;
					fMin0 = DistPointToTriangle( vPoint, Triangle, fTmpTriParm0, fTmpTriParm1 );
					fTmpSegParm = 1.0f;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
				else if( fTriParm1 < 0.0f )  // region 6p
				{
					// min on face TP1=0 or TP0+TP1=1 or SP=1
					TriangleSeg.vOrigin = Triangle.vOrigin;
					TriangleSeg.vDirection = Triangle.vEdge1;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm0 );
					fTriParm1 = 0.0f;
					TriangleSeg.vOrigin = Triangle.vOrigin + Triangle.vEdge1;
					TriangleSeg.vDirection = Triangle.vEdge2 - Triangle.vEdge1;
					fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm1 );
					fTmpTriParm0 = 1.0f - fTmpTriParm1;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
					vPoint = Segment.vOrigin + Segment.vDirection;
					fMin0 = DistPointToTriangle( vPoint, Triangle, fTmpTriParm0, fTmpTriParm1 );
					fTmpSegParm = 1.0f;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
				else    // region 1p, min on face TP0+TP1=1 or SP=1
				{
					TriangleSeg.vOrigin = Triangle.vOrigin + Triangle.vEdge1;
					TriangleSeg.vDirection = Triangle.vEdge2 - Triangle.vEdge1;
					fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm1 );
					fTriParm0 = 1.0f - fTriParm1;
					vPoint = Segment.vOrigin + Segment.vDirection;
					fMin0 = DistPointToTriangle( vPoint, Triangle, fTmpTriParm0, fTmpTriParm1 );
					fTmpSegParm = 1.0f;
					COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );
				}
			}
		}

		return fMin;
	}

	// Segment and triangle are parallel.
	TriangleSeg.vOrigin = Triangle.vOrigin;
	TriangleSeg.vDirection = Triangle.vEdge1;
	fMin = DistSegmentToSegment( Segment, TriangleSeg, fSegParm, fTriParm0 );
	fTriParm1 = 0.0f;

	TriangleSeg.vDirection = Triangle.vEdge2;
	fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm1 );
	fTmpTriParm0 = 0.0f;
	COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );

	TriangleSeg.vOrigin = TriangleSeg.vOrigin + Triangle.vEdge1;
	TriangleSeg.vDirection = TriangleSeg.vDirection - Triangle.vEdge1;
	fMin0 = DistSegmentToSegment( Segment, TriangleSeg, fTmpSegParm, fTmpTriParm1 );
	fTmpTriParm0 = 1.0f - fTmpTriParm1;
	COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );

	fMin0 = DistPointToTriangle( Segment.vOrigin, Triangle, fTmpTriParm0, fTmpTriParm1 );
	fTmpSegParm = 0.0f;
	COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );

	vPoint = Segment.vOrigin + Segment.vDirection;
	fMin0 = DistPointToTriangle( vPoint, Triangle, fTmpTriParm0, fTmpTriParm1 );
	fTmpSegParm = 1.0f;
	COMPARE_MIN( fMin0, fMin, fTmpSegParm, fSegParm, fTmpTriParm0, fTriParm0, fTmpTriParm1, fTriParm1 );

	return fMin;
}
