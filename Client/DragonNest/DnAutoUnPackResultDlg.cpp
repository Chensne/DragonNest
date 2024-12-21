#include "Stdafx.h"

#ifdef PRE_ADD_AUTOUNPACK

#include "ItemSendPacket.h"

#include "DnItemTask.h"

#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnAutoUnPackDlg.h"

#include "DnAutoUnPackResultDlg.h"


CDnAutoUnPackResultDlg::CDnAutoUnPackResultDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor ),
m_fOrgProgressTime(0.0f), m_fProgressTime(0.0f), m_pAutoUnPackDlg(NULL), m_pItemSlotBtn( NULL ), m_pResultProgress( NULL ), m_pStaticItemName( NULL ), m_pResultItem( NULL )
{
	
}

CDnAutoUnPackResultDlg::~CDnAutoUnPackResultDlg()
{
	SAFE_DELETE(m_pResultItem);
}

void CDnAutoUnPackResultDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("InvenBoxOpenResultDlg.ui").c_str(), bShow );
}


void CDnAutoUnPackResultDlg::InitialUpdate()
{
	m_pItemSlotBtn = GetControl<CDnItemSlotButton>("ID_BT_ITEM");
	m_pResultProgress = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR1");
	m_pResultProgress->SetReverse( true );
	m_pStaticItemName = GetControl<CEtUIStatic>("ID_TEXT_ITEMNAME");
}


void CDnAutoUnPackResultDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;
	
	CEtUIDialog::Show( bShow );
	
	if( bShow )
	{
		m_bUnpacking = true;

		// �ڵ�����â.
		if( m_pAutoUnPackDlg == NULL )
		{ 			
			m_pAutoUnPackDlg = GetInterface().GetAutoUnPackDlg();
		}
	
		m_pResultProgress->Show( true );
		m_fOrgProgressTime = m_fProgressTime = 1.15f;
	}	
	else
	{
		m_bUnpacking = false;
	}
}


void CDnAutoUnPackResultDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_CANCEL") )
			m_fProgressTime = 0.0f;

		if( IsCmdControl( "ID_STOP" ) )
		{
			// �ڵ����� �ߴ�.
			if( GetInterface().IsOpenAutoUnPackDlg() )
				m_bUnpacking = false;				
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnAutoUnPackResultDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	// "�����ߴ�" �� ��������. 
	if( !m_bUnpacking )
	{
		// �������º���.
		if( m_pAutoUnPackDlg )
		{
			m_pAutoUnPackDlg->PreStateRestore();
			Show( false );
		}
	}

	// Progress �������.
	else
	{
		m_fProgressTime -= fElapsedTime;
		if( m_fProgressTime < 0.0f )
		{
			m_pResultProgress->Enable( false );
			Show( false );

			// �����ۿ��� �غ�.
			if( m_pAutoUnPackDlg )
				m_pAutoUnPackDlg->ReadyUnPack();
		}
		else
			m_pResultProgress->SetProgress( (1.0f - (m_fProgressTime / m_fOrgProgressTime)) * 100.0f );
	}
}


// ��� ������ ���.
void CDnAutoUnPackResultDlg::SetResultItem( int nItemID, int nCount, int nPeriod, INT64 nGold )
{
	TItemInfo itemInfo;	
	if( CDnItem::MakeItemInfo( nItemID, 1, itemInfo ) == false ) 
		return;

	if( m_pResultItem )
	{
		SAFE_DELETE(m_pResultItem);
		m_pItemSlotBtn->ResetSlot();
	}

	m_pResultItem = GetItemTask().CreateItem( itemInfo );

	if( m_pResultItem )
	{
		std::wstring strText;	

		// ���ȹ�� - 0�� ������ ������� ����. (ex> 1���0�ǹ�1���� => 1���1���� ).
		if( nGold > 0 )
		{	
			wchar_t str[256] = {0,};

			// ���.
			INT64 coin = nGold / 10000;
			if( coin > 0 )
			{
				swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1766 ), coin );
				strText.assign( str );
			}

			// �ǹ�.
			coin = (nGold % 10000) / 100;
			if( coin > 0 )
			{
				strText.append( L" " );
				swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1781 ), coin );			
				strText.append( str );
			}

			// ����.
			coin = nGold % 100;
			if( coin > 0 )
			{
				strText.append( L" " );
				swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1783 ), coin );			
				strText.append( str );
			}

			m_pItemSlotBtn->SetItem( m_pResultItem, 0 );
		}

		// ������ȹ��.
		else
		{
			strText.assign( m_pResultItem->GetName() );
						
			if( nPeriod <= 0 )
				m_pResultItem->SetEternityItem( true );	

			m_pItemSlotBtn->SetItem( m_pResultItem, nCount );		
		}	

		m_pStaticItemName->SetText( strText );
	}
}
#endif