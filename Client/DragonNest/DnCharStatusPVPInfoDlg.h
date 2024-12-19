#pragma once
#include "EtUIDialog.h"

namespace{

	enum PVPInfoUISting{
		Warrior = 46,
		Acher = 47,
		Cleric = 48,
		Socer = 49,
#if defined( PRE_ADD_ACADEMIC )
		Academic = 305,
#endif
#if defined( PRE_ADD_KALI )
		Kali = 306 ,
#endif
#if defined( PRE_ADD_ASSASSIN )
		Assassin = 307 ,
#endif

#if defined( PRE_ADD_LENCEA )
		Lencea = 308,
//#pragma warning("DNCharStatuisPVPInfoDLG ADD LENCEA UISTRING");
#endif

#if defined( PRE_ADD_MACHINA )
		Machina = 376,
//#pragma warning("DNCharStatuisPVPInfoDLG ADD MACHINA UISTRING");
#endif
		#pragma message("DNCharStatuisPVPInfoDLG ADD DARK AVENGER + SILVER HUNTER")

		Win = 120042,
		Loose = 120043,
		Draw = 120044,
		PVPLevel = 120046,
		PVPXP = 120047,
		KILL = 120040,
		KILLED = 120041,
		KILLP = 120032,
		SupP = 120033,
		AssiP = 120034,
		NEXTXP = 120458,
		Ghoul_PlayCount = 121089,			
		Ghoul_GhoulWin = 121090 ,				
		Ghoul_HumanWin = 121091,				
		Ghoul_TimeOver = 121092,				
		Ghoul_GhoulKill = 121094,				
		Ghoul_HumanKill = 121093,				
		Ghoul_HolyWaterUse = 121095,
	};

}

class CDnCharStatusPVPInfoDlg : public CEtUIDialog
{
public:
	CDnCharStatusPVPInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCharStatusPVPInfoDlg(void);
protected:

	CEtUIListBoxEx *m_pListBoxEx;
protected:
	void InsertItem(const  wchar_t * wszItemText , INT64 iData  );
	void InsertItem(const  wchar_t * wszItemText , const wchar_t *wszDataText);
	void RefreshList();
public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	//virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );	
};