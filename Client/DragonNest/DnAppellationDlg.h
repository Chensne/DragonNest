#pragma once

#include "EtUIDialog.h"
#include "DnCustomDlg.h"

class CDnAppellationBookDlg;
class CDnAppellationTabDlg;

class CDnAppellationItemDlg;
class CDnAppellationDlg : public CEtUIDialog 
{
public:
	CDnAppellationDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAppellationDlg();

protected:
	CEtUIListBoxEx*	m_pListBoxEx;
	CEtUIStatic*	m_pCurrentName;
	CEtUITextBox*	m_pEffectDesc;
	CEtUITextBox*	m_pContantDesc;
	CEtUITextBox*	m_pTakeCondition;
	CEtUIComboBox*	m_pStateCombo;

	std::vector< std::pair<wstring, int> >	m_vStateParam;

	CEtUIStatic*			m_pCoverAppellationName;	// 커버로 사용중인 칭호
	CDnAppellationBookDlg*	m_pAppellationBookDlg;
	CDnAppellationTabDlg*	m_pAppellationTabDlg;

public:
	enum eDialogMode{ Appellation = 0, CollectionBook };
	enum eAppellationType{ Appellation_Cover = 0, Appellation_Apply };
	int	m_nCoverAppellationIndex;
	int m_nApplyAppellationIndex;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

	void RefreshList();
	void UnselectAllCheckBox( CDnAppellationItemDlg *pCheckItem );
	void RefreshSelect();
	void SortList( const int nStateIndex );
	int  FindPosition( const int nStateIndex, int nPosition , const float value );
	int  FindList( const CDnAppellationItemDlg * pItem, const int nStateIndex );
	float FindValue( const CDnAppellationItemDlg * pItem, const int nStateIndex );
	void SetSelectAppellationName( eAppellationType AppellationType, int nAppellationIndex );
	void ClearAppellationCheck( eAppellationType AppellationType, int nCheckAppellationIndex );
	void ChangeDialogMode( eDialogMode eMode );
	void SetCollectionMaxPage( int nMaxPage );
	void SortPcBangAppellation();

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
	CDnAppellationTabDlg*  GetAppellationTabDlg()	{ return m_pAppellationTabDlg; }
	CDnAppellationBookDlg* GetAppellationBookDlg()	{ return m_pAppellationBookDlg;}
#endif
};


class CDnAppellationTabDlg : public CDnCustomDlg
{
public:
	CDnAppellationTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAppellationTabDlg( void );

protected:
	CEtUIRadioButton * m_pAppellationButton;
	CEtUIRadioButton * m_pCollectionButton;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void ResetButton();

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
	void SetCheckCollectionBtn(bool bShow) { if(m_pCollectionButton) m_pCollectionButton->SetChecked(bShow); }
#endif
};

