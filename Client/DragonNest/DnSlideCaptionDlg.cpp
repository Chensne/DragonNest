#include "StdAfx.h"
#include "DnSlideCaptionDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSlideCaptionDlg::CDnSlideCaptionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_nLastSlideIndex(-1)
, m_bShowSlideText(false)
, m_bRender(false)
, m_srcColor((DWORD)0x00000000)
, m_destColor((DWORD)0x00000000)
, m_renderColor((DWORD)0x00000000)
, m_pCation(NULL)
{
}

CDnSlideCaptionDlg::~CDnSlideCaptionDlg(void)
{
}

void CDnSlideCaptionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SlideCaptionDlg.ui" ).c_str(), bShow );
}

void CDnSlideCaptionDlg::InitialUpdate()
{
	m_pCation = GetControl<CEtUIStatic>("ID_CAPTION");

	UpdateBlind();
}

void CDnSlideCaptionDlg::Show( bool bShow )
{ 
	if( bShow )
	{
	}
	else
	{
		// 블라인드같은게 열릴때 EtDialog::CloseAllDialog() 함수를 호출하기때문에,
		// 여기서 DelCallCaption해버리면, npc한테 말걸때도 삭제되게 된다.
		//DelAllCaption();
	}

	CEtUIDialog::Show( bShow );
}

void CDnSlideCaptionDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	// 음영부분의 알파 처리는 텍스트가 안보여도 수행되어야하므로(종료될때) 제일 위에서 처리.
	if( m_fFadeTime > 0.0f )
	{
		EtColorLerp( &m_renderColor, &m_srcColor, &m_destColor, 1.0f-(1/powf(m_fConst,2)*powf(m_fFadeTime,2) ) );
		m_fFadeTime -= fElapsedTime;
	}
	else
	{
		m_renderColor = m_destColor;
	}

	// 텍스트가 슬라이드되는 동안엔,
	if( m_bShowSlideText )
	{
		m_pCation->SetPosition( m_pCation->GetUICoord().fX - fElapsedTime / 10.0f, m_uiBlind.fY );

		// 왼쪽 화면으로 나갈 경우,
		if( m_pCation->GetUICoord().fX + m_fTextWidth < m_uiBlind.fX )
		{
			m_bShowSlideText = false;

			// Static컨트롤을 그릴 필요 없으니
			Show( false );

			if( m_listSlideInfo.empty() )
			{
				m_fConst = 1.0f;
				m_fFadeTime = 1.0f;
				m_srcColor = m_renderColor;
				m_destColor = (DWORD)0x00000000;
			}
		}
	}

	// 리스트 없을땐 처리하지 않는다.
	if( m_listSlideInfo.empty() )
		return;

	std::list<SSlideInfo>::iterator iter = m_listSlideInfo.begin();
	for( ; iter != m_listSlideInfo.end(); )
	{
		iter->m_fRemainShowTime -= fElapsedTime;
		if( iter->m_fRemainShowTime <= 0.0f )
		{
			iter = m_listSlideInfo.erase( iter );
			continue;
		}
		++iter;
	}

	// 모두 삭제되는거라면, 인덱스 초기화.
	if( m_listSlideInfo.empty() )
	{
		m_nLastSlideIndex = -1;
		return;
	}

	// 렌더링중인 텍스트가 없다면 텍스트 하나를 설정.
	if( !m_bShowSlideText )
	{
		// m_nLastSlideIndex를 참고해서 이번에 그려야할 iter를 얻어온다.
		int i = 0;
		std::list<SSlideInfo>::iterator iterNext = m_listSlideInfo.begin();
		for( ; iterNext != m_listSlideInfo.end(); )
		{
			++iterNext;
			if( i == m_nLastSlideIndex )
				break;
			++i;
		}

		// 유효한지 확인 후 Last갱신
		if( iterNext == m_listSlideInfo.end() )
		{
			// 공지 리스트범위보다 커서 넘어가면, 처음 것을 얻어온다.(순환)
			iterNext = m_listSlideInfo.begin();
			m_nLastSlideIndex = 0;

			// 그런데도, end()면, 아예 리스트가 없단 이야긴데, 정상적인 상황에선 일어날 수 없는 현상이다.
			if( iterNext == m_listSlideInfo.end() )
			{
				ASSERT(0&&"슬라이드 공지 리스트가 비워져있는데도 process가 돌아간다.");
				return;
			}
		}
		else
		{
			++m_nLastSlideIndex;
		}

		// 렌더링 할 텍스트에 정보를 보내둔다.
		m_RenderSlideInfo = *iterNext;

		// 스태틱 설정 후,
		SetCaption( m_RenderSlideInfo.m_wszMsg.c_str(), m_RenderSlideInfo.m_dwColor );

		// 영역 및 위치 계산.
		SUICoord textCoord;
		CalcTextRect( m_RenderSlideInfo.m_wszMsg.c_str(), m_pCation->GetElement(0), textCoord );
		m_fTextWidth = textCoord.fWidth;
		m_pCation->SetPosition( m_uiBlind.fX+m_uiBlind.fWidth, m_uiBlind.fY );

		// static 컨트롤을 그릴 수 있도록 Show 처리.
		Show( true );
		m_bShowSlideText = true;

		// 음영부분 블렌딩 처리.
		m_fConst = 1.0f;
		m_fFadeTime = 1.0f;
		if( m_renderColor.a != 0.0f )
			m_srcColor = m_renderColor;
		else
			m_srcColor = (DWORD)0x00000000;
		m_destColor = (DWORD)0x80000000;
	}
}

void CDnSlideCaptionDlg::Render( float fElapsedTime )
{
	if( m_renderColor.r > 0.0f || m_renderColor.g > 0.0f || m_renderColor.b > 0.0f )
	{
		ASSERT(0&&"도대체 누가 슬라이드 배경색에 컬러값 집어넣나");
		return;
	}

	// 아무래도 알파값으로만 확인을 하니 뭔가 잘못될 경우가 있는 거 같다.
	if( !m_bRender )
		return;

	if( m_renderColor.a == 0.0f )
		return;

	DrawRect( m_uiBlind, m_renderColor );

	// Show 여부와 상관없이 그려야할 텍스트가 있다면, 그린다.
	// 이게 있어야 블라인드등으로 인해 CEtUIDialog::CloseAllDialog등이 호출되어도
	// 진행중이던 블라인드 텍스트가 계속 렌더링될 수 있다.
	CEtUIControl *pControl(NULL);
	int nVecCtlSize = ( int )m_vecControl.size();
	for( int i = 0; i < nVecCtlSize; i++ )
	{
		pControl = m_vecControl[i];
		if( !pControl ) continue;
		pControl->Render( fElapsedTime );
	}
}

void CDnSlideCaptionDlg::AddCaption( LPCWSTR wszMsg, DWORD dwColor, float fShowTime )
{
	SSlideInfo Info;
	Info.SetInfo( wszMsg, dwColor, fShowTime );
	m_listSlideInfo.push_back( Info );
	m_bRender = true;
}

void CDnSlideCaptionDlg::CloseCaption( bool bForce )
{
	m_listSlideInfo.clear();

	if( bForce )
	{
		m_fConst = 1.0f;
		m_fFadeTime = 0.0f;
		m_srcColor = (DWORD)0x00000000;
		m_destColor = (DWORD)0x00000000;
		m_renderColor = (DWORD)0x00000000;

		m_bShowSlideText = false;
		Show( false );
		m_bRender = false;
	}
}

void CDnSlideCaptionDlg::DelCaption( int nIndex )
{
	if( m_listSlideInfo.empty() ) return;

	int i = 0;
	std::list<SSlideInfo>::iterator iter = m_listSlideInfo.begin();
	for( ; iter != m_listSlideInfo.end(); )
	{
		if( i == nIndex )
		{
			iter = m_listSlideInfo.erase( iter );
			return;
		}
		++iter;
		++i;
	}
}

void CDnSlideCaptionDlg::DelCaption( LPCWSTR wszMsg )
{
	if( m_listSlideInfo.empty() ) return;

	std::list<SSlideInfo>::iterator iter = m_listSlideInfo.begin();
	for( ; iter != m_listSlideInfo.end(); )
	{
		if( iter->m_wszMsg == wszMsg )
		{
			iter = m_listSlideInfo.erase( iter );
			return;
		}
		++iter;
	}
}

void CDnSlideCaptionDlg::SetCaption( LPCWSTR wszMsg, DWORD dwColor )
{
	m_pCation->SetText( wszMsg );
	m_pCation->SetTextColor( dwColor );
	m_pCation->Show(true);
}

void CDnSlideCaptionDlg::UpdateBlind()
{
	m_uiBlind.fX = -GetScreenWidthBorderSize();
	m_uiBlind.fY = -GetScreenHeightBorderSize();
	m_uiBlind.fWidth = GetScreenWidthRatio();
	m_uiBlind.fHeight = m_pCation->GetUICoord().fHeight;// * GetScreenHeightRatio();	// 왜 BlindDlg에선 이 Ratio를 곱한거지?
}

void CDnSlideCaptionDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();
	UpdateBlind();

	// 해상도에 따라 TextWidth도 바뀔 수 있으니 다시 계산한다.
	SUICoord textCoord;
	CalcTextRect( m_RenderSlideInfo.m_wszMsg.c_str(), m_pCation->GetElement(0), textCoord );
	m_fTextWidth = textCoord.fWidth;
}