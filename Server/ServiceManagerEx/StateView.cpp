// StateView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "StateView.h"
#include "ItemData.h"

// CStateView

IMPLEMENT_DYNCREATE(CStateView, CView)

CStateView::CStateView()
	: m_pProperty(NULL)
{

}

CStateView::~CStateView()
{
	delete m_pProperty;
}

BEGIN_MESSAGE_MAP(CStateView, CView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CStateView 그리기입니다.

void CStateView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
	UNUSED_ALWAYS(pDoc);
}

void CStateView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CRect rect;
	GetClientRect(&rect);

	m_pProperty = new CPropertyGrid();
	m_pProperty->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, IDC_PROPERTYGRID_STATE);

	const ViewConfig* pViewConfig = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().GetViewConfig(L"State");
	if (pViewConfig)
	{
		m_pProperty->SetTitleColor(RGB(0, 0, 0));
		m_pProperty->SetBackColor(pViewConfig->GetBgColor());
		m_pProperty->SetTextColor(pViewConfig->GetDefaultFontColor());
		m_pProperty->SetDisabledColor(pViewConfig->GetDefaultFontColor());
	}
	else
	{
		m_pProperty->SetTitleColor(RGB(0, 0, 0));
		m_pProperty->SetBackColor(RGB(0, 0, 0));
		m_pProperty->SetTextColor(RGB(192, 192, 192));
		m_pProperty->SetDisabledColor(RGB(192, 192, 192));
	}
}

// CStateView 진단입니다.

#ifdef _DEBUG
void CStateView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CStateView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG

void CStateView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_pProperty)
		m_pProperty->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE);
}

void CStateView::RefreshView()
{
	if (!m_pProperty)
		return;

	// clear
	m_pProperty->ResetContents();
	m_propertyItems.clear();
	m_propertyItems.resize(CStateView::Max, 0);

	// build property grid
	HSECTION section = m_pProperty->AddSection(L"Basic Information");
	m_propertyItems[CStateView::Type] = m_pProperty->AddStringItem(section, L"Type", L"", false);
	m_propertyItems[CStateView::Name] = m_pProperty->AddStringItem(section, L"Name", L"", false);
	m_propertyItems[CStateView::Id] = m_pProperty->AddStringItem(section, L"ID", L"", false);
	m_propertyItems[CStateView::World] = m_pProperty->AddStringItem(section, L"World ID(s)", L"", false);

	section = m_pProperty->AddSection(L"State Information");
	m_propertyItems[CStateView::State] = m_pProperty->AddStringItem(section, L"State", L"", false);
	m_propertyItems[CStateView::Version] = m_pProperty->AddStringItem(section, L"Version", L"", false);
	m_propertyItems[CStateView::Final] = m_pProperty->AddStringItem(section, L"Final", L"", false);
}

void CStateView::OnSelectedServerItem(const ItemData* pData)
{
	if (!pData || !m_pProperty)
	{
		ASSERT(0);
		return;
	}

	CString strId;
	strId.Format(L"%d", pData->id);

	m_pProperty->SetItemValue(m_propertyItems[CStateView::Type], ItemData::TypeToString(pData->type));
	m_pProperty->SetItemValue(m_propertyItems[CStateView::Name], pData->name.c_str());
	m_pProperty->SetItemValue(m_propertyItems[CStateView::Id], strId.GetBuffer());
	m_pProperty->SetItemValue(m_propertyItems[CStateView::World], pData->world.c_str());
	m_pProperty->SetItemValue(m_propertyItems[CStateView::State], ItemData::StateToString(pData->state));
	m_pProperty->SetItemValue(m_propertyItems[CStateView::Version], pData->version.c_str());
	m_pProperty->SetItemValue(m_propertyItems[CStateView::Final], ItemData::FinalToString(pData->final));


	// 특정 서버를 선택했을때 다른 정보를 보여주고 싶으면 이곳.
	if (pData->type == ItemData::Server)
	{
		if (pData->name.compare(L"Master") == 0)
		{
		}
		else if (pData->name.compare(L"DB") == 0)
		{
		}
		else if (pData->name.compare(L"Village") == 0)
		{
		}
	}
}

;