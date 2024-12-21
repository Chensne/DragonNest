#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_GACHA_JAPAN

const int NUM_GACHA_ROULETTE_ITEM = 6;

class CDnItem;

class CDnGachaRouletteDlg : public CEtUIDialog
{
private:
	CEtUITextBox* m_apTextBox[ NUM_GACHA_ROULETTE_ITEM ];

protected:

public:
	CDnGachaRouletteDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGachaRouletteDlg(void);

	void Initialize( bool bShow );
	void InitialUpdate( void );
	void SetItemText( int iIndex, const wchar_t* pText );
	void SetItemText( int iIndex, const wchar_t* pItemName, const wchar_t* pRankName, DWORD dwRankColor );
	void SetStatText( int iIndex, const wchar_t* pStat, DWORD dwRankColor );
	void Clear( void );
};

#endif // PRE_ADD_GACHA_JAPAN