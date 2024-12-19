#pragma once

#include "LoginSendPacket.h"
//#include "ClientTcpSession.h"
#include "MessageListener.h"

class CLoginClientSession : public CTaskListener
{
public:
	CLoginClientSession();
	virtual ~CLoginClientSession();

protected:

public:
	// CClientTcpSession
	virtual void OnConnectTcp();
	virtual void OnDisconnectTcp( bool bValidDisconnect );
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	void RequestLogin( const TCHAR *szID, const TCHAR *szPass );

	void OnRecvLoginMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvSystemMessage( int nSubCmd, char *pData, int nSize );
#ifdef PRE_ADD_DOORS
	void OnRecvDoorsMessage(int nSubCmd, char *pData, int nSize);
#endif

	virtual void OnRecvLoginCheckLogin( SCCheckLogin *pPacket ) {}
	virtual void OnRecvLoginCheckVersion( SCCheckVersion *pPacket ) {}
	virtual void OnRecvLoginCharList( SCCharList *pPacket ) {}
	virtual void OnRecvLoginWaitUser( SCWaitUser *pPacket ) {}
	virtual void OnRecvLoginBlock( SCCHeckBlock *pPacket ) {}
	virtual void OnRecvLoginCharCreate( SCCreateChar *pPacket ) {}
	virtual void OnRecvLoginCharDelete( SCDeleteChar *pPacket ) {}
	virtual void OnRecvLoginServerList( SCServerList *pPacket ) {}
	virtual void OnRecvLoginChannelList( SCChannelList *pPacket ) {}
	virtual void OnRecvLoginBackButton() {}
	virtual void OnRecvLoginReviveChar( SCReviveChar *pPacket ) {}
#if defined (PRE_ADD_DWC)
	virtual void OnRecvDWCCharCreate(SCCreateDWCChar *pPacket) {}
	//virtual void OnRecvDWCChannelInfo(SCDWCChannelInfo *pPacket) {}
#endif		// PRE_ADD_DWC
#if defined (_TH)
	virtual void OnRecvLoginAsiaSoftReqOTP() {}
	virtual void OnRecvLoginAsiaSoftRetOTP(SCOTRPResult * pPacket) {}
#endif		//#if defined (_TH)

	virtual void OnRecvSystemGameInfo( SCGameInfo *pPacket ) {}
	virtual void OnRecvSystemVillageInfo( SCVillageInfo *pPacket ) {}
#ifdef PRE_ADD_DOORS
	virtual void OnRecvDoorsAuthKey(SCDoorsGetAuthKey * pPacket) {}
	virtual void OnRecvDoorsCancelAuth(SCDoorsCancelAuth * pPacket) {}
	virtual void OnRecvDoorsAuthFlag(SCDoorsGetAuthFlag* pData) {}
#endif		//#ifdef PRE_ADD_DOORS
};
