#pragma once



// 시간축에 이벤트가 일어났을 경우 통지해주는 이벤트이다.
class cwxTLObjectMoveEvent : public wxNotifyEvent
{
private:
	wxString m_strName;
	int m_iObjectID;
	float m_fStartTime;


public:
	cwxTLObjectMoveEvent( wxEventType commandType, int id, wxString& strName, int iObjectID, float fStartTime );
	virtual ~cwxTLObjectMoveEvent(void);

	wxEvent* Clone( void ) const;
	
	int GetObjectID( void ) { return m_iObjectID; };
	float GetActionStartTime( void ) { return m_fStartTime; };
};

DECLARE_EVENT_TYPE( wxEVT_TIMELINE_OBJECT_MOVE, -1 )

typedef void (wxEvtHandler::*wxTLObjectMoveEventFunction) (cwxTLObjectMoveEvent&);

#define EVT_TIMELINE_ACTION_MOVE( id, fn ) \
	DECLARE_EVENT_TABLE_ENTRY( wxEVT_TIMELINE_OBJECT_MOVE, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( wxTLObjectMoveEventFunction, &fn), (wxObject*)NULL),
