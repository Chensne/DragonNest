#include "stdafx.h"
#include <wx/wx.h>
#include "cwxTLObjectMoveEvent.h"



DEFINE_EVENT_TYPE( wxEVT_TIMELINE_OBJECT_MOVE )

cwxTLObjectMoveEvent::cwxTLObjectMoveEvent( wxEventType commandType, int id, wxString& strName, int iObjectID, float fStartTime ) : wxNotifyEvent( commandType, id ), 
													m_strName( strName ), m_iObjectID( iObjectID ), m_fStartTime( fStartTime )
																	
{
}

cwxTLObjectMoveEvent::~cwxTLObjectMoveEvent(void)
{
}


wxEvent* cwxTLObjectMoveEvent::Clone( void ) const
{
	return new cwxTLObjectMoveEvent( *this );
}