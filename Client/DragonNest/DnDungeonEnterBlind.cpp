#include "StdAfx.h"
#include "DnDungeonEnterBlind.h"
#include "DnDungeonEnterDlg.h"
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"

CDnDungeonEnterBlind::CDnDungeonEnterBlind( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnBlindDlg( dialogType, pParentDialog, nID, pCallback  )
{
	SetBillBoardColor(0x50000000);
}

CDnDungeonEnterBlind::~CDnDungeonEnterBlind(void)
{
}

void CDnDungeonEnterBlind::Process( float fElapsedTime )
{
	if( GetBlindMode() == modeClosed )	return;
	if( GetBlindMode() == modeOpened )	return;

	CDnBlindDlg::Process( fElapsedTime );

	if( GetBlindMode() == modeOpened )
	{
		GetDungeonEnterDlg().Show(true);
	}
	else if( GetBlindMode() == modeClosed )
	{
		GetInterface().OpenBaseDialog();
		CDnLocalPlayerActor::LockInput(false);
	}
}