#include "StdAfx.h"
#include <wx/wx.h>
#include "EternityEngine.h"
#include "RTCutSceneMakerFrame.h"
#include "cwxRenderPanel.h"
#include "IRenderBase.h"
#include "CommandSet.h"
#include "cwxTimeLinePanel.h"
#include "cwxTimeLineCtrl.h"
#include "cwxEventTimeLinePanel.h"
#include "ToolData.h"




BEGIN_EVENT_TABLE( cwxRenderPanel, wxPanel )
	EVT_SIZE( cwxRenderPanel::OnSize )
	EVT_IDLE( cwxRenderPanel::OnIdle )
	EVT_MOUSE_EVENTS( cwxRenderPanel::OnMouseEvent )
	//EVT_CHAR( cwxRenderPanel::OnKeyDown ) // bintitle.

	// 팝업 메뉴
	EVT_MENU( POPUP_SELECTED_ACTOR_MOVE, cwxRenderPanel::OnSelectedActorMove )
	EVT_MENU( POPUP_SELECTED_ACTOR_ROTATE, cwxRenderPanel::OnSelectedActorRotate )

	EVT_MENU( POPUP_SELECTED_PARTICLE_MOVE, cwxRenderPanel::OnSelectedParticleMove )
	EVT_MENU( POPUP_SELECTED_PARTICLE_ROTATE, cwxRenderPanel::OnSelectedParticleRotate )
	EVT_MENU( POPUP_SELECTED_PARTICLE_UNSELECT, cwxRenderPanel::OnSelectedParticleUnSelect )

END_EVENT_TABLE()


cwxRenderPanel::cwxRenderPanel( wxWindow* pParent, wxWindowID id ) : wxPanel( pParent, id ),
																	 m_PrevFrameTime( timeGetTime() ),
																	 m_pActionTimeLineCtrl( NULL ),
																	 m_LocalTime( 0 ),
																	 m_bPauseScene( false ),
																	 m_pTimeLinePanel( NULL ),
																	 m_iNowMode( RP_EM_COUNT ),
																	 m_bActorMove( false )
																	 /*m_PlayStartTime( 0 )*/
{
	m_pRenderer = IRenderBase::Create( (HWND)GetHandle() );
}


cwxRenderPanel::~cwxRenderPanel(void)

{	
	delete m_pRenderer;
}



void cwxRenderPanel::SetTimeLinePanel( cwxTimeLinePanel* pTimeLinePanel )
{
	m_pTimeLinePanel = pTimeLinePanel;
}



void cwxRenderPanel::OnSize( wxSizeEvent& SizeEvent )
{
	m_pRenderer->OnResize( SizeEvent.GetSize().GetWidth(), SizeEvent.GetSize().GetHeight() );
}


// 프레임 업데이트 및 렌더링을 위한 아이들 이벤트
void cwxRenderPanel::OnIdle( wxIdleEvent& IdleEvent )
{
	LOCAL_TIME NowTime = timeGetTime();
	LOCAL_TIME FrameTimeDelta = NowTime - m_PrevFrameTime;
	float fDelta = float(FrameTimeDelta) / 1000.0f;
	//float fDelta = 0.0f;

	// 액션 타임 라인 컨트롤의 단위는 초 단위임.
	// 이벤트 타임 라인 컨트롤의 단위는 10ms 단위
	switch( TOOL_DATA.GetEditMode() )
	{
		case CToolData::EM_PLAY:
			{
				if( false == m_bPauseScene )
					m_LocalTime += FrameTimeDelta;

				float fAbsoluteElapsedTime = (float)m_LocalTime * 0.001f;
				m_pActionTimeLineCtrl = TOOL_DATA.GetActionTimeLine();
				m_pActionTimeLineCtrl->UpdateTimeBar( fAbsoluteElapsedTime );

				TOOL_DATA.GetEventTLPanel()->SetTime( fAbsoluteElapsedTime );
				//TOOL_DATA.GetEventTLPanel()->ReflectData();
			}
			break;

		case CToolData::EM_FREE_TIME:
			{
				// 어떤 타임 라인을 컨트롤 했는지가 관건!
				// 단위는 DWORD 로 ms 단위.
				//m_LocalTime = m_pTimeLinePanel->GetNowTime();
				m_LocalTime = TOOL_DATA.GetLocalTime();
				m_pTimeLinePanel->SyncTimePanels( m_LocalTime );				// 타임 라인 패널들의 시간 축을 동조시킨다.

				//m_pActionTimeLineCtrl = TOOL_DATA.GetActionTimeLine();
				//float fNowTLAxis = m_pActionTimeLineCtrl->GetNowTimeByUnit();
				//fNowTLAxis *= 1000.0f;
				//m_LocalTime = /*m_PlayStartTime +*/ DWORD(fNowTLAxis);
			}
			break;
	}
	
	m_pRenderer->OnRender( m_LocalTime, fDelta );

	m_PrevFrameTime = NowTime;

	IdleEvent.RequestMore();
}


// 활성화 비활성화 여부
void cwxRenderPanel::OnActivate( wxActivateEvent& ActiveEvent )
{
	m_pRenderer->SetActivate( ActiveEvent.GetActive() );
}


void cwxRenderPanel::StartPlay( void )
{
	if( false == m_bPauseScene )
	{
		m_pRenderer->SetMode( IRenderBase::PLAY_MODE );
		//m_PlayStartTime = timeGetTime();
		//m_LocalTime = m_PlayStartTime;
		m_LocalTime = 0;
	}
	else
	{
		m_bPauseScene = false;
	}
}


void cwxRenderPanel::PausePlay( void )
{
	m_bPauseScene = true;
}


void cwxRenderPanel::StopPlay( void )
{
	m_pRenderer->SetMode( IRenderBase::EDITOR_MODE );

	m_LocalTime = 0;
	m_bPauseScene = false;
	m_pActionTimeLineCtrl = TOOL_DATA.GetActionTimeLine();
	m_pActionTimeLineCtrl->UpdateTimeBar( 0.0f );

	cwxEventTimeLinePanel* pEventTLPanel = TOOL_DATA.GetEventTLPanel();
	pEventTLPanel->SetTime( 0.0f );
	pEventTLPanel->ReflectData();
}


void cwxRenderPanel::ResetPlay( void )
{
	m_LocalTime = 0;
	m_pActionTimeLineCtrl = TOOL_DATA.GetActionTimeLine();
	m_pActionTimeLineCtrl->UpdateTimeBar( 0.0f );

	cwxEventTimeLinePanel* pEventTLPanel = TOOL_DATA.GetEventTLPanel();
	pEventTLPanel->SetTime( 0.0f );
	pEventTLPanel->ReflectData();
}


void cwxRenderPanel::SeeThis( const wxChar* pActorName )
{
	m_pRenderer->SeeThisActor( pActorName );
}

// bintitle.
void cwxRenderPanel::OnKeyDown(wxKeyEvent& event)
{
	m_pRenderer->OnKeyDown( event );
}

void cwxRenderPanel::OnKeyUp(wxKeyEvent& event)
{
	m_pRenderer->OnKeyUp( event );
}


// 마우스 클릭
void cwxRenderPanel::OnMouseEvent( wxMouseEvent& MouseEvent )
{
	wxPoint Pos = MouseEvent.GetPosition();

	if( MouseEvent.LeftDown() )
	{
		m_pRenderer->OnLButtonDown( Pos.x, Pos.y );

		if( m_pRenderer->GetSelectedObjectName() && m_pRenderer->GetSelectedObjectType() == IRenderBase::SO_ACTOR )
		{
			m_iNowMode = RP_EM_ACTOR_MOVE;
			m_PrevMousePoint = MouseEvent.GetPosition();
		}
	}
	else
	if( MouseEvent.LeftUp() )
	{
		m_pRenderer->OnLButtonUp( Pos.x, Pos.y );
	}
	else if( MouseEvent.RightUp() )
		m_pRenderer->OnRButtonUp( Pos.x, Pos.y );
	else
	if( MouseEvent.RightDown() )
	{
		m_pRenderer->OnRButtonDown( Pos.x, Pos.y );

		if( m_pRenderer->IsObjectSelected() && !m_pRenderer->bIsLockSelect() )
		{
			wxPoint Point = MouseEvent.GetPosition();

			//if( -1 == Point.x && -1 == Point.y )
			//{
			//	wxSize Size = GetSize();
			//	Point.x = Size.x / 2;
			//	Point.y = Size.y / 2;
			//}
			//else
			//{
			//	Point = ScreenToClient( Point );
			//}
			
			// AUI 의 다른 패널들이 들러붙고 남은 영역이 렌더링 패널이 되기 때문에 
			// 정확하게 위치를 다시 더해줘야 한다.
			//Point += GetPosition();

			// 툴 바 Height 역시 적용시켜야 함... 알아오는 걸 모르겠... 그냥 수치로. -_-
			//Point.y += 87;
			//CRTCutSceneMakerFrame* pMainFrame = static_cast<CRTCutSceneMakerFrame*>(TOOL_DATA.GetMainFrame());
			//Point.y += pMainFrame->GetMenuBar()->GetSize().y;
			//Point.y += pMainFrame->GetToolBar()->GetToolSize().y;

			_PopupContextMenu( Point.x, Point.y );
		}
	}
	else
	if( MouseEvent.MiddleDown() )
	{
		m_pRenderer->OnCButtonDown( Pos.x, Pos.y );
	}
	else
	if( MouseEvent.Dragging() )
	{
		if( MouseEvent.LeftIsDown() )
		{
			//if( RP_EM_COUNT == m_iNowMode )
				m_pRenderer->OnLButtonDrag( Pos.x, Pos.y );
		}
		else
		if( MouseEvent.RightIsDown() )
			m_pRenderer->OnRButtonDrag( Pos.x, Pos.y );
		else
		if( MouseEvent.MiddleIsDown() )
			m_pRenderer->OnCButtonDrag( Pos.x, Pos.y );
	}
	else
	if( MouseEvent.GetWheelRotation() != 0 )
	{
		if( RP_EM_COUNT == m_iNowMode )
			m_pRenderer->OnMouseWheel( MouseEvent.GetWheelRotation() );
	}

	_ProcessSelectedObjectEdit( MouseEvent );
}



void cwxRenderPanel::_ProcessSelectedObjectEdit( wxMouseEvent& MouseEvent )
{
	// 이동은 Edit Move 안눌러도 되도록 하자.
	// 차라리 Move Lock 옵션을 걸도록 하자. 그리고 전체적으로 마우스 입력받고 하는 부분 정리해야하지 않나. 
	// 너무 엉망이네 ㅠㅠ
	switch( m_iNowMode )
	{
		case RP_EM_ACTOR_MOVE:
			{
				if( MouseEvent.Dragging() )
				{
					if( MouseEvent.LeftIsDown() )
					{
						// 마우스 위치를 알려주고 그 위치로 projection 되도록 렌더러에서 처리
						// 마우스 델타는 렌더러 내부에서 처리하도록
						wxPoint NowPos = MouseEvent.GetPosition();
						if( 3 < abs( NowPos.x - m_PrevMousePoint.x ) || 3 < abs( NowPos.y - m_PrevMousePoint.y ) )
						{
							m_bActorMove = m_pRenderer->MoveObjectByDragging( m_pRenderer->GetSelectedObjectName(), 
																			  MouseEvent.GetPosition().x, MouseEvent.GetPosition().y );
						}
					}
				}

				if( MouseEvent.LeftUp() )
				{
					m_iNowMode = RP_EM_COUNT;
					const wchar_t* pActorName = m_pRenderer->GetSelectedObjectName();
					int iObjectType = m_pRenderer->GetSelectedObjectType();
					//m_pRenderer->SetActorAlpha( pActorName, 1.0f );
					//EtVector3 vNewPos = *(m_pRenderer->GetSelectedObjectPos( /*m_pRenderer->GetSelectedObjectName()*/ ));

					if( pActorName && iObjectType == IRenderBase::SO_ACTOR && m_bActorMove )
					{
						EtVector3 vNewPos;
						m_pRenderer->GetSelectedObjectPos( &vNewPos );

						CActorResPropChange ActorPropChange( &TOOL_DATA, pActorName, pActorName, vNewPos,
															 TOOL_DATA.GetRegResRot(pActorName), 
															 TOOL_DATA.GetThisActorsFitYPosToMap(pActorName),
															 TOOL_DATA.GetThisActorsInfluenceLightmap(pActorName),
															 TOOL_DATA.GetMonsterTableID(pActorName),
															 TOOL_DATA.GetThisActorsScaleLock(pActorName) );

						TOOL_DATA.RunCommand( &ActorPropChange );
						m_bActorMove = false;
					}
				}
			}
			break;

		case RP_EM_ACTOR_ROTATE:
			{
				// 회전 조절은 휠을 굴려서 하자.
				int iWheelDelta = MouseEvent.GetWheelRotation();
				if( 0 != iWheelDelta )
				{
					m_strRotateActorName.assign( m_pRenderer->GetSelectedObjectName() );
					m_pRenderer->RotateObjectByWheelDelta( iWheelDelta );
				}
				else
				if( MouseEvent.LeftUp() )
				{
					m_iNowMode = RP_EM_COUNT;
					//const wchar_t* pActorName = m_pRenderer->GetSelectedObjectName();

					if( !m_strRotateActorName.empty() )
					{
						m_pRenderer->SetActorAlpha( m_strRotateActorName.c_str(), 1.0f );
						float fRotDegree = m_pRenderer->GetRenderActorRotDegree( m_strRotateActorName.c_str() );

						CActorResPropChange ActorPropChange( &TOOL_DATA, m_strRotateActorName.c_str(), m_strRotateActorName.c_str(), 
															 TOOL_DATA.GetRegResPos(m_strRotateActorName.c_str()),
															 fRotDegree, 
															 TOOL_DATA.GetThisActorsFitYPosToMap(m_strRotateActorName.c_str()),
															 TOOL_DATA.GetThisActorsInfluenceLightmap(m_strRotateActorName.c_str()),
															 TOOL_DATA.GetMonsterTableID(m_strRotateActorName.c_str()),
															 TOOL_DATA.GetThisActorsScaleLock(m_strRotateActorName.c_str()) );
						TOOL_DATA.RunCommand( &ActorPropChange );
					}
				}
			}
			break;

		case RP_EM_PARTICLE_MOVE:
			{
				if( MouseEvent.Dragging() )
				{
					if( MouseEvent.LeftIsDown() )
					{
						// 마우스 위치를 알려주고 그 위치로 projection 되도록 렌더러에서 처리
						// 마우스 델타는 렌더러 내부에서 처리하도록
						m_pRenderer->MoveObjectByDragging( m_pRenderer->GetSelectedObjectName(), 
														   MouseEvent.GetPosition().x, MouseEvent.GetPosition().y );
					}
				}

				if( MouseEvent.LeftUp() )
				{
					m_iNowMode = RP_EM_COUNT;
					const wchar_t* pActorName = m_pRenderer->GetSelectedObjectName();
					EtVector3 vNewPos;
					m_pRenderer->GetSelectedObjectPos( &vNewPos );

					const EventInfo* pEventInfo = TOOL_DATA.GetEventInfoByID( m_pRenderer->GetSelectedObjectID() );
					if( pEventInfo )
					{
						ParticleEventInfo* pParticleEventInfo = static_cast<ParticleEventInfo*>(pEventInfo->clone());
						pParticleEventInfo->vPos = vNewPos;

						CEventPropChange EventPropChange( &TOOL_DATA, pParticleEventInfo );
						TOOL_DATA.RunCommand( &EventPropChange );

						delete pParticleEventInfo;
					}
				}
			}
			break;

		case RP_EM_PARTICLE_ROTATE:
			break;
	}
}




void cwxRenderPanel::_PopupContextMenu( int iXPos, int iYPos )
{
	wxMenu Menu;

	// 편집 모드일 때.
	switch( TOOL_DATA.GetEditMode() )
	{
		case CToolData::EM_EDIT:
			{
				wxString strSelectedObjectName = m_pRenderer->GetSelectedObjectName();
				strSelectedObjectName.append( wxT(" Edit") );
				Menu.SetTitle( strSelectedObjectName.c_str() );
			
				switch( m_pRenderer->GetSelectedObjectType() )
				{
					case IRenderBase::SO_ACTOR:
						{
							//Menu.Append( POPUP_SELECTED_ACTOR_MOVE, wxT("&Move") );
							Menu.Append( POPUP_SELECTED_ACTOR_ROTATE, wxT("&Rotate") );
						}
						break;

					case IRenderBase::SO_PARTICLE:
						{
							Menu.Append( POPUP_SELECTED_PARTICLE_MOVE, wxT("&Move") );
							//Menu.Append( POPUP_SELECTED_PARTICLE_ROTATE, wxT("&Rotate") );
						}
						break;
				}
			}
			break;
	
		case CToolData::EM_PLAY:
			break;

		case CToolData::EM_FREE_TIME:
			break;
	}

	PopupMenu( &Menu, iXPos, iYPos );
}







void cwxRenderPanel::OnSelectedActorMove( wxCommandEvent& MenuEvent )
{
	m_iNowMode = RP_EM_ACTOR_MOVE;

	m_pRenderer->SetActorAlpha( m_pRenderer->GetSelectedObjectName(), 0.5f );
}




void cwxRenderPanel::OnSelectedActorRotate( wxCommandEvent& MenuEvent )
{
	m_iNowMode = RP_EM_ACTOR_ROTATE;

	m_pRenderer->SetActorAlpha( m_pRenderer->GetSelectedObjectName(), 0.5f );

	//m_pRenderer->ShowActorRotate(  );

	//m_pRenderer->SetActorAlpha( m_pRenderer );
}



void cwxRenderPanel::OnSelectedParticleMove( wxCommandEvent& MenuEvent )
{
	m_iNowMode = RP_EM_PARTICLE_MOVE;
}


void cwxRenderPanel::OnSelectedParticleRotate( wxCommandEvent& MenuEvent )
{
	m_iNowMode = RP_EM_PARTICLE_ROTATE;
}


void cwxRenderPanel::OnSelectedParticleUnSelect( wxCommandEvent& MenuEvent )
{
	m_iNowMode = RP_EM_COUNT;
	m_pRenderer->UnSelectObject();
}



void cwxRenderPanel::ToggleCamMode( bool bEditCameraOnPlay )
{
	m_pRenderer->ToggleCamMode( bEditCameraOnPlay );
}



void cwxRenderPanel::ToggleShowSpotPos( bool bShowSpotPos )
{
	m_pRenderer->ToggleShowSpotPos( bShowSpotPos );
}


void cwxRenderPanel::ToggleRenderFog( bool bRenderFog )
{
	m_pRenderer->SetRenderFog( bRenderFog );
}


void cwxRenderPanel::ToggleLetterBox( bool bToggleLetterBox )
{
	m_pRenderer->ToggleLetterBox( bToggleLetterBox );
}


void cwxRenderPanel::ToggleWeapon( bool bToggleWeapon ) // bintitle.
{
	m_pRenderer->ToggleWeapon( bToggleWeapon );
}

void cwxRenderPanel::CommandPerformed( ICommand* pCommand )
{
	switch( pCommand->GetTypeID() )
	{
		case CMD_UPDATE_VIEW:
		case CMD_REGISTER_RES:
		case CMD_UNREGISTER_RES:
		case CMD_ACTOR_PROP_CHANGE:
		case CMD_ACTION_PROP_CHANGE:
			m_pRenderer->UpdateRenderObject();
			break;

		case CMD_CHANGE_ACTOR_RES:
			{
				CChangeActorRes* pActorResChange = static_cast<CChangeActorRes*>( pCommand );
				if( pCommand->IsUndo() )
					m_pRenderer->ChangeActor( pActorResChange->GetNewActorName(), pActorResChange->GetOldActorResName() );
				else
					m_pRenderer->ChangeActor( pActorResChange->GetOldActorName(), pActorResChange->GetNewActorResName() );
			}
			break;

		case CMD_EVENT_PROP_CHANGE:
			{
				m_pRenderer->UpdateSelectedObject();

				//CEventPropChange* pEventPropChange = static_cast<CEventPropChange*>(pCommand);
				//
				//if( false == pEventPropChange->m_bUndo )
				//{
				//	if( m_pRenderer->GetSelectedObjectID() == pEventPropChange->m_pNewEventInfo->iID )
				//		m_pRenderer->UpdateSelectedObject();
				//}
				//else
				//{

				//}
			}
			break;

		case CMD_REMOVE_EVENT:
			{
				CEventRemoveCmd* pEventRemove = static_cast<CEventRemoveCmd*>(pCommand);
				if( m_pRenderer->GetSelectedObjectID() == pEventRemove->m_iEventIDToRemove )
					m_pRenderer->UnSelectObject();
			}
			break;
	}
}