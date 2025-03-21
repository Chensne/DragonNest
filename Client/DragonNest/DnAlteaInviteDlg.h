#pragma once
#include "EtUIDialog.h"

#if defined( PRE_ALTEIAWORLD_EXPLORE )

class CDnAlteaInviteDlg : public CEtUIDialog
{
public:
	CDnAlteaInviteDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAlteaInviteDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	void SetSendTicketCount( const int nCount );
	void SetSendUser( const std::vector<INT64> vecSendUser );

protected:
	void RefreshDialog();
	void RefreshFriendTree();
	void RefreshGuildTree();

	void SetCount( const int nConnectUserCount, CEtUIStatic * pCount_Static );

protected:
	CEtUITreeCtl * m_pFriend_Tree;
	CEtUITreeCtl * m_pGuild_Tree;

	CEtUIEditBox * m_pEditBox;

	CEtUIStatic * m_pFriendCount_Static;
	CEtUIStatic * m_pGuildCount_Static;
	CEtUIStatic * m_pInviteEnable_Static;
	CEtUIStatic * m_pInviteCount_Static;
	CEtUIStatic * m_pNickName_Static;
	CEtUIStatic * m_pNotice_Static;

	CEtUIButton * m_pInvite_Button;

	bool m_bInviteDisable;
};

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )