#pragma once
#include "DnOptionDlg.h"
#include "GameOption.h"

class CDnGameMovieOptDlg : public CDnOptionDlg
{
public:
	CDnGameMovieOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGameMovieOptDlg(void);

protected:
	CEtUIComboBox *m_pComboRecordResolution;
	CEtUIComboBox *m_pComboRecordFPS;
	CEtUIComboBox *m_pComboRecordVideoQuality;
	CEtUIComboBox *m_pComboRecordAudioQuality;

	typedef CDnOptionDlg BaseClass;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	
public:
	void ImportSetting();
	void ExportSetting();

	virtual bool IsChanged();
};
