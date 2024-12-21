#include "StdAfx.h"
#include "DnQuestMainDlg.h"
#include "DnQuestDescriptionDlg.h"
#include "EtUITreeCtl.h"
#include "EtUITextBox.h"

CDnQuestMainDlg::CDnQuestMainDlg( int nID, CEtUICallback *pCallback )
	: CEtUIDialog( nID, pCallback )
	, m_pDescriptionDlg(NULL)
	, m_pTreeControl(NULL)
	, m_pTextBoxTitle(NULL)
{
}

CDnQuestMainDlg::~CDnQuestMainDlg(void)
{
	SAFE_DELETE(m_pDescriptionDlg);
}

void CDnQuestMainDlg::Initialize()
{
	CEtUIDialog::Initialize( g_PathMng.GetFullPathName( "QuestMainDlg.ui" ).c_str() );

	m_pDescriptionDlg = new CDnQuestDescriptionDlg;
	m_pDescriptionDlg->Initialize();
	m_pDescriptionDlg->Show(false);

	m_pTreeControl = static_cast<CEtUITreeCtl*>(GetControl("ID_TREE_QUEST"));
	m_pTextBoxTitle = static_cast<CEtUITextBox*>(GetControl("ID_TITLE"));

	m_pTextBoxTitle->AddText( L"Chapter02 Resicast for the Universal carrier", textcolor::WHITE, UITEXT_SYMBOL );
	m_pTextBoxTitle->AddText( L"The Airbrush company now carry", textcolor::WHITE, UITEXT_SYMBOL );

	CTreeItem *pItem1;
	CTreeItem *pItem2;

	EtTextureHandle hTexture = LoadResource( "QuestFrameBase.dds", RT_TEXTURE );

	pItem1 = m_pTreeControl->AddItem();
	pItem1->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem1->SetText( L"Ch. 1\n악마성의 저주", textcolor::RED );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );
	pItem1 = m_pTreeControl->AddChildItem( pItem1 );
	pItem1->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem1->SetText( L"저주가 걸린 공주를 찾아라...", textcolor::YELLOW );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );
	pItem1 = m_pTreeControl->AddChildItem( pItem1 );
	pItem1->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem1->SetText( L"샬랄라 공주는 어디에...", textcolor::GOLD );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );

	pItem1 = m_pTreeControl->AddItem();
	pItem1->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem1->SetText( L"Ch. 2\n악마성의 저주", textcolor::RED );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );
	pItem1 = m_pTreeControl->AddChildItem( pItem1 );
	pItem1->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem1->SetText( L"저주가 걸린 공주를 찾아라...", textcolor::YELLOW );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );
	pItem1 = m_pTreeControl->AddChildItem( pItem1 );
	pItem1->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem1->SetText( L"샬랄라 공주는 어디에...", textcolor::GOLD );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );

	pItem1 = m_pTreeControl->AddItem();
	pItem1->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem1->SetText( L"Ch. 3\n적의 포로로 잡힌 2MB를 제거하라.", textcolor::RED );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );
	pItem2 = m_pTreeControl->AddChildItem( pItem1 );
	pItem2->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem2->SetText( L"저주가 걸린 공주를 찾아라...", textcolor::YELLOW );
	pItem2->SetTexture( hTexture, 0, 0, 325, 50 );
	pItem1 = m_pTreeControl->AddChildItem( pItem1 );
	pItem1->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem1->SetText( L"샬랄라 공주는 어디에...", textcolor::YELLOW );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );
	pItem2 = m_pTreeControl->AddChildItem( pItem2 );
	pItem2->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem2->SetText( L"저주가 걸린 공주를 찾아라...", textcolor::GOLD );
	pItem2->SetTexture( hTexture, 0, 0, 325, 50 );

	pItem1 = m_pTreeControl->AddItem();
	pItem1->SetText( L"Ch. 4\n무궁화 꽃이...", textcolor::RED );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );
	pItem1 = m_pTreeControl->AddChildItem( pItem1 );
	pItem1->SetText( L"필까? 말까? 영구없다~...", textcolor::YELLOW );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );

	pItem1 = m_pTreeControl->AddItem();
	pItem1->SetOpenType( CTreeItem::ITEM_OPEN );
	pItem1->SetText( L"Ch. 5\n무궁화 꽃이...", textcolor::RED );
	pItem1->SetTexture( hTexture, 0, 0, 325, 50 );
}

void CDnQuestMainDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_TREECONTROL_SELECTION )
	{
		if( IsCmdControl( "ID_TREE_QUEST" ) )
		{
			m_pDescriptionDlg->Show(true);
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}