#pragma once
#include "EtUIDialog.h"
#include "GameOption.h"

#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
#include "DnPartyData.h"
#include "DnPartyEnterDlg.h"
#include "DnPartyEnterPasswordDlg.h"
#endif 

class CEtUIButton;
class CEtUIStatic;

class CDnDirectDlg : public CEtUIDialog, public CEtUICallback
{

	enum
	{
		PARTYJOINPASS_DIALOG,
		PARTYJOIN_DIALOG,
	};

public:
	CDnDirectDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDirectDlg(void);

protected:
	CEtUIButton *m_pButtonParty;
	CEtUIButton *m_pButtonFriend;
	CEtUIButton *m_pButtonGuild;
	CEtUIButton *m_pButtonTrade;
	CEtUIButton *m_pButtonDuel;
	CEtUIButton *m_pButtonInfo;
	CEtUIButton *m_pButtonFollow;
	CEtUIButton *m_pButtonChat;
	CEtUIButton * m_pButtonPupil;
	CEtUIButton * m_pButtonMaster;
	
	CEtUIStatic *m_pStaticUserLevel;
	//CEtUIStatic *m_pStaticWorldLevel;
	CEtUIStatic *m_pStaticId;
	CEtUIStatic *m_pStaticJob;

	DWORD		m_dwSessionID;
	bool		m_bRequestFriend;
	bool		m_bReceivePermission;
	int			m_nLevel;
	TPARTYID	m_PartyID;

	union
	{
		struct
		{
			//use eCommunityAcceptableOptionType
			BYTE m_cPartyInviteAcceptable : 2;
			BYTE m_cGuildInviteAcceptable : 2;
			BYTE m_cTradeRequestAcceptable : 2;
			BYTE m_cDuelRequestAcceptable : 2;
			BYTE m_cObserveStuffAcceptable : 2;
		};
		char m_cCommunityOption[13];			//13byte
	};

	enum {
		MESSAGEBOX_MASTER_APPLICATION = 0,
		MESSAGEBOX_PUPIL_APPLICATION
	};

#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
	CDnPartyEnterDlg*			m_pPartyEnterDlg;
	CDnPartyEnterDlg*			m_pRaidEnterDlg;
	CDnPartyEnterPasswordDlg*	m_pPartyEnterPasswordDlg;
	CDnPartyEnterPasswordDlg*	m_pRaidEnterPasswordDlg;

	bool						m_bDirectReqParty;
	bool						m_bSecret;
	bool						m_Raid;
	ePartyType					m_PartyType;

#endif

public:
	void SetInfo( DWORD dwSessionID, int nUserLevel, const wchar_t *wszJobName, const wchar_t *wszName );
#ifdef PRE_PARTY_DB
	void SetPermission(UINT nSessionID, char *pCommunityOption, TPARTYID nPartyID);
#else
	void SetPermission(UINT nSessionID, char *pCommunityOption, UINT nPartyID);
#endif
	bool IsAskingParty() const;

#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
	inline bool IsDirectPlayerAskParty() { return m_bDirectReqParty; }
	inline void SetDirectPlayerAskParty( bool bdirectAsk )	{ m_bDirectReqParty = bdirectAsk; }
	
#endif

#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
	void OnRecvPartyInfo( const SPartyListInfo& partyInfo );
	void SetPartyEnterDlgByRaidOrNormal(bool bRaid, const SPartyListInfo& partyInfoList, const std::wstring& title, ePartyType type);
	void SetPartyEnterDlgByRaidOrNormal(bool bRaid, const SPartyListInfo& partyInfoList, const std::wstring& title);
	void OnPartyJoinFailed();
	void EnablePartyDlg( bool bShow );
#endif 

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process( float fElapsedTime );

	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

private:
	bool	CheckFriendCondition(const WCHAR* name);
	bool	IsFriend( const WCHAR *name );
	bool	CheckValidActor();
};
