#pragma once

bool TestSphereToSphere( SCollisionSphere &Sphere1, SCollisionSphere &Sphere2 );
bool TestBoxToSphere( SCollisionBox &Box, SCollisionSphere &Sphere );
bool TestSphereToCapsule( SCollisionSphere &Sphere, SCollisionCapsule &Capsule, float &fSegParam );
bool TestBoxToBox( SCollisionBox &Box1, SCollisionBox &Box2 );
bool TestBoxToCapsule( SCollisionBox &Box, SCollisionCapsule &Capsule, float &fSegParam, float &fBoxParam0, float &fBoxParam1, float &fBoxParam2 );
bool TestCapsuleToCapsule( SCollisionCapsule &Capsule1, SCollisionCapsule &Capsule2, float &fSegParam1, float &fSegParam2 );
bool TestCapsuleToTriangle( SCollisionCapsule &Capsule, SCollisionTriangle &Triangle, float &fSegParam1, float &fTriParam1, float &fTriParam2 );
bool TestSphereToTriangle( SCollisionSphere &Sphere, SCollisionTriangle &Triangle, float &fTriParam1, float &fTriParam2 );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TestSegmentToBox( SSegment &Segment, SCollisionBox &Box );
bool TestSegmentToSphere( SSegment &Segment, SCollisionSphere &Sphere );
bool TestSegmentToCapsule( SSegment &Segment, SCollisionCapsule &Capsule );
bool TestSegmentToTriangle( SSegment &Segment, SCollisionTriangle &Triangle );
bool TestSegmentToPrimitive( SSegment &Segment, SCollisionPrimitive &Primitive );

bool FindSegmentToBox( SSegment &Segment, SCollisionBox &Box, float &fContactTime );
bool FindSegmentToSphere( SSegment &Segment, SCollisionSphere &Sphere, float &fContactTime );
bool FindSegmentToCapsule( SSegment& Segment, SCollisionCapsule &Capsule, float &fContactTime );
bool FindSegmentToTriangle( SSegment &Segment, SCollisionTriangle &Triangle, float &fContactTime );

bool TestPlaneToCapsule( EtVector4 &vPlane, SCollisionCapsule &Capsule );
