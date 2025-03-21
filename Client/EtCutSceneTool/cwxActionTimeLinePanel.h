#pragma once

#include "cwxTimeLineCtrl.h"

class ICommand;


class cwxActionTimeLinePanel : public wxPanel
{
private:
	enum
	{
		ACTION_TIMELINE_ID = 21000,
		POPUP_COPY,
		POPUP_PASTE,
		POPUP_SNAP_ACTION,
		POPUP_REMOVE_ACTION,
	};

	wxBoxSizer*					m_pTopSizer;

	cwxTimeLineCtrl*			m_pTimeLine;

	set<int>					m_setRegiActor;

	int							m_iOriEditMode;

private:
	void _PopupContextMenu( int iXPos, int iYPos );
	void _KeyObjectUpdate( ICommand* pCommand, wxCSConv &MBConv );
	void _ActionObjectUpdate( ICommand* pCommand, wxCSConv &MBConv );

public:
	cwxActionTimeLinePanel( wxWindow* pParent, wxWindowID id );
	virtual ~cwxActionTimeLinePanel(void);

	bool Enable( bool bEnable = true );

	void OnTLObjectSelect( cwxTLObjectSelectEvent& TLSelectEvent );
	void OnTLObjectMove( cwxTLObjectMoveEvent& TLMoveEvent );
	void OnTLObjectResize( cwxTLObjectResizeEvent& TLResizeEvent );
	void OnTLAxisEvent( cwxTLAxisEvent& TLAxisEvent );
	//void OnContextMenu( wxContextMenuEvent& ContextEvent );
	void OnMouseEvent( wxMouseEvent& MouseEvent );

	void OnMenuCopy( wxCommandEvent& MenuEvent );
	void OnMenuPaste( wxCommandEvent& MenuEvent );
	void OnMenuSnapAction( wxCommandEvent& MenuEvent );
	void OnMenuRemoveAction( wxCommandEvent& MenuEvent );

	void OnMouseEventFromFrame( wxMouseEvent& MouseEvent );

	void ReflectData( void );
	void ModifyObject( ICommand* pCommand );

	DWORD GetNowTime( void );
	void SetTime( float fTime );


	DECLARE_EVENT_TABLE()
};
