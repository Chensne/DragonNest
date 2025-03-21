#pragma once
#include "ICommandListener.h"
#include <wx/listctrl.h>


class wxListView;

// 등록된 리소스가 표시되는 패널
class cwxRegisteredResPanel : public wxPanel,
							  public ICommandListener
{
public:
	enum
	{
		LIST_REGISTERED_RES_ID = 20400,

		// 팝업 메뉴
		POPUP_INSERT_ACTION,
		POPUP_INSERT_KEY,
		POPUP_INSERT_EVENT,
		POPUP_REMOVE_RES,
		POPUP_SEE_THIS,
		POPUP_SELECT,
	};

private:
	wxBoxSizer*			m_pTopSizer;
	wxListView*			m_pListRegisteredRes;


private:
	void _InsertResListColumn( void );
	void _UpdateRegResList( void );
	void _PopupContextMenu( int iXPos, int iYPos );
	//int _GetResInfo( int iIndex, /*IN OUT*/ wxString* pstrResName, /*IN OUT*/ wxString* pstrResFileName );


public:
	cwxRegisteredResPanel( wxWindow* pParent, wxWindowID Id );
	virtual ~cwxRegisteredResPanel(void);

	// event handling
	void OnContextMenu( wxContextMenuEvent& ContextEvent );
	void OnMenuRemoveRes( wxCommandEvent& MenuEvent );
	void OnMenuInsertAction( wxCommandEvent& MenuEvent );
	void OnMenuInsertKey( wxCommandEvent& MenuEvent );
	void OnMenuSeeThis( wxCommandEvent& MenuEvent );
	void OnMenuSelect( wxCommandEvent& MenuEvent );

	//void OnMenuInsertEvent( wxCommandEvent& MenuEvent );
	void OnSelChangeRegResList( wxListEvent& ListEvent );

	// from IEventListener
	virtual void CommandPerformed( ICommand* pCommand );

	DECLARE_EVENT_TABLE()
};
