#include "StdAfx.h"
#include "XTCustomPropertyGridItemFile.h"

#ifdef FILEDLG_AUTOPATH
CString CXTCustomPropertyGridItemFile::s_szExt;
int CXTCustomPropertyGridItemFile::s_nMode = 0;
char CXTCustomPropertyGridItemFile::s_szDirectory[256] = {0,};
#endif

CXTCustomPropertyGridItemFile::CXTCustomPropertyGridItemFile(CString strCaption, LPCTSTR strValue, CString* m_pBindString)
: CXTPPropertyGridItem(strCaption, strValue, m_pBindString)
{
	m_nFlags = xtpGridItemHasExpandButton;
}

void CXTCustomPropertyGridItemFile::OnInplaceButtonDown()
{
	CString szFilders;
	szFilders.Format( "%s|%s|All Files (*.*)|*.*||", m_szFileDesc.GetBuffer(), m_szExt.GetBuffer() );

	CFileDialog dlg(TRUE, m_szExt, m_szExt, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilders);

	if( false == m_szInitDirectory.empty() )
	{
		char szString[256] = {0, };
		sprintf_s( szString, "%s\\Particle", m_szInitDirectory.c_str() );

		dlg.GetOFN().lpstrInitialDir = szString;
	}

#ifdef FILEDLG_AUTOPATH
	if( s_nMode == 0 ) {
	}
	else if( s_nMode == OtherToWav ) {
		dlg.GetOFN().lpstrInitialDir = "R:\\GameRes\\Resource\\Sound\\UI";
	}
	else if( s_nMode == WavToOther ) {
		dlg.GetOFN().lpstrInitialDir = s_szDirectory;
	}
#endif
	if ( dlg.DoModal( ) == IDOK )
	{
		OnValueChanged( dlg.GetPathName() );
		m_pGrid->Invalidate( FALSE );
		
	}
#ifdef FILEDLG_AUTOPATH
	if( s_nMode != OtherToWav )
		GetCurrentDirectory(256, s_szDirectory);
#endif
};


void CXTCustomPropertyGridItemFile::SetExt( const char *szStr, const char *szFileDesc )
{
#ifdef FILEDLG_AUTOPATH
	// ���簪�� wav�� �ƴϰ�, �����°� wav�϶�,
	if( (strcmp(szStr, "*.wav") == 0) && (strcmp(s_szExt.GetBuffer(), "*.wav") != 0) )
	{
		s_nMode = OtherToWav;
	}
	else if( (strcmp(szStr, "*.wav") != 0) && (strcmp(s_szExt.GetBuffer(), "*.wav") == 0) )
	{
		s_nMode = WavToOther;
	}
	else
	{
		s_nMode = 0;
	}
#endif

	m_szExt = szStr;
	m_szFileDesc = szFileDesc;

#ifdef FILEDLG_AUTOPATH
	// ���簪�� ���.
	s_szExt = m_szExt;
#endif
}

void CXTCustomPropertyGridItemFile::SetInitDirectory( const std::string szInitDirectory )
{
	m_szInitDirectory = szInitDirectory;
}

BOOL CXTCustomPropertyGridItemFile::OnKeyDown(UINT nChar)
{
	if( nChar == 46) {	// Delete Ű
		OnValueChanged("");
	}
	return TRUE;
}
