
#pragma once

typedef void (*GSM_PROC)();

struct TGSMCmd
{
	std::string strCmd;
	std::string strRespectCmd;
	ULONG nCmdTick;
	GSM_PROC pfCall;

	TGSMCmd()
	{
		nCmdTick = 0;
		pfCall = NULL;
	};
};

class CGsmCmdProcess
{
public:
	CGsmCmdProcess();
	virtual ~CGsmCmdProcess();

	static CGsmCmdProcess * GetInstance();

	void AddCmdProcess(const char * pCmd, const char * pRespectCmd, void * pf);
	void ParseCmdProcess(const char * pCmd);

protected:
	std::list <TGSMCmd> m_CmdList;

private:
	CSyncLock m_Sync;
	bool IsExistCmd(const char * pCmd);
};