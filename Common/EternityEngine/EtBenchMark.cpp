#include "StdAfx.h"
#include "EtBenchMark.h"
#include "EtResource.h"
#include "EtType.h"
#include "EtLoader.h"
#include "EtDrawQuad.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CEtBenchMark::CEtBenchMark()
{
	m_PerfGrade = PF_Unknown;
	m_dwScore = 0;
	m_bRun = false;
}

CEtBenchMark::~CEtBenchMark()
{
}

void CEtBenchMark::Reset()
{
	m_PerfGrade = PF_Unknown;
	m_dwScore = 0;
}

void CEtBenchMark::Process()
{
	if( !m_bRun ) return;
	m_bRun = false;
	EtTextureHandle hRTTexture = CEtTexture::CreateRenderTargetTexture( 1024, 1024, FMT_A8R8G8B8 );
	EtDepthHandle hDepth = CEtDepth::CreateDepthStencil( 1024, 1024);

	EtSurface *pBackupRenderTarget = GetEtDevice()->GetRenderTarget();
	EtSurface *pBackupDepthTarget = GetEtDevice()->GetDepthStencilSurface();

	EtViewPort BackupViewport;
	GetEtDevice()->GetViewport( &BackupViewport );

	GetEtDevice()->SetRenderTarget( hRTTexture->GetSurfaceLevel() );
	GetEtDevice()->SetDepthStencilSurface( hDepth->GetDepthBuffer() );
	GetEtDevice()->EnableZ( true );
	GetEtDevice()->ClearBuffer( 0x0, 1.0f, 0, true, true, false);	
	
	EtViewPort viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = 1024;
	viewport.Height = 1024;
	viewport.MinZ = 0;
	viewport.MaxZ = 1;
	GetEtDevice()->SetViewport( &viewport );

	// Create Vertex Buffer
	int nVertexStride = sizeof(EtVector3)+sizeof(EtVector2);
	const int nSliceCount = 8000;	

	EtVertexBuffer *pVertexBuffer = GetEtDevice()->CreateVertexBuffer( (nSliceCount+1) * nVertexStride, 0 );
	BYTE *pVertsPtr = NULL;
	pVertexBuffer->Lock(0, nVertexStride * (nSliceCount+1), (void**)&pVertsPtr, 0);
	*(EtVector3*)(pVertsPtr) = EtVector3(0,0,0);
	*(EtVector2*)(pVertsPtr+sizeof(EtVector3)) = EtVector2(0.5,0.5);
	for( int i = 0; i < nSliceCount; i++) {
		*(EtVector3*)(pVertsPtr + (i+1) * nVertexStride) = EtVector3( cosf( 2.f * ET_PI * (1.0f - float(i) / (nSliceCount-1)) ), sinf( 2.f * ET_PI * (1.0f - float(i) / (nSliceCount-1)) ), 0);
		*(EtVector2*)(pVertsPtr + (i+1) * nVertexStride + sizeof(EtVector3)) = EtVector2( cosf( 2.f * ET_PI * i / nSliceCount ) * 0.5f + 0.5f, sinf( 2.f * ET_PI * i / nSliceCount ) *0.5f + 0.5f) ;
	}
	pVertexBuffer->Unlock();

	// Create Vertex Declaratioin
	D3DVERTEXELEMENT9 VertexElement[] = {	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END() };
	int nVertexDecl = GetEtDevice()->CreateVertexDeclaration( VertexElement );

	// Create Texture
	EtTextureHandle hTexture[ 4 ];
	int a=255, r=0, g=0, b=0;
	for( int i = 0; i < 4; i++) {
		hTexture[ i ] = CEtTexture::CreateNormalTexture( 1024, 1024, FMT_A8R8G8B8, USAGE_DEFAULT, POOL_MANAGED);
		int nTexturePitch = 0;
		DWORD *pPtr = (DWORD*)hTexture[ i ]->Lock( nTexturePitch, true );		
		for( int y = 0; y < 1024; y++) {
			for( int x = 0; x < 1024; x++) {
				*(DWORD*)((((BYTE*)pPtr) + y * nTexturePitch)+x*4) = D3DCOLOR_ARGB(a, r, g, b);
				if(r<255) r++;
				else if(g<255) g++;
				else if(b<255) b++;
				else { r=g=b=0; }
			}
		}
		hTexture[ i ]->Unlock();
	}

	// Load Material
	EtMaterialHandle hMaterial = LoadResource( "BenchMark.fx", RT_SHADER );

	EtMatrix ScaleMat, RotateMat, PositionMat;
	std::vector< SCustomParam >	vecCustomParam;
	AddCustomParam( vecCustomParam, EPT_MATRIX_PTR, hMaterial, "g_ScaleMat", &ScaleMat);
	AddCustomParam( vecCustomParam, EPT_MATRIX_PTR, hMaterial, "g_RotateMat", &RotateMat);
	AddCustomParam( vecCustomParam, EPT_MATRIX_PTR, hMaterial, "g_PositionMat", &PositionMat);

	int nTexIndex = -1;
	nTexIndex = hTexture[0]->GetMyIndex();
	AddCustomParam( vecCustomParam, EPT_TEX, hMaterial, "g_LayerTex1", &nTexIndex);
	nTexIndex = hTexture[1]->GetMyIndex();
	AddCustomParam( vecCustomParam, EPT_TEX, hMaterial, "g_LayerTex2", &nTexIndex);
	nTexIndex = hTexture[2]->GetMyIndex();
	AddCustomParam( vecCustomParam, EPT_TEX, hMaterial, "g_LayerTex3", &nTexIndex);
	nTexIndex = hTexture[3]->GetMyIndex();
	AddCustomParam( vecCustomParam, EPT_TEX, hMaterial, "g_LayerTex4", &nTexIndex);

	LARGE_INTEGER liFreq;
	LARGE_INTEGER liStartTime, liEndTime;

	QueryPerformanceFrequency( &liFreq);
	CEtOcclusionQuery Query;
	Query.Begin();
	GetEtDevice()->BeginScene();
	for( int i = 0; i < 500; i++)  {
		if( i == 1 ) {
			Query.End();
			DWORD dwResult;
			Query.GetResult( dwResult );
			QueryPerformanceCounter( &liStartTime);
			Query.Begin();
		}
		EtMatrixScaling(&ScaleMat, 0.4f+i*0.001f, 0.5f-i*0.001f, 1.0f);
		EtMatrixRotationZ(&RotateMat, D3DX_PI*(0.25f + i * 0.123f));
		EtMatrixTranslation(&PositionMat, 0.5f-0.01f*i, -0.5f+0.01f*i, 0.0f);
		hMaterial->SetTechnique( 0 );
		hMaterial->BeginEffect();
		hMaterial->BeginPass( 0 );
		hMaterial->SetCustomParamList( vecCustomParam );
		hMaterial->CommitChanges();	
//		GetEtDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		GetEtDevice()->SetVertexDeclaration( nVertexDecl );
		GetEtDevice()->SetStreamSource( 0, pVertexBuffer, nVertexStride );
		GetEtDevice()->DrawPrimitive( PT_TRIANGLEFAN, 0, nSliceCount-1);
//		GetEtDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		hMaterial->EndPass();
		hMaterial->EndEffect();
	}
	GetEtDevice()->EndScene();
	Query.End();
	DWORD dwResult;
	Query.GetResult( dwResult );			// scene is guaranteed to have completed rendering.
	QueryPerformanceCounter( &liEndTime);
	DWORD dwMilliSec = (DWORD)( 1000LL * (liEndTime.QuadPart - liStartTime.QuadPart) / liFreq.QuadPart);
	m_dwScore = 100000 / dwMilliSec;

	OutputDebug(" BenchMark Score = %d \n",  m_dwScore);

	// end timing
	GetEtDevice()->SetViewport( &BackupViewport );
	GetEtDevice()->SetRenderTarget( pBackupRenderTarget );
	GetEtDevice()->SetDepthStencilSurface( pBackupDepthTarget );


	SAFE_RELEASE( pVertexBuffer );
	SAFE_RELEASE_SPTR( hTexture[0] );
	SAFE_RELEASE_SPTR( hTexture[1] );
	SAFE_RELEASE_SPTR( hTexture[2] );
	SAFE_RELEASE_SPTR( hTexture[3] );
	SAFE_RELEASE_SPTR( hRTTexture );
	SAFE_RELEASE_SPTR( hDepth );
	SAFE_RELEASE_SPTR( hMaterial );
}

void CEtBenchMark::Run()
{
	m_bRun = true;
}

CEtBenchMark::PerfGrade CEtBenchMark::GetResult( DWORD *pScore )
{
	if( pScore ) {
		*pScore = m_dwScore;
	}
	return m_PerfGrade;
}
