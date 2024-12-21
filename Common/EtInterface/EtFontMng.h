#pragma once
#include "LostDeviceProcess.h"
#include "EtUIType.h"
#include <vector>
#include <string>
#include <map>
#include <deque>
#include <usp10.h>
#pragma comment( lib, "usp10.lib" )

using namespace std;

#define PRE_MOD_MINIMIZE_FORCE_BLUR

//struct SRegisterFontInfo
//{
//	std::string szFontName;
//	HANDLE hFontHandle;
//};

struct SFontInfo_Old
{
	int nFontHeight;
	int nFontWeight;
	char szFontName[ LF_FACESIZE ];
	BOOL bItalic;
};
 
typedef struct FT_FaceRec_*  FT_Face;
struct SFontInfo
{
	enum { VERSION = 0x101 };

	FT_Face Face;
	BYTE *pMemFont;
	int nFontHeight;
	int nFontWeight;
	char szFontName[ LF_FACESIZE ];
	BOOL bItalic;

	map<wchar_t, int> mapAdvanceX;		// 매번 프리타입 함수를 호출해서 advance.x 를 갖고 오면 부하가 걸리므로..
	map<wchar_t, int> mapBoldAdvanceX;	// 볼드체인 경우... 이탤릭체는 아직 없습니다...

	SFontInfo() : Face( NULL ), pMemFont( NULL ), nFontHeight( 0 ), nFontWeight( 0 ), bItalic( false )
	{
		ZeroMemory( szFontName, sizeof(szFontName) );
		//ZeroMemory( this, sizeof(SFontInfo) );
	}

public:
	bool Load( CStream &stream );
	bool Save( CStream &stream );
};

struct SUIFontSet
{
	enum { VERSION = 0x101 };

	int nIndex;
	std::string strFontName;
	std::string strFileName;
	std::string strFullFileName;
	int nFontHeight;
	int nFontWeight;
	bool bItalic;

	SUIFontSet()
		: nIndex(-1)
		, strFontName("")
		, strFileName("")
		, strFullFileName("")
		, nFontHeight(0)
		, nFontWeight(0)
		, bItalic(false)
	{
	}

	SUIFontSet( int Index, std::string FontName, std::string FileName, int FontHeight, int FontWeight, bool Italic )
	{
		nIndex = Index;
		strFontName = FontName;
		strFileName = FileName;
		nFontHeight = FontHeight;
		nFontWeight = FontWeight;
		bItalic = Italic;
	}

	void Load( CStream &stream )
	{
		DWORD dwVersion;
		stream >> dwVersion;

		switch( dwVersion )
		{
		case VERSION:
			{
				stream >> nIndex;
				stream >> strFontName;
				stream >> strFileName;
				stream >> nFontHeight;
				stream >> nFontWeight;
				stream >> bItalic;
			}
			break;
		default:
			ASSERT(0&&"SUIFontSet::Load, default");
			break;
		}
	}

	void Save( CStream &stream )
	{
		stream << VERSION;

		stream << nIndex;
		stream << strFontName;
		stream << strFileName;
		stream << nFontHeight;
		stream << nFontWeight;
		stream << bItalic;
	}
};

struct SFontTextureInfo
{
	//enum
	//{
	//	NORMAL,
	//	SHADOW_BASE,
	//	STROKE_BASE,
	//	GLOW_BASE,
	//	COUNT,
	//};

	EtTextureHandle hTexture;
	/*int nDrawedType;*/
	int Width;
	int Height;

	SFontTextureInfo( void ) : /*nDrawedType(NORMAL),*/ Width( 0 ), Height( 0 ) {};
};

struct SFontDrawEffectInfo
{
	enum
	{
		NORMAL = 0,
		SHADOW,
		STROKE,
		GLOW,
		COUNT,
	};

	int nDrawType;
	int nWeight;
	float fAlphaWeight;
	DWORD dwFontColor;
	DWORD dwEffectColor;

	float fGlobalBlurAlphaWeight;

	SFontDrawEffectInfo( void ) : nDrawType( NORMAL ), nWeight( 1 ), fAlphaWeight( 1.5f ),
								  fGlobalBlurAlphaWeight( 0.0f ),
								  dwFontColor( 0 ), dwEffectColor( 0 )
	{};

	bool operator == ( const SFontDrawEffectInfo& Info )
	{
		return (Info.nDrawType == nDrawType &&
				Info.nWeight == nWeight &&
				Info.fAlphaWeight == fAlphaWeight &&
				Info.fGlobalBlurAlphaWeight == Info.fGlobalBlurAlphaWeight &&
				//(Info.dwFontColor&0x00ffffff) == (dwFontColor&0x00ffffff) &&		// 알파 부분은 제외하고 비교.. 
				Info.dwEffectColor == dwEffectColor);
	}
};

//  cache update 시 vector erase 에서 많이 발생되는 대입연산시의 메모리 할당의 부담을 줄이기 위해서 vector 와 map 을 제거한다.
struct SCachedFont
{
	std::wstring szText;
	int	nFontIndex;
	bool bBold;
	float fWidth;
	SFontTextureInfo TextureInfo;
	SFontDrawEffectInfo FontEffectInfo;
	int nLastUseTick;
};

// 캐럿 정보는 캐쉬에 담지 말고 전부 메모리에 보존한다.
struct SCaretKey
{
	std::wstring szText;
	int nFontIndex;

	const bool operator < ( const SCaretKey & rhs )  const {
		if( szText < rhs.szText ) return true;
		else if( szText > rhs.szText ) return false;
		if( nFontIndex < rhs.nFontIndex ) return true;
		else if( nFontIndex > rhs.nFontIndex ) return false;
		return false;
	}
};

struct SCaretInfo
{
	vector<int> vecCaretPos;
	map<int, int> mapPosByCaret;
	int nHeight;
};

struct SFontInfoHandle
{
	int		nFontHeight;
	char	szFontName[ LF_FACESIZE ];
	HFONT	hFont;
};

struct SAddFontMemResource
{
	char	szFontName[ LF_FACESIZE ];
	HANDLE	hFontHandle;
};

#define UPPER_PADDING_PIXEL		4.0f		// 폰트 텍스쳐 위쪽 여유공간

typedef struct FT_LibraryRec_  *FT_Library;
class CEtFontMng : public CSingleton< CEtFontMng >, public CLostDeviceProcess
{
public:
	CEtFontMng(void);
	virtual ~CEtFontMng(void);

	static float s_fLinePitchRate;	// 행간을 전체적으로 조절한다.
	static bool s_bUseUniscribe;	// 폰트파일 크기가 큰 국가의 경우 ResetDevice시 폰트셋 처리하는데 오래걸리기 때문에 Uniscribe사용하지 않아도 되는경우 제외하기 위해 사용
	static bool s_bUseWordBreak;	// 단어단위 줄내림 사용할지 여부 Uniscribe API를 사용
											
protected:
	std::vector< SFontInfo > m_vecFont;
	std::vector< SUIFontSet > m_vecFontSet;
	std::map<int, int> m_mapFontIndex;
	std::string m_strFontSetFileName;
	FT_Library m_Library;
	std::vector< SCachedFont > m_cachedFontList;
	std::list< EtTextureHandle > m_texturePool;

	std::map< SCaretKey, SCaretInfo > m_caretInfo;

	int		m_nUpdateTerm;

	std::map< std::string, std::pair<BYTE*, DWORD> > m_FontMemCache;
	std::vector< SAddFontMemResource > m_vecAddFontMemResourceList;
	std::vector< SFontInfoHandle > m_vecStFontList;
	HFONT	m_hOldFont;

	float	m_fOriginWidth;

#if defined( PRE_MOD_MINIMIZE_FORCE_BLUR )
	bool	m_bForceBlur;
	float	m_fBlurWeight;
	int		m_nMinimizeSize;
#endif	// #if defined( PRE_MOD_MINIMIZE_FORCE_BLUR )

protected:
	int GetFontIndex( int &nFontSetIndex, int nFontHeight );
	void AccumulateColor( DWORD& dwAlpha, DWORD& dwRed, DWORD& dwGreen, DWORD& dwBlue, DWORD& dwNowColor, int iWeight );
	void Blur( BYTE* pTextureBuffer, BYTE* pOriginalBuffer, bool* pabBlurDir, float fAlphaWeight, int nPitch, int nTexelWidth, int nTexelHeight, int nTextTexelWidth, int nTextTexelHeight, bool bReverseLoop = false );

	void BlurProcess( BYTE* pOriginalBuffer, int nBytePos, bool* pabBlurDir, int nY, int nX, BYTE* pTextureBuffer, int nPitch, int nTexelWidth, int nTexelHeight, float fAlphaWeight );
	
	bool IsExistAddFontResourceList( SFontInfo& stFontInfo );
	HFONT FindFontHandle( SFontInfo& stFontInfo );
	void GetTTFLocalFontName( const char* pFontFileName, char* pChangeFontName );
	int GetGlyphGOffset(  SFontInfo& stFontInfo, wstring& strText, vector<WORD>& vecGlyph, vector<GOFFSET>& vecGOffset );
	void GetWordBreakInfo( wstring& strText, vector<SCRIPT_LOGATTR>& vecLogAttr );
	int WordBreak( wstring& strText, SCaretKey& CaretKey, bool bAlreadyExistCaret, FT_Face& Face, bool bBold, SFontInfo& FontInfo, float fWidth, 
					vector<wstring>& vecStrLine, vector<int>& vecWidthList, bool bUseRemainWidth = false );
	void SetOriginWidth( float& fWidth );
	bool IsSymbol( wchar_t cText );
	bool IsOpenBraket( wchar_t cText );

public:
	void Initialize( const char *szFileName );
	void Finalize();

	int AddFont( const char *pFontFileName, const char *pFontName, int nFontHeight, int nFontWeight, BOOL bItalic = false );
	void DeleteAllFont();

	unsigned int CalcTextureSize(unsigned int size);
	SFontTextureInfo GetFontTexture( int nFontIndex, LPCWSTR szText, float fWidth, DWORD dwFontFormat, 
									 SFontDrawEffectInfo& Info, bool bRemoveCache = true );

	void UpdateFontCache( bool bRemoveCache );
	SCachedFont* GetCachedFontTexture( LPCWSTR szText, float fWidth, SFontDrawEffectInfo& FontEffectInfo, int nFontIndex );

	void DrawTextW( int &nFontSetIndex, int nFontHeight, LPCWSTR szText,  DWORD dwFontFormat, SUICoord &ScreenCoord, 
					int nCount, SFontDrawEffectInfo& FontEffectInfo, bool bRemoveCache = true, float ZValue = 0.f, DWORD dwBgColor = 0, int nBorderFlag = 0 );
	void DrawTextW3D( int &nFontSetIndex, int nFontHeight, LPCWSTR szText, EtVector3 &vPosition, SFontDrawEffectInfo& FontEffectInfo, int nCount, float fScale = 1.0f );

	void CalcTextRect( int &nFontSetIndex, int nFontHeight, LPCWSTR szText, DWORD dwFontFormat, SUICoord &CalcCoord, int nCount, bool bUseCache = true );
	void GetFontInfo( int &nFontSetIndex, int nFontHeight, SFontInfo &FontInfo );

	bool LoadFontSet( const char *szFileName );
	void SaveFontSet( const char *szFileName );
	std::string& GetFontSetFileName() { return m_strFontSetFileName; }
	std::vector< SUIFontSet >& GetFontSetList() { return m_vecFontSet; }
	void AddFontSet( SUIFontSet &fontSet );
	void DeleteFontSet( int nIndex );
	void ModifyFontSet( int nIndex, SUIFontSet &fontSet );
	SUIFontSet *GetFontSet( int nIndex );
	int GetFontSetIndex();
	int GetCaretPos( LPCWSTR szText, int& nFontSetIndex, int nFontHeight, int nCharIndex, int* pHeight = NULL, bool bUseCache = true );
	
	// nTrail 은 텍스트의 맨 끝을 지나서 캐럿을 표시해줘야하는 경우 1이 셋팅된다.
	int GetCaretFromCaretPos( LPCWSTR szText, int& nFontSetIndex, int nFontHeight, int nCaretPos, /*OUT*/ int& nTrail );
	int GetCachedCharAdvanceX( FT_Face Face, SFontInfo& FontInfo, wchar_t ch, bool bBold, int iNowAdvance );

	void FlushFontCache();

	virtual void OnLostDevice();
	virtual void OnResetDevice();

	bool GetWordBreakText( wstring& strText, int nFontSetIndex, int nFontHeight, float fWidth, vector<wstring>& vecStrLine, 
							int& nMaxWidth, bool bUseRemainWidth = false, float fOriginWidth = 0.0f );

	void GetBasicFontTextureInfo(SFontTextureInfo& result, int nFontSetIndex, int nFontHeight, LPCWSTR szText, DWORD dwFontFormat, const SUICoord &ScreenCoord, SFontDrawEffectInfo& Info);

#if defined( PRE_MOD_MINIMIZE_FORCE_BLUR )
	void SetForceBlur( bool bForce ) { m_bForceBlur = bForce; }
	void SetForceBlurWeight( float fWeight ) { m_fBlurWeight = fWeight; }
	float GetForceBlurWeight()	{ return m_fBlurWeight; }
	void SetForceMinimizeSize( int nMinimizeSize ) {m_nMinimizeSize = nMinimizeSize; }
	int GetForceMinimizeSize()	{ return m_nMinimizeSize; }
#endif	// #if defined( PRE_MOD_MINIMIZE_FORCE_BLUR )
};
