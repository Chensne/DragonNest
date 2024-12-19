#pragma once

#include "DnCustomDlg.h"

class CDnPvPRacingResultDlg : public CDnCustomDlg
{
public:
	enum eSlotCount
	{
		eMax_SlotCount = 8,
	};

	enum eUserState{
		Normal_S = 0,
		MINE_S = 1,
		KILLED_S = 2,
	};

	enum
	{
		PVP_MEDAL_ICON_XSIZE = 23,
		PVP_MEDAL_ICON_YSIZE = 23,
	};

	struct SUI_SlotData
	{
		CEtUIStatic * m_pStaticLevel;
		CDnJobIconStatic * m_pStaticJob;

		CEtUITextureControl * m_pTCGrade;
		CEtUITextureControl * m_pTCGuildMark;
		CEtUITextureControl * m_pTCMedalIcon;

		CEtUIStatic * m_pStaticGuildName;
		CEtUIStatic * m_pStaticPlayerName;
		CEtUIStatic * m_pStaticFinalRanking;
		CEtUIStatic * m_pStaticLapTime;
		CEtUIStatic * m_pStaticPCBang;
		CEtUIStatic * m_pStaticReward;
		CEtUIStatic * m_pStaticCover;

		SUI_SlotData() : m_pStaticLevel( NULL ), m_pStaticJob( NULL ), m_pTCGrade( NULL ), m_pTCGuildMark( NULL ), m_pTCMedalIcon( NULL ), m_pStaticGuildName( NULL ),
			m_pStaticPlayerName( NULL ), m_pStaticFinalRanking( NULL ), m_pStaticLapTime( NULL ), m_pStaticPCBang( NULL ), m_pStaticReward( NULL ), m_pStaticCover( NULL )
		{}
	};

	struct SUserInfo
	{
		int nSessionID;
		int nLevel;
		int nState;
		byte cJobClassID;
		byte cPVPlevel;
		
		std::wstring wszUserName;
		TGuildSelfView GuildSelfView;

		DWORD dwLapTime;

		UINT uiXP;
		UINT uiTotalXP;
		UINT uiMedal;

#if defined( PRE_ADD_REVENGE )
		UINT eRevengeUser;
#endif	// #if defined( PRE_ADD_REVENGE )

		SUserInfo() 
			: nSessionID(0)
			, nLevel(0)
			, nState(0)
			, cJobClassID(0)
			, cPVPlevel(0)
			, dwLapTime(0)
			, uiXP(0)
			, uiTotalXP(0)
			, uiMedal(0)
#if defined( PRE_ADD_REVENGE )
			, eRevengeUser( Revenge::RevengeTarget::eRevengeTarget_None )
#endif	// #if defined( PRE_ADD_REVENGE )
		{}
	};

public :
	CDnPvPRacingResultDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPvPRacingResultDlg();

protected:
	SUI_SlotData m_UISlotData[eMax_SlotCount];
	CEtUIButton * m_pBtClose;
	EtTextureHandle m_hPVPMedalIconImage;

	std::vector<SUserInfo> m_vUserInfo;

	bool m_bIsFinalResult;
	float m_fTotal_ElapsedTime;

	void InitializeSlot();
	std::wstring GetLapTimeString( const DWORD dwLapTime );

public:
	void AddUser( DnActorHandle hUser );
	void RemoveUser( DnActorHandle hUser );
	void SetUserScore( const int nSessionID, const DWORD dwLapTime );
	void SetUserState( DnActorHandle hUser ,int nState );
	void SetPVPXP( UINT nSessionID, UINT uiAddXPScore,UINT uiTotalXPScore,UINT uiMedalScore);
	void UpdateUser();

	void FinalResultOpen();
	std::wstring GetBestUserName();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};