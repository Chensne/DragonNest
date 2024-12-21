#pragma once

#include "EtUIDialog.h"

#ifdef PRE_ADD_CASHINVENTAB

class CDnInvenCashTabDlg : public CEtUIDialog
{

private:


	enum ETABTYPE
	{		
		ETABTYPE_COSTUME = 0,   // �ڽ�Ƭ.
		ETABTYPE_CONSUMABLES,   // �Ҹ�ǰ.
		ETABTYPE_FUNCTIONALITY, // ��ɼ�.
		ETABTYPE_ETC,			// ��Ÿ.
		ETABTYPE_MAX
	};

	std::vector< CEtUIStatic * > m_vecStaticNew;
	std::vector< CEtUIRadioButton * > m_vecRadioBtn;

	std::map< int, int > m_mapType; // map< ItemType, TabType >

	int m_newItemTabIndex; // �������� �߰��� ���ε���.


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

	//  �������� ���� �ǿ� New ��ũ���.
	void AddNewItem( int idx );

};

#endif // PRE_ADD_CASHINVENTAB