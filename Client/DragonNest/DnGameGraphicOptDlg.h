#pragma once
#include "DnOptionDlg.h"

class CDnGameGraphicConfirmDlg;

class CDnGameGraphicOptDlg : public CDnOptionDlg, public CEtUICallback
{
public:
	CDnGameGraphicOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGameGraphicOptDlg(void);

	enum eActorRefreshType
	{
		eNone = 0,
		eHideMainWeapon = 1,
		eCameraExtend = 2,
		eMax,
	};

protected:
	CEtUIButton *m_pButtonBrightDown;
	CEtUIButton *m_pButtonBrightUp;
	CEtUISlider *m_pSliderBright;
	CEtUIStatic *m_pStaticBright;

	CEtUIComboBox *m_pComboBoxQuality;
	CEtUIComboBox *m_pComboBoxResolution;

#ifdef PRE_ADD_37745
	CEtUICheckBox *m_pHideMainWeapon;
#endif

	CEtUICheckBox *m_pWindowMode; 

#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	CEtUIComboBox *m_pComboBoxTextureQuality;
	int m_nTextureQualityBack;
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	CEtUIComboBox *m_pComboBoxEffectQuality;
	int m_nEffectQualityBack;	
#endif

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	CEtUICheckBox *m_pCheckExclusivePlayerEffect;
#endif 

	CEtUICheckBox *m_pCheckVSync;


	CEtUICheckBox *m_pExtendView;
#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
	//CEtUIComboBox *m_pComboBoxMemoryOptimize;
	//int m_nMemoryOptimizeBack;
#endif

	int m_nResolutionBack;
	int m_nQualityBack;

	// 풀스크린으로 바꿀 경우엔 확인 메세지를 띄워준다.
	// 이때 취소를 누른다면, 
	bool m_bConfirmMessageBox;
	int m_nPreResolutionIndex;
	bool m_bPreWindow;
	bool m_bPreVSync;
	CDnGameGraphicConfirmDlg *m_pGameGraphicConfirmDlg;

	CEtUIComboBox *m_pComboBoxUISize;
	int m_nUISizeBack;

	CEtUIComboBox *m_pComboBoxScreenShotFormat;
	int m_nScreenShotBack;

	// 다른 옵션과 달리 옵션이 바뀔때 OnLost,OnReset 등 무거운 일을 해서,
	// Cur값과 동일한 선택을 할 경우 처리하지 않도록 하겠다.
	int m_nCurSelectedUISize;

	typedef CDnOptionDlg BaseClass;

protected:
	void SetResolution();
	void SetBright();
	void SetQuality();

#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	void SetTextureQuality();
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	void SetEffectQuality();
#endif

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	void SetEnableSkillEffect();
#endif 

	void SetWindowMode();
	void SetUISize();
	void Set_ETC_Effect();
	void RefreshActorView(eActorRefreshType eRefreshType);
	void SetVSync();
	void SetScreenShotFormat();
#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
	//void SetMemoryOptimize();
#endif

public:
	void ImportSetting() override;
	void ExportSetting() override;
	void CancelOption() override;
	bool IsChanged() override;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
