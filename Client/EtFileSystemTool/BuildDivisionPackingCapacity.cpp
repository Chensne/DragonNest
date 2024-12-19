// BuildDivisionPackingCapacity.cpp : implementation file
//

#include "stdafx.h"
#include "BuildDivisionPackingCapacity.h"
#include "ExportBrowseFolder.h"
#include "FileIOThread.h"


// BuildDivisionPackingCapacity dialog

IMPLEMENT_DYNAMIC(BuildDivisionPackingCapacity, CDialog)

BuildDivisionPackingCapacity::BuildDivisionPackingCapacity(CWnd* pParent /*=NULL*/)
	: CDialog(BuildDivisionPackingCapacity::IDD, pParent)
	, m_nCapacity(0)
	, m_szBaseFileName(_T(""))
	, m_szOutputFolder(_T(""))
	, m_szInputFolder(_T(""))
{

	m_bWorking = false;

	m_nCapacity = 50;
	m_szBaseFileName = _T("Data");
}

BuildDivisionPackingCapacity::~BuildDivisionPackingCapacity()
{
}

void BuildDivisionPackingCapacity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CAPACITY, m_nCapacity);
	DDV_MinMaxInt(pDX, m_nCapacity, 1, 10000);
	DDX_Text(pDX, IDC_EDIT_BASE_FILENAME, m_szBaseFileName);
	DDX_Text(pDX, IDC_STATIC_SETTING_INPUT_FOLDER_CAP, m_szInputFolder);
	DDX_Text(pDX, IDC_STATIC_SETTING_OUT_FOLDER_CAP, m_szOutputFolder);
	DDX_Control(pDX, IDC_PROGRESS_DIV_PACKING_CAP, m_CurProcess);
	DDX_Control(pDX, IDC_PROGRESS_DIV_PACKING_TOTAL_CAP, m_TotalProgress);
	
}


BEGIN_MESSAGE_MAP(BuildDivisionPackingCapacity, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PACKING_CAP, &BuildDivisionPackingCapacity::OnBnClickedButtonPackingCap)
	ON_BN_CLICKED(IDC_BUTTON_SET_OUTPUT_FOLDER_CAP, &BuildDivisionPackingCapacity::OnBnClickedButtonSetOutputFolderCap)
	ON_BN_CLICKED(IDC_BUTTON_SET_INPUT_FOLDER_CAP, &BuildDivisionPackingCapacity::OnBnClickedButtonSetInputFolderCap)
	ON_MESSAGE(WM_NFS_PROGRESS_CAP, OnProgress)
	ON_MESSAGE(WM_NFS_PROGRESS_TOTAL_CAP, OnProgressTotal)
	ON_MESSAGE(WM_COMPLETE_MSG_CAP, OnCompleteMsg)

END_MESSAGE_MAP()


// BuildDivisionPackingCapacity message handlers

void BuildDivisionPackingCapacity::OnBnClickedButtonSetOutputFolderCap()
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	CFolderDialog Dlg;

	if(Dlg.DoModal() == IDCANCEL)
		return;

	m_szOutputFolder = Dlg.GetFolderPath();
	UpdateData(FALSE);

}

void BuildDivisionPackingCapacity::OnBnClickedButtonSetInputFolderCap()
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	CFolderDialog Dlg;

	if(Dlg.DoModal() == IDCANCEL)
		return;

	m_szInputFolder = Dlg.GetFolderPath();
	UpdateData(FALSE);
}


void BuildDivisionPackingCapacity::OnBnClickedButtonPackingCap()
{
	
	// TODO: Add your control notification handler code here
	//m_szInputFolder = _T("d:\\dev\\project\\DragonNest\\Client\\DragonNest\\Resource");
	//m_szInputFolder = _T("c:\\nexon\\dragonnest");
	//m_szOutputFolder = _T("c:\\nexon\\out");
	UpdateData(TRUE);

	if ( m_szInputFolder.IsEmpty() )
	{
		AfxMessageBox(_T("��ŷ ������ �������ּ���."));
		return;

	}
	if ( m_szOutputFolder.IsEmpty() ) 
	{
		AfxMessageBox(_T("��ŷ ������ ����� ������ ������ �ּ���."));
		return;
	}
	if ( m_nCapacity < 1 )
	{
		AfxMessageBox(_T("���ҿ뷮�� 0 �Դϴ�. ������������."));
		return;

	}

	m_nProgressRange = -1;
	m_nProgressTotalRange = -1;

	DIVPackingCapacityInfo info;
	info.m_InputPath = m_szInputFolder;
	info.m_OutputPath = m_szOutputFolder;
	info.nDivCapacity = m_nCapacity;
	info.m_szBaseName = m_szBaseFileName;
	g_pDivPackingCapaThread = new CDivPackingCapacityThread(this->GetSafeHwnd(), info);
	g_pDivPackingCapaThread->Start();
	m_bWorking = true;
}


LRESULT
BuildDivisionPackingCapacity::OnCompleteMsg(WPARAM wParam, LPARAM lParam)
{
	m_bWorking = false;
	SAFE_DELETE(g_pDivPackingCapaThread);
	AfxMessageBox(_T("���� ��ŷ �Ϸ�!!"));

	return 0;
}

LRESULT
BuildDivisionPackingCapacity::OnProgress(WPARAM wParam, LPARAM lParam)
{
	if ( lParam > 0 )
	{
		m_nProgressRange = (int)lParam;
		m_CurProcess.SetRange32((int)0, (int)m_nProgressRange);
	}

	m_CurProcess.SetPos((int)wParam);
	return 0;
}


LRESULT
BuildDivisionPackingCapacity::OnProgressTotal(WPARAM wParam, LPARAM lParam)
{
	if ( lParam > 0 )
	{
		m_nProgressTotalRange = (int)lParam;
		m_TotalProgress.SetRange32((int)0, (int)m_nProgressTotalRange);
	}

	m_TotalProgress.SetPos((int)wParam);
	return 0;
}
