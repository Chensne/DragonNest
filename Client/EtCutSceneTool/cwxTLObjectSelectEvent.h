#pragma once



// 시간축에 이벤트가 일어났을 경우 통지해주는 이벤트이다.
class cwxTLObjectSelectEvent : public wxNotifyEvent
{
private:
	wxString m_strName;
	int m_iObjectID;
	int m_iUseType;
	float m_fStartTime;


public:
	cwxTLObjectSelectEvent( wxEventType commandType, int id, wxString Name, int iObjectID, int iUseType, float fStartTime );
	virtual ~cwxTLObjectSelectEvent(void);

	wxEvent* Clone( void ) const;
	
	int GetObjectID( void ) { return m_iObjectID; };
	int GetObjectUseType( void ) { return m_iUseType; };
	float GetActionStartTime( void ) { return m_fStartTime; };
};

DECLARE_EVENT_TYPE( wxEVT_TIMELINE_OBJECT_SELECT, -1 )

typedef void (wxEvtHandler::*wxTLObjectSelectEventFunction) (cwxTLObjectSelectEvent&);

#define EVT_TIMELINE_ACTION_SELECT( id, fn ) \
	DECLARE_EVENT_TABLE_ENTRY( wxEVT_TIMELINE_OBJECT_SELECT, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( wxTLObjectSelectEventFunction, &fn), (wxObject*)NULL),
