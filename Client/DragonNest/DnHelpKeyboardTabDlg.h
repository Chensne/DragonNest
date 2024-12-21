#pragma once

class CDnHelpKeyboardTabDlg : public CEtUIDialog
{
public:
	CDnHelpKeyboardTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnHelpKeyboardTabDlg();

protected:
	std::string m_szUIFileName;

public:
	virtual void Initialize( bool bShow );

	void SetUIFileName( const char *szFileName );
};