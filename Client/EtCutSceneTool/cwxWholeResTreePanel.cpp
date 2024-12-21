#include "StdAfx.h"
#include <wx/wx.h>
#include "cwxWholeResTreePanel.h"
#include <wx/FileFn.h>
#include "ToolData.h"
#include "RTCutSceneMakerFrame.h"
#include "CommandSet.h"
#include "cwxAddActorDlg.h"
#include "EternityEngine.h"
#include "Timer.h"
#include "RTCutSceneRenderer.h"
#include "LuaDelegate.h"
#include "DnCutSceneActor.h"


//const wxChar* ROOT_RESOURCE_FOLDER = wxT("./Resource");
//const wxChar* ROOT_RESOURCE_FOLDER = wxT("D:/MyWork/RealTime Cut Scene Maker/Resource");



BEGIN_EVENT_TABLE( cwxWholeResTreePanel, wxPanel )
	EVT_BUTTON( BTN_REGISTER_RESOURCE_ID, cwxWholeResTreePanel::OnBtnRegisterRes )
	EVT_TREE_SEL_CHANGED( WHOLE_RESOURCE_TREE_ID, cwxWholeResTreePanel::OnTreeSelChanged )
END_EVENT_TABLE()


cwxWholeResTreePanel::cwxWholeResTreePanel( wxWindow* pParent, wxWindowID id ) : wxPanel( pParent, id ),
																				 m_pTopSizer( NULL ),
																				 m_pTreeCtrl( NULL ),
																				 m_pBtnRegister( NULL )
{
	m_pTopSizer = new wxBoxSizer( wxVERTICAL );

	m_pTreeCtrl = new wxTreeCtrl( this, WHOLE_RESOURCE_TREE_ID, wxDefaultPosition, wxDefaultSize );
	m_pTopSizer->Add( m_pTreeCtrl, 1, wxALL|wxALIGN_CENTER|wxEXPAND, 2 );

	m_pBtnRegister = new wxButton( this, BTN_REGISTER_RESOURCE_ID, wxString(wxT("Register to scene")), wxDefaultPosition, wxSize(200, 40) );
	m_pTopSizer->Add( m_pBtnRegister, 0, wxALL|wxALIGN_CENTER|wxEXPAND, 4 );

	m_TreeRootID = m_pTreeCtrl->AddRoot( wxT("Root") );

	//wxTreeItemIcon_Normal,              // not selected, not expanded
	//wxTreeItemIcon_Selected,            //     selected, not expanded
	//wxTreeItemIcon_Expanded,            // not selected,     expanded
	//wxTreeItemIcon_SelectedExpanded,    //     selected,     expanded
	wxImageList *pImageList = new wxImageList( 16, 16 );
	pImageList->Add( wxBITMAP(folder), wxColour(0xff00ff00) );
	pImageList->Add( wxBITMAP(folder_open), wxColour(0xff00ff00) );
	pImageList->Add( wxBITMAP(object), wxColour(0xff00ff00)  );
	pImageList->Add( wxBITMAP(object_select), wxColour(0xff00ff00) );
	m_pTreeCtrl->AssignImageList( pImageList );

	SetSizer( m_pTopSizer );
	m_pTopSizer->Fit( this );
	m_pTopSizer->SetSizeHints( this );

	_BuildResourceTree();

	// 휠 이벤트가 트리에게 먹지 않도록 한다.
	static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame())->ConnectMouseWheelEventToRenderPanel( m_pTreeCtrl );

	m_pTreeCtrl->Expand( m_TreeRootID );
}

cwxWholeResTreePanel::~cwxWholeResTreePanel(void)
{
	
}


void cwxWholeResTreePanel::_BuildResourceTree( void )
{
	deque<wxString> dqFolderToSearch;
	wxBusyCursor Busy;

	// 허락된 메인 폴더 및 확장자 이외에는 트리에 추가하지 않는다. (소문자 기준)
	m_setAllowMainFolder.insert( wxString(wxT("char")) );
	m_setAllowMainFolder.insert( wxString(wxT("mapdata")) );
	m_setAllowMainFolder.insert( wxString(wxT("sound")) );
	m_setAllowMainFolder.insert( wxString(wxT("particle")) );
	m_setAllowMainFolder.insert( wxString(wxT("weapon")) );

	m_setAllowFileExt.insert( wxString(wxT("act")) );
	m_setAllowFileExt.insert( wxString(wxT("skn")) );
	m_setAllowFileExt.insert( wxString(wxT("ani")) );
	m_setAllowFileExt.insert( wxString(wxT("mp3")) );
	m_setAllowFileExt.insert( wxString(wxT("wav")) );
	m_setAllowFileExt.insert( wxString(wxT("env")) );
	m_setAllowFileExt.insert( wxString(wxT("ptc")) );
	m_setAllowFileExt.insert( wxString(wxT("eff")) );

	const char* pResPath = LUA_DELEGATE.GetString( "resource_path" );
	const char* pMapPath = LUA_DELEGATE.GetString( "mapdata_path" );

	wxCSConv MBConv( wxFONTENCODING_CP949 );
	wxChar caWBuf[ 256 ];
	
	ZeroMemory( caWBuf, sizeof(caWBuf) );
	MBConv.MB2WC( caWBuf, pResPath, 256 );
	wxString strResourcePath( caWBuf );

	ZeroMemory( caWBuf, sizeof(caWBuf) );
	MBConv.MB2WC( caWBuf, pMapPath, 256 );
	wxString strMapDataPath( caWBuf );

	dqFolderToSearch.push_back( strResourcePath );
	dqFolderToSearch.push_back( strMapDataPath );

	while( !dqFolderToSearch.empty() )
	{
		wxString strNowSearchFolder = dqFolderToSearch.front();
		dqFolderToSearch.pop_front();

		strNowSearchFolder.append( wxT("/*.*") );

		wxString strFindedFilePath = wxFindFirstFile( strNowSearchFolder.c_str(), wxFILE|wxDIR );

		while( !strFindedFilePath.empty() )
		{
			// 폴더인 경우
			if( wxDirExists( strFindedFilePath.c_str() ) )
			{
				wxString strName;
				wxSplitPath( strFindedFilePath.c_str(), NULL, &strName, NULL );
				wxString strLower = strName.Lower();

				// 현재 폴더 이름이 무시 패턴과 부합하는가. 그렇다면 패스.
				set<wxString>::iterator iter = m_setIgnoreFolder.find( strLower );
				if( m_setIgnoreFolder.end() != iter )
				{
					strFindedFilePath = wxFindNextFile();
					continue;
				}
				
				dqFolderToSearch.push_back( strFindedFilePath );
			}
			else
			// 파일인 경우
			{
				wxString strCapitalName;
				wxString strExt;
				wxSplitPath( strFindedFilePath.c_str(), NULL, &strCapitalName, &strExt );
				wxString strFileName = strCapitalName+wxT(".")+strExt;

				wxString strLowerExt = strExt.Lower();
				set<wxString>::iterator iter = m_setAllowFileExt.find( strLowerExt );
				if( m_setAllowFileExt.end() != iter )
				{
					strFileName.MakeLower();
					TOOL_DATA.SetFullPath( strFileName.c_str(), strFindedFilePath.c_str() );

					// 액터들인 경우엔 대문자 이름만으로 키를 한 번 더 박아준다.
					if( *iter == wxT("act")  )
					{
						strCapitalName.MakeUpper();
						TOOL_DATA.SetFullPath( strCapitalName.c_str(), strFindedFilePath.c_str() );
					}
				}
			}

			strFindedFilePath = wxFindNextFile();
		}
	}

	m_mapTreeIdKind.insert( make_pair(m_TreeRootID, TREE_ROOT) );
	m_pTreeCtrl->SetItemImage( m_TreeRootID, 0, wxTreeItemIcon_Normal );
	m_pTreeCtrl->SetItemImage( m_TreeRootID, 1, wxTreeItemIcon_Expanded );

	// 실제로 EXT 파일 기준으로 트리에 추가한다. 
	// 중요! 반드시 파일 이름이 EXT 파일에 있는 이름과 같다는 전제가 있어야 나중에 풀 패스가 제대로 얻어진다.
	// 풀 패스는 폴더를 긁어서 갖고 있기 때문이다.
	wxTreeItemId ActorTreeId = m_pTreeCtrl->AppendItem( m_TreeRootID, wxT("Actor") );
	m_pTreeCtrl->SetItemImage( ActorTreeId, 0, wxTreeItemIcon_Normal );
	m_pTreeCtrl->SetItemImage( ActorTreeId, 1, wxTreeItemIcon_Expanded );
	m_mapTreeIdKind.insert( make_pair(ActorTreeId, TREE_ACTOR_SET) );
	int iNumActorRes = TOOL_DATA.GetNumActorResInfo();
	for( int iActor = 0; iActor < iNumActorRes; ++iActor )
	{
		const S_ACTOR_RES_INFO* pActorResInfo = TOOL_DATA.GetActorResInfoByIndex( iActor );
		wxTreeItemId id = m_pTreeCtrl->AppendItem( ActorTreeId, pActorResInfo->strActorName );
		m_pTreeCtrl->SetItemImage( id, 2, wxTreeItemIcon_Normal );
		m_pTreeCtrl->SetItemImage( id, 3, wxTreeItemIcon_Selected );
		TOOL_DATA.SetResourceType( pActorResInfo->strActorName.c_str(), CDnCutSceneData::RT_RES_ACTOR );
	}

	wxTreeItemId MapTreeId = m_pTreeCtrl->AppendItem( m_TreeRootID, wxT("Map") );
	m_pTreeCtrl->SetItemImage( MapTreeId, 0, wxTreeItemIcon_Normal );
	m_pTreeCtrl->SetItemImage( MapTreeId, 1, wxTreeItemIcon_Expanded );
	m_mapTreeIdKind.insert( make_pair(MapTreeId, TREE_MAP_SET) );
	int iNumMapRes = TOOL_DATA.GetNumMapResName();
	for( int iMap = 0; iMap < iNumMapRes; ++iMap )
	{
		wxTreeItemId id = m_pTreeCtrl->AppendItem( MapTreeId, TOOL_DATA.GetMapResNameByIndex( iMap ) );
		m_pTreeCtrl->SetItemImage( id, 2, wxTreeItemIcon_Normal );
		m_pTreeCtrl->SetItemImage( id, 3, wxTreeItemIcon_Selected );
		TOOL_DATA.SetResourceType( TOOL_DATA.GetMapResNameByIndex( iMap ), CDnCutSceneData::RT_RES_MAP );
	}

	m_pTreeCtrl->SortChildren( ActorTreeId );
	m_pTreeCtrl->SortChildren( MapTreeId );
}



void cwxWholeResTreePanel::OnBtnRegisterRes( wxCommandEvent& ButtonEvent )
{
	// 리소스 트리에서 선택된 문자열을 받아오자
	wxTreeItemId SelectedNode = m_pTreeCtrl->GetSelection();
	EtVector3 vPos;

	if( SelectedNode.IsOk() )
	{
		wxString strSelected = m_pTreeCtrl->GetItemText( SelectedNode );
		int iResKind = TOOL_DATA.GetResourceKind( strSelected.c_str() );

		wxString strFinalResName;

		switch( iResKind )
		{
			case CDnCutSceneData::RT_RES_MAP:
				{
					strFinalResName = strSelected;
					vPos.x = 0.0f; vPos.y = 0.0f; vPos.z = 0.0f;
				}
				break;

			case CDnCutSceneData::RT_RES_ACTOR:
				{
					// 이 컷신에서 미지원 액터라면 패스.
					DNTableFileFormat* pActorTable = TOOL_DATA.GetActorTable();
					DNTableFileFormat* pSox = NULL;
					const S_ACTOR_RES_INFO* pActorResInfo = TOOL_DATA.GetActorResInfoByName( strSelected );
					int iActorTypeInTable = pActorTable->GetFieldFromLablePtr( pActorResInfo->iTableID, "_Class" )->GetInteger();
					const SceneInfo* pSceneInfo = TOOL_DATA.GetSceneInfo();
					if( pSceneInfo->bAcademicNotSupported &&
						CDnCutSceneActor::ACADEMIC == iActorTypeInTable )
					{
						// 아카데믹 캐릭터는 지원하지 않음. 패스.
						wxMessageBox( wxT("아카데믹 캐릭터는 이 컷신에서 지원하지 않도록 셋팅되어있습니다. 확인해 주시기 바랍니다."), wxT("아카데믹 미지원"), 
							wxOK|wxCENTRE|wxICON_INFORMATION, TOOL_DATA.GetMainFrame() );
						return;
					}

					wxString strInputted;
					while( true )
					{
						// 이름에 공백이 있으면 안된다.
						if( wxString::npos != strSelected.find_first_of( wxT(" ") ) )
						{
							wxMessageBox( wxT("액터 이름에 공백이 있으면 안됩니다.\n액터 테이블의 _StaticName 의 이름을 변경해주세요."), wxT("Can't Register Actor!"), wxICON_ERROR | wxOK, this );
							return;
						}

						cwxAddActorDlg AddActorDlg( this, -1, wxT("Input actor name") );

						int iResult = AddActorDlg.ShowModal();
						if( wxID_OK == iResult )
						{
							AddActorDlg.GetInputtedString( strInputted );
							if( strInputted  != wxT("") )
							{
								strFinalResName.Printf( wxT("%s_%s"), strSelected, strInputted );

								// 이름에 공백이 있으면 안된다.
								if( wxString::npos != strFinalResName.find_first_of( wxT(" ") ) )
								{
									wxMessageBox( wxT("액터 이름에 공백이 있으면 안됩니다.\n다시 입력해 주세요."), wxT("Can't Register Actor!"), wxICON_ERROR | wxOK, this );
									continue;
								}

								// 중복된 이름이 아닌가 검사
								if( TOOL_DATA.IsRegResource(strFinalResName.c_str()) )
								{
									wxMessageBox( wxT("Actor name is duplicated."), wxT("Can't Register Actor!"), wxICON_ERROR | wxOK, this );
									continue;
								}

								CRTCutSceneRenderer* pRenderer = TOOL_DATA.GetRenderer();
								pRenderer->GetHeightPos( vPos );

								break;
							}
						}
						else
						if( wxID_CANCEL == iResult )
							return;
					}
				}	
				break;

			//case CDnCutSceneData::RT_RES_MAX_CAMERA:
			//	// 아직 카메라에선 딱히 처리할 것이 없다.
			//	strFinalResName = strSelected;
			//	break;
		}

		// for testing..
		// 일단 0, 0, 0 으로 하고 추후에 할 수 없이 렌더 패널에게 적합한 Y 위치를 얻어오는 방법밖에 없을듯.
		CRegisterResCmd Command( CToolData::GetInstancePtr(), strFinalResName.c_str(), iResKind, vPos, 0.0f );
		TOOL_DATA.RunCommand( &Command );
	}
}



void cwxWholeResTreePanel::_UpdateRegResourceBtn( void )
{
	wxTreeItemId SelectedNode = m_pTreeCtrl->GetSelection();

	if( SelectedNode.IsOk() )
	{
		wxString strSelected = m_pTreeCtrl->GetItemText( SelectedNode );

		bool bResult = !TOOL_DATA.IsRegResource( strSelected.c_str() ) && 
					   strSelected != wxT("Root") &&
					   strSelected != wxT("Actor") &&
					   strSelected != wxT("Map");

		m_pBtnRegister->Enable( bResult );
	}
}



void cwxWholeResTreePanel::OnTreeSelChanged( wxTreeEvent& TreeEvent )
{		
	_UpdateRegResourceBtn();

	wxTreeItemId id = TreeEvent.GetItem();
	map<wxTreeItemId, int>::iterator iter = m_mapTreeIdKind.find( id );
	if( m_mapTreeIdKind.end() != iter )
	{
		switch( iter->second )
		{
			case TREE_ROOT:
				break;

			case TREE_ACTOR_SET:
				break;

			case TREE_ACTOR:
				break;

			case TREE_MAP_SET:
				break;

			case TREE_MAP:
				break;
		}
	}
}



void cwxWholeResTreePanel::CommandPerformed( ICommand* pCommand )
{
	switch( pCommand->GetTypeID() )
	{
		case CMD_REGISTER_RES:
		case CMD_UNREGISTER_RES:
			_UpdateRegResourceBtn();
			break;
	}
}