#include "StdAfx.h"
#include "ObjectSubmesh.h"

CObjectSubmesh::CObjectSubmesh()
{
	m_Type = CObjectBase::SUBMESH;
	m_dwVertexCount = 0;
	m_nSubMeshIndex = -1;
	m_nTextureCount = 0;
}

CObjectSubmesh::~CObjectSubmesh()
{
}


void CObjectSubmesh::Activate()
{
	EtAniObjectHandle Handle = ((CObjectSkin*)GetParent())->GetObjectHandle();
	if( !Handle ) return;

	CEtSubMesh *pSubmesh = NULL;
	EtMaterialHandle hMaterial;
	for( int i=0; i<Handle->GetSubMeshCount(); i++ ) {
		if( strcmp( Handle->GetSubMeshName( i ), m_szName ) == NULL ) {
			pSubmesh = Handle->GetSubMesh( i );
			hMaterial = Handle->GetMaterial( i );
			m_nSubMeshIndex = i;
			break;
		}
	}
	if( pSubmesh == NULL ) return;

	m_szEffectName = hMaterial->GetFileName();
	m_dwVertexCount = (int)pSubmesh->GetVertexCount();

	m_nTextureCount = 0;
	m_VecEffectList.clear();
	m_VecEffectSemanticList.clear();
	for( int i=0; i<Handle->GetEffectCount( m_nSubMeshIndex ); i++ ) {
		SCustomParam *pParam = Handle->GetCustomParam( m_nSubMeshIndex, i );
		if( pParam->Type == EPT_MATRIX || pParam->Type == EPT_VARIABLE ) continue;

		if( !pParam->hParamHandle || pParam->hParamHandle == -1) continue;
		m_VecEffectList.push_back( pParam );
		m_VecEffectSemanticList.push_back( hMaterial->GetSemantic( pParam->hParamHandle ) );
		if( pParam->Type == EPT_TEX ) m_nTextureCount++;
	}

	CObjectBase::Activate();
}

void CObjectSubmesh::InitPropertyInfo()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );

	PropertyGridBaseDefine Default[] = {
		{ "Common", "Polygon Count", CUnionValueProperty::Integer, "Polygon Count", FALSE },
		{ "Material", "Effect Name", CUnionValueProperty::String_FileOpen, "Select FX Shader File|HLSL Files|*.fx;*.fxc", FALSE },
		{ "Material", "Enable AlphaBlend", CUnionValueProperty::Boolean, "Enable Alpha Blend", TRUE },
		{ "Material", "Alpha Value", CUnionValueProperty::Float, "Alpha Value|0.f|1.f|0.001f", TRUE },
		{ "Texture", "Using Count", CUnionValueProperty::Integer, "Texture Count", FALSE },
		NULL,
	};

	AddPropertyInfo( Default );

	CUnionValueProperty *pVariable;

	for( DWORD i=0; i<m_VecEffectList.size(); i++ ) {
		if( m_VecEffectList[i]->Type == EPT_TEX ) {
			pVariable = new CUnionValueProperty( CUnionValueProperty::String_FileOpen );
			pVariable->SetCategory( "Texture" );
			pVariable->SetDescription( m_VecEffectSemanticList[i] );
			pVariable->SetSubDescription( "Texture File|All Texture Files|*.dds;*.jpg;*.tga;*.bmp" );
			pVariable->SetDefaultEnable( TRUE );
		}
		else {
			switch( m_VecEffectList[i]->Type ) {
				case EPT_INT:
					pVariable = new CUnionValueProperty( CUnionValueProperty::Integer );
					break;
				case EPT_VECTOR:
					pVariable = new CUnionValueProperty( CUnionValueProperty::Vector4Color );
					break;
				case EPT_FLOAT:
					pVariable = new CUnionValueProperty( CUnionValueProperty::Float );
					break;
			}
			pVariable->SetCategory( "Shader Parameter" );
			pVariable->SetDescription( m_VecEffectSemanticList[i] );
			pVariable->SetSubDescription( "Shader Setting Value" );
			pVariable->SetDefaultEnable( TRUE );
		}

		m_pVecPropertyList.push_back( pVariable );
	}
}

void CObjectSubmesh::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			pVariable->SetVariable( (int)m_dwVertexCount );
			break;
		case 1:
			pVariable->SetVariable( m_szEffectName.GetBuffer() );
			break;
		case 4:
			pVariable->SetVariable( m_nTextureCount );
			break;
		case 2:
			{
				EtAniObjectHandle Handle = ((CObjectSkin*)GetParent())->GetObjectHandle();
				pVariable->SetVariable( Handle->GetSkin()->IsEnableAlphaBlend( m_nSubMeshIndex ) );
			}
			break;
		case 3:
			{
				EtAniObjectHandle Handle = ((CObjectSkin*)GetParent())->GetObjectHandle();
				pVariable->SetVariable( Handle->GetSkin()->GetAlphaValue( m_nSubMeshIndex ) );
			}
			break;
	}

	// Texture Setting
	DWORD nOffset = 5;
	if( dwIndex >= nOffset && dwIndex < nOffset + m_VecEffectList.size() ) {
		SCustomParam *pParam = m_VecEffectList[ dwIndex - nOffset ];
		switch( pParam->Type ) {
			case EPT_INT:
				pVariable->SetVariable( pParam->nInt );
				break;
			case EPT_FLOAT:
				pVariable->SetVariable( pParam->fFloat );
				break;
			case EPT_VECTOR:
				pVariable->SetVariable( (D3DXVECTOR4)pParam->fFloat4 );
				break;
			case EPT_TEX:
				{
					if( pParam->nTextureIndex == -1 ) {
						pVariable->SetVariable( (char*)"Can't Find File" );
						break;
					}

					EtResourceHandle hResource = CEtResource::GetResource( pParam->nTextureIndex );
					if( !hResource ) break;

					pVariable->SetVariable( (char*)hResource->GetFileName() );
				}
				break;
		}
	}

}

void CObjectSubmesh::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 1:
			{
				CString szFullName = pVariable->GetVariableString();

				// 여기서 다시 FX 읽어주구 Refresh 해줘야 한다.
				SetModify( true, "Effect 파일 변경\n" );

				// Property 상에는 이름만 보이게 해준다.
				char szBuf[512] = { 0, };
				_GetFullFileName( szBuf, _countof(szBuf), szFullName );
				pVariable->SetVariable( szBuf );
			}
			break;
		case 2:
			{
				EtAniObjectHandle Handle = ((CObjectSkin*)GetParent())->GetObjectHandle();
				Handle->GetSkin()->EnableAlphaBlend( m_nSubMeshIndex, pVariable->GetVariableBool() );
				SetModify( true, "Material - Alpha Flag\n" );
			}
			break;
		case 3:
			{
				EtAniObjectHandle Handle = ((CObjectSkin*)GetParent())->GetObjectHandle();
				Handle->GetSkin()->SetAlphaValue( m_nSubMeshIndex, pVariable->GetVariableFloat() );
				SetModify( true, "Material - Alpha Value\n" );
			}
			break;
	}

	DWORD nOffset = 5;
	if( dwIndex >= nOffset && dwIndex < nOffset + m_VecEffectList.size() ) {
		EtAniObjectHandle Handle = ((CObjectSkin*)GetParent())->GetObjectHandle();
		SCustomParam *pParam = m_VecEffectList[ dwIndex - nOffset ];
		switch( pParam->Type ) {
			case EPT_INT:
				pParam->nInt = pVariable->GetVariableInt();
				Handle->SetEffect( m_nSubMeshIndex, dwIndex - nOffset, pParam );
				break;
			case EPT_FLOAT:
				pParam->fFloat = pVariable->GetVariableFloat();
				Handle->SetEffect( m_nSubMeshIndex, dwIndex - nOffset, pParam );
				break;
			case EPT_VECTOR:
				pParam->fFloat4[0] = pVariable->GetVariableVector4Ptr()->x;
				pParam->fFloat4[1] = pVariable->GetVariableVector4Ptr()->y;
				pParam->fFloat4[2] = pVariable->GetVariableVector4Ptr()->z;
				pParam->fFloat4[3] = pVariable->GetVariableVector4Ptr()->w;
				Handle->SetEffect( m_nSubMeshIndex, dwIndex - nOffset, pParam );
				break;
			case EPT_TEX:
				Handle->SetTexture( m_nSubMeshIndex, dwIndex - nOffset, pVariable->GetVariableString() );
				// Property 상에는 이름만 보이게 해준다.
				char szBuf[512] = { 0, };
				_GetFullFileName( szBuf, _countof(szBuf), pVariable->GetVariableString() );
				pVariable->SetVariable( szBuf );
				break;
		}
		CString szTemp;
		szTemp.Format( "%s - 변경\n", pVariable->GetDescription() );
		SetModify( true, szTemp );
	}
	EtAniObjectHandle Handle = ((CObjectSkin*)GetParent())->GetObjectHandle();
	Handle->CreateSkinInstance();
}

void CObjectSubmesh::OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
}
