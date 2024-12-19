#include "StdAfx.h"
#include "DnStaffrollFont2Dlg.h"

CDnStaffrollFont2Dlg::CDnStaffrollFont2Dlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
{
	m_pStaticText = NULL;
	m_dwOrigColor = m_dwOrigShadowColor = 0;
	m_dwOrigTexColor = 0;
}

CDnStaffrollFont2Dlg::~CDnStaffrollFont2Dlg()
{
}

void CDnStaffrollFont2Dlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StaffRollPositionDlg.ui" ).c_str(), bShow );
}

void CDnStaffrollFont2Dlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>( "ID_STATIC0" );
	m_dwOrigColor = m_pStaticText->GetTextColor();
	m_dwOrigShadowColor = m_pStaticText->GetShadowColor();
	m_dwOrigTexColor = m_pStaticText->GetTextureColor();
}

void CDnStaffrollFont2Dlg::SetText( const WCHAR *wszStr )
{
	m_pStaticText->SetText( wszStr );
}

void CDnStaffrollFont2Dlg::SetAlpha( float fAlpha )
{
	DWORD dwColor = ( m_dwOrigColor & 0x00ffffff ) | (DWORD)(((BYTE)(fAlpha*255)&0xff)<<24);
	DWORD dwShadowColor = ( m_dwOrigShadowColor & 0x00ffffff ) | (DWORD)(((BYTE)(fAlpha*255)&0xff)<<24);
	DWORD dwTexColor = ( m_dwOrigTexColor & 0x00ffffff ) | (DWORD)(((BYTE)(fAlpha*255)&0xff)<<24);

	m_pStaticText->SetTextureColor( dwColor );
	m_pStaticText->SetTextColor( dwColor );
	m_pStaticText->SetShadowColor( dwShadowColor );
}