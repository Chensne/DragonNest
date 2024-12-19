#include "StdAfx.h"
#include "EtUIDialog.h"
#include "EtUIControl.h"
#include "EtUIStatic.h"
#include "EtUIButton.h"
#include "EtUICheckBox.h"
#include "EtUIRadioButton.h"
#include "EtUIScrollBar.h"
#include "EtUISlider.h"
#include "EtUIListBox.h"
#include "EtUIComboBox.h"
#include "EtUIEditBox.h"
#include "EtUIIMEEditBox.h"
#include "EtUIProgressBar.h"
#include "EtUITemplate.h"
#include "EtUIMan.h"
#include "EtUIControlCreator.h"
#include "EtUISound.h"
#include "DebugSet.h"
#include "EtResourceMng.h"
#include "EtType.h"
#include "EtLoader.h"
#include "EtFontMng.h"
#include "EtSprite.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUIDialog *CEtUIDialog::s_pTooltipDlg = NULL;
int CEtUIDialog::s_nTooltipStringIndex = 0;
std::wstring CEtUIDialog::s_strTooltipString = L"";

CEtUIDialog *CEtUIDialog::s_pFadeDlg = NULL;

float CEtUIDialog::s_fScreenMouseX = 0.0f;
float CEtUIDialog::s_fScreenMouseY = 0.0f;

float CEtUIDialog::s_fDialogScale = 1.0f;
float CEtUIDialog::s_fDialogScaleValueVert = 0.0f;
float CEtUIDialog::s_fDialogScaleValueHori = 0.0f;
float CEtUIDialog::s_fMinDialogScale = 1.0f;
float CEtUIDialog::s_fMaxDialogScale = 1.0f;
int CEtUIDialog::s_nUISize = 0;
void (_stdcall *CEtUIDialog::s_pAutoCursorPtr)( bool bShow ) = NULL;

int CEtUIDialog::s_nDialogTextureSize = 0;

// 0.5f 로 설정할 경우 오프셋이 너무 커서 좌측,우측 메뉴가 겹쳐보이게 된다.(#15687 이슈때문에 0.18로 수정합니다.)
#define UI_DIALOG_SCALE_VALUE	0.18f

//#if defined(_DEBUG) || defined(_RDEBUG)
bool CEtUIDialog::s_bUITest = false;
std::map<std::string, int> CEtUIDialog::s_mapUITest;
//#endif

CEtUIControl *CEtUIDialog::s_pMouseEnterControl = NULL;

#define CHECK_DIALOG_CRASH
#ifdef CHECK_DIALOG_CRASH
char g_szCurMsgProcDialog[256] = {0,};
#endif

#pragma warning(disable:4482)
CEtUIDialog::CEtUIDialog( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
	: CEtUIDialogBase(dialogType)
{
	m_nDialogID = nID;
	m_bShow = true;
	m_bAcceptInputMsgWhenHide = false;
	m_hWnd = NULL;
	m_pDefaultControl = NULL;
	m_pCallback = pCallback;
	m_fScreenWidthRatio = 1.0f;
	m_fScreenHeightRatio = 1.0f;
	m_fScreenWidthBorder = 0.0f;
	m_fScreenHeightBorder = 0.0f;
	m_fScreenWidth = 0.0f;
	m_fScreenHeight = 0.0f;
	m_bMouseInDialog = false;
	m_FadeMode = FadeModeEnum::None;
	m_bFadeOut = false;
	m_bEndInitialize = false;
	m_fMouseX = 0.0f;
	m_fMouseY = 0.0f;
	m_HotKeyState = 0;
	m_nHotKey = 0;
	m_bPassMessageToChild = false;
	m_bShowModal = true;
	m_bAutoCursor = bAutoCursor;
	m_bCursor = false;
	m_bElementDialog = false;
	m_bElementDialogShowState = false;

	if( dialogType >= UI_TYPE_CHILD && dialogType <= UI_TYPE_CHILD_MODAL )
	{
		ASSERT( pParentDialog&&"CEtUIDialog::CEtUIDialog" );
	}

	m_pParentDialog = pParentDialog;
	if( m_pParentDialog )
	{
		// Note : 부모가 있으면 자식 윈도우로 등록한다.
		//
		m_pParentDialog->AddChildDialog( this );

		if( dialogType == UI_TYPE_CHILD_MODAL )
		{
			m_pParentDialog->AddChildModalDialog( this );
		}

		// 당연히 이게 맞는 줄 알고 고쳤더니 차일드들이 안뜬다.
		// 찾아보니, 차일드리스트만 렌더링하고, 차일드모달은 MsgProc에서 모달처리만 하고 더이상 안쓰는 것.
		// 왜 이렇게 했을까..
		//if( dialogType == UI_TYPE_CHILD )
		//{
		//	m_pParentDialog->AddChildDialog( this );
		//}
		//else if( dialogType == UI_TYPE_CHILD_MODAL )
		//{
		//	m_pParentDialog->AddChildModalDialog( this );
		//}
	}
	m_fLastShowDelta = 10.f;
	m_bLoadedTexture = false;
}

CEtUIDialog::~CEtUIDialog(void)
{
	if( m_bAutoCursor && m_bCursor && s_pAutoCursorPtr ) {
		s_pAutoCursorPtr( false );
		m_bCursor = false;
	}
	PopFocusControl();
	DeleteAllControl();
	SAFE_DELETE_PVEC( m_vecUITemplate );
	if( m_hTexture && m_hTexture->GetRefCount() == 1 )
		s_nDialogTextureSize -= m_hTexture->GetFileSize();
	SAFE_RELEASE_SPTR( m_hTexture );
	if( m_hDlgTexture && m_hDlgTexture->GetRefCount() == 1 )
		s_nDialogTextureSize -= m_hDlgTexture->GetFileSize();
	SAFE_RELEASE_SPTR( m_hDlgTexture );
	m_bLoadedTexture = false;

	SAFE_DELETE_PVEC( m_TempControlList );

	// 생성자에서 등록하길래, 소멸자에서 삭제하려고 했더니, 툴팁의 static 다이얼로그때문에 뻑났었다.
	// 소멸순서만 잘 맞춰주면 되니, 제일 하단 자식부터 차례대로 삭제하면 된다.
	if( m_pParentDialog )
	{
		// 부모가 있으면 자식 윈도우리스트에서 제거.
		m_pParentDialog->DelChildDialog( this );

		if( m_emDialogType == UI_TYPE_CHILD_MODAL )
		{
			m_pParentDialog->DelChildModalDialog( this );
		}
	}
}

void CEtUIDialog::OnEndInitialize()
{
	m_bEndInitialize = true;
}

void CEtUIDialog::Initialize( bool bShow )
{
	// Note : 아래 함수들은 호출되는 순서가 중요하다.
	//
	UpdateScreen();

	InitialUpdate();

	if( !IsRootDialog() )
	{
		m_pParentDialog->ShowChildDialog( this, bShow );
	}
	else
	{
		Show( bShow );
	}

	OnEndInitialize();
	ThreadDelay();
}

void CEtUIDialog::Initialize( const char *pFileName, bool bShow )
{
	// Note : 아래 함수들은 호출되는 순서가 중요하다.
	//
	UpdateScreen();

	m_strDialogFileName.clear();

	if( pFileName )
	{
		m_strDialogFileName = pFileName;

		CResMngStream Stream( pFileName );

		if( Stream.IsValid() ) 
		{
			if( !Load( Stream ) )
			{
				CDebugSet::ToLogFile( "CEtUIDialog::Initialize, %s load failed!", pFileName );
			}
		}
		else
		{
			CDebugSet::ToLogFile( "CEtUIDialog::Initialize, %s File Not Found!", pFileName );
		}
	}

	InitialUpdate();

	if( !IsRootDialog() )
	{
		m_pParentDialog->ShowChildDialog( this, bShow );
	}
	else
	{
		Show( bShow );
	}

	OnEndInitialize();
}

bool CEtUIDialog::Load( CStream &Stream )
{
	SUIFileHeader Header;
	char szReserved[ UI_HEADER_RESERVED ]={0};

	Stream.ReadBuffer( &Header, sizeof( SUIFileHeader ) );
	Stream.ReadBuffer( szReserved, UI_HEADER_RESERVED );

	switch( Header.nVersion )
	{
	case UI_FILE_VERSION_01:
		Load_01( Stream, Header );
		break;
	case UI_FILE_VERSION:
		{
			m_DlgInfo.Load(Stream);

			for( int i = 0; i < Header.nTemplateCount; i++ )
			{
				CEtUITemplate *pTemplate = new CEtUITemplate();
				pTemplate->Load( Stream );
				pTemplate->OnLoaded();
				m_vecUITemplate.push_back( pTemplate );
			}

			for( int i = 0; i < Header.nControlCount; i++ )
			{
				SUIControlProperty Property;
				Property.Load(Stream);

				CEtUIControl *pControl = CreateControl( &Property );
				if( Property.bDefaultControl )
				{
					m_pDefaultControl = pControl;
				}
			}
		}
		break;
	default:
		ASSERT(0&&"CEtUIDialog::Load, default");
		break;
	}

	// UI쪽 텍스쳐는 크기에 따라 민감하게 반응해서 2의 승수 지원하지 않는 카드랑 똑같이 보여야 해서 이렇게 한다.
	//CEtTexture::SetPow2( true );
	/*
	if( m_hTexture && m_hTexture->GetRefCount() == 1 )
		s_nDialogTextureSize -= m_hTexture->GetFileSize();
	SAFE_RELEASE_SPTR( m_hTexture );
	if( EtInterface::g_bEtUIInitTool ) CEtResource::FlushWaitDelete( RT_TEXTURE );
	m_hTexture = LoadResource( m_DlgInfo.szUITexturename, RT_TEXTURE, true );
	if( m_hTexture && m_hTexture->GetRefCount() == 1 )
		s_nDialogTextureSize += m_hTexture->GetFileSize();
	if( m_hDlgTexture && m_hDlgTexture->GetRefCount() == 1 )
		s_nDialogTextureSize -= m_hDlgTexture->GetFileSize();
	SAFE_RELEASE_SPTR( m_hDlgTexture );
	if( EtInterface::g_bEtUIInitTool ) CEtResource::FlushWaitDelete( RT_TEXTURE );
	m_hDlgTexture = LoadResource( m_DlgInfo.szDlgTextureName, RT_TEXTURE, true );
	if( m_hDlgTexture && m_hDlgTexture->GetRefCount() == 1 )
		s_nDialogTextureSize += m_hDlgTexture->GetFileSize();
	*/
	
	LoadDialogTexture();
	//CEtTexture::SetPow2( false );

	m_renderDlgColor.dwColor[UI_STATE_NORMAL] = m_DlgInfo.dwDlgColor;
	m_renderDlgColor.dwCurrentColor = m_DlgInfo.dwDlgColor;
	m_BaseDlgCoord = m_DlgInfo.DlgCoord;

	UpdateRects();
	UpdateAllControl();

	for( int i = 0; i < Header.nTemplateCount; i++ )
		SAFE_RELEASE_SPTR( m_vecUITemplate[i]->m_hTemplateTexture );

	return true;
}

bool CEtUIDialog::Save( CStream &Stream )
{
	SUIFileHeader Header;
	char szReserved[ UI_HEADER_RESERVED ]={0};
	
	Header.nTemplateCount = ( int ) m_vecUITemplate.size();
	Header.nControlCount = ( int )m_vecControl.size();

	Stream.WriteBuffer( &Header, sizeof( SUIFileHeader ) );
	Stream.WriteBuffer( szReserved, UI_HEADER_RESERVED );

	m_DlgInfo.Save(Stream);

	for( int i = 0; i < Header.nTemplateCount; i++ )
	{
		m_vecUITemplate[i]->Save( Stream );
	}

	for( int i = 0; i < Header.nControlCount; i++ )
	{
		SUIControlProperty Property;
		m_vecControl[i]->GetProperty( Property );
		Property.Save(Stream);
	}

	return true;
}

bool CEtUIDialog::Load_01( CStream &Stream, SUIFileHeader &fileheader )
{
	m_DlgInfo.Load(Stream);

	ASSERT(fileheader.nFontCount&&"CEtUIDialog::Load, Font Count is 0!");

	for( int i = 0; i < fileheader.nFontCount; i++ )
	{
		SFontInfo FontInfo;
		FontInfo.Load(Stream);
	}

	for( int i = 0; i < fileheader.nTemplateCount; i++ )
	{
		CEtUITemplate *pTemplate = new CEtUITemplate();
		pTemplate->Load( Stream );
		m_vecUITemplate.push_back( pTemplate );
	}

	for( int i = 0; i < fileheader.nControlCount; i++ )
	{
		SUIControlProperty Property;
		Property.Load(Stream);

		CEtUIControl *pControl = CreateControl( &Property );
		if( Property.bDefaultControl )
		{
			m_pDefaultControl = pControl;
		}
	}

	return true;
}

void CEtUIDialog::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if( bShow )
	{
		if( !m_bLoadedTexture ) {
			LoadDialogTexture();
		}
		SetFadeIn();
		PushFocusControl();
	}
	else
	{
		SetFadeOut();
		PopFocusControl();
		
		// 창이 닫히는 상황에서 컨트롤을 누른채 있다면,(강화, 제작요청창의 취소버튼)
		// SetCapture 후 ReleaseCapture가 호출되지 않거나, m_bPressed가 true인 상태로 유지된다거나해서
		// 툴팁 및 마우스오버 렌더링이 제대로 이뤄지지 않게된다.
		// 그래서 컨트롤들을 뒤져서 눌러진 컨트롤들은 원래대로 돌려놓는다.
		//
		// 확장형 리스트박스의 아이템으로 들어가는 엘리먼트 다이얼로그는 SetPressed를 적용시키지 않았었다.
		// 그러나, SELF타입으로 빠지면서 별도처리를 하지 않아도 된다.
		if( !IsElementDialog() )
		{
			bool bTooltipControl = false;

			for( int i = 0; i < ( int )m_vecControl.size(); i++ )
			{
				if( m_vecControl[ i ]->IsPressed() || m_vecControl[ i ]->IsRightPressed() || m_vecControl[ i ]->IsHaveScrollBarControl() )
				{
					m_vecControl[ i ]->SetPressed( false );
					m_vecControl[ i ]->SetRightPressed( false );
					ReleaseCapture();
				}

				// m_bPressed 뿐만 아니라 m_bMouseEnter도 필요하게 되었다.
				// CEtUIDialog의 MsgProc에서 WM_MOUSEMOVE가 들어올때 CEtUIDialog::OnMouseMove함수가 호출되는데,
				// 이 함수를 보면 자신의 다이얼로그 영역안에 마우스 포인터가 없을때 ReleaseMouseEnterControl함수를 호출한다.
				// 이 함수호출을 통해 보통 대부분의 상황에서 해당 컨트롤의 m_bMouseEnter가 풀리게 마련인데,
				// 딱 한가지 경우가 문제였다.
				// 바로, 튜토리얼을 플레이하시겠습니까.라는 메세지박스였는데,
				// (이건 유일하게 다른 모든 창을 다 Hide시킨 상태에서 나오는 메세지박스다.)
				// 이 메세지박스에서 아니오를 누르면 페이드 아웃이 되면서 마을로 가게된다.
				// 이때 아무것도 보여지는 다이얼로그가 없으니 MsgProc가 아예 호출되지 않게되고 이 결과
				// ReleaseMouseEnterControl함수가 한번도 호출되지 않아 아니오버튼의 MouseEnter값을 아무도 변경하지 않는다.
				// 그리고나서 CEtUIDialog::s_pMouseEnterControl에 강제로 NULL을 대입하는 DnInterface::Finalize함수 등이 호출되면서,
				// 아니오 버튼의 m_bMouseEnter는 직접 마우스가 아니오버튼안에 들어왔다 나가지 않는 이상 변하지 않게된다.
				//
				// 생각해보니 Show(false)되는 다이얼로그의 컨트롤의 MouseEnter값이 true인거 자체가 이상한거 같아서,
				// 여기서 해제하기로 하겠다.
				if( m_vecControl[ i ]->IsMouseEnter() )
				{
					m_vecControl[ i ]->MouseEnter(false);
				}

				if( s_pTooltipDlg && s_pTooltipDlg->GetTooltipCtrl() && m_vecControl[ i ] == s_pTooltipDlg->GetTooltipCtrl() )
					bTooltipControl = true;
			}

			if( bTooltipControl && s_pTooltipDlg != this ) s_pTooltipDlg->Show( false );
		}
	}

	if( m_emDialogType == UI_TYPE_MODAL && m_bShowModal )
	{
		ShowModal( this, bShow );
	}

	if( m_bEndInitialize && m_DlgInfo.bSound )
	{
		if( bShow )
		{
			GetUISound().Play( CEtUISound::UI_WINDOW_OPEN );
		}
		else
		{
			GetUISound().Play( CEtUISound::UI_WINDOW_CLOSE );
		}
	}

	m_bShow = bShow;
	m_fMouseX = 0.0f;
	m_fMouseY = 0.0f;
	m_HotKeyState = 0;
	m_nHotKey = 0;

	if( m_bAutoCursor && s_pAutoCursorPtr ) {
		if( m_bShow && m_bCursor == false ) {
			m_bCursor = true;
			s_pAutoCursorPtr( true );
		}
		else if( m_bShow == false && m_bCursor ) {
			m_bCursor = false;
			s_pAutoCursorPtr( false );
		}
	}
}

void CEtUIDialog::PointToFloat( POINT pt, float &fX, float &fY )
{
	fX = pt.x / GetScreenWidth() - GetXCoord();
	fY = pt.y / GetScreenHeight() - GetYCoord();
}

CEtUIControl *CEtUIDialog::GetControl( const char *pszControlName )
{
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		if( strcmp( pszControlName, m_vecControl[ i ]->GetControlName() ) == 0 )
		{
			return m_vecControl[ i ];
		}
	}

	CDebugSet::ToLogFile( "CEtUIDialog::GetControl, %s control not found!", pszControlName );
	// Note : UI가 생성되는 부분이라 ASSERT()를 호출하지 않고 assert()를 호출한다.
	//
	//assert(0&&"CEtUIDialog::GetControl, 경대를 불러주세요!");
	return NULL;
}

CEtUIControl *CEtUIDialog::CreateControl( SUIControlProperty *pProperty )
{
	CEtUIControl *pControl = CreateControl( pProperty->UIType );
	pControl->Initialize( pProperty );

	return pControl;
}

CEtUIControl *CEtUIDialog::CreateControl( UI_CONTROL_TYPE Type )
{
	static CEtUIControlCreator controlCreator;
	CEtUIControl *pControl(NULL);

	pControl = controlCreator.CreateControl( Type, this );
	if( pControl )
	{
		m_vecControl.push_back( pControl );
	}

	return pControl;
}

void CEtUIDialog::DeleteControl( CEtUIControl *pControl )
{
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		if( m_vecControl[ i ] == pControl )
		{
			if( s_pMouseEnterControl == pControl )
				s_pMouseEnterControl = NULL;
			if( focus::IsSameControl(pControl) )
				focus::ReleaseControl();

			delete m_vecControl[ i ];
			m_vecControl.erase( m_vecControl.begin() + i );
			break;
		}
	}
}

void CEtUIDialog::DeleteAllControl()
{
	if( focus::IsSameParent(this) )
	{
		focus::ReleaseControl();
	}

	// 안정성 코드.
	// 확장형 리스트박스의 경우 Show( false )가 되는 동시에 다이얼로그 및 자식 컨트롤들을 지우는데,
	// 이때 마우스를 대고 있었다면, 아래 스태틱 포인터변수의 값이 delete된 컨트롤이 되버린다.
	// 그래서 지울때 확인 후 지우도록 하겠다.
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		if( s_pMouseEnterControl == m_vecControl[ i ] )
		{
			s_pMouseEnterControl->MouseEnter( false );
			s_pMouseEnterControl = NULL;

			ShowTooltipDlg( NULL, false );	
		}
	}

	SAFE_DELETE_PVEC( m_vecControl );
}

bool CEtUIDialog::FindControl( std::vector< CEtUIControl* > &vecControl, int nTypeCount, UI_CONTROL_TYPE *pType, bool bCheckCoveredControl, std::vector<SUICoord> &vecDlgCoord )
{
	std::list< CEtUIDialog* >::reverse_iterator iter;
	if( !m_listChildModalDialog.empty() )
	{
		iter = m_listChildModalDialog.rbegin();
		if( iter != m_listChildModalDialog.rend() )
		{
			if( (*iter) && (*iter)->IsShow() )
			{
				(*iter)->FindControl( vecControl, nTypeCount, pType, bCheckCoveredControl, vecDlgCoord );
				return true;
			}
		}
	}

	CEtUIDialog *pDialog = NULL;
	iter = m_listChildDialog.rbegin();
	for( ; iter != m_listChildDialog.rend(); ++iter )
	{
		// LastRender검사는 우선 생략.
		pDialog = (*iter);
		if( !pDialog ) continue;
		if( (pDialog->GetDialogType() != UI_TYPE_SELF && pDialog->IsElementDialog() == false && pDialog->IsShow()) ||
			(pDialog->IsElementDialog() && pDialog->IsShowElementDialog()) )
		{
			if( pDialog->FindControl( vecControl, nTypeCount, pType, bCheckCoveredControl, vecDlgCoord ) )
				return true;
		}
	}

	std::vector<int> vecType;
	for( int i = 0; i < nTypeCount; ++i )
	{
		int nValue = pType[i];
		vecType.push_back( nValue );
	}

	CEtUIControl *pControl(NULL);
	int nVecCtlSize = ( int )m_vecControl.size();
	for( int i = 0; i < nVecCtlSize; i++ )
	{
		pControl = m_vecControl[i];

		bool bMatched = false;
		for( int j = 0; j < (int)vecType.size(); ++j )
		{
			if( pControl->GetType() == vecType[j] )
			{
				bMatched = true;
				break;
			}
		}
		if( bMatched || (int)vecType.size() == 0 )
		{
			if( pControl->IsShow() )
			{
				bool bPushControl = true;
				if( bCheckCoveredControl )
				{
					// 일반적인 컨트롤로 생각해 가운데 지점이 다른 창에 가려져있는지 확인한다.
					// 리스트박스같은 컨트롤의 경우 리스트아이템들마다 일일이 체크하기엔 너무 검사할게 많아져서 간단하게 체크만 하는 것이다.
					SUICoord ControlCoord;
					pControl->GetUICoord( ControlCoord );
					ControlCoord.fX = ControlCoord.fX + ControlCoord.fWidth/2.0f;
					ControlCoord.fY = ControlCoord.fY + ControlCoord.fHeight/2.0f;
					ControlCoord = DlgCoordToScreenCoord( ControlCoord );

					for( int k = 0; k < (int)vecDlgCoord.size(); ++k )
					{
						if( ControlCoord.fX >= vecDlgCoord[k].fX && ControlCoord.fY >= vecDlgCoord[k].fY &&
							ControlCoord.fX <= vecDlgCoord[k].fX+vecDlgCoord[k].fWidth && ControlCoord.fY <= vecDlgCoord[k].fY+vecDlgCoord[k].fHeight )
						{
							bPushControl = false;
							break;
						}
					}
				}

				if( bPushControl )
					vecControl.push_back( pControl );
			}
		}
	}

	if( bCheckCoveredControl && IsElementDialog() == false )
	{
		SUICoord DlgCoord;
		GetDlgCoord( DlgCoord );
		DlgCoord.fX = 0.0f;
		DlgCoord.fY = 0.0f;
		DlgCoord = DlgCoordToScreenCoord( DlgCoord );
		vecDlgCoord.push_back( DlgCoord );
	}

	return false;
}

void CEtUIDialog::MoveToHead( CEtUIControl *pControl )
{
	int nSize = ( int )m_vecControl.size();
	for( int i = 0; i < nSize; i++ ) {
		if( m_vecControl[ i ] == pControl ) {
			m_vecControl.erase( m_vecControl.begin() + i );
			m_vecControl.insert( m_vecControl.begin(), pControl );
			break;
		}
	}
}

void CEtUIDialog::MoveToTail( CEtUIControl *pControl )
{
	int nSize = ( int )m_vecControl.size();
	for( int i = 0; i < nSize; i++ ) {
		if( m_vecControl[ i ] == pControl ) {
			m_vecControl.erase( m_vecControl.begin() + i );
			m_vecControl.push_back( pControl );
			break;
		}
	}
}

bool CEtUIDialog::IsUsableFocusControl()
{
	if( focus::IsSameParent(this) && focus::IsEnable() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

int CEtUIDialog::FindControlIndex( CEtUIControl *pControl )
{
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		if( pControl == m_vecControl[ i ] )
		{
			return i;
		}
	}

	return -1;
}

CEtUIControl *CEtUIDialog::GetNextControl( CEtUIControl *pControl )
{
	if( m_vecControl.empty() )
	{
		return NULL;
	}

	int nIndex;

	nIndex = FindControlIndex( pControl );
	if( nIndex == -1 )
	{
		nIndex = ( int )m_vecControl.size() - 1;
	}
	if( nIndex == ( int )m_vecControl.size() - 1 )
	{
		return m_vecControl[ 0 ];
	}
	else
	{
		return m_vecControl[ nIndex + 1 ];
	}
}

CEtUIControl *CEtUIDialog::GetPrevControl( CEtUIControl *pControl )
{
	if( m_vecControl.empty() )
	{
		return NULL;
	}

	int nIndex;

	nIndex = FindControlIndex( pControl );
	if( nIndex == -1 )
	{
		nIndex = 0;
	}
	if( nIndex == 0 )
	{
		return m_vecControl[ m_vecControl.size() - 1 ];
	}
	else
	{
		return m_vecControl[ nIndex - 1 ];
	}
}

bool CEtUIDialog::OnCycleFocus( bool bForward )
{
	if( !focus::IsSameParent(this) )
	{
		//pControl = NULL;
		return false;
	}

	CEtUIControl *pControl = focus::GetControl();
	int nLoopCount(0);

	while( true )
	{
		if( bForward )
		{
			pControl = GetNextControl( pControl );
		}
		else
		{
			pControl = GetPrevControl( pControl );
		}

		if( pControl == NULL )
		{
			break;
		}

		if( focus::IsSameControl(pControl) )
		{
			return true;
		}

		if( pControl->CanHaveFocus() )
		{
			RequestFocus( pControl );
			return true;
		}

		nLoopCount++;

		if( nLoopCount > ( int )m_vecControl.size() )
		{
			break;
		}
	}

	return false;
}

void CEtUIDialog::RequestFocus( CEtUIControl* pControl )
{
	if( !pControl )
		return;

	if( focus::IsSameControl(pControl) )
	{
		return;
	}

	if( !pControl->CanHaveFocus() )
	{
		return;
	}

	focus::SetFocus(pControl);
}

void CEtUIDialog::PushFocusControl()
{
	focus::PushControl(this);

	// 도대체 이걸 왜 호출했는지...
	// 이것때문에 서버리스트 show(true)될때 포커스를 리스트박스에 두라고 했는데도,
	// 하나 다음으로 건너서 접속버튼이 포커스를 가지게 되었다.(리스트박스가 3번, 접속버튼이 4번이다.)
	// 우선 이거 호출 안하기로 하니, 이상한 점 발견되면 그때 찾도록 하겠다.
	//OnCycleFocus( true );
}

void CEtUIDialog::PopFocusControl()
{
	focus::PopControl(this);
}

void CEtUIDialog::ShowAllControl( bool bShow )
{
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		m_vecControl[ i ]->Show( bShow );
	}
}

void CEtUIDialog::UpdateAllControl()
{
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		m_vecControl[ i ]->UpdateRects();
	}
}

void CEtUIDialog::EnableAllControl( bool bEnable )
{
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		m_vecControl[ i ]->Enable( bEnable );
	}
}

void CEtUIDialog::ClearRadioButtonGroup( int nButtonGroup )
{
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		if( m_vecControl[ i ] && m_vecControl[ i ]->GetType() == UI_CONTROL_RADIOBUTTON )
		{
			CEtUIRadioButton *pRadioButton = ( CEtUIRadioButton * )m_vecControl[ i ];
			if( pRadioButton->GetButtonGroup() == nButtonGroup )
			{
				pRadioButton->SetChecked( false, false );
			}
		}
	}
}

void CEtUIDialog::ReloadDlgTexture()
{
	// UI쪽 텍스쳐는 크기에 따라 민감하게 반응해서 2의 승수 지원하지 않는 카드랑 똑같이 보여야 해서 이렇게 한다.
	//CEtTexture::SetPow2( true );
	if( m_hDlgTexture )
		s_nDialogTextureSize -= m_hDlgTexture->GetFileSize();
	SAFE_RELEASE_SPTR( m_hDlgTexture );
	m_hDlgTexture = LoadResource( m_DlgInfo.szDlgTextureName, RT_TEXTURE, true );
	if( m_hDlgTexture )
		s_nDialogTextureSize += m_hDlgTexture->GetFileSize();
	//CEtTexture::SetPow2( false );
}

void CEtUIDialog::DeleteDlgTexture()
{
	ZeroMemory( m_DlgInfo.szDlgTextureName, _countof(m_DlgInfo.szDlgTextureName) );
	SAFE_RELEASE_SPTR( m_hDlgTexture );
}

void CEtUIDialog::UpdateRects()
{
	if( ( !m_hDlgTexture ) )
	{
		return;
	}

	if( (m_DlgInfo.nFrameLeft > 0) || (m_DlgInfo.nFrameRight > 0) || (m_DlgInfo.nFrameTop > 0) || (m_DlgInfo.nFrameBottom > 0) )
	{
		UpdateFrameRectsEx();
	}
}

void CEtUIDialog::UpdateFrameRectsEx()
{
	// Note : 다이얼로그는 크기 변경시 깨지지 않도록 하기 위해 텍스쳐를 9부분으로 나누어서 랜더한다.
	//		좌우,위아래 값은 툴에서 셋팅한다. 가운데 부분은 늘어나기 때문에 디자인시 고려해야한다.
	//
	float fFrameLeft = m_DlgInfo.nFrameLeft / ( float )m_hDlgTexture->OriginalWidth();
	float fFrameTop = m_DlgInfo.nFrameTop / ( float )m_hDlgTexture->OriginalHeight();
	float fFrameRight = m_DlgInfo.nFrameRight / ( float )m_hDlgTexture->OriginalWidth();
	float fFrameBottom = m_DlgInfo.nFrameBottom / ( float )m_hDlgTexture->OriginalHeight();
	float fModWidth = 1.0f - (fFrameLeft+fFrameRight);
	float fModHeight = 1.0f - (fFrameTop+fFrameBottom);

	if( fModWidth <= 0.0f || fModHeight <= 0.0f )
	{
		//ASSERT(0&&"Frame Left+Right합산 혹은 Top+Bottom합산이 원본텍스처 크기를 넘었습니다.");
	}

	m_vecDlgFrameElement.resize( 9 );
	m_vecDlgFrameElement[ 0 ].UVCoord.SetCoord( 0.0f,					0.0f,					fFrameLeft,		fFrameTop );
	m_vecDlgFrameElement[ 1 ].UVCoord.SetCoord( fFrameLeft,				0.0f,					fModWidth,		fFrameTop );
	m_vecDlgFrameElement[ 2 ].UVCoord.SetCoord( fFrameLeft+fModWidth,	0.0f,					fFrameRight,	fFrameTop );
	m_vecDlgFrameElement[ 3 ].UVCoord.SetCoord( 0.0f,					fFrameTop,				fFrameLeft,		fModHeight );
	m_vecDlgFrameElement[ 4 ].UVCoord.SetCoord( fFrameLeft,				fFrameTop,				fModWidth,		fModHeight );
	m_vecDlgFrameElement[ 5 ].UVCoord.SetCoord( fFrameLeft+fModWidth,	fFrameTop,				fFrameRight,	fModHeight );
	m_vecDlgFrameElement[ 6 ].UVCoord.SetCoord( 0.0f,					fFrameTop+fModHeight,	fFrameLeft,		fFrameBottom );
	m_vecDlgFrameElement[ 7 ].UVCoord.SetCoord( fFrameLeft,				fFrameTop+fModHeight,	fModWidth,		fFrameBottom );
	m_vecDlgFrameElement[ 8 ].UVCoord.SetCoord( fFrameLeft+fModWidth,	fFrameTop+fModHeight,	fFrameRight,	fFrameBottom );

	fFrameLeft = m_DlgInfo.nFrameLeft / (float)DEFAULT_UI_SCREEN_WIDTH;
	fFrameTop = m_DlgInfo.nFrameTop / (float)DEFAULT_UI_SCREEN_HEIGHT;
	fFrameRight = m_DlgInfo.nFrameRight / (float)DEFAULT_UI_SCREEN_WIDTH;
	fFrameBottom = m_DlgInfo.nFrameBottom / (float)DEFAULT_UI_SCREEN_HEIGHT;
	fModWidth = m_DlgInfo.DlgCoord.fWidth - (fFrameLeft+fFrameRight);
	fModHeight = m_DlgInfo.DlgCoord.fHeight - (fFrameTop+fFrameBottom);

	m_vecDlgFrameCoord.resize( 9 );
	m_vecDlgFrameCoord[ 0 ].SetCoord( 0.0f,						0.0f,					fFrameLeft,		fFrameTop );
	m_vecDlgFrameCoord[ 1 ].SetCoord( fFrameLeft,				0.0f,					fModWidth,		fFrameTop );
	m_vecDlgFrameCoord[ 2 ].SetCoord( fFrameLeft + fModWidth,	0.0f,					fFrameRight,	fFrameTop );
	m_vecDlgFrameCoord[ 3 ].SetCoord( 0.0f,						fFrameTop,				fFrameLeft,		fModHeight );
	m_vecDlgFrameCoord[ 4 ].SetCoord( fFrameLeft,				fFrameTop,				fModWidth,		fModHeight );
	m_vecDlgFrameCoord[ 5 ].SetCoord( fFrameLeft + fModWidth,	fFrameTop,				fFrameRight,	fModHeight );
	m_vecDlgFrameCoord[ 6 ].SetCoord( 0.0f,						fFrameTop + fModHeight,	fFrameLeft,		fFrameBottom );
	m_vecDlgFrameCoord[ 7 ].SetCoord( fFrameLeft,				fFrameTop + fModHeight,	fModWidth,		fFrameBottom );
	m_vecDlgFrameCoord[ 8 ].SetCoord( fFrameLeft + fModWidth,	fFrameTop + fModHeight,	fFrameRight,	fFrameBottom );
}

//void CEtUIDialog::GetScreenSize( float &fWidth, float &fHeight )
//{
//	fHeight = ( float )GetEtDevice()->Height();
//	fWidth = fHeight * 4.0f / 3.0f;
//}

float CEtUIDialog::GetXCoord()
{
	float fBase(0.0f);

	switch( m_DlgInfo.AllignHori )
	{
		case AT_HORI_NONE:			fBase = GetScreenWidthBorderSize();																									break;
		case AT_HORI_LEFT:			fBase = s_fDialogScaleValueHori * UI_DIALOG_SCALE_VALUE;																			break;
		case AT_HORI_CENTER:		fBase = ( ( 1.0f + GetScreenWidthBorderSize() * 2.0f ) - m_DlgInfo.DlgCoord.fWidth ) * 0.5f;										break;
		case AT_HORI_RIGHT:			fBase = ( 1.0f + GetScreenWidthBorderSize() * 2.0f ) - m_DlgInfo.DlgCoord.fWidth - s_fDialogScaleValueHori * UI_DIALOG_SCALE_VALUE;	break;
		case AT_HORI_LEFT_FIXED:	fBase = 0.0f;																														break;
		case AT_HORI_RIGHT_FIXED:	fBase = ( 1.0f + GetScreenWidthBorderSize() * 2.0f ) - m_DlgInfo.DlgCoord.fWidth;													break;
	}

	return m_DlgInfo.DlgCoord.fX + fBase;
}

float CEtUIDialog::GetYCoord()
{
	float fBase(0.0f);

	switch( m_DlgInfo.AllignVert )
	{
		case AT_VERT_NONE:		fBase = GetScreenHeightBorderSize();	break;
		case AT_VERT_TOP:		fBase = 0.0f;							break;
		case AT_VERT_CENTER:	fBase = ( ( 1.0f + GetScreenHeightBorderSize() * 2.0f ) - m_DlgInfo.DlgCoord.fHeight ) * 0.5f;	break;
		case AT_VERT_BOTTOM:	fBase = ( 1.0f + GetScreenHeightBorderSize() * 2.0f ) - m_DlgInfo.DlgCoord.fHeight;				break;
	}

	return m_DlgInfo.DlgCoord.fY + fBase;
}

void CEtUIDialog::MoveDialog( float fX, float fY )
{
	m_DlgInfo.DlgCoord.fX += fX;
	m_DlgInfo.DlgCoord.fY += fY;
	UpdateRects();
}

void CEtUIDialog::GetPosition( float &fX, float &fY )
{ 
	fX = GetXCoord();
	fY = GetYCoord(); 
}

void CEtUIDialog::SetPosition( float fX, float fY )
{
	m_DlgInfo.DlgCoord.fX = fX;
	m_DlgInfo.DlgCoord.fY = fY;

	UpdateRects();
}

CEtUIControl *CEtUIDialog::GetControlAtPoint( float fX, float fY )
{
	CEtUIControl *pControl(NULL);

	for( int i=(int)m_vecControl.size()-1; i>=0; i-- )
	{
		pControl = m_vecControl[i];

		if (pControl && ( pControl->IsInside( fX, fY ) ) && ( pControl->IsShow() ))//( pControl->IsEnable() ) ) // commented by kalliste 090824
		{
			return pControl;
		}
	}

	return NULL;
}

void CEtUIDialog::SetTemplate( int nIndex, CEtUITemplate &Template )
{
	( *m_vecUITemplate[ nIndex ] ) = Template;

	int i;

	for( i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		if( m_vecControl[ i ]->GetTemplateIndex() == nIndex )
		{
			SUIControlProperty *pProperty = m_vecControl[ i ]->GetProperty();
			m_vecControl[ i ]->Initialize( pProperty );
		}
	}
}

int CEtUIDialog::AddTemplate( CEtUITemplate &Template )
{
	CEtUITemplate *pTemplate;
	pTemplate = new CEtUITemplate();
	(*pTemplate) = Template;
	m_vecUITemplate.push_back( pTemplate );

	return ( int )m_vecUITemplate.size() - 1;
}

void CEtUIDialog::DeleteTemplate( int nIndex )
{
	ASSERT( nIndex >= 0 && nIndex < (int)m_vecUITemplate.size() );

	delete m_vecUITemplate[ nIndex ];
	m_vecUITemplate.erase( m_vecUITemplate.begin() + nIndex );
}

void CEtUIDialog::DeleteAllTemplate()
{
	SAFE_DELETE_PVEC( m_vecUITemplate );
}

float CEtUIDialog::GetFontHeight( int &nFontSetIndex, int nFontHeight )
{
	SFontInfo FontInfo;
	CEtFontMng::GetInstance().GetFontInfo( nFontSetIndex, nFontHeight, FontInfo );

	return FontInfo.nFontHeight / GetScreenHeight();
}

//ID3DXFont *CEtUIDialog::GetFont( int &nFontSetIndex, int nFontHeight )
//{
//	SFontInfo FontInfo;
//	CEtFontMng::GetInstance().GetFontInfo( nFontSetIndex, nFontHeight, FontInfo );
//
//	return FontInfo.pFont;
//}

const wchar_t* CEtUIDialog::GetUIString( int nCategoryID, int nIndex )
{
	if( xml::IsValid() )
	{
		return xml::GetString( nCategoryID, nIndex );
	}

	return NULL;
}

bool CEtUIDialog::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_hWnd = hWnd;

	// Note : 다이얼로그가 보이지 않거나 메세지를 자식 윈도우로 보낼 필요가 없다면 return
	//
	if( !m_bShow && !m_bPassMessageToChild && !m_bAcceptInputMsgWhenHide )
	{
		return false;
	}
	else
	{
#ifdef CHECK_DIALOG_CRASH
		// 이미 깨진채로 들어오려나.
		strcpy_s( g_szCurMsgProcDialog, 256, m_strDialogFileName.c_str() );
#endif

		std::list< CEtUIDialog* >::reverse_iterator iter;

		// Note : 모달을 가지고 있고 현재 보이는 상태라면 모달을 처리한다.
		//
		if( !m_listChildModalDialog.empty() )
		{
			iter = m_listChildModalDialog.rbegin();
			if( iter != m_listChildModalDialog.rend() )
			{
				if( *iter != NULL )
				{
					if( (*iter)->IsShow() )
					{
						(*iter)->MsgProc( hWnd, uMsg, wParam, lParam );

						// 파티생성이름입력창이 CHILD_MODAL인데,
						// 이 입력창에서만 캔디데이트에서 선택한 중문이 입력안되는 현상이 나타났다.
						// 원래 코드가 MsgProc호출 후 바로 return true하는 코드였는데,
						// 예상에는 여기서 GCS_RESULTSTR를 가진 Msg가 디폴트프로시저로 전달이 안되서,
						// 외부IME에게 가지 못했고, 그 결과 캔디데이트에서 선택된 중문글자가 외부IME로부터 와야하는데(wm_char메세지로)
						// 이게 안와서 글자가 안찍혔던 것 같다.
						//
						// 그래서 아래 s_bRESULTSTR_NotSendComp를 사용해 캔디데이트가 떴는지를 확인 후
						// 만약 떳다면, 해당 메세지를 리턴하지 않고, 그냥 흘려보내 디폴트프로시저로 가도록 하겠다.
						// _CH는 EtInterface 단이라 사용하지 못한다.
						if( CEtUIIME::s_bRESULTSTR_NotSendComp == false )
							return true;
						else
						{
							if( (uMsg == WM_IME_COMPOSITION) && ((lParam & GCS_COMPSTR) || (lParam & GCS_RESULTSTR)) ) {
							}
							else return true;
						}
					}
				}
				else
				{
					_ASSERT(0&&"CEtUIDialog::MsgProc함수 안 Modal-iterator 중에 NULL 있습니다!");
				}
			}
		}

		iter = m_listChildDialog.rbegin();
		for( ; iter != m_listChildDialog.rend(); ++iter )
		{
			if( *iter == NULL )
			{
				_ASSERT(0&&"CEtUIDialog::MsgProc함수 안 iterator 중에 NULL 있습니다!");
				continue;
			}

			// 핸들을 직접 넣어주는 이유는,
			// IME를 가진 Child다이얼로그를 Show(true)하는 것과 동시에 IME에 Focus처리를 할 경우,
			// m_hWnd값이 NULL인채로 CEtUIIMEEditBox::Focus가 호출되게 된다.
			// 이때 부모핸들값이 NULL이면서 IME-Enable하는 기능이 제대로 작동하지 않게되어,
			// 열었던 창을 닫고 다시 열어 IME에 Focus를 줘서 입력해야만 했다.
			// 그래서 이렇게 직접 윈도핸들을 넣어주기로 한다.
			//
			// Child_Modal일 경우에도 Child리스트엔 들어있으니 Child_Modal에 대해 따로 처리하지 않아도 된다.
			(*iter)->SetHWnd( hWnd );
			if( (*iter)->MsgProc( hWnd, uMsg, wParam, lParam ) )
			{
				return true;
			}
		}
	}

	if( IsUsableFocusControl() )
	{
		if( focus::MsgProc( uMsg, wParam, lParam ) )
		{
			return true;
		}
	}

	// 일반 버튼을 누른 상태에서 모달창이 열리면(드네 헬프창 같은 경우) 해당 버튼의 Pressed 상태가 계속 유지되버린다.
	// 딱히 처리할만한 적당한 곳이 없어서 우선 여기서 처리하도록 한다.
	// ReleaseControl을 호출하는 것보단, Pressed를 취소한다던지 등의 절차만 하는게 더 나을 거 같아 OnReleaseControl함수를 호출하기로 한다.
	if( m_emDialogType == UI_TYPE_MODAL && focus::IsEnable() && focus::GetParent() != this && IsChildDialog( focus::GetParent() ) == false )
		focus::OnReleaseControl();

	switch( uMsg )
	{
	case WM_SIZE:
	case WM_MOVE:
		{
			POINT pt = { -1, -1 };
			float fMouseX, fMouseY;
			PointToFloat( pt, fMouseX, fMouseY );
			OnMouseMove( fMouseX, fMouseY );
		}
		break;
	case WM_ACTIVATEAPP:
		if( IsUsableFocusControl() )
		{
			if( wParam )
			{
				focus::SetFocus();
			}
			else
			{
				//s_pFocusControl->Focus( false );
			}
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			if( IsUsableFocusControl() )
			{
				if( focus::HandleKeyboard( uMsg, wParam, lParam ) )
				{
					return true;
				}
			}

			if( uMsg == WM_KEYDOWN )
			{
				bool bIsFocusEditBox = false;
				if( focus::IsValid() )
				{
					if( focus::GetType() == UI_CONTROL_EDITBOX ||
						focus::GetType() == UI_CONTROL_IMEEDITBOX ||
						focus::GetType() == UI_CONTROL_LINE_EDITBOX ||
						focus::GetType() == UI_CONTROL_LINE_IMEEDITBOX )
						bIsFocusEditBox = true;
				}

				if( !bIsFocusEditBox )
				{
					for( int i = 0; i < ( int )m_vecControl.size(); i++ )
					{
						if( !m_vecControl[i]->IsShow() || !m_vecControl[i]->IsEnable() )
							continue;

						int nHotKey = m_vecControl[i]->GetHotKey();
						SHORT keyState = GetKeyState( nHotKey );

						// Note : 키를 연속으로 누르는 것을 체크해서 처리되지 않도록 한다.
						//		하지만 다른 두개의 키를 연속으로 누르면 체크되지 않는다. 수정해야 할듯...
						//
						if( keyState&0x80 )
						{

							// 고대 버그가 하나 있었던 거 같은데, 아무리 찾아봐도 잘못된 점을 못찾겠다.
							// 재현과정은 다음과 같다.
							// 에딧박스 가지고 있는 창을 열어 특정키를 입력한다.(예, 파티생성창을 연후 파티이름 입력에다가 p키를 눌러 입력)
							// p키를 2회 누른 후 Esc로 닫고 다시 p를 누르면 캐릭스테이터스창 단축키가 작동하면서 스테이터스창이 열린다.
							// 그런데 3회 누른 후 Esc로 닫고 다시 p를 누르면 바로 안뜨고 한번 더 눌러야 뜬다.
							// 정말 이상한건 이게 p를 누른 횟수가 증가될때마다 번갈아가면서(0,2,4,6..은 정상, 1,3,5는 한번 더 눌러야 열림) 상태가 바뀐다는거다.
							// 아무리 찾아봐도 Editbox에 번갈아가면서 뭔가 처리한게 없는거보면,
							// 아래 핫키처리 부분 중 GetKeyState로 상태 얻어와 계속 누르고 있는 거 막는 처리하는 아래가 가장 의심적은데,
							// if( (m_nHotKey != nHotKey) || (m_HotKeyState != keyState) ) 중 m_HotKeyState != keyState 비교문.
							// 그렇다고 지금와서 다른 걸로 교체하기도 애매한 상황이다..
							//
							// 처음에는 Show-Hide될때 m_HotKeyState를 초기화하는게 기본적으로 있으니, 부모,자식들을 찾아서 다같이 초기화할까 했는데,
							// 단축키로 아예 관계없는 다른 창을 열때가 있어서 이 방법은 통하지 않았다.
							// 그래서.. 진짜 필요한 곳에서 강제로 해제하는 방법을 사용하기로 한다.
							if( (m_nHotKey != nHotKey) || (m_HotKeyState != keyState) )
							{
								m_vecControl[i]->OnHotkey();
								m_HotKeyState = keyState;
								m_nHotKey = nHotKey;
								return true;
							}
						}
					}
				}
			}

			if( uMsg == WM_KEYDOWN )
			{
				if( !focus::IsValid() )
					break;

				switch( wParam )
				{
				case VK_RIGHT:
				case VK_DOWN:	return OnCycleFocus( true );
				case VK_LEFT:
				case VK_UP:		return OnCycleFocus( false );
				case VK_TAB:	return OnCycleFocus( ( ( GetKeyState( VK_SHIFT ) & 0x8000 ) == 0 ) );
				}
			}
		}
		break;
	}

	POINT MousePoint;
	float fMouseX, fMouseY;
	MousePoint.x = short( LOWORD( lParam ) );
	MousePoint.y = short( HIWORD( lParam ) );
	PointToFloat( MousePoint, fMouseX, fMouseY );

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			s_fScreenMouseX = MousePoint.x / GetScreenWidth();
			s_fScreenMouseY = MousePoint.y / GetScreenHeight();
			OnMouseMove( fMouseX, fMouseY );
		}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
		{
			if( (uMsg != WM_MOUSEMOVE) && (uMsg != WM_MOUSEWHEEL) )
			{
				g_pFocusDialog = this;
			}

			if( IsUsableFocusControl() )	// Focus Control 보다 GetControlAtPoint 를 우선시한다.... // (퀘스트 TreeCtrl 에 체크박스를 올리기위함) by realgaia 091215 
			{
				if( focus::HandleMouse( uMsg, fMouseX, fMouseY, wParam, lParam ) )
				{
					return true;
				}
			}

			CEtUIControl* pControl = GetControlAtPoint( fMouseX, fMouseY );
			if (pControl && pControl->IsShow()) //&& pControl->IsEnable() ) // commented by kalliste 090824
			{
				if( pControl->HandleMouse( uMsg, fMouseX, fMouseY, wParam, lParam ) )
				{
					return true;
				}
				else {
					//if( IsUsableFocusControl() )		// Focus Control 을 GetControlAtPoint 다음에 처리한다.... 		by realgaia 091215 		
					//{
					//	if( focus::HandleMouse( uMsg, fMouseX, fMouseY, wParam, lParam ) )
					//	{
					//		return true;
					//	}
					//}
				}
			}
			else
			{
				if( uMsg == WM_LBUTTONDOWN )
				{
					if( focus::IsSameParent(this) )
					{
						focus::ReleaseControl();
					}
				}
			}

			if( IsMouseInDlg() && !GetCapture() )
			{
				// Note : 앞쪽 창에 이벤트 발생시 뒤쪽창으로 메세지 전달을
				//		하지 않는다. 창의 순서가 중요하다.
				//
				return true;
			}
#define MOUSEOVER_CONTROL_OUTSIDE_DLG
#ifdef MOUSEOVER_CONTROL_OUTSIDE_DLG
			else
			{
				// 마우스무브 중에 처리되는 컨트롤이 있다면,
				// (텍스처 컨트롤은 제외하기로 했다. 2의 승수 맞추는 것때문에 다이얼로그 영역 벗어나는게 많기 때문에.
				//  그리고 제대로 하려면, GetControlAtPoint 이 함수로 pControl얻을때부터 텍스처컨트롤 빼고 검사해야하는데,
				//  이렇게까지 하는건 좀 오버인거 같아서 우선은 그냥 아래처럼 처리하기로 하겠다.)
				if( uMsg == WM_MOUSEMOVE && pControl && pControl->GetProperty()->UIType != UI_CONTROL_TEXTURECONTROL )
					return true;
			}
#endif
		}
		break;
	}

	return false;
}

bool CEtUIDialog::OnMouseMove( float fX, float fY )
{
	m_fMouseX = fX;
	m_fMouseY = fY;

	SUICoord dlgCoord(m_DlgInfo.DlgCoord);
	dlgCoord.SetPosition( GetXCoord(), GetYCoord() );

	if( dlgCoord.IsInside( s_fScreenMouseX, s_fScreenMouseY ) )
	{
		m_bMouseInDialog = true;

		// Note : 마우스 포인트에 위치한 컨트롤을 얻는다.
		//
		CEtUIControl *pControl = GetControlAtPoint( m_fMouseX, m_fMouseY );
		if( pControl == NULL )
		{
			ReleaseMouseEnterControl();
			return true;
		}
		else if( pControl == s_pMouseEnterControl )
		{
			return true;
		}

		SetMouseEnterControl( pControl );
		return true;
	}
	else
	{
#ifdef MOUSEOVER_CONTROL_OUTSIDE_DLG
		m_bMouseInDialog = false;
		CEtUIControl *pControl = GetControlAtPoint( m_fMouseX, m_fMouseY );
		if( pControl == NULL )
		{
			ReleaseMouseEnterControl();
			return false;
		}
		else if( pControl == s_pMouseEnterControl )
		{
			return true;
		}

		SetMouseEnterControl( pControl );
		return true;
#else
		m_bMouseInDialog = false;
		ReleaseMouseEnterControl();
		return false;
#endif
	}

	return false;
}

void CEtUIDialog::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	if( ( nCommand == EVENT_EDITBOX_STRING ) && m_pDefaultControl )
	{
		if( m_pDefaultControl->IsShow() && m_pDefaultControl->IsEnable() )
			m_pDefaultControl->OnHotkey();
	}

	if( nCommand == EVENT_EDITBOX_ESCAPE ) {
		for( int i = 0; i < ( int )m_vecControl.size(); i++ ) {
			if( !m_vecControl[i]->IsShow() || !m_vecControl[i]->IsEnable() )
				continue;
			if( m_vecControl[i]->GetHotKey() == VK_ESCAPE ) {
				m_vecControl[i]->OnHotkey();
				break;
			}
		}
	}

	if( pControl->IsShow() && pControl->IsEnable() )
	{
		if( m_pCallback )
		{
			// 로딩시 브레이크시 여기서 크래쉬..m_pCallBack 이미 지워짐.
			m_pCallback->OnUICallbackProc( m_nDialogID, nCommand, pControl, uMsg );
		}
	}
}

void CEtUIDialog::Process( float fElapsedTime )
{
	CEtUIControl *pControl(NULL);
	int nVecCtlSize = ( int )m_vecControl.size();
	for( int i = 0; i < nVecCtlSize; i++ )
	{
		pControl = m_vecControl[i];
		if (pControl)
			pControl->Process( fElapsedTime );
	}

	if( m_FadeMode != FadeModeEnum::None )
	{
		if( !IsShow() )	
			FadeOut( fElapsedTime );
		else			
			FadeIn( fElapsedTime );
	}

	std::list<CEtUIDialog*>::iterator iter = m_listChildDialog.begin();
	for( ; iter != m_listChildDialog.end(); ++iter )
	{
		if( *iter == NULL )
		{
			_ASSERT(0&&"CEtUIDialog::Process함수 안 iterator 중에 NULL 있습니다!");
			continue;
		}

		(*iter)->Process( fElapsedTime );
	}	

	ProcessMemoryOptimize( fElapsedTime );
	RefreshChildRenderPriority();
}

#ifndef _FINAL_BUILD
extern bool g_bPauseMode;
#endif

void CEtUIDialog::ProcessMemoryOptimize( float fElapsedTime )
{
#ifndef _FINAL_BUILD
	if( g_bPauseMode ) return;
#endif

	bool bDraging = false;
	if( drag::IsValid() && drag::GetControl()->GetParent() == this ) {
		bDraging = true;
	}
	if( ( IsShow() ) || bDraging || IsAllowRender() ) {
		if( m_fLastShowDelta < 10.f ) m_fLastShowDelta = 10.f;
		m_fLastShowDelta += fElapsedTime;
		if( m_fLastShowDelta > 30.f ) m_fLastShowDelta = 30.f;
	}
	else m_fLastShowDelta -= fElapsedTime;

	if( m_fLastShowDelta <= 0.f ) {
		FreeDialogTexture();
	}
}

void CEtUIDialog::FreeDialogTexture()
{
	if( !m_bLoadedTexture ) return;
	if( m_hDlgTexture && m_hDlgTexture->GetRefCount() == 1 )
		s_nDialogTextureSize -= m_hDlgTexture->GetFileSize();
	SAFE_RELEASE_SPTR( m_hDlgTexture );
	if( m_hTexture && m_hTexture->GetRefCount() == 1 )
		s_nDialogTextureSize -= m_hTexture->GetFileSize();
	SAFE_RELEASE_SPTR( m_hTexture );
	CEtResource::FlushWaitDelete( RT_TEXTURE );
	m_bLoadedTexture = false;

	std::list<CEtUIDialog*>::iterator iter = m_listChildDialog.begin();
	for( ; iter != m_listChildDialog.end(); ++iter )
	{
		(*iter)->FreeDialogTexture();
	}

	CEtUIControl *pControl(NULL);
	int nVecCtlSize = ( int )m_vecControl.size();
	for( int i = 0; i < nVecCtlSize; i++ )
	{
		pControl = m_vecControl[i];
		SAFE_RELEASE_SPTR( pControl->GetTemplate().m_hTemplateTexture );
	}
}

void CEtUIDialog::LoadDialogTexture()
{
	if( m_bLoadedTexture ) return;
	// 템플릿 텍스처 쓰면서 이제 필요없다. 코드는 다 고칠 필요없이 이거 한줄 주석처리하면 끝.
	// 소스 정리는 안정화된 후에 해야겠다.
	//m_hTexture = LoadResource( m_DlgInfo.szUITexturename, RT_TEXTURE, true );
	if( m_hTexture && m_hTexture->GetRefCount() == 1 )
		s_nDialogTextureSize += m_hTexture->GetFileSize();
	m_hDlgTexture = LoadResource( m_DlgInfo.szDlgTextureName, RT_TEXTURE, true );
	if( m_hDlgTexture && m_hDlgTexture->GetRefCount() == 1 )
		s_nDialogTextureSize += m_hDlgTexture->GetFileSize();
	m_bLoadedTexture = true;

	std::list<CEtUIDialog*>::iterator iter = m_listChildDialog.begin();
	for( ; iter != m_listChildDialog.end(); ++iter )
	{
		(*iter)->LoadDialogTexture();
	}

	CEtUIControl *pControl(NULL);
	int nVecCtlSize = ( int )m_vecControl.size();
	for( int i = 0; i < nVecCtlSize; i++ )
	{
		pControl = m_vecControl[i];
		pControl->GetTemplate().OnLoaded();
	}
}

bool CEtUIDialog::IsAllowRender()
{
	if( !IsShow() ) {
		switch( m_FadeMode ) {
			case FadeModeEnum::None: return false;
			case FadeModeEnum::CancelRender:
				if( ( m_renderDlgColor.dwCurrentColor >> 24 ) == 0 ) return false;
				break;
			case FadeModeEnum::AllowRender: return true;
		}
	}
	return true;
}

void CEtUIDialog::RefreshChildRenderPriority()
{
	for( DWORD i=0; i<m_vecChildRenderPriorityInfo.size(); i++ )
	{
		CEtUIDialog* pDlg = m_vecChildRenderPriorityInfo[i].first;
		bool bTail = m_vecChildRenderPriorityInfo[i].second;

		if( !pDlg )
			continue;

		std::list<CEtUIDialog*>::iterator iter = std::find( m_listChildDialog.begin(), m_listChildDialog.end(), pDlg );
		if ( iter != m_listChildDialog.end() )
		{
			CEtUIDialog* pDlg = *iter;

			if (bTail)
				m_listChildDialog.push_back(pDlg);
			else
				m_listChildDialog.push_front(pDlg);
			m_listChildDialog.erase(iter);
		}
	}

	m_vecChildRenderPriorityInfo.clear();
}

void CEtUIDialog::SetChildRenderPriority(CEtUIDialog* pDlg, bool bTail)
{
	if( !pDlg )
		return;

	m_vecChildRenderPriorityInfo.push_back( std::make_pair( pDlg, bTail ) );
}

void CEtUIDialog::Render( float fElapsedTime )	
{
	if( !IsAllowRender() ) return;

	SUICoord DlgCoord(m_DlgInfo.DlgCoord), DlgUVCoord(0.0f, 0.0f, 1.0f, 1.0f);
	DlgCoord.fX = 0.0f;
	DlgCoord.fY = 0.0f;

	if( m_hDlgTexture )
	{
		if( (m_DlgInfo.nFrameLeft > 0) || (m_DlgInfo.nFrameRight > 0) || (m_DlgInfo.nFrameTop > 0) || (m_DlgInfo.nFrameBottom > 0) )
		{
			DrawDlgFrame();
		}
		else
		{
			DrawSprite( m_hDlgTexture, DlgUVCoord, m_renderDlgColor.dwCurrentColor, DlgCoord );
		}
	}
	else if( (m_renderDlgColor.dwCurrentColor & 0xFF000000) != 0 )
	{
		DrawRect( DlgCoord, m_renderDlgColor.dwCurrentColor );
	}

	// Note : 디버깅 할때 쓰세요^^
	//
	//  DrawRect( DlgCoord, EtInterface::debug::BLUE );

	// Note : 자신의 컨트롤들을 랜더한다.
	//
	CEtUIControl *pControl(NULL);
	int nVecCtlSize = ( int )m_vecControl.size();
	for( int i = 0; i < nVecCtlSize; i++ )
	{
		pControl = m_vecControl[i];
		if( !pControl ) continue;
		pControl->Render( fElapsedTime );
	}

	// 자식 윈도우들을 랜더한다.
	//
	bool bRenderMostFocusDlg = false;
	std::list<CEtUIDialog*>::iterator iter = m_listChildDialog.begin();
	for( ; iter != m_listChildDialog.end(); ++iter )
	{
		if( *iter == NULL )
		{
			//_ASSERT(0&&"CEtUIDialog::Render함수 안 iterator 중에 NULL 있습니다!");
			continue;
		}
		(*iter)->Render( fElapsedTime );
	}
}

void CEtUIDialog::DrawDlgFrame()
{
	int nFrameSize = ( int )m_vecDlgFrameElement.size();

	for( int i = 0; i < nFrameSize; i++ )
	{
		DrawSprite( m_hDlgTexture, m_vecDlgFrameElement[ i ].UVCoord, m_renderDlgColor.dwCurrentColor, m_vecDlgFrameCoord[ i ] );
	}
}

void CEtUIDialog::CalcTextRect( LPCWSTR szText, SUIElement *pElement, SUICoord &CalcCoord, int nCount, DWORD dwFontFormat )
{
	ASSERT( pElement&&"CEtUIDialog::CalcTextRect, pElement is NULL!" );
	ASSERT( pElement->nFontIndex>=0&&"CEtUIDialog::CalcTextRect" );

	if( dwFontFormat == 0xffffffff )
	{
		dwFontFormat = pElement->dwFontFormat;
	}
	dwFontFormat |= DT_WORDBREAK;

	CEtFontMng::GetInstance().CalcTextRect( pElement->nFontIndex, pElement->nFontHeight, szText, dwFontFormat, CalcCoord, nCount );
	CalcCoord.fWidth *= GetScreenWidthRatio();
	CalcCoord.fHeight *= GetScreenHeightRatio();
}

void CEtUIDialog::CalcTextRect( LPCWSTR szText, int fontIndex, int fontHeight, SUICoord &CalcCoord, DWORD dwFontFormat, bool bUseCache )
{
	//ASSERT( pElement&&"CEtUIDialog::CalcTextRect, pElement is NULL!" );
	ASSERT( fontHeight>=0&&"CEtUIDialog::CalcTextRect" );

	dwFontFormat |= DT_WORDBREAK;

	CEtFontMng::GetInstance().CalcTextRect( fontIndex, fontHeight, szText, dwFontFormat, CalcCoord, -1, bUseCache );
	CalcCoord.fWidth *= GetScreenWidthRatio();
	CalcCoord.fHeight *= GetScreenHeightRatio();
}

void CEtUIDialog::DrawDlgText( LPCWSTR szText, SUIElement *pElement, DWORD dwFontColor, const SUICoord &Coord, int nCount, DWORD dwFontFormat, bool bClip, float ZValue, DWORD dwBgColor, int nBorderFlag )
{
	ASSERT( pElement&&"CEtUIDialog::DrawDlgText, pElement is NULL!" );
	ASSERT( pElement->nFontIndex>=0&&"CEtUIDialog::DrawDlgText" );

	if( !szText  || szText[0] == '\0' ) 
		return;

	SUICoord ScreenCoord;
	//blondy
	if( m_DlgInfo.bLockScalingByResolution )
	{
		ScreenCoord.fX = Coord.fX + GetXCoord() ;
		ScreenCoord.fY = Coord.fY + GetYCoord() ;
		ScreenCoord.fWidth = Coord.fWidth ;
		ScreenCoord.fHeight = Coord.fHeight ;

	}else
	{
		ScreenCoord.fX = ( Coord.fX + GetXCoord() ) / GetScreenWidthRatio();
		ScreenCoord.fY = ( Coord.fY + GetYCoord() ) / GetScreenHeightRatio();
		ScreenCoord.fWidth = Coord.fWidth / GetScreenWidthRatio();
		ScreenCoord.fHeight = Coord.fHeight / GetScreenHeightRatio();
	}

	if( dwFontFormat == 0xffffffff )
	{
		dwFontFormat = pElement->dwFontFormat;
	}

	if( !bClip )
	{
		dwFontFormat |= DT_WORDBREAK;
	}

	//if( pElement->bShadowFont )
	//{
	//	SUICoord ShadowCoord(ScreenCoord);
	//	ShadowCoord.fX += 1.0f / GetEtDevice()->Width();
	//	ShadowCoord.fY += 1.0f / GetEtDevice()->Height();
	//	CEtFontMng::GetInstance().DrawTextW( pElement->nFontIndex, pElement->nFontHeight, szText, pElement->ShadowFontColor.dwCurrentColor, dwFontFormat, ShadowCoord, nCount );
	//}
	D3DXCOLOR textureColor( dwFontColor );
	D3DXCOLOR shadowColor( pElement->ShadowFontColor.dwCurrentColor );

	if( textureColor.a != 1.0f )
		shadowColor.a = shadowColor.a * textureColor.a;

	SFontDrawEffectInfo Info;
	Info.nDrawType = pElement->nDrawType;
	Info.nWeight = pElement->nWeight;
	Info.fAlphaWeight = pElement->fAlphaWeight;
	Info.fGlobalBlurAlphaWeight = pElement->fGlobalBlurAlphaWeight;
	Info.dwFontColor = dwFontColor;
	Info.dwEffectColor = shadowColor; //pElement->ShadowFontColor.dwColor[UI_STATE_NORMAL];
	CEtFontMng::GetInstance().DrawTextW( pElement->nFontIndex, pElement->nFontHeight, szText, dwFontFormat, ScreenCoord, nCount, Info, true, ZValue , dwBgColor, nBorderFlag );
}

void CEtUIDialog::DrawSprite( EtTextureHandle hTexture, SUICoord &UVCoord, DWORD dwUIColor, SUICoord &Coord, float fRotate, float ZValue )
{
	if( UVCoord.fWidth == 0.0f )
		return;

	if( ( ( dwUIColor & 0xff000000 ) == 0 ) || ( !hTexture ) )
		return;

	if( !hTexture || !hTexture->IsReady() )
		return;

	SUICoord ScreenCoord;
	ScreenCoord.SetPosition( ( Coord.fX + GetXCoord() ) / GetScreenWidthRatio(), ( Coord.fY + GetYCoord() ) / GetScreenHeightRatio() );
	ScreenCoord.SetSize( Coord.fWidth / GetScreenWidthRatio(), Coord.fHeight / GetScreenHeightRatio() );

	CEtSprite::GetInstance().DrawSprite( ( EtTexture * )hTexture->GetTexturePtr(), hTexture->Width(), hTexture->Height(), UVCoord, dwUIColor, ScreenCoord, fRotate, ZValue );
}

void CEtUIDialog::DrawSprite( SUICoord &UVCoord, DWORD dwUIColor, SUICoord &Coord, float fRotate, float ZValue )
{
	DrawSprite( m_hTexture, UVCoord, dwUIColor, Coord, fRotate, ZValue );
}

void CEtUIDialog::DrawRect( SUICoord &Coord, DWORD dwColor )
{
	if( ( Coord.fWidth <= 0.0f ) && ( Coord.fHeight <= 0.0f ) )
	{
		return;
	}

	SUICoord ScreenCoord = DlgCoordToScreenCoord( Coord );

	CEtSprite::GetInstance().DrawRect( ScreenCoord, dwColor );
}

SUICoord CEtUIDialog::DlgCoordToScreenCoord( const SUICoord &Coord )
{
	SUICoord ResultCoord;
	ResultCoord.fX = ( Coord.fX + GetXCoord() ) / GetScreenWidthRatio();
	ResultCoord.fY = ( Coord.fY + GetYCoord() ) / GetScreenHeightRatio();
	ResultCoord.fWidth = Coord.fWidth / GetScreenWidthRatio();
	ResultCoord.fHeight = Coord.fHeight / GetScreenHeightRatio();
	return ResultCoord;
}

SUICoord CEtUIDialog::ScreenCoordToDlgCoord( SUICoord &Coord )
{
	SUICoord ResultCoord;
	ResultCoord.fX = ( Coord.fX * GetScreenWidthRatio() ) - GetXCoord();
	ResultCoord.fY = ( Coord.fY * GetScreenHeightRatio() ) - GetYCoord();
	ResultCoord.fWidth = Coord.fWidth * GetScreenWidthRatio();
	ResultCoord.fHeight = Coord.fHeight * GetScreenHeightRatio();
	return ResultCoord;
}

void CEtUIDialog::UpdateDlgCoord( float fX, float fY, float fWidth, float fHeight )
{
	m_DlgInfo.DlgCoord.fX += fX;
	m_DlgInfo.DlgCoord.fY += fY;
	m_DlgInfo.DlgCoord.fWidth += fWidth;
	m_DlgInfo.DlgCoord.fHeight += fHeight;
	
	UpdateRects();
}

bool CEtUIDialog::IsCmdControl( const char *szCmdCtlName )
{
	ASSERT(szCmdCtlName);
	return (m_strCmdControlName == szCmdCtlName);
}

CEtUITemplate *CEtUIDialog::GetTemplate( int nIndex ) 
{
	if( nIndex >= (int)m_vecUITemplate.size() )
	{
		CDebugSet::ToLogFile("CEtUIDialog::GetTemplate, nIndex >= (int)m_vecUITemplate.size()");
		nIndex = (int)(m_vecUITemplate.size()-1);
	}

	return m_vecUITemplate[ nIndex ];
}

void CEtUIDialog::FadeIn( float fElapsedTime )
{
	m_renderDlgColor.BlendEx( UI_STATE_NORMAL, fElapsedTime, 0.2f );
}

void CEtUIDialog::FadeOut( float fElapsedTime )
{
	m_renderDlgColor.BlendEx( UI_STATE_HIDDEN, fElapsedTime, 0.2f );
}

void CEtUIDialog::GetMouseMovePoints( float &fMouseX, float &fMouseY ) 
{ 
	fMouseX = m_fMouseX;
	fMouseY = m_fMouseY;
}

void CEtUIDialog::ProcessChangeResolution()
{
	for each( CEtUIDialog *pDialog in s_plistDialogBottomMsg ) pDialog->OnChangeResolution();	
	for each( CEtUIDialog *pDialog in s_plistDialogBottom ) pDialog->OnChangeResolution();	
	for each( CEtUIDialog *pDialog in s_plistDialogTopMsg ) pDialog->OnChangeResolution();	
	for each( CEtUIDialog *pDialog in s_plistDialogTop ) pDialog->OnChangeResolution();	
	for each( CEtUIDialog *pDialog in s_plistDialogFocus ) pDialog->OnChangeResolution();	
	for each( CEtUIDialog *pDialog in s_plistDialogModal ) pDialog->OnChangeResolution();	
	for each( CEtUIDialog *pDialog in s_plistDialogMostTop ) pDialog->OnChangeResolution();	

	// Self라도 해주는게 맞는거 같다.(Self로 만든 말풍선다이얼로그하면서 해주는게 맞는 듯..)
	for each( CEtUIDialog *pDialog in s_plistDialogSelf ) pDialog->OnChangeResolution();
}

void CEtUIDialog::SetForceDialogScale( float fScale )
{
	s_fDialogScale = fScale;
	if( CEtFontMng::IsActive() ) {
		CEtFontMng::GetInstance().OnLostDevice();
		CEtFontMng::GetInstance().OnResetDevice();
	}
	ProcessChangeResolution();
}

void CEtUIDialog::OnChangeResolution()
{
	UpdateScreen();
	UpdateRects();

	CEtUIControl *pControl(NULL);
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
	{
		pControl = m_vecControl[i];
		if( !pControl ) continue;
		pControl->OnChangeResolution();
	}
	
	for each( CEtUIDialog *pDialog in m_listChildDialog )
	{
		pDialog->OnChangeResolution();
	}

	for each( CEtUIDialog *pDialog in m_listChildModalDialog )
	{
		pDialog->OnChangeResolution();
	}
}

void CEtUIDialog::SetDlgInfo( SUIDialogInfo &DlgInfo )
{ 
	m_DlgInfo = DlgInfo; 
	m_renderDlgColor.dwColor[UI_STATE_NORMAL] = m_DlgInfo.dwDlgColor;
	m_renderDlgColor.dwCurrentColor = m_DlgInfo.dwDlgColor;
	UpdateRects();
}

void CEtUIDialog::GetDlgInfo( SUIDialogInfo &DlgInfo )
{ 
	DlgInfo = m_DlgInfo; 
}

void CEtUIDialog::ShowTooltipDlg( CEtUIControl *pControl, bool bShow, int nStringIndex, DWORD color, bool bPermanent )
{
	if( bShow )
	{
		s_nTooltipStringIndex = nStringIndex;
	}

	if( s_pTooltipDlg )
	{
		s_pTooltipDlg->SetTooltipControl( pControl );
		s_pTooltipDlg->SetTooltipInfo(color, bPermanent);
		s_pTooltipDlg->Show( bShow );
	}
}

void CEtUIDialog::ShowTooltipDlg( CEtUIControl *pControl, bool bShow, const std::wstring &strTooltip, DWORD color, bool bPermanent )
{
	if( bShow )
	{
		s_strTooltipString = strTooltip;
	}

	if( s_pTooltipDlg )
	{
		s_pTooltipDlg->SetTooltipControl( pControl );
		s_pTooltipDlg->SetTooltipInfo(color, bPermanent);
		s_pTooltipDlg->Show( bShow );
	}
}

bool CEtUIDialog::IsTooltipControl(CEtUIControl* pCtrl)
{
	CEtUIControl* ctrl = s_pTooltipDlg->GetTooltipCtrl();
	if (s_pTooltipDlg && ctrl)
		return (pCtrl == ctrl);

	return false;
}

void CEtUIDialog::GetScreenMouseMovePoints( float &fMouseX, float &fMouseY )
{
	fMouseX = s_fScreenMouseX;
	fMouseY = s_fScreenMouseY;
}

void CEtUIDialog::UpdateScreen()
{
	// 이런 방법으로 처리할까 했는데, 크게 두가지가 걸린다.
	// 하나는 폰트 처리고, 하나는 스크롤바같이 서브형태로 가지는 것이다.
	// 이 두가지에 대한 처리가 끝나야 제대로 사용할 수 있을 듯 하다.
	// from blondy 그냥 그런 것들에 대한 처리를 케이스 바이 케이스로 해야 될듯
	
	if( m_DlgInfo.bLockScalingByResolution )
	{
		m_fScreenHeight = DEFAULT_UI_SCREEN_HEIGHT;
		m_fScreenWidth = DEFAULT_UI_SCREEN_WIDTH;
	}
	else
	{
		m_fScreenHeight = DEFAULT_UI_SCREEN_HEIGHT * s_fDialogScale;
		m_fScreenWidth = DEFAULT_UI_SCREEN_WIDTH * s_fDialogScale;
	}

	m_fScreenHeight = DEFAULT_UI_SCREEN_HEIGHT * s_fDialogScale;
	m_fScreenHeightRatio = GetEtDevice()->Height() / m_fScreenHeight;
	m_fScreenHeightBorder = (m_fScreenHeightRatio - 1.0f) * 0.5f;

	m_fScreenWidth = DEFAULT_UI_SCREEN_WIDTH * s_fDialogScale;
	m_fScreenWidthRatio = GetEtDevice()->Width() / m_fScreenWidth;
	m_fScreenWidthBorder = ( m_fScreenWidthRatio - 1.0f ) * 0.5f;

	//if( (fWidth/fHeight) >= DEFAULT_UI_SCREEN_RATIO )
	//{
	//	// Note : 화면 비율이 기본 4:3 비율보다 크다면 와이드로 적용한다.
	//	//
	//	m_fScreenWidth = fHeight * DEFAULT_UI_SCREEN_RATIO * s_fDialogScale;
	//}
	//else
	//{
	//	m_fScreenWidth = DEFAULT_UI_SCREEN_WIDTH * s_fDialogScale;
	//}
}

void CEtUIDialog::CalcDialogScaleByResolution( int nWidth, int nHeight )
{
	float fHR = float(nHeight) / DEFAULT_UI_SCREEN_HEIGHT;
	float fWR = float(nWidth) / DEFAULT_UI_SCREEN_WIDTH;

	s_fDialogScaleValueVert = ( fHR - 1.0f ) * UI_DIALOG_SCALE_VALUE;
	if( s_fDialogScaleValueVert < 0.0f )
	{
		s_fDialogScaleValueVert = 0.0f;
	}
	s_fDialogScaleValueHori = ( fWR - 1.0f ) * UI_DIALOG_SCALE_VALUE;
	if( s_fDialogScaleValueHori < 0.0f )
	{
		s_fDialogScaleValueHori = 0.0f;
	}

	// 해상도 바뀌면 UISize 다시 처리해야한다.
	SetDialogSize( s_nUISize, nWidth, nHeight );
}

void CEtUIDialog::AddChildDialog( CEtUIDialog *pDialog )
{
	ASSERT(pDialog&&"CEtUIDialog::AddChildDialog");
	m_listChildDialog.push_back( pDialog );
}

void CEtUIDialog::AddChildModalDialog( CEtUIDialog *pDialog )
{
	ASSERT(pDialog&&"CEtUIDialog::AddChildModalDialog");
	m_listChildModalDialog.push_back( pDialog );
}

void CEtUIDialog::DelChildDialog( CEtUIDialog *pDialog )
{
	ASSERT(pDialog&&"CEtUIDialog::DelChildDialog");

	std::list<CEtUIDialog*>::iterator iter = m_listChildDialog.begin();
	for( ; iter != m_listChildDialog.end(); )
	{
		if( (*iter) == pDialog )
		{
			iter = m_listChildDialog.erase( iter );
			continue;
		}
		++iter;
	}
}

void CEtUIDialog::DelChildModalDialog( CEtUIDialog *pDialog )
{
	ASSERT(pDialog&&"CEtUIDialog::DelChildModalDialog");

	std::list<CEtUIDialog*>::iterator iter = m_listChildModalDialog.begin();
	for( ; iter != m_listChildModalDialog.end(); )
	{
		if( (*iter) == pDialog )
		{
			iter = m_listChildModalDialog.erase( iter );
			continue;
		}
		++iter;
	}
}

void CEtUIDialog::ShowChildDialog( CEtUIDialog *pDialog, bool bShow )
{
	if( !pDialog ) 
		return;

	ASSERT( (pDialog!=this)&&"CEtUIDialog::ShowChildDialog" );

	if( pDialog->GetDialogType() == UI_TYPE_CHILD_MODAL )
	{
		ShowChildDialog( m_listChildModalDialog, pDialog, bShow );
	}
	else
	{
		ShowChildDialog( m_listChildDialog, pDialog, bShow );
	}
}

void CEtUIDialog::ShowChildDialog( std::list<CEtUIDialog*> &listDialog, CEtUIDialog *pDialog, bool bShow )
{
	if( !pDialog )				return;
	if( listDialog.empty() )	return;

	CEtUIDialog *pChildDialog(NULL);

	std::list<CEtUIDialog*>::iterator iter = listDialog.begin();
	for( ; iter != listDialog.end(); ++iter )
	{
		if( (*iter) == pDialog )
		{
			pChildDialog = (*iter);
			listDialog.erase( iter );
			break; // Note : 자식 윈도우의 아이디는 유일하다고 가정한다.
		}
	}

	// Note : 자식 윈도우들은 Show할때 리스트에서 순서를 바꿔준다.
	//		결국 랜더와 메세지 처리 순서가 바뀐다.
	//
	if( pChildDialog )
	{
		// content 다이얼로그가 스킬트리처럼 겹쳐있으면 입력 메시지가 작은 다이얼로그에 오지 않기 때문에 
		// CEtUIDialog::ShowChildDialog() 함수 호출 시에 체크해서 포커싱 처리시에 가장 뒤에 오도록 셋팅합니다.
		vector<CEtUIDialog*> vlpContentDialogs;
		pChildDialog->GetContentDialog( vlpContentDialogs );
		if( false == vlpContentDialogs.empty() )
		{
			for( int i = 0; i < (int)vlpContentDialogs.size(); ++i )
			{
				CEtUIDialog* pContentDialog = vlpContentDialogs.at( i );
				list<CEtUIDialog*>::iterator iterContentDlg = find( listDialog.begin(), listDialog.end(), pContentDialog );
				_ASSERT( listDialog.end() != iterContentDlg );
				if( listDialog.end() != iterContentDlg )
					listDialog.erase( iterContentDlg );
			}
		}

		// 메시지 전달은 거꾸로 되므로 포커싱 되는 경우엔 맨 뒤로,
		// 숨겨지는 경우엔 맨 앞으로 옮겨지는 메인 다이얼로그의 바로 뒤로.
		if( bShow )
		{
			listDialog.push_back( pChildDialog );
			if( false == vlpContentDialogs.empty() )
			{
				for( int i = 0; i < (int)vlpContentDialogs.size(); ++i )
				{
					CEtUIDialog* pContentDialog = vlpContentDialogs.at( i );
					listDialog.push_back( pContentDialog );
				}
			}
		}
		else
		{
			if( false == vlpContentDialogs.empty() )
			{
				for( int i = 0; i < (int)vlpContentDialogs.size(); ++i )
				{
					CEtUIDialog* pContentDialog = vlpContentDialogs.at( i );
					listDialog.push_front( pContentDialog );
				}
			}
			listDialog.push_front( pChildDialog );
		}

		pChildDialog->Show( bShow );
	}
	else
	{
		CDebugSet::ToLogFile( "CEtUIDialog::ShowChildDialog, pChildDialog is NULL!" );
	}
}

bool CEtUIDialog::IsChildDialog( const CEtUIDialog *pDialog )
{
	if( !pDialog )
		return false;

	std::list<CEtUIDialog*>::iterator iter = m_listChildDialog.begin();
	for( ; iter != m_listChildDialog.end(); ++iter )
	{
		if( (*iter) == pDialog )
		{
			return true;
		}

		if( (*iter)->IsChildDialog( pDialog ) )
		{
			return true;
		}
	}

	iter = m_listChildModalDialog.begin();
	for( ; iter != m_listChildModalDialog.end(); ++iter )
	{
		if( (*iter) == pDialog )
		{
			return true;
		}

		if( (*iter)->IsChildDialog( pDialog ) )
		{
			return true;
		}
	}

	return false;
}

void CEtUIDialog::UpDialogSize()
{
	CalcDialogScaleValue();

	if( s_fDialogScaleValueVert <= 0.0f )			return;
	if( s_fDialogScale == s_fMaxDialogScale )	return;

	s_fDialogScale += s_fDialogScaleValueVert;
	if( s_fDialogScale > s_fMaxDialogScale )
	{
		s_fDialogScale = s_fMaxDialogScale;
	}

	if( CEtFontMng::IsActive() ) {
		CEtFontMng::GetInstance().OnLostDevice();
		CEtFontMng::GetInstance().OnResetDevice();
	}
	ProcessChangeResolution();
}

void CEtUIDialog::DownDialogSize()
{
	CalcDialogScaleValue();

	if( s_fDialogScaleValueVert <= 0.0f )			return;
	if( s_fDialogScale == s_fMinDialogScale )	return;

	s_fDialogScale -= s_fDialogScaleValueVert;
	if( s_fDialogScale < s_fMinDialogScale )
	{
		s_fDialogScale = s_fMinDialogScale;
	}

	if( CEtFontMng::IsActive() ) {
		CEtFontMng::GetInstance().OnLostDevice();
		CEtFontMng::GetInstance().OnResetDevice();
	}
	ProcessChangeResolution();
}

void CEtUIDialog::CalcDialogScaleValue( int nWidth, int nHeight )
{
	// 디폴트해상도보다 작은 해상도 자꾸 지원하려한다.
	// 그래서 이렇게 디폴트해상도보다 작은 해상도 오면 별도의 UISize조절이 불가능하도록
	// 계산된 기본 스케일 그대로 사용한다.
	if( nWidth < DEFAULT_UI_SCREEN_WIDTH || nHeight < DEFAULT_UI_SCREEN_HEIGHT )
	{
		s_fMaxDialogScale = s_fDialogScale;
		s_fMinDialogScale = s_fDialogScale;
		return;
	}

	if( nWidth == 0 && nHeight == 0 )
	{
		nWidth = GetEtDevice()->Width();
		nHeight = GetEtDevice()->Height();
	}

	float fHR = float(nHeight) / DEFAULT_UI_SCREEN_HEIGHT;
	float fWR = float(nWidth) / DEFAULT_UI_SCREEN_WIDTH;

	s_fMaxDialogScale = min( fHR, fWR );
	s_fMinDialogScale = 2.0f - s_fMaxDialogScale;
	s_fDialogScaleValueVert = (s_fMaxDialogScale - 1.0f) / 2.0f;

	// 너무 작아지게 하지 말자.
	s_fMinDialogScale = (s_fMinDialogScale + 3.0f) / 4.0f;
}

// 윈도우의 최대화버튼때문에 해상도를 인자로 필요로 한다.
void CEtUIDialog::SetDialogSize( int nStep, int nWidth, int nHeight )
{
	if( nStep < 1 || nStep > 4 )
		return;

	if( nWidth == 0 && nHeight == 0 )
	{
		nWidth = GetEtDevice()->Width();
		nHeight = GetEtDevice()->Height();
	}

	// 다이얼로그 사이즈 계산하는게 하도 흩어져있어서 이렇게 일일이 다 처리해야한다.
	// 담에 한번 싹 정리를 해야하던지..해야겠다.
	static int s_nWidthInFunc = -1;
	static int s_nHeightInFunc = -1;
	if( s_nWidthInFunc == nWidth && s_nHeightInFunc == nHeight && s_nUISize == nStep )
		return;
	s_nWidthInFunc = nWidth;
	s_nHeightInFunc = nHeight;

	// 기준값 재계산.
	float fHR = float(nHeight) / DEFAULT_UI_SCREEN_HEIGHT;
	float fWR = float(nWidth) / DEFAULT_UI_SCREEN_WIDTH;

	s_fDialogScaleValueVert = ( fHR - 1.0f ) * UI_DIALOG_SCALE_VALUE;
	if( s_fDialogScaleValueVert < 0.0f ) s_fDialogScaleValueVert = 0.0f;
	s_fDialogScaleValueHori = ( fWR - 1.0f ) * UI_DIALOG_SCALE_VALUE;
	if( s_fDialogScaleValueHori < 0.0f ) s_fDialogScaleValueHori = 0.0f;

	if( fHR < fWR )
	{
		s_fDialogScale = fHR;
		s_fDialogScale -= s_fDialogScaleValueVert;
	}
	else
	{
		s_fDialogScale = fWR;
		s_fDialogScale -= s_fDialogScaleValueHori;
	}

	CalcDialogScaleValue( nWidth, nHeight );

	if( nStep == 1 )
	{
		s_fDialogScale += s_fDialogScaleValueVert;
		if( s_fDialogScale > s_fMaxDialogScale )
		{
			s_fDialogScale = s_fMaxDialogScale;
		}
	}
	else if( nStep == 2 )
	{
	}
	else if( nStep == 3 )
	{
		s_fDialogScale -= s_fDialogScaleValueVert;
		if( s_fDialogScale < s_fMinDialogScale )
		{
			s_fDialogScale = s_fMinDialogScale;
		}
	}
	else if( nStep == 4 )
	{
		s_fDialogScale -= s_fDialogScaleValueVert;
		s_fDialogScale -= s_fDialogScaleValueVert;
		if( s_fDialogScale < s_fMinDialogScale )
		{
			s_fDialogScale = s_fMinDialogScale;
		}
	}

	// 폰트 크기로부터 영역을 구하는 컨트롤들이 있기때문에 폰트 매니저 먼저 리셋하고 ProcessChangeResolution를 호출해야한다.
	if( CEtFontMng::IsActive() ) {
		CEtFontMng::GetInstance().OnLostDevice();
		CEtFontMng::GetInstance().OnResetDevice();
	}
	ProcessChangeResolution();

	s_nUISize = nStep;
}

void CEtUIDialog::ReleaseMouseEnterControl()
{
	if( s_pMouseEnterControl )
	{
		s_pMouseEnterControl->MouseEnter( false );
		s_pMouseEnterControl = NULL;

		ShowTooltipDlg( NULL, false );
	}
}

void CEtUIDialog::SetMouseEnterControl( CEtUIControl *pControl )
{
	ASSERT( pControl&&"CEtUIDialog::SetMouseEnterControl" );

	ReleaseMouseEnterControl();

	// Note : 새로운 컨트롤을 등록하고 툴팁을 표시한다.
	//
	s_pMouseEnterControl = pControl;
	s_pMouseEnterControl->MouseEnter( true );

	if( drag::IsValid() )
	{
		// Note : 드래그 되는 아이템이 있으면 툴팁은 표시하지 않는다.
		//
		return;
	}

	SUIControlProperty sProperty;
	s_pMouseEnterControl->GetProperty( sProperty );

	if( pControl->IsShow() )
	{
		if( sProperty.nTooltipStringIndex > 0 )
		{
			ShowTooltipDlg( pControl, true, sProperty.nTooltipStringIndex );
		}
		else
		{
			std::wstring strTooltipText = s_pMouseEnterControl->GetTooltipText();
			if( !strTooltipText.empty() )
			{
				ShowTooltipDlg( pControl, true, strTooltipText );
			}
		}
	}
}

#ifdef PRE_FIX_COMBOBOX_ELLIPSIS
bool CEtUIDialog::MakeStringWithEllipsis(SUIElement* pElement, float lineWidth, std::wstring& text, const std::wstring& symbol)
#else
void CEtUIDialog::MakeStringWithEllipsis(SUIElement* pElement, float lineWidth, std::wstring& text, const std::wstring& symbol)
#endif
{
#ifdef PRE_FIX_COMBOBOX_ELLIPSIS
	if( !pElement ) return false;
#else
	if( !pElement ) return;
#endif

	int fontIndex		= pElement->nFontIndex;
	int fontHeight		= pElement->nFontHeight;
	DWORD fontFormat	= pElement->dwFontFormat;

	SUICoord sSymbolCoord;
	//CalcTextRect( symbol.c_str(), pElement, sSymbolCoord );
	CalcTextRect(symbol.c_str(), fontIndex, fontHeight, sSymbolCoord, fontFormat, true);

	SUICoord sTextCoord;
	//CalcTextRect( text.c_str(), pElement, sTextCoord );
	CalcTextRect(text.c_str(), fontIndex, fontHeight, sTextCoord, fontFormat, false);
	const float& width = lineWidth;
	const float& height = GetFontHeight( fontIndex, fontHeight );

	if( sTextCoord.fWidth > width )
	{
		float fRemainWidth = width - sSymbolCoord.fWidth;
		int nStrLen = (int)text.length();
		float fTextCoordHeight = sTextCoord.fHeight;
		int i(0), nStartPos(0);

		for( ; (nStartPos+i)<nStrLen; i++)
		{
			SUICoord sSubTextCoord;
			//CalcTextRect( text.substr(nStartPos,i).c_str(), pElement, sSubTextCoord );
			CalcTextRect( text.substr(nStartPos,i).c_str(), fontIndex, fontHeight, sSubTextCoord, fontFormat, false);

			if( sTextCoord.fHeight < (height - fTextCoordHeight) )
			{
				// 텍스트가 다음줄에 계속 찍힌다면...
				if( sSubTextCoord.fWidth > width )
				{
					nStartPos += i-1;
					i = 0;
					fTextCoordHeight += sSubTextCoord.fHeight;
				}
			}
			else
			{
				// 마지막 라인을 검사한다.
				if( sSubTextCoord.fWidth > fRemainWidth )
				{
					i--;

					text = text.substr(0,nStartPos+i);
					text += symbol;
#ifdef PRE_FIX_COMBOBOX_ELLIPSIS
					return true;
#else
					return;
#endif
				}
			}
		}
	}

#ifdef PRE_FIX_COMBOBOX_ELLIPSIS
	return false;
#endif
}
#pragma warning(default:4482)