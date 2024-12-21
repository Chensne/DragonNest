#include "StdAfx.h"
#include "DnChatBalloonDlg.h"
#include "DnActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChatBalloonDlg::CDnChatBalloonDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pTextBox(NULL)
, m_pStaticTail(NULL)
, m_pStaticMark(NULL)
{
	m_fMaxLineWidth = 1.0f;
	m_fMaxWitdh = 0.0f;
	m_dwTextColor = 0;
	m_fDepth = 0.f;
}

CDnChatBalloonDlg::~CDnChatBalloonDlg()
{
}

void CDnChatBalloonDlg::Initialize( const char *pFileName, bool bShow )
{
	CEtUIDialog::Initialize( pFileName, bShow );
}

void CDnChatBalloonDlg::InitialUpdate()
{
	m_pTextBox = GetControl<CEtUITextBox>( "ID_TEXTBOX" );
	m_pStaticTail = GetControl<CEtUIStatic>( "ID_STATIC_TAIL" );
	m_pStaticMark = GetControl<CEtUIStatic>( "ID_ICON_MARK" );
}

void CDnChatBalloonDlg::Show( bool bShow ) 
{ 
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnChatBalloonDlg::Render( float fElapsedTime )
{
	if( !IsAllowRender() ) return;

	SUICoord DlgCoord(m_DlgInfo.DlgCoord), DlgUVCoord(0.0f, 0.0f, 1.0f, 1.0f);
	DlgCoord.fX = 0.0f;
	DlgCoord.fY = 0.0f;

	EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
	if( !hCamera ) return;

	if( m_hDlgTexture )
	{
		if( (m_DlgInfo.nFrameLeft > 0) || (m_DlgInfo.nFrameRight > 0) || (m_DlgInfo.nFrameTop > 0) || (m_DlgInfo.nFrameBottom > 0) )
		{
			int nFrameSize = ( int )m_vecDlgFrameElement.size();

			for( int i = 0; i < nFrameSize; i++ )
			{
				DrawSprite( m_hDlgTexture, m_vecDlgFrameElement[ i ].UVCoord, m_renderDlgColor.dwCurrentColor, m_vecDlgFrameCoord[ i ], 0.0f, m_fDepth );
			}
		}
		else
		{
			DrawSprite( m_hDlgTexture, DlgUVCoord, m_renderDlgColor.dwCurrentColor, DlgCoord, m_fDepth );
		}
	}
	else if( (m_renderDlgColor.dwCurrentColor & 0xFF000000) != 0 )
	{
		DrawRect( DlgCoord, m_renderDlgColor.dwCurrentColor );
	}

	m_pTextBox->Render( fElapsedTime );
	
	if( m_pStaticTail )
	{
		m_pStaticTail->Render( fElapsedTime );
	}

	if( m_pStaticMark )
	{
		m_pStaticMark->Render( fElapsedTime );
	}
}

void CDnChatBalloonDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();

	if( !m_pTextBox ) return;
	if( !IsShow() ) return;

	SetTextLine( m_wszChatMsg.c_str(), m_dwTextColor );
}

void CDnChatBalloonDlg::CalcTailPosition() // 말꼬리 위치 설정.
{
	SUICoord Coord = m_pStaticTail->GetUICoord();
	SUICoord DlgCoord = GetDlgCoord();
	float fYDelta = 4.0f / DEFAULT_UI_SCREEN_HEIGHT;
	Coord.fX = (DlgCoord.fWidth - Coord.fWidth) / 2.0f;
	Coord.fY = DlgCoord.fHeight - fYDelta;
	m_pStaticTail->SetUICoord( Coord );
}

static DWORD TEXT_WIDTH = 250;

void CDnChatBalloonDlg::SetChatMsg( LPCWSTR nText, DWORD dwTextColor, int nWidthSize , DWORD dwTextFormat )
{
	static int MAX_LINE = 4;
	std::wstring strMsg = nText;	
	std::vector<std::wstring> vecTempMsg;

	EtInterface::GetNameLinkMgr().TranslateText(strMsg, nText);

	if( CEtFontMng::s_bUseUniscribe )
	{
		int nMaxWidth;
		if( !CEtFontMng::GetInstance().GetWordBreakText( strMsg, CDnActor::s_nFontIndex, 14, TEXT_WIDTH, vecTempMsg, nMaxWidth ) )
			return;
	}
	else
	{
		if( CEtFontMng::s_bUseWordBreak ) {
				for( int i = 0; i < MAX_LINE && !strMsg.empty() ; i++) {
				int nTrail=0;
				int nCaret = CEtFontMng::GetInstance().GetCaretFromCaretPos( strMsg.c_str(), CDnActor::s_nFontIndex, 14, TEXT_WIDTH, nTrail );
				if( nCaret < (int)strMsg.size()-1 ) {
					nCaret = CEtUITextBox::GetCaretWithWordBreak( strMsg, nCaret, true );
					vecTempMsg.push_back( strMsg.substr(0, nCaret) );
					strMsg = strMsg.substr( nCaret ).c_str();
				}
			}

			if(!strMsg.empty())
				vecTempMsg.push_back(strMsg);
		}
		else {
			for( int i = 0; i < MAX_LINE && !strMsg.empty() ; i++) {
				int nTrail=0;
				int nCaret = CEtFontMng::GetInstance().GetCaretFromCaretPos( strMsg.c_str(), CDnActor::s_nFontIndex, 14, TEXT_WIDTH, nTrail );
				if( nCaret < (int)strMsg.size()-1 ) {
					vecTempMsg.push_back( strMsg.substr(0, nCaret+nTrail) );
					strMsg = strMsg.substr( nCaret+nTrail ).c_str();
				}
			}

			if(!strMsg.empty())
				vecTempMsg.push_back(strMsg);
		}
	}

	m_pTextBox->ClearText();

	for(DWORD i=0;i<vecTempMsg.size();i++)
		m_pTextBox->AddText( vecTempMsg[i].c_str(), dwTextColor, dwTextFormat );

	m_wszChatMsg = nText;
	m_dwTextColor = dwTextColor;

	CalcTailPosition();

}

void CDnChatBalloonDlg::SetTextLine( LPCWSTR nText, DWORD dwTextColor, DWORD dwTextFormat )
{
	m_pTextBox->ClearText();
	if( dwTextFormat == UITEXT_NONE )
		m_pTextBox->SetText( nText, dwTextColor );
	else
		m_pTextBox->AddText( nText, dwTextColor, dwTextFormat );

	m_wszChatMsg = nText;
	m_dwTextColor = dwTextColor;

	CalcTailPosition();
}

void CDnChatBalloonDlg::SetDepth( const float depth )
{
	m_fDepth = depth;
	m_pTextBox->SetZValue(depth);
	m_pStaticTail->SetZValue(depth);
	m_pStaticMark->SetZValue(depth);
}

void CDnChatBalloonDlg::SetMaxWidthFromText( LPCWSTR wszText )
{
	SUICoord textCoord;
	CalcTextRect( wszText, m_pTextBox->GetElement(0), textCoord );
	if( textCoord.fWidth > m_fMaxWitdh )
	{
		// 강제로 텍스트박스의 Variable Type을 가로 가변으로 바꿔서 길이를 이번 텍스트에 맞춘다.
		SUIControlProperty *pProperty = m_pTextBox->GetProperty();
		pProperty->TextBoxProperty.VariableType = UI_TEXTBOX_WIDTH;
		m_pTextBox->ClearText();
		m_pTextBox->SetText( wszText );

		// 다음번에 다른 텍스트가 오더라도 길이 변화하지 말라고 Variable Type을 None으로 바꿔놓는다.
		pProperty->TextBoxProperty.VariableType = UI_TEXTBOX_NONE;
		m_fMaxWitdh = textCoord.fWidth;
	}
}