#include "StdAfx.h"
#include "DnDungeonClearBlind.h"
#include "DnDungeonClearDlg.h"
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"

CDnDungeonClearBlind::CDnDungeonClearBlind( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnBlindDlg( dialogType, pParentDialog, nID, pCallback  )
{
	SetBillBoardColor(0x50000000);
}

CDnDungeonClearBlind::~CDnDungeonClearBlind(void)
{
}

void CDnDungeonClearBlind::Process( float fElapsedTime )
{
	if( GetBlindMode() == modeClosed )	return;
	if( GetBlindMode() == modeOpened )	return;

	CDnBlindDlg::Process( fElapsedTime );

	if( GetBlindMode() == modeOpened )
	{
		GetDungeonClearDlg().Show(true);
	}
	else if( GetBlindMode() == modeClosed )
	{
		GetInterface().OpenBaseDialog();
		CDnLocalPlayerActor::LockInput(false);
	}
}