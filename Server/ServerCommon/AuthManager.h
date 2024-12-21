#pragma once

class CDNDBConnection;

// ��ü ������� ����
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
	bool m_bResetAuthServer;		// ���� ���� �⵿ �� '�������� �ʱ�ȭ (Ư�� ����) ' �� �̷�������� ����
	DWORD m_ResetAuthServerTick;	// ���� ���� �⵿ �� '�������� �ʱ�ȭ �۾��ֱ� ���� (����:ms)

	int m_ServerID;					// �� ���������� ���� ID
};

