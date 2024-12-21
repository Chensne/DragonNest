#include "StdAfx.h"
#include "DnGuildHistoryDlg.h"
#include "DnGuildHistoryItemDlg.h"
#include "DnGuildTask.h"
#include "DnInterfaceString.h"
#include "DnTableDB.h"
#include "DnUIString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildHistoryDlg::CDnGuildHistoryDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pListBoxEx(NULL)
, m_pButtonPagePrev(NULL)
, m_pButtonPageNext(NULL)
, m_pStaticPage(NULL)
, m_nPage(0)
{
}

CDnGuildHistoryDlg::~CDnGuildHistoryDlg(void)
{
	m_pListBoxEx->RemoveAllItems();
}

void CDnGuildHistoryDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildHistoryDlg.ui" ).c_str(), bShow );
}

void CDnGuildHistoryDlg::InitialUpdate()
{
	m_pListBoxEx = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_HISTORY");
	m_pButtonPagePrev = GetControl<CEtUIButton>("ID_BUTTON_PREV");
	m_pButtonPageNext = GetControl<CEtUIButton>("ID_BUTTON_NEXT");
	m_pStaticPage = GetControl<CEtUIStatic>("ID_STATIC_PAGE");
	m_pButtonPagePrev->Enable(false);
	m_pButtonPageNext->Enable(false);
}

void CDnGuildHistoryDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		// 탭다이얼로그 안으로 들어가면서 이렇게 체크한다. 초기화가 끝난 후 MsgProc한번이라도 되야 처리.
		if( m_pParentDialog && m_pParentDialog->GetHWnd() )
			GetGuildTask().RequestGetGuildHistoryList( 1, 0 );	// 항상 처음 열릴땐 0, 0으로 보낸다.
		m_nPage = 1;
	}
	else
	{
		InitControl();
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildHistoryDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_CLOSE") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			if( m_pParentDialog ) m_pParentDialog->Show( false );
			return;
		}
		else if( IsCmdControl("ID_BUTTON_PREV") )
		{
			InitControl();
			GetGuildTask().RequestGetGuildHistoryList( (m_nPage-2)*GUILDHISTORYLIST_MAX+1, 0 );
			m_nPage -= 1;
			return;
		}
		else if( IsCmdControl("ID_BUTTON_NEXT") )
		{
			InitControl();
			GetGuildTask().RequestGetGuildHistoryList( m_nPage*GUILDHISTORYLIST_MAX+1, 0 );
			m_nPage += 1;
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildHistoryDlg::InitControl()
{
	m_pStaticPage->SetText(L"");
	m_pButtonPagePrev->Enable(false);
	m_pButtonPageNext->Enable(false);
	m_pListBoxEx->RemoveAllItems();
}

static bool CompareGuildHistory( TGuildHistory *s1, TGuildHistory *s2 )
{
	// 인덱스가 높은게 최근이니 위로,
	if( s1->biIndex > s2->biIndex ) return false;
	else if( s1->biIndex < s2->biIndex ) return true;

	return false;
}

void CDnGuildHistoryDlg::OnRecvGetGuildHistoryList( SCGetGuildHistoryList *pPacket )
{
	m_pListBoxEx->RemoveAllItems();

	// NextPrev 버튼 Enable 설정
	if( pPacket->nCurCount > GUILDHISTORYLIST_MAX )
		m_pButtonPageNext->Enable(true);
	else
		m_pButtonPageNext->Enable(false);

	if( m_nPage <= 1 )
		m_pButtonPagePrev->Enable(false);
	else
		m_pButtonPagePrev->Enable(true);

	std::wstring wszDate;
	WCHAR wszTemp[256] = {0,};
	WCHAR wszRole[32] = {0,};

	// 히스토리 인덱스 작은 순으로 정렬
	std::vector<TGuildHistory *> vecGuildHistory;
	for( int i = 0; i < pPacket->nCount; ++i )
		vecGuildHistory.push_back(&pPacket->HistoryList[i]);
	std::sort( vecGuildHistory.begin(), vecGuildHistory.end(), CompareGuildHistory );

	// 히스토리 설정
	for( int i = 0; i < pPacket->nCount; ++i ) {
		TGuildHistory *pHistory = vecGuildHistory[i];

		// 타입에 따라 설정
		switch( pHistory->btHistoryType )
		{
		case GUILDHISTORY_TYPE_CREATE:
			{
				// 창설
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3954 ) );
			}
			break;
		case GUILDHISTORY_TYPE_LEVELUP:
			{
				// 길드레벨업
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3765 ), pHistory->nInt1 );
			}
			break;
		case GUILDHISTORY_TYPE_SKILLGAIN:
			{
				//	길드 스킬 획득시
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3955 ), pHistory->Text );
			}
			break;
		case GUILDHISTORY_TYPE_SKILLENFC:
			{
				//	길드 스킬 강화시
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3956 ), pHistory->Text );
			}
			break;
		case GUILDHISTORY_TYPE_MARKGAIN:
			{
				//	길드 마크 획득시
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3957 ) );
			}
			break;
		case GUILDHISTORY_TYPE_MARKCHNG:
			{
				//	길드 마크 변경시
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3958 ) );
			}
			break;
		case GUILDHISTORY_TYPE_JOIN:
			{
				// 가입한 길드원 캐릭명
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3905 ), pHistory->Text );
			}
			break;
		case GUILDHISTORY_TYPE_LEAVE:
			{
				// 탈퇴한 길드원 캐릭명
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3906 ), pHistory->Text );
			}
			break;
		case GUILDHISTORY_TYPE_EXILE:
			{
				// 추방한 길드원 캐릭명
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3907 ), pHistory->Text );
			}
			break;
		case GUILDHISTORY_TYPE_ROLECHNG:
			{
				// 직급 변경
				switch(pHistory->nInt1)
				{
				case GUILDROLE_TYPE_MASTER:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3733 ) );	break;
				case GUILDROLE_TYPE_SUBMASTER:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3734 ) );	break;
				case GUILDROLE_TYPE_SENIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3735 ) );	break;
				case GUILDROLE_TYPE_REGULAR:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3736 ) );	break;
				case GUILDROLE_TYPE_JUNIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3737 ) );	break;
				}
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3903 ), pHistory->Text, wszRole );
			}
			break;
		case GUILDHISTORY_TYPE_MEMBLEVELUP:
			{
				// 길드원 레벨업
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3959 ), pHistory->Text, pHistory->nInt1 );
			}
			break;
		case GUILDHISTORY_TYPE_ITEMENCT:
			{
				// 길드원 고강화 성공시
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
				if( pSox ) {
					std::wstring wszItemName;
					int nNameID = pSox->GetFieldFromLablePtr( pHistory->nInt2, "_NameID" )->GetInteger();
					char *szParam = pSox->GetFieldFromLablePtr( pHistory->nInt2, "_NameIDParam" )->GetString();
					MakeUIStringUseVariableParam( wszItemName, nNameID, szParam );
					swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3960 ), pHistory->Text, pHistory->nInt1, wszItemName.c_str() );
				}
			}
			break;
		case GUILDHISTORY_TYPE_MISSION:
			{
				// 길드 미션 성공
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3961 ), pHistory->Text );
			}
			break;
			//	길드 래더 랭크 진입시	13	길드 래더 랭크 %d위를 달성하였습니다.	3962
		case GUILDHISTORY_TYPE_RENAME :
			{
				// 길드 이름 변경
				wstring strTemp = pHistory->Text;
				wstring::size_type n = strTemp.find_first_of(',',0);
				
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3970 ), (strTemp.substr(0, n)).c_str()			
					,(strTemp.substr(n+1, strTemp.length())).c_str()  );	// UISTRING : 길드 이름이 %s에서 %s로 변경 되었습니다.
			}
			break;
		case GUILDHISTORY_TYPE_WARESIZE:
			{
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3823 ), pHistory->nInt1 );
			}
			break;
		case GUILDHISTORY_TYPE_RECRUITSTART:
			{
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3994 ) );
			}
			break;
		case GUILDHISTORY_TYPE_RECRUITEND:
			{
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3995 ) );
			}
			break;
		}

#ifdef _US
		DN_INTERFACE::STRING::GetDayTextSlash( DN_INTERFACE::STRING::FORMAT_MM_DD_YY, wszDate, pHistory->RegDate );
#elif _RU
		DN_INTERFACE::STRING::GetDayTextSlash( DN_INTERFACE::STRING::FORMAT_DD_MM_YY, wszDate, pHistory->RegDate, '.' );
#else // _US
		DN_INTERFACE::STRING::GetDayText( wszDate, pHistory->RegDate );
#endif // _US

		CDnGuildHistoryItemDlg *pItemDlg = m_pListBoxEx->AddItem<CDnGuildHistoryItemDlg>();
		pItemDlg->SetInfo( wszDate.c_str(), wszTemp );
	}
	m_pStaticPage->SetIntToText( m_nPage );
}