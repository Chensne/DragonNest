#include "StdAfx.h"
#include "DnStaffrollFont3Dlg.h"

CDnStaffrollFont3Dlg::CDnStaffrollFont3Dlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
{
	m_pStaticText = NULL;
	m_dwOrigColor = m_dwOrigShadowColor = m_dwOrigTexColor = 0;
}

CDnStaffrollFont3Dlg::~CDnStaffrollFont3Dlg()
{
}

void CDnStaffrollFont3Dlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StaffRollNameDlg.ui" ).c_str(), bShow );
}

void CDnStaffrollFont3Dlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>( "ID_TEXT_NAME" );
	m_dwOrigColor = m_pStaticText->GetTextColor();
	m_dwOrigShadowColor = m_pStaticText->GetShadowColor();
	m_dwOrigTexColor = m_pStaticText->GetTextureColor();
}

void CDnStaffrollFont3Dlg::SetText( const WCHAR *wszStr )
{
	m_pStaticText->SetText( wszStr );
}

void CDnStaffrollFont3Dlg::SetAlpha( float fAlpha )
{
	DWORD dwColor = ( m_dwOrigColor & 0x00ffffff ) | (DWORD)(((BYTE)(fAlpha*255)&0xff)<<24);
	DWORD dwShadowColor = ( m_dwOrigShadowColor & 0x00ffffff ) | (DWORD)(((BYTE)(fAlpha*255)&0xff)<<24);
	DWORD dwTexColor = ( m_dwOrigTexColor & 0x00ffffff ) | (DWORD)(((BYTE)(fAlpha*255)&0xff)<<24);

	m_pStaticText->SetTextureColor( dwColor );
	m_pStaticText->SetTextColor( dwColor );
	m_pStaticText->SetShadowColor( dwShadowColor );
}