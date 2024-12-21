// DnDynamicCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DnResourceEditer.h"
#include "DnDynamicCtrl.h"
#include "DnControlManager.h"

// CDnDynamicCtrl 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDnDynamicCtrl, CDialog)

CDnDynamicCtrl::CDnDynamicCtrl(CWnd* pParent /*=NULL*/)
	: CDialog(CDnDynamicCtrl::IDD, pParent)
{

}
CDnDynamicCtrl::CDnDynamicCtrl(CString szEnum, CString szName, CWnd* pParent /*=NULL*/)
	: CDialog(CDnDynamicCtrl::IDD, pParent)
{
	m_szName = szName;
	m_szEnum = szEnum;
}

CDnDynamicCtrl::CDnDynamicCtrl(int nLastSelCtrl, CWnd* pParent)
	: CDialog(CDnDynamicCtrl::IDD, pParent), m_nLastSelCtrl(nLastSelCtrl)
{
}

CDnDynamicCtrl::~CDnDynamicCtrl()
{
}

void CDnDynamicCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ENUM_VALUE, m_editEnum);
	DDX_Control(pDX, IDC_EDIT_CTRL_NAME, m_editCtrlName);
}


BEGIN_MESSAGE_MAP(CDnDynamicCtrl, CDialog)
	ON_EN_CHANGE(IDC_EDIT_ENUM_VALUE, &CDnDynamicCtrl::OnEnChangeEditEnumValue)
	ON_EN_CHANGE(IDC_EDIT_CTRL_NAME, &CDnDynamicCtrl::OnEnChangeEditCtrlName)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_EN_SETFOCUS(IDC_EDIT_CTRL_NAME, &CDnDynamicCtrl::OnEnSetfocusEditCtrlName)
END_MESSAGE_MAP()


int CDnDynamicCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) // 1
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CDnDynamicCtrl::OnInitDialog() // 2
{
	CDialog::OnInitDialog();

	m_editEnum.SetWindowText(m_szEnum);
	m_editCtrlName.SetWindowText(m_szName);
	m_editEnum.SetFocus();

	return TRUE;
}


void CDnDynamicCtrl::OnEnChangeEditEnumValue()
{
	CString szCurrentStr;
	m_editEnum.GetWindowText(szCurrentStr);
	DNCTRLMANAGER.m_DynamicControlData.m_nEnumValue = _ttoi(szCurrentStr);

	TRACE("1--> %d\n" , DNCTRLMANAGER.m_DynamicControlData.m_nEnumValue);
}


void CDnDynamicCtrl::OnEnChangeEditCtrlName()
{
	CString str;
	m_editCtrlName.GetWindowText(str);
	DNCTRLMANAGER.m_DynamicControlData.m_szName = str;	
}


void CDnDynamicCtrl::OnEnSetfocusEditCtrlName()
{
	CString str;

	if( DNCTRLMANAGER.m_DynamicControlData.m_nEnumValue >= 0  )
	{
		TRACE("2--> %d\n" , DNCTRLMANAGER.m_DynamicControlData.m_nEnumValue);
		str.Format(L"[ %d ] ", DNCTRLMANAGER.m_DynamicControlData.m_nEnumValue);
		
		CString szName;
		m_editCtrlName.GetWindowText(szName);
		
		
		int nPos  = szName.Find(']') + 1;
		int nSize = szName.GetLength() - nPos;
		szName	  = szName.Right(nSize);
		str += szName;
	}

	m_editCtrlName.SetWindowText(str);
	m_editCtrlName.SetSel(m_editCtrlName.GetWindowTextLength(),m_editCtrlName.GetWindowTextLength());
}

void CDnDynamicCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);
}

BOOL CDnDynamicCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if( pMsg->wParam == VK_ESCAPE )
	{
		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

