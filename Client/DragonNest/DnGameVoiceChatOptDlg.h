#pragma once
#include "DnOptionDlg.h"

#ifdef _USE_VOICECHAT
class CDnGameVoiceChatOptDlg : public CDnOptionDlg
{
public:
	CDnGameVoiceChatOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGameVoiceChatOptDlg(void);

protected:
	CEtUICheckBox *m_pVoiceChat;

	CEtUIButton *m_pButtonDown;
	CEtUIButton *m_pButtonUp;
	CEtUISlider *m_pSliderVoiceChat;
	CEtUIStatic *m_pStaticVoiceChat;

	CEtUIButton *m_pButtonDown2;
	CEtUIButton *m_pButtonUp2;
	CEtUISlider *m_pSliderMic;
	CEtUIStatic *m_pStaticMic;

	enum
	{
		MAX_NUM_TEST = 16
	};
	std::vector<CEtUIStatic *> m_vecStaticTest;

	CEtUIButton *m_pButtonTest;
	CEtUIStatic *m_pStaticTest;
	float m_fTestTime;

	CEtUICheckBox *m_pMicBoost;
	CEtUIStatic *m_pStaticMicBoost;

	CEtUICheckBox *m_pCheckBoxAutoVolume;
	CEtUIButton *m_pButtonDown3;
	CEtUIButton *m_pButtonUp3;
	CEtUISlider *m_pSliderVolume;
	CEtUIStatic *m_pStaticVolume;

	CEtUIComboBox *m_pComboCaptureDevice;

	CEtUIButton *m_pButtonPremium;

protected:	
	void CheckEnableControl();

public:
	void ExportSetting() override;
	void ImportSetting() override;
	virtual bool IsChanged() override;
	void CancelOption() override;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	
	virtual void Process( float fElapsedTime );
};
#endif