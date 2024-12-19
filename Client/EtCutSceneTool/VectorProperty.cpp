#include "stdafx.h"
#include "vectorproperty.h"



wxString
wxToString(const EtVector3 &v)
{
	wxString s;
	s<<wxT("[")<<v[0]<<wxT(";")<<v[1]<<wxT(";")<<v[2]<<wxT("]");
	return s;
}

//wxString
//wxToString(const Quaternion &q)
//{
//	wxString s;
//	s<<"["<<q.x<<";"<<q.y<<";"<<q.z<<";"<<q.w<<"]";
//	return s;
//}
//wxString
//wxToString(const ColourValue &cv)
//{
//	wxString s;
//	s<<"["<<cv.r<<";"<<cv.g<<";"<<cv.b<<";"<<cv.a<<"]";
//	return s;
//}



WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP( EtVector3, VectorProperty, EtVector3() )
WX_PG_IMPLEMENT_PROPERTY_CLASS( VectorProperty, wxBaseParentProperty, EtVector3, const EtVector3&, TextCtrl )

VectorPropertyClass::VectorPropertyClass( const wxString& label, const wxString& name, const EtVector3& Vector ) :
										  wxPGPropertyWithChildren( label, name ), m_value( Vector )
{
	wxPG_INIT_REQUIRED_TYPE(EtVector3)
	DoSetValue( (void*)&m_value );

	AddChild( wxFloatProperty(wxT("x"), wxPG_LABEL, m_value.x) );
	AddChild( wxFloatProperty(wxT("y"), wxPG_LABEL, m_value.y) );
	AddChild( wxFloatProperty(wxT("z"), wxPG_LABEL, m_value.z) );
}

VectorPropertyClass::~VectorPropertyClass(void)
{
}


void VectorPropertyClass::DoSetValue( wxPGVariant value )
{
	EtVector3* pValue = (EtVector3*)wxPGVariantToVoidPtr( value );
	m_value = *pValue;
	RefreshChildren();
}



wxPGVariant VectorPropertyClass::DoGetValue( void ) const
{
	return wxPGVariant((void*)&m_value);
}



void VectorPropertyClass::RefreshChildren( void )
{
	if( !GetCount() ) return;

	Item(0)->DoSetValue( m_value.x );
	Item(1)->DoSetValue( m_value.y );
	Item(2)->DoSetValue( m_value.z );
}


void VectorPropertyClass::ChildChanged( wxPGProperty* p )
{
	switch( p->GetIndexInParent() )
	{
		case 0:
			m_value.x = p->DoGetValue().GetDouble();
			break;

		case 1:
			m_value.y = p->DoGetValue().GetDouble();
			break;

		case 2:
			m_value.z = p->DoGetValue().GetDouble();
			break;
	}
}
