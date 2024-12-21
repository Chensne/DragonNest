#include "StdAfx.h"
#include "DnInvenSymbolDlg.h"
#include "DnInvenSymbolDescDlgLeft.h"
#include "DnInvenSymbolDescDlgRight.h"
#include "DnCustomControlCommon.h"
#include "DnInterface.h"
#include "DnItemAlarmDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInvenSymbolDlg::CDnInvenSymbolDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pButtonPrev(NULL)
	, m_pButtonNext(NULL)
	, m_pStaticPage(NULL)
	, m_nMaxPage(SLOT_MAX_PAGE)
	, m_nCurrentPage(0)
	, m_pSymbolDescRightDlg(NULL)
	, m_pSymbolDescLeftDlg(NULL)
{
	m_vecSymbolItem.resize( SLOT_MAX_COUNT );
}

CDnInvenSymbolDlg::~CDnInvenSymbolDlg(void)
{
	SAFE_RELEASE_SPTR( m_hSymbolImage );
	SAFE_DELETE( m_pSymbolDescRightDlg );
	SAFE_DELETE( m_pSymbolDescLeftDlg );
}

void CDnInvenSymbolDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenSymbolDlg.ui" ).c_str(), bShow );
}

void CDnInvenSymbolDlg::InitialUpdate()
{
	m_hSymbolImage = LoadResource( CEtResourceMng::GetInstance().GetFullName("SimbolIcon01.dds").c_str(), RT_TEXTURE );

	m_pButtonPrev = GetControl<CEtUIButton>("ID_BUTTON_PREV");
	m_pButtonNext = GetControl<CEtUIButton>("ID_BUTTON_NEXT");
	m_pStaticPage = GetControl<CEtUIStatic>("ID_STATIC_PAGE");

	SSymbolSlot symbolSlot;
	char szControlName[32]={0};

	for( int i=0; i<SLOT_MAX_SIZE; i++ )
	{
		sprintf_s( szControlName, 32, "ID_SYMBOL_%02d", i );
		symbolSlot.m_pSymbolImage = GetControl<CEtUITextureControl>(szControlName);

		sprintf_s( szControlName, 32, "ID_NAME_%02d", i );
		symbolSlot.m_pStaticName = GetControl<CEtUIStatic>(szControlName);

		sprintf_s( szControlName, 32, "ID_COUNT_%02d", i );
		symbolSlot.m_pStaticCount = GetControl<CEtUIStatic>(szControlName);

		symbolSlot.Clear();
		m_vecSymbolSlot.push_back( symbolSlot );
	}

	m_pSymbolDescRightDlg = new CDnInvenSymbolDescDlgRight( UI_TYPE_CHILD, this, SYMBOLDESCRIGHT_DIALOG );
	m_pSymbolDescRightDlg->Initialize( false );

	m_pSymbolDescLeftDlg = new CDnInvenSymbolDescDlgLeft( UI_TYPE_CHILD, this, SYMBOLDESCLEFT_DIALOG );
	m_pSymbolDescLeftDlg->Initialize( false );
}

void CDnInvenSymbolDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		UpdateSymbolSlot();
		UpdatePage();
		UpdatePageButton();
	}

	CEtUIDialog::Show( bShow );
}

void CDnInvenSymbolDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_PREV" ) )
		{
			PrevPage();
			return;
		}

		if( IsCmdControl("ID_BUTTON_NEXT" ) )
		{
			NextPage();
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnInvenSymbolDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			float fMouseX, fMouseY;
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			SUICoord uiCoord;

			for( int i=0; i<SLOT_MAX_SIZE; i++ )
			{
				if( !m_vecSymbolSlot[i].m_pSymbolImage->IsShow() )
					continue;

				m_vecSymbolSlot[i].m_pSymbolImage->GetUICoord( uiCoord );

				if( uiCoord.IsInside( fMouseX, fMouseY ) )
				{
					ShowDescDialog( true, i );
					return bRet;
				}
			}

			ShowDescDialog( false );
		}
		break;
	}

	return bRet;
}

void CDnInvenSymbolDlg::ClearAll()
{
	for( int i=0; i<SLOT_MAX_SIZE; i++ )
	{
		m_vecSymbolSlot[i].Clear();
	}

	for( int i=0; i<SLOT_MAX_COUNT; i++ )
	{
		m_vecSymbolItem[i].Clear();
	}
}

void CDnInvenSymbolDlg::AddSymbolItem( int nSlotIndex, const wchar_t *wszName, int nCount, int nImageIndex, const wchar_t *wszTooltip, bool bList )
{
	//if( !bList )
	//{
	//	if( m_vecSymbolItem[nSlotIndex].m_nCount == 0 )
	//	{
	//		GetInterface().ShowSymbolAlarmDialog( wszName, nImageIndex, nCount, CDnItemAlarmDlg::typeAlarmAdd, textcolor::WHITE, 3.0f );
	//	}
	//	else
	//	{
	//		GetInterface().ShowSymbolAlarmDialog( wszName, nImageIndex, nCount, CDnItemAlarmDlg::typeAlarmModify, textcolor::WHITE, 3.0f );
	//	}
	//}

	SSymbolItem sSymbolItem;
	sSymbolItem.m_strName = wszName;
	sSymbolItem.m_nCount = nCount;
	sSymbolItem.m_nImageIndex = nImageIndex;
	sSymbolItem.m_strTooltip = wszTooltip;

	m_vecSymbolItem[nSlotIndex] = sSymbolItem;

	int nSlotPage = nSlotIndex/SLOT_MAX_SIZE;

	if( m_nCurrentPage == nSlotPage )
	{
		int nIndex = nSlotIndex%SLOT_MAX_SIZE;

		m_vecSymbolSlot[nIndex].m_pSymbolImage->SetTexture( m_hSymbolImage, 
															(m_vecSymbolItem[nSlotIndex].m_nImageIndex%SYMBOL_ICON_XCOUNT)*SYMBOL_ICON_XSIZE, 
															(m_vecSymbolItem[nSlotIndex].m_nImageIndex/SYMBOL_ICON_XCOUNT)*SYMBOL_ICON_YSIZE,
															SYMBOL_ICON_XSIZE, 
															SYMBOL_ICON_YSIZE );
		m_vecSymbolSlot[nIndex].m_pSymbolImage->Show(true);
		m_vecSymbolSlot[nIndex].m_pStaticName->SetText(m_vecSymbolItem[nSlotIndex].m_strName);
		m_vecSymbolSlot[nIndex].m_pStaticCount->SetIntToText(m_vecSymbolItem[nSlotIndex].m_nCount);
		m_vecSymbolSlot[nIndex].m_strTooltip = m_vecSymbolItem[nSlotIndex].m_strTooltip;
	}
}

void CDnInvenSymbolDlg::DelSysmbolItem( int nSlotIndex )
{
	//GetInterface().ShowSymbolAlarmDialog( m_vecSymbolItem[nSlotIndex].m_strName.c_str(), 
	//									m_vecSymbolItem[nSlotIndex].m_nImageIndex, 
	//									m_vecSymbolItem[nSlotIndex].m_nCount, 
	//									CDnItemAlarmDlg::typeAlarmDel, textcolor::WHITE, 3.0f );

	m_vecSymbolItem[nSlotIndex].Clear();

	int nSlotPage = nSlotIndex/SLOT_MAX_SIZE;

	if( m_nCurrentPage == nSlotPage )
	{
		int nIndex = nSlotIndex%SLOT_MAX_SIZE;
		m_vecSymbolSlot[nIndex].Clear();
	}
}

void CDnInvenSymbolDlg::UpdateSymbolSlot()
{
	for( int i=0; i<SLOT_MAX_SIZE; i++ )
	{
		m_vecSymbolSlot[i].Clear();
	}

	for( int i=m_nCurrentPage * SLOT_MAX_SIZE, j=0; j<SLOT_MAX_SIZE; i++, j++ )
	{
		if( m_vecSymbolItem[i].m_nCount > 0 )
		{
			m_vecSymbolSlot[j].m_pSymbolImage->SetTexture( m_hSymbolImage, 
														(m_vecSymbolItem[i].m_nImageIndex%SYMBOL_ICON_XCOUNT)*SYMBOL_ICON_XSIZE, 
														(m_vecSymbolItem[i].m_nImageIndex/SYMBOL_ICON_XCOUNT)*SYMBOL_ICON_YSIZE, 
														SYMBOL_ICON_XSIZE, 
														SYMBOL_ICON_YSIZE );
			m_vecSymbolSlot[j].m_pSymbolImage->Show( true );
			m_vecSymbolSlot[j].m_pStaticName->SetText(m_vecSymbolItem[i].m_strName);
			m_vecSymbolSlot[j].m_pStaticCount->SetIntToText(m_vecSymbolItem[i].m_nCount);
			m_vecSymbolSlot[j].m_strTooltip = m_vecSymbolItem[i].m_strTooltip;
		}
	}
}

void CDnInvenSymbolDlg::UpdatePage()
{
	wchar_t wszTemp[80]={0};
	swprintf_s( wszTemp, 80, L"%d/%d", m_nCurrentPage+1, SLOT_MAX_PAGE );
	m_pStaticPage->SetText(wszTemp);
}

void CDnInvenSymbolDlg::NextPage()
{
	if( m_vecSymbolItem.empty() )
		return;

	m_nCurrentPage++;
	if( m_nCurrentPage > (SLOT_MAX_PAGE-1) )
	{
		m_nCurrentPage = (SLOT_MAX_PAGE-1);
	}

	UpdateSymbolSlot();
	UpdatePage();
	UpdatePageButton();
}

void CDnInvenSymbolDlg::PrevPage()
{
	if( m_vecSymbolItem.empty() )
		return;

	m_nCurrentPage--;
	if( m_nCurrentPage < 0 )
	{
		m_nCurrentPage = 0;
	}

	UpdateSymbolSlot();
	UpdatePage();
	UpdatePageButton();
}

void CDnInvenSymbolDlg::UpdatePageButton()
{
	if( m_nCurrentPage >= (SLOT_MAX_PAGE-1) )
	{
		m_pButtonNext->Enable(false);
	}
	else
	{
		m_pButtonNext->Enable(true);
	}

	if( m_nCurrentPage == 0 )
	{
		m_pButtonPrev->Enable(false);
	}
	else
	{
		m_pButtonPrev->Enable(true);
	}
}

void CDnInvenSymbolDlg::ShowDescDialog( bool bShow, int nSlotIndex )
{
	if( bShow )
	{
		SSymbolSlot &symbolSlot = m_vecSymbolSlot[nSlotIndex];

		SUICoord uiCoord;
		symbolSlot.m_pSymbolImage->GetUICoord( uiCoord );

		if( nSlotIndex%2 )
		{
			SUICoord DlgCoord;
			m_pSymbolDescRightDlg->GetDlgCoord( DlgCoord );
			DlgCoord.fX = GetXCoord() + uiCoord.fX - DlgCoord.fWidth;
			DlgCoord.fY = GetYCoord() + uiCoord.fY;
			m_pSymbolDescRightDlg->SetDlgCoord( DlgCoord );

			m_pSymbolDescRightDlg->SetDesc( symbolSlot.m_pStaticName->GetText(), symbolSlot.m_strTooltip.c_str() );
			ShowChildDialog( m_pSymbolDescRightDlg, true );
		}
		else
		{
			SUICoord DlgCoord;
			m_pSymbolDescLeftDlg->GetDlgCoord( DlgCoord );
			DlgCoord.fX = GetXCoord() + uiCoord.Right();
			DlgCoord.fY = GetYCoord() + uiCoord.fY;
			m_pSymbolDescLeftDlg->SetDlgCoord( DlgCoord );

			m_pSymbolDescLeftDlg->SetDesc( symbolSlot.m_pStaticName->GetText(), symbolSlot.m_strTooltip.c_str() );
			ShowChildDialog( m_pSymbolDescLeftDlg, true );
		}
	}
	else
	{
		ShowChildDialog( m_pSymbolDescRightDlg, false );
		ShowChildDialog( m_pSymbolDescLeftDlg, false );
	}
}