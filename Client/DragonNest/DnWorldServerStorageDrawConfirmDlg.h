#pragma once
#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"
#include "DnWorldServerStorageChargeConfirmDlg.h"

class CDnItem;
class CDnWorldServerStorageDrawConfirmDlg : public CDnWorldServerStorageChargeConfirmDlg
{
public:
	CDnWorldServerStorageDrawConfirmDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);

	virtual void InitialUpdate();
};

//////////////////////////////////////////////////////////////////////////

class CDnWorldServerStorageDrawConfirmExDlg : public CDnWorldServerStorageChargeConfirmExDlg
{
public:
	CDnWorldServerStorageDrawConfirmExDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);

	virtual void InitialUpdate();
};