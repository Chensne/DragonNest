#pragma once
#include "DNDefine.h"

#include "DnServiceSetup.h"
#include "DnPacket.h"


#if defined(_JP) && defined(_AUTH)

class DnNHNService :public IServiceSetup
{
public:
	DnNHNService(void);
	~DnNHNService(void);
	
	enum eNHNEventCode
	{
		GameStart = 300, 
		GameExit = 301 , 
		JoinChannel = 301, 
		Server = 302,
		character = 303, 
		channel = 304 , 
		InVillage = 305, 
		InGame = 306,
	};



	virtual int	PreInitialize(void* pCustomData);
	virtual int	Initialize(void* pCustomData);
	virtual LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual int OnEvent(int nType, void* pExtra);
	virtual int Release();

	virtual void OnDispatchMessage(int iMainCmd, int iSubCmd, char * pData, int iLen);
	
	virtual bool WriteStageLog_( INT32 nStage , LPCTSTR szComment );
	
	int ErrorMsg(int ret);
	int ErrorInfoMsg(int ret);
	
	bool HanAuthInit();
	bool HanReportInit();
	
	void HanSendReport(eNHNEventCode EventCode);
	void FileLog(char *str , ...);

	Partner::Type::eType GetPartnerType() { return m_PartnerType; }
private:

	std::string m_szGameString;
	std::string m_szAuthString; 
	std::string m_szUserID;
	std::string m_szMemberID; 
	std::string m_szIdMd5; 
	std::string m_szSiteCode;
	Partner::Type::eType m_PartnerType;

	HMODULE m_Module;
};


#endif 