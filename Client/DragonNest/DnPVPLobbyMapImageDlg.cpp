#include "StdAfx.h"
#include "DnInterface.h"
#include "DnPVPLobbyMapImageDlg.h"
#include "DnTableDB.h"
#ifdef PRE_ADD_PVP_HELP_MESSAGE

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnPVPLobbyMapImageDlg::CDnPVPLobbyMapImageDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pTexture(NULL)
{
}

CDnPVPLobbyMapImageDlg::~CDnPVPLobbyMapImageDlg(void)
{
}

void CDnPVPLobbyMapImageDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpMapTooltip.ui" ).c_str(), bShow );
}

void CDnPVPLobbyMapImageDlg::InitialUpdate()
{
	m_pTexture = GetControl<CEtUITextureControl>("ID_PVP_MAPIMG");
}

void CDnPVPLobbyMapImageDlg::Show( bool bShow )
{
	if(bShow == m_bShow)
		return;

	if(m_pTexture)
		m_pTexture->Show(bShow);

	CEtUIDialog::Show(bShow);
}

void CDnPVPLobbyMapImageDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		UpdateTooltipPosition();
	}
}

void CDnPVPLobbyMapImageDlg::UpdateTooltipPosition()
{
	SUICoord uiCoord;
	GetDlgCoord(uiCoord);
	GetScreenMouseMovePoints(uiCoord.fX, uiCoord.fY);

	if( uiCoord.Right() > GetScreenWidthRatio() )
	{
		uiCoord.fX -= (uiCoord.Right() - GetScreenWidthRatio() - 0.001f);
	}

	if( uiCoord.Bottom() > GetScreenHeightRatio() )
	{
		uiCoord.fY -= (uiCoord.Bottom() - GetScreenHeightRatio() - 0.001f);
	}

	CommonUtil::ClipNumber(uiCoord.fY, 0.f, uiCoord.fY);

	SetDlgCoord(uiCoord);
}

void CDnPVPLobbyMapImageDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );
	
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if (IsCmdControl("ID_BUTTON_ROLLBACK"))
		{
		}
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		if (IsCmdControl("ID_BUTTON_ROLLBACK"))
		{
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPVPLobbyMapImageDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );
}

void CDnPVPLobbyMapImageDlg::SetDialogPosition(float fX, float fY)
{
	SUICoord DlgCoord;

	GetDlgCoord(DlgCoord);

	DlgCoord.fX = fX;
	DlgCoord.fY = fY;
	
	SetDlgCoord(DlgCoord);
}

bool CDnPVPLobbyMapImageDlg::LoadPVPMapImage(int nMapIndex)
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPMAP );
	if ( !pSox ) 
	{
		ErrorLog("void CDnPVPMakeRoomDlg::MakePvPMapData():: PVP Map table Not found!! ");
		return false;
	}

	if(nMapIndex < 0)
		return false;

	std::string szMapName;
	for( int i=0; i<pSox->GetItemCount() ; i++ ) 
	{
		if( pSox->GetFieldFromLablePtr( nMapIndex, "MapTableID" )->GetInteger() != nMapIndex )
			continue;

		if( pSox->GetFieldFromLablePtr( nMapIndex, "ReleaseShow" )->GetInteger() == 0 )
			continue;

		szMapName = pSox->GetFieldFromLablePtr( nMapIndex , "MapImage" )->GetString();

		if(!szMapName.empty())
			break;
	}
	
	if(szMapName.empty())
		return false;

	SAFE_RELEASE_SPTR( m_hTexture );
	m_hTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( szMapName.c_str() ).c_str(), RT_TEXTURE );
	
	if(m_hTexture)
		m_pTexture->SetTexture( m_hTexture, 0, 0, m_hTexture->Width(), m_hTexture->Height() );

	return true;
}



#endif