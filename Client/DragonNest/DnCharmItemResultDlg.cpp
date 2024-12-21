#include "StdAfx.h"
#include "DnCharmItemResultDlg.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnItem.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharmItemResultDlg::CDnCharmItemResultDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pItem(NULL)
, m_pItemSlotButton(NULL)
, m_pStaticItemName(NULL)
{
#if defined(PRE_FIX_43986)
	m_nSoundIndex = -1;
#endif // PRE_FIX_43986
}

CDnCharmItemResultDlg::~CDnCharmItemResultDlg(void)
{
	SAFE_DELETE( m_pItem );
}

void CDnCharmItemResultDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenBoxResultDlg.ui" ).c_str(), bShow );
}

void CDnCharmItemResultDlg::InitialUpdate()
{
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_BUTTON0");
	m_pStaticItemName = GetControl<CEtUIStatic>("ID_STATIC0");
	m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_OK") );

#if defined(PRE_FIX_43986)
	m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( "UI_CharmItemConfirm.ogg", false, false );
#endif // PRE_FIX_43986
}

void CDnCharmItemResultDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_SmartMove.MoveCursor();

		//»ç¿îµå Ãâ·Â..
#if defined(PRE_FIX_43986)
		if (m_nSoundIndex != -1)
			CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndex );
#endif // PRE_FIX_43986
	}
	else
	{
		m_SmartMove.ReturnCursor();
		m_pItemSlotButton->ResetSlot();
		m_pStaticItemName->SetText( L"" );
	}
}

void CDnCharmItemResultDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show(false);
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCharmItemResultDlg::SetResultItem( int nItemID, int nCount, int nPeriod, INT64 nGold )
{
	TItemInfo itemInfo;
	if( CDnItem::MakeItemInfo( nItemID, 1, itemInfo ) == false ) return;
	SAFE_DELETE( m_pItem );
	m_pItem = GetItemTask().CreateItem( itemInfo );

	if( m_pItem )
	{	
		std::wstring strText;	
				
		// °ñµåÈ¹µæ.
		if( nGold > 0 )
		{	
			wchar_t str[256] = {0,};

			// °ñµå.
			INT64 coin = nGold / 10000;
			if( coin > 0 )
			{
				swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1766 ), coin );
				strText.assign( str );
			}

			// ½Ç¹ö.
			coin = (nGold % 10000) / 100;
			if( coin > 0 )
			{
				strText.append( L" " );
				swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1781 ), coin );			
				strText.append( str );
			}

			// ÄÚÆÛ.
			coin = nGold % 100;
			if( coin > 0 )
			{
				strText.append( L" " );
				swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1783 ), nGold%100 );			
				strText.append( str );
			}

			m_pItemSlotButton->SetItem( m_pItem, 0 );
		}

		// ¾ÆÀÌÅÛÈ¹µæ.
		else
		{
			WCHAR wszTemp[256] = {0,};

			if( nPeriod > 0 )
			{
				WCHAR wszTemp2[256] = {0,};
				swprintf_s( wszTemp2, _countof(wszTemp2), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4640 ), nPeriod );
				swprintf_s( wszTemp, _countof(wszTemp), L"%s(%s)", m_pItem->GetName(), wszTemp2 );
			}
			else
			{
				m_pItem->SetEternityItem( true );
				swprintf_s( wszTemp, _countof(wszTemp), L"%s", m_pItem->GetName() );
			}

			strText.assign( wszTemp );
			m_pItemSlotButton->SetItem( m_pItem, nCount );
		}
		
		m_pStaticItemName->SetText( strText );
	}
}
