#include "Stdafx.h"
#include "DumpUtil.h"

void DumpMatrix( FILE *fp, Matrix3 &Matrix )
{
	int i;
	int nChangeTab[ 4 ]={ 0, 2, 1, 3 };
	float fDummy;
	Point3 Row, Output;

	for( i = 0;i < 4;i++ )
	{
		if( i == 3 )
		{
			fDummy = 1.0f;
		}
		else
		{
			fDummy = 0.0f;
		}
		Row = Matrix.GetRow( nChangeTab[ i ] );
		Output.x = Row.x;
		Output.y = Row.z;
		Output.z = Row.y;
		fwrite( &Output, sizeof( Point3 ), 1, fp );
		fwrite( &fDummy,  sizeof( float ), 1, fp );
	}	
}

void DumpAffine( FILE *fp, AffineParts &Affine )
{
	DumpPoint3( fp, Affine.t );
	DumpQuat( fp, Affine.q );
	DumpPoint3( fp, Affine.k );
}

void DumpPoint3( FILE *fp, Point3 &Source )
{
	Point3 Output;

	Output.x = Source.x;
	Output.y = Source.z;
	Output.z = Source.y;
	fwrite( &Output, sizeof( Point3 ), 1, fp );
}

void DumpQuat( FILE *fp, Quat &Source )
{
	Quat Output;

	Output.x = Source.x;
	Output.y = Source.z;
	Output.z = Source.y;
	Output.w = Source.w;
	fwrite( &Output, sizeof( Quat ), 1, fp );
}

