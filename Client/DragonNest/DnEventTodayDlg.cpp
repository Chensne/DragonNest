#include "StdAfx.h"
#include "DnMainFrame.h"
#include "DnEventTodayDlg.h"
#include "DnItem.h"
#include "DnVillageTask.h"
#include "boost/scoped_array.hpp"
#include "DnBridgeTask.h"
#include "shlobj.h"
#include <shellapi.h>
#include <WinInet.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

char EVENT_FILE_NAME[256] =  ".\\EventToday.ini";
char IMAGE_FILE_NAME[256] = "";

bool TextEql(const TCHAR * nChar1, const TCHAR * nChar2);
void NormalFile();
void HiddenFile();

CDnEventTodayDlg::CDnEventTodayDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallBack) 
: CEtUIDialog( dialogType, pParentDialog, nID, pCallBack, true ),
m_bEventToday(true),
m_pEventExplain (NULL),
m_pEventShow (NULL),
m_pButtonClose (NULL),
m_bIsImage( true )
{
	TCHAR pBuffer[MAX_PATH]={0};
	TCHAR pImagePath[MAX_PATH]={0};

	SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), pBuffer, CSIDL_PERSONAL, 0);
	wsprintf( pBuffer, L"%s\\DragonNest", pBuffer );

	//이미지 경로 설정
	wsprintf( pImagePath, L"%s\\Event_Page.png", pBuffer );
	WideCharToMultiByte( CP_ACP, 0, pImagePath, -1, IMAGE_FILE_NAME, sizeof(IMAGE_FILE_NAME), NULL, NULL );

	//ini 경로 설정
	wsprintf( pBuffer, L"%s\\EventToday.ini", pBuffer );
	WideCharToMultiByte( CP_ACP, 0, pBuffer, -1, EVENT_FILE_NAME, sizeof(EVENT_FILE_NAME), NULL, NULL );

	ReadEventToday();
}

CDnEventTodayDlg::~CDnEventTodayDlg()
{
	SAFE_RELEASE_SPTR( m_hTexture );
}

void CDnEventTodayDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "EventWindow.ui" ).c_str(), bShow);
}

void CDnEventTodayDlg::InitialUpdate()
{
#if defined(_KR) || defined(_CH) || defined(_JP) || defined(_TW) || defined(_US) || defined(_SG) || defined(_TH) || defined(_ID) || defined(_KRAZ) || defined(_RU) || defined(_EU)

	std::string wszUrl;
	char buff[256];
	GetPrivateProfileStringA("DragonNest","EventURL","",buff,255,".\\DNConfig.ini");
	wszUrl=buff;
	/*
#ifdef _KR
	wszUrl = "http://dn-nexon14.ktics.co.kr/Homepage/eventpopup/kor_event.png";
#elif defined _CH
	wszUrl = "http://static.sdg-china.com/dn/pic/dn_act/webevent/event.png";
#elif defined _JP
	wszUrl = "http://images.nhncorp.jp/core/dragonnest/popup/jp_popup.png";
#elif defined _TW
	wszUrl = "http://tw.beanfun.com/DN/www/ingame_promotion/promotion.png";
#elif defined _US
	wszUrl = "http://dragonnest.nexon.net/game/banner.png";
#elif defined _SG
	wszUrl = "";
#elif defined _TH
	wszUrl = "http://dragonnest.asiasoft.co.th/popup/event.png";
#elif defined _ID
	wszUrl = "http://img.gemscool.com/dragon/ingame/ingamebanner.png";
#elif defined _KRAZ
	wszUrl = "http://actoz.dn.nowcdn.co.kr/Homepage/eventpopup/kor_event.png";
#elif defined _RU	
	wszUrl = "http://dn.mail.ru/static/dn.mail.ru/client/banner.png"; 
#elif defined _EU
	switch(CGlobalInfo::GetInstance().m_eLanguage)
	{
	case MultiLanguage::SupportLanguage::Eng:
		{
			wszUrl = "http://ingame.dragonnest.eu/pages/en/notice/popup.png";
		}
		break;

	case MultiLanguage::SupportLanguage::Ger:
		{
			wszUrl = "http://ingame.dragonnest.eu/pages/de/notice/popup.png";
		}
		break;

	case MultiLanguage::SupportLanguage::Fra:
		{
			wszUrl = "http://ingame.dragonnest.eu/pages/fr/notice/popup.png";
		}
		break;

	case MultiLanguage::SupportLanguage::Esp:
		{
			wszUrl = "http://ingame.dragonnest.eu/pages/es/notice/popup.png";
		}
		break;

	default:
		{
			wszUrl = "http://ingame.dragonnest.eu/pages/en/notice/popup.png";
		}
		break;
	}
#endif */

	HRESULT hr;

	DeleteUrlCacheEntryA( wszUrl.c_str() );

	hr = URLDownloadToFileA( NULL, wszUrl.c_str(), IMAGE_FILE_NAME , 0, NULL  );

	if( hr == S_OK )
	{
		CEtUITextureControl * pTextureControl = GetControl<CEtUITextureControl>( "ID_TEXTUREL_EVENT" );

		CFileStream Stream( IMAGE_FILE_NAME );
		SAFE_RELEASE_SPTR( m_hTexture );
		m_hTexture = (new CEtTexture)->GetMySmartPtr();
		m_hTexture->LoadResource( &Stream );
		pTextureControl->SetTexture( m_hTexture, 0, 0, m_hTexture->Width(), m_hTexture->Height() );

		m_bIsImage = true;
		DeleteFileA( IMAGE_FILE_NAME );
	}
	else m_bIsImage = false;
#endif

#if defined(_WORK)
	CEtUITextureControl * pTextureControl = GetControl<CEtUITextureControl>( "ID_TEXTUREL_EVENT" );

	DeleteControl( pTextureControl );
#endif

	m_pEventExplain = GetControl<CEtUIStatic>( "ID_STATIC0" );

	m_pEventShow = GetControl<CEtUICheckBox>( "ID_CHKBOX_TODAY" );

	m_pButtonClose = GetControl<CEtUIButton>( "ID_CLOSE" );
}

void CDnEventTodayDlg::Show( bool bShow )
{
	if( !m_bIsImage )
		bShow = false;

	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnEventTodayDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED )
	{
		Show(false);
	}
	else if (nCommand == EVENT_CHECKBOX_CHANGED)
	{
		m_bEventToday = m_pEventShow->IsChecked();

		if( m_bEventToday )
		{
			WriteEventToday(false);
			m_bEventToday = false;
		}
		else
		{
			WriteEventToday(true);
			m_bEventToday = true;
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}


void CDnEventTodayDlg::ReadEventToday()
{
	CFileStream * pStream = new CFileStream( EVENT_FILE_NAME );

	if( pStream && pStream->IsValid() ) 
	{
		char charBuff[2048] = {0,};

		memset(m_szEventToday, NULL, sizeof(TCHAR)*2048);
		pStream->Read( charBuff, ( pStream->Size() > 2048 ) ? 2048 : pStream->Size() );
		MultiByteToWideChar(CP_ACP, 0, charBuff, -1, m_szEventToday, 2048);
	}

	//파일이 없다면 생성 한다.
	if( !pStream->IsValid() )
	{
		WriteEventToday();
	}

	SAFE_DELETE( pStream );

	if( CheckEventToday() )
	{
		WriteEventToday(true);
		m_bEventToday = true;
	}
	else
	{
		WriteEventToday(false);
		m_bEventToday = false;
	}
}

void CDnEventTodayDlg::WriteEventToday(bool nCheck /* true */)
{
	NormalFile();

	CFileStream * pStream = new CFileStream( EVENT_FILE_NAME, CFileStream::OPEN_WRITE );
	if( !pStream->IsValid() ) return;

	char szDate[2048] = {0,};
	char id[2048] = {0,};

	time_t t;
	time(&t);
	struct tm today = *localtime(&t);

//	WideCharToMultiByte(CP_ACP, 0, CGlobalValue::GetInstance().m_szID.c_str(), -1, id, 2048, NULL, NULL);

	sprintf(szDate, "%d\n %d\n %d\n %d\n %d\n", 
		CDnBridgeTask::GetInstance().GetAccountDBID(),
		today.tm_mon,									
		today.tm_mday, 
		today.tm_hour,
		nCheck ? 0 : 1 );

	pStream->Write(szDate, 2048);

	SAFE_DELETE( pStream );

	pStream = new CFileStream( EVENT_FILE_NAME );
	if( !pStream->IsValid() ) return;

	memset(id, NULL, 2048);
	memset(m_szEventToday, NULL, sizeof(TCHAR)*2048);
	pStream->Read( id, ( pStream->Size() > 2048 ) ? 2048 : pStream->Size() );
	MultiByteToWideChar(CP_ACP, 0, id, -1, m_szEventToday, 2048);

	SAFE_DELETE( pStream );

	HiddenFile();
}

bool CDnEventTodayDlg::CheckEventToday()
{
	TCHAR szID[32], szMonth[32], szDay[32], szHour[32], szToday[32];
	TCHAR temp[32] = {0,}, temp1[32] = {0,};
	bool Retval = false;	//false일 경우 오늘하루 열지 않음이 된다.
	unsigned int uiMonth, uiDay, uiHour, uiToday;
	int itr = 0;

	time_t t;
	time(&t);
	struct tm today = *localtime(&t);

	memset(szID, NULL, sizeof(TCHAR)*32);
	memset(szMonth, NULL, sizeof(TCHAR)*32);
	memset(szDay, NULL, sizeof(TCHAR)*32);
	memset(szHour, NULL, sizeof(TCHAR)*32);
	memset(szToday, NULL, sizeof(TCHAR)*32);

	ProcessParse(szID, itr);	
	ProcessParse(szMonth, itr);
	ProcessParse(szDay, itr);
	ProcessParse(szHour, itr);
	ProcessParse(szToday, itr);
	uiMonth = _ttoi(szMonth);
	uiDay = _ttoi(szDay);
	uiHour = _ttoi(szHour);
	uiToday = _ttoi(szToday);
//	_tcscpy(temp, CGlobalValue::GetInstance().m_szID.c_str());
//	_tcscpy(temp1, szID);

	//아이디가 같지 않다면 무조건 이벤트 알리미를 띄운다.
	if( _ttoi( szID ) != CDnBridgeTask::GetInstance().GetAccountDBID() )
//	if (  !TextEql(szID, CGlobalValue::GetInstance().m_szID.c_str()) )
		Retval = true;
	else
	{
		if( uiDay == today.tm_mday )
		{
			if( uiHour < 4 && today.tm_hour >= 4 )
				Retval = true;
			else if( uiToday == 0 )
				Retval = true;
		}
		else 
		{
			if( today.tm_hour >= 4 )
				Retval = true;
		}
	}

	return Retval;
}

void CDnEventTodayDlg::ProcessParse(TCHAR * szData, int & nItr)
{
	int itr = 0;

	int length = (int)_tcslen(m_szEventToday);

	while (true)
	{
		if	( m_szEventToday[nItr] == L' ' || length == nItr || m_szEventToday[nItr] == NULL )	break;

		szData[itr] = m_szEventToday[nItr];
		++itr;
		++nItr;
	}

	szData[itr] = NULL;
	++nItr;
}

bool TextEql(const TCHAR * nChar1, const TCHAR * nChar2)
{
	bool Retval = true;	//같을 경우 true를 리턴한다.

	while(true)
	{
		if( NULL == *nChar1 || NULL == *nChar2 )
			break;

		if( *nChar1 != *nChar2 )
		{
			Retval = false;
			break;
		}

		++nChar1;
		++nChar2;
	}

	return Retval;
}

void NormalFile()
{
	HANDLE hFile = CreateFileA(EVENT_FILE_NAME, GENERIC_WRITE,  0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	CloseHandle(hFile);

	SetFileAttributesA(EVENT_FILE_NAME, FILE_ATTRIBUTE_NORMAL);
}

void HiddenFile()
{
	HANDLE hFile = CreateFileA(EVENT_FILE_NAME, GENERIC_READ,  0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);
	CloseHandle(hFile);

	SetFileAttributesA(EVENT_FILE_NAME, FILE_ATTRIBUTE_HIDDEN);
}

#ifdef PRE_ADD_START_POPUP_QUEUE
void CDnEventTodayDlg::DisplayEventPopUp (const char* szUrlAddress)
#else
void CDnEventTodayDlg::DisplayEventPopUp (char* szUrlAddress)
#endif
{

#if defined (PRE_FIX_26377)
	SAFE_RELEASE_SPTR(m_hTexture);
	m_hTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( szUrlAddress ).c_str(), RT_TEXTURE );
	CEtUITextureControl * pTextureControl = GetControl<CEtUITextureControl>( "ID_TEXTUREL_EVENT" );
	if (pTextureControl && m_hTexture)
	{
		pTextureControl->SetTexture( m_hTexture, 0, 0, m_hTexture->Width(), m_hTexture->Height() );
		m_bIsImage = true;
		Show (true);
	}
#else // #if defined (PRE_FIX_26377)

	TCHAR pUrlAddress[MAX_PATH]={0};
	MultiByteToWideChar( CP_ACP, 0, szUrlAddress, -1, pUrlAddress, MAX_PATH );

	TCHAR pBuffer[MAX_PATH]={0};
	TCHAR pImagePath[MAX_PATH]={0};
	char  szImagePath[MAX_PATH] ={0};

	SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), pBuffer, CSIDL_PERSONAL, 0);
	wsprintf( pBuffer, L"%s\\DragonNest", pBuffer );

	//이미지경로설정
	wsprintf( pImagePath, L"%s\\Event_Page.png", pBuffer );
	WideCharToMultiByte( CP_ACP, 0, pImagePath, -1, szImagePath, sizeof(szImagePath), NULL, NULL );

	HRESULT hr;
	hr = URLDownloadToFile( NULL, pUrlAddress , pImagePath , 0, NULL  );

	if( hr == S_OK )
	{
		CEtUITextureControl * pTextureControl = GetControl<CEtUITextureControl>( "ID_TEXTUREL_EVENT" );

		CFileStream Stream( szImagePath );
		SAFE_RELEASE_SPTR( m_hTexture );
		m_hTexture = (new CEtTexture)->GetMySmartPtr();
		m_hTexture->LoadResource( &Stream );
		pTextureControl->SetTexture( m_hTexture, 0, 0, m_hTexture->Width(), m_hTexture->Height() );
		m_bIsImage = true;
		Show (true);
		DeleteFile( pImagePath );
	}
	else m_bIsImage = false;
	
#endif // #if defined (PRE_FIX_26377)
}