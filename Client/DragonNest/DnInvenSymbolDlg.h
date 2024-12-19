#pragma once
#include "EtUIDialog.h"

class CDnInvenSymbolDescDlgRight;
class CDnInvenSymbolDescDlgLeft;

class CDnInvenSymbolDlg : public CEtUIDialog
{
	enum
	{
		SYMBOLDESCRIGHT_DIALOG,
		SYMBOLDESCLEFT_DIALOG,
	};

	struct SSymbolSlot
	{
		CEtUITextureControl *m_pSymbolImage;
		CEtUIStatic *m_pStaticCount;
		CEtUIStatic *m_pStaticName;
		std::wstring m_strTooltip;

		SSymbolSlot()
			: m_pSymbolImage(NULL)
			, m_pStaticCount(NULL)
			, m_pStaticName(NULL)
		{
		}

		void Clear()
		{
			m_pSymbolImage->Show(false);
			m_pStaticCount->ClearText();
			m_pStaticName->ClearText();
			m_strTooltip.clear();
		}
	};

	struct SSymbolItem
	{
		std::wstring m_strName;
		int m_nCount;
		int m_nImageIndex;
		std::wstring m_strTooltip;

		SSymbolItem()
			: m_nCount(0)
			, m_nImageIndex(0)
		{
		}

		void Clear()
		{
			m_nCount = 0;
			m_nImageIndex = 0;
			m_strName.clear();
			m_strTooltip.clear();
		}
	};

	enum
	{
		SLOT_SIZE_X = 2,
		SLOT_SIZE_Y = 7,
		SLOT_MAX_SIZE = 14,
		SLOT_MAX_COUNT = 56,
		SLOT_MAX_PAGE = 4,
	};

public:
	CDnInvenSymbolDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnInvenSymbolDlg(void);

protected:
	CEtUIButton *m_pButtonPrev;
	CEtUIButton *m_pButtonNext;
	CEtUIStatic *m_pStaticPage;

	std::vector<SSymbolSlot> m_vecSymbolSlot;
	std::vector<SSymbolItem> m_vecSymbolItem;

	CDnInvenSymbolDescDlgRight *m_pSymbolDescRightDlg;
	CDnInvenSymbolDescDlgLeft *m_pSymbolDescLeftDlg;

	EtTextureHandle m_hSymbolImage;

	int m_nMaxPage;
	int m_nCurrentPage;

protected:
	void UpdateSymbolSlot();
	void UpdatePage();
	void UpdatePageButton();

	void NextPage();
	void PrevPage();

	void ShowDescDialog( bool bShow, int nSlotIndex = -1 );

public:
	void ClearAll();
	void AddSymbolItem( int nSlotIndex, const wchar_t *wszName, int nCount, int nImageIndexconst, const wchar_t *wszTooltip, bool bList );
	void DelSysmbolItem( int nSlotIndex );

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
