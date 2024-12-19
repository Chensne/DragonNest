// EventEditView_SelectEditType.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "EventEditView_SelectEditType.h"
#include "EventEditView_CreateModifySignal.h"


// CEventEditView_SelectEditType

IMPLEMENT_DYNCREATE(CEventEditView_SelectEditType, CFormView)

CEventEditView_SelectEditType::CEventEditView_SelectEditType()
	: CFormView(CEventEditView_SelectEditType::IDD)
{
	m_nEditType = 0;
}

CEventEditView_SelectEditType::~CEventEditView_SelectEditType()
{
}

void CEventEditView_SelectEditType::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEventEditView_SelectEditType, CFormView)
	ON_BN_CLICKED(IDC_RADIO1, &CEventEditView_SelectEditType::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CEventEditView_SelectEditType::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CEventEditView_SelectEditType::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CEventEditView_SelectEditType::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &CEventEditView_SelectEditType::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO6, &CEventEditView_SelectEditType::OnBnClickedRadio6)
END_MESSAGE_MAP()


// CEventEditView_SelectEditType 진단입니다.

#ifdef _DEBUG
void CEventEditView_SelectEditType::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CEventEditView_SelectEditType::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CEventEditView_SelectEditType 메시지 처리기입니다.

void CEventEditView_SelectEditType::OnBnClickedRadio1()
{
	m_nEditType = 0;
}

void CEventEditView_SelectEditType::OnBnClickedRadio2()
{
	m_nEditType = 1;
}

void CEventEditView_SelectEditType::OnBnClickedRadio3()
{
	m_nEditType = 2;
}

void CEventEditView_SelectEditType::OnBnClickedRadio4()
{
	m_nEditType = 3;
}

void CEventEditView_SelectEditType::OnBnClickedRadio5()
{
	m_nEditType = 4;
}

void CEventEditView_SelectEditType::OnBnClickedRadio6()
{
	m_nEditType = 5;
}


CWizardCommon::VIEW_TYPE CEventEditView_SelectEditType::GetNextView()
{
	s_nStartType = m_nEditType;
	switch( m_nEditType ) {
		case 0: return CREATE_SIGNAL;
		case 1:	return SELECT_SIGNAL;
		case 2: return SELECT_SIGNAL;
		case 3:	return SELECT_SIGNAL;
		case 4:	return SELECT_SIGNAL;
		case 5:	return SELECT_SIGNAL;
	}
	return UNKNOWN_TYPE;
}


bool CEventEditView_SelectEditType::IsVisivleNextButton()
{
	return true;
}

void CEventEditView_SelectEditType::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	CheckRadioButton( IDC_RADIO1, IDC_RADIO6, IDC_RADIO1 );
}


void CEventEditView_SelectEditType::PreProcess()
{
}

bool CEventEditView_SelectEditType::PostProcess()
{
	switch( m_nEditType ) {
		case 0: // Create
			{
				s_pCurrentSignalItem = new CSignalItem(-1);
				s_pCurrentSignalItem->SetInitialsColor( RGB(rand()%256,rand()%256,rand()%256) );
				s_pCurrentSignalItem->SetBackgroundColor( RGB(rand()%256,rand()%256,rand()%256) );
			}
			break;
		case 1: // Modify
			{
				s_pCurrentSignalItem = new CSignalItem(-1);
			}
			break;
		case 2:	// Delete
			break;
		case 3:	// Create Parameter
			break;
		case 4:	// Modify Parameter
			break;
		case 5:	// Delete Parameter
			break;
	}
	return true;
}
