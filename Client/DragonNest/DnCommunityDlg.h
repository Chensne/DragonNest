#pragma once
#include "EtUITabDialog.h"
#include "DnSmartMoveCursor.h"

class CDnPartyListDlg;
class CDnPartyInfoListDlg;
class CDnFriendDlg;
class CDnGuildDlg;
#ifdef PRE_ADD_GUILD_EASYSYSTEM
class CDnGuildWantedListTabDlg;
#else
class CDnGuildWantedListDlg;
#endif
class CDnGuildBaseDlg;

class CDnBlockDlg;
class CDnNestDlg;
class CEtUIRadioButton;

class CDnMasterListDlg;
class CDnMasterInfoDlg;
class CDnPupilListDlg;
#ifdef PRE_PRIVATECHAT_CHANNEL
class CDnPrivateChannelDlg;
#endif
#ifdef PRE_ADD_DWC
class CDnDwcDlg;
#endif

class CDnCommunityDlg : public CEtUITabDialog
{
public:
	enum emPartyDialogType
	{
		typePartyList,
		typePartyInfoList,
	};

	enum emMasterDilogType
	{
		typeMasterList,
		typeMasterInfo,
		typePupilList
	};
public:
	CDnCommunityDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCommunityDlg(void);

protected:
	CDnPartyListDlg *m_pPartyListDlg;
	CDnPartyInfoListDlg *m_pPartyInfoListDlg;
	CDnFriendDlg	*m_pFriendDlg;
	CDnGuildBaseDlg *m_pGuildBaseDlg;
	CDnBlockDlg		*m_pBlockDlg;

	CDnMasterListDlg *m_pMasterListDlg;
	CDnMasterInfoDlg *m_pMasterInfoDlg;
	CDnPupilListDlg  *m_pPupilListDlg;

	CDnNestDlg		*m_pNestDlg;
#ifdef PRE_ADD_DWC
	CDnDwcDlg		*m_pDWCDlg;	
#endif
#ifdef PRE_ADD_BESTFRIEND
	class CDnBestFriendDlg * m_pBestFriendDlg;
	CEtUIRadioButton *m_pTabBestFriend;
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	CEtUIRadioButton* m_pTabPrivateChannel;
	CDnPrivateChannelDlg* m_pPrivateChannelDlg;
#endif // PRE_PRIVATECHAT_CHANNEL

	CEtUIRadioButton *m_pTabParty;
	CEtUIRadioButton *m_pTabGuild;
	CEtUIRadioButton *m_pTabFriend;
	CEtUIRadioButton *m_pTabBlock;
	CEtUIRadioButton *m_pTabMaster;
	CEtUIRadioButton *m_pTabNestInfo;

	emMasterDilogType m_emMasterDialogType;

	emPartyDialogType m_emPartyDialogType;
	CDnSmartMoveCursorEx m_SmartMoveEx;

//#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
//	std::vector<int> m_vecTabIDList;
//#endif

public:
	// Note : 파티 관련
	//
	int GetPartyListStartIndex();
	void ShowPartyDialog( emPartyDialogType typePartyDialog );
	bool IsShowPartyDialog( emPartyDialogType typePartyDialog );
	void SwapPartyDialog( emPartyDialogType typePartyDialog );

	void RefreshPartyList();
	void RefreshPartyInfoList();
	void RefreshPartyMemberList();
	void OnPartyJoin();
	void SetPartyCreateDifficulties(const BYTE* pDifficulties);
	void OnRecvPartyMemberInfo(ePartyType type);
	void OnPartyJoinFailed();

	void SetPartyInfoLeader( int nLeaderIndex );

	// Note : 사제 관련
	//
	void ChangeMasterDialog( emMasterDilogType nType );
	void RefreshMasterListDialog();
	void RefreshMasterSimpleInfo();
	void RefreshMasterCharacterInfo();
	void RefreshMasterClassmate();
	void RefreshPupilList();
	void RefreshMyMasterInfo();
	void RefreshClassmateInfo();
	void RefreshPupilLeave();
	void RefreshRespectPoint();

#ifdef PRE_ADD_BESTFRIEND
	void SetBFData( struct TBestFriendInfo & bfInfo ); // 절친정보.
	void OpenBestFriendDlg( bool bShow ); // 절친정보Dlg.	
	void SetMemo( bool bFromMe, WCHAR * pStrMemo ); // 메모수신.
	void BrokeupOrCancelBF( bool bCancel, WCHAR * strName ); // 절친파기 or 파기취소.
	void DestroyBF(); // 절친파기.
#endif

public:
	CDnFriendDlg*		GetFriendDialog()		{ return m_pFriendDlg; }
	CDnGuildDlg*		GetGuildDialog();
	CDnNestDlg*			GetNestDialog()			{ return m_pNestDlg; }
	CDnPartyListDlg*	GetPartyListDialog()	{ return m_pPartyListDlg; }
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CDnGuildWantedListTabDlg*	GetGuildWantedListTabDialog();
#else
	CDnGuildWantedListDlg*		GetGuildWantedListDialog();
#endif // PRE_ADD_GUILD_EASYSYSTEM

#ifdef PRE_PRIVATECHAT_CHANNEL
	CDnPrivateChannelDlg* GetPrivateChannelDlg() { return m_pPrivateChannelDlg; }
#endif // PRE_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_DWC
	CDnDwcDlg* GetDWCDialog() { return m_pDWCDlg; }
	void	   SetCommunityTab(bool bHasDWCTeam);
#endif

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

#ifdef PRE_ADD_NEWCOMEBACK
	void SetComebackAppellation( UINT sessionID, bool bComeback );
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
	void SetDwcUIControls();
#endif
//#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
//	enum eRadioButtonTabID
//	{
//		eRadioTap_Party = 1 ,
//		eRadioTap_Friend ,
//		eRadioTap_Block,
//		eRadioTap_Guild,
//		eRadioTap_DList,
//		eRadioTap_Master,
//		eRadioTap_BF,
//		eRadioTap_Channel,
//
//		eRadioTap_MAX
//	};
//protected:
//	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
//#endif

};
