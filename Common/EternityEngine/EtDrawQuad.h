#pragma once

void DrawQuad( EtVector2 *pStart, EtVector2 *pEnd, EtVector2 *pTexStart, EtVector2 *pTexEnd, float fZValue = 0.f );
void DrawQuadWithTex( EtVector2 *pStart, EtVector2 *pEnd, EtVector2 *pTexStart, EtVector2 *pTexEnd, EtBaseTexture *pTexture, float fZValue = 0.f );
void DrawQuadWithDiffuseTex( EtVector2 *pStart, EtVector2 *pEnd, EtVector2 *pTexStart, EtVector2 *pTexEnd, EtBaseTexture *pTexture, D3DCOLOR Color );
void CustomDrawIm( EtMaterialHandle hMaterial, std::vector< SCustomParam > &vecCustomParam, int nTechnique,
				  void *pVertexBuffer, int nVertexCount, int nStride, WORD *pIndexBuffer, int nFaceCount );
void DrawQuadIm( EtMaterialHandle hMaterial, std::vector< SCustomParam > &vecCustomParam, int nTechnique, 
				EtVector2 &Start, EtVector2 &End, EtVector2 &TexStart, EtVector2 &TexEnd );
void DrawFullScreenQuad( EtBaseTexture *pTexture );
