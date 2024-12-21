#pragma once

#include "DnDummyClient.h"

static const wxChar* gs_PlayMode[] = {	wxT("DefaultMode"), wxT("VillageMode"), wxT("MoveMode"), (const wxChar*) NULL};

class DnDummyClientMgr
{
public:
	enum Mode
	{
		DefaultMode = 0,
		VillageMode,
		MoveMode,
	};

	DnDummyClientMgr();
	virtual ~DnDummyClientMgr();

	void CreateDummy(std::vector<DnDummyClient::CreateInfo>& DummyCreateInfo);
	void InitializeDummyInfoListCtrl();

	int  GetPartingUserCnt(){ return m_nCurrentPartingUserCnt;}
	void Update();
	void UpdateDummyInfo();
	DnDummyClient* GetDummyClient(int nIndex);
	int	GetDummyClientSize() { return (int)m_DummyClients.size(); }
	void GetClientCntInfo(int& nConnected, int& nDisConnected, int& nVillageCnt, int& nGameRoomCnt, int& nGamePlayCnt );
	void AllDisconnect();
	void DumpCurrentInfo(std::wstring& szStr);


	int		GetMode() { return m_nMode; }
	void	SetMode(int nMode ) { m_nMode = nMode; }

private:

	std::vector<DnDummyClient*>	m_DummyClients;
	


	// 여기는 udp io 정보 ㅋㅋ
public:
	void		AddSendBytes(DWORD nBytes) { m_nTotalSendBytes += nBytes; }
	void		AddRecvBytes(DWORD nBytes) { m_nTotalRecvBytes += nBytes; }
	void		AddSendCount() { ++m_nTotalSendCnt; }
	void		AddRecvCount() { ++m_nTotalRecvCnt; }
	void		ClearIOInfo() { m_nTotalSendBytes = m_nTotalRecvBytes = m_nTotalSendCnt = m_nTotalRecvCnt = 0; }

	DWORD	m_nTotalSendBytes;
	DWORD	m_nTotalRecvBytes;

	DWORD	m_nTotalSendCnt;
	DWORD	m_nTotalRecvCnt;

	int		m_nCurrentPartingUserCnt;
	int		m_nMode;
};

extern DnDummyClientMgr	g_DummyClientMgr;

bool	LoadDummyClientFile(const CHAR* szFileName, OUT std::vector<DnDummyClient::CreateInfo>& DummyCreateInfo);
