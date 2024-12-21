#pragma once
#include "EtUITextBox.h"
#include "LiteHTMLReader.h"

// Note : 아래는 Html을 읽어서 뿌려주는 텍스트 박스.
//		LiteHTMLReader라는 파서를 사용했습니다. NPC 대화창을 보시면됩니다.
//		현재로는 HTML문법중에 문단, 줄바꿈, 폰트 컬러 정도만 지원됩니다.
//
class CEtUIHtmlTextBox : public CEtUITextBox, public ILiteHTMLReaderEvents
{
public:
	CEtUIHtmlTextBox(CEtUIDialog *pParent);
	virtual ~CEtUIHtmlTextBox(void);

protected:
	DWORD m_dwTextColor;		// 현재 색상
	DWORD m_dwDefaultTextColor;	// 기본 색상

	CLiteHTMLReader m_htmlReader;

protected:
	virtual void BeginParse(DWORD dwAppData, bool &bAbort);
	virtual void StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort);
	virtual void EndTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort);
	virtual void Characters(const CStringW &rText, DWORD dwAppData, bool &bAbort);
	virtual void Comment(const CStringW &rComment, DWORD dwAppData, bool &bAbort);
	virtual void EndParse(DWORD dwAppData, bool bIsAborted);

	virtual void RenderLine( float fElapsedTime, CLine* sLine, bool bRollOver = false );
public:
	bool ReadHtmlString( LPCWSTR lpszString );
	bool ReadHtmlFile( LPCWSTR filename );
	bool ReadHtmlFile( LPCSTR filename );
	bool ReadHtmlStream( CStream *pStream );

	virtual int GetScrollBarTemplate();
	virtual void SetScrollBarTemplate( int nIndex );

	// HtmlTextBox가 많이 사용되지 않는 관계로, 색상을 아직 속성으로 빼지 않았다.
	// 다음에 필요하다면 속성값으로 빼야겠다.(그땐 m_dwTextColor멤버가 빠져도 될듯.)
	// 기본색상은 흰색이다.
	//
	// 만약 속성으로 빼면, 기존 UI파일들에 있는 색상들은 0x00000000으로 설정될테니, 찾아서 다시 설정해야할거다.
	void SetDefaultTextColor( DWORD dwColor ) { m_dwTextColor = m_dwDefaultTextColor = dwColor; }
};
