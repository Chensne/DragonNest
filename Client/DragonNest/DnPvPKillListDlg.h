#pragma once

#include "DnCustomDlg.h"
#include "DnWeapon.h"

class CDnPvPKillListDlg : public CDnCustomDlg
{
public:
	CDnPvPKillListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPvPKillListDlg();

protected:
	CEtUIStatic *m_pStaticBar[4];
	CEtUIStatic *m_pStaticLeftName[4][2];
	CEtUIStatic *m_pStaticRightName[4][2];
	//CEtUIStatic *m_pStaticWeapon[4][CDnWeapon::EquipTypeEnum_Amount];
	CEtUIStatic *m_pStaticWeapon[4];
	CEtUIStatic *m_pStaticGhoul[4];
	CEtUIStatic *m_pStaticOccupation[4];

	struct InfoStruct {
		std::wstring wszKillerName;
		std::wstring wszKilledName;
		bool bKillerTeam;
		bool bKilledTeam;
		bool bIsGhoul;
		bool bIsOccupation;
		CDnWeapon::EquipTypeEnum EquipType;
		float fDelta;

		InfoStruct() : bKillerTeam(true), bKilledTeam(true), bIsGhoul(false), bIsOccupation(false), fDelta( 0.f), EquipType(CDnWeapon::Sword)
		{}
	};
	std::vector<InfoStruct *> m_pVecInfoStruct;

protected:
	void UpdateLine( int nLine, InfoStruct *pStruct );
public:
	// CDnCustonDlg
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

	void AddInfo( DnActorHandle hKiller, DnActorHandle hKilled, CDnWeapon::EquipTypeEnum EquipType );
	void AddOccupationInfo( DnActorHandle hActor, std::wstring & wszFlagName );

	void Clear();
};