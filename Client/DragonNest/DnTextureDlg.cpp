#include "StdAfx.h"
#include "DnTextureDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTextureDlg::CDnTextureDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCommDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_pTextureControl(NULL)
	, m_bDeleteTexture(false)
{
	m_fX = 0.0f;
	m_fY = 0.0f;
	m_nPos = 0;
}

CDnTextureDlg::~CDnTextureDlg(void)
{
}

void CDnTextureDlg::Initialize( bool bShow )
{
	if( !m_pTextureControl )
	{
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "TextureDlg.ui" ).c_str(), bShow );
		SetFadeMode( CEtUIDialog::CancelRender );
	}
}

void CDnTextureDlg::InitialUpdate()
{
	m_pTextureControl = GetControl<CEtUITextureControl>("ID_TEXTURE_01");
}

void CDnTextureDlg::Process( float fElapsedTime )
{
	if( m_fDelayTime > 0.0f )
	{
		m_fDelayTime -= fElapsedTime;
	}
	else
	{
		if( m_bDeleteTexture )
		{
			SUIElement *pElement = m_pTextureControl->GetElement(0);
			if( pElement && pElement->TextureColor.dwCurrentColor == 0 )
			{
				DeleteTexture();
			}
		}
	}

	CEtUIDialog::Process( fElapsedTime );
}

void CDnTextureDlg::Show( bool bShow )
{ 
	if( bShow )
	{
	}
	else
	{
		m_fX = 0.0f;
		m_fY = 0.0f;
		m_nPos = 0;
		m_fDelayTime = 0.0f;
	}

	m_pTextureControl->Show( bShow );
	CEtUIDialog::Show( bShow );
}

void CDnTextureDlg::SetTexture( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, float fDelayTime )
{
	if( !m_pTextureControl )
		return;

	m_fX = fX;
	m_fY = fY;
	m_nPos = nPos;
	m_fDelayTime = fDelayTime;

	m_pTextureControl->SetTexture( hTextureHandle, 0, 0, hTextureHandle->Width(), hTextureHandle->Height() );
	m_bDeleteTexture = true;

	UpdateTextureBox();
}

void CDnTextureDlg::SetTexture( EtTextureHandle hTextureHandle, float fX, float fY, float fDelayTime )
{
	int nTextureWidth = hTextureHandle->Width();
	int nTextureHeight = hTextureHandle->Height();

	m_pTextureControl->SetTexture( hTextureHandle, 0, 0, nTextureWidth, nTextureHeight );

	float fWidth = nTextureWidth / GetScreenWidth();
	float fHeight = nTextureHeight / GetScreenHeight();

	SUICoord uiCoord;
	GetDlgCoord( uiCoord );
	uiCoord.SetPosition( fX, fY );
	uiCoord.SetSize( fWidth, fHeight );
	SetDlgCoord( uiCoord );

	uiCoord.SetPosition( 0.0f, 0.0f );
	uiCoord.SetSize( fWidth, fHeight );
	m_pTextureControl->SetUICoord( uiCoord );

	m_fDelayTime = fDelayTime;
	m_bDeleteTexture = true;
}

#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
void CDnTextureDlg::SetTexture( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, bool bAutoCloseDialog, float fDelayTime)
{
	if( !m_pTextureControl )
		return;

	m_fX = fX;
	m_fY = fY;
	m_nPos = nPos;
	m_fDelayTime = fDelayTime;
	m_bAutoCloseDialog = bAutoCloseDialog;
	m_DlgType = CDnCommDlg::eCommDlgType::eCommDlg_TYPE_NPCTALK;

	m_pTextureControl->SetTexture( hTextureHandle, 0, 0, hTextureHandle->Width(), hTextureHandle->Height() );
	m_bDeleteTexture = true;

	UpdateTextureBox();
}
#endif

void CDnTextureDlg::DeleteTexture()
{
	if( !m_pTextureControl ) return;
	m_pTextureControl->DeleteTexture();

	m_bDeleteTexture = false;
}

void CDnTextureDlg::UpdateTextureBox()
{
	EtTextureHandle hTexture = m_pTextureControl->GetTextrue();
	if( !hTexture ) return;

	float fWidth = hTexture->Width() / GetScreenWidth();
	float fHeight = hTexture->Height() / GetScreenHeight();

	// 다이얼로그 사이즈 셋팅
	SUICoord uiCoord;
	GetDlgCoord( uiCoord );
	uiCoord.SetSize( fWidth, fHeight );
	SetDlgCoord( uiCoord );

	// 텍스쳐 컨트롤 사이즈 셋팅
	uiCoord.SetPosition( 0.0f, 0.0f );
	uiCoord.SetSize( fWidth, fHeight );
	m_pTextureControl->SetUICoord( uiCoord );

	GetDlgCoord( uiCoord );

	float fRatioX = m_fX*GetScreenWidthRatio();
	float fRatioY = m_fY*GetScreenHeightRatio();

	switch( m_nPos )
	{
	case 0: // 왼쪽, 위
		uiCoord.SetPosition( fRatioX, fRatioY );
		break;
	case 1: //	가운데, 위
		uiCoord.SetPosition( fRatioX-(uiCoord.fWidth/2.0f), fRatioY );
		break;
	case 2: // 오른쪽, 위
		uiCoord.SetPosition( fRatioX-uiCoord.fWidth, fRatioY );
		break;
	case 3: // 왼쪽, 가운데
		uiCoord.SetPosition( fRatioX, fRatioY-(uiCoord.fHeight/2.0f) );
		break;
	case 4: // 오른쪽 가운데
		uiCoord.SetPosition( fRatioX-uiCoord.fWidth, fRatioY-(uiCoord.fHeight/2.0f) );
		break;
	case 5: // 왼쪽, 아래
		uiCoord.SetPosition( fRatioX, fRatioY-uiCoord.fHeight );
		break;
	case 6: // 가운데, 아래
		uiCoord.SetPosition( fRatioX-(uiCoord.fWidth/2.0f), fRatioY-uiCoord.fHeight );
		break;
	case 7: // 오른쪽, 아래
		uiCoord.SetPosition( fRatioX-uiCoord.fWidth, fRatioY-uiCoord.fHeight );
		break;
	case 8: // 가운데, 가운데 - 빠져있길래 추가해둔다.
		uiCoord.SetPosition( fRatioX-(uiCoord.fWidth/2.0f), fRatioY-(uiCoord.fHeight/2.0f) );
		break;
	default:
		ASSERT(0&&"CDnTextureDlg::SetTexture, default case!");
		break;
	}

	SetDlgCoord( uiCoord );
}

void CDnTextureDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();
	UpdateTextureBox();
}
