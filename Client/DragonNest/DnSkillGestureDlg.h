#pragma once
#include "DnCustomDlg.h"

class CDnSkillGestureDlg : public CDnCustomDlg
{
	struct SKILL_SLOT
	{
		CDnLifeSkillButton *m_pSlotButton;
		CEtUIStatic *m_pSkillBase;
		CEtUIStatic *m_pSkillName;

		SKILL_SLOT()
			: m_pSkillBase(NULL)
			, m_pSlotButton(NULL)
			, m_pSkillName(NULL)
		{
		}

		void InitSkillSlot()
		{
			m_pSlotButton->ResetSlot();
			m_pSkillName->ClearText();
			m_pSkillBase->SetTextureColor( 0x4CFFFFFF, 0 );
		};

		void SetSkillSlot( int nGestureID, LPCWSTR wszName )
		{
			if( nGestureID == 0 ) return;

			m_pSkillBase->SetTextureColor( 0xFFFFFFFF, 0 );
			m_pSlotButton->SetGestureInfo( nGestureID );
			m_pSkillName->SetText( wszName );
		}
	};

	enum
	{
		ITEMSLOT_MAX_SIZE = 10
	};

public:
	CDnSkillGestureDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL );
	virtual ~CDnSkillGestureDlg(void);

protected:
	std::vector<SKILL_SLOT> m_vecSkillSlot;
	//std::vector<DnSkillHandle> m_vecSkillItem;

	CEtUIStatic *m_pSkillPage;
	CEtUIButton *m_pButtonPagePrev;
	CEtUIButton *m_pButtonPageNext;

	int m_nMaxPage;
	int m_nCurrentPage;

	int m_nDragSoundIndex;

protected:
	void UpdateSkillSlot();
	void UpdateSkillPage();
	void UpdateMaxPageNum();
	void UpdatePageButton();

	void NextPage();
	void PrevPage();

	void InitSkillSlotList();

public:

	void ResetAllItem();

	void UpdateList();

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};