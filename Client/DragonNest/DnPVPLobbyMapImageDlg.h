#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_PVP_HELP_MESSAGE

class CDnPVPLobbyMapImageDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnPVPLobbyMapImageDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPLobbyMapImageDlg(void);

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
	virtual void Process( float fElapsedTime );
	
public:
	bool		LoadPVPMapImage(int nMapIndex);
	void		SetDialogPosition(float fX, float fY);
	void		UpdateTooltipPosition();

protected:
	CEtUITextureControl*	m_pTexture;
	EtTextureHandle			m_hTexture;
	SUICoord				m_TextureCoord;
};

#endif