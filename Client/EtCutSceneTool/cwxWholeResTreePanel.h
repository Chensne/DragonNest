#pragma once

#include <wx/TreeCtrl.h>
#include "ICommandListener.h"


// 현재 지정된 폴더에 있는 리소스 전체를 보여줌
class cwxWholeResTreePanel : public wxPanel,
							 public ICommandListener
{
private:
	enum
	{
		WHOLE_RESOURCE_TREE_ID = 20200,
		BTN_REGISTER_RESOURCE_ID,
	};

	wxBoxSizer*			m_pTopSizer;
	wxTreeCtrl*			m_pTreeCtrl;
	wxTreeItemId		m_TreeRootID;

	wxButton*			m_pBtnRegister;

	//struct S_RES_NODE_PAIR
	//{
	//	wxString		strResourcePath;
	//	wxTreeItemId	TreeId;
	//};

	set<wxString>				m_setAllowMainFolder;
	set<wxString>				m_setAllowFileExt;
	set<wxString>				m_setIgnoreFolder;

	enum
	{
		TREE_ROOT,
		TREE_ACTOR_SET,
		TREE_ACTOR,
		TREE_MAP_SET,
		TREE_MAP,
	};
	map<wxTreeItemId, int>	   m_mapTreeIdKind;

private:
	void _UpdateRegResourceBtn( void );
	void _BuildResourceTree( void );

public:
	cwxWholeResTreePanel( wxWindow* pParent, wxWindowID id = -1 );
	virtual ~cwxWholeResTreePanel(void);

	void OnBtnRegisterRes( wxCommandEvent& ButtonEvent );
	void OnTreeSelChanged( wxTreeEvent& TreeEvent );

	// from ICommandListener
	void CommandPerformed( ICommand* pCommand );

	DECLARE_EVENT_TABLE()
};
