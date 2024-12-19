#pragma once
#include "EtUIControl.h"
#include "EtUIUniBuffer.h"

class CEtUILineEditBox : public CEtUIControl
{
public:
	CEtUILineEditBox( CEtUIDialog *pParent );
	virtual ~CEtUILineEditBox(void);

protected:
	struct SBufferInfo
	{
		CEtUIUniBuffer* pUniBuffer;
		CEtUIUniBuffer* pOriginUniBuffer;
		int nCaretPos;
		int nOriginCaretPos;
		SUICoord TextCoord;

		SBufferInfo *pPrevLine;
		SBufferInfo *pNextLine;

		SBufferInfo( /*ID3DXFont *pFont*/ )
			: nCaretPos(0)
			, nOriginCaretPos(0)
			, pPrevLine(NULL)
			, pNextLine(NULL)
		{
			//ASSERT( pFont );
			pUniBuffer = new CEtUIUniBuffer;
			pUniBuffer->SetMaxEditBoxLength( 200 );
			pOriginUniBuffer = new CEtUIUniBuffer;
		}

		~SBufferInfo()
		{
			SAFE_DELETE( pUniBuffer );
			SAFE_DELETE( pOriginUniBuffer );
		}

		bool IsEndCaret() { return (nCaretPos==pUniBuffer->GetTextSize()); }

		int GetTextSize() { return pUniBuffer->GetTextSize(); }
		int GetOriginTextSize() { return pOriginUniBuffer->GetTextSize(); }
		bool InsertChar( int nIndex, WCHAR wChar ) { return pUniBuffer->InsertChar( nIndex, wChar ); }
		bool InsertOriginChar( int nIndex, WCHAR wChar ) { return pOriginUniBuffer->InsertChar( nIndex, wChar ); }
		bool RemoveChar( int nIndex ) { return pUniBuffer->RemoveChar( nIndex ); }
		bool RemoveOriginChar( int nIndex ) { return pOriginUniBuffer->RemoveChar( nIndex ); }
		HRESULT CPtoX( int nCP, BOOL bTrail, int *pX ) { return pUniBuffer->CPtoX( nCP, bTrail, pX ); }
		HRESULT XtoCP( int nX, int *pCP, int *pnTrail ) { return pUniBuffer->XtoCP( nX, pCP, pnTrail ); }

		bool IsInside( float fX, float fY ) { return TextCoord.IsInside( fX, fY ); }
	};

	std::list<SBufferInfo*> m_listBuffer;
	int m_nCurLine;
	//ID3DXFont *m_pFont;

	float m_fCaretBlinkTime;
	LARGE_INTEGER m_liLastBlinkTime;
	bool m_bCaretOn;
	bool m_bInsertMode;

	int m_nSelStartPos;
	int m_nSelEndPos;
	int m_nOriginSelStartPos;
	int m_nOriginSelEndPos;
	int m_nSelStartLine;
	int m_nSelEndLine;

	bool m_bMouseDrag;

	float m_fCompositionWidth;
	int m_nMaxTextLength;

protected:
	void DeleteBufferList();

	void SetCurrentLine( int nLineIndex );
	SBufferInfo *GetCurrentLine();
	SBufferInfo *GetLine( int nLineIndex );
	SBufferInfo *AddLine( SBufferInfo *pLine );
	bool AppendLine( SBufferInfo *pLine );
	void DeleteLine( SBufferInfo *pLine );

	void UpCurrentLine();
	void DownCurrentLine();

	bool IsBeginLine( SBufferInfo *pLine );
	bool IsEndLine( SBufferInfo *pLine );

	void InsertCharLine( SBufferInfo *pLine, int nIndex, WCHAR wChar, bool bSetText = false, bool bLineFeed = false );
	bool RemoveCharLine( SBufferInfo *pLine, int nIndex );
	bool RemoveOriginCharLine( SBufferInfo* pBufferInfo, int nIndex );

	void PasteTrailFromLine( SBufferInfo *pDestLine, SBufferInfo *pSrcLine );
	void AdjustCaretPosition( SBufferInfo *pCurLine, SBufferInfo *pNextLine );
	void SplitLineToAdd( SBufferInfo *pLine );

	bool IsMaxLine();
	int GetLineMouseIn( float fX, float fY );
	bool IsTextSelected() { return ( (m_nSelStartPos != m_nSelEndPos) || (m_nSelStartLine != m_nSelEndLine) ); }

	void SetSelectStartPos();
	void SetSelectEndPos();

public:
	virtual void Initialize( SUIControlProperty *pProperty );
	virtual void SetTemplate( int nTemplateIndex );

	virtual bool CanHaveFocus() { return ( IsShow() && IsEnable() ); }
	virtual void Focus( bool bFocus );

	//void SetText( LPCWSTR wszText, bool bSelected = false );
	//virtual LPCWSTR GetText() { return m_Buffer.GetBuffer(); }
	HRESULT GetTextCopy( LPWSTR strDest, UINT bufferCount );
	HRESULT GetOriginTextCopy( LPWSTR strDest, UINT bufferCount );
	void ClearText();

	bool IsEmpty();

	virtual void SetTextColor( DWORD dwColor ) { m_Property.LineEditBoxProperty.dwTextColor = dwColor; }
	void SetSelectedTextColor( DWORD dwColor ) { m_Property.LineEditBoxProperty.dwSelTextColor = dwColor; }
	void SetSelectedBackColor( DWORD dwColor ) { m_Property.LineEditBoxProperty.dwSelBackColor = dwColor; }
	void SetCaretColor( DWORD dwColor ) { m_Property.LineEditBoxProperty.dwCaretColor = dwColor; }

	void SetMaxEditLength(DWORD dwMaxLength);
	int GetTextLength() const;
	bool IsTextFull(int textAddedLength) const;

	static void CopyStringToClipboard( const wchar_t *wszText );
	void SetTextBuffer(const WCHAR* pwszText);
	void AddText( LPCWSTR wszText, bool bFocus );

protected:
	void PlaceCaret( SBufferInfo *pBufferInfo, int nCP );
	void PlaceCaretBegin( SBufferInfo *pBufferInfo );
	void PlaceCaretEnd( SBufferInfo *pBufferInfo );
	void DeleteSelectionText();
	void ResetCaretBlink();	
	void CopyToClipboard();
	void PasteFromClipboard();
	int GetOriginTextCaretPos( SBufferInfo* pBufferInfo, int nCaretPos, bool bRetWithCalcTagLast );
	int GetTextCaretPos( SBufferInfo* pBufferInfo, int nOriginTextCaretPos );
	bool CheckTagTextInsertPos( SBufferInfo* pBufferInfo, int nIndex );
	bool CheckTagTextRemove( SBufferInfo* pBufferInfo, int nCaretPos );
	bool IsInTagField( SBufferInfo* pBufferInfo, int nCaretPos, int& nTagFieldStart, int& nTagFieldEnd );

public:
	virtual bool HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual bool HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam );
	virtual bool MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void UpdateRects();
	virtual void Render( float fElapsedTime );	
};