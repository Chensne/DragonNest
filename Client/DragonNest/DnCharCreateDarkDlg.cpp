#include "StdAfx.h"
#include "DnCharCreateDarkDlg.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnTableDB.h"
#include "DnJobIconStatic.h"
#include "DnInterface.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnCharCreateDarkDlg::CDnCharCreateDarkDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pTextureBack = NULL;

	m_pTextureClass[0] = NULL;
	m_pTextureClass[1] = NULL;

	m_pRadioButton[0] = NULL;
	m_pRadioButton[1] = NULL;

	m_pClassName = NULL;
}

CDnCharCreateDarkDlg::~CDnCharCreateDarkDlg(void)
{
}

void CDnCharCreateDarkDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "dark_charcreateselect.ui" ).c_str(), bShow );
}

void CDnCharCreateDarkDlg::InitialUpdate()
{
	//ID_TEXTUREL_IMAGE0 Dark Avenger
	//ID_TEXTUREL_IMAGE1 Silver Hunter
	//ID_TEXTUREL_BACK big picture
	
	m_pTextureBack = GetControl<CEtUITextureControl>("ID_TEXTUREL_BACK");
	
	GetControl<CEtUIStatic>("ID_STATIC_BAR")->Show(true);
	//m_pTextureBack->SetTextureColor(0x00000000);

	SAFE_RELEASE_SPTR(m_hBgTextureClass[0]);
	SAFE_RELEASE_SPTR(m_hBgTextureClass[1]);
	SAFE_RELEASE_SPTR(m_hBgTexture);

	for (int i = 0; i < DARK_CLASS_MAX; i++)
	{
		m_pTextureClass[i] = GetControl<CEtUITextureControl>(FormatA("ID_TEXTUREL_IMAGE%d",i).c_str());
		m_pRadioButton[i] = GetControl<CEtUIRadioButton>(FormatA("ID_RBT%d", i).c_str());
	}

	m_hBgTexture = LoadResource("dwcavenger.dds", RT_TEXTURE, true);
	
	//Dark Avenger Icon
	m_hBgTextureClass[0] = LoadResource("side_avenger.dds", RT_TEXTURE, true);
	//Silver Hunter Icon
	m_hBgTextureClass[1] = LoadResource("side_sliverhunter.dds", RT_TEXTURE, true);

	for (int i = 0; i < DARK_CLASS_MAX; i++)
	{
		m_pTextureClass[i]->SetTexture(m_hBgTextureClass[i]);
		m_pTextureClass[i]->Show(true);
	}

	//butoane pt create char
	m_pRadioButton[0]->SetTooltipText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 374 ));
	m_pRadioButton[1]->SetTooltipText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 381 ));
	

	//Text of character name
	m_pClassName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pClassName->Show(false);

	//hide and show after character select
	GetControl<CEtUIButton>("ID_BT_OK")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_BOARD")->Show(false);


}

void CDnCharCreateDarkDlg::Show( bool bShow )
{
	if( bShow == m_bShow )
		return;
	
	this->ResetSelection();

	CEtUIDialog::Show( bShow );

	//CDnCustomDlg::Show(bShow);
}


void CDnCharCreateDarkDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );
}

void CDnCharCreateDarkDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( !pLoginTask ) return;


	if( EVENT_RADIOBUTTON_CHANGED == nCommand )
	{
		if( strstr( pControl->GetControlName(), "ID_RBT" ) )
		{
			int JobID=0;
			int nSelectIndex = static_cast<CEtUIRadioButton*>( pControl )->GetTabID();
			switch(nSelectIndex){
			case 0:
				JobID = 8;  //Dark Avenger , 9 = machina
				break;
			case 1:
				JobID = 10; //Silver H
				break;
			}
			//pLoginTask->SelectMoveCharacter( nSelectIndex ); // :D
			SetClassName(nSelectIndex);
			pLoginTask->SetSelectedCharacterDark(JobID);
			//GetInterface().SetCharCreateSelect( CharId );
			//

		}
	}

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if( IsCmdControl( "ID_BT_OK" ) ) 
		{
			pLoginTask->StartCharacterChangeParts();
		}
	}
}


void CDnCharCreateDarkDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );	
	
	/*if(m_hBgTexture)
	{
		SUICoord ScreenCoord( 0.0f, 0.0f, 1.0f, 1.0f );
		SUICoord UVCoord;

		int nWidth = CEtDevice::GetInstance().Width();
		int nHeight = CEtDevice::GetInstance().Height();

		float fTemp = ( ( nHeight / 9.f ) * 16.f - nWidth ) / nWidth;
		UVCoord.fX = fTemp / 2.0f;
		UVCoord.fY = 0.0f;
		UVCoord.fWidth = 1.0f - fTemp;
		UVCoord.fHeight = 1.0f;

		CEtSprite::GetInstance().DrawSprite( (EtTexture *)m_hBgTexture->GetTexturePtr(),
			m_hBgTexture->Width(), m_hBgTexture->Height(), UVCoord, 0xFFFFFFFF, ScreenCoord, 0.0f );
	}*/
}


void CDnCharCreateDarkDlg::SetClassName( int id )
{
	if(!m_pClassName->IsShow())
	{
		m_pClassName->Show(true);
		GetControl<CEtUIButton>("ID_BT_OK")->Show(true);
		GetControl<CEtUIStatic>("ID_STATIC_BOARD")->Show(true);
	}

	switch(id)
	{
	case 0:
		m_pClassName->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 374 )); //uistring: 374
		m_hBgTexture = LoadResource("dwcavenger.dds", RT_TEXTURE, true);
		m_pTextureBack->SetTexture(m_hBgTexture);
		//this->ResetCheckbox();
	//	m_pRadioButton[0]->SetChecked(true);
		break;
	case 1:
		m_pClassName->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 381 )); //uistring: 381
		m_hBgTexture = LoadResource("dwcsliverhunter.dds", RT_TEXTURE, true); // NEED CHECK!
		m_pTextureBack->SetTexture(m_hBgTexture);
	//	this->ResetCheckbox();
	//	m_pRadioButton[1]->SetChecked(true);
		break;
	default:
		break;
	}
}


void CDnCharCreateDarkDlg::ResetSelection( )
{
	if(m_hBgTexture)
		m_hBgTexture->Release();
	if(m_pTextureBack)
		m_pTextureBack->DeleteTexture();
	if(m_pClassName)
		m_pClassName->Show(false);

	GetControl<CEtUIButton>("ID_BT_OK")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_BOARD")->Show(false);
}

void CDnCharCreateDarkDlg::ResetCheckbox()
{
	for (int i = 0; i < DARK_CLASS_MAX; i++)
		m_pRadioButton[i]->SetChecked(false);
}