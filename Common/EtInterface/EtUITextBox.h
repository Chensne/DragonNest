#pragma once
#include "EtUIControl.h"
#include "EtUITextUtil.h"
#include "EtUIScrollBar.h"
#include "EtUITextBoxData.h"
#include "EtUINameLinkInterface.h"

using namespace EtInterface;

#define TEXTBOX_RENDERLOCK

class CEtUINameLinkMgr;
class CEtUITextBox : public CEtUIControl, public CEtUINameLinkInterface
{
public:
	CEtUITextBox( CEtUIDialog *pParent );
	virtual ~CEtUITextBox(void);

	enum eNextPageCondition
	{
		NextPage_DlgScreenHeight,	// 부모다이얼로그의 세로 크기가 화면을 넘을 경우 다음 페이지로. 현재 이것만 지원.
		//NextPage_LineCount,			// 라인갯수가 일정 개수에 도달하면 다음 페이지로.
	};

protected:
	VECLINE m_vecLine;

	int m_nStartLine;
	int	m_nEndLine;

	int m_nCurLine;
	int m_nVisibleCount;

	bool m_bAutoScroll;			// 정렬 옵션 bottom일 경우 자동으로 스크롤 됩니다.(채팅창) 이때 이 값이 false라면 스크롤 하지 않습니다.

	float m_fSymbolWidth;		// 심볼 마크"..."의 가로크기
	float m_fTextMargin;		// 컨트롤의 높이에서 실제 보이는 라인의 높이를 뺀 나머지 높이

	CEtUIScrollBar m_ScrollBar;
	bool m_bUseSelect;

	// 텍스트박스의 LineData는 라인을 새로 만들때 값을 전달하는 용도로만 사용된다.
	SLineData m_sLineData;

	bool m_bUseWordBreak;
	int m_nMaxLine;

#ifdef TEXTBOX_RENDERLOCK
	volatile long m_nRenderingState;
#endif

	DWORD					m_RenderTextColor;
	typedef std::vector<CWord> SENTENCE;
	std::list<SENTENCE>		m_OriginalStrings;
	CWord					m_WordSelected;
	CWord					m_WordMouseOver;

	CEtUITextBox *m_pNextPageTextBox;
	eNextPageCondition m_eNextPageCondition;
	bool m_bAddTextToNextPage;	// NextPage에 등록해야하는지를 검사한 값.
	bool m_bNextPage;			// 자신이 NextPage라면 true.
	int m_nNextPageLineCount;	// 두번째 페이지부턴 첫번째 페이지의 맥스라인카운트를 따른다.
	int m_nAdjustValue;			// 두번째 페이지의 시작점이 첫번째 페이지와 다르면 맥스라인카운트도 다르다.
	void SetNextPage( bool bNextPage ) { m_bNextPage = bNextPage; }
	void SetNextPageLineCount( int nCount, int nAdjustValue );

protected:
	virtual void RenderText( float fElapsedTime );
	virtual void RenderLine( float fElapsedTime, CLine *sLine, bool bRollOver = false );
	virtual void RenderWord( float fElapsedTime, CWord& sWord, bool bRollOver = false, int nBorderFlag = 0 );

	void AddWord( const LPCWSTR szText, const LPCWSTR szTextWithTag, SWORD_PROPERTY &wordProperty );
	void AddLine();
	void AddImage( WCHAR *wszIamgeName, int nWidth, int nHeight );

	void SetMaxLine( int nValue ) { m_nMaxLine = nValue; }

	bool UpdateVisibleCount();
	void UpdateAlignText();
	void UpdateText();
	void UpdateText( float fX, float fY );

	void CalcSymbolTextRect();

	bool SelectLine( float fX, float fY );
	bool SelectWord(float fX, float fY);
	void SetMouseInWord( float fX, float fY );
	void SetMouseInLine( float fX, float fY );
	int GetMouseEnteredLine();
	CLine* GetEndLine();

protected:

	// Note : 텍스트 박스 고정
	void AppendTextF( const LPCWSTR szText, const LPCWSTR szTextWithTag, const D3DCOLOR TextColor, DWORD dwFormat, bool bAdd, const D3DCOLOR BgColor = 0 );
	// Note : 텍스트 박스 가변
	void AppendTextV( const LPCWSTR szText, const LPCWSTR szTextWithTag, const D3DCOLOR TextColor, DWORD dwFormat, bool bAdd, const D3DCOLOR BgColor = 0);

public:
	void UpdateTextBoxHeight();
	void UpdateTextBoxWidth();
	void UpdateTextBox();

	// Note : 텍스트를 기존 텍스트 라인에 추가한다.
	void AppendText( LPCWSTR szText, const D3DCOLOR TextColor = textcolor::WHITE, DWORD dwFormat = UITEXT_NONE, bool bAdd = false, const D3DCOLOR BgColor = 0, LPCWSTR szTextWithTag = L"" );

	// Note : 텍스트를 새로운 라인에 추가한다.
	void AddText( LPCWSTR szText, const D3DCOLOR TextColor = textcolor::WHITE, DWORD dwFormat = UITEXT_NONE, const D3DCOLOR BgColor = 0 );
	void DoAddText( LPCWSTR szText, LPCWSTR szTextWithTag, const D3DCOLOR TextColor = textcolor::WHITE, DWORD dwFormat = UITEXT_NONE, const D3DCOLOR BgColor = 0 );
	
	// Note : '\n'을 포함한 문장 또는 문단을 입력한다.
	void SetText( LPCWSTR szText, const D3DCOLOR TextColor = textcolor::WHITE );

	// 아이템 디스크립션이나, 시스템메세지에서 사용하는 컬러텍스트
	void AddColorText( LPCWSTR szText, const D3DCOLOR DefaultTextColor = textcolor::WHITE, const D3DCOLOR BgColor = 0, const D3DCOLOR DecreaseColor = 0 );

	void ClearText();

	// Note : 모든 라인의 줄 간격을 일괄적으로 설정한다.
	void SetLineSpace( float fSpace );

	void SetLineData( const SLineData &sLineData ) { m_sLineData = sLineData; }
	SLineData &GetLineData() { return m_sLineData; }
	void ClearLineData() { m_sLineData.Clear(); }

protected:
	int Scroll( int nScrollAmount );

public:

	void ScrollLineUp();
	void ScrollLineDown();
	void ScrollHome();
	void ScrollEnd();
	void ScrollPageUp();
	void ScrollPageDown();

	bool IsEmpty()								{ return m_vecLine.empty(); }
	bool IsScrollMode()							{ return m_nVisibleCount<(int)m_vecLine.size(); }
	bool IsLastPage();
	
	int  GetLineSize()                          { return (int)m_vecLine.size(); }
	std::wstring GetLineText(int nIndex);

	// Note : 텍스트 박스의 크기를 현재 보이는 라인의 높이로 크기를 맞춘다.
	void FitTextBoxToLine();

	// Note : NPC 대화창을 위한 페이지 넘김.
	void ScrollPageTurn();

	// Note : 한라인 추가될때 마다 자동으로 스크롤 한다.
	void SetAutoScroll( bool bAutoScroll )		{ m_bAutoScroll = bAutoScroll; }

	//int AddFont( const char *pFontName );
	//void SetFontHeight( int nFontHeight );

	virtual int GetScrollBarTemplate() { return m_Property.TextBoxProperty.nScrollBarTemplate; }
	virtual void SetScrollBarTemplate( int nIndex ) { m_Property.TextBoxProperty.nScrollBarTemplate = nIndex; }

	// Note : 현재 선택된 라인의 인덱스를 얻어온다.
	//
	int GetSelectedLineIndex( bool bClearSelection = true ) const;
	void ClearMouseEnteredLine();
	void ClearSelectedLine();

	bool GetSelectedLineData( SLineData &sLineData, bool bClearSelect = true ) const;
	const CWord& GetSelectedWordData() const;

	// 마지막 라인 정보를 구한다.
	std::wstring GetLastLineText();
	bool GetLastLineCoord( SUICoord &Coord );

	// 마지막 라인 지우기?
	void RemoveLastLine();

	// nLineSize = 몇번째줄 남길것인지 <1,2,3,4> 라인 사이즈 조절하고 남는 텍스트는 마지막에 심볼 붙임 , 사이즈가 클때 자르기 위한 용도로 사용
	void ResizeLineTextWithSymbol(int nLineSize , std::wstring wszSymbol);

	// WordBreak
	void EnableWorkBreak(bool bEnable = true) { m_bUseWordBreak = bEnable; }
	static int GetCaretWithWordBreak( const std::wstring& strText, const int nOrigCaret, bool bFirstWordInLine );

	// NextPage
	void SetNextPageTextBox( CEtUITextBox *pControl, eNextPageCondition eCondition = NextPage_DlgScreenHeight, int nAdjustValue = 0 );

	// NextPage에 내용이 채워졌는지 확인한다. 숫자가 0이면 안채워진거고, 1이면 첫번째 NextPage까지 채워진거고, 2면 두번째 NextPage까지 채워진 것이다.
	int GetUsedNextPage();

	virtual void FindInputPos( std::vector<EtVector2> &vecPos );

	// 라인의 문자열을 변경...
	

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void Render( float fElapsedTime );
	virtual bool CanHaveFocus() { return ( IsShow() && IsEnable() ); }
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual void UpdateRects();
	virtual void Process( float fElapsedTime );
};