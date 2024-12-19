#pragma once

#ifdef PRE_MOD_PVPOBSERVER

#include "DnCustomDlg.h"

class CDnPvPObserverOrderListDlg : public CDnCustomDlg
{
	enum eOrderListCommon
	{
		GROUPCAPTAIN_POSITION = 0,
		MAX_PLAYER = 8,
		TIME_LIMIT = 10,
	};

	enum ETeamInfo
	{
		ETeamInfo_BLUE = 0,
		ETeamInfo_RED = 1,		
		ETeamInfo_MAX	
	};

	struct sSelectSlot
	{
		UINT nUserSessionID;
		bool bIsAlive;
		CEtUIStatic *pStatic_Selection;
		CEtUIStatic *pStatic_Ready;
		CEtUIStatic *pStatic_Name;

		sSelectSlot()
		{
			nUserSessionID = 0;
			bIsAlive = true;
			pStatic_Selection = NULL;
			pStatic_Ready = NULL;
			pStatic_Name = NULL;
		}
	};

protected:

	struct SSelectPlayer
	{
		sSelectSlot m_sSelectSlot[eOrderListCommon::MAX_PLAYER];
		CEtUIStatic *m_pStaticGroupCaptain;
		CEtUIStatic *m_pStaticTimeLimit;
	};
	SSelectPlayer m_SelectPlayer[ ETeamInfo::ETeamInfo_MAX ];

	float m_fTimeLimit;
	int m_nCrrCaptainOrder;
	bool m_bForceView;
	UINT m_uiGroupCaptainSessionID[ ETeamInfo_MAX ];

public:
	CDnPvPObserverOrderListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPvPObserverOrderListDlg();

	virtual void Show( bool bShow );
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	void SelectPlayer( int nUserSessionID, int orderListCnt );
	void SelectGroupCaptain( int nUserSessionID );
	void MakePlayerList();
	void StartCountDown();
	void ForceView( bool bShow );

};

#endif // PRE_MOD_PVPOBSERVER