#pragma once
#include "EtUIDialogInfo.h"
#include "EtUIControlProperty.h"
#include "EtUIDialogBase.h"
#include "LostDeviceProcess.h"
#include "EtUITemplate.h"

#define UI_FILE_STRING	"Eternity Engine UI File 0.3"
#define UI_HEADER_RESERVED	( 1024 - sizeof( SUIFileHeader ) )
#define UI_FILE_VERSION_01		0x0101
#define UI_FILE_VERSION			0x0102

#define PRE_FIX_COMBOBOX_ELLIPSIS

struct SUIFileHeader
{
	char szHeaderString[ 256 ];
	int nVersion;
	int nTemplateCount;
	int nControlCount;
	int nFontCount;

	SUIFileHeader()
	{
		ZeroMemory( this, sizeof(SUIFileHeader) );
		strcpy_s( szHeaderString, sizeof(szHeaderString), UI_FILE_STRING );
		nVersion = UI_FILE_VERSION;
	}
};

class CEtUIControl;
class CEtUITemplate;
struct SUIElement;
class CEtUIDialog;

class CEtUICallback
{
public:
	CEtUICallback() {}
	~CEtUICallback() {}

public:
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 ) {}
};

class CEtUIDialog : public CEtUIDialogBase
{
public:
	CEtUIDialog( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CEtUIDialog(void);

	enum FadeModeEnum {
		None,
		AllowRender,
		CancelRender,
	};

protected:
	bool m_bShow;
	bool m_bAcceptInputMsgWhenHide;		// show 상태던 아니던 메시지를 받을 수 있게 해주는 플래그. 엮여있는 다이얼로그를 hide 시키고 따로 렌더링하는 스킬트리에서 메시지 처리하기 위해 사용함.
	HWND m_hWnd;
	std::vector< CEtUIControl * > m_vecControl;
	std::vector< CEtUITemplate * > m_vecUITemplate;
	std::vector< SUIElement > m_vecDlgFrameElement;
	std::vector< SUICoord > m_vecDlgFrameCoord;
	EtTextureHandle m_hTexture;
	EtTextureHandle m_hDlgTexture;
	CEtUIControl *m_pDefaultControl;
	std::vector< CEtUIControl* > m_TempControlList;

	int m_nDialogID;
	CEtUICallback *m_pCallback;

	SUIDialogInfo m_DlgInfo;

	float m_fScreenWidth;
	float m_fScreenWidthRatio;
	float m_fScreenWidthBorder;

	float m_fScreenHeight;
	float m_fScreenHeightRatio;
	float m_fScreenHeightBorder;

	std::string m_strCmdControlName;	// Note : ProcessCommand안에서 사용될 컨트롤 이름
	
	SUIColorEx m_renderDlgColor;
	bool m_bFadeOut;
	FadeModeEnum m_FadeMode;
	bool m_bMouseInDialog;
	bool m_bEndInitialize;

	float m_fMouseX;
	float m_fMouseY;

	SHORT m_HotKeyState;
	int m_nHotKey;

	CEtUIDialog *m_pParentDialog;						// Note : 자신의 부모 윈도우.
	std::list<CEtUIDialog*> m_listChildDialog;			// Note : 자식 윈도우들.
	std::list<CEtUIDialog*> m_listChildModalDialog;		// Note : 자식인데 모달로 생성된 윈도우들.

	std::string m_strDialogFileName;
	SUICoord m_BaseDlgCoord;

	bool m_bPassMessageToChild;
	bool m_bShowModal;
	bool m_bAutoCursor;
	bool m_bCursor;

	// 리스트박스 아이템이나, 엘리먼트 아이템일 경우 일반 다이얼로그와 구분을 위해 체크를 해둡니다.
	bool m_bElementDialog;
	bool m_bElementDialogShowState;

	std::map< std::string, CEtUIDialog*> m_cachedControls;
	std::vector< std::pair<CEtUIDialog*, bool> > m_vecChildRenderPriorityInfo;

public:
	// 마켓다이얼로그의 강제스케일때문에 에디트박스 캐럿이 안맞는 문제가 발생했습니다.
	// 어쩔 수 없는 경우 빼고는 최대한 다른 곳에서는 사용하지 말아주세요.
	//
	// 기존의 강제스케일 방식의 단점은,
	// FontMng와 연동되지 않아 마우스 드래그 및 렌더링에 별도처리를 해줘야한다.
	// 어차피 보여질 다이얼로그(마켓,인벤,툴팁)에 대해 스케일을 셋팅하는 방식이라면 s_fDialogScale을 건드리는게 낫다.
	// 이므로, s_fDialogScale값을 강제로 바꾸는 방식으로 변경하게 되었습니다.
	static void SetForceDialogScale( float fScale );

public:
	static float s_fScreenMouseX;
	static float s_fScreenMouseY;

protected:
	// Note : 아래는 툴팁에 관련된 스태틱 변수들. 지금은 툴팁이 한번에 하나만 출력되는 구조이다.
	//		하지만 이런 구조는 별로 유연하지 못한듯 하다. 나중에는 스태틱이 아니라 다이얼로그마다
	//		이 변수들과 함수를 가지는게 좋겠다.
	//
	static int s_nTooltipStringIndex;
	static std::wstring s_strTooltipString;
	static CEtUIDialog *s_pTooltipDlg;

	static float s_fDialogScale;
	static float s_fDialogScaleValueVert;
	static float s_fDialogScaleValueHori;
	static float s_fMinDialogScale;
	static float s_fMaxDialogScale;
	static int s_nUISize;
	static void (_stdcall *s_pAutoCursorPtr)( bool bShow );

	static int s_nDialogTextureSize;

	float m_fLastShowDelta;
	bool m_bLoadedTexture;
	

public:
	static CEtUIControl *s_pMouseEnterControl;

	static void ReleaseMouseEnterControl();
	static void SetMouseEnterControl( CEtUIControl *pControl );
	static void SetAutoCursorCallback( void (_stdcall *pFunc)( bool bShow ) ) { s_pAutoCursorPtr = pFunc; }

	// FadeForBlow는 UI숨긴 상태에서로 렌더해야한다고 한다. 할수없이 상위단으로 뺀다.
	static CEtUIDialog *s_pFadeDlg;

public:
	void AddChildDialog( CEtUIDialog *pDialog );
	void AddChildModalDialog( CEtUIDialog *pDialog );
	void SetChildRenderPriority(CEtUIDialog* pDlg, bool bTail);
	void RefreshChildRenderPriority();

protected:
	// EtUIListBoxEx가 나오기전까지는 자식 다이얼로그를 중간에 지운적이 없어서인지,
	// 아예 자식다이얼로그를 지우는 함수자체가 없었다.
	// 그러다 EtUIListBoxEx의 아이템을 자식 다이얼로그로 처리하려 할때 추가한 함수다.
	// (실제로는 자식 다이얼로그로 처리하지 않기때문에 사용하지는 않는다.)
	//
	// 현재는 EtUIDialog의 소멸자에서 자신이 삭제될때 부모에게 리스트에서 빼라는 용도로 사용된다.
	void DelChildDialog( CEtUIDialog *pDialog );
	void DelChildModalDialog( CEtUIDialog *pDialog );

public:
	bool IsRootDialog() { return (m_pParentDialog==NULL)?true:false; }
	bool IsChildDialog( const CEtUIDialog *pDialog );
	CEtUIDialog* GetParentDialog() { return m_pParentDialog; }

public:
	// Note : 자식 윈도우의 Show()를 할때는 그 윈도우의 Show()를 호출하지 않고 아래 함수를 호출한다.
	//
	void ShowChildDialog( CEtUIDialog *pDialog, bool bShow );

protected:
	void ShowChildDialog( std::list<CEtUIDialog*> &listDialog, CEtUIDialog *pDialog, bool bShow );
	bool IsAllowRender();

public:
	static void SetTooltipDlg( CEtUIDialog *pTooltipDlg ) { s_pTooltipDlg = pTooltipDlg; }
	static void ShowTooltipDlg( CEtUIControl *pControl, bool bShow, int nStringIndex = 0, DWORD color = 0xffffffff, bool bPermanent = false );
	static void ShowTooltipDlg( CEtUIControl *pControl, bool bShow, const std::wstring &strTooltip, DWORD color = 0xffffffff, bool bPermanent = false );
	static int GetTooltipStringIndex()		{ return s_nTooltipStringIndex; }
	static bool IsTooltipControl(CEtUIControl* pCtrl);
	static bool IsShowTooltipDlg() { return s_pTooltipDlg->IsShow(); }

	virtual void SetTooltipControl( CEtUIControl *pControl ) {}
	virtual void SetTooltipInfo(DWORD color = 0xffffffff, bool bPermanent = true)	{ _ASSERT(0); }
	virtual void ClearTooltipInfo()													{ _ASSERT(0); }
	virtual CEtUIControl* GetTooltipCtrl() const									{ _ASSERT(0); return NULL; }

	static float GetDialogScale() { return s_fDialogScale; }	
	static void UpDialogSize();
	static void DownDialogSize();
	static void CalcDialogScaleValue( int nWidth = 0, int nHeight = 0 );

	// 해상도에 따른 스케일값 구해 적용하기.
	static void CalcDialogScaleByResolution( int nWidth, int nHeight );

	static void SetDialogSize( int nStep, int nWidth = 0,  int nHeight = 0 );

	static int GetTextureMemorySize() { return s_nDialogTextureSize; }
	void ProcessMemoryOptimize( float fElapsedTime );
	void FreeDialogTexture();
	void LoadDialogTexture();

public:
	void SetFadeMode( FadeModeEnum Mode )	{ m_FadeMode = Mode; }

	bool IsMouseInDlg() { return m_bMouseInDialog; }
	void GetMouseMovePoints( float &fMouseX, float &fMouseY );
	void GetScreenMouseMovePoints( float &fMouseX, float &fMouseY );

	void SetRenderColor( DWORD dwColor )
	{ 
		m_renderDlgColor.dwColor[UI_STATE_NORMAL] = dwColor;
		m_renderDlgColor.dwCurrentColor = dwColor;
	}
	void SetSoundEnable( bool bEnable )			{ m_DlgInfo.bSound = bEnable; }
	// Note : 부모 윈도우가 보이지 않아도 자식의 메세지프로시져에 들어가도록 한다.
	//		예를 들면 채팅창에서 채팅창이 안보일때 마우스 클릭으로 채팅창에 포커스가 가도록 하기 위해 쓰였다.
	//
	void SetPassMessageToChild( bool bPass )	{ m_bPassMessageToChild = bPass; }
	void SetShowModal( bool bShowModal )		{ m_bShowModal = bShowModal; }

	void SetElementDialog( bool bElement )		{ m_bElementDialog = bElement; }
	bool IsElementDialog()						{ return m_bElementDialog; }
	// ElementDialog의 경우엔 보통 UI_TYPE_SELF로 생성해서 직접 MsgProc및 Render되는 경우가 많기때문에
	// 현재 보여지고 있는 다이얼로그인지를 IsShow함수로 검사할 수 없을때가 많다. 이땐 직접 설정해서 얻어오는 수밖에 없다.
	virtual void SetElementDialogShowState( bool bShow )	{ m_bElementDialogShowState = bShow; }
	bool IsShowElementDialog()					{ return m_bElementDialogShowState; }

	// 핫키버그때문에 어쩔 수 없이 만든 함수.
	void SetHotKeyState( SHORT HotKeyState )	{ m_HotKeyState = HotKeyState; }

	bool IsCmdControl( const char *szCmdCtlName );
protected:
	void UpdateScreen();
	void UpdateFrameRectsEx();

	void SetCmdControlName( const char *szCmdCtlName )		{ m_strCmdControlName = szCmdCtlName; }
	const std::string& GetCmdControlName() const			{ return m_strCmdControlName; }

	void FadeIn( float fElapsedTime );
	void FadeOut( float fElapsedTime );

	void SetFadeIn()		{ m_bFadeOut = false; }
	void SetFadeOut()		{ m_bFadeOut = true; }

	virtual bool OnMouseMove( float fX, float fY );

public:
	void CreateTestUIControl();

	bool Load_01( CStream &Stream, SUIFileHeader &fileheader );
	bool Load( CStream &Stream );
	bool Save( CStream &Stream );
	const char *GetDialogFileName() { return m_strDialogFileName.c_str(); }
	const char *GetDialogUITextureFileName() { return m_DlgInfo.szUITexturename; }

	void SetDialogID( int nID ) { m_nDialogID = nID; }
	int GetDialogID() { return m_nDialogID; }

	virtual void Show( bool bShow );
	virtual bool IsShow() const { return m_bShow; }
	void SetAcceptInputMsgWhenHide( bool bAcceptMsg ) { m_bAcceptInputMsgWhenHide = bAcceptMsg; };

	HWND GetHWnd() { return m_hWnd; }
	void SetHWnd( HWND hWnd ) { m_hWnd = hWnd; }

	void SetCallback( CEtUICallback *pCallback )	{ m_pCallback = pCallback; }
	CEtUICallback *GetCallBack()					{ return m_pCallback; }

	void SetDlgInfo( SUIDialogInfo &DlgInfo );
	void GetDlgInfo( SUIDialogInfo &DlgInfo );
	void ReloadDlgTexture();
	void DeleteDlgTexture();

	// Note : 게임의 기본 디폴트 해상도는 1024x768. 
	//		Border는 현재 스크린의 크기에서 디폴트 스크린의 크기를 뺀 부분의 사이즈입니다.
	//		Ratio는 디폴트 크기와 현재 스크린의 크기의 비율입니다.
	//
	float GetScreenWidth() { return m_fScreenWidth; }
	float GetScreenWidthBorderSize() { return m_fScreenWidthBorder; }
	float GetScreenWidthRatio() { return m_fScreenWidthRatio; }

	float GetScreenHeight() { return m_fScreenHeight; }
	float GetScreenHeightBorderSize() { return m_fScreenHeightBorder; }
	float GetScreenHeightRatio() { return m_fScreenHeightRatio; }

	//void GetScreenSize( float &fWidth, float &fHeight );
	float GetXCoord();
	float GetYCoord();
	virtual void MoveDialog( float fX, float fY );

	void SetAllignType( UIAllignHoriType Hori, UIAllignVertType Vert ) { m_DlgInfo.AllignHori = Hori; m_DlgInfo.AllignVert = Vert; }
	void GetAllignType( UIAllignHoriType &Hori, UIAllignVertType &Vert ) { Hori = m_DlgInfo.AllignHori; Vert = m_DlgInfo.AllignVert; }
	virtual void SetDlgCoord( const SUICoord &DlgCoord ) { m_DlgInfo.DlgCoord = DlgCoord; UpdateRects(); }
	void GetDlgCoord( SUICoord &Coord ) { Coord = m_DlgInfo.DlgCoord; }
	const SUICoord& GetDlgCoord() const { return m_DlgInfo.DlgCoord; }

	SUICoord DlgCoordToScreenCoord( const SUICoord &Coord );
	SUICoord ScreenCoordToDlgCoord( SUICoord &Coord );
	float Width() { return m_DlgInfo.DlgCoord.fWidth; }
	float Height() { return m_DlgInfo.DlgCoord.fHeight; }
	void GetPosition( float &fX, float &fY );
	void SetPosition( float fX, float fY );
	CEtUIControl *GetControlAtPoint( float fX, float fY );

	int GetTemplateCount() { return ( int )m_vecUITemplate.size(); }
	void SetTemplate( int nIndex, CEtUITemplate &Template );
	int AddTemplate( CEtUITemplate &Template );
	void DeleteTemplate( int nIndex );
	void DeleteAllTemplate();
	CEtUITemplate *GetTemplate( int nIndex );
	float GetFontHeight( int &nFontSetIndex, int nFontHeight );

	//ID3DXFont *GetFont( int &nFontSetIndex, int nFontHeight );
	const wchar_t* GetUIString( int nCategoryID, int nIndex );

	void PointToFloat( POINT pt, float &fX, float &fY );

	void SetUITextureName( const char *pszTextureName ) { strcpy_s( m_DlgInfo.szUITexturename, _MAX_PATH, pszTextureName ); }
	EtTextureHandle GetUITexture() { return m_hTexture; }
	void SetUITexture( EtTextureHandle hTexture ) { SAFE_RELEASE_SPTR( m_hTexture ); m_hTexture = hTexture; }

	// Note : 같은 클래스 타입의 컨트롤 리스트를 반환한다.
	//		ex) std::vector<CButton*> vector
	//
	template < typename T > void GetGontrolList( std::vector< T* > &vecControl );
	std::vector<CEtUIControl*>& GetControlList() { return m_vecControl; }

	int GetControlCount() { return ( int )m_vecControl.size(); }
	CEtUIControl *GetControl( const char *pszControlName );
	template < typename T > 
	T* _GC( const char *pszControlName );
	template < typename T > 
	T* GetControl( const char *pszControlName, bool *pExistControl = NULL );

//#if defined(_DEBUG) || defined(_RDEBUG)
	static bool s_bUITest;
	static std::map<std::string, int> s_mapUITest;
//#endif

	CEtUIControl *GetControl( int nIndex ) { return m_vecControl[ nIndex ]; }
	void SetControl( int nIndex, CEtUIControl *pControl ) { m_vecControl[ nIndex ] = pControl; }
	CEtUIControl *CreateControl( UI_CONTROL_TYPE Type );
	void DeleteControl( CEtUIControl *pControl );
	void DeleteAllControl();

	virtual bool FindControl( std::vector< CEtUIControl* > &vecControl, int nTypeCount, UI_CONTROL_TYPE *pType, bool bCheckCoveredControl, std::vector<SUICoord> &vecDlgCoord );

	void MoveToHead( CEtUIControl *pControl );
	void MoveToTail( CEtUIControl *pControl );

	//void DeleteAllFont();
	bool IsUsableFocusControl();
	int FindControlIndex( CEtUIControl *pControl );
	CEtUIControl *GetNextControl( CEtUIControl *pControl );
	CEtUIControl *GetPrevControl( CEtUIControl *pControl );

	bool OnCycleFocus( bool bForward );
	void RequestFocus( CEtUIControl* pControl );
	void PushFocusControl();
	void PopFocusControl();
#ifdef PRE_FIX_COMBOBOX_ELLIPSIS
	bool MakeStringWithEllipsis(SUIElement* pElement, float lineWidth, std::wstring& text, const std::wstring& symbol);
#else
	void MakeStringWithEllipsis(SUIElement* pElement, float lineWidth, std::wstring& text, const std::wstring& symbol);
#endif

	// Note : 아래 함수들은 UITool에서 쓰인다.
	//
	void ShowAllControl( bool bShow );
	void EnableAllControl( bool bEnable );
	void UpdateAllControl();

	void ClearRadioButtonGroup( int nButtonGroup );

	void DrawDlgFrame();
	void CalcTextRect( LPCWSTR strText, SUIElement *pElement, SUICoord &Coord, int nCount = -1, DWORD dwFontFormat = 0xffffffff );
	void CalcTextRect( LPCWSTR szText, int fontIndex, int fontHeight, SUICoord &CalcCoord, DWORD dwFontFormat, bool bUseCache );
	void DrawDlgText( LPCWSTR szText, SUIElement *pElement, DWORD dwFontColor, const SUICoord &Coord, int nCount = -1, DWORD dwFontFormat = 0xffffffff, bool bClip = false, float ZValue = 0.f, DWORD dwBgColor = 0, int nBorderFlag = 0 );
	void DrawSprite( EtTextureHandle hTexture, SUICoord &UVCoord, DWORD dwUIColor, SUICoord &Coord, float fRotate = 0.0f, float ZValue = 0.f );
	void DrawSprite( SUICoord &UVCoord, DWORD dwUIColor, SUICoord &Coord, float fRotate = 0.0f, float ZValue = 0.f );
	void DrawRect( SUICoord &Coord, DWORD dwColor );

public:
	virtual void Initialize( const char *pFileName, bool bShow );
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate() {}
	virtual CEtUIControl *CreateControl( SUIControlProperty *pProperty );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Render( float fElapsedTime );
	virtual void Process( float fElapsedTime );
	virtual void UpdateDlgCoord( float fX, float fY, float fWidth, float fHeight );
	virtual void UpdateRects();
	virtual void OnCloseCompletely() {}
	// 스킬트리에서 사용함. 형제 다이얼로그의 포인터를 리턴.
	// content 다이얼로그가 스킬트리처럼 겹쳐있으면 입력 메시지가 작은 다이얼로그에 오지 않기 때문에 
	// CEtUIDialog::ShowChildDialog() 함수 호출 시에 체크해서 포커싱 처리시에 가장 뒤에 오도록 셋팅합니다.
	virtual void GetContentDialog( /*IN OUT*/ std::vector<CEtUIDialog*>& vlpContentDialogs ) {};

	virtual void OnChangeResolution();
	static void ProcessChangeResolution();

	virtual void OnEndInitialize();

	virtual void OnHandleMsg(int protocol, char* pData) {}
};

template < typename T > 
void CEtUIDialog::GetGontrolList( std::vector< T* > &vecControl )
{
	for( int i=0; i<(int)m_vecControl.size(); i++ )
	{
		if( m_vecControl[i] )
		{
			if( typeid(*m_vecControl[i]) == typeid( T ) )
			{
				vecControl.push_back( (T*)m_vecControl[i] );
			}
		}
	}
}

#define CONTROL( cls, name )	_GC<CEtUI##cls>(#name)
#define DN_CONTROL( cls, name )	_GC<CDn##cls>(#name)
// 최적화 버전
template < typename T > 
T* CEtUIDialog::_GC( const char *pszControlName )
{
	std::map< std::string, CEtUIDialog*>::iterator it = m_cachedControls.find(pszControlName);
	if( it != m_cachedControls.end() ) {
		return (T*)it->second;
	}
	bool bExistControl = false;
	T *pResult = GetControl<T>( pszControlName, &bExistControl);
	if( bExistControl ) {	// 존재하지 않는 경우엔 계속 경고메세지가 떠서 알 수  있도록 한다..
		m_cachedControls[ pszControlName ] = (CEtUIDialog*)pResult;
	}
	else {
		static bool bFirst = true;
		if( bFirst ) {
			ASSERT( false );
			bFirst = false;
		}
	}
	return pResult;
}

template < typename T > 
T* CEtUIDialog::GetControl( const char *pszControlName, bool *pExistControl )
{
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		if( strcmp( pszControlName, m_vecControl[ i ]->GetControlName() ) == 0 )
		{
#if defined(_DEBUG) || defined(_RDEBUG)			// 혹시 모를 실수에 대비해 디버깅 시에만 타입체크를 해준다.
			T* pResult = dynamic_cast<T*>(m_vecControl[ i ]);
			if( !pResult ) {
				ASSERT( 0 && "UI Control 캐스팅 실패!!");
				T* pTempControl = new T( 0 );
				m_TempControlList.push_back( pTempControl );
				if( pExistControl ) *pExistControl = false;
				return pTempControl;
			}
#else
			T* pResult = static_cast<T*>(m_vecControl[ i ]);
#endif
			if( pExistControl ) *pExistControl = true;
			return pResult;
		}
	}

//#if defined(_DEBUG) || defined(_RDEBUG)
	//assert( 0 && "UI Control 을 찾을 수 없습니다! 자세한 정보는 EtUIDialog.h의 374라인으로");
	char szFileName[256];
	_GetFullFileName(szFileName, _countof(szFileName), m_strDialogFileName.c_str());
    CDebugSet::ToLogFile(" [UI Error] %s \"%s\" 未找到!\n", szFileName, pszControlName);
//#endif

	// 파일에 출력할때는 한번 출력한거 건너뛰게 하기 위해, string 맵에 넣어두고 없을때만 출력.
	if( s_bUITest )
	{
		std::string szKey = szFileName;
		szKey += "_";
		szKey += pszControlName;

		std::map<std::string, int>::iterator it = s_mapUITest.find( szKey );
		if( it == s_mapUITest.end() )
		{
			FILE *fp = NULL;
			fopen_s( &fp, "uitest.txt", "at" );
			fprintf_s( fp, "[UI Error] \"%s\" 파일의 %s Control 을 찾을 수 없습니다!\n", szFileName, pszControlName );
			fclose( fp );
			s_mapUITest.insert( make_pair( szKey, 1 ) );
		}
	}

	// 컨트롤이 없어도 뻑은 나지 않는다.  대신 동작은 제대로 하지 않음..
	for( DWORD i=0; i<m_TempControlList.size(); i++ ) {
		if( strcmp( m_TempControlList[i]->GetControlName(), pszControlName ) == NULL ) return static_cast<T*>(m_TempControlList[i]);
	}
	T* pTempControl = new T( 0 );
	pTempControl->SetParent( this );
	if( pszControlName ) _strcpy( pTempControl->GetProperty()->szUIName, _countof(pTempControl->GetProperty()->szUIName), pszControlName, (int)strlen(pszControlName) );
	m_TempControlList.push_back( pTempControl );
	if( pExistControl ) *pExistControl = false;
	return pTempControl;
}
