#include "StdAfx.h"
#include "EtUIHtmlTextBox.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUIHtmlTextBox::CEtUIHtmlTextBox(CEtUIDialog *pParent)
	: CEtUITextBox(pParent)
	, m_dwTextColor(textcolor::WHITE)
	, m_dwDefaultTextColor(textcolor::WHITE)
{
	m_Property.UIType = UI_CONTROL_HTMLTEXTBOX;
	m_htmlReader.setEventHandler(this);
}

CEtUIHtmlTextBox::~CEtUIHtmlTextBox(void)
{
}

void CEtUIHtmlTextBox::BeginParse(DWORD dwAppData, bool &bAbort)
{
	ClearText();
	bAbort = false;
}

void CEtUIHtmlTextBox::StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort)
{
	if( pTag )
	{
		static CStringW strStartTag;
		strStartTag = pTag->getTagName();

		if( strStartTag.CompareNoCase(L"font") == 0 )
		{
			CLiteHTMLAttributes *pAtt = const_cast<CLiteHTMLAttributes*>(pTag->getAttributes());
			if( pAtt )
			{
				for( int i=0; i<pAtt->getCount(); i++ )
				{
					CLiteHTMLElemAttr &pElemAttr = pAtt->getAttribute(i);
					if( pElemAttr.isColorValue() )
					{
						// ������ �𸣰����� html �÷��� r �� b �� �ݴ��ΰ� ����..
						COLORREF clr = pElemAttr.getColorValue();
						BYTE b = GetRValue(clr);
						BYTE g = GetGValue(clr);
						BYTE r = GetBValue(clr);

						m_dwTextColor = D3DCOLOR_XRGB(r,g,b);
						break;
					}
				}
			}
		}
		else if( strStartTag.CompareNoCase(L"br") == 0 )
		{
			AddLine();
		}
		else if( strStartTag.CompareNoCase(L"p") == 0 )
		{
			AddLine();
		}
		else if (strStartTag.CompareNoCase(L"div") == 0)
		{
			CLiteHTMLAttributes *pAtt = const_cast<CLiteHTMLAttributes*>(pTag->getAttributes());
			if( pAtt )
			{
				for (int i = 0; i < pAtt->getCount(); ++i)
				{
					//	todo : vertical center align
					CLiteHTMLElemAttr &pElemAttr = pAtt->getAttribute(i);
					if (pElemAttr.getValue().CompareNoCase(L"center") == 0)
						m_Property.TextBoxProperty.AllignHori = AT_HORI_CENTER;
				}
			}
		}
		else if( strStartTag.CompareNoCase(L"img") == 0 ) {
			CLiteHTMLAttributes *pAtt = const_cast<CLiteHTMLAttributes*>(pTag->getAttributes());
			if( pAtt )
			{
				int nWidth = 0;
				int nHeight = 0;
				std::wstring wszImageName;
				for (int i = 0; i < pAtt->getCount(); ++i)
				{
					//	todo : vertical center align
					CLiteHTMLElemAttr &pElemAttr = pAtt->getAttribute(i);
					if (pElemAttr.getName().CompareNoCase(L"src") == 0) {
						wszImageName = pElemAttr.getValue().GetBuffer();
					}
					else if (pElemAttr.getName().CompareNoCase(L"width") == 0) {
						nWidth = _wtoi( pElemAttr.getValue().GetBuffer() );
					}
					else if (pElemAttr.getName().CompareNoCase(L"height") == 0) {
						nHeight = _wtoi( pElemAttr.getValue().GetBuffer() );
					}
				}
				if( !wszImageName.empty() ) AddImage( (WCHAR*)wszImageName.c_str(), nWidth, nHeight );
			}
		}
	}

	bAbort = false;
}

void CEtUIHtmlTextBox::EndTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort)
{
	if( pTag )
	{
		static CStringW strEndTag;
		strEndTag = pTag->getTagName();

		if( strEndTag.CompareNoCase(L"font") == 0 )
		{
			// �����۾����� �⺻������ ������� ��ο� ������ ����Ǿ���.
			// �ٸ� ���̾�α׿����� �� ��Ʈ���� ����ϰ��־ �̷��� �⺻ ������ü�� �����Ű�� �ȵȴ�.
			//m_dwTextColor = textcolor::WHITE;
			//m_dwTextColor = textcolor::NPC_SPEECH;
			m_dwTextColor = m_dwDefaultTextColor;
		}
		else if( strEndTag.CompareNoCase(L"p") == 0 )
		{
			AddLine();
		}
		else if (strEndTag.CompareNoCase(L"div") == 0)
		{
			m_Property.TextBoxProperty.AllignHori = AT_HORI_NONE;
		}
	}

	bAbort = false;
}

void CEtUIHtmlTextBox::Characters(const CStringW &rText, DWORD dwAppData, bool &bAbort)
{
	// Npc���� ����� ���´�.
	// ��簡 ������ �Ѿ���� ó���� ���⼭ �ϸ� �� ��.
	AppendText( rText, m_dwTextColor );
	bAbort = false;
}

void CEtUIHtmlTextBox::Comment(const CStringW &rComment, DWORD dwAppData, bool &bAbort)
{
	bAbort = false;
}

void CEtUIHtmlTextBox::EndParse(DWORD dwAppData, bool bIsAborted)
{
}

bool CEtUIHtmlTextBox::ReadHtmlString( LPCWSTR lpszString )
{
	if( m_htmlReader.Read( lpszString ) == 0U )
	{
		return false;
	}

	if ( m_ScrollBar.IsShowThumb() )
	{
		m_ScrollBar.Show(true);
	}
	else
	{
		m_ScrollBar.Show(false);
	}


	return true;
}

bool CEtUIHtmlTextBox::ReadHtmlFile( LPCWSTR filename )
{
	if( m_htmlReader.ReadFile( filename ) == 0U )
	{
		return false;
	}

	return true;
}

bool CEtUIHtmlTextBox::ReadHtmlFile( LPCSTR filename )
{
	if( m_htmlReader.ReadFile( filename ) == 0U )
	{
		return false;
	}

	return true;
}

bool CEtUIHtmlTextBox::ReadHtmlStream( CStream *pStream )
{
	int nSize = pStream->Size() + 1;
	char *szBuffer = new char[nSize];
	WCHAR *wszBuffer = new WCHAR[nSize];
	memset( szBuffer, 0, nSize );
	pStream->Read( (void*)szBuffer, pStream->Size() );
	MultiByteToWideChar( CP_UTF8, 0, szBuffer + 3, -1, wszBuffer, nSize );


	bool bResult = ( m_htmlReader.Read( wszBuffer ) == 0 ) ? false : true;
	SAFE_DELETEA( wszBuffer );
	SAFE_DELETEA( szBuffer );
	return bResult;
}

int CEtUIHtmlTextBox::GetScrollBarTemplate()
{
	return m_Property.HtmlTextBoxProperty.nScrollBarTemplate; 
}

void CEtUIHtmlTextBox::SetScrollBarTemplate( int nIndex ) 
{ 
	m_Property.HtmlTextBoxProperty.nScrollBarTemplate = nIndex;
}



void CEtUIHtmlTextBox::RenderLine( float fElapsedTime, CLine* sLine, bool bRollOver )
{
	static float fGapX = -0.002f;
	static float fGapY = m_Property.HtmlTextBoxProperty.fLineSpace - 0.0013f;
	SUIElement* pBaseElement = GetElement(2);
	if ( !bRollOver && pBaseElement )
	{
		SUICoord sLineCoord;
		sLineCoord = sLine->m_uiCoord;
		sLineCoord.fX += fGapX;
		sLineCoord.fY += fGapY;
		sLineCoord.fWidth = m_Property.UICoord.fWidth-m_Property.TextBoxProperty.fScrollBarSize;
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pBaseElement->TemplateUVCoord, 0xffffffff, sLineCoord );
		else
			m_pParent->DrawSprite( pBaseElement->UVCoord, 0xffffffff, sLineCoord );
		//m_pParent->DrawRect( sLineCoord, textcolor::ORANGE );
	}

	// ���÷ѿ��� �̹����� ������ 
	SUIElement* pElement = GetElement(1);
	if ( bRollOver && pElement )
	{
		SUICoord sLineCoord;
		sLineCoord = sLine->m_uiCoord;
		sLineCoord.fX += fGapX;
		sLineCoord.fY += fGapY;
		sLineCoord.fWidth = m_Property.UICoord.fWidth-m_Property.TextBoxProperty.fScrollBarSize;
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, sLineCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, 0xffffffff, sLineCoord );
		//m_pParent->DrawRect( sLineCoord, textcolor::ORANGE );
	}

	switch( sLine->GetLineType() ) {
		case CLine::Line:
			{
				VECWORD & vecWord = sLine->m_vecWord;
				int nWordSize = (int)vecWord.size();

				for( int i=0; i<nWordSize; i++ )
				{
					RenderWord( fElapsedTime, vecWord[i], bRollOver );
				}
			}
			break;
		case CLine::Image:
			{
				CImageLine *pImageLine = (CImageLine *)sLine;
				EtTextureHandle hTexture = pImageLine->GetTexture();
				if( !hTexture ) {
					break;
				}

				SUIElement *pElement = GetElement(0);
				if( !pElement ) return;

				SUICoord uiRenderCoord, uiResult;
				uiRenderCoord = pImageLine->m_uiCoord;
				uiRenderCoord.fX += pElement->fFontHoriOffset;
				uiRenderCoord.fY += pElement->fFontVertOffset;
//				uiResult = m_pParent->DlgCoordToScreenCoord( uiRenderCoord );

				// �׸� �������� uv �Ѿ�°� ���� ���̰� uv���缭 ����.
				SUICoord uiUVCoord = pImageLine->GetTextureUV();
				if( uiUVCoord.fY + uiUVCoord.fHeight > 1.f ) {
					float fOrigHeight = uiUVCoord.fHeight;
					uiUVCoord.fHeight = 1.f - uiUVCoord.fY;
					uiRenderCoord.fHeight = uiRenderCoord.fHeight * uiUVCoord.fHeight / fOrigHeight;
				}

				m_pParent->DrawSprite( hTexture, pImageLine->GetTextureUV(), 0xFFFFFFFF, uiRenderCoord, 0.f, m_fZValue );
//				CEtSprite::GetInstance().DrawSprite( (EtTexture*)hTexture->GetTexturePtr(), hTexture->Width(), hTexture->Height(), pImageLine->GetTextureUV(), 0xFFFFFFFF, uiResult, 0.f );
			}
			break;
	}
}
