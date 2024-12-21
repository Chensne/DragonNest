#pragma once

#include "EtUIDialog.h"

class CDnChatBalloonDlg : public CEtUIDialog
{
private :
	CEtUITextBox *	m_pTextBox;
	CEtUIStatic	 *	m_pStaticTail;
	CEtUIStatic  *  m_pStaticMark;

	std::wstring	m_wszChatMsg;
	DWORD			m_dwTextColor;

	float			m_fMaxLineWidth;
	float			m_fDepth;
	float			m_fMaxWitdh;

public:
	CDnChatBalloonDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatBalloonDlg();

	virtual void InitialUpdate();
	virtual void Initialize( const char *pFileName, bool bShow );
	virtual void Show( bool bShow );
	virtual void Render( float fElapsedTime );
	virtual void OnChangeResolution();

	void SetTextLine( LPCWSTR nText, DWORD dwTextColor, DWORD dwTextFormat = UITEXT_NONE );
	void SetChatMsg( LPCWSTR nText, DWORD dwTextColor, int nWidthSize = 8 , DWORD dwTextFormat = UITEXT_NONE );
	void SetDepth( const float depth );

	void CalcTailPosition();
	DWORD GetTextColor() { return m_dwTextColor; }

	// PR채팅에 쓰는 말풍선같은 경우엔 PR글 길이가 제각각 달라,
	// 로테이션 될때마다 채팅글 가로길이가 계속 변하니 이상하다고 한다.
	// 그래서 가장 긴걸로 텍스트박스 길이를 조정한다.
	void SetMaxWidthFromText( LPCWSTR wszText );
};