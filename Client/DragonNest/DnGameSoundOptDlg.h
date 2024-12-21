#pragma once
#include "DnOptionDlg.h"

class CDnGameSoundOptDlg : public CDnOptionDlg
{
	struct SVolumeSlider
	{
		int m_nCurVolume;
		CEtUIButton *m_pButtonDown;
		CEtUIButton *m_pButtonUp;
		CEtUISlider *m_pSlider;
		CEtUICheckBox *m_pCheckBox;
		CEtUIStatic *m_pStatic;

		SVolumeSlider()
			: m_nCurVolume(0)
			, m_pButtonDown(NULL)
			, m_pButtonUp(NULL)
			, m_pSlider(NULL)
			, m_pCheckBox(NULL)
			, m_pStatic(NULL)
		{
		}

		void SetVolume( float fVolume )
		{
			m_nCurVolume = int(fVolume*100.0f+0.01f);
			m_pStatic->SetIntToText( m_nCurVolume );
			m_pSlider->SetValue( m_nCurVolume );
		}

		float GetVolume() { return m_nCurVolume/100.0f; }

		void SetMute( bool bMute )
		{
			m_pButtonDown->Enable(!bMute);
			m_pButtonUp->Enable(!bMute);
			m_pSlider->Enable(!bMute);
			m_pStatic->Enable(!bMute);
		}

		void OnSliderChanged()
		{
			m_nCurVolume = m_pSlider->GetValue();
			m_pStatic->SetIntToText( m_nCurVolume );
		}

		void OnClicked_ButtonUp()
		{
			if( (++m_nCurVolume) > 100 ) m_nCurVolume = 100;
			m_pStatic->SetIntToText( m_nCurVolume );
			m_pSlider->SetValue( m_nCurVolume );
		}

		void OnClicked_ButtonDown()
		{
			if( (--m_nCurVolume) < 0 ) m_nCurVolume = 0;
			m_pStatic->SetIntToText( m_nCurVolume );
			m_pSlider->SetValue( m_nCurVolume );
		}
	};

	enum
	{
		sound_ALL,
		sound_BGM,
		sound_3D,
		sound_2D,
		sound_VOICE,
		sound_Amount,
	};

public:
	CDnGameSoundOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGameSoundOptDlg(void);

protected:
	CEtUIComboBox *m_pComboBoxQuality;

	SVolumeSlider m_volumeSlider[sound_Amount];

	typedef CDnOptionDlg BaseClass;

protected:
	void SetQuality();

public:
	void ExportSetting() override;
	void ImportSetting() override;
	bool IsChanged() override;
	void CancelOption() override;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );	
};
