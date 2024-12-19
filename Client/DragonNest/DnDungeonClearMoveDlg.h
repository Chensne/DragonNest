#pragma once
#include "EtUIDialog.h"

class CDnDungeonClearMoveDlg : public CEtUIDialog
{
public:
	CDnDungeonClearMoveDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDungeonClearMoveDlg(void);

protected:
	CEtUIStatic *m_pStaticMissionFailGuide;
	CEtUIStatic *m_pStaticNormalGuide;
	CEtUIButton *m_pButtonToWorld;
	CEtUIButton *m_pButtonToVillage;
	CEtUIButton *m_pButtonClose;
	CEtUIStatic *m_pStaticTimer;
#ifdef PRE_ADD_RETURN_VILLAGE_SHOWNAME
	CEtUIStatic *m_pStaticVillageName;
#endif 

	float m_fElapsedTime;
	int m_nCount;
	wchar_t m_wszTimer[80];

	bool m_bMissionFailMode;
	bool m_bTriggeredByUser_DisableWorld;

protected:
	void SetTime( int nTime );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

	void SetMissionFailMode(bool bSet);
#ifdef PRE_ADD_RETURN_VILLAGE_SHOWNAME
	void SetReturnVillage( int nVillage );
#endif

	void EnableWarpWorldZone( bool bEnable ) { if( m_pButtonToWorld ) m_pButtonToWorld->Enable( bEnable ); }
	void TriggeredByUser_DisableWorld()	{ m_bTriggeredByUser_DisableWorld = true; }
};
