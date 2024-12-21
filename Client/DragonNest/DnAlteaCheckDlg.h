#pragma once
#include "DnCustomDlg.h"

#if defined( PRE_ALTEIAWORLD_EXPLORE )

class CDnAlteaCheckDlg : public CDnCustomDlg
{
public:
	struct SClearUI
	{
		CDnItemSlotButton * m_pSlotButton;
		CEtUIStatic * m_pSlot_Static;
		CEtUIStatic * m_pCount_Static;
		CEtUIStatic * m_pBoard_Static;
		CEtUIStatic * m_pCheck_Static;
		CEtUIStatic * m_pText_Static;

		SClearUI() : m_pSlotButton( NULL ), m_pSlot_Static( NULL ), m_pCount_Static( NULL ), m_pBoard_Static( NULL ), m_pCheck_Static( NULL ), m_pText_Static( NULL )
		{}

		void ResetSlot();
		void SetSlot( const int nItemID, const int nCount );
	};

	enum{
		E_Clear_Count = 7,
	};

public:
	CDnAlteaCheckDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAlteaCheckDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );

public:
	void SetNotice( const int nTime );
	void SetWeeklyClearCount( const int nCount );

protected:
	void InitializeTable();
	int GetAlteaMissionByOderID( const int nMissionID );
	void SetSlot( const int nCount, void * pData );

protected:
	SClearUI m_pClearUI[E_Clear_Count];
	CEtUIStatic * m_pNotice_Static;
	std::vector< std::pair<int, int> > m_vAlteaMission;
};

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )