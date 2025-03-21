/////////////////////////////////////////////////////////////////////////////
// Name:        propgridsample.cpp
// Purpose:     wxPropertyGrid sample
// Author:      Jaakko Salli
// Modified by:
// Created:     Sep-25-2004
// RCS-ID:      $Id:
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

//
//
// NOTES
//
// * Examples of custom property classes are in sampleprops.cpp.
//
// * Additional ones can be found below.
//
// * Currently there is no example of a custom property editor. However,
//   SpinCtrl editor sample is well-commented. It can be found in
//   contrib/src/propgrid/advprops.cpp.
//
// * To find code that populates the grid with properties, search for
//   string "::Populate".
//
// * To find code that handles property grid changes, search for string
//   "::OnPropertyGridChange".
//
// * At the end of file there is example code for using the owner-drawn combo
//   box independently outside the wxPropertyGrid.
//
//

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/numdlg.h>

// -----------------------------------------------------------------------


// Main propertygrid header.
#include <wx/propgrid/propgrid.h>

// Needed for implementing custom properties.
#include <wx/propgrid/propdev.h>

// Extra property classes.
#include <wx/propgrid/advprops.h>

// This defines wxPropertyGridManager.
#include <wx/propgrid/manager.h>

// XRC-handler (note that the source portion has to be added separately in the project)
#include <wx/propgrid/xh_propgrid.h>

#include "propgridsample.h"
#include "sampleprops.h"

#if wxUSE_DATEPICKCTRL
    #include <wx/datectrl.h>
#endif

#include <wx/artprov.h>

// -----------------------------------------------------------------------
// wxTestCustomFlagsProperty
// -----------------------------------------------------------------------

//
// Constant definitions required by wxFlagsProperty examples.
//

static const wxChar* _fs_framestyle_labels[] = {
    wxT("wxCAPTION"),
    wxT("wxMINIMIZE"),
    wxT("wxMAXIMIZE"),
    wxT("wxCLOSE_BOX"),
    wxT("wxSTAY_ON_TOP"),
    wxT("wxSYSTEM_MENU"),
    wxT("wxRESIZE_BORDER"),
    wxT("wxFRAME_TOOL_WINDOW"),
    wxT("wxFRAME_NO_TASKBAR"),
    wxT("wxFRAME_FLOAT_ON_PARENT"),
    wxT("wxFRAME_SHAPED"),
    (const wxChar*) NULL
};

static const long _fs_framestyle_values[] = {
    wxCAPTION,
    wxMINIMIZE,
    wxMAXIMIZE,
    wxCLOSE_BOX,
    wxSTAY_ON_TOP,
    wxSYSTEM_MENU,
    wxRESIZE_BORDER,
    wxFRAME_TOOL_WINDOW,
    wxFRAME_NO_TASKBAR,
    wxFRAME_FLOAT_ON_PARENT,
    wxFRAME_SHAPED
};


WX_PG_IMPLEMENT_CUSTOM_FLAGS_PROPERTY(wxTestCustomFlagsProperty,
                                      _fs_framestyle_labels,
                                      _fs_framestyle_values,
                                      wxDEFAULT_FRAME_STYLE)

WX_PG_IMPLEMENT_CUSTOM_ENUM_PROPERTY(wxTestCustomEnumProperty,
                                      _fs_framestyle_labels,
                                      _fs_framestyle_values,
                                      wxCAPTION)


// Colour labels. Last (before NULL, if any) must be Custom.
static const wxChar* mycolprop_labels[] = {
    wxT("Black"),
    wxT("Blue"),
    wxT("Brown"),
    wxT("Custom"),
    (const wxChar*) NULL
};

// Relevant colour values as unsigned longs.
static unsigned long mycolprop_colours[] = {
    wxPG_COLOUR(0,0,0),
    wxPG_COLOUR(0,0,255),
    wxPG_COLOUR(166,124,81),
    wxPG_COLOUR(0,0,0)
};

// Implement property class. Third argument is optional values array,
// but in this example we are only interested in creating a shortcut
// for user to access the colour values. Last arg is itemcount, but
// it will be deprecated in the future.
WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(wxMyColourProperty)
WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR(wxMyColourProperty,
                                                     mycolprop_labels,
                                                     (long*)NULL,
                                                     mycolprop_colours)


WX_PG_DECLARE_CUSTOM_COLOUR_PROPERTY(wxMyColour2Property)
WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY(wxMyColour2Property,
                                       mycolprop_labels,
                                       (long*)NULL,
                                       mycolprop_colours)



// Just testing the macros
WX_PG_DECLARE_STRING_PROPERTY(wxTestStringProperty)
WX_PG_IMPLEMENT_STRING_PROPERTY(wxTestStringProperty,wxPG_NO_ESCAPE)
bool wxTestStringProperty::OnButtonClick( wxPropertyGrid*,
                                          wxString& )
{
    ::wxMessageBox(wxT("Button Clicked"));
    return true;
}

WX_PG_DECLARE_STRING_PROPERTY(wxTextStringPropertyWithValidator)
WX_PG_IMPLEMENT_STRING_PROPERTY_WITH_VALIDATOR(wxTextStringPropertyWithValidator,
                                               wxPG_NO_ESCAPE)

bool wxTextStringPropertyWithValidator::OnButtonClick( wxPropertyGrid* WXUNUSED(propgrid),
                                                       wxString& WXUNUSED(value) )
{
    ::wxMessageBox(wxT("Button Clicked"));
    return true;
}

wxValidator* wxTextStringPropertyWithValidator::DoGetValidator() const
{
#if wxUSE_VALIDATORS
    WX_PG_DOGETVALIDATOR_ENTRY()
    wxTextValidator* validator = new
        wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST);
    wxArrayString oValid;
    oValid.Add(wxT("0"));
    oValid.Add(wxT("1"));
    oValid.Add(wxT("2"));
    oValid.Add(wxT("3"));
    oValid.Add(wxT("4"));
    oValid.Add(wxT("5"));
    oValid.Add(wxT("6"));
    oValid.Add(wxT("7"));
    oValid.Add(wxT("8"));
    oValid.Add(wxT("9"));
    oValid.Add(wxT("$"));
    validator->SetIncludes(oValid);
    WX_PG_DOGETVALIDATOR_EXIT(validator)
#else
    return NULL;
#endif
}

// -----------------------------------------------------------------------

//
// Test customizing wxColourProperty via subclassing
//
// * Includes custom colour entry.
// * Includes extra custom entry.
//
class MyColourProperty3 : public wxColourProperty
{
public:
    MyColourProperty3( const wxString& label = wxPG_LABEL,
                       const wxString& name = wxPG_LABEL,
                       const wxColour& value = *wxWHITE )
        : wxColourProperty(label, name, value)
    {
        wxPGChoices colours;
        colours.Add(wxT("White"));
        colours.Add(wxT("Black"));
        colours.Add(wxT("Red"));
        colours.Add(wxT("Green"));
        colours.Add(wxT("Blue"));
        colours.Add(wxT("Custom"));
        colours.Add(wxT("None"));
        m_choices = colours;
        SetIndex(0);
        wxVariant variant;
        variant << value;
        SetValue(variant);
    }

    virtual ~MyColourProperty3()
    {
    }

    virtual wxColour GetColour( int index ) const
    {
        switch (index)
        {
            case 0: return *wxWHITE;
            case 1: return *wxBLACK;
            case 2: return *wxRED;
            case 3: return *wxGREEN;
            case 4: return *wxBLUE;
            case 5:
                // Return current colour for the custom entry
                wxColour col;
                if ( GetIndex() == GetCustomColourIndex() )
                {
                    if ( m_value.IsNull() )
                        return col;
                    col << m_value;
                    return col;
                }
                return *wxWHITE;
        };
        return wxColour();
    }

    virtual wxString ColourToString( const wxColour& col, int index ) const
    {
        if ( index == (int)(m_choices.GetCount()-1) )
            return wxT("");

        return wxColourProperty::ColourToString(col, index);
    }

    virtual int GetCustomColourIndex() const
    {
        return m_choices.GetCount()-2;
    }
};

// -----------------------------------------------------------------------

#if wxUSE_VALIDATORS

// wxValidator for testing

class wxInvalidWordValidator : public wxValidator
{
public:

    wxInvalidWordValidator( const wxString& invalidWord )
        : wxValidator(), m_invalidWord(invalidWord)
    {
    }

    virtual wxObject* Clone() const
    {
        return new wxInvalidWordValidator(m_invalidWord);
    }

    virtual bool Validate(wxWindow* WXUNUSED(parent))
    {
        wxTextCtrl* tc = wxDynamicCast(GetWindow(), wxTextCtrl);
        wxCHECK_MSG(tc, true, wxT("validator window must be wxTextCtrl"));

        wxString val = tc->GetValue();

        if ( val.find(m_invalidWord) == wxString::npos )
            return true;

        ::wxMessageBox(wxString::Format(wxT("%s is not allowed word"),m_invalidWord.c_str()),
                       wxT("Validation Failure"));

        return false;
    }

private:
    wxString    m_invalidWord;
};

#endif // wxUSE_VALIDATORS

// -----------------------------------------------------------------------
// wxSampleMultiButtonEditor
//   A sample editor class that has multiple buttons.
// -----------------------------------------------------------------------

class wxSampleMultiButtonEditor : public wxPGTextCtrlEditor
{
    WX_PG_DECLARE_EDITOR_CLASS(wxSampleMultiButtonEditor)
public:
    wxSampleMultiButtonEditor() {}
    virtual ~wxSampleMultiButtonEditor() {}

    wxPG_DECLARE_CREATECONTROLS
    virtual bool OnEvent( wxPropertyGrid* propGrid,
                          wxPGProperty* property,
                          wxWindow* ctrl,
                          wxEvent& event ) const;

};

WX_PG_IMPLEMENT_EDITOR_CLASS(SampleMultiButtonEditor,wxSampleMultiButtonEditor,
                             wxPGTextCtrlEditor)


wxPGWindowList wxSampleMultiButtonEditor::CreateControls( wxPropertyGrid* propGrid,
                                                          wxPGProperty* property,
                                                          const wxPoint& pos,
                                                          const wxSize& sz ) const
{
    // Create and populate buttons-subwindow
    wxPGMultiButton* buttons = new wxPGMultiButton( propGrid, sz );

    buttons->Add( wxT("...") );
    buttons->Add( wxT("A") );
#if wxUSE_BMPBUTTON
    buttons->Add( wxArtProvider::GetBitmap(wxART_FOLDER) );
#endif

    // Create the 'primary' editor control (textctrl in this case)
    wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls
                             ( propGrid, property, pos, buttons->GetPrimarySize() );

    // Finally, move buttons-subwindow to correct position and make sure
    // returned wxPGWindowList contains our custom button list.
    buttons->FinalizePosition(pos);

    wndList.SetSecondary( buttons );
    return wndList;
}

bool wxSampleMultiButtonEditor::OnEvent( wxPropertyGrid* propGrid,
                                         wxPGProperty* property,
                                         wxWindow* ctrl,
                                         wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        wxPGMultiButton* buttons = (wxPGMultiButton*) propGrid->GetEditorControlSecondary();

        if ( event.GetId() == buttons->GetButtonId(1) )
        {
            wxMessageBox(wxT("Second button was pressed"));
            return true;
        }
        if ( event.GetId() == buttons->GetButtonId(2) )
        {
            wxMessageBox(wxT("Third button was pressed"));
            return true;
        }
    }
    return wxPGTextCtrlEditor::OnEvent(propGrid, property, ctrl, event);
}

// -----------------------------------------------------------------------
// AdvImageFile Property
// -----------------------------------------------------------------------

class wxMyImageInfo;

WX_DECLARE_OBJARRAY(wxMyImageInfo, wxArrayMyImageInfo);

class wxMyImageInfo
{
public:
    wxString    m_path;
    wxBitmap*   m_pThumbnail1; // smaller thumbnail
    wxBitmap*   m_pThumbnail2; // larger thumbnail

    wxMyImageInfo ( const wxString& str )
    {
        m_path = str;
        m_pThumbnail1 = (wxBitmap*) NULL;
        m_pThumbnail2 = (wxBitmap*) NULL;
    }
    virtual ~wxMyImageInfo()
    {
        if ( m_pThumbnail1 )
            delete m_pThumbnail1;
        if ( m_pThumbnail2 )
            delete m_pThumbnail2;
    }

};


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxArrayMyImageInfo);

wxArrayMyImageInfo  g_myImageArray;


// Preferred thumbnail height.
#define PREF_THUMBNAIL_HEIGHT       64


wxPGChoices wxAdvImageFileProperty::ms_choices;

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxAdvImageFileProperty,wxFileProperty,
                               wxString,const wxString&,ChoiceAndButton)


wxAdvImageFileProperty::wxAdvImageFileProperty( const wxString& label,
                                                          const wxString& name,
                                                          const wxString& value)
    : wxFileProperty(label,name,value)
{
    m_wildcard = wxPGGetDefaultImageWildcard();

    m_index = -1;

    m_pImage = (wxImage*) NULL;

    // Only show names.
    m_flags &= ~(wxPG_PROP_SHOW_FULL_FILENAME);
}

wxAdvImageFileProperty::~wxAdvImageFileProperty ()
{
    // Delete old image
    if ( m_pImage )
    {
        delete m_pImage;
        m_pImage = (wxImage*) NULL;
    }
}

void wxAdvImageFileProperty::OnSetValue()
{
    wxFileProperty::OnSetValue();

    // Delete old image
    if ( m_pImage )
    {
        delete m_pImage;
        m_pImage = (wxImage*) NULL;
    }

    wxString imagename = GetValueAsString(0);

    if ( imagename.length() )
    {
        size_t prevCount = g_myImageArray.GetCount();
        int index = ms_choices.Index(imagename);

        // If not in table, add now.
        if ( index == wxNOT_FOUND )
        {
            ms_choices.Add( imagename );
            g_myImageArray.Add( new wxMyImageInfo( m_filename.GetFullPath() ) );

            index = g_myImageArray.GetCount() - 1;
        }

        // If no thumbnail ready, then need to load image.
        if ( !g_myImageArray[index].m_pThumbnail2 )
        {
            // Load if file exists.
            if ( m_filename.FileExists() )
                m_pImage = new wxImage( m_filename.GetFullPath() );
        }

        m_index = index;

        wxPropertyGrid* pg = GetGrid();
        wxWindow* control = pg->GetEditorControl();

        if ( pg->GetSelection() == this && control )
        {
            wxString name = GetValueAsString(0);

            if ( g_myImageArray.GetCount() != prevCount )
            {
                wxASSERT( g_myImageArray.GetCount() == (prevCount+1) );

                // Add to the control's array.
                // (should be added to own array earlier)

                if ( control )
                    GetEditorClass()->InsertItem(control, name, -1);
            }

            if ( control )
               GetEditorClass()->UpdateControl(this, control);
        }
    }
    else
        m_index = -1;
}

int wxAdvImageFileProperty::GetChoiceInfo( wxPGChoiceInfo* choiceinfo )
{
    if ( choiceinfo )
        choiceinfo->m_choices = &ms_choices;

    return m_index;
}

bool wxAdvImageFileProperty::IntToValue( wxVariant& variant, int number, int WXUNUSED(argFlags) ) const
{
    wxASSERT( number >= 0 );
    return StringToValue( variant, ms_choices.GetLabel(number), wxPG_FULL_VALUE );
}

bool wxAdvImageFileProperty::OnEvent( wxPropertyGrid* propgrid, wxWindow* primary,
   wxEvent& event )
{
    if ( propgrid->IsMainButtonEvent(event) )
    {
        return wxFileProperty::OnEvent(propgrid,primary,event);
    }
    return false;
}

wxSize wxAdvImageFileProperty::OnMeasureImage( int item ) const
{
    if ( item == -1 )
        return wxPG_DEFAULT_IMAGE_SIZE;

    return wxSize(PREF_THUMBNAIL_HEIGHT,PREF_THUMBNAIL_HEIGHT);
}

void wxAdvImageFileProperty::LoadThumbnails( size_t index )
{
    wxMyImageInfo& mii = g_myImageArray[index];

    if ( !mii.m_pThumbnail2 )
    {

        if ( !m_pImage || !m_pImage->Ok() ||
             m_filename != mii.m_path
           )
        {
            if ( m_pImage )
                delete m_pImage;
            m_pImage = new wxImage( mii.m_path );
        }

        if ( m_pImage && m_pImage->Ok() )
        {
            int im_wid = m_pImage->GetWidth();
            int im_hei = m_pImage->GetHeight();
            if ( im_hei > PREF_THUMBNAIL_HEIGHT )
            {
                // TNW = (TNH*IW)/IH
                im_wid = (PREF_THUMBNAIL_HEIGHT*m_pImage->GetWidth())/m_pImage->GetHeight();
                im_hei = PREF_THUMBNAIL_HEIGHT;
            }

            m_pImage->Rescale( im_wid, im_hei );

            mii.m_pThumbnail2 = new wxBitmap( *m_pImage );

            wxSize cis = GetParentState()->GetGrid()->GetImageSize();
            m_pImage->Rescale ( cis.x, cis.y );

            mii.m_pThumbnail1 = new wxBitmap( *m_pImage );

        }

        if ( m_pImage )
        {
            delete m_pImage;
            m_pImage = (wxImage*) NULL;
        }
    }
}

void wxAdvImageFileProperty::OnCustomPaint( wxDC& dc,
                                                 const wxRect& rect,
                                                 wxPGPaintData& pd )
{
    int index = m_index;
    if ( pd.m_choiceItem >= 0 )
        index = pd.m_choiceItem;

    //wxLogDebug(wxT("%i"),index);

    if ( index >= 0 )
    {
        LoadThumbnails(index);

        // Is this a measure item call?
        if ( rect.x < 0 )
        {
            // Variable height
            //pd.m_drawnHeight = PREF_THUMBNAIL_HEIGHT;
            wxBitmap* pBitmap = (wxBitmap*)g_myImageArray[index].m_pThumbnail2;
            if ( pBitmap )
                pd.m_drawnHeight = pBitmap->GetHeight();
            else
                pd.m_drawnHeight = 16;
            return;
        }

        // Draw the thumbnail

        wxBitmap* pBitmap;

        if ( pd.m_choiceItem >= 0 )
            pBitmap = (wxBitmap*)g_myImageArray[index].m_pThumbnail2;
        else
            pBitmap = (wxBitmap*)g_myImageArray[index].m_pThumbnail1;

        if ( pBitmap )
        {
            dc.DrawBitmap ( *pBitmap, rect.x, rect.y, FALSE );

            // Tell the caller how wide we drew.
            pd.m_drawnWidth = pBitmap->GetWidth();

            return;
        }
    }

    // No valid file - just draw a white box.
    dc.SetBrush ( *wxWHITE_BRUSH );
    dc.DrawRectangle ( rect );

}

// -----------------------------------------------------------------------
// wxVectorProperty
// -----------------------------------------------------------------------

// See propgridsample.h for wxVector3f

WX_PG_IMPLEMENT_VARIANT_DATA(wxVector3fVariantData, wxVector3f)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxVectorProperty,wxPGProperty,
                               wxVector3f,const wxVector3f&,TextCtrl)


wxVectorProperty::wxVectorProperty( const wxString& label,
                                              const wxString& name,
                                              const wxVector3f& value )
    : wxPGProperty(label,name)
{
    SetValue( wxVector3fToVariant(value) );
    AddChild( new wxFloatProperty(wxT("X"),wxPG_LABEL,value.x) );
    AddChild( new wxFloatProperty(wxT("Y"),wxPG_LABEL,value.y) );
    AddChild( new wxFloatProperty(wxT("Z"),wxPG_LABEL,value.z) );
}

wxVectorProperty::~wxVectorProperty() { }

void wxVectorProperty::RefreshChildren()
{
    if ( !GetCount() ) return;
    wxVector3f& vector = wxVector3fFromVariant(m_value);
    Item(0)->SetValue( vector.x );
    Item(1)->SetValue( vector.y );
    Item(2)->SetValue( vector.z );
}

void wxVectorProperty::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{
    wxVector3f& vector = wxVector3fFromVariant(thisValue);
    switch ( childIndex )
    {
        case 0: vector.x = childValue.GetDouble(); break;
        case 1: vector.y = childValue.GetDouble(); break;
        case 2: vector.z = childValue.GetDouble(); break;
    }
}


// -----------------------------------------------------------------------
// wxTriangleProperty
// -----------------------------------------------------------------------

// See propgridsample.h for wxTriangle

WX_PG_IMPLEMENT_VARIANT_DATA(wxTriangleVariantData, wxTriangle)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxTriangleProperty,wxPGProperty,
                               wxTriangle,const wxTriangle&,TextCtrl)


wxTriangleProperty::wxTriangleProperty( const wxString& label,
                                                  const wxString& name,
                                                  const wxTriangle& value)
    : wxPGProperty(label,name)
{
    SetValue( wxTriangleToVariant(value) );
    AddChild( new wxVectorProperty(wxT("A"),wxPG_LABEL,value.a) );
    AddChild( new wxVectorProperty(wxT("B"),wxPG_LABEL,value.b) );
    AddChild( new wxVectorProperty(wxT("C"),wxPG_LABEL,value.c) );
}

wxTriangleProperty::~wxTriangleProperty() { }

void wxTriangleProperty::RefreshChildren()
{
    if ( !GetCount() ) return;
    wxTriangle& triangle = wxTriangleFromVariant(m_value);
    Item(0)->SetValue( wxVector3fToVariant(triangle.a) );
    Item(1)->SetValue( wxVector3fToVariant(triangle.b) );
    Item(2)->SetValue( wxVector3fToVariant(triangle.c) );
}

void wxTriangleProperty::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{
    wxTriangle& triangle = wxTriangleFromVariant(thisValue);
    wxVector3f& vector = wxVector3fFromVariant(childValue);
    switch ( childIndex )
    {
        case 0: triangle.a = vector; break;
        case 1: triangle.b = vector; break;
        case 2: triangle.c = vector; break;
    }
}

// -----------------------------------------------------------------------


class wxSingleChoiceDialogAdapter : public wxPGEditorDialogAdapter
{
public:

    wxSingleChoiceDialogAdapter( const wxPGChoices& choices )
        : wxPGEditorDialogAdapter(), m_choices(choices)
    {
    }

    virtual bool DoShowDialog( wxPropertyGrid* WXUNUSED(propGrid),
                               wxPGProperty* WXUNUSED(property) )
    {
        wxString s = ::wxGetSingleChoice(wxT("Message"),
                                         wxT("Caption"),
                                         m_choices.GetLabels());
        if ( s.length() )
        {
            SetValue(s);
            return true;
        }

        return false;
    }

protected:
    const wxPGChoices&  m_choices;
};


class SingleChoiceProperty : public wxStringProperty
{
public:

    SingleChoiceProperty( const wxString& label,
                          const wxString& name = wxPG_LABEL,
                          const wxString& value = wxEmptyString )
        : wxStringProperty(label, name, value)
    {
        // Prepare choices
        m_choices.Add(wxT("Cat"));
        m_choices.Add(wxT("Dog"));
        m_choices.Add(wxT("Gibbon"));
        m_choices.Add(wxT("Otter"));
    }

    // Set editor to have button
    virtual const wxPGEditor* DoGetEditorClass() const
    {
        return wxPG_EDITOR(TextCtrlAndButton);
    }

    // Set what happens on button click
    virtual wxPGEditorDialogAdapter* GetEditorDialog() const
    {
        return new wxSingleChoiceDialogAdapter(m_choices);
    }

protected:
    wxPGChoices m_choices;
};

// -----------------------------------------------------------------------

enum
{
    PGID = 1,
    TCID,
    ID_ABOUT,
    ID_QUIT,
    ID_APPENDPROP,
    ID_APPENDCAT,
    ID_INSERTPROP,
    ID_INSERTCAT,
    ID_ENABLE,
    ID_HIDE,
    ID_DELETE,
    ID_DELETER,
    ID_DELETEALL,
    ID_UNSPECIFY,
    ID_ITERATE1,
    ID_ITERATE2,
    ID_ITERATE3,
    ID_ITERATE4,
    ID_CLEARMODIF,
    ID_FREEZE,
    ID_DUMPLIST,
    ID_COLOURSCHEME1,
    ID_COLOURSCHEME2,
    ID_COLOURSCHEME3,
    ID_CATCOLOURS,
    ID_SETCOLOUR,
    ID_STATICLAYOUT,
    ID_CLEAR,
    ID_POPULATE1,
    ID_POPULATE2,
    ID_COLLAPSE,
    ID_COLLAPSEALL,
    ID_GETVALUES,
    ID_SETVALUES,
    ID_SETVALUES2,
    ID_RUNTESTFULL,
    ID_RUNTESTPARTIAL,
    ID_SAVETOFILE,
    ID_SAVETOFILE2,
    ID_LOADFROMFILE,
    ID_CHANGEFLAGSITEMS,
    ID_TESTINSERTCHOICE,
    ID_TESTDELETECHOICE,
    ID_INSERTPAGE,
    ID_REMOVEPAGE,
    ID_SETSPINCTRLEDITOR,
    ID_SETPROPERTYVALUE,
    ID_TESTREPLACE,
    ID_SETCOLUMNS,
    ID_TESTXRC,
    ID_ENABLECOMMONVALUES,
    ID_SELECTSTYLE,
    ID_SAVESTATE,
    ID_LOADSTATE
};

BEGIN_EVENT_TABLE(FormMain, wxFrame)
    EVT_IDLE(FormMain::OnIdle)
    EVT_MOVE(FormMain::OnMove)
    EVT_SIZE(FormMain::OnResize)
    EVT_PAINT(FormMain::OnPaint)

    // This occurs when a property is selected
    EVT_PG_SELECTED( PGID, FormMain::OnPropertyGridSelect )
    // This occurs when a property value changes
    EVT_PG_CHANGED( PGID, FormMain::OnPropertyGridChange )
    // This occurs just prior a property value is changed
    EVT_PG_CHANGING( PGID, FormMain::OnPropertyGridChanging )
    // This occurs when a mouse moves over another property
    EVT_PG_HIGHLIGHTED( PGID, FormMain::OnPropertyGridHighlight )
    // This occurs when mouse is right-clicked.
    EVT_PG_RIGHT_CLICK( PGID, FormMain::OnPropertyGridItemRightClick )
    // This occurs when mouse is double-clicked.
    EVT_PG_DOUBLE_CLICK( PGID, FormMain::OnPropertyGridItemDoubleClick )
    // This occurs when propgridmanager's page changes.
    EVT_PG_PAGE_CHANGED( PGID, FormMain::OnPropertyGridPageChange )
    // This occurs when property's editor button (if any) is clicked.
    EVT_BUTTON( PGID, FormMain::OnPropertyGridButtonClick )

    EVT_PG_ITEM_COLLAPSED( PGID, FormMain::OnPropertyGridItemCollapse )
    EVT_PG_ITEM_EXPANDED( PGID, FormMain::OnPropertyGridItemExpand )

    EVT_TEXT( PGID, FormMain::OnPropertyGridTextUpdate )

    EVT_KEY_DOWN( FormMain::OnPropertyGridKeyEvent )
    EVT_KEY_UP( FormMain::OnPropertyGridKeyEvent )

    EVT_MENU( ID_APPENDPROP, FormMain::OnAppendPropClick )
    EVT_MENU( ID_APPENDCAT, FormMain::OnAppendCatClick )
    EVT_MENU( ID_INSERTPROP, FormMain::OnInsertPropClick )
    EVT_MENU( ID_INSERTCAT, FormMain::OnInsertCatClick )
    EVT_MENU( ID_DELETE, FormMain::OnDelPropClick )
    EVT_MENU( ID_DELETER, FormMain::OnDelPropRClick )
    EVT_MENU( ID_UNSPECIFY, FormMain::OnMisc )
    EVT_MENU( ID_DELETEALL, FormMain::OnClearClick )
    EVT_MENU( ID_ENABLE, FormMain::OnEnableDisable )
    EVT_MENU( ID_HIDE, FormMain::OnHideShow )
    EVT_MENU( ID_ITERATE1, FormMain::OnIterate1Click )
    EVT_MENU( ID_ITERATE2, FormMain::OnIterate2Click )
    EVT_MENU( ID_ITERATE3, FormMain::OnIterate3Click )
    EVT_MENU( ID_ITERATE4, FormMain::OnIterate4Click )
    EVT_MENU( ID_SETCOLOUR, FormMain::OnMisc )
    EVT_MENU( ID_CLEARMODIF, FormMain::OnClearModifyStatusClick )
    EVT_MENU( ID_FREEZE, FormMain::OnFreezeClick )
    EVT_MENU( ID_DUMPLIST, FormMain::OnDumpList )

    EVT_MENU( ID_COLOURSCHEME1, FormMain::OnColourScheme )
    EVT_MENU( ID_COLOURSCHEME2, FormMain::OnColourScheme )
    EVT_MENU( ID_COLOURSCHEME3, FormMain::OnColourScheme )
    EVT_MENU( ID_COLOURSCHEME4, FormMain::OnColourScheme )

    EVT_MENU( ID_ABOUT, FormMain::OnAbout )
    EVT_MENU( ID_QUIT, FormMain::OnCloseClick )

    EVT_MENU( ID_CATCOLOURS, FormMain::OnCatColours )
    EVT_MENU( ID_SETCOLUMNS, FormMain::OnSetColumns )
    EVT_MENU( ID_TESTXRC, FormMain::OnTestXRC )
    EVT_MENU( ID_ENABLECOMMONVALUES, FormMain::OnEnableCommonValues )
    EVT_MENU( ID_SELECTSTYLE, FormMain::OnSelectStyle )

    EVT_MENU( ID_STATICLAYOUT, FormMain::OnMisc )
    EVT_MENU( ID_CLEAR, FormMain::OnMisc )
    EVT_MENU( ID_COLLAPSE, FormMain::OnMisc )
    EVT_MENU( ID_COLLAPSEALL, FormMain::OnMisc )

    EVT_MENU( ID_POPULATE1, FormMain::OnPopulateClick )
    EVT_MENU( ID_POPULATE2, FormMain::OnPopulateClick )

    EVT_MENU( ID_GETVALUES, FormMain::OnMisc )
    EVT_MENU( ID_SETVALUES, FormMain::OnMisc )
    EVT_MENU( ID_SETVALUES2, FormMain::OnMisc )

    EVT_MENU( ID_SAVETOFILE, FormMain::OnSaveToFileClick )
    EVT_MENU( ID_SAVETOFILE2, FormMain::OnSaveToFileClick )
    EVT_MENU( ID_LOADFROMFILE, FormMain::OnLoadFromFileClick )

    EVT_MENU( ID_CHANGEFLAGSITEMS, FormMain::OnChangeFlagsPropItemsClick )

    EVT_MENU( ID_RUNTESTFULL, FormMain::OnMisc )
    EVT_MENU( ID_RUNTESTPARTIAL, FormMain::OnMisc )

    EVT_MENU( ID_TESTINSERTCHOICE, FormMain::OnInsertChoice )
    EVT_MENU( ID_TESTDELETECHOICE, FormMain::OnDeleteChoice )

    EVT_MENU( ID_INSERTPAGE, FormMain::OnInsertPage )
    EVT_MENU( ID_REMOVEPAGE, FormMain::OnRemovePage )

    EVT_MENU( ID_SAVESTATE, FormMain::OnSaveState )
    EVT_MENU( ID_LOADSTATE, FormMain::OnLoadState )

    EVT_MENU( ID_SETSPINCTRLEDITOR, FormMain::OnSetSpinCtrlEditorClick )
    EVT_MENU( ID_TESTREPLACE, FormMain::OnTestReplaceClick )
    EVT_MENU( ID_SETPROPERTYVALUE, FormMain::OnSetPropertyValue )

    EVT_CONTEXT_MENU( FormMain::OnContextMenu )

    // propgrid may be destroyed at close event, so we won't be testing CanClose right now.
    //EVT_CLOSE( FormMain::OnCloseEvent )
END_EVENT_TABLE()

// -----------------------------------------------------------------------

void FormMain::OnMove( wxMoveEvent& event )
{
    if ( !m_pPropGridMan )
    {
        // this check is here so the frame layout can be tested
        // without creating propertygrid
        event.Skip();
        return;
    }

    // Update position properties
    int x, y;
    GetPosition(&x,&y);

    wxPGId id;

#if TESTING_WXPROPERTYGRIDADV

    // Must check if they exist (as they may be deleted).

    // Using m_pPropGridMan, we can scan all pages automatically.
    id = m_pPropGridMan->GetPropertyByName( wxT("X") );
    if ( wxPGIdIsOk(id) )
        m_pPropGridMan->SetPropertyValue( id, x );

    id = m_pPropGridMan->GetPropertyByName( wxT("Y") );
    if ( wxPGIdIsOk(id) )
        m_pPropGridMan->SetPropertyValue( id, y );

    id = m_pPropGridMan->GetPropertyByName( wxT("Position") );
    if ( wxPGIdIsOk(id) )
        m_pPropGridMan->SetPropertyValue( id, wxPoint(x,y) );

#else

    // Since grid may on another page, we must check that the names exist.
    id = m_propGrid->GetPropertyByName( wxT("X") );
    if ( wxPGIdIsOk(id) )
        m_propGrid->SetPropertyValue( id, x );

    id = m_propGrid->GetPropertyByName( wxT("Y") );
    if ( wxPGIdIsOk(id) )
        m_propGrid->SetPropertyValue( id, y );

    id = m_propGrid->GetPropertyByName( wxT("Position") );
    if ( wxPGIdIsOk(id) )
        m_propGrid->SetPropertyValue( id, wxPoint(x,y) );

#endif

    // Should always call event.Skip() in frame's MoveEvent handler
    event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnResize( wxSizeEvent& event )
{
    if ( !m_pPropGridMan )
    {
        // this check is here so the frame layout can be tested
        // without creating propertygrid
        event.Skip();
        return;
    }

    // Update size properties
    int w, h;
    GetSize(&w,&h);

    wxPGId id;

#if TESTING_WXPROPERTYGRIDADV

    // Must check if they exist (as they may be deleted).

    // Using m_pPropGridMan, we can scan all pages automatically.
    id = m_pPropGridMan->GetPropertyByName ( wxT("Width") );
    if ( wxPGIdIsOk(id) )
        m_pPropGridMan->SetPropertyValue( id, w );

    id = m_pPropGridMan->GetPropertyByName ( wxT("Height") );
    if ( wxPGIdIsOk(id) )
        m_pPropGridMan->SetPropertyValue( id, h );

    id = m_pPropGridMan->GetPropertyByName ( wxT("Size") );
    if ( wxPGIdIsOk(id) )
        m_pPropGridMan->SetPropertyValue( id, wxSize(w,h) );

#else

    // Since grid may on another page, we must check that the names exist.
    id = m_propGrid->GetPropertyByName ( wxT("Width") );
    if ( wxPGIdIsOk(id) )
        m_propGrid->SetPropertyValue( id, w );
    id = m_propGrid->GetPropertyByName ( wxT("Height") );
    if ( wxPGIdIsOk(id) )
        m_propGrid->SetPropertyValue( id, h );

    id = m_propGrid->GetPropertyByName ( wxT("Size") );
    if ( wxPGIdIsOk(id) )
        m_propGrid->SetPropertyValue( id, wxSize(w,h) );

#endif

    // Should always call event.Skip() in frame's SizeEvent handler
    event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnPaint( wxPaintEvent& )
{
    wxPaintDC(this);
}

// -----------------------------------------------------------------------

void TestPaintCallback(wxPGProperty* WXUNUSED(property), wxDC& dc,
                       const wxRect& rect, wxPGPaintData& paintdata);

void FormMain::OnPropertyGridChanging( wxPropertyGridEvent& event )
{
    wxPGProperty* p = event.GetProperty();

    if ( p->GetName() == wxT("Font") )
    {
        int res =
        wxMessageBox(wxString::Format(wxT("'%s' is about to change (to variant of type '%s')\n\nAllow or deny?"),
                                      p->GetName().c_str(),event.GetValue().GetType().c_str()),
                     wxT("Testing wxEVT_PG_CHANGING"), wxYES_NO, m_pPropGridMan);

        if ( res == wxNO )
        {
            wxASSERT(event.CanVeto());
            event.Veto();
        }
    }
}
//
// Note how we use three types of value getting in this method:
//   A) event.GetPropertyValueAsXXX
//   B) event.GetPropertValue, and then variant's GetXXX
//   C) grid's GetPropertyValueAsXXX(id)
//
void FormMain::OnPropertyGridChange( wxPropertyGridEvent& event )
{
    wxPGProperty* id = event.GetProperty();

    const wxString& name = event.GetPropertyName();
    wxVariant value = event.GetPropertyValue();

    // Don't handle 'unspecified' values
    if ( value.IsNull() )
        return;

    //wxLogDebug( wxT("Changed value of '%s' to '%s'"),
    //            name.c_str(),
    //            event.GetPropertyValueAsString().c_str() );

    wxPGProperty* cpId = m_pPropGridMan->GetPropertyByName(wxT("CustomProperty"));

    //
    // Must first check ifs a child of CustomProperty
    // (since it also has sub-item named Label, which is also a top-level item).
    //
    if ( event.GetMainParent() == cpId )
    {
        wxString subName = id->GetBaseName();

        // Parse CustomProperty manipulation

        if ( subName == wxT("Label") )
        {
            // Changing label
            m_pPropGridMan->SetPropertyLabel( cpId, event.GetPropertyValueAsString() );
        }
        else if ( subName == wxT("Image") )
        {
            // Changing OnCustomPaint image
            wxImage image;
            wxString path = event.GetPropertyValueAsString();

            if ( ::wxFileExists(path) )
                image.LoadFile(path);

            if ( image.Ok() )
            {
                wxBitmap bmp(image);
                m_pPropGridMan->SetPropertyImage( cpId, bmp );
            }
            else
            {
                m_pPropGridMan->SetPropertyImage( cpId, wxNullBitmap );
            }
            Refresh();
        }
        else if ( subName == wxT("Editor") )
        {
            // Changing editor class
            int index = event.GetPropertyValueAsInt();

            wxPGEditor* editor = wxPGEditor_TextCtrl;
            switch( index )
            {
                case 0:
                    editor = wxPGEditor_TextCtrl;
                    break;
                case 1:
                    editor = wxPGEditor_Choice;
                    break;
                case 2:
                    editor = wxPGEditor_ComboBox;
                    break;
                case 3:
                    editor = wxPGEditor_TextCtrlAndButton;
                    break;
                case 4:
                    editor = wxPGEditor_ChoiceAndButton;
                    break;
            }

            m_pPropGridMan->SetPropertyEditor ( cpId, editor );

            wxPGId propChoices = m_pPropGridMan->GetPropertyByName(wxT("CustomProperty.Choices"));
            wxASSERT(wxPGIdIsOk(propChoices));

            // Toggle disabled-state of choices property
            if ( index == 1 || index == 2 || index == 4 )
            {
                m_pPropGridMan->EnableProperty(propChoices,true);

                wxPGChoices choices(m_pPropGridMan->
                    GetPropertyValueAsArrayString(propChoices));

                // Also set the current choices now
                m_pPropGridMan->SetPropertyChoices( cpId, choices );
            }
            else
            {
                m_pPropGridMan->EnableProperty(propChoices,false);
            }

        }
        else if ( subName == wxT("Choices") )
        {
            wxPGChoices choices(m_pPropGridMan->
                GetPropertyValueAsArrayString(wxT("CustomProperty.Choices")));

            // Changing list of choices for
            m_pPropGridMan->SetPropertyChoices( cpId, choices );
        }
        else if ( subName == wxT("Paint Mode") )
        {
            // Test either callback or bitmap
            int ind = event.GetPropertyValueAsInt();
            void* callback = NULL;
            if ( ind )
                callback = (void*) TestPaintCallback;

            m_pPropGridMan->SetPropertyAttribute(cpId,
                                                 wxPG_CUSTOM_PAINT_CALLBACK,
                                                 callback);
        }
    }
    else
    // Some settings are disabled outside Windows platform
    if ( name == wxT("X") )
        SetSize ( m_pPropGridMan->GetPropertyValueAsInt(id), -1, -1, -1, wxSIZE_USE_EXISTING );
    else if ( name == wxT("Y") )
    // wxPGVariantToInt is safe long int value getter
        SetSize ( -1, wxPGVariantToInt(value), -1, -1, wxSIZE_USE_EXISTING );
    else if ( name == wxT("Width") )
        SetSize ( -1, -1, event.GetPropertyValueAsInt(), -1, wxSIZE_USE_EXISTING );
    else if ( name == wxT("Height") )
        SetSize ( -1, -1, -1, wxPGVariantToInt(value), wxSIZE_USE_EXISTING );
    //else if ( name == wxT("Position") )
    //    SetPosition ( event.GetPropertyValueAsPoint() );
    //else if ( name == wxT("Size") )
    //    SetSize ( event.GetPropertyValueAsSize() );
    else if ( name == wxT("Label") )
    {
        SetTitle ( m_pPropGridMan->GetPropertyValueAsString(id) );
    }
    else if ( name == wxT("Password") )
    {
        static int pwdMode = 0;

        //m_pPropGridMan->SetPropertyAttribute(id, wxPG_STRING_PASSWORD, (long)pwdMode);

        pwdMode++;
        pwdMode &= 1;
    }
    else
    if ( name == wxT("Font") )
    {
        wxFont& font = *wxDynamicCast(event.GetPropertyValueAsWxObjectPtr(), wxFont);
        //wxFont& font = *((wxFont*)event.GetPropertyValueAsWxObjectPtr());
        wxASSERT( &font && font.Ok() );

        m_pPropGridMan->SetFont( font );
    }
    else
    if ( name == wxT("Margin Colour") )
    {
        wxColourPropertyValue& cpv = *wxGetVariantCast(value,wxColourPropertyValue);
        m_pPropGridMan->GetGrid()->SetMarginColour ( cpv.m_colour );

        // Test wxGetVariantCast for normal variant (causes memory leak?)
        //wxVariant black_col(wxBLACK);
        //SetBackgroundColour ( *wxGetVariantCast(black_col,wxColour) );
        //m_pPropGridMan->GetGrid()->SetBackgroundColour ( *wxGetVariantCast(black_col,wxColour) );
    }
    else if ( name == wxT("Cell Colour") )
    {
        wxColourPropertyValue* cpv = wxGetVariantCast(value,wxColourPropertyValue);
        wxASSERT( cpv );
        m_pPropGridMan->GetGrid()->SetCellBackgroundColour( cpv->m_colour );
    }
    else if ( name == wxT("Line Colour") )
    {
        wxColourPropertyValue* cpv = wxGetVariantCast(value,wxColourPropertyValue);
        wxASSERT( cpv );
        m_pPropGridMan->GetGrid()->SetLineColour( cpv->m_colour );
    }
    else if ( name == wxT("Cell Text Colour") )
    {
        wxColourPropertyValue* cpv = wxGetVariantCast(value,wxColourPropertyValue);
        wxASSERT( cpv );
        m_pPropGridMan->GetGrid()->SetCellTextColour( cpv->m_colour );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridSelect( wxPropertyGridEvent& event )
{
    wxPGId id = event.GetProperty();
    if ( wxPGIdIsOk(id) )
    {
        m_itemEnable->Enable( TRUE );
        if ( event.IsPropertyEnabled() )
            m_itemEnable->SetText( wxT("Disable") );
        else
            m_itemEnable->SetText( wxT("Enable") );
    }
    else
    {
        m_itemEnable->Enable( FALSE );
    }

#if wxUSE_STATUSBAR && TESTING_WXPROPERTYGRIDADV
    wxPGId prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( wxPGIdIsOk(prop) )
    {
        wxString text(wxT("Selected: "));
        text += m_pPropGridMan->GetPropertyLabel( prop );
        sb->SetStatusText ( text );
    }
#endif

}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridPageChange( wxPropertyGridEvent& WXUNUSED(event) )
{
#if wxUSE_STATUSBAR && TESTING_WXPROPERTYGRIDADV
    wxStatusBar* sb = GetStatusBar();
    wxString text(wxT("Page Changed: "));
    text += m_pPropGridMan->GetPageName(m_pPropGridMan->GetSelectedPage());
    sb->SetStatusText( text );
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridHighlight( wxPropertyGridEvent& /*event*/ )
{
/*#if wxUSE_STATUSBAR
    wxPGId prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( wxPGIdIsOk(prop) )
    {
        wxString text(wxT("Highlighted: "));
        text += m_pPropGridMan->GetPropertyLabel( prop );
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxEmptyString );
    }
#endif*/
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemRightClick( wxPropertyGridEvent& event )
{
#if wxUSE_STATUSBAR
    wxPGId prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( wxPGIdIsOk(prop) )
    {
        wxString text(wxT("Right-clicked: "));
        text += event.GetPropertyLabel();
        text += wxT(", name=");
        text += m_pPropGridMan->GetPropertyName(prop);
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxEmptyString );
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemDoubleClick( wxPropertyGridEvent& event )
{
#if wxUSE_STATUSBAR
    wxPGId prop = event.GetProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( wxPGIdIsOk(prop) )
    {
        wxString text(wxT("Double-clicked: "));
        text += event.GetPropertyLabel();
        text += wxT(", name=");
        text += m_pPropGridMan->GetPropertyName(prop);
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxEmptyString );
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridButtonClick ( wxCommandEvent& )
{
#if wxUSE_STATUSBAR
    wxPGId prop = m_pPropGridMan->GetSelectedProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( wxPGIdIsOk(prop) )
    {
        wxString text(wxT("Button clicked: "));
        text += m_pPropGridMan->GetPropertyLabel(prop);
        text += wxT(", name=");
        text += m_pPropGridMan->GetPropertyName(prop);
        sb->SetStatusText ( text );
    }
    else
    {
        sb->SetStatusText ( wxT("SHOULD NOT HAPPEN!!!") );
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemCollapse( wxPropertyGridEvent& )
{
    wxLogDebug(wxT("Item was Collapsed"));
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridItemExpand( wxPropertyGridEvent& )
{
    wxLogDebug(wxT("Item was Expanded"));
}

// -----------------------------------------------------------------------

// EVT_TEXT handling
void FormMain::OnPropertyGridTextUpdate( wxCommandEvent& event )
{
    //wxLogDebug(wxT("FormMain::OnPropertyGridTextUpdate(text=\"%s\")"),event.GetString().c_str());
    event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnPropertyGridKeyEvent( wxKeyEvent& /*event*/ )
{
    // Occurs on wxGTK mostly, but not wxMSW.

    //wxLogDebug(wxT("FormMain::OnPropertyGridKeyEvent()"));
/*#if wxUSE_STATUSBAR
    wxPGId prop = m_pPropGridMan->GetSelectedProperty();
    wxStatusBar* sb = GetStatusBar();
    if ( wxPGIdIsOk(prop) )
    {
        sb->SetStatusText( wxString::Format(wxT("Key event: %i"),(int)event.GetKeyCode()) );
    }
    else
    {
        sb->SetStatusText( wxT("SHOULD NOT HAPPEN!!!") );
    }
#endif*/
}

// -----------------------------------------------------------------------

void FormMain::OnLabelTextChange( wxCommandEvent& WXUNUSED(event) )
{
// Uncomment following to allow property label modify in real-time
//    wxPGProperty& p = m_pPropGridMan->GetGrid()->GetSelection();
//    if ( !p.IsOk() ) return;
//    m_pPropGridMan->SetPropertyLabel( p, m_tcPropLabel->DoGetValue() );
}

// -----------------------------------------------------------------------

//
// Normally, wxPropertyGrid does not check whether item with identical
// label already exists. However, since in this sample we use labels for
// identifying properties, we have to be sure not to generate identical
// labels.
//
#if TESTING_WXPROPERTYGRIDADV
void GenerateUniquePropertyLabel( wxPropertyGridManager* pg, wxString& baselabel )
#else
void GenerateUniquePropertyLabel( wxPropertyGrid* pg, wxString& baselabel )
#endif
{
    int count = -1;
    wxString newlabel;

    if ( pg->GetPropertyByLabel( baselabel ) )
    {
        for (;;)
        {
            count++;
            newlabel.Printf(wxT("%s%i"),baselabel.c_str(),count);
            if ( !pg->GetPropertyByLabel( newlabel ) ) break;
        }
    }

    if ( count >= 0 )
    {
        baselabel = newlabel;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnInsertPropClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString propLabel;

    if ( !m_pPropGridMan->GetChildrenCount() )
    {
        wxMessageBox(wxT("No items to relate - first add some with Append."));
        return;
    }

    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !wxPGIdIsOk(id) )
    {
        wxMessageBox(wxT("First select a property - new one will be inserted right before that."));
        return;
    }
    if ( propLabel.Len() < 1 ) propLabel = wxT("Property");

    GenerateUniquePropertyLabel( m_pPropGridMan, propLabel );

    m_pPropGridMan->Insert( m_pPropGridMan->GetPropertyParent(id),
                            m_pPropGridMan->GetPropertyIndex(id),
                            new wxStringProperty(propLabel) );

}

// -----------------------------------------------------------------------

void FormMain::OnAppendPropClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString propLabel;

    //propLabel = m_tcPropLabel->GetValue();
    if ( propLabel.Len() < 1 ) propLabel = wxT("Property");

    GenerateUniquePropertyLabel( m_pPropGridMan, propLabel );

    m_pPropGridMan->Append( new wxStringProperty(propLabel) );

    m_pPropGridMan->Refresh();
}

// -----------------------------------------------------------------------

void FormMain::OnClearClick( wxCommandEvent& WXUNUSED(event) )
{
    m_pPropGridMan->GetGrid()->Clear();
}

// -----------------------------------------------------------------------

void FormMain::OnAppendCatClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString propLabel;

    if ( propLabel.Len() < 1 ) propLabel = wxT("Category");

    GenerateUniquePropertyLabel( m_pPropGridMan, propLabel );

    m_pPropGridMan->Append( new wxPropertyCategory (propLabel) );

    m_pPropGridMan->Refresh();

}

// -----------------------------------------------------------------------

void FormMain::OnInsertCatClick( wxCommandEvent& WXUNUSED(event) )
{
    wxString propLabel;

    if ( !m_pPropGridMan->GetChildrenCount() )
    {
        wxMessageBox(wxT("No items to relate - first add some with Append."));
        return;
    }

    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !wxPGIdIsOk(id) )
    {
        wxMessageBox(wxT("First select a property - new one will be inserted right before that."));
        return;
    }

    if ( propLabel.Len() < 1 ) propLabel = wxT("Category");

    GenerateUniquePropertyLabel( m_pPropGridMan, propLabel );

    m_pPropGridMan->Insert( m_pPropGridMan->GetPropertyParent(id),
                            m_pPropGridMan->GetPropertyIndex(id),
                            new wxPropertyCategory (propLabel) );

}

// -----------------------------------------------------------------------

void FormMain::OnDelPropClick( wxCommandEvent& WXUNUSED(event) )
{
    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !wxPGIdIsOk(id) )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }
    /*
    else if ( m_propGrid->IsSubProperty(id) )
    {
        wxMessageBox(wxT("Do not try to delete sub-properties."));
        return;
    }
    */

    m_pPropGridMan->DeleteProperty( id );

}

// -----------------------------------------------------------------------

void FormMain::OnDelPropRClick( wxCommandEvent& WXUNUSED(event) )
{
    // Delete random property
    wxPGProperty* p = m_pPropGridMan->GetGrid()->GetRoot();

    for (;;)
    {
        if ( !p->IsCategory() )
        {
            m_pPropGridMan->DeleteProperty( wxPGIdGen(p) );
            break;
        }

        if ( !p->GetChildCount() )
            break;

        int n = rand() % ((int)p->GetChildCount());

        p = p->Item(n);
    }
}

// -----------------------------------------------------------------------

void FormMain::OnContextMenu( wxContextMenuEvent& event )
{
    wxLogDebug(wxT("FormMain::OnContextMenu(%i,%i)"),
        event.GetPosition().x,event.GetPosition().y);

    //event.Skip();
}

// -----------------------------------------------------------------------

void FormMain::OnCloseClick( wxCommandEvent& WXUNUSED(event) )
{
/*#ifdef __WXDEBUG__
    m_pPropGridMan->GetGrid()->DumpAllocatedChoiceSets();
    wxLogDebug(wxT("\\-> Don't worry, this is perfectly normal in this sample."));
#endif*/

    Close(false);
}

// -----------------------------------------------------------------------

int IterateMessage( wxPGProperty* prop )
{
    wxString s;

    s.Printf( wxT("\"%s\" class = %s, valuetype = %s"), prop->GetLabel().c_str(),
        prop->GetClassName(), prop->GetType().c_str() );

    return wxMessageBox( s, wxT("Iterating... (press CANCEL to end)"), wxOK|wxCANCEL );
}

// -----------------------------------------------------------------------

void FormMain::OnIterate1Click( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGridIterator it;

    for ( it = m_pPropGridMan->GetCurrentPage()->
            GetIterator();
          !it.AtEnd();
          it++ )
    {
        wxPGProperty* p = *it;
        int res = IterateMessage( p );
        if ( res == wxCANCEL ) break;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnIterate2Click( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGridIterator it;

    for ( it = m_pPropGridMan->GetCurrentPage()->
            GetIterator( wxPG_ITERATE_VISIBLE );
          !it.AtEnd();
          it++ )
    {
        wxPGProperty* p = *it;

        int res = IterateMessage( p );
        if ( res == wxCANCEL ) break;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnIterate3Click( wxCommandEvent& WXUNUSED(event) )
{
    // iterate over items in reverse order
    wxPropertyGridIterator it;

    for ( it = m_pPropGridMan->GetCurrentPage()->
                GetIterator( wxPG_ITERATE_DEFAULT, wxBOTTOM );
          !it.AtEnd();
          it-- )
    {
        wxPGProperty* p = *it;

        int res = IterateMessage( p );
        if ( res == wxCANCEL ) break;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnIterate4Click( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGridIterator it;

    for ( it = m_pPropGridMan->GetCurrentPage()->
            GetIterator( wxPG_ITERATE_CATEGORIES );
          !it.AtEnd();
          it++ )
    {
        wxPGProperty* p = *it;

        int res = IterateMessage( p );
        if ( res == wxCANCEL ) break;
    }
}

// -----------------------------------------------------------------------

void FormMain::OnChangeFlagsPropItemsClick( wxCommandEvent& WXUNUSED(event) )
{

    wxPGId id = m_pPropGridMan->GetPropertyByName(wxT("Window Styles"));

    wxPGChoices newChoices;

    newChoices.Add(wxT("Fast"),0x1);
    newChoices.Add(wxT("Powerful"),0x2);
    newChoices.Add(wxT("Safe"),0x4);
    newChoices.Add(wxT("Sleek"),0x8);

    m_pPropGridMan->SetPropertyChoices(id,newChoices);
    //m_pPropGridMan->ReplaceProperty(wxT("Window Styles"),
    //    wxFlagsProperty(wxT("Window Styles"),wxPG_LABEL,newChoices));
}

// -----------------------------------------------------------------------

void FormMain::OnEnableDisable( wxCommandEvent& )
{
    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !wxPGIdIsOk(id) )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }
    /*
    else if ( m_propGrid->IsSubProperty(id) )
    {
        wxMessageBox(wxT("Do not try to enable/disable sub-properties."));
        return;
    }
    */

    if ( m_pPropGridMan->IsPropertyEnabled( id ) )
    {
        m_pPropGridMan->DisableProperty ( id );
        m_itemEnable->SetText( wxT("Enable") );
    }
    else
    {
        m_pPropGridMan->EnableProperty ( id );
        m_itemEnable->SetText( wxT("Disable") );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnHideShow( wxCommandEvent& WXUNUSED(event) )
{
    wxPGId id = m_pPropGridMan->GetGrid()->GetSelection();
    if ( !wxPGIdIsOk(id) )
    {
        wxMessageBox(wxT("First select a property."));
        return;
    }
    /*
    else if ( m_propGrid->IsSubProperty(id) )
    {
        wxMessageBox(wxT("Do not try to enable/disable sub-properties."));
        return;
    }
    */

    if ( m_pPropGridMan->IsPropertyShown( id ) )
    {
        m_pPropGridMan->HideProperty( id, true );
        m_itemEnable->SetText( wxT("Show") );
    }
    else
    {
        m_pPropGridMan->HideProperty( id, false );
        m_itemEnable->SetText( wxT("Hide") );
    }

    wxPropertyGridPage* curPage = m_pPropGridMan->GetCurrentPage();

    // Check for bottomY precalculation validity
    unsigned int byPre = curPage->GetVirtualHeight();
    unsigned int byAct = curPage->GetActualVirtualHeight();

    if ( byPre != byAct )
    {
        wxLogDebug(wxT("VirtualHeight is %u, should be %u"), byPre, byAct);
    }
}

// -----------------------------------------------------------------------

void FormMain::OnInsertPage( wxCommandEvent& WXUNUSED(event) )
{
#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->AddPage(wxT("New Page"));
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnRemovePage( wxCommandEvent& WXUNUSED(event) )
{
#if TESTING_WXPROPERTYGRIDADV
    m_pPropGridMan->RemovePage(m_pPropGridMan->GetSelectedPage());
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnSaveState( wxCommandEvent& WXUNUSED(event) )
{
#if TESTING_WXPROPERTYGRIDADV
    wxPGMEditableState state(m_pPropGridMan);
    m_savedState = state.GetAsString();
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnLoadState( wxCommandEvent& WXUNUSED(event) )
{
#if TESTING_WXPROPERTYGRIDADV
    wxPGMEditableState state;
    bool res = state.SetFromString(m_savedState);
    wxASSERT(res);
    m_pPropGridMan->RestoreEditableState(state);
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnSetSpinCtrlEditorClick( wxCommandEvent& WXUNUSED(event) )
{
#if wxUSE_SPINBTN
    wxPGId pgId = m_pPropGridMan->GetSelectedProperty();
    if ( wxPGIdIsOk(pgId) )
        m_pPropGridMan->SetPropertyEditor( pgId, wxPG_EDITOR(SpinCtrl) );
    else
        wxMessageBox(wxT("First select a property"));
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnTestReplaceClick( wxCommandEvent& WXUNUSED(event) )
{
    wxPGId pgId = m_pPropGridMan->GetSelectedProperty();
    if ( wxPGIdIsOk(pgId) )
    {
        wxPGChoices choices;
        choices.Add(wxT("Flag 0"),0x0001);
        choices.Add(wxT("Flag 1"),0x0002);
        choices.Add(wxT("Flag 2"),0x0004);
        choices.Add(wxT("Flag 3"),0x0008);
        wxPGId newId = m_pPropGridMan->ReplaceProperty( pgId,
            new wxFlagsProperty(wxT("ReplaceFlagsProperty"), wxPG_LABEL, choices, 0x0003) );
        m_pPropGridMan->SetPropertyAttribute( newId,
                                              wxPG_BOOL_USE_CHECKBOX,
                                              true,
                                              wxPG_RECURSE );
    }
    else
        wxMessageBox(wxT("First select a property"));
}

// -----------------------------------------------------------------------

void FormMain::OnClearModifyStatusClick( wxCommandEvent& WXUNUSED(event) )
{
    m_pPropGridMan->ClearModifiedStatus();
}

// -----------------------------------------------------------------------

// Freeze check-box checked?
void FormMain::OnFreezeClick( wxCommandEvent& event )
{
    if ( !m_pPropGridMan ) return;

    if ( event.IsChecked() )
    {
        if ( !m_pPropGridMan->IsFrozen() )
        {
            m_pPropGridMan->Freeze();
        }
    }
    else
    {
        if ( m_pPropGridMan->IsFrozen() )
        {
            m_pPropGridMan->Thaw();
            m_pPropGridMan->Refresh();
        }
    }
}

// -----------------------------------------------------------------------

void FormMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( wxT("wxPropertyGrid %i.%i.%i Sample")
#if wxUSE_UNICODE
  #if defined(wxUSE_UNICODE_UTF8) && wxUSE_UNICODE_UTF8
                wxT(" <utf-8>")
  #else
                wxT(" <unicode>")
  #endif
#else
                wxT(" <ansi>")
#endif
#ifdef __WXDEBUG__
                wxT(" <debug>")
#else
                wxT(" <release>")
#endif
                wxT("\n\n")
                wxT("Programmed by %s ( %s ).\n\n")
                wxT("Powered by %s.\n\n"),
            wxPROPGRID_MAJOR, wxPROPGRID_MINOR, wxPROPGRID_RELEASE,
            wxT("Jaakko Salli"), wxT("jmsalli79@hotmail.com, jaakko.salli@pp.inet.fi"), wxVERSION_STRING
            );

    wxMessageBox(msg, _T("About"), wxOK | wxICON_INFORMATION, this);
}

// -----------------------------------------------------------------------

#ifdef wxPGS_USE_TABCTRL
void FormMain::OnTabChange( wxTabEvent& WXUNUSED(event) )
{
    int sel = m_tabctrl->GetSelection();
#else
void FormMain::OnRadioBoxChange( wxCommandEvent& WXUNUSED(event) )
{
    int sel = m_radiobox->GetSelection();
#endif

    if ( !m_propGrid )
        return;

    if ( sel == 0 )
        m_propGrid->EnableCategories( TRUE );
    else
        m_propGrid->EnableCategories( FALSE );

    if ( !m_propGrid->IsFrozen() )
        m_propGrid->Refresh();
}

// -----------------------------------------------------------------------

void FormMain::OnColourScheme( wxCommandEvent& event )
{
    //m_pPropGridMan->SetPropertyValue(wxT("ColourProperty"),*wxBLACK);

    int id = event.GetId();
    if ( id == ID_COLOURSCHEME1 )
    {
        m_pPropGridMan->GetGrid()->ResetColours();
    }
    else if ( id == ID_COLOURSCHEME2 )
    {
        // white
        wxColour my_grey_1(212,208,200);
        wxColour my_grey_3(113,111,100);
        m_pPropGridMan->Freeze();
        m_pPropGridMan->GetGrid()->SetMarginColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCaptionBackgroundColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCellBackgroundColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCellTextColour( my_grey_3 );
        m_pPropGridMan->GetGrid()->SetLineColour( my_grey_1 ); //wxColour(160,160,160)
        m_pPropGridMan->Thaw();
    }
    else if ( id == ID_COLOURSCHEME3 )
    {
        // .NET
        wxColour my_grey_1(212,208,200);
        wxColour my_grey_2(236,233,216);
        m_pPropGridMan->Freeze();
        m_pPropGridMan->GetGrid()->SetMarginColour( my_grey_1 );
        m_pPropGridMan->GetGrid()->SetCaptionBackgroundColour( my_grey_1 );
        m_pPropGridMan->GetGrid()->SetLineColour( my_grey_1 );
        m_pPropGridMan->Thaw();
    }
    else if ( id == ID_COLOURSCHEME4 )
    {
        // cream

        wxColour my_grey_1(212,208,200);
        wxColour my_grey_2(241,239,226);
        wxColour my_grey_3(113,111,100);
        m_pPropGridMan->Freeze();
        m_pPropGridMan->GetGrid()->SetMarginColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCaptionBackgroundColour( *wxWHITE );
        m_pPropGridMan->GetGrid()->SetCellBackgroundColour( my_grey_2 );
        m_pPropGridMan->GetGrid()->SetCellBackgroundColour( my_grey_2 );
        m_pPropGridMan->GetGrid()->SetCellTextColour( my_grey_3 );
        m_pPropGridMan->GetGrid()->SetLineColour( my_grey_1 );
        m_pPropGridMan->Thaw();
    }
}

// -----------------------------------------------------------------------

void FormMain::OnCatColours( wxCommandEvent& event )
{
    m_pPropGridMan->Freeze();
    if ( event.IsChecked() )
    {
        // Set custom colours.
        m_pPropGridMan->SetCaptionTextColour( wxT("Appearance"), wxColour(255,0,0) );
        m_pPropGridMan->SetPropertyBackgroundColour( wxT("Appearance"), wxColour(255,255,183) );
        m_pPropGridMan->SetPropertyTextColour( wxT("Appearance"), wxColour(255,0,183) );
        m_pPropGridMan->SetCaptionTextColour( wxT("PositionCategory"), wxColour(0,255,0) );
        m_pPropGridMan->SetPropertyBackgroundColour( wxT("PositionCategory"), wxColour(255,226,190) );
        m_pPropGridMan->SetPropertyTextColour( wxT("PositionCategory"), wxColour(255,0,190) );
        m_pPropGridMan->SetCaptionTextColour( wxT("Environment"), wxColour(0,0,255) );
        m_pPropGridMan->SetPropertyBackgroundColour( wxT("Environment"), wxColour(208,240,175) );
        m_pPropGridMan->SetPropertyTextColour( wxT("Environment"), wxColour(255,255,255) );
        m_pPropGridMan->SetPropertyBackgroundColour( wxT("More Examples"), wxColour(172,237,255) );
        m_pPropGridMan->SetPropertyTextColour( wxT("More Examples"), wxColour(172,0,255) );
    }
    else
    {
        // Revert to original.
        m_pPropGridMan->SetPropertyColourToDefault( wxT("Appearance") );
        m_pPropGridMan->SetPropertyColourToDefault( wxT("PositionCategory") );
        m_pPropGridMan->SetPropertyColourToDefault( wxT("Environment") );
        m_pPropGridMan->SetPropertyColourToDefault( wxT("More Examples") );
    }
    m_pPropGridMan->Thaw();
    m_pPropGridMan->Refresh();
}

// -----------------------------------------------------------------------

#define ADD_FLAG(FLAG) \
        chs.Add(wxT(#FLAG)); \
        vls.Add(FLAG); \
        if ( (flags & FLAG) == FLAG ) sel.Add(ind); \
        ind++;

void FormMain::OnSelectStyle( wxCommandEvent& WXUNUSED(event) )
{
    int style;
    int extraStyle;

    {
        wxArrayString chs;
        wxArrayInt vls;
        wxArrayInt sel;
        unsigned int ind = 0;
        int flags = m_pPropGridMan->GetWindowStyle();
        ADD_FLAG(wxPG_HIDE_CATEGORIES)
        ADD_FLAG(wxPG_AUTO_SORT)
        ADD_FLAG(wxPG_BOLD_MODIFIED)
        ADD_FLAG(wxPG_SPLITTER_AUTO_CENTER)
        ADD_FLAG(wxPG_TOOLTIPS)
        ADD_FLAG(wxPG_STATIC_SPLITTER)
        ADD_FLAG(wxPG_HIDE_MARGIN)
        ADD_FLAG(wxPG_LIMITED_EDITING)
        ADD_FLAG(wxPG_TOOLBAR)
        ADD_FLAG(wxPG_DESCRIPTION)
        wxMultiChoiceDialog dlg( this, wxT("Select window styles to use"),
                                 wxT("wxPropertyGrid Window Style"), chs );
        dlg.SetSelections(sel);
        if ( dlg.ShowModal() == wxID_CANCEL )
            return;

        flags = 0;
        sel = dlg.GetSelections();
        for ( ind = 0; ind < sel.size(); ind++ )
            flags |= vls[sel[ind]];

        style = flags;
    }

    {
        wxArrayString chs;
        wxArrayInt vls;
        wxArrayInt sel;
        unsigned int ind = 0;
        int flags = m_pPropGridMan->GetExtraStyle();
        ADD_FLAG(wxPG_EX_INIT_NOCAT)
        ADD_FLAG(wxPG_EX_NO_FLAT_TOOLBAR)
        ADD_FLAG(wxPG_EX_MODE_BUTTONS)
        ADD_FLAG(wxPG_EX_HELP_AS_TOOLTIPS)
        ADD_FLAG(wxPG_EX_NATIVE_DOUBLE_BUFFERING)
        ADD_FLAG(wxPG_EX_AUTO_UNSPECIFIED_VALUES)
        ADD_FLAG(wxPG_EX_WRITEONLY_BUILTIN_ATTRIBUTES)
        ADD_FLAG(wxPG_EX_LEGACY_VALIDATORS)
        wxMultiChoiceDialog dlg( this, wxT("Select extra window styles to use"),
                                 wxT("wxPropertyGrid Extra Style"), chs );
        dlg.SetSelections(sel);
        if ( dlg.ShowModal() == wxID_CANCEL )
            return;

        flags = 0;
        sel = dlg.GetSelections();
        for ( ind = 0; ind < sel.size(); ind++ )
            flags |= vls[sel[ind]];

        extraStyle = flags;
    }

    CreateGrid( style, extraStyle );

    SetSize(
        (wxSystemSettings::GetMetric(wxSYS_SCREEN_X)/10)*4,
        (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)/10)*8
        );
    Centre();
}

// -----------------------------------------------------------------------

void FormMain::OnSetColumns( wxCommandEvent& WXUNUSED(event) )
{
    long colCount = ::wxGetNumberFromUser(wxT("Enter number of columns (2-20)."),wxT("Columns:"),
                                          wxT("Change Columns"),m_pPropGridMan->GetColumnCount(),
                                          2,20);

    if ( colCount >= 2 )
    {
        m_pPropGridMan->SetColumnCount(colCount);
    }
}

// -----------------------------------------------------------------------

#include <wx/textfile.h>

static void WritePropertiesToFile( wxPropertyGrid* pg, wxPGProperty* parent, wxTextFile& f, int depth, bool useClassName )
{
    wxString s;
    wxString s2;
    wxString s_value;
    wxString s_attribs;
    unsigned int i;

    for ( i=0; i<parent->GetChildCount(); i++ )
    {
        wxPGProperty* p = parent->Item(i);

        // Write property info ( as classname,label,value)
        // Note how we omit the name as it is convenient
        // to assume that it matches the label.
        s_value = pg->GetPropertyValueAsString(p);

        // Since we surround tokens by '"'s, we need to
        // replace '"' with '\"' and '\'s with '\\'
        s_value.Replace(wxT("\\"),wxT("\\\\"));
        s_value.Replace(wxT("\""),wxT("\\\""));

        wxString classname;

        // If we categorize using class name, then just get
        // property's short class name.
        if ( useClassName )
            classname = pg->GetPropertyShortClassName(p);
        else
        // If we categorize using value types, use string
        // "category" for categories and real value type
        // name for others.
            if ( wxPGIdIsOk(pg->GetFirstChild(p)) )
                classname = wxT("category");
            else
                classname = pg->GetPropertyValueType(p);


        s.Printf(wxT("%*s\"%s\" \"%s\" \"%s\""),
            depth*2,
            wxT(""),
            classname.c_str(),
            pg->GetPropertyLabel(p).c_str(),
            s_value.c_str());

        // Write attributes, if any
        // FIXME
        /*
        s_attribs = pg->GetPropertyAttributes(id);
        if ( s_attribs.length() )
        {
            s.Append(wxT(" \""));
            s.Append(s_attribs);
            s.Append(wxT("\""));
        }
        */

        // Append choices, if any
        wxPGChoices& choices = pg->GetPropertyChoices(p);
        if ( choices.IsOk() )
        {
            // If no attribs already, add empty token as a substitute
            // (must be because we add tokens after it)
            if ( !s_attribs.length() )
                s.Append(wxT(" \"\""));

            // First add id of the choices list inorder to optimize
            s2.Printf(wxT(" \"%X\""),(ptrdiff_t)choices.GetId());
            s.Append(s2);

            size_t i;
            for ( i=0; i<choices.GetCount(); i++ )
            {
                if ( choices.HasValue(i) )
                    s2.Printf(wxT(" \"%s||%i\""),choices.GetLabel(i).c_str(),choices.GetValue(i));
                else
                    s2.Printf(wxT(" \"%s\""),choices.GetLabel(i).c_str());
                s.Append(s2);
            }
        }

        f.AddLine(s);

        // Write children, if any
        if ( p->GetChildCount() )
        {
            WritePropertiesToFile( pg, (wxPGProperty*)p, f, depth+1, useClassName );

            // Add parent's terminator
            s.Printf(wxT("%*s\"%s\" \"Ends\""),
                depth*2,
                wxT(""),
                classname.c_str());
            f.AddLine(s);
        }
    }
}

// -----------------------------------------------------------------------

void FormMain::OnSaveToFileClick ( wxCommandEvent& event )
{
    int id = event.GetId();

    wxFileDialog dlg(this,
                     wxT("Choose File to Save"),
                     wxEmptyString,
                     wxEmptyString,
                     wxT("Text files (*.txt)|*.txt|All files (*.*)|*.*"),
#if wxCHECK_VERSION(2,8,0)
                     wxFD_SAVE|wxFD_OVERWRITE_PROMPT
#else
                     wxSAVE|wxOVERWRITE_PROMPT
#endif
                    );

    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();

    if ( dlg.ShowModal() == wxID_OK )
    {
        wxTextFile f(dlg.GetPath());

        // Determine whether we want to categorize properties by
        // their class names or value types.
        if ( id==ID_SAVETOFILE )
            f.AddLine(wxT("\"wxPropertyGrid State ByClass\""));
        else
            f.AddLine(wxT("\"wxPropertyGrid State ByValueType\""));

        // Iterate through properties
        WritePropertiesToFile(pg,pg->GetRoot(),f,0,id==ID_SAVETOFILE);

        f.Write();
    }
}

// -----------------------------------------------------------------------

void FormMain::OnLoadFromFileClick ( wxCommandEvent& )
{
#if 0
    wxFileDialog dlg(this,
                     wxT("Choose File to Load"),
                     wxEmptyString,
                     wxEmptyString,
                     wxT("Text files (*.txt)|*.txt|All files (*.*)|*.*"),
#if wxCHECK_VERSION(2,8,0)
                     wxFD_OPEN
#else
                     wxOPEN
#endif
                     );

    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();

    if ( dlg.ShowModal() == wxID_OK )
    {
        pg->Freeze();
        pg->Clear();

        wxTextFile f(dlg.GetPath());

        f.Open();

        wxPropertyGridPopulator populator(pg);

        unsigned int linenum = 0;

        wxString s_class;
        wxString s_name;
        wxString s_value;
        wxString s_attr;
        wxPGChoicesId choices_id;

        bool useClassName = true;

        wxArrayString choice_labels;
        wxArrayInt choice_values;

        // Identify property categorization (i.e. class name or value type)
        if ( linenum < f.GetLineCount() )
        {
            wxString& s = f.GetLine(linenum);
            linenum++;
            if ( s.SubString(1,14) != wxT("wxPropertyGrid") )
            {
                wxLogError(wxT("File is not of valid type."));
                linenum = f.GetLineCount();
            }
            else
            if ( s.Find(wxT("ByValueType")) != wxNOT_FOUND )
                useClassName = false;
        }

        while ( linenum < f.GetLineCount() )
        {
            wxString& s = f.GetLine(linenum);
            linenum++;

            //wxLogDebug(wxT("%s"),s.c_str());

            int step = 0;
            choices_id = 0;
            int choices_mode = 0; // 1 = no values, 2 = with values
            size_t end = s.length();

            choice_labels.Empty();
            choice_values.Empty();
            s_attr.Empty();

            if ( s.length() && s.GetChar(0) != wxT('#') )
            {

                // Parse tokens
                // NOTE: This code *requires* inclusion of propdev.h
                //   (due to tokenizer)
                WX_PG_TOKENIZER2_BEGIN(s,wxT('"'))
                    if ( step == 0 )
                        s_class = token;
                    else if ( step == 1 )
                        s_name = token;
                    else if ( step == 2 )
                        s_value = token;
                    else if ( step == 3 )
                        s_attr = token;
                    else if ( step == 4 )
                    {
                        unsigned long a;
                        if ( token.ToULong(&a,16) )
                        {
                            choices_id = (wxPGChoicesId)a;
                            if ( populator.HasChoices(choices_id) )
                                break;
                        }
                        else
                            wxLogError(wxT("Line %i: Fourth token, if any, must be hexadecimal identifier (it was \"%s\")"),linenum,token.c_str());
                    }
                    else
                    {
                        // Choices
                        if ( !choices_mode )
                        {
                            choices_mode = 1;
                            if ( s.find(wxT("||")) < end )
                                choices_mode = 2;
                        }

                        if ( choices_mode == 1 )
                        {
                            choice_labels.Add(token);
                        }
                        else
                        {
                            size_t delim_pos = token.rfind(wxT("||"));
                            if ( delim_pos < end )
                            {
                                choice_labels.Add(token.substr(0,delim_pos));
                                long int_val = 0;
                                if ( token.length() > (size_t)(delim_pos+2) )
                                {
                                    wxString int_str = token.substr(delim_pos+2);
                                    int_str.ToLong(&int_val,10);
                                }

                                choice_values.Add(int_val);

                            }
                            else
                            {
                                choice_labels.Add(token);
                                choice_values.Add(0);
                            }
                        }

                        //wxLogDebug(wxT("%s=%i"),choice_labels[choice_labels.GetCount()-1].c_str(),
                        //    choice_values.GetCount()?choice_values[choice_values.GetCount()-1]:0);

                    }

                    step++;
                WX_PG_TOKENIZER2_END()

                if ( step )
                {

                    wxLogDebug(wxT("%s: %s %s"),s_class.c_str(),s_name.c_str(),s_value.c_str());

                    // First check for group terminator
                    if ( step == 2 &&
                         s_name == wxT("Ends") )
                    {
                        populator.EndChildren();
                    }
                    else
                    // There is no value type for category, so we need to
                    // check it separately.
                    if ( !useClassName && s_class == wxT("category") )
                    {
                        populator.AppendByClass(wxT("Category"),
                                                s_name,
                                                wxPG_LABEL);
                        populator.BeginChildren();
                    }
                    else if ( step >= 3 )
                    {
                        // Prepare choices, if any
                        if ( choices_id && !populator.HasChoices(choices_id) )
                        {
                            populator.AddChoices(choices_id,choice_labels,choice_values);
                        }

                        // New property
                        if ( useClassName )
                            populator.AppendByClass(s_class,
                                                    s_name,
                                                    wxPG_LABEL,
                                                    s_value,
                                                    s_attr,
                                                    choices_id);
                        else
                            populator.AppendByType(s_class,
                                                   s_name,
                                                   wxPG_LABEL,
                                                   s_value,
                                                   s_attr,
                                                   choices_id);

                        // Automatically start adding children
                        // This returns false if that really was not possible
                        // (i.e. we appended something else than category)
                        populator.BeginChildren();

                    }
                    else
                    {
                        wxLogError(wxT("Line %i: Only %i tokens (minimum of 3 required for this type)"),linenum,step);
                    }
                }
            }
        }

        pg->Thaw();
    }
#endif
}

// -----------------------------------------------------------------------

void FormMain::OnSetPropertyValue( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();
    wxPGId selected = pg->GetSelection();

    if ( wxPGIdIsOk(selected) )
    {
        wxString value = ::wxGetTextFromUser( wxT("Enter new value:") );
        pg->SetPropertyValue( selected, value );
    }
}

// -----------------------------------------------------------------------

void FormMain::OnInsertChoice( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();

    wxPGId selected = pg->GetSelection();
    wxPGChoices& choices = pg->GetPropertyChoices(selected);

    // Insert new choice to the center of list

    if ( choices.IsOk() )
    {
        int pos = choices.GetCount() / 2;
        pg->InsertPropertyChoice(selected,wxT("New Choice"),pos);
    }
    else
    {
        ::wxMessageBox(wxT("First select a property with some choices."));
    }
}

// -----------------------------------------------------------------------

void FormMain::OnDeleteChoice( wxCommandEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = m_pPropGridMan->GetGrid();

    wxPGId selected = pg->GetSelection();
    wxPGChoices& choices = pg->GetPropertyChoices(selected);

    // Deletes choice from the center of list

    if ( choices.IsOk() )
    {
        int pos = choices.GetCount() / 2;
        pg->DeletePropertyChoice(selected,pos);
    }
    else
    {
        ::wxMessageBox(wxT("First select a property with some choices."));
    }
}

// -----------------------------------------------------------------------

#include <wx/colordlg.h>

void FormMain::OnMisc ( wxCommandEvent& event )
{
    int id = event.GetId();
    if ( id == ID_STATICLAYOUT )
    {
        long wsf = m_pPropGridMan->GetWindowStyleFlag();
        if ( event.IsChecked() ) m_pPropGridMan->SetWindowStyleFlag( wsf|wxPG_STATIC_LAYOUT );
        else m_pPropGridMan->SetWindowStyleFlag( wsf&~(wxPG_STATIC_LAYOUT) );
    }
    else if ( id == ID_CLEAR )
    {
        m_pPropGridMan->ClearPropertyValue(m_pPropGridMan->GetGrid()->GetSelection());
    }
    else if ( id == ID_COLLAPSEALL )
    {
#if wxPG_COMPATIBILITY_1_2_0
        m_pPropGridMan->GetGrid()->CollapseAll();
#else
        wxPGVIterator it;
        wxPropertyGrid* pg = m_pPropGridMan->GetGrid();

        for ( it = pg->GetVIterator( wxPG_ITERATE_ALL ); !it.AtEnd(); it.Next() )
            it.GetProperty()->SetExpanded( false );

        pg->RefreshGrid();
#endif
    }
    else if ( id == ID_GETVALUES )
    {
        m_storedValues = m_pPropGridMan->GetGrid()->GetPropertyValues(wxT("Test"),
                                                                      m_pPropGridMan->GetGrid()->GetRoot(),
                                                                      wxPG_KEEP_STRUCTURE|wxPG_INC_ATTRIBUTES);
    }
    else if ( id == ID_SETVALUES )
    {
        if ( m_storedValues.GetType() == wxT("list") )
        {
            m_pPropGridMan->GetGrid()->SetPropertyValues(m_storedValues);
        }
        else
            wxMessageBox(wxT("First use Get Property Values."));
    }
    else if ( id == ID_SETVALUES2 )
    {
        wxVariant list;
        list.NullList();
        list.Append( wxVariant((long)1234,wxT("VariantLong")) );
        list.Append( wxVariant((bool)TRUE,wxT("VariantBool")) );
        list.Append( wxVariant(wxT("Test Text"),wxT("VariantString")) );
        m_pPropGridMan->GetGrid()->SetPropertyValues(list);
    }
    else if ( id == ID_COLLAPSE )
    {
        // Collapses selected.
        wxPGId id = m_pPropGridMan->GetSelectedProperty();
        if ( wxPGIdIsOk(id) )
        {
            m_pPropGridMan->Collapse(id);
        }
    }
    else if ( id == ID_RUNTESTFULL )
    {
        // Runs a regression test.
        RunTests(true);
    }
    else if ( id == ID_RUNTESTPARTIAL )
    {
        // Runs a regression test.
        RunTests(false);
    }
    else if ( id == ID_UNSPECIFY )
    {
        wxPGId prop = m_pPropGridMan->GetSelectedProperty();
        if ( wxPGIdIsOk(prop) )
        {
            m_pPropGridMan->SetPropertyUnspecified(prop);
        }
    }
    else if ( id == ID_SETCOLOUR )
    {
        wxPGId prop = m_pPropGridMan->GetSelectedProperty();
        if ( wxPGIdIsOk(prop) )
        {
            wxColourData data;
            data.SetChooseFull(true);
            int i;
            for ( i = 0; i < 16; i++)
            {
                wxColour colour(i*16, i*16, i*16);
                data.SetCustomColour(i, colour);
            }

            wxColourDialog dialog(this, &data);
            if ( dialog.ShowModal() == wxID_OK )
            {
                wxColourData retData = dialog.GetColourData();
                m_pPropGridMan->SetPropertyBackgroundColour(prop,retData.GetColour());
            }
        }
    }
}

// -----------------------------------------------------------------------

void FormMain::OnPopulateClick( wxCommandEvent& event )
{
    int id = event.GetId();
    m_propGrid->Clear();
    m_propGrid->Freeze();
    if ( id == ID_POPULATE1 )
    {
        PopulateWithStandardItems();
    }
    else if ( id == ID_POPULATE2 )
    {
        PopulateWithLibraryConfig();
    }
    m_propGrid->Thaw();
}

// -----------------------------------------------------------------------

void FormMain::OnCloseEvent( wxCloseEvent& event )
{
    /*if ( !m_propGrid->CanClose() )
    {
        ::wxMessageBox(wxT("Cannot exit: value being edited in the property grid is invalid."));
        event.Veto();
        return;
    }*/
    event.Skip();
}

// -----------------------------------------------------------------------

static const wxChar* _fs_windowstyle_labels[] = {
    wxT("wxSIMPLE_BORDER"),
    wxT("wxDOUBLE_BORDER"),
    wxT("wxSUNKEN_BORDER"),
    wxT("wxRAISED_BORDER"),
    wxT("wxNO_BORDER"),
    wxT("wxTRANSPARENT_WINDOW"),
    wxT("wxTAB_TRAVERSAL"),
    wxT("wxWANTS_CHARS"),
#if wxNO_FULL_REPAINT_ON_RESIZE
    wxT("wxNO_FULL_REPAINT_ON_RESIZE"),
#endif
    wxT("wxVSCROLL"),
    wxT("wxALWAYS_SHOW_SB"),
    wxT("wxCLIP_CHILDREN"),
#if wxFULL_REPAINT_ON_RESIZE
    wxT("wxFULL_REPAINT_ON_RESIZE"),
#endif
    (const wxChar*) NULL // terminator is always needed
};

static const long _fs_windowstyle_values[] = {
    wxSIMPLE_BORDER,
    wxDOUBLE_BORDER,
    wxSUNKEN_BORDER,
    wxRAISED_BORDER,
    wxNO_BORDER,
    wxTRANSPARENT_WINDOW,
    wxTAB_TRAVERSAL,
    wxWANTS_CHARS,
#if wxNO_FULL_REPAINT_ON_RESIZE
    wxNO_FULL_REPAINT_ON_RESIZE,
#endif
    wxVSCROLL,
    wxALWAYS_SHOW_SB,
    wxCLIP_CHILDREN,
#if wxFULL_REPAINT_ON_RESIZE
    wxFULL_REPAINT_ON_RESIZE
#endif
};

// -----------------------------------------------------------------------

// for wxCustomProperty testing
bool TestCallback(wxPropertyGrid*,wxPGProperty*,wxWindow*,int)
{
    ::wxMessageBox(wxT("Button was clicked."),wxT("Callback works"));
    return false;
}

// for wxCustomProperty testing
void TestPaintCallback(wxPGProperty* WXUNUSED(property),wxDC& dc,
                       const wxRect& rect,wxPGPaintData& paintdata)
{
    if ( rect.x < 0 )
    {
        // Measuring
        paintdata.m_drawnHeight = 20+paintdata.m_choiceItem;
    }
    else
    {
        // Drawing

        wxRect r = rect;
        r.width += paintdata.m_choiceItem;
        if ( paintdata.m_choiceItem >= 0 )
            r.height = 20+paintdata.m_choiceItem;

        dc.SetClippingRegion(r);
        wxColour oldTxCol = dc.GetTextForeground();

        dc.SetPen(*wxBLACK);
        dc.SetBrush(*wxWHITE);
        dc.DrawRectangle(r);
        dc.SetTextForeground(*wxBLACK);
        wxString s;
        s.Printf(wxT("%i"),paintdata.m_choiceItem);
        dc.DrawText(s,r.x+6,r.y+2);

        paintdata.m_drawnWidth = r.width;

        dc.SetTextForeground(oldTxCol);
        dc.DestroyClippingRegion();
    }
}

#if wxUSE_XRC && wxCHECK_VERSION(2,8,0)
#include "wx/xrc/xh_all.h"

void FormMain::OnTestXRC(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource* xmlResource = wxXmlResource::Get();
    xmlResource->AddHandler(new wxDialogXmlHandler);
    xmlResource->AddHandler(new wxSizerXmlHandler);
    xmlResource->AddHandler(new wxButtonXmlHandler);
    xmlResource->AddHandler(new wxPropertyGridXmlHandler());
    xmlResource->Load(wxT("../samples/sample.xrc"));
    xmlResource->LoadDialog(&dlg, this, wxT("dlg1"));
    dlg.ShowModal();
}
#else

void FormMain::OnTestXRC(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxT("wxPropertyGrid XRC support requires wxWidgets 2.8.0 or newer."));
}

#endif

void FormMain::OnEnableCommonValues(wxCommandEvent& WXUNUSED(event))
{
    wxPGProperty* prop = m_pPropGridMan->GetSelectedProperty();
    if ( prop )
        prop->EnableCommonValue();
    else
        wxMessageBox(wxT("First select a property"));
}

void FormMain::PopulateWithStandardItems ()
{
#if TESTING_WXPROPERTYGRIDADV
    wxPropertyGridManager* pgman = m_pPropGridMan;
    wxPropertyGridPage* pg = pgman->GetPage(wxT("Standard Items"));
#else
    wxPropertyGrid* pg = m_propGrid;
#endif
    wxPGId pid;

    // Append is ideal way to add items to wxPropertyGrid.
    pg->Append( new wxPropertyCategory(wxT("Appearance"),wxPG_LABEL) );

    pg->Append( new wxStringProperty(wxT("Label"),wxPG_LABEL,GetTitle()) );
    pg->Append( new wxFontProperty(wxT("Font"),wxPG_LABEL) );
    pg->SetPropertyHelpString ( wxT("Font"), wxT("Editing this will change font used in the property grid.") );

    pg->Append( new wxSystemColourProperty(wxT("Margin Colour"),wxPG_LABEL,
        pg->GetGrid()->GetMarginColour()) );

    pg->Append( new wxSystemColourProperty(wxT("Cell Colour"),wxPG_LABEL,
        pg->GetGrid()->GetCellBackgroundColour()) );
    pg->Append( new wxSystemColourProperty(wxT("Cell Text Colour"),wxPG_LABEL,
        pg->GetGrid()->GetCellTextColour()) );
    pg->Append( new wxSystemColourProperty(wxT("Line Colour"),wxPG_LABEL,
        pg->GetGrid()->GetLineColour()) );
    pg->Append( new wxFlagsProperty(wxT("Window Styles"),wxPG_LABEL,
        m_combinedFlags, GetWindowStyle()) );

    //pg->SetPropertyAttribute(wxT("Window Styles"),wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING,true,wxPG_RECURSE);

    pg->Append( new wxCursorProperty(wxT("Cursor"),wxPG_LABEL) );

    pg->Append( new wxPropertyCategory(wxT("Position"),wxT("PositionCategory")) );
    pg->SetPropertyHelpString( wxT("PositionCategory"), wxT("Change in items in this category will cause respective changes in frame.") );
    pg->Append( new wxIntProperty(wxT("Height"),wxPG_LABEL,480) );
    pg->Append( new wxIntProperty(wxT("Width"),wxPG_LABEL,640) );
    pg->Append( new wxIntProperty(wxT("X"),wxPG_LABEL,10) );
    pg->Append( new wxIntProperty(wxT("Y"),wxPG_LABEL,10) );

    const wxChar* disabledHelpString = wxT("This property is simply disabled. Inorder to have label disabled as well, ")
                                       wxT("you need to set wxPG_EX_GREY_LABEL_WHEN_DISABLED using SetExtraStyle.");

    pg->Append( new wxPropertyCategory(wxT("Environment"),wxPG_LABEL) );
    pg->Append( new wxStringProperty(wxT("Operating System"),wxPG_LABEL,::wxGetOsDescription()) );

    pg->Append( new wxStringProperty(wxT("User Id"),wxPG_LABEL,::wxGetUserId()) );
    pg->Append( new wxDirProperty(wxT("User Home"),wxPG_LABEL,::wxGetUserHome()) );
    pg->Append( new wxStringProperty(wxT("User Name"),wxPG_LABEL,::wxGetUserName()) );

    // Disable some of them
    pg->DisableProperty( wxT("Operating System") );
    pg->DisableProperty( wxT("User Id") );
    pg->DisableProperty( wxT("User Name") );

    pg->SetPropertyHelpString( wxT("Operating System"), disabledHelpString );
    pg->SetPropertyHelpString( wxT("User Id"), disabledHelpString );
    pg->SetPropertyHelpString( wxT("User Name"), disabledHelpString );

    pg->Append( new wxPropertyCategory(wxT("More Examples"),wxPG_LABEL) );

    pg->Append( new wxFontDataProperty( wxT("FontDataProperty"), wxPG_LABEL) );
    pg->SetPropertyHelpString( wxT("FontDataProperty"),
        wxT("This demonstrates wxFontDataProperty class defined in this sample app. ")
        wxT("It is exactly like wxFontProperty from the library, but also has colour sub-property.")
        );

    pg->Append( new wxDirsProperty(wxT("DirsProperty"),wxPG_LABEL) );
    pg->SetPropertyHelpString( wxT("DirsProperty"),
        wxT("This demonstrates wxDirsProperty class defined in this sample app. ")
        wxT("It is built with WX_PG_IMPLEMENT_ARRAYSTRING_PROPERTY_WITH_VALIDATOR macro, ")
        wxT("with custom action (dir dialog popup) defined.")
        );

    pg->Append( new wxAdvImageFileProperty(wxT("AdvImageFileProperty"),wxPG_LABEL) );
    pg->SetPropertyHelpString( wxT("AdvImageFileProperty"),
        wxT("This demonstrates wxAdvImageFileProperty class defined in this sample app. ")
        wxT("Button can be used to add new images to the popup list.")
        );

    wxArrayDouble arrdbl;
    arrdbl.Add(-1.0);
    arrdbl.Add(-0.5);
    arrdbl.Add(0.0);
    arrdbl.Add(0.5);
    arrdbl.Add(1.0);

    pg->Append( new wxArrayDoubleProperty(wxT("ArrayDoubleProperty"),wxPG_LABEL,arrdbl) );
    //pg->SetPropertyAttribute(wxT("ArrayDoubleProperty"),wxPG_FLOAT_PRECISION,(long)2);
    pg->SetPropertyHelpString( wxT("ArrayDoubleProperty"),
        wxT("This demonstrates wxArrayDoubleProperty class defined in this sample app. ")
        wxT("It is an example of a custom list editor property.")
        );

    pg->Append( new wxLongStringProperty(wxT("Information"),wxPG_LABEL,
        wxT("Editing properties will have immediate effect on this window, ")
        wxT("and vice versa (atleast in most cases, that is).")
        ) );
    pg->SetPropertyHelpString( wxT("Information"),
                               wxT("This property is read-only.") );

    pg->SetPropertyReadOnly( wxT("Information"), true );

    //
    // wxCustomProperty demonstration
    // NOTE: Most customization features in this class have already been
    //   merged in wxPGProperty.
    //

    pg->Append( new wxPropertyCategory(wxT("Custom User Property")) );

    pid = pg->Append( new wxCustomProperty(wxT("CustomProperty"),wxPG_LABEL) );
    pg->SetPropertyHelpString( pid,
        wxT("This is example of wxCustomProperty, easily customizable property class. ")
        wxT("Editing child properties will modify this property in real-time.") );

#ifdef wxPG_COMPATIBILITY_1_0_0
    // In 1.1.0+ button click event handler should handle this.
        pg->SetPropertyAttribute( pid, wxPG_CUSTOM_CALLBACK, (void*) TestCallback );
#endif

    // Add items for editing the custom property in real-time
    // See FormMain::OnPropertyGridChange for appropriate event handling

    pg->AppendIn( pid, new wxStringProperty(wxT("Label"),
                                            wxPG_LABEL,
                                            wxT("CustomProperty")) );

    pg->AppendIn( pid, new wxImageFileProperty(wxT("Image"),
                                               wxPG_LABEL) );

    static const wxChar* editors[] = {wxT("TextCtrl"),
                                      wxT("Choice"),
                                      wxT("ComboBox"),
                                      wxT("TextCtrlAndButton"),
                                      wxT("ChoiceAndButton"),
                                      (const wxChar*) NULL};

    pg->AppendIn( pid, new wxEnumProperty(wxT("Editor"),
                                          wxPG_LABEL,
                                          editors) );

    wxArrayString defaultChoices;
    defaultChoices.Add(wxT("First Choice"));

    pg->AppendIn( pid, new wxArrayStringProperty(wxT("Choices"),
                                                 wxPG_LABEL,
                                                 defaultChoices) );
    pg->DisableProperty(wxT("CustomProperty.Choices")); // Initially cannot be modified
    pg->SetPropertyHelpString(wxT("CustomProperty.Choices"),
        wxT("Disabled for non-Choice editors"));

    static const wxChar* paintMode[] = {wxT("Use Image"),
                                        wxT("Use Callback"),
                                        (const wxChar*) NULL};

    pg->AppendIn( pid, new wxEnumProperty(wxT("Paint Mode"),
                                          wxPG_LABEL,
                                          paintMode) );

    //
    // Set test information for cells
    wxPropertyGridIterator it;
    wxBitmap bmp = wxArtProvider::GetBitmap(wxART_FOLDER);

    for ( it = pg->GetGrid()->GetIterator();
          !it.AtEnd();
          it++ )
    {
        wxPGProperty* p = *it;
        if ( p->IsCategory() )
            continue;

        pg->SetPropertyCell( p, 2, wxT("Cell 2"), bmp );
        pg->SetPropertyCell( p, 3, wxT("Cell 3"), wxNullBitmap, *wxWHITE, *wxBLACK );
    }
}

// -----------------------------------------------------------------------

void FormMain::PopulateWithExamples ()
{
#if TESTING_WXPROPERTYGRIDADV
    wxPropertyGridManager* pgman = m_pPropGridMan;
    wxPropertyGridPage* pg = pgman->GetPage(wxT("Examples"));
#else
    wxPropertyGrid* pg = m_propGrid;
#endif
    wxPGId pid;

    //pg->Append( new wxPropertyCategory(wxT("Examples (low priority)"),wxT("Examples")) );
    //pg->SetPropertyHelpString ( wxT("Examples"), wxT("This category has example of (almost) every built-in property class.") );

#if wxUSE_SPINBTN
    pg->Append( new wxIntProperty ( wxT("SpinCtrl"), wxPG_LABEL, 0 ) );

    pg->SetPropertyEditor( wxT("SpinCtrl"), wxPG_EDITOR(SpinCtrl) );
    pg->SetPropertyAttribute( wxT("SpinCtrl"), wxT("Min"), (long)-10 );
    pg->SetPropertyAttribute( wxT("SpinCtrl"), wxT("Max"), (long)10 );
    pg->SetPropertyAttribute( wxT("SpinCtrl"), wxT("Step"), (long)2 );
    //pg->SetPropertyAttribute( wxT("SpinCtrl"), wxT("Wrap"), true );

    pg->SetPropertyHelpString( wxT("SpinCtrl"),
        wxT("This is regular wxIntProperty, which editor has been ")
        wxT("changed to wxPG_EDITOR(SpinCtrl). Note however that ")
        wxT("static wxPropertyGrid::RegisterAdditionalEditors() ")
        wxT("needs to be called prior to using it."));

#endif

    // Add bool property
    pg->Append( new wxBoolProperty( wxT("BoolProperty"), wxPG_LABEL, false ) );

    // Add bool property with check box
    pg->Append( new wxBoolProperty( wxT("BoolProperty with CheckBox"), wxPG_LABEL, false ) );
    pg->SetPropertyAttribute( wxT("BoolProperty with CheckBox"),
                              wxPG_BOOL_USE_CHECKBOX,
                              true );

    pg->SetPropertyHelpString( wxT("BoolProperty with CheckBox"),
        wxT("Property attribute wxPG_BOOL_USE_CHECKBOX has been set to true.") );


    wxPGProperty* lineThickProp;
    wxPGProperty* lineThickUnitProp;
    lineThickProp = new wxFloatProperty(wxT("Line Thickness"));
    lineThickProp->SetEditor(wxPG_EDITOR(SpinCtrl));
    lineThickProp->SetAttribute(wxT("Min"), wxVariant(0.01));

     wxPGChoices choices;
     choices.Add(wxT("millimetres"), 1);
     choices.Add(wxT("inches"), 2);
    lineThickUnitProp = new wxEnumProperty(
             wxT("Unit"), wxPG_LABEL, choices,
             static_cast<int>(1));

     pg->Append(lineThickProp);
     pg->AppendIn(lineThickProp, lineThickUnitProp);

     lineThickProp->SetValue(wxVariant());
     /*if (!pg->HideProperty(lineThickUnitProp, true))
     {
             return; 
     }*/

     pid = pg->Append( new wxFloatProperty( wxT("FloatProperty"),
                                       wxPG_LABEL,
                                       1234500.23 ) );

    // A string property that can be edited in a separate editor dialog.
    pg->Append( new wxLongStringProperty( wxT("LongStringProperty"), wxT("LongStringProp"),
        wxT("This is much longer string than the first one. Edit it by clicking the button.") ) );

    // A property that edits a wxArrayString.
    wxArrayString example_array;
    example_array.Add( wxT("String 1"));
    example_array.Add( wxT("String 2"));
    example_array.Add( wxT("String 3"));
    pg->Append( new wxArrayStringProperty( wxT("ArrayStringProperty"), wxPG_LABEL,
                                           example_array) );

#if wxUSE_VALIDATORS
    // For testing validator in the string editor dialog
    wxInvalidWordValidator iwValidator(wxT("test"));
    pg->SetPropertyValidator( wxT("ArrayStringProperty"), iwValidator );
    pg->SetPropertyHelpString( wxT("ArrayStringProperty"),
        wxT("Has validator that prevents using text 'test'. This should work in the ")
        wxT("dialog as well.") );
#endif

    // Test adding same category multiple times ( should not actually create a new one )
    //pg->Append( new wxPropertyCategory(wxT("Examples (low priority)"),wxT("Examples")) );

    // A file selector property. Note that argument between name
    // and initial value is wildcard (format same as in wxFileDialog).
    wxPGProperty* prop = new wxFileProperty( wxT("FileProperty"), wxT("TextFile") );
    pg->Append( prop );

    prop->SetAttribute(wxPG_FILE_WILDCARD,wxT("Text Files (*.txt)|*.txt"));
    prop->SetAttribute(wxPG_FILE_DIALOG_TITLE,wxT("Custom File Dialog Title"));
    prop->SetAttribute(wxPG_FILE_SHOW_FULL_PATH,false);

#ifdef __WXMSW__
    prop->SetAttribute(wxPG_FILE_SHOW_RELATIVE_PATH,wxT("C:\\Windows"));
    pg->SetPropertyValue(prop,wxT("C:\\Windows\\System32\\msvcrt71.dll"));
#endif

#if wxUSE_IMAGE
    // An image file property. Arguments are just like for FileProperty, but
    // wildcard is missing (it is autogenerated from supported image formats).
    // If you really need to override it, create property separately, and call
    // its SetWildcard method.
    pg->Append( new wxImageFileProperty( wxT("ImageFile"), wxPG_LABEL ) );
#endif

    pid = pg->Append( new wxColourProperty(wxT("ColourProperty"),wxPG_LABEL,*wxRED) );
    //pg->SetPropertyAttribute(pid,wxPG_COLOUR_ALLOW_CUSTOM,false);
    pg->SetPropertyEditor( wxT("ColourProperty"), wxPG_EDITOR(ComboBox) );
    pg->GetProperty(wxT("ColourProperty"))->SetFlag(wxPG_PROP_AUTO_UNSPECIFIED);
    pg->SetPropertyHelpString( wxT("ColourProperty"),
        wxT("wxPropertyGrid::SetPropertyEditor method has been used to change ")
        wxT("editor of this property to wxPG_EDITOR(ComboBox)"));

    //
    // This demonstrates using alternative editor for colour property
    // to trigger colour dialog directly from button.
    pg->Append( new wxColourProperty(wxT("ColourProperty2"),wxPG_LABEL,*wxGREEN) );

    pg->Append( new wxMyColourProperty(wxT("CustomColourProperty1")) );

    pg->SetPropertyHelpString(wxT("CustomColourProperty1"),
        wxT("This is a wxMyColourProperty from the sample app. ")
        wxT("It is built with WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY_USES_WXCOLOUR macro ")
        wxT("and has wxColour as its data type"));

    pg->Append( new wxMyColour2Property(wxT("CustomColourProperty2")) );

    pg->SetPropertyHelpString(wxT("CustomColourProperty2"),
        wxT("This is a wxMyColour2Property from the sample app. ")
        wxT("It is built with WX_PG_IMPLEMENT_CUSTOM_COLOUR_PROPERTY macro ")
        wxT("and has wxColourPropertyValue as its data type"));

    pg->Append( new MyColourProperty3(wxT("CustomColourProperty3"), wxPG_LABEL, *wxGREEN) );
    pg->GetProperty(wxT("CustomColourProperty3"))->SetFlag(wxPG_PROP_AUTO_UNSPECIFIED);
    pg->SetPropertyEditor( wxT("CustomColourProperty3"), wxPG_EDITOR(ComboBox) );

    pg->SetPropertyHelpString(wxT("CustomColourProperty3"),
        wxT("This is a MyColourProperty3 from the sample app. ")
        wxT("It is built by subclassing wxColourProperty."));

    //
    // wxEnumProperty does not store strings or even list of strings
    // ( so that's why they are static in function ).
    static const wxChar* enum_prop_labels[] = { wxT("One Item"),
        wxT("Another Item"), wxT("One More"), wxT("This Is Last"), NULL };

    // this value array would be optional if values matched string indexes
    static long enum_prop_values[] = { 40, 80, 120, 160 };

    // note that the initial value (the last argument) is the actual value,
    // not index or anything like that. Thus, our value selects "Another Item".
    //
    // 0 before value is number of items. If it is 0, like in our example,
    // number of items is calculated, and this requires that the string pointer
    // array is terminated with NULL.
    pg->Append( new wxEnumProperty(wxT("EnumProperty"),wxPG_LABEL,
        enum_prop_labels, enum_prop_values, 80 ) );

    wxPGChoices soc;

    // use basic table from our previous example
    // can also set/add wxArrayStrings and wxArrayInts directly.
    soc.Set( enum_prop_labels, enum_prop_values );

    // add extra items
    soc.Add( wxT("Look, it continues"), 200 );
    soc.Add( wxT("Even More"), 240 );
    soc.Add( wxT("And More"), 280 );
    soc.Add( wxT("True End of the List"), 320 );

    // Test custom colours ([] operator of wxPGChoices returns
    // references to wxPGChoiceEntry).
    soc[1].SetFgCol(*wxRED);
    soc[1].SetBgCol(*wxLIGHT_GREY);
    soc[2].SetFgCol(*wxGREEN);
    soc[2].SetBgCol(*wxLIGHT_GREY);
    soc[3].SetFgCol(*wxBLUE);
    soc[3].SetBgCol(*wxLIGHT_GREY);
    soc[4].SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER));

    pg->Append( new wxEnumProperty(wxT("EnumProperty 2"),
                                   wxPG_LABEL,
                                   soc,
                                  240) );
    pg->AddPropertyChoice(wxT("EnumProperty 2"),wxT("Testing Extra"),360);

    // Add a second time to test that the caching works
    pg->Append( new wxEnumProperty(wxT("EnumProperty 3"),wxPG_LABEL,
        soc, 360 ) );
    pg->SetPropertyHelpString(wxT("EnumProperty 3"),
        wxT("Should have same choices as EnumProperty 2"));

    pg->Append( new wxEnumProperty(wxT("EnumProperty 4"),wxPG_LABEL,
        soc, 240 ) );
    pg->SetPropertyHelpString(wxT("EnumProperty 4"),
        wxT("Should have same choices as EnumProperty 2"));

    pg->Append( new wxEnumProperty(wxT("EnumProperty 5"),wxPG_LABEL,
        soc, 240 ) );
    pg->SetPropertyChoicesExclusive(wxT("EnumProperty 5"));
    pg->AddPropertyChoice(wxT("EnumProperty 5"),wxT("5th only"),360);
    pg->SetPropertyHelpString(wxT("EnumProperty 5"),
        wxT("Should have one extra item when compared to EnumProperty 4"));

    // Password property example.
    pg->Append( new wxStringProperty(wxT("Password"),wxPG_LABEL, wxT("password")) );
    pg->SetPropertyAttribute( wxT("Password"), wxPG_STRING_PASSWORD, true );
    pg->SetPropertyHelpString( wxT("Password"),
        wxT("Has attribute wxPG_STRING_PASSWORD set to true") );

    // String editor with dir selector button. Uses wxEmptyString as name, which
    // is allowed (naturally, in this case property cannot be accessed by name).
    pg->Append( new wxDirProperty( wxT("DirProperty"), wxPG_LABEL, ::wxGetUserHome()) );
    pg->SetPropertyAttribute( wxT("DirProperty"),
                              wxPG_DIR_DIALOG_MESSAGE,
                              wxT("This is a custom dir dialog message") );

    // Add string property - first arg is label, second name, and third initial value
    pg->Append( new wxStringProperty ( wxT("StringProperty"), wxPG_LABEL ) );
    pg->SetPropertyMaxLength( wxT("StringProperty"), 6 );
    pg->SetPropertyHelpString( wxT("StringProperty"),
        wxT("Max length of this text has been limited to 6, using wxPropertyGrid::SetPropertyMaxLength.") );

    // Set value after limiting so that it will be applied
    pg->SetPropertyValue( wxT("StringProperty"), wxT("some text") );


    // this value array would be optional if values matched string indexes
    //long flags_prop_values[] = { wxICONIZE, wxCAPTION, wxMINIMIZE_BOX, wxMAXIMIZE_BOX };

    //pg->Append( wxFlagsProperty(wxT("Example of FlagsProperty"),wxT("FlagsProp"),
    //    flags_prop_labels, flags_prop_values, 0, GetWindowStyle() ) );


    // Multi choice dialog.
    wxArrayString tchoices;
    /*tchoices.Add(wxT("Cabbage"));
    tchoices.Add(wxT("Carrot"));
    tchoices.Add(wxT("Onion"));
    tchoices.Add(wxT("Potato"));
    tchoices.Add(wxT("Strawberry"));*/

    wxArrayString tchoicesValues;
    tchoicesValues.Add(wxT("Carrot"));
    tchoicesValues.Add(wxT("Potato"));

    pg->Append( new wxEnumProperty(wxT("EnumProperty X"),wxPG_LABEL, tchoices ) );

    pg->Append( new wxMultiChoiceProperty( wxT("MultiChoiceProperty"), wxPG_LABEL,
                                           tchoices, tchoicesValues ) );
    pg->SetPropertyAttribute( wxT("MultiChoiceProperty"), wxT("UserStringMode"), true );

    pg->Append( new wxTestCustomFlagsProperty(wxT("Custom FlagsProperty"), wxPG_LABEL ) );
    pg->SetPropertyEditor( wxT("Custom FlagsProperty"), wxPG_EDITOR(TextCtrlAndButton) );

    pg->Append( new wxTestCustomEnumProperty(wxT("Custom EnumProperty"), wxPG_LABEL ) );

    pg->Append( new wxSizeProperty( wxT("SizeProperty"), wxT("Size"), GetSize() ) );
    pg->Append( new wxPointProperty( wxT("PointProperty"), wxT("Position"), GetPosition() ) );


    // UInt samples
    pg->Append( new wxUIntProperty( wxT("UIntProperty"), wxPG_LABEL, wxULongLong(wxULL(0xFEEEFEEEFEEE))));
    pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_PREFIX, wxPG_PREFIX_NONE );
    pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_BASE, wxPG_BASE_HEX );
    //pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_PREFIX, wxPG_PREFIX_NONE );
    //pg->SetPropertyAttribute( wxT("UIntProperty"), wxPG_UINT_BASE, wxPG_BASE_OCT );

    //
    // wxEditEnumProperty
    wxPGChoices eech;
    eech.Add(wxT("Choice 1"));
    eech.Add(wxT("Choice 2"));
    eech.Add(wxT("Choice 3"));
    pg->Append( new wxEditEnumProperty(wxT("EditEnumProperty"), wxPG_LABEL, eech) ); // , wxT("Choice 2")

    //wxString v_;
    //wxTextValidator validator1(wxFILTER_NUMERIC,&v_);
    //pg->SetPropertyValidator( wxT("EditEnumProperty"), validator1 );

#if wxUSE_DATETIME
    //
    // wxDateTimeProperty
    pg->Append( new wxDateProperty(wxT("DateProperty"), wxPG_LABEL, wxDateTime::Now() ) );

#if wxUSE_DATEPICKCTRL
    pg->SetPropertyAttribute( wxT("DateProperty"), wxPG_DATE_PICKER_STYLE,
                              (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY) );

    pg->SetPropertyHelpString( wxT("DateProperty"),
        wxT("Attribute wxPG_DATE_PICKER_STYLE has been set to (long)(wxDP_DROPDOWN | wxDP_SHOWCENTURY).")
        wxT("Also note that wxPG_ALLOW_WXADV needs to be defined inorder to use wxDatePickerCtrl.") );
#endif

#endif

/*
    wxPGId categoryID = pg->Append( new wxPropertyCategory(wxT("Cat1"), wxPG_LABEL));
    wxPGId parentID = pg->AppendIn(categoryID, wxParentProperty(wxT("Pivot"), wxPG_LABEL));
    pg->AppendIn(parentID, wxFloatProperty(wxT("x"), wxPG_LABEL, 1));
    pg->AppendIn(parentID, wxFloatProperty(wxT("y"), wxPG_LABEL, 1));
    pg->AppendIn(parentID, wxFloatProperty(wxT("z"), wxPG_LABEL, 1));
    parentID = pg->AppendIn(categoryID, wxParentProperty(wxT("Position_"), wxPG_LABEL));
    pg->AppendIn(parentID, wxFloatProperty(wxT("x"), wxPG_LABEL, 1));
    pg->AppendIn(parentID, wxFloatProperty(wxT("y"), wxPG_LABEL, 1));
    pg->AppendIn(parentID, wxFloatProperty(wxT("z"), wxPG_LABEL, 1)); 
*/

    //
    // Add Triangle properties as both wxTriangleProperty and
    // a generic parent property (using wxStringProperty).
    //
    wxPGId topId = pg->Append( new wxStringProperty(wxT("3D Object"), wxPG_LABEL, wxT("<composed>")) );

    pid = pg->AppendIn( topId, new wxStringProperty(wxT("Triangle 1"), wxT("Triangle 1"), wxT("<composed>")) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("A"), wxEmptyString ) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("B"), wxEmptyString ) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("C"), wxEmptyString ) );

    pg->AppendIn( topId, new wxTriangleProperty( wxT("Triangle 2"), wxT("Triangle 2") ) );

    pg->SetPropertyHelpString( wxT("3D Object"),
        wxT("3D Object is wxStringProperty with value \"<composed>\". Two of its children are similar wxStringProperties with ")
        wxT("three wxVectorProperty children, and other two are custom wxTriangleProperties.") );

    pid = pg->AppendIn( topId, new wxStringProperty(wxT("Triangle 3"), wxT("Triangle 3"), wxT("<composed>")) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("A"), wxEmptyString ) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("B"), wxEmptyString ) );
    pg->AppendIn( pid, new wxVectorProperty( wxT("C"), wxEmptyString ) );

    pg->AppendIn( topId, new wxTriangleProperty( wxT("Triangle 4"), wxT("Triangle 4") ) );

    pg->Append( new wxTextStringPropertyWithValidator(wxT("TestProp1"), wxPG_LABEL) );

    //
    // This snippet is a doc sample test
    //
    pid = pg->Append( new wxStringProperty(wxT("Car"),wxPG_LABEL,wxT("<composed>")) );

    pg->AppendIn( pid, new wxStringProperty(wxT("Model"),
                                            wxPG_LABEL,
                                            wxT("Lamborghini Diablo SV")) );

    pg->AppendIn( pid, new wxIntProperty(wxT("Engine Size (cc)"),
                                         wxPG_LABEL,
                                         5707) );

    wxPGId speedId = pg->AppendIn( pid, new wxStringProperty(wxT("Speeds"),wxPG_LABEL,wxT("<composed>")) );
    pg->AppendIn( speedId, new wxIntProperty(wxT("Max. Speed (mph)"),wxPG_LABEL,290) );
    pg->AppendIn( speedId, new wxFloatProperty(wxT("0-100 mph (sec)"),wxPG_LABEL,3.9) );
    pg->AppendIn( speedId, new wxFloatProperty(wxT("1/4 mile (sec)"),wxPG_LABEL,8.6) );

    pg->AppendIn( pid, new wxIntProperty(wxT("Price ($)"),
                                         wxPG_LABEL,
                                         300000) );

    // Make sure the child properties can be accessed correctly
    pg->SetPropertyValue( wxT("Car.Speeds.Max. Speed (mph)"), 300 );

    // Displayed value of "Car" property is now:
    // "Lamborghini Diablo SV; [300; 3.9; 8.6]; 300000"

    //
    // Test wxSampleMultiButtonEditor
    wxPGRegisterEditorClass( SampleMultiButtonEditor );
    pg->Append( new wxLongStringProperty(wxT("MultipleButtons"), wxPG_LABEL) );
    pg->SetPropertyEditor(wxT("MultipleButtons"), wxPG_EDITOR(SampleMultiButtonEditor) );

    // Test SingleChoiceProperty
    pg->Append( new SingleChoiceProperty(wxT("SingleChoiceProperty")) );


    //
    // Test adding variable height bitmaps in wxPGChoices
    wxPGChoices bc;

    bc.Add(wxT("Wee"), wxBitmap(16, 16));
    bc.Add(wxT("Not so wee"), wxBitmap(32, 32));
    bc.Add(wxT("Friggin' huge"), wxBitmap(64, 64));

    pg->Append( new wxEnumProperty(wxT("Variable Height Bitmaps"),
                                   wxPG_LABEL,
                                   bc,
                                   0) );
}

// -----------------------------------------------------------------------

void FormMain::PopulateWithLibraryConfig ()
{
#if TESTING_WXPROPERTYGRIDADV
    wxPropertyGridManager* pgman = m_pPropGridMan;
    wxPropertyGridPage* pg = pgman->GetPage(wxT("wxWidgets Library Config"));
#else
    wxPropertyGrid* pg = m_propGrid;
#endif

    wxPGProperty* cat;

    wxBitmap bmp = wxArtProvider::GetBitmap(wxART_REPORT_VIEW);

    wxPGProperty* pid;

#define ADD_WX_LIB_CONF_GROUP(A) \
    cat = pg->AppendIn( pid, new wxPropertyCategory(A) ); \
    pg->SetPropertyCell( cat, 0, wxPG_LABEL, bmp );

#define ADD_WX_LIB_CONF(A) pg->Append( new wxBoolProperty(wxT(#A),wxPG_LABEL,(bool)((A>0)?true:false)));
#define ADD_WX_LIB_CONF_NODEF(A) pg->Append( new wxBoolProperty(wxT(#A),wxPG_LABEL,(bool)false) ); \
                            pg->DisableProperty(wxT(#A));

    pid = pg->Append( new wxPropertyCategory( wxT("wxWidgets Library Configuration") ) );
    pg->SetPropertyCell( pid, 0, wxPG_LABEL, bmp );

    ADD_WX_LIB_CONF_GROUP(wxT("Global Settings"))
    ADD_WX_LIB_CONF( wxUSE_GUI )

    ADD_WX_LIB_CONF_GROUP(wxT("Compatibility Settings"))
#if defined(WXWIN_COMPATIBILITY_2_2)
    ADD_WX_LIB_CONF( WXWIN_COMPATIBILITY_2_2 )
#endif
#if defined(WXWIN_COMPATIBILITY_2_4)
    ADD_WX_LIB_CONF( WXWIN_COMPATIBILITY_2_4 )
#endif
#if defined(WXWIN_COMPATIBILITY_2_6)
    ADD_WX_LIB_CONF( WXWIN_COMPATIBILITY_2_6 )
#endif
#ifdef wxFONT_SIZE_COMPATIBILITY
    ADD_WX_LIB_CONF( wxFONT_SIZE_COMPATIBILITY )
#else
    ADD_WX_LIB_CONF_NODEF ( wxFONT_SIZE_COMPATIBILITY )
#endif
#ifdef wxDIALOG_UNIT_COMPATIBILITY
    ADD_WX_LIB_CONF( wxDIALOG_UNIT_COMPATIBILITY )
#else
    ADD_WX_LIB_CONF_NODEF ( wxDIALOG_UNIT_COMPATIBILITY )
#endif

    ADD_WX_LIB_CONF_GROUP(wxT("Debugging Settings"))
    ADD_WX_LIB_CONF( wxUSE_DEBUG_CONTEXT )
    ADD_WX_LIB_CONF( wxUSE_MEMORY_TRACING )
    ADD_WX_LIB_CONF( wxUSE_GLOBAL_MEMORY_OPERATORS )
    ADD_WX_LIB_CONF( wxUSE_DEBUG_NEW_ALWAYS )
    ADD_WX_LIB_CONF( wxUSE_ON_FATAL_EXCEPTION )

    ADD_WX_LIB_CONF_GROUP(wxT("Unicode Support"))
    ADD_WX_LIB_CONF( wxUSE_UNICODE )
    ADD_WX_LIB_CONF( wxUSE_UNICODE_MSLU )
    ADD_WX_LIB_CONF( wxUSE_WCHAR_T )

    ADD_WX_LIB_CONF_GROUP(wxT("Global Features"))
    ADD_WX_LIB_CONF( wxUSE_EXCEPTIONS )
    ADD_WX_LIB_CONF( wxUSE_EXTENDED_RTTI )
    ADD_WX_LIB_CONF( wxUSE_STL )
    ADD_WX_LIB_CONF( wxUSE_LOG )
    ADD_WX_LIB_CONF( wxUSE_LOGWINDOW )
    ADD_WX_LIB_CONF( wxUSE_LOGGUI )
    ADD_WX_LIB_CONF( wxUSE_LOG_DIALOG )
    ADD_WX_LIB_CONF( wxUSE_CMDLINE_PARSER )
    ADD_WX_LIB_CONF( wxUSE_THREADS )
    ADD_WX_LIB_CONF( wxUSE_STREAMS )
    ADD_WX_LIB_CONF( wxUSE_STD_IOSTREAM )

    ADD_WX_LIB_CONF_GROUP(wxT("Non-GUI Features"))
    ADD_WX_LIB_CONF( wxUSE_LONGLONG )
    ADD_WX_LIB_CONF( wxUSE_FILE )
    ADD_WX_LIB_CONF( wxUSE_FFILE )
    ADD_WX_LIB_CONF( wxUSE_FSVOLUME )
    ADD_WX_LIB_CONF( wxUSE_TEXTBUFFER )
    ADD_WX_LIB_CONF( wxUSE_TEXTFILE )
    ADD_WX_LIB_CONF( wxUSE_INTL )
    ADD_WX_LIB_CONF( wxUSE_DATETIME )
    ADD_WX_LIB_CONF( wxUSE_TIMER )
    ADD_WX_LIB_CONF( wxUSE_STOPWATCH )
    ADD_WX_LIB_CONF( wxUSE_CONFIG )
#ifdef wxUSE_CONFIG_NATIVE
    ADD_WX_LIB_CONF( wxUSE_CONFIG_NATIVE )
#else
    ADD_WX_LIB_CONF_NODEF ( wxUSE_CONFIG_NATIVE )
#endif
    ADD_WX_LIB_CONF( wxUSE_DIALUP_MANAGER )
    ADD_WX_LIB_CONF( wxUSE_DYNLIB_CLASS )
    ADD_WX_LIB_CONF( wxUSE_DYNAMIC_LOADER )
    ADD_WX_LIB_CONF( wxUSE_SOCKETS )
    ADD_WX_LIB_CONF( wxUSE_FILESYSTEM )
    ADD_WX_LIB_CONF( wxUSE_FS_ZIP )
    ADD_WX_LIB_CONF( wxUSE_FS_INET )
    ADD_WX_LIB_CONF( wxUSE_ZIPSTREAM )
    ADD_WX_LIB_CONF( wxUSE_ZLIB )
    ADD_WX_LIB_CONF( wxUSE_APPLE_IEEE )
    ADD_WX_LIB_CONF( wxUSE_JOYSTICK )
    ADD_WX_LIB_CONF( wxUSE_FONTMAP )
    ADD_WX_LIB_CONF( wxUSE_MIMETYPE )
    ADD_WX_LIB_CONF( wxUSE_PROTOCOL )
    ADD_WX_LIB_CONF( wxUSE_PROTOCOL_FILE )
    ADD_WX_LIB_CONF( wxUSE_PROTOCOL_FTP )
    ADD_WX_LIB_CONF( wxUSE_PROTOCOL_HTTP )
    ADD_WX_LIB_CONF( wxUSE_URL )
#ifdef wxUSE_URL_NATIVE
    ADD_WX_LIB_CONF( wxUSE_URL_NATIVE )
#else
    ADD_WX_LIB_CONF_NODEF ( wxUSE_URL_NATIVE )
#endif
    ADD_WX_LIB_CONF( wxUSE_REGEX )
    ADD_WX_LIB_CONF( wxUSE_SYSTEM_OPTIONS )
    ADD_WX_LIB_CONF( wxUSE_SOUND )
#ifdef wxUSE_XRC
    ADD_WX_LIB_CONF( wxUSE_XRC )
#else
    ADD_WX_LIB_CONF_NODEF ( wxUSE_XRC )
#endif
    ADD_WX_LIB_CONF( wxUSE_XML )

    // Set them to use check box.
    pg->SetPropertyAttribute(pid,wxPG_BOOL_USE_CHECKBOX,true,wxPG_RECURSE);
}


#if TESTING_WXPROPERTYGRIDADV

//
// Handle events of the third page here.
class wxMyPropertyGridPage : public wxPropertyGridPage
{
public:

    // Return false here to indicate unhandled events should be
    // propagated to manager's parent, as normal.
    virtual bool IsHandlingAllEvents() const { return false; }

protected:

    virtual wxPGProperty* DoInsert( wxPGProperty* parent,
                                    int index,
                                    wxPGProperty* property )
    {
        return wxPropertyGridPage::DoInsert(parent,index,property);
    }

    void OnPropertySelect( wxPropertyGridEvent& event );
    void OnPropertyChange( wxPropertyGridEvent& event );
    void OnPageChange( wxPropertyGridEvent& event );

private:
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(wxMyPropertyGridPage, wxPropertyGridPage)
    EVT_PG_SELECTED( wxID_ANY, wxMyPropertyGridPage::OnPropertySelect )
    EVT_PG_CHANGED( wxID_ANY, wxMyPropertyGridPage::OnPropertyChange )
    EVT_PG_PAGE_CHANGED( wxID_ANY, wxMyPropertyGridPage::OnPageChange )
END_EVENT_TABLE()


void wxMyPropertyGridPage::OnPropertySelect( wxPropertyGridEvent& WXUNUSED(event) )
{
    wxLogDebug(wxT("wxMyPropertyGridPage::OnPropertySelect()"));
}

void wxMyPropertyGridPage::OnPropertyChange( wxPropertyGridEvent& WXUNUSED(event) )
{
    wxLogDebug(wxT("wxMyPropertyGridPage::OnPropertyChange()"));
}

void wxMyPropertyGridPage::OnPageChange( wxPropertyGridEvent& WXUNUSED(event) )
{
    wxLogDebug(wxT("wxMyPropertyGridPage::OnPageChange()"));
}

#endif

class wxPGKeyHandler : public wxEvtHandler
{
public:

    void OnKeyEvent( wxKeyEvent& event )
    {
        wxMessageBox(wxString::Format(wxT("%i"),event.GetKeyCode()));
        event.Skip();
    }
private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxPGKeyHandler,wxEvtHandler)
    EVT_KEY_DOWN( wxPGKeyHandler::OnKeyEvent )
END_EVENT_TABLE()


// -----------------------------------------------------------------------

void FormMain::CreateGrid( int style, int extraStyle )
{
    //
    // This function (re)creates the property grid in our sample
    //

#if TESTING_WXPROPERTYGRIDADV
    if ( style == -1 )
        //style = m_pPropGridMan->GetWindowStyle();
        style = // default style
                wxPG_BOLD_MODIFIED |
                wxPG_SPLITTER_AUTO_CENTER |
                wxPG_AUTO_SORT |
                //wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
                //wxPG_TOOLTIPS |
                //wxPG_HIDE_CATEGORIES |
                //wxPG_LIMITED_EDITING |
                wxTAB_TRAVERSAL |
                wxPG_TOOLBAR |
                wxPG_DESCRIPTION;

    if ( extraStyle == -1 )
        //extraStyle = m_pPropGridMan->GetExtraStyle();
        // default extra style
        extraStyle = wxPG_EX_MODE_BUTTONS;
                //| wxPG_EX_AUTO_UNSPECIFIED_VALUES
                //| wxPG_EX_GREY_LABEL_WHEN_DISABLED
                //| wxPG_EX_NATIVE_DOUBLE_BUFFERING
                //| wxPG_EX_HELP_AS_TOOLTIPS
#endif

    bool wasCreated = false;

    if ( m_panel )
    {
        wasCreated = true;
        m_panel->Destroy();
    }

    wxWindow* panel = new wxPanel(this,-1,wxPoint(0,0),wxSize(400,400));
    m_panel = panel;

//#if 0
    // Column
    wxBoxSizer* topSizer = new wxBoxSizer ( wxVERTICAL );

#if !TESTING_WXPROPERTYGRIDADV && !SIMPLE_DEMO
    //
    // Tabs or RadioBox
    //
#ifdef wxPGS_USE_TABCTRL
    m_tabctrl = new wxTabCtrl( panel, TCID );
    m_tabctrl->InsertItem(0,wxT("Categorized Mode"));
    m_tabctrl->InsertItem(1,wxT("Alphabetic Mode"));

    Connect(m_tabctrl->GetId(), wxEVT_COMMAND_TAB_SEL_CHANGED,
            (wxObjectEventFunction) (wxEventFunction) (wxTabEventFunction)
            &FormMain::OnTabChange );

    topSizer->Add( m_tabctrl, 0, wxEXPAND|wxFIXED_MINSIZE );
#else
    const wxString rb_items[] = {wxString(wxT("Categorized")),
        wxString(wxT("Alphabetic"))};
    m_radiobox = new wxRadioBox ( panel, TCID, wxT(" Mode "),
        wxDefaultPosition, wxDefaultSize,
        2, rb_items, 2, wxRA_SPECIFY_COLS );

    Connect(m_radiobox->GetId(), wxEVT_COMMAND_RADIOBOX_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &FormMain::OnRadioBoxChange );

    topSizer->Add( m_radiobox, 0, wxEXPAND );
#endif

#endif

    //
    // PropertyGrid
    //

    //
    // This shows how to combine two constant descriptors
    m_combinedFlags.Add( _fs_windowstyle_labels, _fs_windowstyle_values );
    m_combinedFlags.Add( _fs_framestyle_labels, _fs_framestyle_values );
#if TESTING_WXPROPERTYGRIDADV

    wxPropertyGridManager* pgman = m_pPropGridMan =
        new wxPropertyGridManager(panel,
                                  // Don't change this into wxID_ANY in the sample, or the
                                  // event handling will obviously be broken.
                                  PGID, /*wxID_ANY*/
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  style );

    m_propGrid = pgman->GetGrid();

    pgman->SetExtraStyle(extraStyle);

    //pgman->GetGrid()->SetCellDisabledTextColour( wxColour(255,0,0) );
    //pgman->GetGrid()->CenterSplitter();

#else

    wxPropertyGrid* pg = m_propGrid =
        new wxPropertyGrid(panel,
                           // Don't change this into wxID_ANY in the sample, or the
                           // event handling will obviously be broken.
                           PGID, /*wxID_ANY*/
                           wxDefaultPosition,
                           wxDefaultSize,
                           style );

    pg->SetExtraStyle(extraStyle);

#endif

    m_pPropGridMan->GetGrid()->SetVerticalSpacing( 2 );

#if TESTING_WXPROPERTYGRIDADV
    pgman->AddPage(wxT("Standard Items"));

    PopulateWithStandardItems();

    pgman->AddPage(wxT("wxWidgets Library Config"));

    PopulateWithLibraryConfig();

    wxPropertyGridPage* myPage = new wxMyPropertyGridPage();
    myPage->Append( new wxIntProperty ( wxT("IntProperty"), wxPG_LABEL, 12345678 ) );

    // Use wxMyPropertyGridPage (see above) to test the
    // custom wxPropertyGridPage feature.
    pgman->AddPage(wxT("Examples"),wxNullBitmap,myPage);

    PopulateWithExamples();

#else
    PopulateWithStandardItems();
#endif

    // Change some attributes in all properties
    //pgman->SetPropertyAttributeAll(wxPG_BOOL_USE_DOUBLE_CLICK_CYCLING,true);
    //pgman->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX,true);

    //m_pPropGridMan->SetSplitterLeft(true);
    //m_pPropGridMan->SetSplitterPosition(137);

    /*
    // This would setup event handling without event table entries
    Connect(m_pPropGridMan->GetId(), wxEVT_PG_SELECTED,
            (wxObjectEventFunction) (wxEventFunction) (wxPropertyGridEventFunction)
            &FormMain::OnPropertyGridSelect );
    Connect(m_pPropGridMan->GetId(), wxEVT_PG_CHANGED,
            (wxObjectEventFunction) (wxEventFunction) (wxPropertyGridEventFunction)
            &FormMain::OnPropertyGridChange );
    */

    topSizer->Add( m_pPropGridMan, 1, wxEXPAND );

#if !SIMPLE_DEMO
    // Additional control (for keyboard navigation testing)
    topSizer->Add( new wxButton(panel,1234,wxT("Button for Tab Traversal Testing")), 0, wxEXPAND|wxFIXED_MINSIZE );
#endif

#if 0
    m_pPropGridMan = NULL;
    m_propGrid = new wxPropertyGrid(panel,
					wxID_ANY,
					wxDefaultPosition,
					wxDefaultSize,
					wxPG_BOLD_MODIFIED |
					//wxPG_SPLITTER_AUTO_CENTER |
					wxPG_AUTO_SORT |
					//wxPG_HIDE_MARGIN | wxPG_STATIC_SPLITTER |
					//wxPG_TOOLTIPS |
					//wxPG_NOCATEGORIES |
					wxTAB_TRAVERSAL |
					wxSUNKEN_BORDER
					);

	m_propGrid->Append( new wxPropertyCategory(wxT("Appearance"),wxPG_LABEL) );

	m_propGrid->Append( new wxStringProperty(wxT("Label"),wxPG_LABEL,wxT("Test")) );

	wxBoxSizer* topSizer = new wxBoxSizer (wxVERTICAL);
	topSizer->Add( m_propGrid, 1, wxALL|wxEXPAND );
	panel->SetSizer(topSizer);
	Layout();
#endif

    panel->SetSizer( topSizer );
    topSizer->SetSizeHints( panel );

    wxBoxSizer* panelSizer = new wxBoxSizer( wxHORIZONTAL );
    panelSizer->Add( panel, 1, wxEXPAND|wxFIXED_MINSIZE );
    SetSizer( panelSizer );
    panelSizer->SetSizeHints( this );

    if ( wasCreated )
    {
        SetSize(
            (wxSystemSettings::GetMetric(wxSYS_SCREEN_X)/10)*4,
            (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)/10)*8
            );
        Centre();
    }
}

// -----------------------------------------------------------------------

FormMain::FormMain(const wxString& title, const wxPoint& pos, const wxSize& size) :
           wxFrame((wxFrame *)NULL, -1, title, pos, size,
               (wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCAPTION|
                wxTAB_TRAVERSAL|wxCLOSE_BOX|wxNO_FULL_REPAINT_ON_RESIZE) )
{
    m_propGrid = NULL;
    m_panel = NULL;

#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = ID_ABOUT;
#endif

#if wxUSE_IMAGE
    // This is here to really test the wxImageFileProperty.
    wxInitAllImageHandlers();
#endif

    CreateGrid( // style
                wxPG_BOLD_MODIFIED |
                wxPG_SPLITTER_AUTO_CENTER |
                wxPG_AUTO_SORT |
                //wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
                //wxPG_TOOLTIPS |
                //wxPG_HIDE_CATEGORIES |
                //wxPG_LIMITED_EDITING |
                wxTAB_TRAVERSAL |
                wxPG_TOOLBAR |
                wxPG_DESCRIPTION,
                // extra style
                wxPG_EX_MODE_BUTTONS
                //| wxPG_EX_AUTO_UNSPECIFIED_VALUES
                //| wxPG_EX_GREY_LABEL_WHEN_DISABLED
                //| wxPG_EX_NATIVE_DOUBLE_BUFFERING
                //| wxPG_EX_HELP_AS_TOOLTIPS
              );

    // Register all editors (SpinCtrl etc.)
    m_pPropGridMan->RegisterAdditionalEditors();

    //
    // Create menubar
    wxMenu *menuFile = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    wxMenu *menuTry = new wxMenu;
    wxMenu *menuTools1 = new wxMenu;
    wxMenu *menuTools2 = new wxMenu;
    wxMenu *menuHelp = new wxMenu;

    menuHelp->Append(ID_ABOUT, wxT("&About..."), wxT("Show about dialog") );

    menuTools1->Append(ID_APPENDPROP, wxT("Append New Property") );
    menuTools1->Append(ID_APPENDCAT, wxT("Append New Category\tCtrl-S") );
    menuTools1->AppendSeparator();
    menuTools1->Append(ID_INSERTPROP, wxT("Insert New Property\tCtrl-Q") );
    menuTools1->Append(ID_INSERTCAT, wxT("Insert New Category\tCtrl-W") );
    menuTools1->AppendSeparator();
    menuTools1->Append(ID_DELETE, wxT("Delete Selected") );
    menuTools1->Append(ID_DELETER, wxT("Delete Random") );
    menuTools1->Append(ID_DELETEALL, wxT("Delete All") );
    menuTools1->AppendSeparator();
    menuTools1->Append(ID_SETCOLOUR, wxT("Set Bg Colour") );
    menuTools1->Append(ID_UNSPECIFY, wxT("Set to Unspecified") );
    menuTools1->Append(ID_CLEAR, wxT("Set Value to Default") );
    menuTools1->AppendSeparator();
    m_itemEnable = menuTools1->Append(ID_ENABLE, wxT("Enable"),
        wxT("Toggles item's enabled state.") );
    m_itemEnable->Enable( FALSE );
    menuTools1->Append(ID_HIDE, wxT("Hide"), wxT("Shows or hides a property") );

    menuTools2->Append(ID_ITERATE1, wxT("Iterate Over Properties") );
    menuTools2->Append(ID_ITERATE2, wxT("Iterate Over Visible Items") );
    menuTools2->Append(ID_ITERATE3, wxT("Reverse Iterate Over Properties") );
    menuTools2->Append(ID_ITERATE4, wxT("Iterate Over Categories") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_SETPROPERTYVALUE, wxT("Set Property Value") );
    menuTools2->Append(ID_CLEARMODIF, wxT("Clear Modified Status"), wxT("Clears wxPG_MODIFIED flag from all properties.") );
    menuTools2->AppendSeparator();
    m_itemFreeze = menuTools2->AppendCheckItem(ID_FREEZE, wxT("Freeze"),
        wxT("Disables painting, auto-sorting, etc.") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_DUMPLIST, wxT("Display Values as wxVariant List"), wxT("Tests GetAllValues method and wxVariant conversion.") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_GETVALUES, wxT("Get Property Values"), wxT("Stores all property values.") );
    menuTools2->Append(ID_SETVALUES, wxT("Set Property Values"), wxT("Reverts property values to those last stored.") );
    menuTools2->Append(ID_SETVALUES2, wxT("Set Property Values 2"), wxT("Adds property values that should not initially be as items (so new items are created).") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_SAVESTATE, wxT("Save Editable State") );
    menuTools2->Append(ID_LOADSTATE, wxT("Load Editable State") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_ENABLECOMMONVALUES, wxT("Enable Common Value"),
        wxT("Enable values that are common to all properties, for selected property."));
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_COLLAPSE, wxT("Collapse Selected") );
    menuTools2->Append(ID_COLLAPSEALL, wxT("Collapse All") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_INSERTPAGE, wxT("Add Page") );
    menuTools2->Append(ID_REMOVEPAGE, wxT("Remove Page") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_CHANGEFLAGSITEMS, wxT("Change Children of FlagsProp") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_TESTINSERTCHOICE, wxT("Test InsertPropertyChoice") );
    menuTools2->Append(ID_TESTDELETECHOICE, wxT("Test DeletePropertyChoice") );
    menuTools2->AppendSeparator();
    menuTools2->Append(ID_SETSPINCTRLEDITOR, wxT("Use SpinCtrl Editor") );
    menuTools2->Append(ID_TESTREPLACE, wxT("Test ReplaceProperty") );

    menuTry->Append(ID_SELECTSTYLE, wxT("Set Window Style"),
        wxT("Select window style flags used by the grid."));
    menuTry->AppendSeparator();
#if !TESTING_WXPROPERTYGRIDADV
    menuTry->AppendRadioItem( ID_POPULATE1, wxT("Default Items") );
    menuTry->AppendRadioItem( ID_POPULATE2, wxT("wxWidgets Library Config") );
    menuTry->AppendSeparator();
#endif
    menuTry->AppendRadioItem( ID_COLOURSCHEME1, wxT("Standard Colour Scheme") );
    menuTry->AppendRadioItem( ID_COLOURSCHEME2, wxT("White Colour Scheme") );
    menuTry->AppendRadioItem( ID_COLOURSCHEME3, wxT(".NET Colour Scheme") );
    menuTry->AppendRadioItem( ID_COLOURSCHEME4, wxT("Cream Colour Scheme") );
    menuTry->AppendSeparator();
    m_itemCatColours = menuTry->AppendCheckItem(ID_CATCOLOURS, wxT("Category Specific Colours"),
        wxT("Switches between category-specific cell colours and default scheme (actually done using SetPropertyTextColour and SetPropertyBackgroundColour).") );
    menuTry->AppendSeparator();
    menuTry->AppendCheckItem(ID_STATICLAYOUT, wxT("Static Layout"),
        wxT("Switches between user-modifiedable and static layouts.") );
    menuTry->Append(ID_SETCOLUMNS, wxT("Set Number of Columns") );
    menuTry->AppendSeparator();
    menuTry->Append(ID_TESTXRC, wxT("Display XRC sample") );
    menuTry->AppendSeparator();
    menuTry->Append(ID_RUNTESTFULL, wxT("Run Tests (full)") );
    menuTry->Append(ID_RUNTESTPARTIAL, wxT("Run Tests (fast)") );

    menuFile->Append(ID_SAVETOFILE, wxT("&Save Page (use class names)..."),
        wxT("Saves current property page to a text file, using property class names.") );
    menuFile->Append(ID_SAVETOFILE2, wxT("&Save Page (use value names)..."),
        wxT("Saves current property page to a text file, using property value type names.") );
    menuFile->Append(ID_LOADFROMFILE, wxT("&Load Page..."),
        wxT("Loads current property page from a text file") );
    menuFile->AppendSeparator();
    menuFile->Append(ID_QUIT, wxT("E&xit\tAlt-X"), wxT("Quit this program") );

    // Now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, wxT("&File") );
    menuBar->Append(menuTry, wxT("&Try These!") );
    menuBar->Append(menuTools1, wxT("&Basic") );
    menuBar->Append(menuTools2, wxT("&Advanced") );
    menuBar->Append(menuHelp, wxT("&Help") );

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    // create a status bar
    CreateStatusBar(1);
    SetStatusText(wxEmptyString);
#endif // wxUSE_STATUSBAR


    //
    // Finalize
    //

    SetSize(
        (wxSystemSettings::GetMetric(wxSYS_SCREEN_X)/10)*4,
        (wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)/10)*8
        );
    Centre();
}

// -----------------------------------------------------------------------

FormMain::~FormMain()
{
}

// -----------------------------------------------------------------------

IMPLEMENT_APP(cxApplication)

bool cxApplication::OnInit()
{
    //wxLocale Locale;
    //Locale.Init(wxLANGUAGE_FINNISH);

	FormMain* frame = Form1 = new FormMain( wxT("wxPropertyGrid Sample"), wxPoint(0,0), wxSize(300,500) );
	//SetTopWindow(frame);
	frame->Show(true);

	return true;
}

// -----------------------------------------------------------------------

void FormMain::OnIdle( wxIdleEvent& event )
{
    /*
    // This code is useful for debugging focus problems
    static wxWindow* last_focus = (wxWindow*) NULL;

    wxWindow* cur_focus = ::wxWindow::FindFocus();

    if ( cur_focus != last_focus )
    {
        const wxChar* class_name = wxT("<none>");
        if ( cur_focus )
            class_name = cur_focus->GetClassInfo()->GetClassName();
        last_focus = cur_focus;
        wxLogDebug( wxT("FOCUSED: %s %X"),
            class_name,
            (unsigned int)cur_focus);
    }
    */

    event.Skip();
}

// -----------------------------------------------------------------------
