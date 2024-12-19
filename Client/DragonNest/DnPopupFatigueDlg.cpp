#include "StdAfx.h"
#include "DnPopupFatigueDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPopupFatigueDlg::CDnPopupFatigueDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStatic1(NULL)
, m_pStatic2(NULL)
, m_pStatic3(NULL)
, m_pStaticEvent0(NULL)
, m_pStaticEvent1(NULL)
, m_pStaticEvent2(NULL)
#ifdef PRE_ADD_VIP
, m_pStaticVIP(NULL)
#endif
{
}

CDnPopupFatigueDlg::~CDnPopupFatigueDlg(void)
{
}

void CDnPopupFatigueDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PopupFatigueDlg.ui" ).c_str(), bShow );
}

void CDnPopupFatigueDlg::InitialUpdate()
{
	m_pStatic1 = GetControl<CEtUIStatic>( "ID_STATIC1" );
	m_pStatic2 = GetControl<CEtUIStatic>( "ID_STATIC2" );
	m_pStatic3 = GetControl<CEtUIStatic>( "ID_STATIC3" );
#ifdef PRE_ADD_VIP
	m_pStaticVIP = GetControl<CEtUIStatic>( "ID_STATIC6" );
#endif
	m_pStaticEvent0 = GetControl<CEtUIStatic>( "ID_STATIC_EVENT0" );
	m_pStaticEvent1 = GetControl<CEtUIStatic>( "ID_STATIC_EVENT1" );
	m_pStaticEvent2 = GetControl<CEtUIStatic>( "ID_STATIC_EVENT2" );
	m_pStaticEvent0->Show( false );
#ifdef _CH
#else
	m_pStaticEvent1->Show( false );
#endif
	m_pStaticEvent2->Show( false );

#ifdef _CH
	if (m_pStaticEvent0)
		m_pStaticEvent0->Show(false);
	if (m_pStaticEvent2)
		m_pStaticEvent2->Show(false);
	if (m_pStatic3)
		m_pStatic3->Show( false );
#endif
#ifdef _US
	m_pStatic1->Show( false );
#endif
}

void CDnPopupFatigueDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}
	else
	{
		// 미리 하이드 시켜놔야 블렌딩되는거 안보이게 할 수 있다.
		m_pStaticEvent0->Show( false );
#ifdef _CH
#else
		m_pStaticEvent1->Show( false );
#endif
		m_pStaticEvent2->Show( false );
	}

	CEtUIDialog::Show( bShow );
}

bool CDnPopupFatigueDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	bool bRet;
	bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{	
	case WM_MOUSEMOVE:
		{
			if( IsMouseInDlg() )
				Show( false );
		}
		break;
	}

	return bRet;
}

#ifdef PRE_ADD_VIP
void CDnPopupFatigueDlg::SetFatigue(CDnItemTask::eFatigueType type, int value)
{
	int uiStringNum = 0;
	if (type == CDnItemTask::ePCBANGFTG)		uiStringNum = 5127;
	else if (type == CDnItemTask::eDAILYFTG)	uiStringNum = 5128;
	else if (type == CDnItemTask::eWEEKLYFTG)	uiStringNum = 5129;
	else if (type == CDnItemTask::eEVENTFTG)	uiStringNum = 101010;
	else if (type == CDnItemTask::eVIPFTG)		uiStringNum = 2020027;
	else
	{
		_ASSERT(0);
		return;
	}

	std::wstring str;
	str = FormatW(L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, uiStringNum ), value);

	if (type == CDnItemTask::ePCBANGFTG)		m_pStatic1->SetText(str.c_str());
	else if (type == CDnItemTask::eDAILYFTG)	m_pStatic2->SetText(str.c_str());
#ifdef _CH
#else
	else if (type == CDnItemTask::eWEEKLYFTG)	m_pStatic3->SetText(str.c_str());
#endif
	else if (type == CDnItemTask::eVIPFTG)
	{
		if (m_pStaticVIP)
			m_pStaticVIP->SetText(str.c_str());
	}
	else if (type == CDnItemTask::eEVENTFTG)
	{
		m_pStaticEvent1->SetText(str.c_str());

#ifdef _CH
#else
		m_pStaticEvent1->Show( value ? true : false );
		m_pStaticEvent0->Show( value ? true : false );
		m_pStaticEvent2->Show( value ? true : false );
#endif
	}
	else
	{
		_ASSERT(0);
		return;
	}
}
#endif // PRE_ADD_VIP

void CDnPopupFatigueDlg::SetFatigue( int nFatigue, int nWeekFatigue, int nPCRoomFatigue, int nEventFatigue )
{
	WCHAR wszTemp[64];
	swprintf_s( wszTemp, _countof(wszTemp), L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5127 ), nPCRoomFatigue );
	m_pStatic1->SetText( wszTemp );
	swprintf_s( wszTemp, _countof(wszTemp), L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5128 ), nFatigue );
	m_pStatic2->SetText( wszTemp );
#ifdef _CH
#else
	swprintf_s( wszTemp, _countof(wszTemp), L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5129 ), nWeekFatigue );
	m_pStatic3->SetText( wszTemp );
#endif

	swprintf_s( wszTemp, _countof(wszTemp), L"%s : %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101010 ), nEventFatigue );
	m_pStaticEvent1->SetText( wszTemp );

#ifdef _CH
#else
	m_pStaticEvent1->Show( nEventFatigue ? true : false );
	m_pStaticEvent0->Show( nEventFatigue ? true : false );
	m_pStaticEvent2->Show( nEventFatigue ? true : false );
#endif

#ifdef _KRAZ
	m_pStatic1->Show( false );
#endif
}