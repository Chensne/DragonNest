#include "StdAfx.h"
#include "DnPlateMainDlg.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnPlateListSelectDlg.h"
#include "DnMainMenuDlg.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPlateMainDlg::CDnPlateMainDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pPlateListSelectDlg(NULL)
, m_pButtonMake(NULL)
, m_pButtonCancel(NULL)
, m_pSlotButton(NULL)
, m_pItem(NULL)
, m_eStep(eStepClose)
{
}

CDnPlateMainDlg::~CDnPlateMainDlg(void)
{
	SAFE_DELETE( m_pPlateListSelectDlg );
}

void CDnPlateMainDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PlateMainDlg.ui" ).c_str(), bShow );
}

void CDnPlateMainDlg::InitialUpdate()
{
	DN_CONTROL( ItemSlotButton , ID_ITEM_PLATE )->SetSlotIndex( 0 );
	DN_CONTROL( ItemSlotButton , ID_ITEM_PLATE )->SetSlotType( ST_PLATE );

	m_pButtonMake = GetControl<CEtUIButton>("ID_BUTTON_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");

	m_pPlateListSelectDlg = new CDnPlateListSelectDlg( UI_TYPE_CHILD, this );
	m_pPlateListSelectDlg->Initialize( false );

	m_pButtonMake->Enable( false );
}

void CDnPlateMainDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		EnableButton( true );
		ChangeStep(eStepPlate);
	}
	else
	{
		ChangeStep(eStepClose);
		GetInterface().CloseBlind();
	}

	CEtUIDialog::Show( bShow );
}

void CDnPlateMainDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_OK") )
		{
			CDnSlotButton *pDragButton = (CDnSlotButton *)drag::GetControl();
			if( pDragButton ) {
				pDragButton->DisableSplitMode(true);
				drag::ReleaseControl();
			}
			if( m_pButtonMake->IsEnable() )
				GetInterface().MessageBox( 9020, MB_YESNO, COMPOUND_CONFIRM_DIALOG, this );	//���� �Ͻðڽ��ϱ�?

			return;
		}
		if( IsCmdControl("ID_BUTTON_CANCEL") )
		{
			switch( m_eStep )
			{
			case eStepPlate :
				m_pParentDialog->CloseAllDialog();
				break;

			case eStepMake :
			case eStepRune :
				ChangeStep( eStepPlate );
				break;
			}
			return;
		}
		if( IsCmdControl("ID_ITEM_PLATE") )
		{
			// ��Ŭ������ ����
			if( uMsg == WM_RBUTTONUP )
			{
				ChangeStep( eStepPlate );
				return;
			}

			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			if( pDragButton )
			{
				pDragButton->DisableSplitMode(false);
				drag::ReleaseControl();
				if( false == SetPlateItem( (CDnQuickSlotButton*)pDragButton ) )
					pDragButton->DisableSplitMode(true);
			}
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_CREATE_PLATE);
		}
#endif
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPlateMainDlg::EnableButton( bool bEnable )
{
	m_pButtonMake->Enable( bEnable );
	m_pButtonCancel->Enable( bEnable );
}

void CDnPlateMainDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case COMPOUND_CONFIRM_DIALOG:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl("ID_YES") )
			{
				OutputDebug( "�÷���Ʈ ���� ��Ŷ ����\n" );
				GetItemTask().RequestEmblemCompoundOpen( 
					static_cast<CDnItem*>(m_pSlotButton->GetItem()), 
					m_pPlateListSelectDlg->GetSelectedCompoundID(), 
					m_pPlateListSelectDlg->GetCompoundInfo() );
				EnableButton( false );
			}
		}
		break;

	case COMPOUND_MOVIE_DIALOG:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			// �Ⱥ��̴� ��ư. ���μ��� ó���� �������� �˸���.
			if( IsCmdControl("ID_PROCESS_COMPLETE") )
			{
				// �Ϸ� ��Ŷ�� ������.
				// Send...
				GetItemTask().RequestEmblemCompound(
					static_cast<CDnItem*>(m_pSlotButton->GetItem()), 
					m_pPlateListSelectDlg->GetSelectedCompoundID(), 
					m_pPlateListSelectDlg->GetCompoundInfo() );

				OutputDebug( "���庸�� ���� �ϼ� ��Ŷ ����\n" );

				ChangeStep( eStepPlate );
			}
			else if( IsCmdControl("ID_BUTTON_CANCEL") )
			{
				// ��� ��Ŷ�� ������.
				// Send...
				OutputDebug( "���庸�� ���� �� ĵ�� ��Ŷ ����\n" );
				GetItemTask().RequestEmblemCompoundCancel();
			}
		}
		break;
	}
}

void CDnPlateMainDlg::OnRecvEmblemCompoundOpen()
{
	// �����Ϸ��� �����ۿ� ���� ������ �ð��� ���ؿ� ��
	int nCompoundID = m_pPlateListSelectDlg->GetSelectedCompoundID();
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
	GetItemTask().GetCompoundInfo( nCompoundID, &CompoundInfo );

	GetInterface().OpenMovieProcessDlg(
		"PlateCombine.avi", CompoundInfo.fTimeRequired / 1000.0f, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8035 ), COMPOUND_MOVIE_DIALOG, this );

	EnableButton( true );
	// ���� ȣ���ϴ°� �ƴ϶�, �ٸ� ������ ȣ��� this����Ϸ���,
	// GetInterface().GetMainMenuDialog(CDnMainMenuDlg::PLATE_LIST_SELECT_DIALOG)
}

void CDnPlateMainDlg::OnRecvEmblemCompound()
{
	/*
	// ���庸�� ���հ���� �����ϴ��� �����ϴ��� �⺻�������� ���ư���.
	ChangeStep( stepBase );
	*/
}

bool CDnPlateMainDlg::SetPlateItem( CDnQuickSlotButton *pPressedButton )
{
	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());

	// �ø��� �� �ø��°Ÿ� �н�
	if( m_pSlotButton && (pItem == m_pSlotButton->GetItem()) )
	{
		return false;
	}

	// ������ ���� �˻�
	if( pItem->GetItemType() == ITEMTYPE_QUEST )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
		return false;
	}

	// �÷���Ʈ ���������� �˻�.
	if( pItem->GetItemType() != ITEMTYPE_PLATE )
	{
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9008), true); // UISTRING : �÷���Ʈ�� ����� �� �ֽ��ϴ�.
		return false;
	}

	// ĭ�� �ϳ��̹Ƿ� �� �̻� ����� �� �����ϴ�.���� ������ ����.
	CDnItem *pOriginItem = static_cast<CDnItem *>(DN_CONTROL( ItemSlotButton , ID_ITEM_PLATE )->GetItem());
	if( pOriginItem )
	{
		// �ش� �������� â���� �����Ѵ�.
		SAFE_DELETE(m_pItem);
		DN_CONTROL( ItemSlotButton , ID_ITEM_PLATE )->ResetSlot();
		m_pSlotButton->SetRegist( false );
		m_pSlotButton->DisableSplitMode(true);
		m_pSlotButton = NULL;
	}

	m_pSlotButton = (CDnItemSlotButton *)pPressedButton;
	m_pSlotButton->SetRegist( true );

	TItemInfo itemInfo;
	pItem->GetTItemInfo(itemInfo);

	m_pItem = GetItemTask().CreateItem(itemInfo);
	pPressedButton->EnableSplitMode(CDnSlotButton::ITEM_ORIGINAL_COUNT);
	DN_CONTROL( ItemSlotButton , ID_ITEM_PLATE )->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	ChangeStep( eStepRune );
	
	return true;
}

int CDnPlateMainDlg::GetPlateItemID()
{
	if( eStepPlate < m_eStep )
	{
		CDnItem *pPlateItem = static_cast<CDnItem *>(m_pSlotButton->GetItem());
		return pPlateItem->GetClassID();
	}
	
	return 0;
}

void CDnPlateMainDlg::ChangeStep(eStep nStep)
{
	m_eStep = nStep;

	switch( nStep )
	{
	case eStepClose :
	case eStepPlate :
		SAFE_DELETE(m_pItem);
		m_pButtonMake->Enable(false);
		DN_CONTROL( ItemSlotButton , ID_ITEM_PLATE )->ResetSlot();
		if( m_pSlotButton )
		{
			m_pSlotButton->SetRegist( false );
			m_pSlotButton->DisableSplitMode(true);
			m_pSlotButton = NULL;
		}
		ShowChildDialog(m_pPlateListSelectDlg, false);
		break;

	case eStepRune :
		ShowChildDialog(m_pPlateListSelectDlg, true);
		m_pPlateListSelectDlg->RefreshPlateList();
		break;

	case eStepMake:
		//���� ���� ������� �˻�
		if( m_pPlateListSelectDlg->EnoughCoin() && m_pPlateListSelectDlg->EnoughRune() )
			m_pButtonMake->Enable( true );
		else
			m_pButtonMake->Enable( false );
		break;
	}
}