#include "StdAfx.h"
#include "BaseView.h"


IMPLEMENT_DYNAMIC(CBaseView, CFormView)

CBaseView::CBaseView(UINT nIDTemplate)
	: CFormView(nIDTemplate), m_PageIndex(EV_PAGEIDX_DEF), m_nIDTemplate(nIDTemplate)
{
}

CBaseView::~CBaseView()
{
	
}
