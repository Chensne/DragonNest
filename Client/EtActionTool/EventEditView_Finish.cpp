// EventEditView_Finish.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "EventEditView_Finish.h"
#include "SignalManager.h"
#include "GlobalValue.h"
#include "UnionValueProperty.h"


// CEventEditView_Finish

IMPLEMENT_DYNCREATE(CEventEditView_Finish, CFormView)

CEventEditView_Finish::CEventEditView_Finish()
	: CFormView(CEventEditView_Finish::IDD)
	, m_bCheckExport(FALSE)
	, m_szExportFileName("")
{

}

CEventEditView_Finish::~CEventEditView_Finish()
{
}

void CEventEditView_Finish::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bCheckExport);
	DDX_Text(pDX, IDC_EDIT1, m_szExportFileName);
}

BEGIN_MESSAGE_MAP(CEventEditView_Finish, CFormView)
	ON_BN_CLICKED(IDC_CHECK1, &CEventEditView_Finish::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON1, &CEventEditView_Finish::OnBnClickedButton1)
END_MESSAGE_MAP()


// CEventEditView_Finish �����Դϴ�.

#ifdef _DEBUG
void CEventEditView_Finish::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEventEditView_Finish::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEventEditView_Finish �޽��� ó�����Դϴ�.

void CEventEditView_Finish::OnBnClickedCheck1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData();
	GetDlgItem( IDC_EDIT1 )->EnableWindow( m_bCheckExport );
	GetDlgItem( IDC_BUTTON1 )->EnableWindow( m_bCheckExport );

}

void CEventEditView_Finish::PreProcess()
{
	char szStr[2048] = { 0, };
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "LastExportFullName", szStr, 2048 );
	DWORD dwValue;
	GetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "ExportHeader", dwValue );
	m_szExportFileName = szStr;
	m_bCheckExport = ( dwValue == TRUE );

	GetDlgItem( IDC_EDIT1 )->EnableWindow( m_bCheckExport );
	GetDlgItem( IDC_BUTTON1 )->EnableWindow( m_bCheckExport );

	UpdateData( FALSE );
}

bool CEventEditView_Finish::PostProcess()
{
	UpdateData();
	if( m_bCheckExport == TRUE ) {
		if( m_szExportFileName.IsEmpty() ) {
			MessageBox( "�ͽ���Ʈ �� ������ �������ּ���", "����", MB_OK );
			return false;
		}
		FILE *fp;
		fopen_s( &fp, m_szExportFileName, "wt" );
		if( fp == NULL ) {
			MessageBox( "������ �� �� �����ϴ�.", "����", MB_OK );
			return false;
		}
		fclose(fp);
	}

	switch( s_nStartType ) {
		case 0:
			CSignalManager::GetInstance().AddSignalItem( s_pCurrentSignalItem );
			s_pCurrentSignalItem = NULL;
			break;
		case 1:
			{
				CSignalItem *pItem = CSignalManager::GetInstance().GetSignalItemFromName( s_szCurrentSignalName );

				pItem->SetName( s_pCurrentSignalItem->GetName() );
				pItem->SetInitialsColor( s_pCurrentSignalItem->GetInitialsColor() );
				pItem->SetBackgroundColor( s_pCurrentSignalItem->GetBackgroundColor() );
				pItem->SetHasLength( s_pCurrentSignalItem->IsHasLength() );

				SAFE_DELETE( s_pCurrentSignalItem );
			}
			break;
		case 2:
			CSignalManager::GetInstance().RemoveSignalItem( s_szCurrentSignalName );
			break;
		case 3:
			if( s_pCurrentSignalItem->AddParameter( s_pCurrentPropertyVariable, true ) == false ) {
				SAFE_DELETE( s_pCurrentPropertyVariable );
				MessageBox( "���̻� �Ķ���� �߰��� �� �� �����!!\n������� �������� �ʿ��ؿ�!!", "����", MB_OK );
			}
			s_pCurrentSignalItem = NULL;
			s_pCurrentPropertyVariable = NULL;
			break;
		case 4:
			{
				CUnionValueProperty *pVariable = s_pCurrentSignalItem->GetParameterFromName( s_szCurrentPropertyName );
				*pVariable = *s_pCurrentPropertyVariable;

				s_pCurrentSignalItem = NULL;
				SAFE_DELETE( s_pCurrentPropertyVariable );
			}
			break;
		case 5:
			s_pCurrentSignalItem->RemoveParameter( s_szCurrentPropertyName );
			break;
	}

	if( m_bCheckExport == TRUE ) { 
		CSignalManager::GetInstance().ExportHeader( m_szExportFileName );
		SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "LastExportFullName", m_szExportFileName.GetBuffer() );
	}
	SetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "ExportHeader", (DWORD)m_bCheckExport );
	return true;
}

void CEventEditView_Finish::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	TCHAR szFilter[] = _T( "C/C++ Header File (*.h)|*.h|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("h"), _T("*.h"), OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, this );

	dlg.m_ofn.lpstrTitle = "Export Eternity ActionTool Signal Refrence Header File";
	if( dlg.DoModal() == IDOK ) {
		m_szExportFileName = dlg.GetPathName();
		UpdateData( FALSE );
	}
}
