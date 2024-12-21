#pragma once
#include "EtUITextBox.h"
#include "LiteHTMLReader.h"

// Note : �Ʒ��� Html�� �о �ѷ��ִ� �ؽ�Ʈ �ڽ�.
//		LiteHTMLReader��� �ļ��� ����߽��ϴ�. NPC ��ȭâ�� ���ø�˴ϴ�.
//		����δ� HTML�����߿� ����, �ٹٲ�, ��Ʈ �÷� ������ �����˴ϴ�.
//
class CEtUIHtmlTextBox : public CEtUITextBox, public ILiteHTMLReaderEvents
{
public:
	CEtUIHtmlTextBox(CEtUIDialog *pParent);
	virtual ~CEtUIHtmlTextBox(void);

protected:
	DWORD m_dwTextColor;		// ���� ����
	DWORD m_dwDefaultTextColor;	// �⺻ ����

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

	// HtmlTextBox�� ���� ������ �ʴ� �����, ������ ���� �Ӽ����� ���� �ʾҴ�.
	// ������ �ʿ��ϴٸ� �Ӽ������� ���߰ڴ�.(�׶� m_dwTextColor����� ������ �ɵ�.)
	// �⺻������ ����̴�.
	//
	// ���� �Ӽ����� ����, ���� UI���ϵ鿡 �ִ� ������� 0x00000000���� �������״�, ã�Ƽ� �ٽ� �����ؾ��ҰŴ�.
	void SetDefaultTextColor( DWORD dwColor ) { m_dwTextColor = m_dwDefaultTextColor = dwColor; }
};
