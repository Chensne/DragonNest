#pragma once
#include "EtUIControl.h"
#include "EtUIUniBuffer.h"
#include "EtUINameLinkInterface.h"

class CEtUINameLinkMgr;
class CEtUIEditBox : public CEtUIControl, public CEtUINameLinkInterface
{
public:
	enum eFilterOption
	{
		eDIGIT		= 0x0001,
		eALPHABET	= 0x0002,
	};
	CEtUIEditBox( CEtUIDialog *pParent );
	virtual ~CEtUIEditBox(void);

	static SUICoord s_CurrentCaretPos;
	static bool s_bCheckThaiLanguageVowelRule;

protected:
	CEtUIUniBuffer m_Buffer;
	CEtUIUniBuffer m_RenderBuffer;
	SUICoord m_TextCoord;
	SUICoord m_RenderCoord[ 5 ];

	float m_fCaretBlinkTime;
	LARGE_INTEGER m_liLastBlinkTime;
	bool m_bCaretOn;
	int m_nCaret;
	bool m_bInsertMode;
	int m_nSelStart;
	int m_nFirstVisible;
	int m_nLastVisible;

	bool m_bMouseDrag;

	// 조합중인 문자열의 길이 
	float m_fCompositionWidth;
	int m_nFilterOption;

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void SetTemplate( int nTemplateIndex );

	virtual bool CanHaveFocus() { return ( IsShow() && IsEnable() ); }
	virtual void Focus( bool bFocus );

	void SetText( LPCWSTR wszText, bool bSelected = false );
	void SetTextWithEllipsis(const wchar_t* text, const wchar_t* wszSymbol, bool bSelected = false);
	virtual LPCWSTR GetText() { return m_FullTextBuffer.GetBuffer(); }
	int GetFullTextLength() const { return m_FullTextBuffer.GetTextSize(); }
	int GetTextLength() { return m_Buffer.GetTextSize(); }
	HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );

	void AddText(LPCWSTR wszText, bool bFocus);
	void SetFilterOption(int option);

	virtual void ClearText();
	virtual void ClearTextMemory();
	virtual void SetTextColor( DWORD dwColor ) { m_Property.EditBoxProperty.dwTextColor = dwColor; }
	void SetSelectedTextColor( DWORD dwColor ) { m_Property.EditBoxProperty.dwSelTextColor = dwColor; }
	void SetSelectedBackColor( DWORD dwColor ) { m_Property.EditBoxProperty.dwSelBackColor = dwColor; }
	void SetCaretColor( DWORD dwColor ) { m_Property.EditBoxProperty.dwCaretColor = dwColor; }
	void ParseFloatArray( float *pNumbers, int nCount );
	void SetTextFloatArray( const float *pNumbers, int nCount );
	void UpdateLastVisible( void );

	virtual int GetTextToInt();
	virtual void SetIntToText( int nValue );
	virtual INT64 GetTextToInt64();
	virtual void SetInt64ToText( INT64 nValue );
	// SetMaxEditLength 와 동일한 함수같은데 왜 따로 만들었는지 모르겠지만 의도를 몰라 그냥 여기 마춰서 작업합니다.
	virtual int GetMaxChar() const;
	virtual void SetMaxChar( DWORD dwChar );

	void	SetSelection(int startPos, int caretPos);
	int		GetCaretPos() const { return m_nCaret; }

public:
	//void SetFont( ID3DXFont *pFont );
	void SetMaxEditLength( DWORD dwMaxLength );

	static void AddInvalidCharacter( wchar_t wCharStart, wchar_t wCharEnd );

	static int	CheckThaiLanguageVowel( WCHAR wChar );
	static int	GetThaiLength( const WCHAR* pWChar );
	static WCHAR	GetThaiStartChar();
	static int		GetThaiBelowVowelStartIndex();

protected:
	void PlaceCaret( int nCP );
	void DeleteSelectionText();
	void ResetCaretBlink();
	void CopyToClipboard();
	void PasteFromClipboard();

	bool OnParseTextColor(DWORD color);
	bool OnParseTextFailColor();
	bool OnParseTextRestoreColor();

	void ProcessHalfWidth(WPARAM &wParam);
	bool CheckValidCharacter(WCHAR wChar);
	static std::vector<WCHAR> s_vecInvalidCharater;

	void		RenderText(float fElapsedTime, SUIElement* pElement, const SUICoord& TempCoord, bool bNeedDivide);

	bool		DrawTextPiece(const std::wstring& piece, int pieceStart, int pieceEnd, SUIElement* pElement, SUICoord& drawCoord);
	int			DrawTextWithTraslation(const WCHAR* originalStr, SUIElement* pElement, SUICoord& drawCoord);

	int			GetFullTextCaretPos(int caretPos, bool bRetWithCalcTagLast) const;
	int			GetTextCaretPos(int fullTextCaretPos);
	void		GetFullTextTagFieldPos(int& tagFieldStart, int& tagFieldEnd, int curCursorPos) const;

	bool		InsertChar(WCHAR wChar);
	bool		OverTypeChar(WCHAR wChar);
	bool		RemoveChar(int startCaretPos, int length);

	bool		CheckThaiLanguageVowelRule( WCHAR wChar );
	int			GetPrevCaret();
	int			GetNextCaret();
	int			GetTextMoveCount( int nMoveCount, bool bForward );

	DWORD					m_RenderTextColor;
	CEtUIUniBuffer			m_FullTextBuffer;
	int						m_FullTextCaret;
	std::vector<int>		m_FullTextCaretMapping;

public:
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void UpdateRects();
	virtual void Render( float fElapsedTime );
};

