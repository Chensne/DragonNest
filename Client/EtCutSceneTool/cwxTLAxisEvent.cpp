#include "stdafx.h"
#include <wx/wx.h>
#include "cwxTLAxisEvent.h"



DEFINE_EVENT_TYPE( wxEVT_TIMELINE_AXIS_EVENT )

cwxTLAxisEvent::cwxTLAxisEvent( wxEventType commandType, int id ) : wxNotifyEvent( commandType, id ), 
																	m_bOnLBDown( false ),
																	m_bOnLBUp( false ),
																	m_bLBIsDown( false ),
																	m_bLBIsUp( false ),
																	m_bIsAxisMove( false ),
																	m_fNowTime( 0.0f )
{
}

cwxTLAxisEvent::~cwxTLAxisEvent(void)
{
}


wxEvent* cwxTLAxisEvent::Clone( void ) const
{
	return new cwxTLAxisEvent( *this );
}