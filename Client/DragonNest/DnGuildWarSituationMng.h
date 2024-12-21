#pragma once

class CDnMiniPlayerGuildWarGaugeDlg;
class CDnMiniPlayerGuildWarMainTabDlg;
class CDnMiniPlayerGuildWarTabDlg;
class CDnMiniPlayerGuildWarPopupDlg;

class CDnGuildWarSituationMng
{
public :
	CDnGuildWarSituationMng();
	~CDnGuildWarSituationMng();

	enum{
		MAX_PARTY_COUNT = 4,
		MAX_USER_COUNT = 16,
		MAX_PARTY_USER_COUNT = 4,
	};

protected :

	CDnMiniPlayerGuildWarMainTabDlg * m_pMainTabDlg;
	CDnMiniPlayerGuildWarTabDlg * m_pPartyTabDlgs[MAX_PARTY_COUNT];
	CDnMiniPlayerGuildWarGaugeDlg * m_pUserGaugeDlgs[MAX_USER_COUNT];
	CDnMiniPlayerGuildWarGaugeDlg * m_pPartyUserGaugeDlgs[MAX_PARTY_USER_COUNT];
	CDnMiniPlayerGuildWarPopupDlg * m_pPopupDlg;
	CDnMiniPlayerGuildWarTabDlg * m_pSelectPartyTabDlg;

	bool m_bPartyShow;
	bool m_bSituationShow;
	bool m_bNeedUpdate;
	bool m_bPartyRefresh;
	bool m_bShowPopup;
	bool m_bPartyMove;
	BYTE m_cSelectUserCount;

	UINT m_uiMyState;
	BYTE m_cMyPartyPosition;
	BYTE m_cPartyCount;

public :

	void Initialize();
	void InitializeSituaion();

	void AddUser(DnActorHandle hActor, void * pData );
	void RemoveUser(DnActorHandle hActor);
	void MoveUser( SCPvPMemberIndex * pPacket );
	void SwapUser();
	void SetUserState( UINT uiSessionID, UINT uiUserState );
	void SetSlotMove( bool bMove );

	void PartyShow( bool bShow );
	void SituationShow( bool bShow );

	void PartyPosition();
	void SituationPosition();

	void SetPartyCount( BYTE cPartyCount )	{ m_cPartyCount = cPartyCount; }
	BYTE GetPartyCount()	{ return m_cPartyCount; }

	void OnChangeResolution();
	void NeedUpdate()	{ m_bNeedUpdate = true; }
	void Process( float fDelta );
	void Refresh();
	void PartyRefresh();

	void ShowPopupDlg( float fx, float fy, UINT uiUserState, UINT uiSessionID, bool bShow );
	void SelectUser( CDnMiniPlayerGuildWarGaugeDlg * pDlg );

	bool IsPartyMember( DnActorHandle hActor );
};
