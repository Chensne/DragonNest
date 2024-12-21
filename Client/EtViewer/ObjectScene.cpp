#include "StdAfx.h"
#include "ObjectScene.h"
#include "RenderBase.h"

CObjectScene::CObjectScene()
{
	m_Type = CObjectBase::SCENE;
}

CObjectScene::~CObjectScene()
{
}

void CObjectScene::Process( LOCAL_TIME LocalTime )
{
	if( m_bShow == false ) return;
	CObjectBase::Process( LocalTime );
}

void CObjectScene::Show( bool bShow )
{
	CObjectBase::Show( bShow );

	for( DWORD i=0; i<m_pVecChild.size(); i++ ) {
		m_pVecChild[i]->Show( bShow );
	}
}

void CObjectScene::Activate()
{
	InitPropertyInfo();
}

void CObjectScene::InitPropertyInfo()
{
	SAFE_DELETE_PVEC( m_pVecPropertyList );

	PropertyGridBaseDefine Default[] = {
		{ "Common", "Background Color", CUnionValueProperty::Color, "Background Color", TRUE },
		{ "Common", "Grid", CUnionValueProperty::Boolean, "Show/Hide Grid", TRUE },
		{ "Environment", "Texture", CUnionValueProperty::String_FileOpen, "Texture File|All Texture Files|*.dds;*.jpg;*.tga;*.bmp", TRUE },
		{ "Common", "Axis", CUnionValueProperty::Boolean, "Show/Hide Axis", TRUE },
		NULL,
	};

	AddPropertyInfo( Default );
}

void CObjectScene::OnSetPropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			{
				DWORD dwColor = CRenderBase::GetInstance().GetBackgroundColor();
				pVariable->SetVariable( (int)RGB( GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor) ) );
			}
			break;
		case 1:
			pVariable->SetVariable( CRenderBase::GetInstance().IsShowGrid() );
			break;
		case 2:
			pVariable->SetVariable( (char*)CRenderBase::GetInstance().GetEnvironmentTexture() );
			break;
		case 3:
			pVariable->SetVariable( CRenderBase::GetInstance().IsShowAxis() );
			break;
	}
}

void CObjectScene::OnChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 0:
			{
				BYTE r = GetRValue(pVariable->GetVariableInt());
				BYTE g = GetGValue(pVariable->GetVariableInt());
				BYTE b = GetBValue(pVariable->GetVariableInt());
				CRenderBase::GetInstance().SetBackgroundColor( D3DCOLOR_ARGB( 255, r, g, b ) );
				EternityEngine::SetCaptureScreenClearColor( D3DCOLOR_ARGB( 255, r, g, b ) );
			}
			break;
		case 1:
			CRenderBase::GetInstance().ShowGrid( pVariable->GetVariableBool() );
			break;
		case 2:
			CRenderBase::GetInstance().SetEnvironmentTexture( pVariable->GetVariableString() );
			break;
		case 3:
			CRenderBase::GetInstance().ShowAxis( pVariable->GetVariableBool() );
			break;
	}
}

void CObjectScene::OnSelectChangePropertyValue( DWORD dwIndex, CUnionValueProperty *pVariable )
{
	switch( dwIndex ) {
		case 1:
			CRenderBase::GetInstance().ShowGrid( pVariable->GetVariableBool() );
			break;
		case 3:
			CRenderBase::GetInstance().ShowAxis( pVariable->GetVariableBool() );
			break;
	}
}

void CObjectScene::ExportObject( FILE *fp, int &nCount )
{
	fwrite( &m_Type, sizeof(int), 1, fp );

	DWORD dwColor = CRenderBase::GetInstance().GetBackgroundColor();
	bool bShowGrid = CRenderBase::GetInstance().IsShowGrid();
	fwrite( &dwColor, sizeof(DWORD), 1, fp );
	fwrite( &bShowGrid, sizeof(bool), 1, fp );

	MatrixEx *pCamera = CRenderBase::GetInstance().GetCameraCross();

	fwrite( pCamera, sizeof(MatrixEx), 1, fp );

	fwrite( &m_bShow, sizeof(bool), 1, fp );

	nCount++;

	CObjectBase::ExportObject( fp, nCount );
}

void CObjectScene::ImportObject( FILE *fp )
{
	DWORD dwColor;
	bool bShowGrid;
	fread( &dwColor, sizeof(DWORD), 1, fp );
	fread( &bShowGrid, sizeof(bool), 1, fp );

	MatrixEx Camera;
	fread( &Camera, sizeof(MatrixEx), 1, fp );

	// ���⼭ �ٷ� Show�ϴ� �ڽı����� �����ش�.
	// �ٵ� �ڽı��� �Ϸ��� �ڽ� �� �ε�� �Ŀ� �ѹ� �� ������ϴ°Ŷ�, �ָ��ϴ�. �켱 ����.
	fread( &m_bShow, sizeof(bool), 1, fp );

	Show( m_bShow );

	CRenderBase::GetInstance().SetCameraCross( &Camera );

	CRenderBase::GetInstance().SetBackgroundColor( dwColor );
	CRenderBase::GetInstance().ShowGrid( bShowGrid );

}