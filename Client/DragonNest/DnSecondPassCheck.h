#pragma once
#include "EtUIDialog.h"
//#define PRE_ADD_SECOND_SETCHECK

class CDnSecondPassCheck : public CEtUIDialog
{
public:
	CDnSecondPassCheck( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnSecondPassCheck(void);

protected:

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );



	void SelectChannel();
	void SetSelectChannel(int ChannelID){m_ChannelID = ChannelID;}
	void SetCheckShow(bool bCheck);

#if defined(PRE_ADD_23829)
	void SecondPassCheck(bool bCheck);
#endif // PRE_ADD_23829

	int m_ChannelID;
	CEtUIProgressBar* m_pProgressBar;
	CEtUICheckBox *m_pCheckBox; 

	float m_fTimer;
	float m_MaxTimer;

};

