#include "StdAfx.h"
#include "EtInstancingMng.h"
#include "EtOptionController.h"
#include "EtMRTMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define MAX_INSTANCING_COUNT 1024

bool NormalRenderInstancingSort( SRenderStackElement *Element1, SRenderStackElement *Element2 )
{
	if( Element1->renderPriority != Element2->renderPriority ) 
	{
		if( Element1->renderPriority < Element2->renderPriority )
			return true;
		else
			return false;
	}
#ifdef PRE_FIX_MATERIAL_DUMP
	else if( Element1->hMaterial && Element2->hMaterial && Element1->hMaterial != Element2->hMaterial )
#else
	else if( Element1->hMaterial != Element2->hMaterial )
#endif
	{
		int nRef1, nRef2;

		nRef1 = Element1->hMaterial->GetRefCount();
		nRef2 = Element2->hMaterial->GetRefCount();
		if( nRef1 > nRef2 )
		{
			return true;
		}
		else if (nRef1 < nRef2 )
		{
			return false;
		}
		else
		{
			return Element1->hMaterial.GetIndex() > Element2->hMaterial.GetIndex();
		}
	}
	else if( Element1->nTechniqueIndex != Element2->nTechniqueIndex )
	{
		if( Element1->nTechniqueIndex < Element2->nTechniqueIndex )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if( Element1->pRenderSubMesh != Element2->pRenderSubMesh )
	{
		if( Element1->pRenderSubMesh < Element2->pRenderSubMesh )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if( Element1->fDirLightAttenuation != Element2->fDirLightAttenuation )
	{
		if( Element1->fDirLightAttenuation < Element2->fDirLightAttenuation )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if( Element1->fDist < Element2->fDist )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

CEtInstancingMng::CEtInstancingMng()
{
	m_bInit = false;
	m_bEnable = false;
	m_pWorldViewMatVB = NULL;
	memset(m_nVertexDecl, 0, sizeof(m_nVertexDecl));
}

CEtInstancingMng::~CEtInstancingMng()
{
	SAFE_RELEASE( m_pWorldViewMatVB );
	SAFE_RELEASE_SPTR( m_hMaterial );
	SAFE_RELEASE_SPTR( m_hSourceMaterial );
}

void CEtInstancingMng::Enable( bool bEnable )
{
	if( bEnable ) {
		int nVSVersion = ( GetEtDevice()->GetVSVersion() & 0xffff ) >> 8;
		int nPSVersion = ( GetEtDevice()->GetPSVersion() & 0xffff ) >> 8;

		if( ( nVSVersion < 3 ) || ( nPSVersion < 3 ) )	 {		// 쉐이더 3.0 이상에서만 인스턴싱이 지원된다.. 
			m_bEnable = false;
			OutputDebug("Gpu does not support instancing.\n");
		}
		else {
			m_bEnable = true;
			/*OutputDebug("Instancing Enable\n");*/
		}
	}
	else {
		m_bEnable = false;
		/*OutputDebug("Instancing Disable\n");*/
	}
}

void CEtInstancingMng::Initialize()
{	
	if( m_bInit ) {
		return;
	}
	m_pWorldViewMatVB = GetEtDevice()->CreateVertexBuffer( sizeof(EtMatrix) * MAX_INSTANCING_COUNT, 0 );

	D3DVERTEXELEMENT9 VertexElement0[] = {	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
																{1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
																{2, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
																{3, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
																{3, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
																{3, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
																{3, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
																D3DDECL_END() };

	D3DVERTEXELEMENT9 VertexElement1[] = {	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
																{1, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
																{2, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
																{2, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
																{2, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
																{2, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
																D3DDECL_END() };

	D3DVERTEXELEMENT9 VertexElement2[] = {	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
																{1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
																{1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
																{1, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
																{1, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
																D3DDECL_END() };

	m_nVertexDecl[ 0 ] = GetEtDevice()->CreateVertexDeclaration( VertexElement0 );
	m_nVertexDecl[ 1 ] = GetEtDevice()->CreateVertexDeclaration( VertexElement1 );
	m_nVertexDecl[ 2 ] = GetEtDevice()->CreateVertexDeclaration( VertexElement2 );

	m_hMaterial = LoadResource( "DiffuseInstancing.fx", RT_SHADER );
	m_hSourceMaterial = LoadResource( "Diffuse.fx", RT_SHADER);
	m_bInit = true;
}

void CEtInstancingMng::ClassifyBlock( std::vector< SRenderStackElement* > &vecBlock, std::vector< SRenderStackElement* > &vecInstancingBlock )
{
	if( !m_bEnable || vecBlock.empty() || !CEtCamera::GetActiveCamera() ) {
		return;
	}

	Initialize();

	std::sort( vecBlock.begin(), vecBlock.end(), NormalRenderInstancingSort );

	int i ;
	ASSERT( vecInstancingBlock.empty() );
	int nDup = 0;
	for( i = 0; i< ( int )vecBlock.size()-1; i++)
	{
		SRenderStackElement *pBlock = vecBlock[ i ];
		SRenderStackElement *pNextBlock = vecBlock[ i + 1 ];
		int nLight = 0;
		for( nLight = 0; nLight < MAX_POINT_LIGHT_COUNT + MAX_SPOT_LIGHT_COUNT; nLight++) {
			if( pBlock->pInfluenceLight[ nLight ] == NULL && pNextBlock->pInfluenceLight[ nLight ] == NULL ) {
				nLight = MAX_POINT_LIGHT_COUNT + MAX_SPOT_LIGHT_COUNT;
				break;
			}
			if( pBlock->pInfluenceLight[ nLight ] != pNextBlock->pInfluenceLight[ nLight ] ) {
				break;
			}
		}
		bool bSameLight = ( nLight == MAX_POINT_LIGHT_COUNT + MAX_SPOT_LIGHT_COUNT );

		if( bSameLight && 
			pBlock->hMaterial == m_hSourceMaterial && 
			pBlock->nSaveMatIndex == -1 && 			
			pBlock->pRenderMeshStream == NULL && 
			pBlock->pRenderSubMesh == pNextBlock->pRenderSubMesh && 
			pBlock->fDirLightAttenuation == pNextBlock->fDirLightAttenuation &&
			pBlock->nTechniqueIndex == pNextBlock->nTechniqueIndex ) {
				nDup++;
				vecInstancingBlock.push_back( pBlock );

				bool bLastElement = (i == vecBlock.size() - 2);
				if( bLastElement ) {
					pNextBlock->bLastElement = true;
					vecInstancingBlock.push_back( pNextBlock );
					vecBlock.erase( vecBlock.begin() + i );
				}
				vecBlock.erase( vecBlock.begin() + i );
				i--;
		}
		else {
			if( nDup != 0 ) {

				vecBlock[i]->bLastElement = true;
				vecInstancingBlock.push_back( vecBlock[i] );
				vecBlock.erase( vecBlock.begin() + i );
				i--;
			}
			nDup = 0;
		}
	}

	if( vecInstancingBlock.empty() ) {
		return;
	}

	ASSERT( vecInstancingBlock.size() <= MAX_INSTANCING_COUNT );

	EtMatrix *pLockedPtr = NULL;
	m_pWorldViewMatVB->Lock( 0, sizeof(EtMatrix)*(int)vecInstancingBlock.size(), (void**)&pLockedPtr, 0);	
	int nSize = (int)vecInstancingBlock.size();
	for( i = 0; i < nSize; i++) {
		EtMatrixMultiply( &pLockedPtr[ i ], &vecInstancingBlock[ i ]->WorldMat, CEtCamera::GetActiveCamera()->GetViewMat() );
	}
	m_pWorldViewMatVB->Unlock();
}

void CEtInstancingMng::RenderDepth( std::vector< SRenderStackElement* > &vecBlock )
{
	if( !m_bEnable || vecBlock.empty() ) {
		return;
	}
	if( !CEtRenderStack::IsDepthRender() ) {
		return;
	}

	Initialize();
	
	int i, nPasses;
	int nDup = 0;
	int nStartIndex = 0;
	
	EtMatrix IdentMat;
	EtMatrixIdentity( &IdentMat );

	CEtMRTMng::GetInstance().BeginDepthTarget();
	for( i = 0; i < ( int )vecBlock.size(); i++ )
	{		
		nDup++;
		SRenderStackElement *pCurBlock = vecBlock[ i ];
		if( !pCurBlock->bLastElement ) {
			continue;
		}
		ASSERT( pCurBlock->nBakeDepthIndex == DT_NORMAL || pCurBlock->nBakeDepthIndex == DT_OPAQUE || pCurBlock->nBakeDepthIndex == DT_VOLUME );
		int nTechnique = (pCurBlock->nBakeDepthIndex == DT_OPAQUE ) ? 2 : 1;

		m_hMaterial->SetTechnique( nTechnique );
		m_hMaterial->BeginEffect( nPasses );
		m_hMaterial->BeginPass( 0 );
		m_hMaterial->SetGlobalParams();

		m_hMaterial->SetWorldMatParams( &IdentMat, &IdentMat );
		if( pCurBlock->pvecCustomParam ) {
			m_hMaterial->SetCustomParamList( *pCurBlock->pvecCustomParam );
		}
		m_hMaterial->CommitChanges();

		pCurBlock->pRenderSubMesh->GetMeshStream()->DrawInstancing( m_nVertexDecl[ nTechnique ] , m_pWorldViewMatVB, nStartIndex, nDup );
		nStartIndex += nDup;
		nDup = 0;

		m_hMaterial->EndPass();
		m_hMaterial->EndEffect();
	}
	CEtMRTMng::GetInstance().EndDepthTarget();
}

void CEtInstancingMng::Render( std::vector< SRenderStackElement* > &vecBlock )
{
	if( !m_bEnable || vecBlock.empty() ) {
		return;
	}

	Initialize();	

	int i, nPasses;
	
	EtMatrix IdentMat;
	EtMatrixIdentity( &IdentMat );

	m_hMaterial->SetTechnique( 0 );
	m_hMaterial->BeginEffect( nPasses );
	m_hMaterial->BeginPass( 0 );	
	m_hMaterial->SetGlobalParams();
	
	int nDup = 0;
	int nStartIndex = 0;
	for( i = 0; i< ( int )vecBlock.size(); i++) {
		nDup++;
		SRenderStackElement *pCurBlock = vecBlock[ i ];
		if( !pCurBlock->bLastElement ) {
			continue;
		}
		CEtLight::SetInfluenceLight( pCurBlock->pInfluenceLight );
		CEtLight::SetDirLightAttenuation( pCurBlock->fDirLightAttenuation );		
		
		m_hMaterial->SetWorldMatParams( &IdentMat, &IdentMat );
		if( pCurBlock->pvecCustomParam ) {
			m_hMaterial->SetCustomParamList( *pCurBlock->pvecCustomParam );
		}
		m_hMaterial->CommitChanges();
		pCurBlock->pRenderSubMesh->GetMeshStream()->DrawInstancing( m_nVertexDecl[ 0 ], m_pWorldViewMatVB, nStartIndex, nDup );
		nStartIndex += nDup;
		nDup = 0;
	}
	m_hMaterial->EndPass();
	m_hMaterial->EndEffect();

	vecBlock.clear();
}
