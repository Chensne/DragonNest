#include "StdAFx.h"
#include "EtPrimitive.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


void SOBB::Init( SAABox &Box, EtMatrix &WorldMat )
{
	float fLengthX = EtVec3Length( ( EtVector3 * )&( WorldMat._11 ) );
	float fLengthY = EtVec3Length( ( EtVector3 * )&( WorldMat._21 ) );
	float fLengthZ = EtVec3Length( ( EtVector3 * )&( WorldMat._31 ) );

	Extent[ 0 ] = fabs( ( Box.Max.x - Box.Min.x ) * 0.5f ) * fLengthX;
	Extent[ 1 ] = fabs( ( Box.Max.y - Box.Min.y ) * 0.5f ) * fLengthY;
	Extent[ 2 ] = fabs( ( Box.Max.z - Box.Min.z ) * 0.5f ) * fLengthZ;

	Center = ( Box.Max + Box.Min ) * 0.5f;
	EtVec3TransformCoord(&Center, &Center, &WorldMat );

	Axis[ 0 ] = *( EtVector3 * )&( WorldMat._11 ) / fLengthX;
	Axis[ 1 ] = *( EtVector3 * )&( WorldMat._21 ) / fLengthY;
	Axis[ 2 ] = *( EtVector3 * )&( WorldMat._31 ) / fLengthZ;

	CalcVertices();
}
