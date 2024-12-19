#include "StdAfx.h"

#ifdef PRE_ADD_CHAOSCUBE

#include "DnChaosCubeResultDlg.h"

#ifdef PRE_ADD_CHOICECUBE
#include "DnChoiceCubeDlg.h"
#endif

#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnItem.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChaosCubeResultDlg::CDnChaosCubeResultDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pItemSlotButton(NULL)
, m_pStaticItemName(NULL)
{
#if defined(PRE_FIX_43986)
	m_nSoundIndex = -1;
#endif // PRE_FIX_43986
}

CDnChaosCubeResultDlg::~CDnChaosCubeResultDlg(void)
{	
}

void CDnChaosCubeResultDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenBoxResultDlg.ui" ).c_str(), bShow );
}

void CDnChaosCubeResultDlg::InitialUpdate()
{
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_BUTTON0");
	m_pStaticItemName = GetControl<CEtUIStatic>("ID_STATIC0");
	m_SmartMove.SetControl( GetControl<CEtUIButton>("ID_OK") );

#if defined(PRE_FIX_43986)
	m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( "UI_CharmItemConfirm.ogg", false, false );
#endif // PRE_FIX_43986
}

void CDnChaosCubeResultDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_SmartMove.MoveCursor();

		//사운드 출력..
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

void CDnChaosCubeResultDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show(false);

			// ChaosCube.
			if( GetInterface().IsOpenChaosCubeDlg() )
				GetInterface().RecvResultItemComplet(); // 결과아이템 수령완료.

#ifdef PRE_ADD_CHOICECUBE
			// ChaosCube.
			else if( GetInterface().IsOpenChoiceCubeDlg() )
				GetInterface().RecvChoiceResultItemComplet(); // 결과아이템 수령완료.
#endif

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnChaosCubeResultDlg::SetResultItem( int nItemID, int nCount )
{
	TItemInfo itemInfo;
	if( CDnItem::MakeItemInfo( nItemID, 1, itemInfo ) == false ) return;	
	CDnItem * pItem = GetItemTask().CreateItem( itemInfo );

	if( pItem )
	{	
		wchar_t str[256] = {0,};
		std::wstring strText;			

		if( nItemID == 1073750029 )
		{
			INT64 nGold = nCount;
			// 골드획득.
			if( nGold > 0 )
			{	
				// 골드.
				INT64 coin = nGold / 10000;
				if( coin > 0 )
				{
					swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1766 ), coin );
					strText.assign( str );
				}

				// 실버.
				coin = (nGold % 10000) / 100;
				if( coin > 0 )
				{
					strText.append( L" " );
					swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1781 ), coin );			
					strText.append( str );
				}

				// 코퍼.
				coin = nGold % 100;
				if( coin > 0 )
				{
					strText.append( L" " );
					swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1783 ), nGold%100 );			
					strText.append( str );
				}

				m_pItemSlotButton->SetItem( pItem, 1 );
				m_pStaticItemName->SetText( strText );
			}
		}

		// 아이템획득.
		else
		{
			pItem->SetOverlapCount( nCount );
			m_pItemSlotButton->SetItem( pItem, nCount );

			swprintf_s( str, 256, L"%s", pItem->GetName() ); // 아이템명.
			
#ifdef PRE_ADD_CHOICECUBE

			// 기간제아이템의 경우 기간 텍스트추가.]			
			strText.assign( str );
			CDnChoiceCubeDlg * pDlg = GetInterface().GetChoiceCubeDlg();
			if( GetInterface().IsOpenChoiceCubeDlg() && pDlg )
			{
				int nPeriod = pDlg->GetPeriodItemByID( pItem->GetClassID() );
				if( nPeriod > 0 )
				{					
					swprintf_s( str, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4613 ), nPeriod );
					strText.append( str );					
				}
			}	

			m_pStaticItemName->SetText( strText.c_str() );	
#else

		m_pStaticItemName->SetText( str );

#endif					
			
			

		}
		
	}

}


#endif