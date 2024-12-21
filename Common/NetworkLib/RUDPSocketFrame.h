
#pragma once

#include <list>
#include <map>
#include <vector>
#include <winsock.h>
#include <tchar.h>
#include "RUDPCommonDef.h"

const unsigned int _RESEND_TIME = 250;			//	RTO : retransmission time out
const unsigned int _RTO_LIST_MAX = 24*8;				// RTOLISTMAX : retrans instance max size;
const unsigned int _RTO_TICK_MAX = 1000*30;
const unsigned int _SELECT_TIMEOUT_SRV = 1000; //	select timeout time server
const unsigned int _SELECT_BUSYTIME = 100;		//	메세지 처리시간 제한
const unsigned int _CHECK_UNREACHABLE = 150;		//	메세지 받았는 지 검사하는 타이밍
const unsigned int _IDLE_PROCESS = 100;			//	메세지 큐관련 처리 타이밍
const unsigned int _PINGSENDTICK = 13 * 1000;		// 핑체크 간격
const unsigned int _CHECK_RECVTICK = 60 * 1000;		// 언리처블과 별도로 좀비체킹 패킷로스를 고려 조금 길게 잡음

struct _addr_less_ : std::binary_function <SOCKADDR_IN *, SOCKADDR_IN, bool>
{
	bool operator() (const SOCKADDR_IN * a, const SOCKADDR_IN * b) const {
		if (a->sin_port < b->sin_port)
			return true;
		if (a->sin_port == b->sin_port &&
			a->sin_addr.S_un.S_addr < b->sin_addr.S_un.S_addr)
			return true;
		return false;
	}
};


class CRUDPSocketFrame
{
public:
	CRUDPSocketFrame();
	virtual ~CRUDPSocketFrame();

	bool Open(int nID, int nAffinity, int iPort /*= 0*/, bool bIsClient /*= false*/, bool bUseAffinity /*= false*/);
	void Close();

	int Connect(const char * pIP, int iPort);
	void DisConnect(int iNetID);
	void DisConnectAsync(int iNetID);

	int Send(int iNetID, void * data, int len, int prior = _RELIABLE);			//기본은 순서까지 지켜줘효~

	inline unsigned int GetThreadID() { return m_nThreadid; }
	inline bool IsClient() { return m_bClient; }
	inline char * GetPublicIP() { return m_szPublicIP; }
	inline char * GetPrivateIP() { return m_szPrivateIP; }
	inline unsigned long GetCurTick() { return m_nCurTick; }
	
protected:	
	void ThreadMain();
#ifdef PRE_ADD_THREADAFFINITY
	void CheckAffinitySetting(DWORD dwTime);
	void EnableAffinitySetting();
#endif	//#ifdef PRE_ADD_THREADAFFINITY

	void GetHostAddr();
	virtual bool Accept(int iNetID, SOCKADDR_IN * addr, const void * buf, int len) = 0;
	virtual void Recv(int iNetID, void * data, int len) {}
	virtual void DisConnected(int iNetID, bool bForce, bool bUnreachable) {}

	virtual void TimeEvent() {}

	void SendTo(void * data, int len, SOCKADDR_IN * addr);
	int CheckPacket(const void * data, int len, void * outbuf);

	SOCKET m_hSocket;
	unsigned int m_nThreadid;

private:
	friend class CRUDPConnect;

	bool _SetThreadAffinityMask(DWORD dwAffinityMask);

	CRUDPConnect * Connect(unsigned long iIP, int iPort);
	void DisConnectPtr(CRUDPConnect * pCon, bool bForce, bool bUnreachable = false);

	void Recv(void * pData, int iLen, SOCKADDR_IN * pAddr);

	void CheckUnreachable(unsigned long nCurTick);
	void CheckRecvTick(unsigned long nCurTick);
	void FlushAck();
	
	void PingCheck();

	bool IsAlive() { return m_bAlive; }

	void Enter();
	void Leave();

#ifdef _DEBUG
#ifdef _SYNC_RUDP
	bool m_ProtectedSync;
#endif
#endif
	char m_szPublicIP[IPLENMAX];
	char m_szPrivateIP[IPLENMAX];
	short m_nPort;

#ifdef _SYNC_RUDP
	CRITICAL_SECTION m_CS;
#endif
	HANDLE m_hThread;
	volatile bool m_bAlive;
	volatile bool m_bClient;
	unsigned long m_nLastCheckTime[3];
	unsigned long m_nCurTick;
	DWORD m_dwAffinityNum;
	volatile bool m_bEnableAffinityCheck;

	int m_iNetIDCnt;
	std::map <SOCKADDR_IN *, CRUDPConnect *, _addr_less_> m_ConnectRef;
	std::map <int, CRUDPConnect*> m_ConnectList;
	std::vector < std::pair <bool, int> > m_DisConnectList;

	static unsigned int __stdcall _threadmain(void * pParam);
};


class CRUDPConnect
{
public:
	CRUDPConnect(unsigned long iIP, int iPort, int iNetID);
	~CRUDPConnect();

	bool CheckUnreachable(unsigned long iTimeTick, CRUDPSocketFrame * pSocket);
	bool CheckRecvTick(unsigned long nCurTick);
	void FlushAck(CRUDPSocketFrame * pSocket);
	void PingCheck(CRUDPSocketFrame * pSocket);

	int Send(void * pData, int iLen, int iPrior, CRUDPSocketFrame * pSocket);
	void Recv(void * pData, int iLen, CRUDPSocketFrame * pSocket);

	int GetID() const { return m_iNetID; }
	SOCKADDR_IN * GetAddr() { return &m_TargetAddr; }
	static unsigned char GetCRC(const unsigned char * data, int len);

	int GetSendQueueSize(){ return static_cast<int>(m_SendQueue.size()); }

private:
	SOCKADDR_IN m_TargetAddr;
	int m_iNetID;
	BYTE m_cUnreachableCnt;
	UINT m_uiRTOTick;

	unsigned short m_AckList[3][16];
	int m_AckListCnt[3];

	void SendAck(int iFlags, int iAck, SOCKADDR_IN * pAddr, CRUDPSocketFrame * pSocket);

	//packet queue
	bool ReSendQueue(unsigned short iType, unsigned short * iAckList, int iAckNum, CRUDPSocketFrame * pSocket);
	bool RemoveQueue(unsigned short iType, unsigned short * iAckList, int iAckNum);
	std::list <_PACKET_QUEUE*> m_SendQueue;		//중요한 정보!를 받았다는 연락이 없으면 줘낸 쏘는 거다!
	std::list <_PACKET_QUEUE*> m_RecvQueue;		//순서대로 받아야 할 때, 나중 녀석이 먼저 오면 보관~

	//queue memoey
	_PACKET_QUEUE * CreateQueue(CRUDPSocketFrame *pSocket, void * pBuf, int iLen, int iType = 0);
	void ReleaseQueue(_PACKET_QUEUE * pQueue);
	static int MemLevel(int iSize);
	
	std::vector <_PACKET_QUEUE*> m_EmptyQueue[7]; //0~31, 32~63, 64~127, 128~255, 256~511 , 512~1023, 1024~2047
	std::vector <char *> m_AllocedQueue;

	//ack queue
	std::list <unsigned short> m_AckQueue;

	//timing
	unsigned long m_RTOTick;
	unsigned short m_SeqCnt, m_AckCnt, m_FastCnt;
	unsigned short m_RecvSeqCnt, m_RecvAckCnt, m_RecvFastCnt;
	unsigned long m_RecvTick;

	unsigned int m_lastchecktime;
	unsigned int m_sendbytes, m_cnt;

#ifdef _DEBUG
	unsigned int m_nDebugPriorCnt[3];
#endif
};
