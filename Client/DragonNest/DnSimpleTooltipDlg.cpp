#include "StdAfx.h"
#include "DnSimpleTooltipDlg.h"
#include "DnCommonUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSimpleTooltipDlg::CDnSimpleTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pTextBox(NULL)
	, m_pTooltipCtl(NULL)
	, m_fDelayTime(0.0f)
	, m_bPermanent(false)
	, m_TextColor(textcolor::WHITE)
	, m_bTopPosition( false )
	, m_bBottomPosition( false )
	, m_fDeltaPositionX( 0.f )
	, m_fDeltaPositionY( 0.f )
	, m_fOriginalWidth( 0.f )
	, m_bRefreshControlTooltip( false )
	, m_fRefreshTime( 0.5f )
{
}

CDnSimpleTooltipDlg::~CDnSimpleTooltipDlg(void)
{
}

void CDnSimpleTooltipDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SimpleTooltipDlg.ui" ).c_str(), bShow );
	//SetFadeMode(true);
}

void CDnSimpleTooltipDlg::InitialUpdate()
{
	m_pTextBox = GetControl<CEtUITextBox>("ID_TEXTBOX");
	m_fOriginalWidth = m_pTextBox->GetUICoord().fWidth;
}

void CDnSimpleTooltipDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		UpdateTooltipDlg();

		//// Note : 월드맵이나 존맵의 경우는 다이나믹하게 콘트롤이 생성되었다가 삭제되므로
		////		아래 코드에서 잘못된 포인트를 참조하게 된다.
		////
		//if( m_pTooltipCtl )
		//{
		//	CEtUIDialog *pParentDlg = m_pTooltipCtl->GetParent();
		//	if( pParentDlg && !pParentDlg->IsShow() )
		//	{
		//		m_fDelayTime = 0.0f;
		//		Show(false);
		//		return;
		//	}
		//}

		if (m_bPermanent == false)
		{
			if( m_fDelayTime <= 0.0f )
			{
				Show(false);
			}
			else
			{
				m_fDelayTime -= fElapsedTime;
			}
		}

		if( m_bRefreshControlTooltip )
		{
			if( m_fRefreshTime < 0.f )
			{
				UpdateTooltipText();
				m_fRefreshTime = 0.5f;
			}
			else
			{
				m_fRefreshTime -= fElapsedTime;
			}
		}

	}
}

void CDnSimpleTooltipDlg::Show( bool bShow )
{ 
	if( bShow )
	{
		m_bTopPosition = false;
		m_bBottomPosition = false;
		m_fDelayTime = 30.0f;	// 1초 보여지던거에서 초 늘림. 안없어지기 원하면 bPermanent기본값을 true로 하자.

		ApplyTooltipInfo();
		UpdateTooltipText();
		UpdateTooltipDlg();
	}
	else
	{
		m_pTooltipCtl = NULL;
		CEtUIDialog::s_nTooltipStringIndex = 0;
		CEtUIDialog::s_strTooltipString.clear();
		SetDeltaPosition( 0.f, 0.f );
		ClearTooltipInfo();
	}

	m_pTextBox->Show( bShow );
	CEtUIDialog::Show( bShow );
}

void CDnSimpleTooltipDlg::UpdateTooltipText()
{
	m_pTextBox->ClearText();

	if( CEtUIDialog::s_nTooltipStringIndex > 0 )
	{
		m_pTextBox->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CEtUIDialog::s_nTooltipStringIndex ), m_TextColor );
	}
	else
	{
		if( m_bRefreshControlTooltip && m_pTooltipCtl )
		{
			m_pTextBox->AddColorText( m_pTooltipCtl->GetTooltipText().c_str() , m_TextColor );
		}
		else
		{
			m_pTextBox->AddColorText( s_strTooltipString.c_str(), m_TextColor );
		}
	}
}


void CDnSimpleTooltipDlg::UpdateTooltipDlg()
{
	SUICoord uiCoord;
	GetDlgCoord( uiCoord );
	GetScreenMouseMovePoints( uiCoord.fX, uiCoord.fY );
	uiCoord.fX -= 0.01f;

	if(m_pTextBox && m_pTextBox->GetLineSize() > 0)
		uiCoord.fY -= uiCoord.fHeight * GetScreenHeightRatio();

	if( m_bTopPosition )
		uiCoord.fY -= uiCoord.fHeight / 2.0f;
	if( m_bBottomPosition )
		uiCoord.fY += uiCoord.fHeight + 0.04f; // 마우스 포인트에 가려서 넣은듯싶습니다.

	if( uiCoord.Right() > GetScreenWidthRatio() )
	{
		uiCoord.fX -= (uiCoord.Right() - GetScreenWidthRatio() - 0.001f);
	}

	if( uiCoord.Bottom() > GetScreenHeightRatio() )
	{
		uiCoord.fY -= (uiCoord.Bottom() - GetScreenHeightRatio() - 0.001f);
	}

	CommonUtil::ClipNumber(uiCoord.fY, 0.f, uiCoord.fY);

	uiCoord.fX += m_fDeltaPositionX;
	uiCoord.fY += m_fDeltaPositionY;

	SetDlgCoord( uiCoord );
}

void CDnSimpleTooltipDlg::SetTooltipInfo(DWORD color, bool bPermanent )
{
	m_TextColor		= color;
	m_bPermanent	= bPermanent;
}

void CDnSimpleTooltipDlg::ClearTooltipInfo()
{
	m_TextColor		= textcolor::WHITE;
	m_bPermanent	= false;
	m_bRefreshControlTooltip = false;
}

CEtUIControl* CDnSimpleTooltipDlg::GetTooltipCtrl() const
{
	return m_pTooltipCtl;
}

void CDnSimpleTooltipDlg::SetDeltaPosition( const float fX, const float fY )
{
	m_fDeltaPositionX = fX;
	m_fDeltaPositionY = fY;
}

void CDnSimpleTooltipDlg::SetPermanent( bool bPermanent )
{
	m_bPermanent = bPermanent;
}

void CDnSimpleTooltipDlg::SetDelayTime( float fDelayTime )
{
	m_fDelayTime = fDelayTime;
}


// 다이얼로그의 속성에 따라서 툴팁에 변화를 주는경우 처리

void CDnSimpleTooltipDlg::ApplyTooltipInfo()
{
	if( m_pTooltipCtl )
	{
		const int ADD_BASE_GAP = 10;
		float LIMIT_WIDTH = m_pTooltipCtl->GetTooltipInfo().fWidthSize;
		m_bRefreshControlTooltip =  m_pTooltipCtl->GetTooltipInfo().bRefresh;
		m_bBottomPosition = m_pTooltipCtl->GetTooltipInfo().bBottomPosition;

		if( m_pTooltipCtl->GetTooltipInfo().fWidthSize > 0.f )
		{
			SUICoord sTextCoord;
			CalcTextRect( s_strTooltipString.c_str() , m_pTextBox->GetElement(0) , sTextCoord );

			if( sTextCoord.fWidth > LIMIT_WIDTH / DEFAULT_UI_SCREEN_WIDTH )
			{
				m_pTextBox->GetUICoord().fWidth = LIMIT_WIDTH / DEFAULT_UI_SCREEN_WIDTH;
				m_pTextBox->GetProperty()->TextBoxProperty.VariableType = UI_TEXTBOX_HEIGHT;
				m_DlgInfo.DlgCoord.fWidth = (LIMIT_WIDTH + ADD_BASE_GAP) / DEFAULT_UI_SCREEN_WIDTH;
			}
		}
		else if( m_pTextBox->GetProperty()->TextBoxProperty.VariableType != UI_TEXTBOX_BOTH )
		{
			m_pTextBox->GetUICoord().fWidth = m_fOriginalWidth / DEFAULT_UI_SCREEN_WIDTH;
			m_pTextBox->GetProperty()->TextBoxProperty.VariableType = UI_TEXTBOX_BOTH;
			m_DlgInfo.DlgCoord.fWidth = (m_fOriginalWidth + ADD_BASE_GAP) / DEFAULT_UI_SCREEN_WIDTH;
		}
	}
}

