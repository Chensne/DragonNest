#include "Axis3DModel.h"

CAxis3DModel::CAxis3DModel(void) : m_pd3dDevice( NULL )
{

}

CAxis3DModel::~CAxis3DModel(void)
{
}


void CAxis3DModel::Initialize( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;	
}



int CAxis3DModel::CreateCone( VERTEX_FORMAT **pVertex, DWORD dwColor, float fRadius, float fLength, int nSegment, int nAxis, float fAxisPos )
{
	*pVertex = new VERTEX_FORMAT[ ( nSegment + 1 ) * 2 ];
	int nRadianOffset = 360 / (nSegment-1);
	EtVector3 vPos;

	switch( nAxis ) {
		case 0:
			vPos = EtVector3( fAxisPos + fLength, 0, 0 );
			break;
		case 1:
			vPos = EtVector3( 0, fAxisPos + fLength, 0 );
			break;
		case 2:
			vPos = EtVector3( 0, 0, fAxisPos + fLength );
			break;
	}
	if( nAxis > 2 ) (*pVertex)[0].vPos = -vPos;
	else (*pVertex)[0].vPos = vPos;
	(*pVertex)[0].dwColor = dwColor;
	for( int i=0; i<nSegment; i++ ) {
		switch( nAxis ) {
			case 0:
				vPos.x = fAxisPos;
				vPos.y = cos( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				vPos.z = sin( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				break;
			case 1:
				vPos.x = cos( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				vPos.y = fAxisPos;
				vPos.z = sin( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				break;
			case 2:
				vPos.x = cos( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				vPos.y = sin( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				vPos.z = fAxisPos;
				break;
		}
		if( nAxis > 2 )
			(*pVertex)[1+i].vPos = -vPos;
		else 
			(*pVertex)[1+i].vPos = vPos;
		(*pVertex)[1+i].dwColor = dwColor;
	}

	return ( nSegment - 1 );
}