#pragma once


#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/propdev.h>
#include <wx/propgrid/advprops.h>

#include "EternityEngine.h"


wxString wxToString( EtVector3& v );


// ���͸� ��Ÿ���� ������Ƽ'
// ���� �����ϳ�. ��.-_-;
WX_PG_DECLARE_VALUE_TYPE_VOIDP( EtVector3 )
WX_PG_DECLARE_PROPERTY( VectorProperty, const EtVector3&, EtVector3() );
class VectorPropertyClass : public wxPGPropertyWithChildren
{
	WX_PG_DECLARE_PROPERTY_CLASS()

public:
	VectorPropertyClass( const wxString& label, const wxString& name, const EtVector3& Vector );
	virtual ~VectorPropertyClass(void);

	WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
	WX_PG_DECLARE_PARENTAL_METHODS()

protected:
	EtVector3			m_value;
};
