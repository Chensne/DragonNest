#include "StdAfx.h"
#include <wx/wx.h>
#include "cwxTimeLineCtrl.h"

//IMPLEMENT_DYNAMIC_CLASS( cwxTimeLineCtrl, wxControl );

const int DEFAULT_ITEM_VERTICAL_LINE = 150;
const int DEFAULT_TIME_LINE_WIDTH = 300;
const int DEFAULT_TIME_LINE_HEIGHT = 200;
const float DEFAULT_PIXEL_STEP = 10.0f;
const int SCROLL_PIXEL_PER_UNIT_X = 10;
const int SCROLL_PIXEL_PER_UNIT_Y = 10;
const int DEFAULT_FONT_SIZE = 7;
const int HORIZON_RULER_LINE_Y_OFFSET = 15;
const int HORIZON_RULER_LONG_GRADUATE_SIZE = 10;		// 자의 긴 눈금 사이즈
const int HORIZON_RULER_SHORT_GRADUATE_SIZE = 5;
const int DEFAULT_TIME_BAR_BOX_WIDTH = 50;
const int DEFAULT_TIME_BAR_BOX_HEIGHT = 16;
const int DEFAULT_OBJECT_BOX_HEIGHT = 20;



BEGIN_EVENT_TABLE( cwxTimeLineCtrl, wxScrolledWindow )
	EVT_PAINT( cwxTimeLineCtrl::OnPaint )
	EVT_SCROLLWIN( cwxTimeLineCtrl::OnScroll )
	EVT_SIZE( cwxTimeLineCtrl::OnResize )
	EVT_MOUSE_EVENTS( cwxTimeLineCtrl::OnMouseEvent )
END_EVENT_TABLE()


cwxTimeLineCtrl::cwxTimeLineCtrl( void )
{
	SetBackgroundColour(wxColour(wxT("WHITE")));
}


cwxTimeLineCtrl::cwxTimeLineCtrl( wxWindow* pParent, wxWindowID id ) : wxScrolledWindow( pParent, id, wxDefaultPosition, wxDefaultSize,
																						 wxSUNKEN_BORDER |
																						 wxNO_FULL_REPAINT_ON_RESIZE |
																						 wxVSCROLL | wxHSCROLL ),
																					   m_dwWidth( DEFAULT_TIME_LINE_WIDTH ),
																					   m_dwHeight( DEFAULT_TIME_LINE_HEIGHT ),
																					   m_fOriRulerPixelStep( DEFAULT_PIXEL_STEP ),
																					   m_fViewSize( 1.0f ),
																					   m_dwItemVerticalOffset( DEFAULT_ITEM_VERTICAL_LINE ),
																					   m_pSelectedObjectInfo( NULL ),
																					   m_iLastClkXPos( 0 ),
																					   m_bIsLBtnDown( false ),
																					   m_iSelectedObjectPart( OBJECT_NOT_SELECTED ),
																					   m_fScale( 1.0f ),
																					   //m_dwNowTimeBarPosByUnit( 0 ),
																					   m_fNowTimeBarPosByPixel( 0 ),
																					   m_bTimeBarClked( false ),
																					   m_bShowTimeLineAsFloat( false ),
																					   m_bEnable( true ),
																					   m_dwObjectDrawStyle( TL_SHOW_OBJECT_TIME_LENGTH | TL_SHOW_OBJECT_NAME ),
																					   m_dwLastPaintTime( timeGetTime() ),
																					   m_bMoved( false ),
																					   m_iOnOverYLineIndex( 0 ),
																					   m_fMousePosTime( 0.0f ),
																					   m_iXScrollOffset( 0 ),
																					   m_iYScrollOffset( 0 ),
																					   m_bNeedUpdateOffset( false )
{
	SetBackgroundStyle( wxBG_STYLE_CUSTOM );
	SetBackgroundColour(wxColour(wxT("WHITE")));

	// 스크롤 바 계산
	//_UpdateScrollBar();

	// for testing..
	//SetViewScale( 1.5f );
	_UpdateViewScale();

	// 유닛 사이즈를 지정하는 함수는 액션이 추가되기 전에 반드시 먼저 호출되어야 함.
	//SetScale( 10 );

	//AddActorObject( wxString(wxT("Test Object")), wxColour(wxT("BLUE")) );
	//AddObject( wxString(wxT("Test Object")), wxString(wxT("Some Object")), 10 );
	//AddObject( wxString(wxT("Test Object")), wxString(wxT("Some Object1")), 2 );

	//DelObject( wxString(wxT("Test Object")), wxString(wxT("Some Object")) );

	//AddActorObject( wxString(wxT("Test Object 1")), wxColour(wxT("BLUE")) );
	//AddObject( wxString(wxT("Test Object 1")), wxString(wxT("Some Object2")), 13 );

	//AddActorObject( wxString(wxT("Test Object 2")), wxColour(wxT("BLUE")) );
	//AddObject( wxString(wxT("Test Object 2")), wxString(wxT("Some Object3")), 5 );

	//AddActorObject( wxString(wxT("Test Object 3")), wxColour(wxT("BLUE")) );
	//AddObject( wxString(wxT("Test Object 3")), wxString(wxT("Some Object4")), 20 );

	//UpdateTimeBar( 8 );
}

cwxTimeLineCtrl::~cwxTimeLineCtrl(void)
{
	for_each( m_vlpItemInfo.begin(), m_vlpItemInfo.end(), 
			  DeleteData<S_ITEM_INFO*>() );
}



void cwxTimeLineCtrl::SetStyle( DWORD dwStyle )
{
	m_dwObjectDrawStyle = dwStyle;
}



void cwxTimeLineCtrl::Clear( void )
{
	m_dwWidth = DEFAULT_TIME_LINE_WIDTH;
	m_dwHeight = DEFAULT_TIME_LINE_HEIGHT;
	//m_dwRulerPixelStep = DEFAULT_PIXEL_STEP;
	//m_fViewSize = 1.0f;
	m_dwItemVerticalOffset = DEFAULT_ITEM_VERTICAL_LINE;
	m_pSelectedObjectInfo = NULL;
	m_iLastClkXPos = 0;
	m_bIsLBtnDown = false;
	m_iSelectedObjectPart = OBJECT_NOT_SELECTED;
	m_fScale = 1.0f;
	//m_dwNowTimeBarPosByUnit = 0;
	m_fNowTimeBarPosByPixel = 0.0f;
	m_bTimeBarClked = false;
	m_bShowTimeLineAsFloat = false;
	m_strUnitName.clear();

	for_each( m_vlpItemInfo.begin(), m_vlpItemInfo.end(), 
			  DeleteData<S_ITEM_INFO*>() );

	m_mapItemInfo.clear();
	m_vlpItemInfo.clear();
	m_vlpObjects.clear();
}


bool cwxTimeLineCtrl::Enable(bool bEnable /* = true  */)
{
	m_bEnable = bEnable;

	return true;
}


void cwxTimeLineCtrl::OnResize( wxSizeEvent& SizeEvent )
{
	_UpdateScrollBar();
}



float cwxTimeLineCtrl::GetNextObjectStartTime( const wxChar* pActorName )
{
	float fResult = 0.0f;

	map<wxString, S_ITEM_INFO*>::iterator iter = m_mapItemInfo.find( wxString(pActorName) );
	if( m_mapItemInfo.end() != iter )
	{
		vector<S_OBJECT_INFO*>& vlObjects = iter->second->m_vlObjectInfo;
		int iNumObject = (int)vlObjects.size();
		for( int iObject = 0; iObject < iNumObject; ++iObject )
		{
			S_OBJECT_INFO* pObjectInfo = vlObjects.at( iObject );
			
			if( fResult < pObjectInfo->fEndTime ) 
				fResult = pObjectInfo->fEndTime;
		}
	}

	return fResult;
}


float cwxTimeLineCtrl::GetPrevObjectEndTime( const wxChar* pActorName, int iID )
{
	float fResult = 0.0f;

	map<wxString, S_ITEM_INFO*>::iterator iter = m_mapItemInfo.find( wxString(pActorName) );
	if( m_mapItemInfo.end() != iter )
	{
		vector<S_OBJECT_INFO*>& vlObjects = iter->second->m_vlObjectInfo;
		S_OBJECT_INFO* pDestObject = NULL;
		int iNumObject = (int)vlObjects.size();
		for( int iObject = 0; iObject < iNumObject; ++iObject )
		{
			S_OBJECT_INFO* pObjectInfo = vlObjects.at( iObject );
			if( pObjectInfo->iID == iID )
			{
				pDestObject = pObjectInfo;
				break;
			}
		}

		if( pDestObject )
		{
			float fNearestEndTime = 0.0f;
			float fNearestDistance = 999999999.0f;
			for( int iObject = 0; iObject < iNumObject; ++iObject )
			{
				S_OBJECT_INFO* pNowObject = vlObjects.at( iObject );
				if( pDestObject != pNowObject )
				{
					float fDistance = pDestObject->fEndTime - pNowObject->fEndTime;
					if( fDistance > 0.0f && fDistance < fNearestDistance )
					{
						fNearestEndTime = pNowObject->fEndTime;
						fNearestDistance = fDistance;
					}
				}
			}

			fResult = fNearestEndTime;
		}
	}
	
	return fResult;
}




int cwxTimeLineCtrl::GetPrevObjectID( const wxChar* pActorName, int iID )
{
	int iFindedID = -1;

	map<wxString, S_ITEM_INFO*>::iterator iter = m_mapItemInfo.find( wxString(pActorName) );
	if( m_mapItemInfo.end() != iter )
	{
		vector<S_OBJECT_INFO*>& vlObjects = iter->second->m_vlObjectInfo;
		S_OBJECT_INFO* pDestObject = NULL;
		int iNumObject = (int)vlObjects.size();
		for( int iObject = 0; iObject < iNumObject; ++iObject )
		{
			S_OBJECT_INFO* pObjectInfo = vlObjects.at( iObject );
			if( pObjectInfo->iID == iID )
			{
				pDestObject = pObjectInfo;
				break;
			}
		}

		if( pDestObject )
		{
			int iNearestDistActID = -1;
			float fNearestDistance = 999999999.0f;
			for( int iObject = 0; iObject < iNumObject; ++iObject )
			{
				S_OBJECT_INFO* pNowObject = vlObjects.at( iObject );
				if( pDestObject != pNowObject )
				{
					float fDistance = pDestObject->fEndTime - pNowObject->fEndTime;
					if( fDistance > 0.0f && fDistance < fNearestDistance )
					{
						iNearestDistActID = pNowObject->iID;
						fNearestDistance = fDistance;
					}
				}
			}

			iFindedID = iNearestDistActID;
		}
	}

	return iFindedID;
}




// 전체 타임 시퀀스의 길이가 달라지면 이 함수가 호출되어 업데이트 되어야 합니다.
void cwxTimeLineCtrl::_UpdateScrollBar( void )
{
	int iClientWidth = 0;
	int iClientHeight = 0;
	
	// 현재 너비, 높이 전체 길이와 떠 있는 윈도우 사이즈를 갖고 스크롤바 비례를 맞춰준다.
	DWORD dwWholeWidth = m_dwWidth*DWORD(m_fOriRulerPixelStep * m_fViewSize) + m_dwItemVerticalOffset;
	m_dwHeight = DEFAULT_TIME_LINE_HEIGHT + DWORD(m_vlpItemInfo.size() * DEFAULT_OBJECT_BOX_HEIGHT);
	DWORD dwWholeHeight = m_dwHeight;

	SetScrollbars( SCROLL_PIXEL_PER_UNIT_X, SCROLL_PIXEL_PER_UNIT_Y, 
				   dwWholeWidth/SCROLL_PIXEL_PER_UNIT_X, dwWholeHeight/SCROLL_PIXEL_PER_UNIT_Y, 
				   m_iXScrollOffset/SCROLL_PIXEL_PER_UNIT_X, 
				   m_iYScrollOffset/SCROLL_PIXEL_PER_UNIT_Y );

	m_bNeedUpdateOffset = true;
}



void cwxTimeLineCtrl::_DrawTimeBar( wxAutoBufferedPaintDC &BufferedDC, DWORD dwDrawLineYOffset, DWORD dwWholeHeight )
{
	wxString strBuf;
	wxPen ActorPen;
	ActorPen.SetColour( wxColour( 0, 0, 0 ) );

	if( false == m_bShowTimeLineAsFloat )
		strBuf = strBuf.Format( wxT("%d %s"), DWORD((m_fNowTimeBarPosByPixel*1000/m_iRulerDivideFactor)/m_fRulerStep), m_strUnitName );
	else
		strBuf = strBuf.Format( wxT("%.2f %s"), m_fNowTimeBarPosByPixel / float(m_iRulerDivideFactor) / float(m_fRulerStep), m_strUnitName );

	wxFont TimeLineFont;
	TimeLineFont.SetPointSize( 7 );
	TimeLineFont.SetWeight( wxFONTWEIGHT_BOLD );	
	BufferedDC.SetFont( TimeLineFont );

	DWORD dwTimeBarLabelSize = BufferedDC.GetTextExtent( strBuf ).GetWidth();

	// 마지막으로 현재 타임 바 드로우
	BufferedDC.SetPen( ActorPen );
	BufferedDC.SetBrush( wxBrush( wxColour(0, 255, 255) ) );

	DWORD dwNowTimePos = (DWORD)m_fNowTimeBarPosByPixel;
	BufferedDC.DrawRectangle( m_dwItemVerticalOffset+dwNowTimePos, dwDrawLineYOffset,
							  /*DEFAULT_TIME_BAR_BOX_WIDTH*/dwTimeBarLabelSize+10, DEFAULT_TIME_BAR_BOX_HEIGHT );

	BufferedDC.SetFont( TimeLineFont );
	BufferedDC.DrawText( strBuf, m_dwItemVerticalOffset+dwNowTimePos+3, dwDrawLineYOffset+2 );

	wxPen wxTimeLinePen( wxColour(0, 0, 255), 2, wxSHORT_DASH );
	BufferedDC.SetPen( wxTimeLinePen );
	BufferedDC.DrawLine( m_dwItemVerticalOffset+dwNowTimePos, dwDrawLineYOffset+HORIZON_RULER_LINE_Y_OFFSET+10, 
						 m_dwItemVerticalOffset+dwNowTimePos, dwWholeHeight );
}


void cwxTimeLineCtrl::_DrawObjects( wxAutoBufferedPaintDC &BufferedDC, DWORD dwDrawLineXOffset, 
								   DWORD dwDrawLineYOffset, wxSize &ClientSize )
{
	// 액터 오브젝트를 드로우
	wxString strBuf;
	wxFont ActorFont;
	wxFont ObjectFont;
	wxPen ActorPen;
	wxPen ObjectPen;
	wxBrush ObjectBrush;

	ObjectPen = *wxRED_PEN;
	ObjectPen.SetWidth( 3 );

	int iCount = 0;
	vector<S_ITEM_INFO*>::iterator iter = m_vlpItemInfo.begin();
	vector<S_ITEM_INFO*>::iterator iterEnd = m_vlpItemInfo.end();
	for( iter; iter != iterEnd; ++iter )
	{
		//// 우선 항목 먼저 드로우
		S_ITEM_INFO* pActor = *iter;

		DWORD dwYOffset = dwDrawLineYOffset+10 + iCount*20;

		// 액션 드로우
		ObjectFont.SetPointSize( 8 );
		ObjectFont.SetFaceName( wxT("Arial") );
		BufferedDC.SetFont( ObjectFont );

		int iNumObject = (int)pActor->m_vlObjectInfo.size();
		for( int iObject = 0; iObject < iNumObject; ++iObject )
		{
			S_OBJECT_INFO* pObject = pActor->m_vlObjectInfo.at( iObject );

			if( m_pSelectedObjectInfo == pObject )
			{

				ObjectFont.SetWeight( wxFONTWEIGHT_BOLD );
				BufferedDC.SetFont( ObjectFont );
				BufferedDC.SetPen( ObjectPen );
			}
			else
			{
				ObjectFont.SetWeight( wxFONTWEIGHT_NORMAL );
				BufferedDC.SetFont( ObjectFont );
				BufferedDC.SetPen( ActorPen );
			}

			ObjectBrush.SetColour( pObject->Color );
			BufferedDC.SetBrush( ObjectBrush );

			DWORD dwSize = pObject->dwEndXPixel-pObject->dwStartXPixel;
			float fTimeLength = pObject->fEndTime - pObject->fStartTime;
			BufferedDC.DrawRectangle( m_dwItemVerticalOffset + pObject->dwStartXPixel, dwYOffset, dwSize+1, 21 );

			strBuf.Clear();

			if( (m_dwObjectDrawStyle & TL_SHOW_OBJECT_TIME_LENGTH) &&
				!(m_dwObjectDrawStyle & TL_SHOW_OBJECT_NAME) )
			{
				//if( false == m_bShowTimeLineAsFloat )
				//	strBuf = strBuf.Format( wxT("%d %s"), int(fTimeLength*1000), m_strUnitName );
				//else
					strBuf = strBuf.Format( wxT("%.2f %s"), fTimeLength, m_strUnitName );
			}
			else
			if( (m_dwObjectDrawStyle & TL_SHOW_OBJECT_NAME) &&
				!(m_dwObjectDrawStyle & TL_SHOW_OBJECT_TIME_LENGTH) )
			{
				strBuf = strBuf.Format( wxT("%s"), pObject->Name );
			}
			else
			if( (m_dwObjectDrawStyle & TL_SHOW_OBJECT_TIME_LENGTH) &&
				(m_dwObjectDrawStyle & TL_SHOW_OBJECT_NAME) )
			{
				//if( false == m_bShowTimeLineAsFloat )
				//	strBuf = strBuf.Format( wxT("%s (%d %s)"), pObject->Name, int(fTimeLength*1000), m_strUnitName );
				//else
					strBuf = strBuf.Format( wxT("%s (%.2f %s)"), pObject->Name, fTimeLength, m_strUnitName );
			}

			DWORD dwTextCenterPos = (dwSize - BufferedDC.GetTextExtent( strBuf ).GetWidth()) / 2;
			BufferedDC.DrawText( strBuf, m_dwItemVerticalOffset + pObject->dwStartXPixel + dwTextCenterPos, 
								 dwDrawLineYOffset+12 + iCount*DEFAULT_OBJECT_BOX_HEIGHT );
		}

		++iCount;
	}
}


//void cwxTimeLineCtrl::_DrawGraduation( wxAutoBufferedPaintDC& BufferedDC, DWORD dwDrawLineYOffset, DWORD dwGraduationStep, DWORD dwStartPixel, DWORD dwEndPixel )
//{
//	wxString strBuf;
//	DWORD dwStartIndex = dwStartPixel / dwGraduationStep;
//	DWORD dwEndIndex = dwEndPixel / dwGraduationStep; // 나눠져서 버려질 경우가 생기므로 한 스텝씩 여유를 둔다.
//	//DWORD dwEndIndex = (dwDrawLineXOffset+ClientSize.GetX()+m_dwActorVerticalOffset)/dwGraduationStep; // 나눠져서 버려질 경우가 생기므로 한 스텝씩 여유를 둔다.
//	//DWORD dwNumGraduationCountToDraw = (dwEndPixel - dwStartPixel) / dwGraduationStep;
//
//	//// 왼쪽 부분도 여유를 두기 위해 좀 더 드로우 시키도록 한다. 
//	//// 스크롤 버튼을 눌렀을 때 dirty 영역만 업데이트 되기 때문이다..
//	//if( dwStartIndex < 5 )
//	//	dwStartIndex = 0;
//
//	////
//
//	// 5번째 마다 길게 그려줌.
//	for( DWORD i = dwStartIndex; i < dwEndIndex; ++i )
//	{
//		DWORD dwGraduationSize = HORIZON_RULER_SHORT_GRADUATE_SIZE;
//		bool bIsFifth = (i % 5 == 0);
//
//		if( bIsFifth )
//		{
//			strBuf.Clear();
//			if( m_bShowTimeLineAsFloat )
//				strBuf = strBuf.Format( wxT("%.2f %s"), (float)i, m_strUnitName.c_str() );
//			else
//			{
//				// ms 단위이므로 1000 곱해준다.
//				strBuf = strBuf.Format( wxT("%d %s"), i*1000, m_strUnitName.c_str() );
//			}
//			BufferedDC.DrawText( strBuf, m_dwItemVerticalOffset + dwStartPixel + i*dwGraduationStep, dwDrawLineYOffset+2 );
//
//			dwGraduationSize = HORIZON_RULER_LONG_GRADUATE_SIZE;
//		}
//
//		BufferedDC.DrawLine( m_dwItemVerticalOffset + dwStartPixel + i*dwGraduationStep, dwDrawLineYOffset+HORIZON_RULER_LINE_Y_OFFSET+1, 
//							 m_dwItemVerticalOffset + dwStartPixel + i*dwGraduationStep, dwDrawLineYOffset+HORIZON_RULER_LINE_Y_OFFSET+1 + dwGraduationSize );
//
//		if( dwGraduationStep > 100 )
//		{
//			_DrawGraduation( BufferedDC, dwDrawLineYOffset, dwGraduationStep/5, dwStartPixel + i*dwGraduationStep, 
//																				dwStartPixel + (i+1)*dwGraduationStep );
//		}
//	}
//}


void cwxTimeLineCtrl::_DrawRuler( wxAutoBufferedPaintDC &BufferedDC, DWORD dwDrawLineXOffset, DWORD dwDrawLineYOffset, wxSize &ClientSize )
{
	wxPen Pen( *wxBLACK, 1 );
	wxBrush Brush( *wxWHITE_BRUSH );

	BufferedDC.SetPen( Pen );
	BufferedDC.SetBrush( Brush );

	// 클라이언트 영역에서 상대적으로 눈금을 단위 번호를 찍을 x offset 부분을 연산
	DWORD dwRulerXOffset = 0;
	if( m_dwItemVerticalOffset < dwDrawLineXOffset )
		dwRulerXOffset = dwDrawLineXOffset - m_dwItemVerticalOffset;
	else
		dwRulerXOffset = 0;

	// 클리어 역할의 커다란 하얀 사각형 먼저 드로우.
	BufferedDC.DrawRectangle( dwDrawLineXOffset, dwDrawLineYOffset, dwDrawLineXOffset+ClientSize.GetX(), 
							  HORIZON_RULER_LINE_Y_OFFSET+HORIZON_RULER_LONG_GRADUATE_SIZE+1 );

	// 줄 자의 구분선이 되는 가로 라인 드로우.
	BufferedDC.DrawLine( dwDrawLineXOffset, dwDrawLineYOffset+HORIZON_RULER_LINE_Y_OFFSET, 
						 dwDrawLineXOffset+ClientSize.GetX()+m_dwItemVerticalOffset, dwDrawLineYOffset+HORIZON_RULER_LINE_Y_OFFSET );

	//DWORD dwGraduationStep = m_fRulerStep;
	//_DrawGraduation( BufferedDC, dwDrawLineYOffset, dwGraduationStep, dwRulerXOffset, dwDrawLineXOffset+ClientSize.GetX()+m_dwItemVerticalOffset );

	DWORD dwStartIndex = DWORD((float)dwRulerXOffset / m_fRulerStep);
	DWORD dwEndPoint = DWORD(float(dwDrawLineXOffset+ClientSize.GetX()+m_dwItemVerticalOffset)/m_fRulerStep); // 나눠져서 버려질 경우가 생기므로 한 스텝씩 여유를 둔다.

	// 왼쪽 부분도 여유를 두기 위해 좀 더 드로우 시키도록 한다. 
	// 스크롤 버튼을 눌렀을 때 dirty 영역만 업데이트 되기 때문이다..
	if( dwStartIndex < 5 )
		dwStartIndex = 0;

	//

	// 5번째 마다 길게 그려줌.
	wxString strBuf;
	for( DWORD i = dwStartIndex; i < dwEndPoint; ++i )
	{
		DWORD dwVerticalUnitLength = 5;

		bool bIsFifth = (i % 5 == 0);

		if( bIsFifth )
		{
			strBuf.Clear();
			if( m_bShowTimeLineAsFloat )
				strBuf = strBuf.Format( wxT("%.2f %s"), (float)i / m_iRulerDivideFactor, m_strUnitName.c_str() );
			else
			{
				// ms 단위이므로 1000 곱해준다.
				strBuf = strBuf.Format( wxT("%d %s"), i*1000 / m_iRulerDivideFactor, m_strUnitName.c_str() );
			}
			BufferedDC.DrawText( strBuf, m_dwItemVerticalOffset + DWORD((float)i*m_fRulerStep), dwDrawLineYOffset+2 );

			dwVerticalUnitLength = HORIZON_RULER_LONG_GRADUATE_SIZE;
		}

		BufferedDC.DrawLine( m_dwItemVerticalOffset + DWORD((float)i*m_fRulerStep), dwDrawLineYOffset+HORIZON_RULER_LINE_Y_OFFSET+1, 
							 m_dwItemVerticalOffset + DWORD((float)i*m_fRulerStep), dwDrawLineYOffset+HORIZON_RULER_LINE_Y_OFFSET+1 + dwVerticalUnitLength );
	}
}


void cwxTimeLineCtrl::_DrawBaseLine( wxDC &BufferedDC, DWORD dwDrawLineXOffset, DWORD dwDrawLineYOffset, wxSize &ClientSize )
{
	wxPen Pen( *wxBLACK, 1 );
	wxBrush Brush( *wxWHITE_BRUSH );
	wxFont ItemFont;
	wxPen ItemPen;

	BufferedDC.SetPen( Pen );
	BufferedDC.SetBrush( Brush );

	// 먼저 하얀 사각형을 그려줌으로써 클리어 해준다.
	BufferedDC.DrawRectangle( dwDrawLineXOffset, dwDrawLineYOffset, DEFAULT_ITEM_VERTICAL_LINE, m_dwHeight );

	int iCount = 0;
	vector<S_ITEM_INFO*>::iterator iter = m_vlpItemInfo.begin();
	vector<S_ITEM_INFO*>::iterator iterEnd = m_vlpItemInfo.end();
	for( iter; iter != iterEnd; ++iter )
	{
		// 항목 드로우
		S_ITEM_INFO* pActor = *iter;
		ItemFont.SetPointSize( 9 );
		ItemFont.SetWeight( wxFONTWEIGHT_BOLD );
		ItemFont.SetFaceName( wxT("Arial") );
		BufferedDC.SetFont( ItemFont );

		ItemPen.SetColour( pActor->Color );
		BufferedDC.SetPen( ItemPen );

		int iYOffset = HORIZON_RULER_LINE_Y_OFFSET+HORIZON_RULER_LONG_GRADUATE_SIZE + iCount*20;
		if( (DWORD)iYOffset >= dwDrawLineYOffset+HORIZON_RULER_LINE_Y_OFFSET+HORIZON_RULER_LONG_GRADUATE_SIZE )
		{
			BufferedDC.DrawLine( dwDrawLineXOffset, iYOffset,
								 dwDrawLineXOffset+ClientSize.GetX()+m_dwItemVerticalOffset, HORIZON_RULER_LINE_Y_OFFSET+10 + iCount*20 );

			BufferedDC.DrawText( pActor->Name, dwDrawLineXOffset+5, iYOffset+2 );
		}

		++iCount;
	}
}



void cwxTimeLineCtrl::OnPaint( wxPaintEvent& PaintEvent )
{
	if( m_bNeedUpdateOffset )
	{
		GetViewStart( &m_iXScrollOffset, &m_iYScrollOffset );
		m_iXScrollOffset *= SCROLL_PIXEL_PER_UNIT_X;
		m_iYScrollOffset *= SCROLL_PIXEL_PER_UNIT_Y;
	}

	wxAutoBufferedPaintDC BufferedDC( this );
	PrepareDC(BufferedDC);

	DWORD dwWholeWidth = DWORD((float)m_dwWidth*m_fRulerStep) + m_dwItemVerticalOffset;
	DWORD dwWholeHeight = m_dwHeight;

	BufferedDC.SetBackground( wxBrush(GetBackgroundColour(), wxSOLID) );
	BufferedDC.Clear();

	// 클라이언트 사이즈만큼만 드로우하며 스크롤바에 따라 dirty 영역만 갱신하게 됨.
	wxSize ClientSize = GetClientSize();

	// 먼저 눈금을 그려보자... 길이는 꽤 길게.
	BufferedDC.SetPen( *wxBLACK_PEN );

	wxFont Font = *wxSWISS_FONT;
	Font.SetPointSize( DEFAULT_FONT_SIZE );
	BufferedDC.SetFont( Font );

	DWORD dwDrawLineXOffset = (DWORD)m_iXScrollOffset;
	DWORD dwDrawLineYOffset = (DWORD)m_iYScrollOffset;

	_DrawObjects( BufferedDC, dwDrawLineXOffset, HORIZON_RULER_LINE_Y_OFFSET, ClientSize );
	_DrawRuler( BufferedDC, dwDrawLineXOffset, dwDrawLineYOffset, ClientSize );
	_DrawTimeBar( BufferedDC, dwDrawLineYOffset, dwWholeHeight );
	_DrawBaseLine( BufferedDC, dwDrawLineXOffset, dwDrawLineYOffset, ClientSize );

}


void cwxTimeLineCtrl::OnScroll( wxScrollWinEvent& ScrollEvent )
{
	int iOffset = 0;

	wxEventType eEvent = ScrollEvent.GetEventType();

	m_bNeedUpdateOffset = true;
	if( wxEVT_SCROLLWIN_THUMBTRACK == eEvent || wxEVT_SCROLLWIN_THUMBRELEASE == eEvent )
	{
		if( wxHORIZONTAL == ScrollEvent.GetOrientation() )
			m_iXScrollOffset = ScrollEvent.GetPosition() * SCROLL_PIXEL_PER_UNIT_X;
		else
		if( wxVERTICAL == ScrollEvent.GetOrientation() )
			m_iYScrollOffset = ScrollEvent.GetPosition() * SCROLL_PIXEL_PER_UNIT_Y;

		m_bNeedUpdateOffset = false;
	}

	Refresh();

	ScrollEvent.Skip();
}


void cwxTimeLineCtrl::_NotifyEvent( int iEvent )
{
	if( iEvent == wxEVT_TIMELINE_OBJECT_SELECT )
	{
		cwxTLObjectSelectEvent SelectEvent( wxEVT_TIMELINE_OBJECT_SELECT, GetId(), m_pSelectedObjectInfo->Actor, 
			m_pSelectedObjectInfo->iID, m_pSelectedObjectInfo->iUseType, m_pSelectedObjectInfo->fStartTime );
		wxPostEvent( GetParent(), SelectEvent );
	}
	else
	if( iEvent == wxEVT_TIMELINE_OBJECT_MOVE )
	{
		cwxTLObjectMoveEvent MoveEvent( wxEVT_TIMELINE_OBJECT_MOVE, GetId(), m_pSelectedObjectInfo->Actor, 
			m_pSelectedObjectInfo->iID, m_pSelectedObjectInfo->fStartTime );
		wxPostEvent( GetParent(), MoveEvent );
	}
	else
	if( iEvent == wxEVT_TIMELINE_OBJECT_RESIZE )
	{
		cwxTLObjectResizeEvent ResizeEvent( wxEVT_TIMELINE_OBJECT_RESIZE, GetId(), m_pSelectedObjectInfo->Actor, 
			m_pSelectedObjectInfo->iID, m_pSelectedObjectInfo->fStartTime,
			m_pSelectedObjectInfo->fEndTime - m_pSelectedObjectInfo->fStartTime );
		wxPostEvent( GetParent(), ResizeEvent );
	}
}



void cwxTimeLineCtrl::OnMouseEvent( wxMouseEvent& MouseEvent )
{
	int iViewStartX = 0;
	int iViewStartY = 0;
	GetViewStart( &iViewStartX, &iViewStartY );
	wxPoint Pos = MouseEvent.GetPosition();
	Pos.x += iViewStartX * SCROLL_PIXEL_PER_UNIT_X;
	Pos.y += iViewStartY * SCROLL_PIXEL_PER_UNIT_Y;

	int iYIndex = Pos.y - HORIZON_RULER_LINE_Y_OFFSET+10;
	m_iOnOverYLineIndex = iYIndex / 20;
	if( m_iOnOverYLineIndex > 0 )
		m_iOnOverYLineIndex -= 1;

	m_fMousePosTime = (MouseEvent.GetPosition().x - (int)m_dwItemVerticalOffset) / (m_fOriRulerPixelStep * m_fViewSize);
	if( m_fMousePosTime < 0.0f )
		m_fMousePosTime = 0.0f;

	if( MouseEvent.LeftDown() )
	{
		bool bSelected = false;

		m_LastClkMousePos = MouseEvent.GetPosition();

		if( m_bEnable &&
			Pos.y > m_iYScrollOffset + HORIZON_RULER_LINE_Y_OFFSET+HORIZON_RULER_LONG_GRADUATE_SIZE )
		{
			DWORD dwYIndex = DWORD(Pos.y) - HORIZON_RULER_LINE_Y_OFFSET+HORIZON_RULER_LONG_GRADUATE_SIZE;
			dwYIndex /= 20;

			if( dwYIndex > 0 )
				dwYIndex -= 1;
		
			DWORD dwMouseXPos = (DWORD)Pos.x - m_dwItemVerticalOffset;

			if( dwYIndex < (DWORD)m_vlpItemInfo.size() )
			{
				S_ITEM_INFO* pActorObject = m_vlpItemInfo.at( dwYIndex );

				int iNumObject = (int)pActorObject->m_vlObjectInfo.size();
				for( int iObject = 0; iObject < iNumObject; ++iObject )
				{
					S_OBJECT_INFO* pObject = pActorObject->m_vlObjectInfo.at( iObject );

					DWORD dwObjectStartPos = pObject->dwStartXPixel;
					DWORD dwObjectEndPos = pObject->dwEndXPixel;

					// 양쪽 끝부분 2픽셀 정도에서 움직인다면 해당 부분만 사이즈 바꿔줌
					if( (dwObjectStartPos-3 < dwMouseXPos) && (dwMouseXPos < dwObjectStartPos+3) )
					{
						m_iSelectedObjectPart = OBJECT_BEGIN;

						m_iLastClkXPos = Pos.x;
						m_bIsLBtnDown = true;
						bSelected = true;

						if( m_pSelectedObjectInfo != pObject )
						{
							m_pSelectedObjectInfo = pObject;
							_NotifyEvent( wxEVT_TIMELINE_OBJECT_SELECT );
						}
						break;
					}
					else
					if( (dwObjectEndPos-3 < dwMouseXPos) && (dwMouseXPos < dwObjectEndPos+3) )
					{
						m_iSelectedObjectPart = OBJECT_END;

						m_iLastClkXPos = Pos.x;
						m_bIsLBtnDown = true;
						bSelected = true;

						if( m_pSelectedObjectInfo != pObject )
						{
							m_pSelectedObjectInfo = pObject;
							_NotifyEvent( wxEVT_TIMELINE_OBJECT_SELECT );
						}
						break;
					}
					else
					if( ( dwObjectStartPos < dwMouseXPos ) && ( dwMouseXPos < dwObjectEndPos) )
					{
						m_iSelectedObjectPart = OBJECT_CENTER;

						m_iLastClkXPos = Pos.x;
						m_bIsLBtnDown = true;
						bSelected = true;

						if( m_pSelectedObjectInfo != pObject )
						{
							m_pSelectedObjectInfo = pObject;
							_NotifyEvent( wxEVT_TIMELINE_OBJECT_SELECT );
						}
						break;
					}
				}
			}
		}
		else
		{
			// 타임 바의 탭을 눌렀는지.
			DWORD dwMouseXPos = (DWORD)Pos.x - m_dwItemVerticalOffset;
		
			if( (DWORD)m_fNowTimeBarPosByPixel < dwMouseXPos && dwMouseXPos < (DWORD)m_fNowTimeBarPosByPixel+DEFAULT_TIME_BAR_BOX_WIDTH &&
				m_iYScrollOffset < Pos.y && Pos.y < m_iYScrollOffset + DEFAULT_TIME_BAR_BOX_HEIGHT )
			{
				m_bTimeBarClked = true;
				m_iLastClkXPos = Pos.x;

				cwxTLAxisEvent AxisEvent( wxEVT_TIMELINE_AXIS_EVENT, GetId() );
				AxisEvent.SetOnLBDown( true );
				wxPostEvent( GetParent(), AxisEvent );
			}
		}

		if( false == bSelected )
			m_pSelectedObjectInfo = NULL;
		else
			m_PrevObjectInfo = *m_pSelectedObjectInfo;

		Refresh( true, &GetClientRect() );
	}
	
	if( MouseEvent.Dragging() )
	{
		//wxPoint Pos = MouseEvent.GetPosition();

		// 이전 위치와 비교해서 3픽셀 이상 움직인 거 아니면 움직이지 마라~
		if( m_bMoved || 3 < abs( m_LastClkMousePos.x - MouseEvent.GetPosition().x ) )
		{
			m_bMoved = true;

			int iDelta = Pos.x - m_iLastClkXPos;
			m_iLastClkXPos = Pos.x;

			if( m_bTimeBarClked )
			{
				float fResult = m_fNowTimeBarPosByPixel + (float)iDelta;
				if( fResult > 0.0f ) 
				{
					m_fNowTimeBarPosByPixel = fResult;
					//m_dwNowTimeBarPosByUnit = m_fNowTimeBarPosByPixel / m_dwRulerStepPixel;
				}
				else
				{
					m_fNowTimeBarPosByPixel = 0;
					//m_dwNowTimeBarPosByUnit = 0;
				}

				cwxTLAxisEvent AxisEvent( wxEVT_TIMELINE_AXIS_EVENT, GetId() );
				AxisEvent.SetAxisMove( true );
				AxisEvent.SetLBIsDown( true );
				float fNowTime = m_fNowTimeBarPosByPixel / (m_fOriRulerPixelStep * m_fViewSize);
				AxisEvent.SetNowTime( fNowTime );
				wxPostEvent( GetParent(), AxisEvent );

				Refresh( true, &GetClientRect() );
			}
			else
			if( m_bEnable &&
				m_pSelectedObjectInfo && m_bIsLBtnDown )
			{
				DWORD dwObjectStartPos = m_pSelectedObjectInfo->dwStartXPixel;
				DWORD dwObjectEndPos = m_pSelectedObjectInfo->dwEndXPixel;

				switch( m_iSelectedObjectPart )
				{
					case OBJECT_BEGIN:
					{
						if( false == m_pSelectedObjectInfo->bSizeFixed )
						{
							if( 0 < int(dwObjectStartPos) + iDelta)
							{
								if( (20 < int(dwObjectEndPos) - int(dwObjectStartPos)) || (iDelta < 0) )
								{
									int iResult = (int)m_pSelectedObjectInfo->dwStartXPixel + iDelta;
									if( iResult >= 0 )
									{
										m_pSelectedObjectInfo->dwStartXPixel = iResult;
										m_pSelectedObjectInfo->fStartTime = (float)m_pSelectedObjectInfo->dwStartXPixel / (m_fOriRulerPixelStep * m_fViewSize);
									}
									else
									{
										m_pSelectedObjectInfo->fStartTime = 0.0f;
									}

									//SetCursor(wxCursor(wxCURSOR_MAGNIFIER));
									Refresh( true, &GetClientRect() );
								}
							}
						}
					}
					break;
					
					case OBJECT_END:
					{
						if( false == m_pSelectedObjectInfo->bSizeFixed )
						{
							if( 20 < int(dwObjectEndPos) - int(dwObjectStartPos) || (iDelta > 0) )
							{
								m_pSelectedObjectInfo->dwEndXPixel = (int)dwObjectEndPos + iDelta;
								m_pSelectedObjectInfo->fEndTime = (float)m_pSelectedObjectInfo->dwEndXPixel / (m_fOriRulerPixelStep * m_fViewSize);
								//SetCursor(wxCursor(wxCURSOR_MAGNIFIER));
								Refresh( true, &GetClientRect() );
							}
						}
					}
					break;

					case OBJECT_CENTER:
					{
						if( 0 < int(dwObjectStartPos) + iDelta)
						{
							//int iResult = (int)m_pSelectedObjectInfo->dwStartTimeInUnit + (iDelta/(int)m_dwUnit);
							//DWORD dwOriStartPosInUnit = m_pSelectedObjectInfo->dwStartTimeInUnit;
							int iResult = m_pSelectedObjectInfo->dwStartXPixel = (int)dwObjectStartPos + iDelta;
							if( iResult >= 0 )
							{
								m_pSelectedObjectInfo->dwStartXPixel = (int)dwObjectStartPos + iDelta;
								m_pSelectedObjectInfo->fStartTime = (float)m_pSelectedObjectInfo->dwStartXPixel / (m_fOriRulerPixelStep * m_fViewSize);
								
								m_pSelectedObjectInfo->dwEndXPixel = (int)dwObjectEndPos + iDelta;
								m_pSelectedObjectInfo->fEndTime = (float)m_pSelectedObjectInfo->dwEndXPixel / (m_fOriRulerPixelStep * m_fViewSize);
							}
							//else
							//{
							//	m_pSelectedObjectInfo->dwStartTimeInUnit = (int)m_pSelectedObjectInfo->dwStartTimeInUnit + (iDelta/m_dwUnit);
							//	m_pSelectedObjectInfo->dwEndTimeInUnit = (int)m_pSelectedObjectInfo->dwEndTimeInUnit + (iDelta/m_dwUnit);
							//}
							Refresh( true, &GetClientRect() );
						}

					}
					break;
				}
				
			}
		}
	}

	// 조정된 dwStartXPixel, dwEndXPixel 을 원본 좌표에도 반영해준다.
	if( m_pSelectedObjectInfo )
	{
		m_pSelectedObjectInfo->fOriStartXPixel = (float)m_pSelectedObjectInfo->dwStartXPixel / m_fViewSize;
		m_pSelectedObjectInfo->fOriEndXPixel = (float)m_pSelectedObjectInfo->dwEndXPixel / m_fViewSize;
	}

	if( MouseEvent.LeftUp() )
	{
		if( m_bEnable && m_bMoved && m_pSelectedObjectInfo )
		{
			if( m_PrevObjectInfo != *m_pSelectedObjectInfo )
			{
				switch( m_iSelectedObjectPart )
				{
					case OBJECT_BEGIN:
						_NotifyEvent( wxEVT_TIMELINE_OBJECT_RESIZE );
						break;

					case OBJECT_END:
						_NotifyEvent( wxEVT_TIMELINE_OBJECT_RESIZE );
						break;

					case OBJECT_CENTER:
						_NotifyEvent( wxEVT_TIMELINE_OBJECT_MOVE );
						break;
				}
			}
		}

		if( m_bTimeBarClked )
		{
			cwxTLAxisEvent AxisEvent( wxEVT_TIMELINE_AXIS_EVENT, GetId() );
			AxisEvent.SetOnLBUp( true );
			wxPostEvent( GetParent(), AxisEvent );
		}

		m_bIsLBtnDown = false;
		m_bTimeBarClked = false;
		m_bMoved = false;
		m_iSelectedObjectPart = OBJECT_NOT_SELECTED;
		m_iLastClkXPos = 0;
		//SetCursor(wxCursor(wxCURSOR_ARROW));
	}
	
	if( MouseEvent.GetWheelRotation() != 0 )
	{
		m_fViewSize += float(MouseEvent.GetWheelRotation()) / 100.0f;
		if( m_fViewSize < 1.0f )
			m_fViewSize = 1.0f;
		_UpdateViewScale();
		Refresh();
	}
	else
		MouseEvent.Skip( true );
		MouseEvent.ResumePropagation( 2 );
}



bool cwxTimeLineCtrl::AddItem( const wxString& Name, const wxColour& Color )
{
	bool bResult = false;

	// 중복된 이름은 넣지 않는다.
	map<wxString, S_ITEM_INFO*>::iterator iter = m_mapItemInfo.find( Name );
	if( m_mapItemInfo.end() == iter )
	{
		S_ITEM_INFO* pNewActorObject = new S_ITEM_INFO;
		pNewActorObject->Name = Name;
		m_mapItemInfo.insert( make_pair(Name, pNewActorObject) );
		m_vlpItemInfo.push_back( pNewActorObject );

		bResult = true;
	}

	return bResult;
}


bool cwxTimeLineCtrl::DelItem( const wxString& Name )
{
	bool bResult = false;
	
	map<wxString, S_ITEM_INFO*>::iterator iter = m_mapItemInfo.find( Name );
	if( m_mapItemInfo.end() != iter )
	{
		m_mapItemInfo.erase( iter );

		vector<S_ITEM_INFO*>::iterator iterVec = find( m_vlpItemInfo.begin(),
															   m_vlpItemInfo.end(), iter->second );
		m_vlpItemInfo.erase( iterVec );

		bResult = true;
	}

	return bResult;
}



bool cwxTimeLineCtrl::AddObject( const wxString& ActorName, const wxString& ObjectName, int iObjectID, 
								  int iUseType, bool bSizeFixed, float fStartTime /*= 0.0f*/, float fLength /*= 20.0f */, 
								  const wxColour& Color/* = wxColour(wxT("LIGHT_GREY"))*/ )
{
	bool bResult = false;

	map<wxString, S_ITEM_INFO*>::iterator iter = m_mapItemInfo.find( ActorName );
	if( m_mapItemInfo.end() != iter )
	{
		S_ITEM_INFO* pActorObject = iter->second;
		S_OBJECT_INFO* pNewObject = new S_OBJECT_INFO;

		pNewObject->Actor = iter->second->Name;
		pNewObject->Name = ObjectName;
		pNewObject->iID = iObjectID;
		pNewObject->fStartTime = fStartTime;
		pNewObject->fEndTime = fStartTime+fLength;
		pNewObject->iUseType = iUseType;
		pNewObject->bSizeFixed = bSizeFixed;
		pNewObject->Color = Color;
		
		pNewObject->fOriStartXPixel = fStartTime * m_fOriRulerPixelStep;
		pNewObject->fOriEndXPixel = (fStartTime+fLength) * m_fOriRulerPixelStep;

		pActorObject->m_vlObjectInfo.push_back( pNewObject );
		m_vlpObjects.push_back( pNewObject );

		_UpdateViewScale();
		_UpdateScrollBar();

		bResult = true;
	}

	return bResult;
}




bool cwxTimeLineCtrl::ModifyObject( const wxString& ActorName, const wxString& ObjectName, int iObjectID, 
								     int iUseType, bool bSizeFixed, float fStartTime/* = 0.0f*/, float fLength/* = 20.0f*/, 
									 const wxColour& Color/* = wxColour(200, 200, 200)*/ )
{
	bool bResult = false;

	map<wxString, S_ITEM_INFO*>::iterator iter = m_mapItemInfo.find( ActorName );
	if( m_mapItemInfo.end() != iter )
	{
		S_ITEM_INFO* pActorObject = iter->second;

		vector<S_OBJECT_INFO*>::iterator iterVec = pActorObject->m_vlObjectInfo.begin();
		vector<S_OBJECT_INFO*>::iterator iterEnd = pActorObject->m_vlObjectInfo.end();
		for( iterVec; iterVec != iterEnd; ++iterVec )
		{
			if( (*iterVec)->iID == iObjectID )
			{
				S_OBJECT_INFO* pObject = (*iterVec);
				pObject->Actor = ActorName;
				pObject->Name = ObjectName;
				pObject->iID = iObjectID;
				pObject->fStartTime = fStartTime;
				pObject->fEndTime = fStartTime+fLength;
				pObject->iUseType = iUseType;
				pObject->bSizeFixed = bSizeFixed;
				pObject->Color = Color;

				pObject->fOriStartXPixel = fStartTime * m_fOriRulerPixelStep;
				pObject->fOriEndXPixel = (fStartTime+fLength) * m_fOriRulerPixelStep;
				//vector<S_ACT_UNIT_INFO*>::iterator iterObjects = find( m_vlpObjects.begin(), m_vlpObjects.end(), *iterVec );
				//m_vlpObjects.erase( iterObjects );
				_UpdateViewScale();

				//delete (*iterVec);
				//pActorObject->m_vlObjectInfo.erase( iterVec );
				bResult = true;

				break;
			}
		}
	}

	return bResult;
}




bool cwxTimeLineCtrl::DelObject( const wxString& ActorName, int iObjectID )
{
	bool bResult = false;

	map<wxString, S_ITEM_INFO*>::iterator iter = m_mapItemInfo.find( ActorName );
	if( m_mapItemInfo.end() != iter )
	{
		S_ITEM_INFO* pActorObject = iter->second;

		//// 액션 이름으로 액션을 찾아주는 함수 객체
		//struct FindByObjectName : public unary_function<const S_Object_INFO*, bool>
		//{
		//	private:
		//		wxString	m_ObjectNameToFind;

		//	public:
		//		FindByObjectName( const wxString& ObjectNameToFind ) : m_ObjectNameToFind( ObjectNameToFind ) {};

		//		bool operator () ( const S_Object_INFO* pObjectInfo )
		//		{
		//			return (pObjectInfo->Name == m_ObjectNameToFind);
		//		}

		//};
		
		// ... 이 코드 왜 컴파일러가 뻑나버리지. -_-
		//vector<S_Object_INFO*>::iterator iterVec = find_if( pActorObject->m_vlObjectInfo.begin(), 
		//												    pActorObject->m_vlObjectInfo.end(), 
		//													FindByObjectName( ActorName ) );
		
		vector<S_OBJECT_INFO*>::iterator iterVec = pActorObject->m_vlObjectInfo.begin();
		vector<S_OBJECT_INFO*>::iterator iterEnd = pActorObject->m_vlObjectInfo.end();
		for( iterVec; iterVec != iterEnd; ++iterVec )
		{
			if( (*iterVec)->iID == iObjectID )
			{
				vector<S_OBJECT_INFO*>::iterator iterObjects = find( m_vlpObjects.begin(), m_vlpObjects.end(), *iterVec );
				m_vlpObjects.erase( iterObjects );

				delete (*iterVec);
				pActorObject->m_vlObjectInfo.erase( iterVec );
				bResult = true;

				break;
			}
		}
	}

	return bResult;
}



bool cwxTimeLineCtrl::FixObjectSize( int iObjectID, bool bFix )
{
	bool bResult = false;

	vector<S_OBJECT_INFO*>::iterator iterVec = m_vlpObjects.begin();
	vector<S_OBJECT_INFO*>::iterator iterEnd = m_vlpObjects.end();
	for( iterVec; iterVec != iterEnd; ++iterVec )
	{
		if( (*iterVec)->iID == iObjectID )
		{
			m_vlpObjects.at( iObjectID )->bSizeFixed = bFix;
			bResult = true;
		}
	}

	return bResult;
}



int cwxTimeLineCtrl::GetSelectedObjectID( void )
{
	if( m_pSelectedObjectInfo )
		return m_pSelectedObjectInfo->iID;
	else
		return wxNOT_FOUND;
}




int cwxTimeLineCtrl::GetSelectedObjectUseType( void )
{
	if( m_pSelectedObjectInfo )
		return m_pSelectedObjectInfo->iUseType;
	else
		return wxNOT_FOUND;
}




void cwxTimeLineCtrl::SelectObject( int iObjectID )
{
	if( wxNOT_FOUND != iObjectID )
	{
		vector<S_OBJECT_INFO*>::iterator iterVec = m_vlpObjects.begin();
		vector<S_OBJECT_INFO*>::iterator iterEnd = m_vlpObjects.end();
		for( iterVec; iterVec != iterEnd; ++iterVec )
		{
			if( (*iterVec)->iID == iObjectID )
			{
				m_pSelectedObjectInfo = *iterVec;
				break;
			}
		}
		
		Refresh( true, &GetClientRect() );
	}
}





void cwxTimeLineCtrl::_UpdateViewScale( void )
{
	//m_dwRulerStepPixel = DWORD(m_fOriRulerPixelStep * m_fViewSize);
	//m_dwNowTimeBarPosByPixel = (DWORD)((float)m_dwNowOriTimeBarPosByPixel * m_fViewSize);

	float fScaleFactor = m_fViewSize;
	m_iRulerDivideFactor = 1;
	while( fScaleFactor > 5.0f )
	{
		fScaleFactor /= 5.0f;
		m_iRulerDivideFactor *= 5;
	}

	m_fRulerStep = m_fOriRulerPixelStep * fScaleFactor;

	int iNumObject = (int)m_vlpObjects.size();
	for( int iObject = 0; iObject < iNumObject; ++iObject )
	{
		S_OBJECT_INFO* pNowObject = m_vlpObjects.at( iObject );

		pNowObject->dwStartXPixel = (DWORD)((float)pNowObject->fOriStartXPixel * m_fViewSize);
		pNowObject->dwEndXPixel = (DWORD)((float)pNowObject->fOriEndXPixel * m_fViewSize );
	}

	// 단위 갱신
	//if( m_fRulerStep < 50 )
	//{
		m_strUnitName.assign( wxT("sec") );
		m_bShowTimeLineAsFloat = true;
	//}
	//else
	//{
	//	m_strUnitName.assign( wxT("ms") );
	//	m_bShowTimeLineAsFloat = false;
	//}

	_UpdateScrollBar();
}



//void cwxTimeLineCtrl::SetUnitName( wxString& strUnitName )
//{
//	m_strUnitName = strUnitName;
//}


//void cwxTimeLineCtrl::SetScale( float fScale )
//{
//	m_fScale = fScale;
//}


void cwxTimeLineCtrl::SetViewScale( float fScaleSize )
{
	m_fViewSize = fScaleSize;

	_UpdateViewScale();
}



void cwxTimeLineCtrl::UpdateTimeBar( float fTime )
{
	//if( m_bShowTimeLineAsFloat )
	//{
	//	m_dwNowTimeBarPosByUnit = (DWORD)fUnitTime;	
	//	m_dwNowTimeBarPosByPixel = DWORD(fUnitTime * m_dwRulerPixelStep);
	//}
	//else
	//{
		//m_dwNowTimeBarPosByUnit = (DWORD)fUnitTime;

		m_fNowTimeBarPosByPixel = fTime * (m_fOriRulerPixelStep * m_fViewSize);
		Refresh( true, &GetClientRect() );
	//}
}


void cwxTimeLineCtrl::ResetTimeBar( void )
{
	//m_dwNowTimeBarPosByUnit = 0;
}


float cwxTimeLineCtrl::GetNowTime( void )
{
	float fNowTimeByUnit = (float)(m_fNowTimeBarPosByPixel) / (m_fOriRulerPixelStep * m_fViewSize);
	
	return fNowTimeByUnit;
}
