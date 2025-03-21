#pragma once



// 시간축에 이벤트가 일어났을 경우 통지해주는 이벤트이다.
class cwxTLAxisEvent : public wxNotifyEvent
{
private:
	// 마우스 버튼 조작 관련
	bool			m_bOnLBDown;
	bool			m_bOnLBUp;
	bool			m_bLBIsDown;
	bool			m_bLBIsUp;
	bool			m_bIsAxisMove;
	float			m_fNowTime;


public:
	cwxTLAxisEvent( wxEventType commandType, int id );
	virtual ~cwxTLAxisEvent(void);

	wxEvent* Clone( void ) const;
	
	void SetOnLBDown( bool bLBDown ) { m_bOnLBDown = bLBDown; };
	void SetOnLBUp( bool bLBUp ) { m_bOnLBUp = bLBUp; };
	void SetLBIsDown( bool bLBIsDown ) { m_bLBIsDown = bLBIsDown; };
	void SetLBIsUp( bool bLBIsUp ) { m_bLBIsUp = bLBIsUp; };
	void SetAxisMove( bool bAxisMove ) { m_bIsAxisMove = bAxisMove; };
	void SetNowTime( float fNowTime ) { m_fNowTime = fNowTime; };

	bool OnLBDown( void ) { return m_bOnLBDown; };
	bool OnLBUp( void ) { return m_bOnLBUp; };
	bool IsLBDown( void ) { return m_bLBIsDown; };
	bool IsLBUp( void ) { return m_bLBIsUp; };
	bool IsAxisMove( void ) { return m_bIsAxisMove; };
	float GetNowTime( void ) { return m_fNowTime; };
};

DECLARE_EVENT_TYPE( wxEVT_TIMELINE_AXIS_EVENT, -1 )

typedef void (wxEvtHandler::*wxTLAxisEventFunction) (cwxTLAxisEvent&);

#define EVT_TIMELINE_AXIS_EVENT( id, fn ) \
	DECLARE_EVENT_TABLE_ENTRY( wxEVT_TIMELINE_AXIS_EVENT, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( wxTLAxisEventFunction, &fn), (wxObject*)NULL),
