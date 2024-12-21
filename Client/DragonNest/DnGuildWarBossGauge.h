#pragma once

#include "DnCustomDlg.h"
#include "PvPOccupationScoreSystem.h"

class CDnGuildWarBossGaugeDlg : public CDnCustomDlg
{
public:
	CDnGuildWarBossGaugeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGuildWarBossGaugeDlg(void);

	enum {
		BOSS_GAUGE = 0,
		PROP_GAUGE = 1,
		LAST_TIME = 10000,
	};

protected:

	CEtUIStatic * m_pStaticBoss;
	CEtUIStatic * m_pStaticBossLayer;
	CEtUIStatic * m_pStaticBossBar;
	CEtUIStatic * m_pStaticBossSuperBar;
	CEtUIProgressBar * m_pProgressBoss;
	CEtUIProgressBar * m_pProgressSuper;

	CEtUIStatic * m_pStaticProp;
	CEtUIStatic * m_pStaticPropLayer;
	CEtUIStatic * m_pStaticPropBar;
	CEtUIProgressBar * m_pProgressProp;

	DnMonsterActorHandle m_hActor;

	int m_eType;	// Boss || Prop
	int m_BossID[PvPCommon::TeamIndex::Max];
	int m_DoorID[PvPCommon::TeamIndex::Max][PvPCommon::Common::DoorCount];

	int m_nPrevLayerCount;

	LOCAL_TIME m_LastTime;

	int GetSeparateCount();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );

	bool SetActor( DnMonsterActorHandle hActor );
	void SetControl( int eType );
	void SetHP( float fHP );
	void SetSP( float fSP);
};
