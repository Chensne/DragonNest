#pragma once
#include "EtUIDialog.h"
#include "EtUIDialogGroup.h"
#include "DnFriendTask.h"

class CDnFriendInfoDlg;
class CDnFriendAddDlg;
class CDnFriendGroupDlg;

#define _MAX_FRIEND_TITLE			128
#define _FRIEND_LIST_REFRESH_TIME	30

class CDnFriendDlg : public CEtUIDialog, public CEtUICallback
{
	enum
	{
		FRIEND_INFO_DIALOG,
		FRIEND_ADD_DIALOG,
		FRIEND_GROUP_DIALOG,
	};

public:
	CDnFriendDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnFriendDlg(void);

protected:
	CDnFriendInfoDlg *m_pFriendInfoDlg;
	CDnFriendAddDlg *m_pFriendAddDlg;
	CDnFriendGroupDlg *m_pFriendGroupDlg;
	CEtUIButton *m_pButtonInvite;  // Rotha - 친구초대 버튼 설정

	CEtUIButton *m_pButtonGroup;
	CEtUIButton *m_pButtonInfo;
	CEtUIButton *m_pButtonSendChat;
	CEtUIButton *m_pButtonSendMail;
	CEtUIButton *m_pButtonParty;
	CEtUIButton *m_pButtonGuild;
	CEtUIButton *m_pButtonAdd;
	CEtUIButton *m_pButtonDel;
	CEtUICheckBox *m_pCheckBoxHide;
	CEtUIStatic *m_pStaticCount;
	CEtUITreeCtl *m_pTreeFriend;

	CEtUIDialogGroup	m_FriendDlgGroup;
	float				m_ListUpdateTime;

protected:
	void SetCount( int nCur, int nMax );
	void EnableButton( bool bEnable );
	void MakeGroupTitle(wchar_t* title, int groupId, const wchar_t* wszGroupName);
	void SetStateIcon(CTreeItem* friendItem, BYTE location);
	bool IsFriendInGame(const TCommunityLocation& location) const;
	void Process( float fElapsedTime );

public:
	void RefreshFriendGroup();
	void RefreshFriendInfo( INT64 biCharDBID );
	void RefreshFriendList();
	void RefreshGroupDialog();

	void OnRecvDeleteFriend();

public:
	void AddFriendGroup( int nGroupID, const wchar_t *wszGroupName );
	void OnAddFriendGroup();
	void RenameFriendGroup( int nGroupID, const wchar_t *wszGroupName );
	void DeleteFriendGroup( int nGroupID );

	void AddFriend( int nGroupID, INT64 biCharDBID, CDnFriendTask::eLocationState state, const wchar_t *wszFriendName );
	void DeleteFriend( INT64 biCharDBID );
	bool GetHideOffline() const;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
#ifdef PRE_FIX_FRIEND_SORT
	void SortList(std::vector<CDnFriendTask::SFriendInfo>& result, const CDnFriendTask::FRIENDINFO_MAP& friendInfoList);
	static bool SortByName(const std::pair<INT64, CDnFriendTask::SFriendInfo>& comp1, const std::pair<INT64, CDnFriendTask::SFriendInfo>& comp2);
#endif
};