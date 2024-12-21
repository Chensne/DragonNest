#pragma once
#include "DnCustomDlg.h"
#include "DnDungeonEnterDlg.h"

class CDnItem;

class CDnDarklairEnterLevelDlg : public CDnCustomDlg
{
public:
	CDnDarklairEnterLevelDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDarklairEnterLevelDlg();

protected:
	CEtUIButton*		m_pButtonEnter;
	CEtUIButton*		m_pButtonCancel;
	CEtUIStatic*		m_pStaticTotalRound;
	CEtUIStatic*		m_pStaticLevel;
	CEtUIStatic*		m_pStaticItemCount;
	CEtUIStatic*		m_pStaticTimer;
	CEtUIStatic*		m_pStaticName;
	CDnItemSlotButton*	m_pButtonItemSlot;
	CEtUIStatic*		m_pStaticItemSlot;

	CDnItem*			m_pNeedItem;
	float				m_fElapsedTime;

	CEtUITextureControl*	m_pTextureDungeonImage;
	EtTextureHandle			m_hIntroArea;

protected:
	void SetTimer( int nTime );

public:
	// CDnCustomDlg
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

	void UpdateFloorInfo( int nMapIndex, int nMinLevel, char cPermitFlag = 0 );
};

