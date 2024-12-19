#include "stdafx.h"
#include "DnOccupationModeClimaxDlg.h"
#include "DnOccupationModeHUD.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnOccupationModeClimaxDlg::CDnOccupationModeClimaxDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{

}

CDnOccupationModeClimaxDlg::~CDnOccupationModeClimaxDlg(void)
{
	
}

void CDnOccupationModeClimaxDlg::InitialUpdate()
{
	CDnClimaxEffect::m_pBaseTexture = GetControl<CEtUITextureControl>("ID_TEXTUREL_CLIMAX");
	CDnClimaxEffect::m_pScaleTexture = GetControl<CEtUITextureControl>("ID_TEXTUREL_CLIMAXAFTER");

	CDnClimaxEffect::m_hClimaxTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Pvp_Climax.dds" ).c_str(), RT_TEXTURE );
	CDnClimaxEffect::m_pBaseTexture->SetTexture( m_hClimaxTexture );
	CDnClimaxEffect::m_pScaleTexture->SetTexture( m_hClimaxTexture );

	CDnClimaxEffect::Init( 0.5f, 1.f, 3.f, 1.f, 2.f, false );
}

void CDnOccupationModeClimaxDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvPFlagClimaxDlg.ui" ).c_str(), bShow );
}


void CDnOccupationModeClimaxDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !CDnClimaxEffect::IsShow() )
		Show( false );
	else
		CDnClimaxEffect::Process( fElapsedTime );
}

void CDnOccupationModeClimaxDlg::StartClimaxEffect()
{
	Show( true );

	CDnClimaxEffect::Start();
}
