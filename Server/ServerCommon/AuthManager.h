#pragma once

class CDNDBConnection;

// 전체 인증기능 관리
class CAuthManager
{

public:
	CAuthManager();

	virtual bool Init();

	bool IsResetAuthServer() const { return m_bResetAuthServer; }
	void SetResetAuthServer(bool bResetAuthServer) { m_bResetAuthServer = bResetAuthServer; }

	void Run(DWORD CurTick);

	INT64 GetCertifyingKey();
	int GetServerID() const { return m_ServerID; }

protected:
	virtual int QueryResetAuthServer() = 0;

private:
	const int CalculateServerID();
	char* GetLocalIp(OUT in_addr* pInAddr = NULL);

protected:
	bool m_bResetAuthServer;		// 최초 서버 기동 시 '인증정보 초기화 (특정 서버) ' 가 이루어졌는지 여부
	DWORD m_ResetAuthServerTick;	// 최초 서버 기동 시 '인증정보 초기화 작업주기 관리 (단위:ms)

	int m_ServerID;					// 각 서버마다의 고유 ID
};

