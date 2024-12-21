#include "Stdafx.h"
#include "DnDummyClientMgr.h"
#include "DnDummyInfoListCtrl.h"
#include "DnDummyClientFrame.h"
#include "LogWnd.h"



//------------------------------------------------------------------
DnDummyClientMgr g_DummyClientMgr;

DnDummyClientMgr::DnDummyClientMgr()
{
	//_CrtSetBreakAlloc(771);
	ClearIOInfo();
	//LogWnd::CreateLog();

	m_nMode = 0;

}

DnDummyClientMgr::~DnDummyClientMgr()
{
	AllDisconnect();

	for ( int i = 0 ; i < (int)m_DummyClients.size() ; i++ )
	{
		DnDummyClient* pDummy = m_DummyClients[i];
		if ( pDummy )
			delete pDummy;
	}

	m_DummyClients.clear();
	//LogWnd::DestroyLog();
}

void DnDummyClientMgr::AllDisconnect()
{

	for ( int i = 0 ; i < (int)m_DummyClients.size() ; i++ )
	{
		DnDummyClient* pDummy = m_DummyClients[i];
		if ( pDummy )
			pDummy->Disconnect();
	}
}

void DnDummyClientMgr::CreateDummy(std::vector<DnDummyClient::CreateInfo>& DummyCreateInfo)
{
	for ( int i = 0 ; i < (int)DummyCreateInfo.size() ; i++ )
	{
		DnDummyClient* pDummyclient = new DnDummyClient();
		pDummyclient->Create(DummyCreateInfo[i]);
		m_DummyClients.push_back(pDummyclient);
	}

	LogWnd::Log(1, _T("더미 생성"));
	InitializeDummyInfoListCtrl();
}

void DnDummyClientMgr::InitializeDummyInfoListCtrl()
{
	if ( !g_pDummyInfoListCtrl )
		return;

	g_pDummyInfoListCtrl->ResetAll();

	for ( int i = 0 ; i < (int)m_DummyClients.size() ; i++ )
	{
		g_pDummyInfoListCtrl->InsertDummyInfo(i, m_DummyClients[i]->GetProperty() );

	}
}

DnDummyClient* 
DnDummyClientMgr::GetDummyClient(int nIndex)
{
	if ( nIndex < 0 || nIndex > (int) m_DummyClients.size() )
		return NULL;


	return m_DummyClients[nIndex];
}

void 
DnDummyClientMgr::UpdateDummyInfo()
{
	for ( int i = 0 ; i < (int)m_DummyClients.size() ; i++ )
	{
		g_pDummyInfoListCtrl->UpdateDummyInfo(i, m_DummyClients[i]->GetProperty() );
	}

}


void
DnDummyClientMgr::Update()
{

	static DWORD __nCnt = 0;
	static DWORD __nTime = GetTickCount();

	m_nCurrentPartingUserCnt = 0;
	for ( int i = 0 ; i < (int)m_DummyClients.size() ; i++ )
	{
		if ( m_DummyClients[i]->GetProperty().nPlayState == PlayStateInfo::PS_THINKING_PARTY )
		{
			m_nCurrentPartingUserCnt++;
		}
	}

	for ( int i = 0 ; i < (int)m_DummyClients.size() ; i++ )
	{
		m_DummyClients[i]->Update();
	}

	__nCnt++;

	DWORD nCurTime = GetTickCount();
	if ( nCurTime - __nTime >= 1000 )
	{
		__nTime = nCurTime;
		LogWnd::Log(1, _T("1초당 : %d Cnt" ), __nCnt);
		__nCnt = 0;
	}

	
}

void
DnDummyClientMgr::DumpCurrentInfo(std::wstring& szStr)
{

	for ( int i = 0 ; i < (int)m_DummyClients.size() ; i++ )
	{
		//if ( m_DummyClients[i]->IsConnected() == false )	continue;

		if ( i == 106 ) 
		{
			int a = 0 ;
		}
		DnDummyClient::Property& property = m_DummyClients[i]->GetProperty();

		wxString strUserIdx = wxString::Format( _T("%d"), i);
		wxString strResponseTime = wxString::Format( _T("%d"), property.nResponseTime);
		wxString strResponseAvrTime = wxString::Format( _T("%d"), property.nResponseAvrTime);
		wxString strOnDamage = wxString::Format( _T("%d"), property.nDamageCnt);
		wxString strHit = wxString::Format( _T("%d"), property.nHitCnt);
		std::wstring strTime = GetTimeString(property.ConnectTime);

		szStr += strUserIdx.c_str();
		szStr += L",";
		
		szStr += gs_State[property.nConnectionState].szState.c_str();
		szStr += L",";

		szStr += property.szUserID.c_str();
		szStr += L",";

		szStr += property.szCharacter.c_str();
		szStr += L",";

		szStr += gs_PlayState[property.nPlayState].szState.c_str();
		szStr += L",";

		szStr += gs_CurrentServerInfo[property.nCurPosition].szState.c_str();
		szStr += L",";
		
		szStr += strTime.c_str();
		szStr += L",";

		szStr += strResponseTime.c_str();
		szStr += L",";

		szStr += strResponseAvrTime.c_str();
		szStr += L",";

		szStr += strOnDamage.c_str();
		szStr += L",";

		szStr += strHit.c_str();
		szStr += L",";
	
		szStr += L"\n";

	}
}

void
DnDummyClientMgr::GetClientCntInfo(int& nConnected, int& nDisConnected, int& nVillageCnt, int& nGameRoomCnt, int& nGamePlayCnt )
{	
	nConnected = 0;
	nDisConnected = 0;
	nVillageCnt = 0;
	nGameRoomCnt = 0;
	nGamePlayCnt = 0;



	for ( int i = 0 ; i < (int)m_DummyClients.size() ; i++ )
	{
		if ( m_DummyClients[i]->GetProperty().nConnectionState == StateInfo::_CONNECTED ) 
		{
			nConnected++;
		}
		else 
		{
			nDisConnected++;
			continue;
		}


		int nPlayState = m_DummyClients[i]->GetProperty().nPlayState;

		/*if ( nPlayState > PlayStateInfo::PS_NONE && nPlayState < PlayStateInfo::PS_GAME_PLAY )
			nVillageCnt++;

		if ( nPlayState <= PlayStateInfo::PS_GAME_PLAY)
			nGamePlayCnt++;

		if ( nPlayState == PlayStateInfo::PS_GAME_PLAY )
		{
			if ( m_DummyClients[i]->IsPartyLeader() )
				nGameRoomCnt++;
		}*/
	}
}




bool	LoadDummyClientFile(const CHAR* szFileName, OUT std::vector<DnDummyClient::CreateInfo>& DummyCreateInfo)
{
	TiXmlDocument doc;
	doc.LoadFile(szFileName, TIXML_ENCODING_UTF8 );

	std::string msg;
	std::wstring wmsg;

	if ( doc.Error() )
	{
		msg = "xml 파일 파싱 실패 : ";
		msg += szFileName;
		msg += " error msg : ";
		const char* errmsg = doc.ErrorDesc();
		msg += errmsg;

		ToWideString(msg, wmsg);
		Log( wmsg.c_str() );

		return false;
	}

	TiXmlElement* pElement = NULL;
	pElement = doc.RootElement();

	if ( !pElement )
	{
		msg = "xml 파일 파싱 실패 : ";
		msg += szFileName;
		msg += " error msg : ";
		const char* errmsg = doc.ErrorDesc();
		msg += errmsg;

		ToWideString(msg, wmsg);
		Log( wmsg.c_str() );
	}

	TiXmlNode* pNode = pElement->FirstChild("user");

	if ( !pNode )		return false;

	pElement = pNode->ToElement();

	for ( pElement ; pElement != NULL ; pElement = pElement->NextSiblingElement() )
	{
		DnDummyClient::CreateInfo info;

		const char* szid = pElement->Attribute("id");
		const char* szpasswd = pElement->Attribute("passwd");
		const char* szChar = pElement->Attribute("character");
		const char* szExtra = pElement->Attribute("extra");

		if ( szid == NULL || szpasswd == NULL || szChar == NULL )
			continue;

		ToWideString((CHAR*)szid, info.szUserID);
		ToWideString((CHAR*)szpasswd, info.szPasswd);
		ToWideString((CHAR*)szChar, info.szCharacter);

		if ( szExtra )
			ToWideString((CHAR*)szExtra, info.szExtra);
		

		DummyCreateInfo.push_back(info);

	}

	g_DummyClientMgr.CreateDummy(DummyCreateInfo);
	return true;
}
