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
	SUICoord		uiCoord;		// �ؽ�Ʈ�� ��µ� ��ǥ�� ũ��
	SUIColorEx		uiColor;
	DWORD			dwFormat;		// �ؽ�Ʈ ����(UITEXT_CLIP, UITEXT_SYMBOL)
	bool			bSymbol;		// �ܾ� ���� "..."�� �߰��� ������ �ƴ��� ����
	bool			bAdd;			// AddText()�� �ܾ��� true, AppendText()��� false
	//int				nFontIndex;		// �ٸ� ��Ʈ�� ����ϱ� ���� �ε���
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

// Note : �߰� �Ǵ� �ؽ�Ʈ�� �Ѱ��� ������ ������.
//		���� ������ �Ǵ��� �ϳ��� ����� ����.
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

// �� ���ο� ������ ��Ʈ������ ����صд�.
// ä��â�� ���� Ŭ���� �ش� ������ �Ӹ� �ִ� ���� �����ϱ� �����ε� �ϴ�.
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

// Note : ������ ��µǴ� �ؽ�Ʈ�� �������� �и��Ѵ�.
//		���߿� ���δ����� �����ϱ� ���ؼ���.
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
	float m_fLineSpace;			// Note : ���ΰ� ���� ������ ����.

protected:
	bool m_bSelected;
	bool m_bMouseInLine;

	SLineData m_sLineData;		// ������ ������ ������ ��Ʈ�� ��.

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