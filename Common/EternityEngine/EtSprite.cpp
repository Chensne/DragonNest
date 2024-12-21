#include "StdAfx.h"
#include "EtSprite.h"
#include "EtCamera.h"
#include "../EternityEngine/D3DDevice9/EtStateManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtCustomSprite::CEtCustomSprite()
{
	EtMatrixIdentity(&m_WorldMat);
	EtMatrixIdentity(&m_ViewMat);
	EtMatrixIdentity(&m_ProjMat);

	D3DVERTEXELEMENT9 VertexElement[] =  {	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
																{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
																D3DDECL_END() };

	m_nVertexDecl = GetEtDevice()->CreateVertexDeclaration( VertexElement );
	m_dwFlag = 0;
}

void CEtCustomSprite::Begin( DWORD Flags)
{
	EtMatrixIdentity(&m_WorldMat);
	EtMatrixIdentity(&m_ViewMat);
	EtMatrixIdentity(&m_ProjMat);
	m_dwFlag = Flags;
}

void CEtCustomSprite::SetTransform( EtMatrix *pTransform)
{
	m_ProjMat = *pTransform;
}

void CEtCustomSprite::SetWorldViewLH( EtMatrix *pWorld, EtMatrix *pView)
{
	m_WorldMat = *pWorld;
	m_ViewMat = *pView;
}

void CEtCustomSprite::Draw( LPDIRECT3DTEXTURE9 pTexture, CONST RECT *pSrcRect, CONST D3DXVECTOR3 *pCenter, CONST D3DXVECTOR3 *pPosition, D3DCOLOR Color)
{
	RECT Rect = *pSrcRect;
	EtMatrix TransMat = m_ProjMat;
	if( Rect.top == Rect.bottom ) 
	{
		Rect.bottom += (Rect.right-Rect.left);
		TransMat._21 = 0.f;
		TransMat._22 = 1.f;
		TransMat._23 = 0.f;
		TransMat._24 = 0.f;
	}

	LPDIRECT3DDEVICE9 pd3dDevice = (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr();
	EtMatrix BackupProjMat, BackupWorldMat;
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &BackupProjMat );
	pd3dDevice->GetTransform( D3DTS_WORLD, &BackupWorldMat);
	if( m_dwFlag & D3DXSPRITE_OBJECTSPACE ) 
	{
		GetEtDevice()->SetWorldTransform( &TransMat );		
	}
	else
	{		
		EtMatrix OffsetMat;
		EtMatrix ProjMat;
		EtMatrixTranslation(&OffsetMat, -0.5f, -0.5f, 0.f);
		D3DXMatrixOrthoOffCenterLH(&ProjMat, 0.f, (float)GetEtDevice()->Width(), (float)GetEtDevice()->Height(), 0.f, 0.f, 1.f);
		EtMatrixMultiply(&TransMat, &TransMat, &OffsetMat);
		EtMatrixMultiply(&TransMat, &TransMat, &ProjMat);
		EtMatrix MatIdent;
		EtMatrixIdentity(&MatIdent);
		GetEtDevice()->SetWorldTransform( &MatIdent );
		GetEtDevice()->SetViewTransform( &MatIdent );
		GetEtDevice()->SetProjTransform( &TransMat );
	}	
	GetEtDevice()->SetCullMode( CULL_NONE );
	GetEtDevice()->SetVertexShader( NULL );
	GetEtDevice()->SetPixelShader( NULL );
	GetEtDevice()->EnableAlphaBlend( true );
	GetEtDevice()->SetFVF( FVF_XYZ | FVF_TEX1 );
	GetEtDevice()->SetTexture( 0, pTexture );
	GetEtDevice()->SetVertexDeclaration( m_nVertexDecl );
	
	GetEtDevice()->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	GetEtDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

	GetEtDevice()->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	GetEtDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	GetEtDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	GetEtDevice()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

	GetEtDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	GetEtDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

	DWORD dwTextureFilterCaps = GetEtDevice()->GetTextureFilterCaps();

	if( dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) {
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
	}
	else {
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	}
	if( dwTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) {
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	}
	else {
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	}

	if( dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR) {
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	}
	else {
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
	}
	GetEtDevice()->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, GetEtDevice()->GetMaxAnisotropy() );
	GetEtDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	GetEtDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	GetEtDevice()->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, 0);
	
	GetEtDevice()->SetRenderState( D3DRS_TEXTUREFACTOR, Color);

	D3DSURFACE_DESC Desc;
	pTexture->GetLevelDesc( 0, &Desc);

	int nTexWidth = Desc.Width;
	int nTexHeight = Desc.Height;

	float fScaleX = (float)(Rect.right - Rect.left) / nTexWidth;
	float fScaleY = (float)(Rect.bottom - Rect.top ) / nTexHeight;
	STextureVertex Vertices[ 4 ];
	Vertices[ 0 ].Position = EtVector3( 0, 0, 0.f);
	Vertices[ 0 ].TexCoordinate = EtVector2( (float)Rect.left / nTexWidth , (float)Rect.top / nTexHeight );
	Vertices[ 1 ].Position = EtVector3( nTexWidth*fScaleX, 0, 0.f);
	Vertices[ 1 ].TexCoordinate = EtVector2( (float)Rect.right / nTexWidth , (float)Rect.top / nTexHeight );
	Vertices[ 2 ].Position = EtVector3( 0, nTexHeight*fScaleY, 0.f);
	Vertices[ 2 ].TexCoordinate = EtVector2( (float)Rect.left / nTexWidth, (float)Rect.bottom / nTexHeight );
	Vertices[ 3 ].Position = EtVector3( nTexWidth*fScaleX, nTexHeight*fScaleY, 0.f);
	Vertices[ 3 ].TexCoordinate = EtVector2( (float)Rect.right / nTexWidth, (float)Rect.bottom / nTexHeight );

	GetEtDevice()->DrawPrimitiveUP( PT_TRIANGLESTRIP, 2, Vertices, sizeof( STextureVertex ) );

	GetEtDevice()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	GetEtDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	
	GetEtDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	
	GetEtDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );	
	GetEtDevice()->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, 1 );
	GetEtDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	GetEtDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	GetEtDevice()->SetProjTransform( &BackupProjMat );
	GetEtDevice()->SetWorldTransform( &BackupWorldMat );
}
//////////////////////////////////////////////////////////////////////////

CEtSprite::CEtSprite(void)
{
	m_bBegin = false;
	m_pSprite = NULL;	
	m_pCustomSprite = NULL;
	m_pDXSprite = NULL;
}

CEtSprite::~CEtSprite(void)
{
	SAFE_DELETE( m_pCustomSprite );
	SAFE_DELETE( m_pDXSprite );
}

void CEtSprite::Initialize()
{
	m_pCustomSprite = new CEtCustomSprite;
	m_pDXSprite = new CEtDXSprite;
	m_pSprite = m_pDXSprite;	
}

void CEtSprite::SetTransform( EtMatrix &TransMat )
{
	m_pSprite->SetTransform( &TransMat );
}

void CEtSprite::SetWorldViewMat( EtMatrix &WorldMat, EtMatrix &ViewMat )
{
	m_pSprite->SetWorldViewLH( &WorldMat, &ViewMat );
}

void CEtSprite::Begin( DWORD dwFlags )
{
	m_pSprite = m_pDXSprite;//m_pDXSprite;

	if( m_pSprite != m_pCustomSprite &&  GetEtStateManager()->IsEnable() ) {
		GetEtStateManager()->Capture();

		GetEtDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		GetEtDevice()->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
		GetEtDevice()->SetRenderState( D3DRS_ALPHAREF, 0x00 );
		GetEtDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		GetEtDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		GetEtDevice()->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );		
		GetEtDevice()->SetRenderState( D3DRS_SRCBLEND ,D3DBLEND_SRCALPHA );
		GetEtDevice()->SetRenderState( D3DRS_DESTBLEND ,D3DBLEND_INVSRCALPHA );

		GetEtDevice()->SetSamplerState( 0 ,D3DSAMP_ADDRESSU ,D3DTADDRESS_CLAMP );
		GetEtDevice()->SetSamplerState( 0 ,D3DSAMP_ADDRESSV ,D3DTADDRESS_CLAMP );	

		DWORD dwTextureFilterCaps = GetEtDevice()->GetTextureFilterCaps();
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER, ( dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR );	
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER, ( dwTextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR );
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MIPFILTER, ( dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR) ? D3DTEXF_LINEAR : D3DTEXF_POINT );
		GetEtDevice()->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, GetEtDevice()->GetMaxAnisotropy() );

		//GetEtStateManager()->FlushDeferedStates();
		m_pSprite->Begin( dwFlags | D3DXSPRITE_DONOTSAVESTATE );
	}
	else {
		m_pSprite->Begin( dwFlags );
	}
	m_bBegin = true;
}

void CEtSprite::End()
{
	m_pSprite->End();

	if( m_pSprite != m_pCustomSprite && GetEtStateManager()->IsEnable() ) {
		GetEtStateManager()->Restore();
	}

	m_bBegin = false;
}

void CEtSprite::Flush()
{
	m_pSprite->Flush();
}

void CEtSprite::DrawSprite( EtTexture *pTexture, int nTexWidth, int nTexHeight, SUICoord &UVCoord, DWORD dwColor, SUICoord &ScreenCoord, float fRotate, float fZValue, float fShear )
{
	if( fZValue > 1.0f || fZValue < 0.f ) { 
		return;
	}

	bool bInternalBegin = false;
	if( m_bBegin == false )
	{
		Begin( 0 );
		bInternalBegin = true;
	}

	RECT Rect;
	float fScaleX, fScaleY;
	EtVector3 vPosition;
	EtMatrix ScaleMat, TransMat;
	float fSpriteSizeX, fSpriteSizeY;

	fSpriteSizeX = ( UVCoord.fX + UVCoord.fWidth ) * nTexWidth;
	fSpriteSizeY = ( UVCoord.fY + UVCoord.fHeight ) * nTexHeight;
	SetRect( &Rect, ( int )(floorf( UVCoord.fX * nTexWidth + 0.5f )), ( int )(floorf( UVCoord.fY * nTexHeight + 0.5f )), ( int )(floorf(fSpriteSizeX+0.5f)), ( int )(floorf(fSpriteSizeY+0.5f)) );

	fScaleX = ScreenCoord.fWidth * GetEtDevice()->Width() / ( UVCoord.fWidth * nTexWidth );
	fScaleY = ScreenCoord.fHeight * GetEtDevice()->Height() / ( UVCoord.fHeight * nTexHeight );	

	EtMatrixScaling( &ScaleMat, fScaleX, fScaleY, 1.0f );
	EtMatrixTranslation( &TransMat, ScreenCoord.fX * GetEtDevice()->Width(), ScreenCoord.fY * GetEtDevice()->Height(), fZValue );
	EtMatrixMultiply( &TransMat, &ScaleMat, &TransMat );	

	if( fShear != 0.0f ) {
		EtMatrix ShearMat;
		EtMatrixIdentity(&ShearMat);
		ShearMat._21 = -fShear * fScaleY / fScaleX;
		EtMatrixMultiply( &TransMat, &ShearMat, &TransMat );
	}

	if( fRotate != 0.0f )
	{
		EtMatrix MoveOffsetMat, RotateMat;
		float fSpriteCenterX, fSpriteCenterY;

		fSpriteCenterX = UVCoord.fWidth * nTexWidth * 0.5f;
		fSpriteCenterY = UVCoord.fHeight * nTexHeight * 0.5f;
		EtMatrixTranslation( &MoveOffsetMat, -fSpriteCenterX, -fSpriteCenterY, 0.0f );
		EtMatrixRotationZ( &RotateMat, EtToRadian( fRotate ) );
		EtMatrixMultiply( &RotateMat, &MoveOffsetMat, &RotateMat );
		EtMatrixTranslation( &MoveOffsetMat, fSpriteCenterX, fSpriteCenterY, 0.0f );
		EtMatrixMultiply( &RotateMat, &RotateMat, &MoveOffsetMat );
		EtMatrixMultiply( &TransMat, &RotateMat, &TransMat );
	}

	/*EtViewPort viewPort;
	GetEtDevice()->GetViewport( &viewPort );

	float fViewportScaleX = (float)GetEtDevice()->Width() / viewPort.Width;
	float fViewportScaleY = (float)GetEtDevice()->Height() / viewPort.Height;

	D3DXMATRIX viewPortMat( fViewportScaleX	, 0.f, 0.f, 0.f,
							0.f, fViewportScaleY, 0.f, 0.f,
							0.f, 0.f, 1.f, 0.f,
							-fViewportScaleX * viewPort.X, 
							-fViewportScaleY * viewPort.Y, 0.f, 1.f );
	
	EtMatrixMultiply(&TransMat, &TransMat, &viewPortMat);*/
	m_pSprite->SetTransform( &TransMat );
	Draw( pTexture, &Rect, NULL, NULL, dwColor );
	
	if( bInternalBegin )
	{
		End();
	}
}

void CEtSprite::Draw( LPDIRECT3DTEXTURE9 pTexture, CONST RECT *pSrcRect, CONST D3DXVECTOR3 *pCenter, CONST D3DXVECTOR3 *pPosition, D3DCOLOR Color)
{
	GetEtStateManager()->FlushDeferedStates();
	m_pSprite->Draw( pTexture, pSrcRect, pCenter, pPosition, Color );
}

void CEtSprite::DrawSprite3D( EtTexture *pTexture, int nTexWidth, int nTexHeight, DWORD dwColor, EtVector2 &vSize, EtVector3 &vPosition, float fRotate, SUICoord *pCoord )
{
	EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
	if( !hCamera ) return;
	EtMatrix *pViewMat = hCamera->GetViewMat();
	EtMatrix *pProjMat = hCamera->GetProjMat();
	EtVector3 vUpLeft, vDownRight;
	float fTransZ;

	EtVec3TransformCoord( &vUpLeft, &vPosition, pViewMat );
	vDownRight = vUpLeft;
	vUpLeft.x -= vSize.x * 0.5f;
	vUpLeft.y += vSize.y * 0.5f;
	vDownRight.x += vSize.x * 0.5f;
	vDownRight.y -= vSize.y * 0.5f;
	EtVec3TransformCoord( &vUpLeft, &vUpLeft, pProjMat );
	EtVec3TransformCoord( &vDownRight, &vDownRight, pProjMat );
	fTransZ = vUpLeft.z;

	SUICoord UVCoord, ScreenCoord;

	if( pCoord ) UVCoord = *pCoord;
	else UVCoord.SetCoord( 0.0f, 0.0f, 1.0f, 1.0f );

	ScreenCoord.SetCoord( ( vUpLeft.x + 1.0f ) * 0.5f, 1.0f - ( vUpLeft.y + 1.0f ) * 0.5f, ( vDownRight.x - vUpLeft.x ) * 0.5f, ( vUpLeft.y - vDownRight.y ) * 0.5f );
	DrawSprite( pTexture, nTexWidth, nTexHeight, UVCoord, dwColor, ScreenCoord, fRotate, fTransZ );
}

void CEtSprite::DrawRect( SUICoord &ScreenCoord, DWORD dwColor, float fZValue )
{
	SUICoord UVCoord;
	UVCoord.SetCoord( 0.0f, 0.0f, 1.0f, 1.0f );
	DrawSprite( (EtTexture*)CEtTexture::GetWhiteTexture()->GetTexturePtr(), 1, 1, UVCoord, dwColor, ScreenCoord, 0.f, fZValue, 0.f );
}

void CEtSprite::OnLostDevice()
{
	if( m_pDXSprite )
	{
		m_pDXSprite->OnLostDevice();
	}
}

void CEtSprite::OnResetDevice()
{
	if( m_pDXSprite )
	{
		m_pDXSprite->OnResetDevice();
	}
}