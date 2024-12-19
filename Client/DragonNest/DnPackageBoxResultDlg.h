#pragma once

#include "DnCustomDlg.h"
#ifdef PRE_ADD_SMARTMOVE_PACKAGEBOX
#include "DnSmartMoveCursor.h"
#endif

#if defined( PRE_ADD_EASYGAMECASH )

class CDnItem;
class CDnItemSlotButton;

class CDnPackageBoxResultDlg : public CDnCustomDlg
{
public:
	enum{
		ePackageBox_Item_Count = 5,
		ePackageBox_Page_Count = 5,
		ePackageBox_Max_1Page_Count = 5,
	};

	struct SPackageListItem
	{
		CDnItemSlotButton * m_pItemSlot;
		CEtUIStatic * m_pItemName;

		SPackageListItem() : m_pItemSlot( NULL ), m_pItemName( NULL )
		{}
	};

public:
	CDnPackageBoxResultDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPackageBoxResultDlg(void);

protected:
	SPackageListItem m_pPackageList[ePackageBox_Item_Count];
	CEtUIButton * m_pPageButton[ePackageBox_Page_Count];
	CEtUIButton * m_pPriorButton;
	CEtUIButton * m_pNextButton;
	CEtUIStatic * m_pQuestionStatic;

	std::vector<boost::tuple<int, int, int>> m_vPackageItem;
	int m_nSelectPage;
	int m_nMaxSelectPage;

#ifdef PRE_ADD_SMARTMOVE_PACKAGEBOX
	CDnSmartMoveCursor m_SmartMove;
#endif

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
	int m_nSoundIndex;
#endif 

public:
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
	void SetInvenData( const BYTE cInvenType, const short sInvenIndex, TRangomGiveItemData* pItemlist, int nitemCount );
#endif 
#ifdef PRE_FIX_MAKECHARMITEM
	void SetInvenData( const BYTE cInvenType, const short sInvenIndex, const INT64& nCharmItemSerial );
#else
	void SetInvenData( const BYTE cInvenType, const short sInvenIndex );
#endif
 

protected:
	void NextPage();
	void PrevPage();
	void RefreshControl();
	void ClearPackageList();
	void SetPackageBoxItem();
	void SelectPage( const int nSelectPage );
	void ProcessPerminPlayer();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif	// #if defined( PRE_ADD_EASYGAMECASH )