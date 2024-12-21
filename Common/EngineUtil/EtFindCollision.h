#pragma once

#include "EtCollisionPrimitive.h"

typedef bool ( *CollisionFinder )( SCollisionPrimitive &, SCollisionPrimitive &, EtVector3 &, SCollisionResponse &, bool );

class CEtCollisionFinder : public CSingleton< CEtCollisionFinder >
{
public:
	CEtCollisionFinder();
	~CEtCollisionFinder();

protected:
	CollisionFinder m_aCollisionTable[ CT_TYPE_COUNT ][ CT_TYPE_COUNT ];

public:
	bool FindCollision( SCollisionPrimitive &Primitive1, SCollisionPrimitive &Primitive2, SCollisionResponse &Response, bool bCalcContactTime );
};

void FindPointToBox( EtVector3 &Point, SCollisionBox &Box, EtVector3 &CollisionNormal );

bool FindCollisionDummy( SCollisionPrimitive &Primitive1, SCollisionPrimitive &Primitive2, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindSphereToTriangle( SCollisionSphere &Sphere, SCollisionTriangle &Triangle, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindSphereToSphere( SCollisionSphere &Sphere1, SCollisionSphere &Sphere2, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindSphereToBox( SCollisionSphere &Sphere, SCollisionBox &Box, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindSphereToCapsule( SCollisionSphere &Sphere, SCollisionCapsule &Capsule, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindBoxToSphere( SCollisionBox &Box, SCollisionSphere &Sphere, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindBoxToBox( SCollisionBox &Box1, SCollisionBox &Box2, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindBoxToCapsule( SCollisionBox &Box, SCollisionCapsule &Capsule, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindCapsuleToTriangle( SCollisionCapsule &Capsule, SCollisionTriangle &Triangle, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindCapsuleToSphere( SCollisionCapsule &Capsule, SCollisionSphere &Sphere, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindCapsuleToBox( SCollisionCapsule &Capsule, SCollisionBox &Box, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
bool FindCapsuleToCapsule( SCollisionCapsule &Capsule1, SCollisionCapsule &Capsule2, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime );
