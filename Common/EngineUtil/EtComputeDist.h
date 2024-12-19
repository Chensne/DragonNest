#pragma once

float DistLine2DToLine2D( EtVector2 &Line1, EtVector2 &Line2, EtVector2 &Line3, EtVector2 &Line4, EtVector2 &PressDir );
float DistPointToLine2D( EtVector2 &Point, EtVector2 &Line1, EtVector2 &Line2, EtVector2 &PressDir );
float DistPointToSegment( EtVector3 &Point, SSegment &Segment, float &fSegParam );
float DistPointToOBB( EtVector3 &Point, SCollisionBox &Box, float &fBoxParam0, float &fBoxParam1, float &fBoxParam2 );
float DistSegmentToSegment( SSegment &Segment1, SSegment &Segment2, float &fSeg0Param, float &fSeg1Param );
float DistSegToBox( SSegment &Segment, SCollisionBox &Box, float &fSegParam, float &fBoxParam0, float &fBoxParam1, float &fBoxParam2 );

void Face( SCollisionBox &Box, int i0, int i1, int i2, EtVector3 &Point, EtVector3 &Direction, EtVector3 &rkPmE, 
		  float &fLParam, float &fSqrDistance );
void CaseNoZeros( SCollisionBox &Box, EtVector3 &Point, EtVector3 &Direction, float &fLParam, float &fSqrDistance );
void Case0( SCollisionBox &Box, int i0, int i1, int i2, EtVector3 &Point, EtVector3 &Direction, float &fLParam, float &fSqrDistance );
void Case00( SCollisionBox &Box, int i0, int i1, int i2, EtVector3 &Point, EtVector3 &Direction, float &fLParam, float &fSqrDistance );
void Case000( SCollisionBox &Box, EtVector3 &Point, float &fSqrDistance );

float DistPointToTriangle( EtVector3 &vPoint, SCollisionTriangle &Triangle, float &fTriParam0, float &fTriParam1 );
float DistSegmentToTriangle( SSegment &Segment, SCollisionTriangle &Triangle, float &fSegParm, float &fTriParm0, float &fTriParm1 );
