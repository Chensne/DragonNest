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
		// ����ε尰���� ������ EtDialog::CloseAllDialog() �Լ��� ȣ���ϱ⶧����,
		// ���⼭ DelCallCaption�ع�����, npc���� ���ɶ��� �����ǰ� �ȴ�.
		//DelAllCaption();
	}

	CEtUIDialog::Show( bShow );
}

void CDnSlideCaptionDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	// �����κ��� ���� ó���� �ؽ�Ʈ�� �Ⱥ����� ����Ǿ���ϹǷ�(����ɶ�) ���� ������ ó��.
	if( m_fFadeTime > 0.0f )
	{
		EtColorLerp( &m_renderColor, &m_srcColor, &m_destColor, 1.0f-(1/powf(m_fConst,2)*powf(m_fFadeTime,2) ) );
		m_fFadeTime -= fElapsedTime;
	}
	else
	{
		m_renderColor = m_destColor;
	}

	// �ؽ�Ʈ�� �����̵�Ǵ� ���ȿ�,
	if( m_bShowSlideText )
	{
		m_pCation->SetPosition( m_pCation->GetUICoord().fX - fElapsedTime / 10.0f, m_uiBlind.fY );

		// ���� ȭ������ ���� ���,
		if( m_pCation->GetUICoord().fX + m_fTextWidth < m_uiBlind.fX )
		{
			m_bShowSlideText = false;

			// Static��Ʈ���� �׸� �ʿ� ������
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

	// ����Ʈ ������ ó������ �ʴ´�.
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

	// ��� �����Ǵ°Ŷ��, �ε��� �ʱ�ȭ.
	if( m_listSlideInfo.empty() )
	{
		m_nLastSlideIndex = -1;
		return;
	}

	// ���������� �ؽ�Ʈ�� ���ٸ� �ؽ�Ʈ �ϳ��� ����.
	if( !m_bShowSlideText )
	{
		// m_nLastSlideIndex�� �����ؼ� �̹��� �׷����� iter�� ���´�.
		int i = 0;
		std::list<SSlideInfo>::iterator iterNext = m_listSlideInfo.begin();
		for( ; iterNext != m_listSlideInfo.end(); )
		{
			++iterNext;
			if( i == m_nLastSlideIndex )
				break;
			++i;
		}

		// ��ȿ���� Ȯ�� �� Last����
		if( iterNext == m_listSlideInfo.end() )
		{
			// ���� ����Ʈ�������� Ŀ�� �Ѿ��, ó�� ���� ���´�.(��ȯ)
			iterNext = m_listSlideInfo.begin();
			m_nLastSlideIndex = 0;

			// �׷�����, end()��, �ƿ� ����Ʈ�� ���� �̾߱䵥, �������� ��Ȳ���� �Ͼ �� ���� �����̴�.
			if( iterNext == m_listSlideInfo.end() )
			{
				ASSERT(0&&"�����̵� ���� ����Ʈ�� ������ִµ��� process�� ���ư���.");
				return;
			}
		}
		else
		{
			++m_nLastSlideIndex;
		}

		// ������ �� �ؽ�Ʈ�� ������ �����д�.
		m_RenderSlideInfo = *iterNext;

		// ����ƽ ���� ��,
		SetCaption( m_RenderSlideInfo.m_wszMsg.c_str(), m_RenderSlideInfo.m_dwColor );

		// ���� �� ��ġ ���.
		SUICoord textCoord;
		CalcTextRect( m_RenderSlideInfo.m_wszMsg.c_str(), m_pCation->GetElement(0), textCoord );
		m_fTextWidth = textCoord.fWidth;
		m_pCation->SetPosition( m_uiBlind.fX+m_uiBlind.fWidth, m_uiBlind.fY );

		// static ��Ʈ���� �׸� �� �ֵ��� Show ó��.
		Show( true );
		m_bShowSlideText = true;

		// �����κ� ���� ó��.
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
		ASSERT(0&&"����ü ���� �����̵� ������ �÷��� ����ֳ�");
		return;
	}

	// �ƹ����� ���İ����θ� Ȯ���� �ϴ� ���� �߸��� ��찡 �ִ� �� ����.
	if( !m_bRender )
		return;

	if( m_renderColor.a == 0.0f )
		return;

	DrawRect( m_uiBlind, m_renderColor );

	// Show ���ο� ������� �׷����� �ؽ�Ʈ�� �ִٸ�, �׸���.
	// �̰� �־�� ����ε������ ���� CEtUIDialog::CloseAllDialog���� ȣ��Ǿ
	// �������̴� ����ε� �ؽ�Ʈ�� ��� �������� �� �ִ�.
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
	m_uiBlind.fHeight = m_pCation->GetUICoord().fHeight;// * GetScreenHeightRatio();	// �� BlindDlg���� �� Ratio�� ���Ѱ���?
}

void CDnSlideCaptionDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();
	UpdateBlind();

	// �ػ󵵿� ���� TextWidth�� �ٲ� �� ������ �ٽ� ����Ѵ�.
	SUICoord textCoord;
	CalcTextRect( m_RenderSlideInfo.m_wszMsg.c_str(), m_pCation->GetElement(0), textCoord );
	m_fTextWidth = textCoord.fWidth;
}