#pragma once
#include "EtUIEditBox.h"
#include "EtUIIME.h"

class CEtUIIMEEditBox : public CEtUIEditBox, public CEtUIIME
{
public:
	CEtUIIMEEditBox( CEtUIDialog *pParent );
	virtual ~CEtUIIMEEditBox(void);

protected:
	DWORD m_dwCompTargetNonBkColor;
	DWORD m_dwCompTargetBkColor;
	DWORD m_dwCompTargetColor;
	DWORD m_dwCompTargetNonColor;
	SUICoord m_IndicatorCoord;

	// Note: 조합중인 문자열이 너무 길어서 완성된 문자열을 밀어내서 하나도 보이지 않게 되는 경우를 막는다.
	// 안그러면 m_nFirstVisible 값이 문자열 길이를 넘어가게 되어버림.
	bool m_bFirstVisibleOver;
	int m_nTempCompSize;

	// 일본 후보리스트 렌더링할때 기본적으로는 조합중인 글자 아래에 렌더링하고,
	// 다이얼로그 영역을 넘어간다면 위에다 렌더링한다.
	// 이걸 강제로 아래에다 렌더링하려면 true로 바꿔주면 된다.
	bool m_bForceBottomPosition;

public:
	void SetCompTextColor( DWORD dwColor ) { m_Property.IMEEditBoxProperty.dwCompTextColor = dwColor; }
	void CheckIMEToggleState();
	void CancelIMEComposition();
	void GetIMECompositionString(std::wstring& str);

	int GetMaxChar() const;
	void SetMaxChar( DWORD dwChar );

	void ForceRenderCandidateReadingWindowBottomPosition( bool bForce ) { m_bForceBottomPosition = bForce; }

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void SetTemplate( int nTemplateIndex );

	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void Focus( bool fFocus );

	virtual void ClearText();
	virtual void Show( bool bShow );

	void PumpMessage();

	virtual void UpdateRects();

	virtual void Render( float fElapsedTime );
	virtual void RenderCandidateReadingWindow( float fElapsedTime, bool bReading );
	virtual void RenderComposition( float fElapsedTime );
	virtual void RenderIndicator( float fElapsedTime );

protected:
	void TruncateCompString( bool bUseBackSpace = true, int iNewStrLen = 0 );
	void FinalizeString( bool bSend );
	void SendCompString();
	// Note : IME에서 문자조합시 문자열이 벗어나는 것을 조정해준다.
	//		실제 현재 커서를 수정하지는 않는다. 문자열의 시작 위치만 조정한다.
	//		조합중인 문자의 수를 넘겨준다.
	void PlaceCaret( int nCP );
};
