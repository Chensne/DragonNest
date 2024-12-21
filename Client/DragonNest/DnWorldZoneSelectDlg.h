#pragma once

class CDnWorldZoneSelectDlg : public CEtUIDialog
{
public:
	CDnWorldZoneSelectDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnWorldZoneSelectDlg(void);

protected:
	CEtUIStatic * m_pStaticTitleName;
	CEtUIButton * m_pButtonStart;
	CEtUIButton * m_pButtonCancel;
	CEtUIButton * m_pButtonPartyDlgOpen;
	CEtUIListBoxEx * m_pListBox;
	
	CEtUIStatic * m_pStaticDungeon;
	CEtUIListBoxEx *ID_LISTBOX_DUNGEONLIST;

public:
	void EnableButtons();
	void SetMapInfo( const BYTE cGateNo );

	BYTE GetSelectIndex();
	void SetSelectIndex( BYTE cSelectIndex );

#ifdef PRE_ADD_QUESTCOUNT_IN_STAGELIST
	int	GetStageQuestCount( int nStageNumber );
#endif 

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );
};
