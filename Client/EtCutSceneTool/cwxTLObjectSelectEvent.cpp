#include "stdafx.h"
#include <wx/wx.h>
#include "cwxTLObjectSelectEvent.h"



DEFINE_EVENT_TYPE( wxEVT_TIMELINE_OBJECT_SELECT )

cwxTLObjectSelectEvent::cwxTLObjectSelectEvent( wxEventType commandType, int id, wxString Name, 
												int iObjectID, int iUseType, float fStartTime ) : wxNotifyEvent( commandType, id ), 
												m_strName( Name ), m_iObjectID( iObjectID ), m_iUseType( iUseType ), m_fStartTime( fStartTime )
{
}

cwxTLObjectSelectEvent::~cwxTLObjectSelectEvent(void)
{
}


wxEvent* cwxTLObjectSelectEvent::Clone( void ) const
{
	return new cwxTLObjectSelectEvent( *this );
}