#include "StdAfx.h"
#include "DnPotentialStateItemDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnPotentialStateItemDlg::CDnPotentialStateItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	for(int i = 0 ; i < E_TEXT_MAX ; ++i)
		m_pStaticText[i] = NULL;
}

CDnPotentialStateItemDlg::~CDnPotentialStateItemDlg(void)
{
}

void CDnPotentialStateItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemPowerTooltipListDlg.ui" ).c_str(), bShow );
}

void CDnPotentialStateItemDlg::InitialUpdate()
{
	for(int i = 0 ; i < E_TEXT_MAX ; ++i)
		m_pStaticText[i] = GetControl<CEtUIStatic>(FormatA("ID_TEXT%d",i).c_str());
}

void CDnPotentialStateItemDlg::SetInfo( const std::wstring wszItemText1 , const wchar_t* wszItemText2 , const wchar_t* wszItemText3 , bool bStatsIsUP)
{
	DWORD dwTextColor = 0;
	(bStatsIsUP)? dwTextColor = textcolor::FORESTGREEN : dwTextColor = textcolor::RED; 

	float fOffset = 0.02f;

	float fWidth  = m_pStaticText[E_TEXT_STATS_NAME]->GetUICoord().fWidth + fOffset;
	float fHeight = m_pStaticText[E_TEXT_STATS_NAME]->GetUICoord().fHeight;
	m_pStaticText[E_TEXT_STATS_NAME]->SetSize(fWidth, fHeight);
	m_pStaticText[E_TEXT_STATS_NAME]->SetTextWithEllipsis(wszItemText1.c_str(), L"...");
	m_pStaticText[E_TEXT_STATS_NAME]->SetTooltipText(wszItemText1.c_str());

	float fX = m_pStaticText[E_TEXT_STATS_VALUE]->GetUICoord().fX + fOffset;
	float fY = m_pStaticText[E_TEXT_STATS_VALUE]->GetUICoord().fY;
	m_pStaticText[E_TEXT_STATS_VALUE]->SetPosition(fX, fY);
	m_pStaticText[E_TEXT_STATS_VALUE]->SetTextWithEllipsis(wszItemText2, L"...");

	fX = m_pStaticText[E_TEXT_CHANGE_STATS]->GetUICoord().fX + fOffset;
	fY = m_pStaticText[E_TEXT_CHANGE_STATS]->GetUICoord().fY;
	m_pStaticText[E_TEXT_CHANGE_STATS]->SetPosition(fX, fY);
	m_pStaticText[E_TEXT_CHANGE_STATS]->SetTextColor(dwTextColor);
	m_pStaticText[E_TEXT_CHANGE_STATS]->SetTextWithEllipsis(wszItemText3, L"...");
}