#include "stdafx.h"
#include <wx/wx.h>
#include "ToolData.h"
#include "cwxBatchPropertyEditorDlg.h"
#include "RTCutSceneMakerFrame.h"
#include "CommandSet.h"


BEGIN_EVENT_TABLE( cwxBatchPropertyEditorDlg, wxDialog )
	EVT_PG_CHANGED( FILTER_PROP_GRID_ID, cwxBatchPropertyEditorDlg::OnFilterPropertyChanged )
	EVT_PG_CHANGED( CHANGE_VALUE_GRID_ID, cwxBatchPropertyEditorDlg::OnChangeValuePropertyChanged )
	EVT_BUTTON( wxID_OK, cwxBatchPropertyEditorDlg::OnOK )
END_EVENT_TABLE()


cwxBatchPropertyEditorDlg::cwxBatchPropertyEditorDlg( wxWindow* pParent, wxWindowID id, const wxString& title, const wxPoint& pos /* = wxDefaultPosition */, 
													  const wxSize& size /* = wxDefaultSize */, long style /* = wxDEFAULT_DIALOG_STYLE  */) : wxDialog( pParent, id, title, pos, size, style),
													  m_pTopSizer( NULL ),
													  m_pBasicBtnSizer( NULL ),
													  m_pFilterPropGrid( NULL ),
													  m_pChangeValueGrid( NULL ),
													  m_pResultListView( NULL ),
													  m_fStartTimeDelta( 0.0f )
{
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );
	m_pPropertySizer = new wxBoxSizer( wxHORIZONTAL );
	m_pTopSizer->SetMinSize( size );
	m_pFilterPropGridSizer = new wxStaticBoxSizer( wxHORIZONTAL, this, wxT("Filter") );
	m_pChangeValueGridSizer = new wxStaticBoxSizer( wxHORIZONTAL, this, wxT("Change Value") );
	m_pResultListSizer = new wxStaticBoxSizer( wxHORIZONTAL, this, wxT("Filtering Result") );
	m_pBasicBtnSizer = CreateButtonSizer( wxOK|wxCANCEL );

	// ���͸� ���� ������Ƽ �׸���
	_InitFilterProperty();

	// ���� �� �Է� �޴� ������Ƽ �׸���
	_InitChangeValueProperty();
	
	m_pFilterPropGridSizer->Add( m_pFilterPropGrid, wxSizerFlags(1).Align(wxALIGN_CENTER).Expand().Border(wxALL, 3) );
	m_pChangeValueGridSizer->Add( m_pChangeValueGrid, wxSizerFlags(1).Align(wxALIGN_CENTER).Expand().Border(wxALL, 3) );

	m_pPropertySizer->Add( m_pFilterPropGridSizer, wxSizerFlags(1).Align(wxALIGN_CENTER).Expand().Border(wxALL, 3) );
	m_pPropertySizer->Add( m_pChangeValueGridSizer, wxSizerFlags(1).Align(wxALIGN_CENTER).Expand().Border(wxALL, 3) );

	// ���͸� ��� ����Ʈ ��
	m_pResultListView = new wxListView( this, LIST_FILTERED_RESULT_ID,
										wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	_SetupResultListColumn();

	m_pResultListSizer->Add( m_pResultListView, wxSizerFlags(1).Align(wxALIGN_CENTER).Expand().Border(wxALL, 3) );
	
	m_pTopSizer->Add( m_pPropertySizer, wxSizerFlags(1).Align(wxALIGN_CENTER).Expand().Border(wxALL, 3) );
	m_pTopSizer->Add( m_pResultListSizer, wxSizerFlags(1).Align(wxALIGN_CENTER).Expand().Border(wxALL, 3) );
	m_pTopSizer->Add( m_pBasicBtnSizer, wxSizerFlags(0).Align(wxALIGN_RIGHT).Border(wxALL, 10) );

	SetSizer( m_pTopSizer );
	m_pTopSizer->SetSizeHints( this );
	m_pTopSizer->Fit( this );

	CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
	pMainFrame->ConnectMouseWheelEventToRenderPanel( m_pFilterPropGrid );

	for( int iEvent = EventInfo::CAMERA; iEvent < EventInfo::TYPE_COUNT; ++iEvent )
	{
		if( (EventInfo::SOUND_1 <= iEvent) && (iEvent <= EventInfo::SOUND_8) )
			m_mapEventIndex[ iEvent ] = INCLUDE_SOUND;
		else
		if( EventInfo::SOUND_8 < iEvent )
			m_mapEventIndex[ iEvent ] = iEvent+INCLUDE_CAMERA-(EventInfo::SOUND_8-EventInfo::SOUND_1);
		else
			m_mapEventIndex[ iEvent ] = INCLUDE_CAMERA+iEvent;
	}

	_ReflectFilter();
}



cwxBatchPropertyEditorDlg::~cwxBatchPropertyEditorDlg(void)
{
	m_pFilterPropGrid->Clear();
}



void cwxBatchPropertyEditorDlg::_InitFilterProperty( void )
{
	m_pFilterPropGrid = new wxPropertyGrid( this, FILTER_PROP_GRID_ID, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxEXPAND );

	_BuildFilterProperty();
}

void cwxBatchPropertyEditorDlg::_InitChangeValueProperty( void )
{
	m_pChangeValueGrid = new wxPropertyGrid( this, CHANGE_VALUE_GRID_ID, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxEXPAND );

	_BuildChangeValueProperty();
}

void cwxBatchPropertyEditorDlg::_BuildFilterProperty( void )
{
	m_pFilterPropGrid->AppendCategory( wxT( "Search Criteria" ) );
	m_aPGID[ STANDARD_TIME ] = m_pFilterPropGrid->Append( wxFloatProperty( wxT("Standard Time"), wxPG_LABEL, 0 ) );

	m_pFilterPropGrid->AppendCategory( wxT( "Action" ) );
	m_aPGID[ INCLUDE_ACTIONS ] = m_pFilterPropGrid->Append( wxBoolProperty( wxT("Include ACTION"), wxPG_LABEL, true ) );
	m_aPGID[ ACTION_PARENT_PROP ] = m_pFilterPropGrid->Append( wxParentProperty( wxT("ACTORS"), wxEmptyString ) );

	// ���� �ö� �ִ� ���͵��� ��������.
	int iNumActor = TOOL_DATA.GetNumActors();
	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		wxString strActorName;
		TOOL_DATA.GetActorNameByIndex( iActor, strActorName );

		wxPGId id = m_pFilterPropGrid->AppendIn( m_aPGID[ ACTION_PARENT_PROP ], wxBoolProperty( strActorName, wxPG_LABEL, true ) );
		m_pFilterPropGrid->SetPropertyAttribute( id, wxPG_BOOL_USE_CHECKBOX, true );
		m_vlActionActorNamePGID.push_back( id );
	}
	
	m_pFilterPropGrid->AppendCategory( wxT( "Key" ) );
	m_aPGID[ INCLUDE_KEYS ] = m_pFilterPropGrid->Append( wxBoolProperty( wxT("Include KEY"), wxPG_LABEL, true ) );
	m_aPGID[ KEY_PARENT_PROP ] = m_pFilterPropGrid->Append( wxParentProperty( wxT("KEYS"), wxEmptyString ) );

	for( int iActor = 0; iActor < iNumActor; ++iActor )
	{
		wxString strActorName;
		TOOL_DATA.GetActorNameByIndex( iActor, strActorName );

		wxPGId id = m_pFilterPropGrid->AppendIn( m_aPGID[ KEY_PARENT_PROP ], wxBoolProperty( strActorName, wxPG_LABEL, true ) );
		m_pFilterPropGrid->SetPropertyAttribute( id, wxPG_BOOL_USE_CHECKBOX, true );
		m_vlKeyActorNamePGID.push_back( id );
	}

	m_pFilterPropGrid->AppendCategory( wxT( "Event" ) );
	m_aPGID[ INCLUDE_EVENTS ] = m_pFilterPropGrid->Append( wxBoolProperty( wxT("Include EVENT"), wxPG_LABEL, true ) );
	m_aPGID[ EVENT_PARENT_PROP ] = m_pFilterPropGrid->Append( wxParentProperty( wxT("EVENTS"), wxEmptyString ) );
	m_aPGID[ INCLUDE_CAMERA ] = m_pFilterPropGrid->AppendIn( m_aPGID[ EVENT_PARENT_PROP ], wxBoolProperty( wxT("CAMERA"), wxPG_LABEL, true ) );
	m_aPGID[ INCLUDE_PARTICLE ] = m_pFilterPropGrid->AppendIn( m_aPGID[ EVENT_PARENT_PROP ], wxBoolProperty( wxT("PARTICLE"), wxPG_LABEL, true ) );
	m_aPGID[ INCLUDE_DOF ] = m_pFilterPropGrid->AppendIn( m_aPGID[ EVENT_PARENT_PROP ], wxBoolProperty( wxT("DOF"), wxPG_LABEL, true ) );
	m_aPGID[ INCLUDE_FADE ] = m_pFilterPropGrid->AppendIn( m_aPGID[ EVENT_PARENT_PROP ], wxBoolProperty( wxT("FADE"), wxPG_LABEL, true ) );
	m_aPGID[ INCLUDE_PROP ] = m_pFilterPropGrid->AppendIn( m_aPGID[ EVENT_PARENT_PROP ], wxBoolProperty( wxT("PROP"), wxPG_LABEL, true ) );
	m_aPGID[ INCLUDE_SOUND ] = m_pFilterPropGrid->AppendIn( m_aPGID[ EVENT_PARENT_PROP ], wxBoolProperty( wxT("SOUND"), wxPG_LABEL, true ) );
	m_aPGID[ INCLUDE_SUBTITLE ] = m_pFilterPropGrid->AppendIn( m_aPGID[ EVENT_PARENT_PROP ], wxBoolProperty( wxT("SUBTITLE"), wxPG_LABEL, true ) );
	m_aPGID[ INCLUDE_IMAGE ] = m_pFilterPropGrid->AppendIn( m_aPGID[ EVENT_PARENT_PROP ], wxBoolProperty( wxT("IMAGE(���������)"), wxPG_LABEL, true ) );

	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_ACTIONS ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_KEYS ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_EVENTS ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_CAMERA ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_PARTICLE ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_DOF ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_FADE ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_PROP ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_SOUND ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_SUBTITLE ], wxPG_BOOL_USE_CHECKBOX, true );
	m_pFilterPropGrid->SetPropertyAttribute( m_aPGID[ INCLUDE_IMAGE ], wxPG_BOOL_USE_CHECKBOX, true );
}


void cwxBatchPropertyEditorDlg::_BuildChangeValueProperty( void )
{
	m_pChangeValueGrid->AppendCategory( wxT("Value") );
	m_aPGID[ START_TIME_DELTA ] = m_pChangeValueGrid->Append( wxFloatProperty( wxT("StartTime Delta"), wxPG_LABEL, 0 ) );
}


void cwxBatchPropertyEditorDlg::_SetupResultListColumn( void )
{
	m_pResultListView->InsertColumn( 0, wxT("Name"), wxLIST_FORMAT_CENTRE, 200 );
	m_pResultListView->InsertColumn( 1, wxT("Type"), wxLIST_FORMAT_CENTRE, 70 );
	m_pResultListView->InsertColumn( 2, wxT("Start Time"), wxLIST_FORMAT_CENTRE, 100 );
	m_pResultListView->InsertColumn( 3, wxT("Length"), wxLIST_FORMAT_CENTRE, 100 );
}


void cwxBatchPropertyEditorDlg::_ReflectFilter( void )
{
	// ����Ʈ �� Ŭ����
	m_pResultListView->ClearAll();
	_SetupResultListColumn();

	m_vlpFilteredActionInfo.clear();
	m_vlpFilteredEventInfo.clear();
	m_vlpFilteredKeyInfo.clear();

	// ���� ����� ������ �������� �ٽ� ����Ʈ�� �ѷ��ش�.
	bool bIncludeActions = m_pFilterPropGrid->GetPropertyValueAsBool( m_aPGID[ INCLUDE_ACTIONS ] );

	// ���� �ð�
	float fStandardStartTime = (float)m_pFilterPropGrid->GetPropertyValueAsDouble( m_aPGID[ STANDARD_TIME ] );

	int iListViewOffset = 0;

	if( bIncludeActions )
	{
		//vector<wxString> vlstrFilteredActor;
		int iNumIncludeActors = (int)m_vlActionActorNamePGID.size();
		for( int iActor = 0; iActor < iNumIncludeActors; ++iActor )
		{
			const wxString& strActorName = m_pFilterPropGrid->GetPropertyName( m_vlActionActorNamePGID.at(iActor) );
			bool bAllowed = m_pFilterPropGrid->GetPropertyValueAsBool( m_vlActionActorNamePGID.at(iActor) );
			if( bAllowed )
			{
				//vlstrFilteredActor.push_back( strActorName );

				// ���͸� �� ��������.. ���� �ð� ������ �׼ǵ��� ã�Ƽ� ����Ʈ �信 ���
				int iItemCount = 0;
				int iNumActions = TOOL_DATA.GetThisActorsActionNum( strActorName.c_str() );
				for( int iAction = 0; iAction < iNumActions; ++iAction )
				{
					const ActionInfo* pActionInfo = TOOL_DATA.GetThisActorsActionInfoByIndex( strActorName.c_str(), iAction );

					float fStartTime = pActionInfo->fStartTime;
					if( fStandardStartTime <= fStartTime )
					{
						m_vlpFilteredActionInfo.push_back( pActionInfo );

						wxCSConv MBConv( wxFONTENCODING_CP949 );

						wxChar caWActionName[ 256 ];
						ZeroMemory( caWActionName, sizeof(caWActionName) );
						MBConv.MB2WC( caWActionName, pActionInfo->strActionName.c_str(), 256 );

						long iTemp = m_pResultListView->InsertItem( iItemCount, caWActionName );
						m_pResultListView->SetItemData( iTemp, iItemCount );

						m_pResultListView->SetItem( iItemCount, 1, wxT("Action") );
						
						wxChar caWBuf[ 128 ];
						ZeroMemory( caWBuf, sizeof(caWBuf) );
						wxSnprintf( caWBuf, 128, wxT("%2.2f sec"), fStartTime );
						m_pResultListView->SetItem( iItemCount, 2, caWBuf );

						wxSnprintf( caWBuf, 128, wxT("%2.2f sec"), pActionInfo->fTimeLength );
						m_pResultListView->SetItem( iItemCount, 3, caWBuf );

						++iListViewOffset;
						++iItemCount;
					}
				}
			}
		}
	}
	
	// ���� �ɼǵ� enable
	m_pFilterPropGrid->EnableProperty( m_aPGID[ ACTION_PARENT_PROP ], bIncludeActions );

	bool bIncludeKeys = m_pFilterPropGrid->GetPropertyValueAsBool( m_aPGID[ INCLUDE_KEYS ] );
	if( bIncludeKeys )
	{
		int iNumIncludeActors = (int)m_vlKeyActorNamePGID.size();
		int iNowListViewOffset = iListViewOffset;
		for( int iActor = 0; iActor < iNumIncludeActors; ++iActor )
		{
			const wxString& strActorName = m_pFilterPropGrid->GetPropertyName( m_vlKeyActorNamePGID.at(iActor) );
			bool bAllowed = m_pFilterPropGrid->GetPropertyValueAsBool( m_vlKeyActorNamePGID.at(iActor) );
			if( bAllowed )
			{
				int iItemCount = 0;
				int iNumKeys = TOOL_DATA.GetThisActorsKeyNum( strActorName.c_str() );
				for( int iKey = 0; iKey < iNumKeys; ++iKey )
				{
					const KeyInfo* pKeyInfo = TOOL_DATA.GetThisActorsKeyInfoByIndex( strActorName.c_str(), iKey );
					float fStartTime = pKeyInfo->fStartTime;
					if( fStandardStartTime <= fStartTime )
					{
						m_vlpFilteredKeyInfo.push_back( pKeyInfo );

						wxCSConv MBConv( wxFONTENCODING_CP949 );

						wxChar caWKeyName[ 256 ];
						ZeroMemory( caWKeyName, sizeof(caWKeyName) );
						MBConv.MB2WC( caWKeyName, pKeyInfo->strKeyName.c_str(), 256 );

						long iTemp = m_pResultListView->InsertItem( iNowListViewOffset+iItemCount, caWKeyName );
						m_pResultListView->SetItemData( iTemp, iNowListViewOffset+iItemCount );

						m_pResultListView->SetItem( iNowListViewOffset+iItemCount, 1, wxT("Key") );

						wxChar caWBuf[ 128 ];
						ZeroMemory( caWBuf, sizeof(caWBuf) );
						wxSnprintf( caWBuf, 128, wxT("%2.2f sec"), fStartTime );
						m_pResultListView->SetItem( iNowListViewOffset+iItemCount, 2, caWBuf );

						wxSnprintf( caWBuf, 128, wxT("%2.2f sec"), pKeyInfo->fTimeLength );
						m_pResultListView->SetItem( iNowListViewOffset+iItemCount, 3, caWBuf );

						++iListViewOffset;
						++iItemCount;
					}
				}
			}
		}
	}

	// ���� �ɼǵ� enable
	m_pFilterPropGrid->EnableProperty( m_aPGID[ KEY_PARENT_PROP ], bIncludeKeys );


	bool bIncludeEvents = m_pFilterPropGrid->GetPropertyValueAsBool( m_aPGID[ INCLUDE_EVENTS ] );
	if( bIncludeEvents )
	{
		bool bIncludeEventType[ PROP_COUNT - INCLUDE_CAMERA ];
		int iNowListViewOffset = iListViewOffset;
		for( int i = EventInfo::CAMERA; i < EventInfo::TYPE_COUNT; ++i )		// ���� �̺�Ʈ�� �߰��� 8���� �ִ�.
		{
			//int iIncludeEventTypesEnum = 0;
			//if( EventInfo::SOUND_1 <= i && i <= EventInfo::SOUND_8 )
			//	iIncludeEventTypesEnum = INCLUDE_SOUND;
			//else
			//if( EventInfo::TYPE_COUNT-1 == i )
			//	iIncludeEventTypesEnum = PROP_COUNT-1;
			//else
			//	iIncludeEventTypesEnum = i + INCLUDE_CAMERA; 
			int iIncludeEventTypesEnum = m_mapEventIndex[ i ];

			bIncludeEventType[ iIncludeEventTypesEnum-INCLUDE_CAMERA ] = m_pFilterPropGrid->GetPropertyValueAsBool( m_aPGID[ iIncludeEventTypesEnum ] );
			if( bIncludeEventType[ iIncludeEventTypesEnum-INCLUDE_CAMERA ] )
			{
				int iEventTypeOnData = i;
				//if( EventInfo::SOUND_2 == iEventTypeOnData )
				//	iEventTypeOnData = EventInfo::SUBTITLE;

				int iItemCount = 0;
				int iNumEvents = TOOL_DATA.GetThisTypesEventNum( iEventTypeOnData );
				for( int iEvent = 0; iEvent < iNumEvents; ++iEvent )
				{
					const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByIndex( iEventTypeOnData, iEvent );
					float fStartTime = pEventInfo->fStartTime;
					if( fStandardStartTime <= fStartTime )
					{
						m_vlpFilteredEventInfo.push_back( pEventInfo );

						wxCSConv MBConv( wxFONTENCODING_CP949 );

						wxChar caWEventName[ 256 ];
						ZeroMemory( caWEventName, sizeof(caWEventName) );
						MBConv.MB2WC( caWEventName, pEventInfo->strEventName.c_str(), 256 );

						long iTemp = m_pResultListView->InsertItem( iNowListViewOffset+iItemCount, caWEventName );
						m_pResultListView->SetItemData( iTemp, iNowListViewOffset+iItemCount );

						wxChar caWBuf[ 128 ];
						wxString strEventTypeName;
						ZeroMemory( caWBuf, sizeof(caWBuf) );

						switch( iEventTypeOnData )
						{
							case EventInfo::CAMERA:
								strEventTypeName.assign( wxT("Camera") );
								break;

							case EventInfo::PARTICLE:
								strEventTypeName.assign( wxT("Particle") );
								break;

							case EventInfo::DOF:
								strEventTypeName.assign( wxT("DOF") );
								break;

							case EventInfo::FADE:
								strEventTypeName.assign( wxT("Fade") );
								break;

							case EventInfo::PROP:
								strEventTypeName.assign( wxT("Prop") );
								break;

							case EventInfo::SOUND_1:
								strEventTypeName.assign( wxT("Sound_1") );
								break;

							case EventInfo::SOUND_2:
								strEventTypeName.assign( wxT("Sound_2") );
								break;

							case EventInfo::SOUND_3:
								strEventTypeName.assign( wxT("Sound_3") );
								break;

							case EventInfo::SOUND_4:
								strEventTypeName.assign( wxT("Sound_4") );
								break;

							case EventInfo::SOUND_5:
								strEventTypeName.assign( wxT("Sound_5") );
								break;

							case EventInfo::SOUND_6:
								strEventTypeName.assign( wxT("Sound_6") );
								break;

							case EventInfo::SOUND_7:
								strEventTypeName.assign( wxT("Sound_7") );
								break;

							case EventInfo::SOUND_8:
								strEventTypeName.assign( wxT("Sound_8") );
								break;

							case EventInfo::SUBTITLE:
								strEventTypeName.assign( wxT("Subtitle") );
								break;

							case EventInfo::IMAGE:
								strEventTypeName.assign( wxT("Image") );
								break;
						}
						wxSnprintf( caWBuf, 128, wxT("%s Event"), strEventTypeName.c_str() );
						m_pResultListView->SetItem( iNowListViewOffset+iItemCount, 1, caWBuf );

						wxSnprintf( caWBuf, 128, wxT("%2.2f sec"), fStartTime );
						m_pResultListView->SetItem( iNowListViewOffset+iItemCount, 2, caWBuf );

						wxSnprintf( caWBuf, 128, wxT("%2.2f sec"), pEventInfo->fTimeLength );
						m_pResultListView->SetItem( iNowListViewOffset+iItemCount, 3, caWBuf );

						++iItemCount;
					}
				}
			}
		}
	}

	// ���� �ɼǵ� enable
	m_pFilterPropGrid->EnableProperty( m_aPGID[ EVENT_PARENT_PROP ], bIncludeEvents );
}



void cwxBatchPropertyEditorDlg::OnFilterPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	_ReflectFilter();
}


void cwxBatchPropertyEditorDlg::OnChangeValuePropertyChanged( wxPropertyGridEvent& PGEvent )
{
	// �ð� ���氪
	m_fStartTimeDelta = (float)m_pChangeValueGrid->GetPropertyValueAsDouble( m_aPGID[ START_TIME_DELTA ] );
}


void cwxBatchPropertyEditorDlg::OnOK( wxCommandEvent& event )
{
	if( (!m_vlpFilteredActionInfo.empty() || !m_vlpFilteredKeyInfo.empty() || !m_vlpFilteredEventInfo.empty())
		&& m_fStartTimeDelta != 0.0f )
	{
		CBatchEdit BatchEdit( &TOOL_DATA, m_fStartTimeDelta, 
								m_vlpFilteredActionInfo, m_vlpFilteredKeyInfo, m_vlpFilteredEventInfo );
		TOOL_DATA.RunCommand( &BatchEdit );
	}

	Close();
}