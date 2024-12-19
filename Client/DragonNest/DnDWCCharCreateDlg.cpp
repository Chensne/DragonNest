#include "stdafx.h"

#if defined(PRE_ADD_DWC)
#include "DnInterface.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnDWCCharCreateDlg.h"
#include "DnDWCCharCreateSelectDlg.h"
#include "DnDWCCharCreateButtonDlg.h"
#ifdef PRE_MOD_SELECT_CHAR
#include "DnCharLoginTitleDlg.h"
#endif // PRE_MOD_SELECT_CHAR

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDWCCharCreateDlg::CDnDWCCharCreateDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pDWCCharButtonDlg(NULL)
, m_pDWCCharSelectDlg(NULL)
, m_pTextureCtrl(NULL)
, m_pClassTextStatic(NULL)
#ifdef PRE_MOD_SELECT_CHAR
, m_pCreateCharTitleDlg(NULL)
#endif // PRE_MOD_SELECT_CHAR
{
}

CDnDWCCharCreateDlg::~CDnDWCCharCreateDlg()
{
#ifdef PRE_MOD_SELECT_CHAR
	SAFE_DELETE(m_pCreateCharTitleDlg);
#endif // PRE_MOD_SELECT_CHAR
	SAFE_DELETE(m_pDWCCharSelectDlg);
	SAFE_DELETE(m_pDWCCharButtonDlg);
}

void CDnDWCCharCreateDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvP_CharCreateBase.ui" ).c_str(), bShow );

	m_pDWCCharButtonDlg = new CDnDWCCharCreateButtonDlg(UI_TYPE_BOTTOM, this);
	m_pDWCCharButtonDlg->Initialize( false );

	m_pDWCCharSelectDlg = new CDnDWCCharCreateSelectDlg(UI_TYPE_CHILD, this);
	m_pDWCCharSelectDlg->Initialize( false );

#ifdef PRE_MOD_SELECT_CHAR
	m_pCreateCharTitleDlg = new CDnCharLoginTitleDlg(UI_TYPE_CHILD, this);
	m_pCreateCharTitleDlg->Initialize(false);
	m_pCreateCharTitleDlg->SetTitle(120246); // DWC ĳ���� ����
#endif // PRE_MOD_SELECT_CHAR
}

void CDnDWCCharCreateDlg::InitialUpdate()
{
	m_pTextureCtrl		= GetControl<CEtUITextureControl>("ID_TEXTUREL0");
	m_pClassTextStatic	= GetControl<CEtUIStatic>("ID_STATIC_NAME");
}

void CDnDWCCharCreateDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if(IsCmdControl(""))
		{

		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnDWCCharCreateDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	if(bShow)
	{
		ShowChildDialog(m_pCreateCharTitleDlg, bShow);
		ShowChildDialog(m_pDWCCharButtonDlg, bShow);
		ShowChildDialog(m_pDWCCharSelectDlg, bShow);		
	}
	else
	{
		ShowChildDialog(m_pCreateCharTitleDlg, false);
		ShowChildDialog(m_pDWCCharSelectDlg, false);
		ShowChildDialog(m_pDWCCharButtonDlg, false);
	}

	CEtUIDialog::Show(bShow);
}

void CDnDWCCharCreateDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );
}

void CDnDWCCharCreateDlg::ChangeCharImage(std::string strFileName, int nClassMidID)
{
	if(strFileName.empty()) return;

	EtTextureHandle hTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( strFileName.c_str() ).c_str(), RT_TEXTURE );
	if(hTexture) m_pTextureCtrl->SetTexture(hTexture, 0, 0, hTexture->Width(), hTexture->Height());
	
	std::wstring wszClassStr = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nClassMidID);
	m_pClassTextStatic->SetText(wszClassStr);
}

const INT64 CDnDWCCharCreateDlg::GetCharDBID()
{ 
	if(!m_pDWCCharSelectDlg)
		return 0;

	return m_pDWCCharSelectDlg->GetSelectCharDBID();
}

const int CDnDWCCharCreateDlg::GetCharJobID()
{
	if(!m_pDWCCharSelectDlg)
		return 0;

	return m_pDWCCharSelectDlg->GetSelectCharJobID();
}

#endif // PRE_ADD_DWC