#pragma once

#include "EtCamera.h"
class CEtPrimitiveDraw : public CSingleton< CEtPrimitiveDraw > 
{
public:
	CEtPrimitiveDraw(void);
	virtual ~CEtPrimitiveDraw(void);

protected:
	CEtCustomMeshStream m_Line2DStream;
	CEtCustomMeshStream m_Triangle2DStream;
	CEtCustomMeshStream m_QuadTex2DStream;
	std::vector < EtBaseTexture * > m_vecQuad2DTexture;

	CEtCustomMeshStream m_Point3DApplyZStream;
	CEtCustomMeshStream m_Point3DStream;
	CEtCustomMeshStream m_Line3DStream;
	CEtCustomMeshStream m_Triangle3DStream;
	CEtCustomMeshStream m_QuadTex3DStream;
	std::vector < EtBaseTexture * > m_vecQuad3DTexture;

public:
	void Clear();
	void Render( EtCameraHandle hActiveCamera );

	void DrawLine2D( EtVector2 &Start, EtVector2 &End, DWORD dwColor );
	void DrawTriangle2D( EtVector2 &Point1, EtVector2 &Point2, EtVector2 &Point3, DWORD dwColor );

	void DrawPoint3D( EtVector3 &Point, DWORD dwColor, EtMatrix *pWorldMat = NULL, bool bIgnoreZBuffer = true );
	void DrawLine3D( EtVector3 &Start, EtVector3 &End, DWORD dwColor, EtMatrix *pWorldMat = NULL );
	void DrawTriangle3D( EtVector3 &Point1, EtVector3 &Point2, EtVector3 &Point3, DWORD dwColor, EtMatrix *pWorldMat = NULL );
	void DrawTriangle3D( EtVector3 *pBuffer, int dwTriangleCount, DWORD dwColor, EtMatrix *pWorldMat = NULL );
	void DrawTriangle3D( SPrimitiveDraw3D *pBuffer, int dwTriangleCount );

	void DrawQuad3DWithTex( EtVector3 *pVertices, EtVector2 *pTexCoord, EtBaseTexture *pTexture, EtMatrix *pWorldMat = NULL );
	void DrawQuad2DWithTex( EtVector4 *pVertices, EtVector2 *pTexCoord, EtBaseTexture *pTexture );

	void Flush();
};
