#include "StdAfx.h"

#ifdef PRE_ADD_PVP_COMBOEXERCISE

#include "DnComboExerciseModeHUD.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnComboExerciseModeHUD::CDnComboExerciseModeHUD( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnPVPBaseHUD( dialogType, pParentDialog, nID, pCallback )
{
}

CDnComboExerciseModeHUD::~CDnComboExerciseModeHUD(void)
{
	
}

void CDnComboExerciseModeHUD::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpRespawnInGame_AllDlg.ui" ).c_str(), bShow );
}


void CDnComboExerciseModeHUD::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );
}

void CDnComboExerciseModeHUD::InitialUpdate()
{
	m_pRoomName = GetControl<CEtUIStatic>("ID_ROOMNAME"); //  SetRoomName 은 BaseHUD에 있다.

	// CDnPVPBaseHUD //
	CDnPVPBaseHUD::InitialUpdate();

	// 숨김.
	CEtUIStatic * pStatic = NULL;
	char * strID[16] = { "ID_BESTUSER", "ID_NAME", "ID_KILLCOUNT", "ID_TIME_MINUTE", "ID_TIME_SECOND", "ID_STATIC20" };
	for( int i=0; i<6; ++i )
	{
		pStatic = GetControl<CEtUIStatic>( strID[i] );
		if( pStatic )
			pStatic->Show( false );
	}

	// 보임.
	pStatic = GetControl<CEtUIStatic>("ID_STATIC_UNLIMIT"); // Tool Property - Init State : 1 // 기본으로 숨겨져있음.
	if( pStatic )
		pStatic->Show( true );

	pStatic = GetControl<CEtUIStatic>("ID_STATIC_SUMMON"); // Tool Property - Init State : 1 // 기본으로 숨겨져있음.
	if( pStatic )
		pStatic->Show( true );

}


void CDnComboExerciseModeHUD::ShowMaster( bool bMaster )
{
	CEtUIStatic * pStatic = GetControl<CEtUIStatic>("ID_STATIC_SUMMON");
	if( pStatic )
		pStatic->Show( bMaster );
}

#endif // PRE_ADD_PVP_COMBOEXERCISE

