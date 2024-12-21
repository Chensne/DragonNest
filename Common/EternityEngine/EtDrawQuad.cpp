#include "Stdafx.h"
#include "EtDrawQuad.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

void DrawQuad( EtVector2 *pStart, EtVector2 *pEnd, EtVector2 *pTexStart, EtVector2 *pTexEnd, float fZValue )
{
	STextureVertex2D Vertices[ 4 ];
	int nWidth, nHeight;
	EtViewPort Viewport;

	GetEtDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	GetEtDevice()->GetViewport( &Viewport );

	nWidth = Viewport.Width;
	nHeight = Viewport.Height;

	Vertices[ 0 ].Position = EtVector4( pStart->x * nWidth - 0.5f, pStart->y * nHeight - 0.5f, fZValue, 1.0f );
	Vertices[ 0 ].TexCoordinate = EtVector2( pTexStart->x, pTexStart->y );

	Vertices[ 1 ].Position = EtVector4( pEnd->x * nWidth - 0.5f, pStart->y * nHeight - 0.5f, fZValue, 1.0f );
	Vertices[ 1 ].TexCoordinate = EtVector2( pTexEnd->x, pTexStart->y );

	Vertices[ 2 ].Position = EtVector4( pEnd->x * nWidth - 0.5f, pEnd->y * nHeight - 0.5f, fZValue, 1.0f );
	Vertices[ 2 ].TexCoordinate = EtVector2( pTexEnd->x, pTexEnd->y );

	Vertices[ 3 ].Position = EtVector4( pStart->x * nWidth - 0.5f, pEnd->y * nHeight - 0.5f, fZValue, 1.0f );
	Vertices[ 3 ].TexCoordinate = EtVector2( pTexStart->x, pTexEnd->y );

	GetEtDevice()->DrawPrimitiveUP( PT_TRIANGLEFAN, 2, Vertices, sizeof( STextureVertex2D ) );	
}

void DrawQuadWithTex( EtVector2 *pStart, EtVector2 *pEnd, EtVector2 *pTexStart, EtVector2 *pTexEnd, EtBaseTexture *pTexture, float fZValue )
{
	STextureVertex2D Vertices[ 4 ];
	int nWidth, nHeight;
	EtViewPort Viewport;

	GetEtDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	GetEtDevice()->SetPixelShader( NULL );
	GetEtDevice()->SetTexture( 0, pTexture );
	GetEtDevice()->GetViewport( &Viewport );

	nWidth = Viewport.Width;
	nHeight = Viewport.Height;

	Vertices[ 0 ].Position = EtVector4( pStart->x * nWidth - 0.5f, pStart->y * nHeight - 0.5f, fZValue, 1.0f );
	Vertices[ 0 ].TexCoordinate = EtVector2( pTexStart->x, pTexStart->y );

	Vertices[ 1 ].Position = EtVector4( pEnd->x * nWidth - 0.5f, pStart->y * nHeight - 0.5f, fZValue, 1.0f );
	Vertices[ 1 ].TexCoordinate = EtVector2( pTexEnd->x, pTexStart->y );

	Vertices[ 2 ].Position = EtVector4( pEnd->x * nWidth - 0.5f, pEnd->y * nHeight - 0.5f, fZValue, 1.0f );
	Vertices[ 2 ].TexCoordinate = EtVector2( pTexEnd->x, pTexEnd->y );

	Vertices[ 3 ].Position = EtVector4( pStart->x * nWidth - 0.5f, pEnd->y * nHeight - 0.5f, fZValue, 1.0f );
	Vertices[ 3 ].TexCoordinate = EtVector2( pTexStart->x, pTexEnd->y );

	GetEtDevice()->DrawPrimitiveUP( PT_TRIANGLEFAN, 2, Vertices, sizeof( STextureVertex2D ) );	
}

void DrawQuadWithDiffuseTex( EtVector2 *pStart, EtVector2 *pEnd, EtVector2 *pTexStart, EtVector2 *pTexEnd, EtBaseTexture *pTexture, D3DCOLOR Color )
{
	STextureDiffuseVertex2D Vertices[ 4 ];
	int nWidth, nHeight;

	GetEtDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
	GetEtDevice()->SetPixelShader( NULL );
	GetEtDevice()->SetTexture( 0, pTexture );

	nWidth = GetEtDevice()->Width();
	nHeight = GetEtDevice()->Height();

	Vertices[ 0 ].Position = EtVector4( pStart->x * nWidth - 0.5f, pStart->y * nHeight - 0.5f, 0.0f, 1.0f );
	Vertices[ 0 ].TexCoordinate = EtVector2( pTexStart->x, pTexStart->y );
	Vertices[ 0 ].Color = Color;

	Vertices[ 1 ].Position = EtVector4( pEnd->x * nWidth - 0.5f, pStart->y * nHeight - 0.5f, 0.0f, 1.0f );
	Vertices[ 1 ].TexCoordinate = EtVector2( pTexEnd->x, pTexStart->y );
	Vertices[ 1 ].Color = Color;

	Vertices[ 2 ].Position = EtVector4( pEnd->x * nWidth - 0.5f, pEnd->y * nHeight - 0.5f, 0.0f, 1.0f );
	Vertices[ 2 ].TexCoordinate = EtVector2( pTexEnd->x, pTexEnd->y );
	Vertices[ 2 ].Color = Color;

	Vertices[ 3 ].Position = EtVector4( pStart->x * nWidth - 0.5f, pEnd->y * nHeight - 0.5f, 0.0f, 1.0f );
	Vertices[ 3 ].TexCoordinate = EtVector2( pTexStart->x, pTexEnd->y );
	Vertices[ 3 ].Color = Color;

	GetEtDevice()->DrawPrimitiveUP( PT_TRIANGLEFAN, 2, Vertices, sizeof( STextureDiffuseVertex2D ) );
}

CEtCustomMeshStream g_CustomDrawStream;
void CustomDrawIm( EtMaterialHandle hMaterial, std::vector< SCustomParam > &vecCustomParam, int nTechnique,
				  void *pVertexBuffer, int nVertexCount, int nStride, WORD *pIndexBuffer, int nFaceCount )
{
	int nPasses;	
	const bool bLockVB = false;

	CMemoryStream Stream;

	if( bLockVB ) {
		Stream.Initialize( pVertexBuffer, nVertexCount * nStride );
		g_CustomDrawStream.LoadVertexStream( &Stream, nVertexCount, nStride );
		Stream.Initialize( pIndexBuffer, nFaceCount * sizeof( WORD ) * 3 );
		g_CustomDrawStream.LoadIndexStream( &Stream, nFaceCount * 3 );
	}

	hMaterial->SetTechnique( nTechnique );
	hMaterial->SetCustomParamList( vecCustomParam );	
	hMaterial->BeginEffect( nPasses );
	hMaterial->BeginPass( 0 );
	
	int nVertexDeclIndex = hMaterial->GetVertexDeclIndex( nTechnique, 0, true );
	if( nVertexDeclIndex != -1 ) {
		if( bLockVB ) {
			g_CustomDrawStream.Draw( nVertexDeclIndex, 0, nFaceCount );
		}
		else {
			GetEtDevice()->SetVertexDeclaration( nVertexDeclIndex );
			GetEtDevice()->DrawIndexedPrimitiveUP( PT_TRIANGLELIST, 0, nVertexCount, nFaceCount, pIndexBuffer, FMT_INDEX16, pVertexBuffer, nStride );
		}
	}
	else {
		ASSERT( false );
	}
	hMaterial->EndPass();
	hMaterial->EndEffect();

}

void DrawQuadIm( EtMaterialHandle hMaterial, std::vector< SCustomParam > &vecCustomParam, int nTechnique, 
				EtVector2 &Start, EtVector2 &End, EtVector2 &TexStart, EtVector2 &TexEnd )
{
	STextureVertex Vertices[ 4 ];
	WORD Indicies[ 6 ];

	Vertices[ 0 ].Position = EtVector3( ( Start.x - 0.5f ) * 2.0f, ( 0.5f - Start.y ) * 2.0f, 0.0f );
	Vertices[ 0 ].TexCoordinate = EtVector2( TexStart.x, TexStart.y );

	Vertices[ 1 ].Position = EtVector3( ( End.x - 0.5f ) * 2.0f, ( 0.5f - Start.y ) * 2.0f, 0.0f );
	Vertices[ 1 ].TexCoordinate = EtVector2( TexEnd.x, TexStart.y );

	Vertices[ 2 ].Position = EtVector3( ( End.x - 0.5f ) * 2.0f , ( 0.5f - End.y ) * 2.0f, 0.0f );
	Vertices[ 2 ].TexCoordinate = EtVector2( TexEnd.x, TexEnd.y );

	Vertices[ 3 ].Position = EtVector3( ( Start.x - 0.5f ) * 2.0f, ( 0.5f - End.y ) * 2.0f, 0.0f );
	Vertices[ 3 ].TexCoordinate = EtVector2( TexStart.x, TexEnd.y );

	Indicies[ 0 ] = 0;
	Indicies[ 1 ] = 1;
	Indicies[ 2 ] = 2;
	Indicies[ 3 ] = 0;
	Indicies[ 4 ] = 2;
	Indicies[ 5 ] = 3;

	CustomDrawIm( hMaterial, vecCustomParam, nTechnique, Vertices, 4, sizeof( STextureVertex ), Indicies, 2 );
}

void DrawFullScreenQuad( EtBaseTexture *pTexture )
{ 
	DrawQuadWithTex(&EtVector2(0,0), &EtVector2(1,1), &EtVector2(0,0), &EtVector2(1,1), pTexture ); 
}
