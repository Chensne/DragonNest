#pragma once

#include "Task.h"
#include "MessageListener.h"

class CDnGameProfileOptDlg;
class CDnGameOptionTask : public CTask, public CTaskListener, public CSingleton<CDnGameOptionTask>
{
public:
	CDnGameOptionTask();
	~CDnGameOptionTask();

	void OnDisconnectTcp(bool bValidDisconnect) {}
	void OnDisconnectUdp(bool bValidDisconnect) {}
	void OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize);

	bool Initialize() {return true;}

	void ReqSendSetGameOption( TGameOptions &pOption );
	void ReqSendUserComm( UINT nSessionID );
	void ReqSendKeySetting();
	void ReqSendJoypadSetting();

	void SetProfileDialog( CDnGameProfileOptDlg *pDialog ) { m_pProfileDialog = pDialog; }

protected:
	void OnRecvGameOptionMessage(int nSubCmd, char * pData, int nSize);

	//Recv
	void OnRecvSystemCharCommOption(SCGameOptionCommOption * pPacket);
	void OnRecvRefreshNotifier( SCGameOptionRefreshNotifier *pPacket );
	void OnRecvSelectKeySetting( SCGameOptionSelectKeySetting *pPacket );
	void OnRecvSelectPadSetting( SCGameOptionSelectPadSetting *pPacket );
	void OnRecvGetProfile( SCGameOptionGetProfile *pPacket );
	void OnRecvDisplayProfile( SCGameOptionDisplayProfile *pPacket );

	CDnGameProfileOptDlg *m_pProfileDialog;
};

#define GetGameOptionTask()	CDnGameOptionTask::GetInstance()