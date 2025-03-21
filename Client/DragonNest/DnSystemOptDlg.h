#pragma once
#include "DnOptionTabDlg.h"

class CDnGameGraphicOptDlg;
class CDnGameSoundOptDlg;
class CDnGameControlOptDlg;
class CDnGameVoiceChatOptDlg;

class CDnSystemOptDlg : public CDnOptionTabDlg, public CEtUICallback
{
	enum
	{
		SYSTEM_CONTROL_DIALOG,
		SYSTEM_GRAPHIC_DIALOG,
		SYSTEM_SOUND_DIALOG,
		SYSTEM_VOICECHAT_DIALOG,
	};

public:
	CDnSystemOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnSystemOptDlg();

protected:
	CEtUIRadioButton *m_pTabButtonGraphic;
	CEtUIRadioButton *m_pTabButtonSound;
	CEtUIRadioButton *m_pTabButtonControl;
	CEtUIRadioButton *m_pTabButtonVoiceChat;

	CDnGameGraphicOptDlg *m_pGameGraphicOptDlg;
	CDnGameSoundOptDlg *m_pGameSoundOptDlg;
	CDnGameControlOptDlg *m_pGameControlOptDlg;
	CDnGameVoiceChatOptDlg *m_pGameVoiceChatOptDlg;

	typedef CDnOptionTabDlg BaseClass;

public: 
	void ImportSetting();
	void ExportSetting() {}
	bool IsChanged() { return false;}

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	/*virtual void Show( bool bShow );*/
	//virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};