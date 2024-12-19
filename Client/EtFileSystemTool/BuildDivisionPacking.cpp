// BuildDivisionPacking.cpp : implementation file
//

#include "stdafx.h"
#include "NxFileSystemTool.h"
#include "BuildDivisionPacking.h"
#include "ExportBrowseFolder.h"
#include "FileIOThread.h"

// BuildDivisionPacking dialog

IMPLEMENT_DYNAMIC(BuildDivisionPacking, CDialog)

BuildDivisionPacking::BuildDivisionPacking(CWnd* pParent /*=NULL*/)
	: CDialog(BuildDivisionPacking::IDD, pParent)
	, m_SettingFileName(_T(""))
	, m_PackingState(_T(""))
	, m_szOutFolder(_T(""))
{
	m_bWorking = false;

}

BuildDivisionPacking::~BuildDivisionPacking()
{
}

void BuildDivisionPacking::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_SETTING_FILE, m_SettingFileName);
	DDX_Control(pDX, IDC_PROGRESS_DIV_PACKING, m_DivPackingProgress);
	DDX_Text(pDX, IDC_STATIC_SETTING_OUT_FOLDER, m_szOutFolder);
	DDX_Control(pDX, IDC_PROGRESS_DIV_PACKING_TOTAL, m_DivPackingProgressTotal);
}


BEGIN_MESSAGE_MAP(BuildDivisionPacking, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FILE_OPEN, &BuildDivisionPacking::OnBnClickedButtonFileOpen)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_FILE, &BuildDivisionPacking::OnBnClickedButtonViewFile)
	ON_BN_CLICKED(IDC_BUTTON_PACKING, &BuildDivisionPacking::OnBnClickedButtonPacking)
	ON_BN_CLICKED(IDC_BUTTON_SET_OUTPUT_FOLDER, &BuildDivisionPacking::OnBnClickedButtonSetOutputFolder)
	ON_MESSAGE(WM_COMPLETE_MSG, OnCompleteMsg)
	ON_MESSAGE(WM_NFS_PROGRESS, OnProgress)
	ON_MESSAGE(WM_NFS_PROGRESS_TOTAL, OnProgressTotal)
END_MESSAGE_MAP()


// BuildDivisionPacking message handlers

void BuildDivisionPacking::OnBnClickedButtonFileOpen()
{
	// TODO: Add your control notification handler code here
	CFileDialog Dlg(TRUE,_T(".xml"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("파일(*.xml)|*.xml||"),NULL);

	if ( Dlg.DoModal() == IDCANCEL )
		return;

	m_SettingFileName = Dlg.GetPathName();
	UpdateData(FALSE);

	bool bResult = false;
#ifdef _UNICODE
	std::string str;
	ToMultiString(m_SettingFileName.GetBuffer(), str);
	bResult = ParseSettingFile(str.c_str(), m_PackingInfo);
#else
	bResult = ParseSettingFile(m_SettingFileName.GetBuffer(), m_PackingInfo);
#endif

	if ( bResult == false )
	{
		AfxMessageBox(_T("세팅파일 로드 실패."));
		m_SettingFileName.Empty();
		UpdateData(FALSE);
		return;
	}

}

void BuildDivisionPacking::OnBnClickedButtonViewFile()
{
	// TODO: Add your control notification handler code here
	if ( m_SettingFileName.IsEmpty() )
	{
		AfxMessageBox(_T("세팅 파일을 로드해 주세요."));
		return;
	}

	ShellExecute(NULL, _T("open"), _T("notepad.exe"), m_SettingFileName.GetBuffer(), NULL, SW_SHOW); 

}

void BuildDivisionPacking::OnBnClickedButtonPacking()
{
	// TODO: Add your control notification handler code here
	if ( m_SettingFileName.IsEmpty() )
	{
		AfxMessageBox(_T("세팅 파일을 로드해 주세요."));
		return;

	}
	if ( m_szOutFolder.IsEmpty() ) 
	{
		AfxMessageBox(_T("패킹 파일이 저장될 폴더를 선택해 주세요."));
		return;
	}

	m_nProgressRange = -1;
	m_nProgressTotalRange = -1;

	m_bWorking = true;
	g_pDivPackingThread = new CDivPackingThread(this->GetSafeHwnd(), m_szOutFolder, m_PackingInfo);
	g_pDivPackingThread->Start();
}

void BuildDivisionPacking::OnBnClickedButtonSetOutputFolder()
{
	// TODO: Add your control notification handler code here
	

	CFolderDialog Dlg;
	
	if(Dlg.DoModal() == IDCANCEL)
		return;

	m_szOutFolder = Dlg.GetFolderPath();
	UpdateData(FALSE);


}

bool ParseSettingFile(const CHAR* szFileName, OUT std::vector<_PackingInfo>& PackingInfo )
{
	TiXmlDocument doc;
	doc.LoadFile(szFileName, TIXML_ENCODING_UTF8 );

	std::string msg;
	std::wstring wmsg;


	if ( doc.Error() )
	{
		msg = "xml 파일 파싱 실패 : ";
		msg += szFileName;
		msg += " error msg : ";
		const char* errmsg = doc.ErrorDesc();
		msg += errmsg;

		ToWideString(msg, wmsg);
		return false;
	}

	TiXmlElement* pElement = NULL;
	pElement = doc.RootElement();

	if ( !pElement )
	{
		msg = "xml 파일 파싱 실패 : ";
		msg += szFileName;
		msg += " error msg : ";
		const char* errmsg = doc.ErrorDesc();
		msg += errmsg;

		ToWideString(msg, wmsg);
	}

	TiXmlNode* pNode = pElement->FirstChild("Packing");

	if ( !pNode )		return false;

	pElement = pNode->ToElement();

	tstring tszSrcFolder;
	tstring tszFile;
	tstring tszBaseFolder;

	for ( pElement ; pElement != NULL ; pElement = pElement->NextSiblingElement() )
	{
		const char* szSrcFolder = pElement->Attribute("SrcFolder");
		const char* szFile = pElement->Attribute("File");
		const char* szBaseFolder = pElement->Attribute("BaseFolder");
		int nIncludeSubFolder = 1 ;
		pElement->Attribute("IncludeSubFolder", &nIncludeSubFolder );
#ifdef UNICODE
		ToWideString((CHAR*)szSrcFolder, tszSrcFolder );
		ToWideString((CHAR*)szFile, tszFile );
		ToWideString((CHAR*)szFile, tszBaseFolder );
#else
		tszSrcFolder = szSrcFolder ? szSrcFolder : "" ;
		tszFile = szFile ? szFile : "" ;
		tszBaseFolder = szBaseFolder ? szBaseFolder:"" ;
#endif 
		_PackingInfo info;
		info.szSrcFolder = tszSrcFolder;
		info.szFileName = tszFile;
		info.szBaseFolder = tszBaseFolder;

		if ( nIncludeSubFolder > 0)
			info.bIncludeSubFolder = true;
		else
			info.bIncludeSubFolder = false;

		PackingInfo.push_back( info );
	}

	return true;

}

LRESULT
BuildDivisionPacking::OnCompleteMsg(WPARAM wParam, LPARAM lParam)
{
	m_bWorking = false;
	SAFE_DELETE(g_pDivPackingThread);
	AfxMessageBox(_T("분할 패킹 완료!!"));
	
	return 0;
}

LRESULT
BuildDivisionPacking::OnProgress(WPARAM wParam, LPARAM lParam)
{
	if ( lParam > 0 )
	{
		m_nProgressRange = (int)lParam;
		m_DivPackingProgress.SetRange32((int)0, (int)m_nProgressRange);
	}
	
	m_DivPackingProgress.SetPos((int)wParam);
	return 0;
}


LRESULT
BuildDivisionPacking::OnProgressTotal(WPARAM wParam, LPARAM lParam)
{
	if ( lParam > 0 )
	{
		m_nProgressTotalRange = (int)lParam;
		m_DivPackingProgressTotal.SetRange32((int)0, (int)m_nProgressTotalRange);
	}

	m_DivPackingProgressTotal.SetPos((int)wParam);
	return 0;
}
