#pragma once
#include "DnDungeonEnterLevelDlg.h"

class CDnEventDungeonEnterLevelDlg : public CDnDungeonEnterLevelDlg
{
public:
	CDnEventDungeonEnterLevelDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnEventDungeonEnterLevelDlg();

protected:

protected:

public:
	virtual void SetDungeonDesc( const wchar_t *wszStr );
	virtual void SetSelectDungeonInfo( int nMapIndex, int nDifficult );

	virtual void UpdateDungeonInfo( CDnDungeonEnterDlg::SDungeonGateInfo *pInfo );
	virtual void UpdateLevelButton( char *cState, int nAbyssMinLvl, int nAbyssMaxLvl, int nAbyssNeedQuestID ) {}
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

	virtual int GetDungeonLevel() { return 0; }
};