#include "StdAfx.h"
#include "DNNexonAuth.h"
#include "DNIocpManager.h"
#include "Log.h"
#include "DNServerPacket.h"
#include "Stream.h"
#include "Util.h"
#include "DNUser.h"
#include "DNDivisionManager.h"
#include "DNLoginConnection.h"

extern TMasterConfig g_Config;

#if defined(_KR) && defined(_FINAL_BUILD)

CDNNexonAuth* g_pNexonAuth;

CDNNexonAuth::CDNNexonAuth(void): CConnection()
{
	SetIp(g_Config.AuthInfo.szIP);
	SetPort(g_Config.AuthInfo.nPort);

	Init(1024 * 200, 1024 * 200);

	m_dwReconnectTick = 0; 
	m_dwAuthAliveTick = timeGetTime();
}

CDNNexonAuth::~CDNNexonAuth(void)
{
}

void CDNNexonAuth::DoUpdate(DWORD CurTick)
{
	if (m_dwAuthAliveTick + 60000 < CurTick){
		SendAlive();
		m_dwAuthAliveTick = CurTick;
	}
}

void CDNNexonAuth::Reconnect(DWORD CurTick)
{
	if (m_dwReconnectTick + 5000 < CurTick){
		m_dwReconnectTick = CurTick;

		if (!GetActive() && !GetConnecting() )
		{
			SetConnecting(true);
			if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_AUTH, m_szIP, m_wPort) < 0){
				SetConnecting(false);
				g_Log.Log(LogType::_FILELOG, L"AuthServer Fail(%S, %d)\r\n", m_szIP, m_wPort);
			}
			else 
			{
				g_Log.Log(LogType::_FILELOG, L"AuthServer(%S, %d) Connecting..\r\n", m_szIP, m_wPort);				
			}
		}
	}
}

int CDNNexonAuth::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{
	int nStreamSize = 0;

	CByteStream Stream(pData, nLen + 3);

	BYTE cHeader = 0;
	USHORT wLength = 0;
	BYTE cPacketType = 0;

	Stream.Read(&cHeader, sizeof(BYTE));
	Stream.Read(&wLength, sizeof(USHORT));
	Stream.Read(&cPacketType, sizeof(BYTE));

	switch (cPacketType)
	{
	case AUTH_INITIALIZE:
		{
			BYTE cInitializeType = 0;
			BYTE cResult = 0;
			BYTE cDomainSN = 0;
			char szMessage[64] = { 0, };
			BYTE cPropertyCount = 0;

			Stream.Read(&cInitializeType, sizeof(BYTE));
			Stream.Read(&cResult, sizeof(BYTE));
			Stream.Read(&cDomainSN, sizeof(BYTE));
			Stream.Read(&szMessage[0], sizeof(BYTE));
			Stream.Read(&szMessage[1], szMessage[0]);
			Stream.Read(&cPropertyCount, sizeof(BYTE));

			if (cResult != INIT_OK)
				g_Log.Log(LogType::_FILEDBLOG, L"AUTH_INITIALIZE Failed (%d, %S) \r\n", cResult, &szMessage[1]);
			else
				g_Log.Log(LogType::_FILEDBLOG, L"AUTH_INITIALIZE OK! (%d, %S) \r\n", cResult, &szMessage[1]);
		}
		break;

	case AUTH_LOGIN:
		{
			BYTE cLoginType = 0;
			Stream.Read(&cLoginType, sizeof(BYTE));

			INT64 biSessionNo = 0;
			Stream.Read(&biSessionNo, sizeof(INT64));
			biSessionNo = SWAP64(biSessionNo);

			BYTE cAccountLen = 0;
			char szAccountID[IDLENMAX * 2] = { 0, };	// 상위계정아이디
			Stream.Read(&cAccountLen, sizeof(BYTE));
			Stream.Read(szAccountID, cAccountLen);

			char cAuthorizeResult = 0;		// eAuthResult
			Stream.Read(&cAuthorizeResult, sizeof(char));

			char cAuthorizeType = 0;		// (AddressDesc) 사용자 ip에 대한 인증결과
			Stream.Read(&cAuthorizeType, sizeof(char));	// ip에 대한 인증결과

			char cChargeType = 0;			// (AccountDesc) 사용자 id에 대한 인증결과
			Stream.Read(&cChargeType, sizeof(char));	// 계정에 대한 인증결과

			char cOption = 0;				// 사용자에게 보여주는 메시지
			Stream.Read(&cOption, sizeof(char));		// 사용자에게 보여주는 메시지

			int nArgument = 0;				// 기타 더 필요한 데이터를 여기에 첨부 (ex. 시간제의 경우 남은시간, 정액제인 경우 만료날짜 등등)
			Stream.Read(&nArgument, sizeof(int));
			nArgument = SWAP32(nArgument);

			char cIsCharged = 0;			// (GameBangBillType) ip가 게임방 과금되는 경우 1. 아니면 0
			Stream.Read(&cIsCharged, sizeof(char));

			char cPropertyCount = 0;
			Stream.Read(&cPropertyCount, sizeof(char));

			USHORT wPropertyNo = 0;
			int nPCBangNo = 0;
			BYTE cPCBangNameLen = 0;
			char szPCBangName[128] = { 0, };
			char cPCBangValided = 0;
			BYTE cPCBangZipCodeLen = 0;
			char szPCBangZipCode[7] = { 0, };
			char cPCBangLevel = 0;
			bool bShutdowned = false;
			char cPolicyError = 0;
			int nShutdownTime = 0;

			for (int i = 0; i < cPropertyCount; i++){
				nStreamSize = Stream.Read(&wPropertyNo, sizeof(USHORT));
				if (nStreamSize == 0) break;

				wPropertyNo = SWAP16(wPropertyNo);

				switch (wPropertyNo)
				{
				case 7:		// pcbang no
					Stream.Read(&nPCBangNo, sizeof(int));
					nPCBangNo = SWAP32(nPCBangNo);
					break;

				case 8:		// pcbang valided
					Stream.Read(&cPCBangValided, sizeof(char));
					break;

				case 9:		// pcbang name
					Stream.Read(&cPCBangNameLen, sizeof(BYTE));
					Stream.Read(szPCBangName, cPCBangNameLen);
					break;

				case 10:	// pcbang zipcode
					Stream.Read(&cPCBangZipCodeLen, sizeof(BYTE));
					Stream.Read(szPCBangZipCode, cPCBangZipCodeLen);
					break;

				case 11:	// pcbang level
					Stream.Read(&cPCBangLevel, sizeof(char));
					break;

				case 19:	// policy Result
					char cPolicyCount = 0;
					Stream.Read(&cPolicyCount, sizeof(char));

					for (int j = 0; j < cPolicyCount; j++){
						char cPolicyNo = 0;
						Stream.Read(&cPolicyNo, sizeof(char));

						BYTE cPolicyLen = 0;
						char szPolicy[64] = { 0, };
						Stream.Read(&cPolicyLen, sizeof(BYTE));
						Stream.Read(szPolicy, cPolicyLen);

						char cPolicyResult = 0;
						Stream.Read(&cPolicyResult, sizeof(char));

						BYTE cPolicyResultLen = 0;
						char szPolicyResult[256] = { 0, };
						Stream.Read(&cPolicyResultLen, sizeof(BYTE));
						Stream.Read(szPolicyResult, cPolicyResultLen);

						switch (cPolicyNo)
						{
						case 10:	// 게임이용차단정책
							{
								if (cPolicyResult == 1)
									bShutdowned = true;

								if (cPolicyResultLen > 0){
									g_Log.Log(LogType::_SHUTDOWN, L"[A:%S (%lld)] AUTH_LOGIN Shutdown (PolicyNo:%d PolicyResult:%d %S)\r\n", szAccountID, biSessionNo, cPolicyNo, cPolicyResult, szPolicyResult);

									std::vector<std::string> Tokens1, Tokens2;
									Tokens1.clear();
									TokenizeA(szPolicyResult, Tokens1, "&");
									if (!Tokens1.empty()){
										for (i = 0; i < (int)Tokens1.size(); i++){
											Tokens2.clear();
											TokenizeA(Tokens1[i].c_str(), Tokens2, "=");

											if ((!Tokens2.empty()) && (Tokens2.size() > 1)){
												if (strcmp(Tokens2[0].c_str(), "time") == 0){
													nShutdownTime = atoi(Tokens2[1].c_str());
												}
												else if (strcmp(Tokens2[0].c_str(), "error") == 0){
													cPolicyError = atoi(Tokens2[1].c_str());
												}
											}
										}
									}
								}
							}
							break;
						}
					}
					break;
				}
			}

			WCHAR wszAccountName[IDLENMAX];
			MultiByteToWideChar(CP_ACP, 0, szAccountID, -1, wszAccountName, IDLENMAX);

			CDNUser *pUser = g_pDivisionManager->GetUserByAccountName(wszAccountName);
			if (!pUser){
				g_Log.Log(LogType::_NORMAL, L"[A:%S (%lld)] AUTH_LOGIN Fail!! (Ret:%d Address:%c Option:%d Argument:%d) PCBang:%d %d (Valided:%d) Shutdowned:%d Time:%d error:%d\r\n", 
					szAccountID, biSessionNo, cAuthorizeResult, cAuthorizeType, cOption, nArgument, nPCBangNo, cPCBangLevel, cPCBangValided, bShutdowned, nShutdownTime, cPolicyError);

				return ERROR_UNKNOWN_HEADER;
			}

			pUser->m_biNexonSessionNo = biSessionNo;

			pUser->m_cPCBangResult = cAuthorizeResult;
			pUser->m_cPCBangAuthorizeType = cAuthorizeType;
			pUser->m_cPCBangOption = cOption;
			pUser->m_nPCBangArgument = nArgument;
			pUser->m_bShutdowned = bShutdowned;
			pUser->m_cPolicyError = cPolicyError;
			pUser->m_nShutdownTime = nShutdownTime;

			g_Log.Log(LogType::_NORMAL, pUser, L"[A:%u %S (%lld)] AUTH_LOGIN (Ret:%d Address:%c Option:%d Argument:%d) Ip:%S PCBang:%d %d (Valided:%d) Shutdowned:%d Time:%d error:%d\r\n", 
				pUser->GetAccountDBID(), szAccountID, biSessionNo, cAuthorizeResult, cAuthorizeType, cOption, nArgument, pUser->GetIp(), nPCBangNo, cPCBangLevel, cPCBangValided, bShutdowned, nShutdownTime, cPolicyError);

			switch (cAuthorizeResult)
			{
			case Result_Trial:
			case Result_Forbidden:	// 미가입PC방 또는 IP당 접속수를 초과한 경우
				break;

			case Result_Allowed:	// 가입된 PC방
				pUser->SetPCBang(true);
				g_pDivisionManager->SendPCBangResult(pUser);
				break;

			case Result_Terminate:	// 정량제 이용하다 시간 만료된 경우
				g_pDivisionManager->SendPCBangResult(pUser);
				break;
			}

			if (pUser->m_nShutdownTime > 0)
				g_pDivisionManager->SendPCBangResult(pUser);
		}
		break;

	case AUTH_TERMINATE:
		{
			BYTE cTerminateType = 0;
			Stream.Read(&cTerminateType, sizeof(BYTE));

			INT64 biSessionNo = 0;
			Stream.Read(&biSessionNo, sizeof(INT64));
			biSessionNo = SWAP64(biSessionNo);

			BYTE cAccountLen = 0;
			char szAccountID[IDLENMAX * 2] = { 0, };	// 상위계정아이디
			Stream.Read(&cAccountLen, sizeof(BYTE));
			Stream.Read(szAccountID, cAccountLen);

			BYTE cCharLen = 0;
			char szCharName[NAMELENMAX * 2] = { 0, };
			Stream.Read(&cCharLen, sizeof(BYTE));
			Stream.Read(szCharName, cCharLen);

			char cOption = 0;							// 사용자에게 보여주는 메시지
			Stream.Read(&cOption, sizeof(char));

			bool bShutdowned = false;
			char cPropertyCount = 0;
			Stream.Read(&cPropertyCount, sizeof(char));

			int nShutdownTime = 0;
			char cPolicyError = 0;
			USHORT wPropertyNo = 0;
			for (int i = 0; i < cPropertyCount; i++){
				nStreamSize = Stream.Read(&wPropertyNo, sizeof(USHORT));
				if (nStreamSize == 0) break;

				wPropertyNo = SWAP16(wPropertyNo);

				switch (wPropertyNo)
				{
				case 19:	// policy Result
					char cPolicyCount = 0;
					Stream.Read(&cPolicyCount, sizeof(char));

					for (int j = 0; j < cPolicyCount; j++){
						char cPolicyNo = 0;
						Stream.Read(&cPolicyNo, sizeof(char));

						BYTE cPolicyLen = 0;
						char szPolicy[64] = { 0, };
						Stream.Read(&cPolicyLen, sizeof(BYTE));
						Stream.Read(szPolicy, cPolicyLen);

						char cPolicyResult = 0;
						Stream.Read(&cPolicyResult, sizeof(char));

						BYTE cPolicyResultLen = 0;
						char szPolicyResult[256] = { 0, };
						Stream.Read(&cPolicyResultLen, sizeof(BYTE));
						Stream.Read(szPolicyResult, cPolicyResultLen);

						switch (cPolicyNo)
						{
						case 10:	// 게임이용차단정책
							{
								if (cPolicyResult == 1)
									bShutdowned = true;

								if (cPolicyResultLen > 0){
									g_Log.Log(LogType::_SHUTDOWN, L"[A:%S (%lld)] AUTH_LOGIN Shutdown (PolicyNo:%d PolicyResult:%d %S)\r\n", szAccountID, biSessionNo, cPolicyNo, cPolicyResult, szPolicyResult);

									std::vector<std::string> Tokens1, Tokens2;
									Tokens1.clear();
									TokenizeA(szPolicyResult, Tokens1, "&");
									if (!Tokens1.empty()){
										for (i = 0; i < (int)Tokens1.size(); i++){
											Tokens2.clear();
											TokenizeA(Tokens1[i].c_str(), Tokens2, "=");

											if ((!Tokens2.empty()) && (Tokens2.size() > 1)){
												if (strcmp(Tokens2[0].c_str(), "time") == 0){
													nShutdownTime = atoi(Tokens2[1].c_str());
												}
												else if (strcmp(Tokens2[0].c_str(), "error") == 0){
													cPolicyError = atoi(Tokens2[1].c_str());
												}
											}
										}
									}
								}
							}
							break;
						}
					}
					break;
				}
			}

			CDNUser *pUser = g_pDivisionManager->GetUserByNexonSessionNo(biSessionNo);
			if (!pUser) return ERROR_UNKNOWN_HEADER;

			pUser->m_cPCBangResult = Result_Terminate;
			pUser->m_cPCBangOption = cOption;
			pUser->m_bShutdowned = bShutdowned;
			pUser->m_nShutdownTime = nShutdownTime;
			pUser->m_cPolicyError = cPolicyError;

			g_pDivisionManager->SendPCBangResult(pUser);
			g_Log.Log(LogType::_NORMAL, pUser, L"[A:%u %S (%lld)] AUTH_TERMINATE (Address:%c Option:%d Argument:%d) Ip:%S Shutdowned:%d Time:%d error:%d\r\n", 
				pUser->GetAccountDBID(), szAccountID, biSessionNo, pUser->m_cPCBangAuthorizeType, pUser->m_cPCBangOption, pUser->m_nPCBangArgument, pUser->GetIp(), bShutdowned, nShutdownTime, cPolicyError);
		}
		break;

	case AUTH_MESSAGE:
		{
			BYTE cMessageType = 0;
			Stream.Read(&cMessageType, sizeof(BYTE));

			INT64 biSessionNo = 0;
			Stream.Read(&biSessionNo, sizeof(INT64));
			biSessionNo = SWAP64(biSessionNo);

			BYTE cAccountLen = 0;
			char szAccountID[IDLENMAX * 2] = { 0, };	// 상위계정아이디
			Stream.Read(&cAccountLen, sizeof(BYTE));
			Stream.Read(szAccountID, cAccountLen);

			BYTE cCharLen = 0;
			char szCharName[NAMELENMAX * 2] = { 0, };
			Stream.Read(&cCharLen, sizeof(BYTE));
			Stream.Read(szCharName, cCharLen);

			char cOption = 0;							// 사용자에게 보여주는 메시지
			Stream.Read(&cOption, sizeof(char));

			int nArgument = 0;							// 기타 더 필요한 데이터를 여기에 첨부 (ex. 시간제의 경우 남은시간, 정액제인 경우 만료날짜 등등)
			Stream.Read(&nArgument, sizeof(int));
			nArgument = SWAP32(nArgument);

			WCHAR wszCharName[NAMELENMAX];
			MultiByteToWideChar(CP_ACP, 0, szCharName, -1, wszCharName, NAMELENMAX);

			CDNUser *pUser = g_pDivisionManager->GetUserByName(wszCharName);
			if (!pUser) return ERROR_UNKNOWN_HEADER;

			pUser->m_cPCBangResult = Result_Message;
			pUser->m_cPCBangOption = cOption;
			pUser->m_nPCBangArgument = nArgument;

			g_pDivisionManager->SendPCBangResult(pUser);
		}
		break;

	case AUTH_SYNCHRONIZE:
		{
			char cIsMonitoring = 0;
			Stream.Read(&cIsMonitoring, sizeof(char));

			int nCount = 0;
			Stream.Read(&nCount, sizeof(int));
			nCount = SWAP32(nCount);

			std::vector<INT64> VecSessionNoList;
			VecSessionNoList.clear();

			INT64 biSessionNo = 0;
			for (int i = 0; i < nCount; i++){
				biSessionNo = 0;

				Stream.Read(&biSessionNo, sizeof(INT64));
				biSessionNo = SWAP64(biSessionNo);

				VecSessionNoList.push_back(biSessionNo);
			}

			SendSynchronize(nCount, VecSessionNoList, cIsMonitoring);
		}
		break;
	}

	return ERROR_NONE;
}

void CDNNexonAuth::SendInitialize(int nDomainSn)
{
	BYTE cPacketType = AUTH_INITIALIZE;
	BYTE cInitializeType = 2;
	BYTE cGameSn = 46;
	BYTE cDomainSn = nDomainSn;

	char szDomainName[64] = {0,};
	// 일단 컴파일을 위해 임시로 막았습니다.
	//WideCharToMultiByte(CP_ACP, 0, g_Config.wszWorldName, -1, szDomainName, 64, NULL, NULL);
	BYTE cDomainNameLen = (BYTE)strlen(szDomainName);

	BYTE cSynchronizeType = 1;		// 0:동기화 안함 1:세션번호 동기화 2:세션번호, 마스터계정 아이디 동기화
	BYTE cUseSynchronize = 1;		// 0: 동기화 안함 1: 동기화 함
	int nSynchronizeCount = 100;	// 패킷당 동기화할 대상 세션수 (100 - 500 권장)
	BYTE cPropertyCount = 0;		// 확장속성개수 (없을경우 0)

	// Header
	BYTE cHeader = 0xAA;
	// cPacketType(BYTE) + cInitializeType(BYTE) + cGameSn(BYTE) + cDomainSn(BYTE) + cDomainNameLen(BYTE) + cDomainNameLen + cSynchronizeType(BYTE) + cUseSynchronize(BYTE) + nSynchronizeCount(int) + cPropertyCount(BYTE)
	USHORT wLength = (sizeof(BYTE) * 5) + cDomainNameLen + (sizeof(BYTE) * 2) + sizeof(int) + sizeof(BYTE);

	wLength = SWAP16(wLength);

	CByteStream Stream;

	Stream.Write(&cHeader, sizeof(BYTE));
	Stream.Write(&wLength, sizeof(USHORT));

	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&cInitializeType, sizeof(BYTE));
	Stream.Write(&cGameSn, sizeof(BYTE));
	Stream.Write(&cDomainSn, sizeof(BYTE));
	Stream.Write(&cDomainNameLen, sizeof(BYTE));
	Stream.Write(szDomainName, cDomainNameLen);
	Stream.Write(&cSynchronizeType, sizeof(BYTE));
	Stream.Write(&cUseSynchronize, sizeof(BYTE));
	nSynchronizeCount = SWAP32(nSynchronizeCount);
	Stream.Write(&nSynchronizeCount, sizeof(int));
	Stream.Write(&cPropertyCount, sizeof(BYTE));

	AddSendData(Stream.GetBuffer(), Stream.Size());

	g_Log.Log(LogType::_NORMAL, L"SendInitialize (Domain:%d)\r\n", cDomainSn);
}

void CDNNexonAuth::SendLogin(CDNUser *pUser)
{
	BYTE cPacketType = AUTH_LOGIN;
	BYTE cLoginType = 1;

	char szAccountName[64] = { 0, }, szCharName[64] = { 0, };
	if (pUser->GetAccountName()) WideCharToMultiByte(CP_ACP, 0, pUser->GetAccountName(), -1, szAccountName, 64, NULL, NULL);
	BYTE cAccountLen = (BYTE)strlen(szAccountName);	// UserID

	UINT nClientIp = (UINT)_inet_addr(pUser->GetIp());	// ClientIP

	BYTE cPropertyCount = 4;	// 1:CharName, 2:LocalIP, 5:MachineID, 17:PolicyResult
	USHORT wPropertyNo1 = 1;	// 1:CharName
	if (pUser->GetCharacterName()) WideCharToMultiByte(CP_ACP, 0, pUser->GetCharacterName(), -1, szCharName, 64, NULL, NULL);
	BYTE cCharLen = (BYTE)strlen(szCharName);
	
	USHORT wPropertyNo2 = 2;	// 2:LocalIP
	UINT nLocalIp = (UINT)_inet_addr(pUser->GetVirtualIp());

	USHORT wPropertyNo3 = 5;	// 5:MachineID
	USHORT wPropertyNo4 = 17;	// 17:PolicyResult
	BYTE cPolicyCount = 1;		// 적용정책 개수
	BYTE cPolicyNo = 10;		// 해당 사용자에 관련된 정책번호 - 게임이용차단정책

	// Header
	BYTE cHeader = 0xAA;
	// cPacketType(BYTE) + cLoginType(BYTE) + cAccountLen(BYTE) + cAccountLen + nClientIp(UINT) + cPropertyCount(BYTE)
	// + wPropertyNo1(USHORT) + cCharLen(BYTE) + cCharLen + wPropertyNo2(USHORT) + nLocalIp(UINT) + wPropertyNo3(USHORT) + MACHINEIDMAX + wPropertyNo4(USHORT) + cPolicyCount(BYTE) + cPolicyNo(BYTE)
	USHORT wLength = (sizeof(BYTE) * 7) + (sizeof(UINT) * 2) + (sizeof(USHORT) * 4) + cAccountLen + cCharLen + MACHINEIDMAX;	// 65536까지

	wLength = SWAP16(wLength);

	CByteStream Stream;

	Stream.Write(&cHeader, sizeof(BYTE));
	Stream.Write(&wLength, sizeof(USHORT));

	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&cLoginType, sizeof(BYTE));
	Stream.Write(&cAccountLen, sizeof(BYTE));
	Stream.Write(szAccountName, cAccountLen);		// UserID
	Stream.Write(&nClientIp, sizeof(UINT));			// ClientIP

	Stream.Write(&cPropertyCount, sizeof(BYTE));	// PropertyCount

	wPropertyNo1 = SWAP16(wPropertyNo1);			// PropertyNo (1:CharName)
	Stream.Write(&wPropertyNo1, sizeof(USHORT));
	Stream.Write(&cCharLen, sizeof(BYTE));
	Stream.Write(szCharName, cCharLen);				// CharName

	wPropertyNo2 = SWAP16(wPropertyNo2);			// PropertyNo (2:LocalIP)
	Stream.Write(&wPropertyNo2, sizeof(USHORT));
	Stream.Write(&nLocalIp, sizeof(UINT));			// LocalIp

	wPropertyNo3 = SWAP16(wPropertyNo3);			// PropertyNo (5:MachineID)
	Stream.Write(&wPropertyNo3, sizeof(USHORT));
	Stream.Write(pUser->m_szMID, MACHINEIDMAX);		// MachineID

	wPropertyNo4 = SWAP16(wPropertyNo4);			// PropertyNo (17:PolicyResult)
	Stream.Write(&wPropertyNo4, sizeof(USHORT));
	Stream.Write(&cPolicyCount, sizeof(BYTE));		// 적용정책개수
	Stream.Write(&cPolicyNo, sizeof(BYTE));			// 정책번호

	AddSendData(Stream.GetBuffer(), Stream.Size());

	g_Log.Log(LogType::_NORMAL, pUser, L"[A:%u (%s) C:%s] CDNNexonAuth::SendLogin (%S)\r\n", pUser->GetAccountDBID(), pUser->GetAccountName(), pUser->GetCharacterName(), pUser->GetIp());
}

void CDNNexonAuth::SendLogout(CDNUser *pUser)
{
	BYTE cPacketType = AUTH_LOGOUT;
	BYTE cLogoutType = 1;

	char szAccountName[64] = { 0, };
	if (pUser->GetAccountName()) WideCharToMultiByte( CP_ACP, 0, pUser->GetAccountName(), -1, szAccountName, 64, NULL, NULL );
	BYTE cAccountLen = (BYTE)strlen(szAccountName);	// UserID
	BYTE cPropertyCount = 0;

	// Header
	BYTE cHeader = 0xAA;
	// cPacketType(BYTE) + cLogoutType(BYTE) + cAccountLen(BYTE) + cAccountLen + biNexonSessionNo(INT64) + cPropertyCount(BYTE)
	USHORT wLength = (sizeof(BYTE) * 3) + cAccountLen + sizeof(INT64) + sizeof(BYTE);

	wLength = SWAP16(wLength);

	CByteStream Stream;

	Stream.Write(&cHeader, sizeof(BYTE));
	Stream.Write(&wLength, sizeof(USHORT));

	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&cLogoutType, sizeof(BYTE));

	Stream.Write(&cAccountLen, sizeof(BYTE));
	Stream.Write(szAccountName, cAccountLen);

	INT64 biNexon = SWAP64(pUser->m_biNexonSessionNo);
	Stream.Write(&biNexon, sizeof(INT64));
	Stream.Write(&cPropertyCount, sizeof(BYTE));

	AddSendData(Stream.GetBuffer(), Stream.Size());

	g_Log.Log(LogType::_NORMAL, pUser, L"[A:%u (%s)] CDNNexonAuth::SendLogout (%lld) %S\r\n", pUser->GetAccountDBID(), pUser->GetAccountName(), pUser->m_biNexonSessionNo, pUser->GetIp());
}

void CDNNexonAuth::SendSynchronize(int nCount, std::vector<INT64> &VecSessionList, BYTE cIsMonitoring)
{
	BYTE cPacketType = AUTH_SYNCHRONIZE;

	// Header
	BYTE cHeader = 0xAA;
	// cPacketType(BYTE) + cIsMonitoring(BYTE) + nCount(int) + (biSessionNo(INT64) * nCount) + (cSessionAlived(BYTE) * nCount)
	USHORT wLength = (USHORT)((sizeof(BYTE) * 2) + sizeof(int) + (sizeof(INT64) * nCount) + (sizeof(BYTE) * nCount));
	wLength = SWAP16(wLength);

	CByteStream Stream;

	Stream.Write(&cHeader, sizeof(BYTE));
	Stream.Write(&wLength, sizeof(USHORT));

	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&cIsMonitoring, sizeof(BYTE));

	int nTotalCount = SWAP32(nCount);
	Stream.Write(&nTotalCount, sizeof(int));

	CDNUser *pUser = NULL;
	BYTE cSessionAlived = 0;
	for (int i = 0; i < nCount; i++){
		cSessionAlived = 0;

		pUser = g_pDivisionManager->GetUserByNexonSessionNo(VecSessionList[i]);
		if (pUser)
			cSessionAlived = 1;

		INT64 biSessionNo = VecSessionList[i];
		biSessionNo = SWAP64(biSessionNo);
		Stream.Write(&biSessionNo, sizeof(INT64));
		Stream.Write(&cSessionAlived, sizeof(BYTE));
	}

	AddSendData(Stream.GetBuffer(), Stream.Size());

	g_Log.Log(LogType::_NORMAL, L"CDNNexonAuth::SendSynchronize Count:%d IsMonitoring:%d\r\n", nCount, cIsMonitoring);
}

void CDNNexonAuth::SendAlive()
{
	BYTE cHeader = 0xAA;
	USHORT wLength = (USHORT)sizeof(BYTE);
	wLength = SWAP16(wLength);
	BYTE cType = AUTH_ALIVE;

	CByteStream Stream;

	Stream.Write(&cHeader, sizeof(BYTE));
	Stream.Write(&wLength, sizeof(USHORT));
	Stream.Write(&cType, sizeof(BYTE));

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

#endif	// _KR