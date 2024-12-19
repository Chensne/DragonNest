#pragma once



// �ð��࿡ �̺�Ʈ�� �Ͼ�� ��� �������ִ� �̺�Ʈ�̴�.
class cwxTLObjectResizeEvent : public wxNotifyEvent
{
private:
	wxString m_strName;
	int m_iObjectID;
	float m_fStartTime;
	float m_fTimeLength;



public:
	cwxTLObjectResizeEvent( wxEventType commandType, int id, wxString& strName, int iObjectID, float fStartTime, float fTimeLength );
	virtual ~cwxTLObjectResizeEvent(void);

	wxEvent* Clone( void ) const;
	
	int GetObjectID( void ) { return m_iObjectID; };
	float GetActionStartInUnit( void ) { return m_fStartTime; };
	float GetNewWidthInUnit( void ) { return m_fTimeLength; };
};

DECLARE_EVENT_TYPE( wxEVT_TIMELINE_OBJECT_RESIZE, -1 )

typedef void (wxEvtHandler::*wxTLObjectResizeEventFunction) (cwxTLObjectResizeEvent&);

#define EVT_TIMELINE_ACTION_RESIZE( id, fn ) \
	DECLARE_EVENT_TABLE_ENTRY( wxEVT_TIMELINE_OBJECT_RESIZE, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( wxTLObjectResizeEventFunction, &fn), (wxObject*)NULL),
