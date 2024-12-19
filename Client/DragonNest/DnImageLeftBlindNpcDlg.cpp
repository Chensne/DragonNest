#include "StdAfx.h"
#include "DnImageLeftBlindNpcDlg.h"

#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
CDnImageLeftBlindNpcDlg::CDnImageLeftBlindNpcDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pTextureControl(NULL)
, m_pNameStatic(NULL)
{
}

CDnImageLeftBlindNpcDlg::~CDnImageLeftBlindNpcDlg()
{
	SAFE_RELEASE_SPTR( m_hTexture );
}

void CDnImageLeftBlindNpcDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "NpcLeftDlg.ui" ).c_str(), bShow );
}

void CDnImageLeftBlindNpcDlg::InitialUpdate()
{
	m_pTextureControl = GetControl<CEtUITextureControl>("ID_TEXTUREL_NPCLEFT");
	//m_pNameStatic = GetControl<CEtUIStatic>("ID_STATIC_BLIND");
}

void CDnImageLeftBlindNpcDlg::Show( bool bShow )
{
	if(bShow == m_bShow) return;

	if(bShow)
	{
	}
	else
	{
		SAFE_RELEASE_SPTR(m_hTexture);
	}

	CEtUIDialog::Show(bShow);
}

void CDnImageLeftBlindNpcDlg::SetNpcTexture( WCHAR* wszNPCName, const char* szTextureFileName, bool bIsFlipH/* = false*/)
{
	Reset();

	if(m_pNameStatic && !wszNPCName)
		m_pNameStatic->SetText(wszNPCName);

	m_hTexture = EternityEngine::LoadTexture( szTextureFileName );
	if(m_hTexture) 
	{
		int nWidth  = m_hTexture->Width();
		int nHeight = m_hTexture->Height();
		
		if(bIsFlipH)
			m_pTextureControl->SetFlipHTexture( m_hTexture, 0, 0, nWidth, nHeight, bIsFlipH );
		else
			m_pTextureControl->SetTexture( m_hTexture, 0, 0, nWidth, nHeight);
	}
}

void CDnImageLeftBlindNpcDlg::Reset()
{
	if(m_pNameStatic)
		m_pNameStatic->ClearText();
	if(m_pTextureControl)
		m_pTextureControl->DeleteTexture();
}

#endif // PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
