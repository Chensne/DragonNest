#pragma once
#include "EtUIDialog.h"


#ifdef PRE_ADD_FADE_TRIGGER

class CDnFadeCaptionDlg : public CEtUIDialog
{
public:
	CDnFadeCaptionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnFadeCaptionDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

public:
	void SetCaption( const wchar_t* wszCaption, float fDelayTime );
	void ClearCaption();

protected:
	CEtUIStatic* m_pStaticCaption;
	float m_fDelayTime;
};

#endif // PRE_ADD_FADE_TRIGGER