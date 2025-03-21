#pragma once

// Note : Line
#define UITEXT_NONE		0x00000000
#define UITEXT_CLIP		0x00000001
#define UITEXT_SYMBOL	0x00000010
// Note : Word
#define UITEXT_RIGHT	0x00000100
#define UITEXT_CENTER	0x00001000

using namespace EtInterface;

struct SWORD_PROPERTY
{
	SUICoord		uiCoord;		// 텍스트가 출력될 좌표와 크기
	SUIColorEx		uiColor;
	DWORD			dwFormat;		// 텍스트 포맷(UITEXT_CLIP, UITEXT_SYMBOL)
	bool			bSymbol;		// 단어 끝에 "..."를 추가할 것인지 아닌지 여부
	bool			bAdd;			// AddText()된 단어라면 true, AppendText()라면 false
	//int				nFontIndex;		// 다른 폰트를 사용하기 위한 인덱스
	D3DCOLOR		BgColor;

	SWORD_PROPERTY()
		: dwFormat(UITEXT_NONE)
		//, nFontIndex(-1)
		, bSymbol(false)
		, bAdd(false)
		, BgColor(0)
	{
	}

	void Clear()
	{
		uiCoord.SetCoord(0.f, 0.f, 0.f, 0.f);
		uiColor.Clear();
		dwFormat = UITEXT_NONE;
		bSymbol = false;
		bAdd = false;
		BgColor = 0;
	}
};

// Note : 추가 되는 텍스트는 한가지 색상을 가진다.
//		따라서 문장이 되더라도 하나의 워드로 본다.
class CWord
{
public:
	CWord();
	~CWord();

public:
	std::wstring m_strWord;
	SWORD_PROPERTY m_sProperty;
	std::wstring m_strWordWithTag;

public:
	void UpdatePos( float fX, float fY );
	void Clear();
};
typedef	std::vector<CWord>	VECWORD;

// 각 라인에 별도의 스트링값을 기억해둔다.
// 채팅창에 라인 클릭시 해당 유저로 귓말 넣는 것을 구현하기 위함인듯 하다.
struct SLineData
{
	std::wstring m_strData;
	int m_nData;

	void Clear()
	{
		m_strData.clear();
		m_nData = 0;
	}
};

// Note : 라인은 출력되는 텍스트를 라인으로 분리한다.
//		나중에 라인단위로 관리하기 위해서다.
class CLine
{
public:
	CLine();
	virtual ~CLine();

	enum LineTypeEnum {
		Line,
		Image,
	};
public:
	VECWORD m_vecWord;	
	SUICoord m_uiCoord;
	float m_fLineSpace;			// Note : 라인과 라인 사이의 간격.

protected:
	bool m_bSelected;
	bool m_bMouseInLine;

	SLineData m_sLineData;		// 라인이 가지는 별도의 스트링 값.

public:
	void Select( bool bSelect )				{ m_bSelected = bSelect; }
	bool IsSelected()						{ return m_bSelected; }
	void SetMouseInLine( bool bMouseInCtl )	{ m_bMouseInLine = bMouseInCtl; }
	bool IsMouseInLine()					{ return m_bMouseInLine; }

public:
	std::wstring GetText();

	void SetLineData( SLineData &sLineData ) { m_sLineData = sLineData; }
	SLineData &GetLineData() { return m_sLineData; }

public:
	void SetLineSpace( float fLineSpace );
	float GetLineSpace()					{ return m_fLineSpace; }
	void UpdatePos( float fX, float fY );

	virtual LineTypeEnum GetLineType() { return CLine::Line; }
};

class CImageLine : public CLine
{
public:
	CImageLine();
	virtual ~CImageLine();

protected:
	EtTextureHandle m_hTexture;
	std::string m_szFileName;
	SUICoord m_uiUV;
	
public:
	void SetImage( WCHAR *wszFileName, SUICoord &Coord );
	void SetImage( char *szFileName, SUICoord &Coord );
	EtTextureHandle GetTexture();
	SUICoord &GetTextureUV() { return m_uiUV; }

	virtual LineTypeEnum GetLineType() { return CLine::Image; }
};

typedef	std::vector<CLine *>	VECLINE;
typedef	VECLINE::iterator	VECLINE_ITER;