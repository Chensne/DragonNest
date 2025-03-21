
#include "Stdafx.h"
#include "ConnectionInfoMngr.h"
#include "XMLParser.h"
#include "NetConnection.h"

#include "ServiceMonitor.h"
#include "MainFrm.h"
#include "BasePartitionView.h"


DEFINE_SINGLETON_CLASS(CConnectionInfoMngr);

CConnectionInfoMngr::CConnectionInfoMngr()
{

}

bool CConnectionInfoMngr::LoadData ()
{
	m_vConnectionList.clear();

	CXMLParser parser;
	if (parser.Open("ConnectionList.xml") == false)
		return false;

	if (parser.FirstChildElement("ConnectionList", true))
	{
		if (parser.FirstChildElement("Connection") == true)
		{
			do {
				stConnectionInfo ConInfo;
				memset(&ConInfo, 0, sizeof(ConInfo));


				WCHAR wszTemp[1024];
				wcscpy_s(wszTemp, NAMELENMAX, parser.GetAttribute("UserName"));
				WideCharToMultiByte(CP_ACP, 0, wszTemp, -1, ConInfo.szUserName, NAMELENMAX, NULL, NULL);

				wcscpy_s(wszTemp, NAMELENMAX, parser.GetAttribute("PasswordId"));
				WideCharToMultiByte(CP_ACP, 0, wszTemp, -1, ConInfo.szPassWord, NAMELENMAX, NULL, NULL);

				wcscpy_s(wszTemp, IPLENMAX, parser.GetAttribute("IP"));
				WideCharToMultiByte(CP_ACP, 0, wszTemp, -1, ConInfo.szIpAddress, IPLENMAX, NULL, NULL);

				ConInfo.wPort = (WORD)_tstoi(parser.GetAttribute("Port"));
				
				ConInfo.nConID = inet_addr((char*)ConInfo.szIpAddress);

				m_vConnectionList.push_back(ConInfo);

			} while (parser.NextSiblingElement("Connection"));
		}
	}

	return true;
}

void CConnectionInfoMngr::ConnectToServerManager ()
{
	stConnectionInfo ConInfo = {0,};
	for (int i=0; i<(int)m_vConnectionList.size(); i++)
	{
		ConInfo = m_vConnectionList[i];

		CNetConnection* pNetConnection = CNetConnectionMgr::GetInstancePtr()->GetConnection (ConInfo.nConID);
		if (pNetConnection) // 기존에 접속한 경우
		{
			CNetSession* pNetSession = pNetConnection->GetSession();
			if (!pNetSession)
				continue;

			DWORD dwRetVal = pNetSession->Connect(ConInfo.nConID, ConInfo.wPort);
			if (NOERROR != dwRetVal)
				continue;
		}
		else // 새로운 접속
		{
			// 아래호출순서대로해야정상작동합니다.
			pNetConnection = CNetConnectionMgr::GetInstancePtr()->CreateConnection();
			if (!pNetConnection)
				continue;

			CNetSession* pNetSession = pNetConnection->GetSession();
			if (!pNetSession)
				continue;

			CMainFrame* pMainFrame = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetMainFrame();
			if (!pMainFrame)
				continue;
			
			CBasePartitionView* pPartitionView = pMainFrame->AddPartition (pNetConnection, ConInfo.nConID);
			if (!pPartitionView)
				continue;

			DWORD dwRetVal = pNetSession->Connect(ConInfo.nConID, ConInfo.wPort);
			if (NOERROR != dwRetVal)
				continue;

			// 커낵션아이디저장
			pPartitionView->SetConnectionID(ConInfo.nConID);

			// 커넥션등록
			CNetConnectionMgr::GetInstancePtr()->RegistConnection (pNetConnection);
			

		}

	}
}

bool CConnectionInfoMngr::GetConInfo (UINT nConID, stConnectionInfo& ConInfo)
{
	for (size_t i=0; i<m_vConnectionList.size(); i++)
	{
		if (m_vConnectionList[i].nConID == nConID)
		{
			ConInfo = m_vConnectionList[i];
			return true;
		}
	}

	return false;
}