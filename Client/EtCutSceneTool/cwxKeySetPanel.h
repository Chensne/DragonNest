#pragma once

#include <wx/treectrl.h>
#include "ICommandListener.h"


// 키 셋트를 추가 및 관리할 수 있는 패널입니다.
class cwxKeySetPanel : public wxPanel,
					   public ICommandListener
{
private:
	enum
	{
		TREE_KEY_SET_ID = 2000,

		POPUP_ADD_KEYSET,
		POPUP_DEL_KEYSET,
		
		POPUP_ADD_KEY,
		POPUP_DEL_KEY,
	};

	wxBoxSizer*				m_pTopSizer;
	wxTreeCtrl*				m_pTreeKeySet;
	wxTreeItemId			m_TreeRootID;


private:
	void _PopupContextMenu( int iXPos, int iYPos );

public:
	cwxKeySetPanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxKeySetPanel(void);

	void OnContextMenu( wxContextMenuEvent& ContextEvent );
	void OnAddKeySet( wxCommandEvent& CommandEvent );
	void OnDelKeySet( wxCommandEvent& CommandEvent );
	void OnAddKey( wxCommandEvent& CommandEvent );
	void OnDelKey( wxCommandEvent& CommandEvent );

	// from ICommandListener
	void CommandPerformed( ICommand* pCommand );

	DECLARE_EVENT_TABLE()
};
