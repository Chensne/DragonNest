#pragma once
#include "EtUIDialog.h"

//const float AutoCloseTime = 10.0f;

namespace PVPResult
{
	enum PVPResult
	{
		Win = 0,
		Lose = 1,
		Draw = 2,
		RoundEND = 3
	};

	enum PVPResultSound
	{
		GameWinSound = 10009,
		GameLoseSound = 10010,
		GameDrawSound = 10011,
		RoundResultSound = 10012
	};

}

class CDnPVPModeEndDlg : public CEtUIDialog
{
public:
	CDnPVPModeEndDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPVPModeEndDlg(void);
protected:

	CEtUIStatic *m_pStaticResult;
	CEtUIStatic *m_pStaticSecResult;
	CEtUIStatic *m_pStaticThirResult;
	CEtUIStatic *m_pStaticQuartResult;

	int m_nGameWinSound;
	int m_nGameLoseSound;
	int m_nGameDrawSound;
	int m_nRoundResultSound;

public:	
	void Reset();

	void SetResultString1( const WCHAR * wszString );
	void SetResultString2( const WCHAR * wszString );
#ifdef PRE_ADD_PVP_TOURNAMENT
	void SetResultString2(const WCHAR* wszString, DWORD color);
#endif
	void SetResultString3( const WCHAR * wszString );
	void SetResultString4( const WCHAR * wszString );

#ifdef PRE_MOD_PVPOBSERVER
	void SetResultStringObServer( UINT winTeam, const wchar_t * str, bool bZombieMode=false, bool bVictory=false );
#endif // PRE_MOD_PVPOBSERVER

	void PlayResultSound(  byte cResult );

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};
