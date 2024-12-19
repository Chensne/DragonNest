#include "StdAfx.h"
#include "DnNpcBlind.h"
#include "DnNpcDlg.h"
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnNpcBlind::CDnNpcBlind( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnBlindDlg( dialogType, pParentDialog, nID, pCallback  )
{
	SetBillBoardColor(0x00000000);
}

CDnNpcBlind::~CDnNpcBlind(void)
{
}

void CDnNpcBlind::Process( float fElapsedTime )
{
	if( GetBlindMode() == modeClosed )	return;
	if( GetBlindMode() == modeOpened )	return;

	CDnBlindDlg::Process( fElapsedTime );

	if( GetBlindMode() == modeOpened )
	{
		GetNpcDlg().Open();
	}
	else if( GetBlindMode() == modeClosed )
	{
		GetInterface().OpenBaseDialog();
		CDnLocalPlayerActor::SetTakeNpcUID(0xffffffff);
		CDnLocalPlayerActor::LockInput(false);
	}
}