#pragma once
#include "EtUIDialog.h"
#include "DnCustomDlg.h"

#ifdef PRE_MOD_SELECT_CHAR

enum eCharacterSlotType
{
	SLOT_PLAYER = 0,	// 캐릭터가 있는 슬롯
	SLOT_DELETE_WAIT,	// 캐릭터 삭제 대기 슬롯
	SLOT_ENABLE,		// 캐릭터 생성가능 슬롯
	SLOT_DISABLE,		// 캐릭터 생성불가 슬롯
};

class CDnCharSelectListDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnCharSelectListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharSelectListDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process( float fElapsedTime );
	
public:
	void SetCharacterList( std::vector<TCharListData>& vecCharListData, int nMaxCharCount );
	void SelectCharIndex( int nSelectIndex );
#if defined(PRE_ADD_23829)
	void Show2ndPassCreateButton( bool bShow );
#endif // PRE_ADD_23829
#ifdef PRE_CHARLIST_SORTING
	void SetCharacterListSort( int nCharacterListSort ) { if( m_pCharacterListBoxSortComboBox ) m_pCharacterListBoxSortComboBox->SetSelectedByValue( nCharacterListSort ); }
#endif // PRE_CHARLIST_SORTING

protected:
	CEtUIListBoxEx* m_pCharacterListBox;
	CEtUIStatic*	m_pStaticCharCount;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	CEtUIStatic* m_pTotalLevelInfo;
	void UpdateTotalLevelInfo(int nSelectIndex);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef PRE_CHARLIST_SORTING
	CEtUIComboBox* m_pCharacterListBoxSortComboBox;
#endif // PRE_CHARLIST_SORTING

#ifdef PRE_ADD_NEWCOMEBACK
	std::vector<TCharListData> m_vecCharListData;	
#endif // PRE_ADD_NEWCOMEBACK

public:
#ifdef PRE_ADD_NEWCOMEBACK
	int GetSelectCharIndex();	
	std::vector<TCharListData> & GetCharListData(){
		return m_vecCharListData;
	}
	bool ValidReward();
#endif // PRE_ADD_NEWCOMEBACK
	bool IsNewCharacter();
};


class CDnCharacterInfoDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnCharacterInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharacterInfoDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

public:
	void SetCharacterInfo( eCharacterSlotType eSlotType, TCharListData* pTCharListData, int nSlotIndex );
	eCharacterSlotType GetCharacterSlotType() { return m_eSlotType; }
	void SetSelect( bool bSelect );

protected:
	void ResetControl();
	void SetRemainTime( __time64_t RemainTime );
	void UpdateDeleteRemainTime();

#ifdef PRE_ADD_NEWCOMEBACK
	void EnableComebackUI( bool bComeback );
#endif // PRE_ADD_NEWCOMEBACK

protected:
	CEtUIStatic*		m_pStaticLevel;
	CEtUIStatic*		m_pStaticName;
	CEtUIStatic*		m_pStaticServer;
	CEtUIStatic*		m_pStaticVillage;
	CDnJobIconStatic*	m_pStaticJobIcon;
	CEtUIStatic*		m_pStaticBackground;

	CEtUIStatic*	m_pStaticCreate;
	CEtUIStatic*	m_pStaticCreateText;

	CEtUIStatic*	m_pStaticDelete;
	CEtUIStatic*	m_pStaticDeleteText;
	CEtUIStatic*	m_pStaticDeleteTime;
	CEtUIButton*	m_pButtonDeleteCancel;

	CEtUIStatic*	m_pStaticDisable;

#ifdef PRE_ADD_NEWCOMEBACK
	CEtUIStatic * m_pStaticComebackLight;
	CEtUIStatic * m_pStaticComebackTooltip;
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
	CEtUIStatic* m_pStaticDWCBoard;
#endif // PRE_ADD_DWC
	CEtUIStatic* m_pStaticDWCToolTip;

	int					m_nSlotIndex;
	eCharacterSlotType	m_eSlotType;
	__time64_t			m_ExpectTime;
	BYTE				m_cJob;
	BYTE				m_cAccountLevel;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
protected:
	int m_nCharLevel;
	int m_nWorldID;

public:
	void SetCharLevel(int nLevel) { m_nCharLevel = nLevel; }
	int GetCharLevel() { return m_nCharLevel; }
	void SetWorldID(int nID) { m_nWorldID = nID; }
	int GetWorldID() { return m_nWorldID; }
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

public:
#ifdef PRE_ADD_NEWCOMEBACK
	bool IsComebackUser();	
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
	bool IsDWCChar();
#endif
};

#endif // PRE_MOD_SELECT_CHAR