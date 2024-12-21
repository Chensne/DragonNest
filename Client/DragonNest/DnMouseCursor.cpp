#include "Stdafx.h"
#include "DnMouseCursor.h"
#include "resource.h"
#include "DnMainFrame.h"
#include "DnSlotButton.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
CDnMouseCursor::CDnMouseCursor()
: CInputReceiver( true )
{
	m_CursorState = CursorStateEnum::NORMAL;
	m_LastCursorState = CursorStateEnum::NOASSIGN;
	m_fMouseSensitivity = 0.07f;
	memset( m_CursorArray, 0, sizeof(m_CursorArray) );
	m_bShowCursor = false;
	m_nShowRefCnt = 0;
	for ( int i = 0 ; i < CursorStateEnum_Amount ; i++ )
	{
		m_CursorTexture[i].Identity();
		m_nXHotSpotArray[i] = 1;
		m_nYHotSpotArray[i] = 1;
	}
	m_bStatic = false;
	m_bForceClipCursorNull = false;
	m_prevClipCursorRect.left = m_prevClipCursorRect.right = m_prevClipCursorRect.top = m_prevClipCursorRect.bottom = 0;
	m_prevCursorPos.x = m_prevCursorPos.y = 0;
}

CDnMouseCursor::~CDnMouseCursor()
{
	for( int i=0; i<CursorStateEnum_Amount; i++ ) {
		SAFE_RELEASE_SPTR( m_CursorTexture[i] );
	}
}


bool CDnMouseCursor::Create()
{
	m_CursorTexture[NORMAL] = EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName("Cursor_Normal.dds").c_str() );
	m_CursorTexture[CLICK] = EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName("Cursor_Click.dds").c_str() );
	m_CursorTexture[SIZE] = EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName("Cursor_Size.dds").c_str() );
	SetHotSpot(SIZE, 16, 16);

	if ( !m_CursorTexture[NORMAL] )
		return false;

	GetEtDevice()->SetCursorProperties( m_CursorTexture[NORMAL]->GetSurfaceLevel() );
	GetEtDevice()->ShowCursor( false );
//	::ShowCursor( false );

	return true;
}

void CDnMouseCursor::SetCursorState( CursorStateEnum State )
{
	m_CursorState = State;
//	::SetCursor( m_CursorArray[State] );
}

void CDnMouseCursor::SetHotSpot( UINT nStateIndex, UINT nXHotSpot, UINT nYHotSpot )
{
	m_nXHotSpotArray[nStateIndex] = nXHotSpot;
	m_nYHotSpotArray[nStateIndex] = nYHotSpot;
}

void CDnMouseCursor::ClipCursor( RECT *pRect )
{
	RECT currentRect;
	currentRect.left = currentRect.right = currentRect.top = currentRect.bottom = 0;

	if( pRect )	 {
		currentRect = *pRect;
	}
	
	if( currentRect.left != m_prevClipCursorRect.left || currentRect.right != m_prevClipCursorRect.right ||
		currentRect.top != m_prevClipCursorRect.top || currentRect.bottom != m_prevClipCursorRect.bottom ) 
	{
		if( pRect ) {
			::ClipCursor( &currentRect );
		}
		else {
			::ClipCursor( NULL );
		}
		m_prevClipCursorRect = currentRect;
	}

	if( m_bForceClipCursorNull )
	{
		::ClipCursor( NULL );
		if( !GetClipCursor( &currentRect ) )
			m_bForceClipCursorNull = false;
	}
}

void CDnMouseCursor::SetCursorPosToDevice( int nX, int nY )
{
	if( nX != m_prevCursorPos.x || nY != m_prevCursorPos.y ) {
		GetEtDevice()->SetCursorPos( nX, nY );
		m_prevCursorPos.x = nX;
		m_prevCursorPos.y = nY;
	}
}

void CDnMouseCursor::UpdateCursor()
{
	bool bClipCursor = false;
	if( !IsShowCursor() && !CDnMainFrame::GetInstance().IsBeginCaption() ) {
		if( GetFocus() == CDnMainFrame::GetInstance().GetHWnd() &&
			GetActiveWindow() == CDnMainFrame::GetInstance().GetHWnd() )
		{
			RECT rcRect;
			GetClientRect( CDnMainFrame::GetInstance().GetHWnd(), &rcRect );

			ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), (POINT*)(&rcRect.left ) );
			ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), (POINT*)(&rcRect.right ) );

			POINT p;
			p.x = rcRect.left + ( ( rcRect.right - rcRect.left ) / 2 );
			p.y = rcRect.top + ( ( rcRect.bottom - rcRect.top ) / 2 );

			GetEtDevice()->SetCursorPos( p.x, p.y );
//			SetCursorPos( p.x, p.y );
			bClipCursor = true;
		}
	}
	GetEtDevice()->ShowCursor( IsShowCursor() );
//#ifdef _FINAL_BUILD
	if( bClipCursor ) {
		RECT rcRect;
		GetClientRect( CDnMainFrame::GetInstance().GetHWnd(), &rcRect );

		ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), (POINT*)(&rcRect.left ) );
		ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), (POINT*)(&rcRect.right ) );

		rcRect.left = rcRect.left + ( ( rcRect.right - rcRect.left ) / 2 );
		rcRect.right = rcRect.left;
		rcRect.top = rcRect.top + ( ( rcRect.bottom - rcRect.top ) / 2 );
		rcRect.bottom = rcRect.top;
		ClipCursor( &rcRect );
	}
	else {
		ClipCursor( NULL );
	}
//#endif

	// 일반적인 상황에선 기존의 처리를 따른다.
	// 그 외의 경우엔 SetCursorState를 통해 바뀌어야한다.
	if( m_CursorState == CursorStateEnum::NORMAL || m_CursorState == CursorStateEnum::CLICK )
	{
		if( IsPushMouseButton(0) ) {
			m_CursorState = CursorStateEnum::CLICK;
		}
		else {
			m_CursorState = CursorStateEnum::NORMAL;
		}
	}

	if( m_LastCursorState != m_CursorState && IsShowCursor() && m_CursorTexture[m_CursorState] ) {
		/*
		POINT p;
		GetCursorPos( &p );
		ScreenToClient( CDnMainFrame::GetInstance().GetHWnd(), &p );

		EtVector4 Vertices[ 4 ];
		EtVector2 TexCoord[ 4 ];
		int nWidth, nHeight;
		int nTexWidth, nTexHeight;

		nWidth = CEtDevice::GetInstance().Width();
		nHeight = CEtDevice::GetInstance().Height();
		nTexWidth = m_CursorTexture[m_CursorState]->Width() - 9;
		nTexHeight = m_CursorTexture[m_CursorState]->Height() - 9;

		Vertices[0] = EtVector4( (float)p.x, (float)p.y, 0.f, 0.f );
		Vertices[1] = EtVector4( (float)p.x + nTexWidth, (float)p.y, 0.f, 0.f );
		Vertices[2] = EtVector4( (float)p.x + nTexWidth, (float)p.y + nTexHeight, 0.f, 0.f );
		Vertices[3] = EtVector4( (float)p.x, (float)p.y + nTexHeight, 0.f, 0.f );
		TexCoord[ 0 ] = EtVector2( 0.0f, 0.0f );
		TexCoord[ 1 ] = EtVector2( 1.0f, 0.0f );
		TexCoord[ 2 ] = EtVector2( 1.0f, 1.0f );
		TexCoord[ 3 ] = EtVector2( 0.0f, 1.0f );

		EternityEngine::DrawQuad2DWithTex( Vertices, TexCoord, m_CursorTexture[m_CursorState]->GetTexturePtr() );
		*/
		GetEtDevice()->SetCursorProperties( m_CursorTexture[m_CursorState]->GetSurfaceLevel(), m_nXHotSpotArray[m_CursorState], m_nYHotSpotArray[m_CursorState] );
		m_LastCursorState = m_CursorState;
	}
}

void CDnMouseCursor::OnLostDevice()
{

}
void CDnMouseCursor::OnResetDevice()
{
	EtSurface *pSurface = m_CursorTexture[m_CursorState]->GetSurfaceLevel();
	if( pSurface ) {
		GetEtDevice()->SetCursorProperties( pSurface );
	}
	m_prevClipCursorRect.left = m_prevClipCursorRect.right = m_prevClipCursorRect.top = m_prevClipCursorRect.bottom = 0;
	m_prevCursorPos.x = m_prevCursorPos.y = 0;
}

void CDnMouseCursor::ShowCursor( bool bShow, bool bForce ) 
{
	bool bReleaseDrag = false;
	if( bForce )
	{
		m_bShowCursor = bShow;
		if( !bShow ) {
			bReleaseDrag = true;
		}
	}
	else
	{
		if( m_bStatic )
		{
			m_bShowCursor = true;
			return;
		}

		if( bShow )
		{
			m_nShowRefCnt++;
			m_bShowCursor = bShow;
		}
		else
		{
			m_nShowRefCnt--;
			if( m_nShowRefCnt <= 0 )
			{
				bReleaseDrag = true;
				m_nShowRefCnt = 0;
				m_bShowCursor = bShow;
			}
		}
	}
	if( bReleaseDrag ) {
		if( drag::GetControl() ) {
			CDnSlotButton *pDragButton = dynamic_cast<CDnSlotButton *>(drag::GetControl());
			if( pDragButton ) pDragButton->DisableSplitMode(true);
		}
		focus::ReleaseControl();
		drag::ReleaseControl();
	}
}

void CDnMouseCursor::SetMouseSensitivity( float fValue )
{
	m_fMouseSensitivity = fValue;
}

void CDnMouseCursor::AddShowRefCnt()
{
	m_nShowRefCnt++;
}

void CDnMouseCursor::ReleaseShowRefCnt()
{
	m_nShowRefCnt--;
	if( m_nShowRefCnt < 0 )
	{
		m_nShowRefCnt = 0;
	}
}

void CDnMouseCursor::SetCursorPos( int nX, int nY )
{
	if( IsShowCursor() )
	{
		if( GetFocus() == CDnMainFrame::GetInstance().GetHWnd() &&
			GetActiveWindow() == CDnMainFrame::GetInstance().GetHWnd() )
		{
			POINT pt;
			pt.x = nX;
			pt.y = nY;
			ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), &pt );
			GetEtDevice()->SetCursorPos( pt.x, pt.y	);
		}
	}
}

void CDnMouseCursor::GetCursorPos( int *pX, int *pY )
{
	if( IsShowCursor() )
	{
		if( GetFocus() == CDnMainFrame::GetInstance().GetHWnd() &&
			GetActiveWindow() == CDnMainFrame::GetInstance().GetHWnd() )
		{
			POINT pt;
			::GetCursorPos(&pt);
			ScreenToClient( CDnMainFrame::GetInstance().GetHWnd(), &pt );
			if( pX ) *pX = pt.x;
			if( pY ) *pY = pt.y;
		}
	}
}


bool CDnMouseCursor::IsShowCursor() 
{ 
	return m_bShowCursor; 
}

float CDnMouseCursor::GetCursorWidth()
{
	return static_cast<float>( m_CursorTexture[m_CursorState]->Width() ) / CEtDevice::GetInstance().Width();
}

float CDnMouseCursor::GetCursorHeight()
{
	return static_cast<float>( m_CursorTexture[m_CursorState]->Height() ) / CEtDevice::GetInstance().Height();
}