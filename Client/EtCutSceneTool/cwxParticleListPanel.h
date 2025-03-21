#pragma once
#include "ICommandListener.h"


// 파티클 이벤트를 정리해서 보여주는 리스트 뷰를 담고 있는 패널...
// 화면 편집이 필요한 이벤트들은 picking 말고 쉽게 제어할 수 있는 수단이 필요하다.
class cwxParticleListPanel : public wxPanel,
							 public ICommandListener
{
private:
	enum
	{
		LIST_PARTICLE = 10000,

		POPUP_SELECT,
		//POPUP_SEE_THIS,
	};

	wxBoxSizer*				m_pTopSizer;
	wxListBox*				m_pParticleEventList;

private:
	//void _PopupContextMenu( int iXPos, int iYPos );
	void _UpdateParticleList( void );

public:
	cwxParticleListPanel( wxWindow* pParent, int id = -1 );
	virtual ~cwxParticleListPanel(void);
	
	//void OnContextMenu( wxContextMenuEvent& ContextMenu );
	void OnSelChangeParticleList( wxCommandEvent& ListEvent );
	//void OnMenuSeeThis( wxCommandEvent& MenuEvent );
	//void OnMenuSelect( wxCommandEvent& MenuEvent );
	
	// from ICommandListener
	void CommandPerformed( ICommand* pCommand );

	DECLARE_EVENT_TABLE()
};
