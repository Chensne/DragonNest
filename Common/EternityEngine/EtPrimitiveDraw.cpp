#include "StdAfx.h"
#include "EtPrimitiveDraw.h"
#include "EtCamera.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtPrimitiveDraw g_PrimitiveMng;

CEtPrimitiveDraw::CEtPrimitiveDraw()
{
	m_Line2DStream.SetPrimitiveType( PT_LINELIST );
	m_Triangle2DStream.SetPrimitiveType( PT_TRIANGLELIST );
	m_QuadTex2DStream.SetPrimitiveType( PT_TRIANGLELIST );

	m_Point3DApplyZStream.SetPrimitiveType( PT_POINTLIST );
	m_Point3DStream.SetPrimitiveType( PT_POINTLIST );
	m_Line3DStream.SetPrimitiveType( PT_LINELIST );
	m_Triangle3DStream.SetPrimitiveType( PT_TRIANGLELIST );
	m_QuadTex3DStream.SetPrimitiveType( PT_TRIANGLELIST );

}

CEtPrimitiveDraw::~CEtPrimitiveDraw()
{
	Clear();
}

void CEtPrimitiveDraw::Clear()
{
	m_Line2DStream.Clear();
	m_Triangle2DStream.Clear();
	m_QuadTex2DStream.Clear();

	m_Point3DApplyZStream.Clear();
	m_Point3DStream.Clear();
	m_Line3DStream.Clear();
	m_Triangle3DStream.Clear();
	m_QuadTex3DStream.Clear();
}

void CEtPrimitiveDraw::DrawLine2D( EtVector2 &Start, EtVector2 &End, DWORD dwColor )
{
	SPrimitiveDraw2D Vertex[ 2 ];

	Vertex[ 0 ].Position.x = Start.x * GetEtDevice()->Width();
	Vertex[ 0 ].Position.y = Start.y * GetEtDevice()->Height();
	Vertex[ 0 ].Position.z = 0.0f;
	Vertex[ 0 ].Position.w = 1.0f;
	Vertex[ 0 ].dwColor = dwColor;

	Vertex[ 1 ].Position.x = End.x * GetEtDevice()->Width();
	Vertex[ 1 ].Position.y = End.y * GetEtDevice()->Height();
	Vertex[ 1 ].Position.z = 0.0f;
	Vertex[ 1 ].Position.w = 1.0f;
	Vertex[ 1 ].dwColor = dwColor;

	CMemoryStream Stream( Vertex, sizeof( SPrimitiveDraw2D ) * 2 );
	m_Line2DStream.AddVertexStream( &Stream, 2, sizeof( SPrimitiveDraw2D ) );
}

void CEtPrimitiveDraw::DrawTriangle2D( EtVector2 &Point1, EtVector2 &Point2, EtVector2 &Point3, DWORD dwColor )
{
	SPrimitiveDraw2D Vertex[ 3 ];

	Vertex[ 0 ].Position.x = Point1.x * GetEtDevice()->Width();
	Vertex[ 0 ].Position.y = Point1.y * GetEtDevice()->Height();
	Vertex[ 0 ].Position.z = 0.0f;
	Vertex[ 0 ].Position.w = 1.0f;
	Vertex[ 0 ].dwColor = dwColor;

	Vertex[ 1 ].Position.x = Point2.x * GetEtDevice()->Width();
	Vertex[ 1 ].Position.y = Point2.y * GetEtDevice()->Height();
	Vertex[ 1 ].Position.z = 0.0f;
	Vertex[ 1 ].Position.w = 1.0f;
	Vertex[ 1 ].dwColor = dwColor;

	Vertex[ 2 ].Position.x = Point3.x * GetEtDevice()->Width();
	Vertex[ 2 ].Position.y = Point3.y * GetEtDevice()->Height();
	Vertex[ 2 ].Position.z = 0.0f;
	Vertex[ 2 ].Position.w = 1.0f;
	Vertex[ 2 ].dwColor = dwColor;

	CMemoryStream Stream( Vertex, sizeof( SPrimitiveDraw2D ) * 3 );
	m_Triangle2DStream.AddVertexStream( &Stream, 3, sizeof( SPrimitiveDraw2D ));
}

void CEtPrimitiveDraw::DrawLine3D( EtVector3 &Start, EtVector3 &End, DWORD dwColor, EtMatrix *pWorldMat )
{
	SPrimitiveDraw3D Vertex[ 2 ];

	if( pWorldMat ) 
	{
		EtVec3TransformCoord( &Vertex[ 0 ].Position, &Start, pWorldMat );
		EtVec3TransformCoord( &Vertex[ 1 ].Position, &End, pWorldMat );
	}
	else
	{
		Vertex[ 0 ].Position = Start;
		Vertex[ 1 ].Position = End;
	}
	Vertex[ 0 ].dwColor = dwColor;
	Vertex[ 1 ].dwColor = dwColor;

	CMemoryStream Stream( Vertex, sizeof( SPrimitiveDraw3D ) * 2 );
	m_Line3DStream.AddVertexStream( &Stream, 2, sizeof( SPrimitiveDraw3D ));
}

void CEtPrimitiveDraw::DrawPoint3D( EtVector3 &Point, DWORD dwColor, EtMatrix *pWorldMat, bool bIgnoreZBuffer )
{
	SPrimitiveDraw3D Vertex;

	if( pWorldMat ) 
	{
		EtVec3TransformCoord( &Vertex.Position, &Point, pWorldMat );
	}
	else
	{
		Vertex.Position = Point;
	}
	Vertex.dwColor = dwColor;

	CMemoryStream Stream( &Vertex, sizeof( SPrimitiveDraw3D ) );
	if( bIgnoreZBuffer )
	{
		m_Point3DStream.AddVertexStream( &Stream, 1, sizeof( SPrimitiveDraw3D ));
	}
	else
	{
		m_Point3DApplyZStream.AddVertexStream( &Stream, 1, sizeof( SPrimitiveDraw3D ));
	}
}

void CEtPrimitiveDraw::DrawTriangle3D( EtVector3 &Point1, EtVector3 &Point2, EtVector3 &Point3, DWORD dwColor, EtMatrix *pWorldMat )
{
	SPrimitiveDraw3D Vertex[ 3 ];

	if( pWorldMat )
	{
		EtVec3TransformCoord( &Vertex[ 0 ].Position, &Point1, pWorldMat );
		EtVec3TransformCoord( &Vertex[ 1 ].Position, &Point2, pWorldMat );
		EtVec3TransformCoord( &Vertex[ 2 ].Position, &Point3, pWorldMat );
	}
	else
	{
		Vertex[ 0 ].Position = Point1;
		Vertex[ 1 ].Position = Point2;
		Vertex[ 2 ].Position = Point3;
	}
	Vertex[ 0 ].dwColor = dwColor;
	Vertex[ 1 ].dwColor = dwColor;
	Vertex[ 2 ].dwColor = dwColor;

	CMemoryStream Stream( &Vertex, sizeof( SPrimitiveDraw3D ) * 3 );
	m_Triangle3DStream.AddVertexStream( &Stream, 3, sizeof( SPrimitiveDraw3D ));
}

void CEtPrimitiveDraw::DrawTriangle3D( EtVector3 *pBuffer, int nVertexCount, DWORD dwColor, EtMatrix *pWorldMat )
{
	int i;
	SPrimitiveDraw3D *pVertex;

	pVertex = new SPrimitiveDraw3D[ nVertexCount * 3 ];
	for( i = 0; i < nVertexCount; i++ )
	{
		if( pWorldMat )
		{
			EtVec3TransformCoord( &pVertex[ i ].Position, pBuffer + i, pWorldMat );
		}
		else
		{
			pVertex[ i ].Position = pBuffer[ i ];
		}
		pVertex[ i ].dwColor = dwColor;
	}

	CMemoryStream Stream( pVertex, sizeof( SPrimitiveDraw3D ) * nVertexCount );
	m_Triangle3DStream.AddVertexStream( &Stream, nVertexCount, sizeof( SPrimitiveDraw3D ));

	SAFE_DELETEA( pVertex );
}

void CEtPrimitiveDraw::DrawTriangle3D( SPrimitiveDraw3D *pBuffer, int nTriangleCount )
{
	CMemoryStream Stream( pBuffer, sizeof( SPrimitiveDraw3D ) * nTriangleCount * 3 );
	m_Triangle3DStream.AddVertexStream( &Stream, nTriangleCount * 3, sizeof( SPrimitiveDraw3D ));
}

void CEtPrimitiveDraw::DrawQuad3DWithTex( EtVector3 *pVertices, EtVector2 *pTexCoord, EtBaseTexture *pTexture, EtMatrix *pWorldMat )
{
	int i;
	SPrimitiveDraw3DTex Vertex[ 6 ];

	for( i = 0; i < 4; i++ )
	{
		if( pWorldMat ) 
		{
			EtVec3TransformCoord( &Vertex[ i ].Position, pVertices + i, pWorldMat );
		}
		else
		{
			Vertex[ i ].Position = pVertices[ i ];
		}
		Vertex[ i ].TexCoord = pTexCoord[ i ];
	}
	Vertex[ 4 ] = Vertex[ 0 ];
	Vertex[ 5 ] = Vertex[ 2 ];
	m_vecQuad3DTexture.push_back( pTexture );

	CMemoryStream Stream( Vertex, sizeof( SPrimitiveDraw3DTex ) * 6 );
	m_QuadTex3DStream.AddVertexStream( &Stream, 6, sizeof( SPrimitiveDraw3DTex ));
}

void CEtPrimitiveDraw::DrawQuad2DWithTex( EtVector4 *pVertices, EtVector2 *pTexCoord, EtBaseTexture *pTexture )
{
	int i;
	SPrimitiveDraw2DTex Vertex[ 6 ];

	for( i = 0; i < 4; i++ )
	{
		Vertex[ i ].Position.x = pVertices[ i ].x;
		Vertex[ i ].Position.y = pVertices[ i ].y;
		Vertex[ i ].Position.z = pVertices[ i ].z;
		Vertex[ i ].TexCoord = pTexCoord[ i ];
	}
	Vertex[ 4 ] = Vertex[ 0 ];
	Vertex[ 5 ] = Vertex[ 2 ];
	m_vecQuad2DTexture.push_back( pTexture );

	CMemoryStream Stream( Vertex, sizeof( SPrimitiveDraw2DTex ) * 6 );
	m_QuadTex2DStream.AddVertexStream( &Stream, 6, sizeof( SPrimitiveDraw2DTex ));
}

void CEtPrimitiveDraw::Render( EtCameraHandle hActiveCamera )
{
	EtMatrix WorldMat;

	EtMatrixIdentity( &WorldMat );
	GetEtDevice()->SetWorldTransform( &WorldMat );
	if( hActiveCamera ) {
		GetEtDevice()->SetViewTransform( hActiveCamera->GetViewMat() );
		GetEtDevice()->SetProjTransform( hActiveCamera->GetProjMat() );
	}
	GetEtDevice()->SetCullMode( CULL_NONE );

	GetEtDevice()->SetVertexShader( NULL );
	GetEtDevice()->SetPixelShader( NULL );

	int i;
	GetEtDevice()->EnableAlphaBlend( true );
	GetEtDevice()->SetFVF( FVF_XYZ | FVF_TEX1 );
	if( m_QuadTex3DStream.GetVertexCount() )
	{
		for( i = 0; i < m_QuadTex3DStream.GetVertexCount() / 6; i++ )
		{
			GetEtDevice()->SetTexture( 0, m_vecQuad3DTexture[ i ] );
			m_QuadTex3DStream.Draw( -1, i * 2, 2 );
		}
	}

	// GMA4500 에서 RHW 로 된것 제대로 지원안해서 이렇게 고칩니다.
	GetEtDevice()->SetFVF( FVF_XYZ | FVF_TEX1 );
	GetEtDevice()->SetWorldTransform( &WorldMat );
	GetEtDevice()->SetViewTransform( &WorldMat );
	EtMatrix ProjMat;
	D3DXMatrixOrthoOffCenterLH(&ProjMat, 0, (float)GetEtDevice()->Width(), (float)GetEtDevice()->Height(), 0, 0, 1);
	GetEtDevice()->SetProjTransform( &ProjMat);

	if( m_QuadTex2DStream.GetVertexCount() )
	{
		for( i = 0; i < m_QuadTex2DStream.GetVertexCount() / 6; i++ )
		{
			GetEtDevice()->SetTexture( 0, m_vecQuad2DTexture[ i ] );			
			m_QuadTex2DStream.Draw( -1, i * 2, 2 );
		}
	}

	GetEtDevice()->SetWorldTransform( &WorldMat );
	if( hActiveCamera ) {
		GetEtDevice()->SetViewTransform( hActiveCamera->GetViewMat() );
		GetEtDevice()->SetProjTransform( hActiveCamera->GetProjMat() );
	}

	GetEtDevice()->SetFVF( FVF_XYZ | FVF_DIFFUSE );
	GetEtDevice()->SetTexture( 0, CEtTexture::GetWhiteTexture()->GetTexturePtr() );

	if( m_Line3DStream.GetVertexCount() )
	{
		m_Line3DStream.Draw( -1 );
	}
	if( m_Point3DStream.GetVertexCount() )
	{
		float fPointSize = 5.f;
		GetEtDevice()->SetRenderState( D3DRS_POINTSIZE, *( DWORD * )&fPointSize );
		GetEtDevice()->SetRenderState( D3DRS_ZENABLE, FALSE );
		m_Point3DStream.Draw( -1 );
		GetEtDevice()->SetRenderState( D3DRS_ZENABLE, TRUE );
	}
	if( m_Point3DApplyZStream.GetVertexCount() )
	{
		float fPointSize = 5.f;
		GetEtDevice()->SetRenderState( D3DRS_POINTSIZE, *( DWORD * )&fPointSize );
		m_Point3DApplyZStream.Draw( -1 );
	}
	if( m_Triangle3DStream.GetVertexCount() )
	{
		m_Triangle3DStream.Draw( -1 );
	}

	GetEtDevice()->SetFVF( FVF_XYZRHW | FVF_DIFFUSE );

	if( m_Line2DStream.GetVertexCount() )
	{
		m_Line2DStream.Draw( -1 );
	}
	if( m_Triangle2DStream.GetVertexCount() )
	{
		m_Triangle2DStream.Draw( -1 );
	}

	GetEtDevice()->EnableAlphaBlend( false );
	GetEtDevice()->SetCullMode( CULL_CCW );

	GetEtDevice()->SetFVF( 0 );

	Flush();
}

void CEtPrimitiveDraw::Flush()
{
	m_QuadTex3DStream.MakeEmpty();
	m_vecQuad3DTexture.clear();
	m_QuadTex2DStream.MakeEmpty();
	m_vecQuad2DTexture.clear();
	m_Line3DStream.MakeEmpty();
	m_Point3DStream.MakeEmpty();
	m_Point3DApplyZStream.MakeEmpty();
	m_Triangle3DStream.MakeEmpty();
	m_Line2DStream.MakeEmpty();
	m_Triangle2DStream.MakeEmpty();
}
