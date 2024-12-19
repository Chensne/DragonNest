#include "StdAfx.h"
#include "EtTransitionFilter.h"
#include "EtBackBufferMng.h"
#include "EtDrawQuad.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtTransitionFilter::CEtTransitionFilter()
{
	m_Type = SF_TRANSITION;
	m_nBackBufferIndex = -1;
	m_bCaptured = false;	
	m_fElapsedTime = FLT_MAX;
}

CEtTransitionFilter::~CEtTransitionFilter()
{
	Clear();	
}

void CEtTransitionFilter::Clear()
{
	CEtPostProcessFilter::Clear();
	if( m_nBackBufferIndex != -1 ) {
		GetEtBackBufferMng()->ReleaseUserBackBuffer( m_nBackBufferIndex );
		m_nBackBufferIndex = -1;
	}
}

void CEtTransitionFilter::Initialize()
{	
}

void CEtTransitionFilter::Render( float fElapsedTime )
{
	if( m_bCaptured ) {
		if( m_nBackBufferIndex == -1 ) {
			m_nBackBufferIndex = GetEtBackBufferMng()->CreateUserBackBuffer();
		}
		GetEtBackBufferMng()->DumpUserBackBuffer( m_nBackBufferIndex );
		m_bCaptured = false;
		m_fElapsedTime = 0.f;		
	}

	GetEtDevice()->SetVertexShader( NULL );	// Use Fixed Pipeline
	bool bAlphaBlend = GetEtDevice()->EnableAlphaBlend( true );

	DWORD dwWidth = GetEtDevice()->Width();
	DWORD dwHeight = GetEtDevice()->Height();

	static int s_nType = 0;
	switch( s_nType )
	{
	case 0:
		{
			float t = m_fElapsedTime / 0.7f;
			if( t > 1.f ) break;
			BYTE btAlpha = 255 - (BYTE)(min(255, 255 * (t)));
			DrawQuadWithDiffuseTex( &EtVector2( 0, 0 ), &EtVector2( 1, 1 ), &EtVector2( 0, 0 ), &EtVector2( 1, 1 ), 
				GetEtBackBufferMng()->GetUserBackBuffer( m_nBackBufferIndex)->GetTexturePtr() , D3DCOLOR_ARGB(btAlpha, 255, 255, 255) );		
		}
		break;
	case 1:
		{/*
			static const int segmentH = 16;
			static const int segmentV = 12;

			unsigned short indices[ (segmentH*(segmentV*2-1))*3 ];
			int index=0;
			for( int i = 0; i < segmentH; i++) {
				indices[index++] = 0;		
				indices[index++] = (i+1)%segmentH+1;
				indices[index++] = i+1;
			}

			for( int j = 1; j < segmentV; j++) 
				for( int i = 0; i < segmentH; i++) {
					int i0 = (j-1)*segmentH;
					int i1 = (j)*segmentH;
					int h0 = i;
					int h1 = (i+1)%segmentH;
					indices[index++] = h0+i0+1;		
					indices[index++] = h1+i1+1;
					indices[index++] = h0+i1+1;

					indices[index++] = h0+i0+1;		
					indices[index++] = h1+i0+1;
					indices[index++] = h1+i1+1;
				}

				float t = m_fElapsedTime / 3.f;
				if(t > 1) break;

				EtVector3 center = D3DXVECTOR3(0,0,0);
				t = GainCurve(t, 0.85f);	
				t = BiasCurve(t, 0.7f);
				float fRadius =  max(0.f, t *2*8.0f-0.5f);
				int vertsCount = (1 + segmentH * segmentV);
				int primCount = (segmentH*(segmentV*2-1));

				STextureDiffuseVertex2D pos[(1 + segmentH * segmentV)];

				pos[0].Position = EtVector4(center.x, center.y, center.z, 1.f);
				pos[0].Color = D3DCOLOR_ARGB(0,255,255,255);

				const float radiusSeg = fRadius / segmentV;
				for( int j = 0; j < segmentV; j++) {
					float radius = (j + 1) * radiusSeg;
					if( j == segmentV-1 ) radius += 5.f;
					float t = (float)(segmentV-j) / (segmentV-1);
					t = SCurve(t);
					t = BiasCurve(t, 0.2f);
					t = GainCurve(t, 0.7f);
					int alpha = min(255, max(0, 255-(int)(255*t) - j * 5));
					int col = 255-(int)(100*t);
					assert(alpha<=255&&alpha>=0);
					D3DCOLOR color = D3DCOLOR_ARGB(alpha,col,col,col);
					for( int i = 0; i < segmentH; i++) {
						float ang = D3DX_PI * 2.0f * (i+0.5f) / segmentH;
						pos[1 + i + j*segmentH].Position.x = cosf(ang)*radius + center.x;
						pos[1 + i + j*segmentH].Position.y = sinf(ang)*radius + center.y;

						pos[1 + i + j*segmentH].Position.z = center.z;
						pos[1 + i + j*segmentH].Position.w = 1.f;
						pos[1 + i + j*segmentH].Color = color;
					}
				}
				for( int i = 0; i < vertsCount; i++) {
					pos[i].TexCoordinate.x = pos[i].Position.x*0.5f+0.5f + (0.5f/dwWidth);
					pos[i].TexCoordinate.y = (-pos[i].Position.y*0.5f+0.5f) + (0.5f/dwHeight);
					pos[i].Position.x = (pos[i].Position.x*0.5f+0.5f) * dwWidth;
					pos[i].Position.y = (pos[i].Position.y*-0.5f+0.5f) * dwHeight;
				}


				GetEtDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
				GetEtDevice()->SetVertexShader( NULL );
				GetEtDevice()->SetPixelShader( NULL );
				GetEtDevice()->SetTexture( 0, GetEtBackBufferMng()->GetUserBackBuffer( m_nBackBufferIndex)->GetTexturePtr() );
				GetEtDevice()->EnableLight( false );
				GetEtDevice()->DrawIndexedPrimitiveUP( PT_TRIANGLELIST, 0, vertsCount, primCount, indices, FMT_INDEX16,
					pos, sizeof(STextureDiffuseVertex2D) );
					*/
		}
		break;
	}

	m_fElapsedTime += fElapsedTime;

	GetEtDevice()->EnableAlphaBlend( bAlphaBlend  );
}