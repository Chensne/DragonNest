#pragma once
#include "DnCustomDlg.h"

class CDnInspectGuildInfoDlg : public CDnCustomDlg
{
public:
	CDnInspectGuildInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnInspectGuildInfoDlg( void );

protected:
	CEtUIStatic* m_pPlayerName;
	CEtUIStatic* m_pGuildName;
	CEtUIStatic* m_pGuildMasterName;
	CEtUIStatic* m_pGuildLevel;
	CEtUIStatic* m_pGuildMembers;
	CEtUITextBox* m_pGuildRewards;
	
	CEtUITextureControl* m_pTextureGuildMark;
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

	void RefreshGuildInfo();
#ifdef PRE_ADD_BEGINNERGUILD
	bool SetBeginnerGuildInfo(const SCPlayerGuildInfo& playerGuildInfo);
	bool SetNormalGuildInfo(SCPlayerGuildInfo& playerGuildInfo);
#endif
};
