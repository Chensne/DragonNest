#pragma once

enum CollisionType
{
	CT_BOX = 0,
	CT_SPHERE = 1,
	CT_CYLINDER = 2,
	CT_TRIANGLES = 3,
};

struct SOBB
{
	Point3 Center;
	Point3 Axis[ 3 ];
	float Extent[ 3 ];
};

struct SSphere
{
	Point3 Center;
	float fRadius;
};

struct SCylinder
{
	Point3 Origin;
	Point3 Direction;
	float fRadius;
};