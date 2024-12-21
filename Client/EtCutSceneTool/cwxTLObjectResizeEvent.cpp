#include "stdafx.h"
#include <wx/wx.h>
#include "cwxTLObjectResizeEvent.h"



DEFINE_EVENT_TYPE( wxEVT_TIMELINE_OBJECT_RESIZE )

cwxTLObjectResizeEvent::cwxTLObjectResizeEvent( wxEventType commandType, int id, wxString& strName, int iObjectID, float fStartTime, float fTimeLength ) : wxNotifyEvent( commandType, id ), 
													m_strName( strName ), m_iObjectID( iObjectID ), m_fStartTime( fStartTime ), m_fTimeLength( fTimeLength )
																	
{
}

cwxTLObjectResizeEvent::~cwxTLObjectResizeEvent(void)
{
}


wxEvent* cwxTLObjectResizeEvent::Clone( void ) const
{
	return new cwxTLObjectResizeEvent( *this );
}