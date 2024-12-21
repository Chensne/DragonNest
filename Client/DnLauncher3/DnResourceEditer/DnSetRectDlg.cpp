// DnSetRectDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DnResourceEditer.h"
#include "DnSetRectDlg.h"
#include "DnControlManager.h"


// CDnSetRectDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDnSetRectDlg, CDialog)

CDnSetRectDlg::CDnSetRectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnSetRectDlg::IDD, pParent)
{
}

CDnSetRectDlg::~CDnSetRectDlg()
{
}

void CDnSetRectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SETRECT_1, m_editRect1);
	DDX_Control(pDX, IDC_EDIT_SETRECT_2, m_editRect2);
	DDX_Control(pDX, IDC_EDIT_SETRECT_3, m_editRect3);
	DDX_Control(pDX, IDC_EDIT_SETRECT_4, m_editRect4);
}


BEGIN_MESSAGE_MAP(CDnSetRectDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CDnSetRectDlg::OnBnClickedOk)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CDnSetRectDlg 메시지 처리기입니다.

void CDnSetRectDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strData[4];
	m_editRect1.GetWindowText(strData[0]);
	m_editRect2.GetWindowText(strData[1]);
	m_editRect3.GetWindowText(strData[2]);
	m_editRect4.GetWindowText(strData[3]);

	SetRect(&DNCTRLMANAGER.m_RectData, _ttoi(strData[0]),_ttoi(strData[1]),_ttoi(strData[2]),_ttoi(strData[3]));

	OnOK();
}

void CDnSetRectDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CDnSetRectDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if( pMsg->message == WM_KEYDOWN )
	{
		bool bControl = ((GetKeyState(VK_CONTROL) & 0x8000) != 0);

		if( bControl )
		{
			if(pMsg->wParam == 'v' ||pMsg->wParam == 'V')
			{
				return GetClipBoardData();
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDnSetRectDlg::ParsingClipBoard(char* pString, std::vector<CString>* pList)
{
	char		*p = NULL;
	CString		szTemp;

	p = strtok(pString, ",");

	while( p )
	{
		szTemp.Empty();
		szTemp = p;
		pList->push_back(szTemp);
		p = strtok(NULL, ",");
	}
}

bool CDnSetRectDlg::GetClipBoardData()
{
	if( OpenClipboard() )
	{
		char *pchData = NULL;
		std::vector<CString> vTempList;

		if( IsClipboardFormatAvailable(CF_TEXT) || IsClipboardFormatAvailable(CF_OEMTEXT) )
		{
			HANDLE hClipboardData = GetClipboardData(CF_TEXT);
			pchData = (char*)GlobalLock(hClipboardData);
			ParsingClipBoard(pchData, &vTempList);
			GlobalUnlock(hClipboardData);
		}
		CloseClipboard();

		if( !vTempList.empty() ) 
		{
			m_editRect1.Clear();
			m_editRect2.Clear();
			m_editRect3.Clear();
			m_editRect4.Clear();

			if(vTempList.size() <= 1)
			{
				return false;
			}

			m_editRect1.SetWindowText(vTempList[0]);
			m_editRect2.SetWindowText(vTempList[1]);
			m_editRect3.SetWindowText(vTempList[2]);
			m_editRect4.SetWindowText(vTempList[3]);
		}
	}

	return true;
}
