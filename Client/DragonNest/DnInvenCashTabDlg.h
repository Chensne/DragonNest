#pragma once

#include "EtUIDialog.h"

#ifdef PRE_ADD_CASHINVENTAB

class CDnInvenCashTabDlg : public CEtUIDialog
{

private:


	enum ETABTYPE
	{		
		ETABTYPE_COSTUME = 0,   // 코스튬.
		ETABTYPE_CONSUMABLES,   // 소모품.
		ETABTYPE_FUNCTIONALITY, // 기능성.
		ETABTYPE_ETC,			// 기타.
		ETABTYPE_MAX
	};

	std::vector< CEtUIStatic * > m_vecStaticNew;
	std::vector< CEtUIRadioButton * > m_vecRadioBtn;

	std::map< int, int > m_mapType; // map< ItemType, TabType >

	int m_newItemTabIndex; // 아이템이 추가된 탭인덱스.


public:

	CDnInvenCashTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnInvenCashTabDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
//	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//	virtual void MoveDialog( float fX, float fY );


public:

	//  아이템이 들어온 탭에 New 마크출력.
	void AddNewItem( int idx );

};

#endif // PRE_ADD_CASHINVENTAB