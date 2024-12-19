#pragma once
#include "DnCustomDlg.h"

#if defined( PRE_ALTEIAWORLD_EXPLORE )

class CDnAlteaRank_ListDlg : public CDnCustomDlg
{
public:
	enum{
		E_Slot_Count = 4,
	};

public:
	CDnAlteaRank_ListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAlteaRank_ListDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

public:
	void SetPersonalData( int nRank, BYTE cJob, int nCount, WCHAR * wszName, WCHAR * wszGuildName, int nRankType );
	void SetGuildData( int nRank, int nCount, int nGuildID, short wGuildMark, short wGuildMarkBG, short wGuildMarkBorder, WCHAR * wszGuildName );

protected:
	void SetCountData( int nCount, int nRankType );
	void SetGuildMark( int nGuildID, short wGuildMark, short wGuildMarkBG, short wGuildMarkBorder );
	void SetRankItem( const int nRank, const int nDialogType, const int nRankType );

protected:
	CEtUIStatic * m_pRank_Static;
	CDnJobIconStatic * m_pClass_Static;
	CEtUITextureControl * m_pGuildMark;
	CEtUIStatic * m_pName_Static;
	CEtUIStatic * m_pGuildName_Static;
	CEtUIStatic * m_pTime_Static;

	CDnItemSlotButton * m_pItemSlot[ E_Slot_Count ];
};

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )