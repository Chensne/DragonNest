#pragma once
#include "DnCustomDlg.h"
#include "DnSkill.h"

class CEtUIButton;
class CDnSkillLevelUpDlg;
class CEtUIScrollBar;

class CDnSkillDlg : public CDnCustomDlg, public CEtUICallback
{
	enum
	{
		SKILL_LEVELUP_DIALOG,
	};

	struct SKILL_SLOT
	{
		CDnQuickSlotButton *m_pSlotButton;
		CEtUIStatic *m_pSkillBase;
		CEtUIStatic *m_pSkillName;
		CEtUIStatic *m_pSkillLevel;
		CEtUIStatic *m_pSkillPoint;
		CDnSkillUpButton *m_pSkillUpButton;

		SKILL_SLOT()
			: m_pSkillBase(NULL)
			, m_pSlotButton(NULL)
			, m_pSkillName(NULL)
			, m_pSkillLevel(NULL)
			, m_pSkillPoint(NULL)
			, m_pSkillUpButton(NULL)
		{
		}

		void InitSkillSlot()
		{
			m_pSlotButton->ResetSlot();
			m_pSkillName->ClearText();
			m_pSkillLevel->ClearText();
			m_pSkillPoint->ClearText();
			m_pSkillBase->SetTextureColor( 0x4CFFFFFF, 0 );
		};

		void SetSkillSlot( CDnSkill *pSkillItem, LPWSTR wszName, int nLevel, int nSkillPoint )
		{
			if( !pSkillItem ) return;

			m_pSkillBase->SetTextureColor( 0xFFFFFFFF, 0 );

			m_pSlotButton->SetQuickItem( pSkillItem );
			m_pSkillName->SetText( wszName );

			wchar_t szTemp[36] = {0};
			swprintf_s( szTemp, _countof(szTemp), L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 64 ), nLevel );
			m_pSkillLevel->SetText( szTemp );

			swprintf_s( szTemp, _countof(szTemp), L"%s %d", L"SP", nSkillPoint );
			m_pSkillPoint->SetText( szTemp );
		}

		bool IsInside( float fX, float fY )
		{
			SUICoord uiCoords, uiCoorde;

			m_pSkillName->GetUICoord(uiCoords);
			m_pSkillUpButton->GetUICoord(uiCoorde);
			uiCoords.fWidth = uiCoorde.Right() - uiCoords.fX;
			uiCoords.fHeight = uiCoorde.Bottom() - uiCoords.fY;

			return uiCoords.IsInside( fX, fY );
		}
	};

	enum
	{
		ITEM_SIZE_X = 2,
		ITEM_SIZE_Y = 5,
		ITEMSLOT_MAX_SIZE = 10
	};

public:
	CDnSkillDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL );
	virtual ~CDnSkillDlg(void);

protected:
	std::vector<SKILL_SLOT> m_vecSkillSlot;
	std::vector<DnSkillHandle> m_vecSkillItem;

	CEtUIStatic *m_pSkillPage;
	CEtUIStatic *m_pSkillPoint;
	CEtUIButton *m_pButtonPagePrev;
	CEtUIButton *m_pButtonPageNext;
	CEtUIStatic *m_pSelectBar;

	int m_nMaxPage;
	int m_nCurrentPage;
	int m_nSelectedIndex;

	//CDnSkillLevelUpDlg *m_pSkillLevelUpDlg;

	int m_nDragSoundIndex;
	
	CEtUIScrollBar* m_pScrollBar;

protected:
	void UpdateSkillISlot();
	void UpdateSkillPage();
	void UpdateMaxPageNum();
	void UpdatePageButton();
	void UpdateSkillUpButton();
	void UpdateSkillPoint();

	void NextPage();
	void PrevPage();

	void InitSkillSlotList();

	void UpdateSelectBar( int nSlotIndex );
	int FindInsideItem( float fX, float fY );

public:
	void SetItem( MIInventoryItem *pItem );
	void ResetAllItem();

	void OnSkillLevelUp( bool bSuccessed );
	void UpdatePage();

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Process( float fElapsedTime );

	virtual void Render( float fElapsedTime );
};