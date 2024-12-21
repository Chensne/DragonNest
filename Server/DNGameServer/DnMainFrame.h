#pragma once

#include "Timer.h"
#include "Singleton.h"

class CInputDevice;
class CTaskManager;
class CDnTableDB;
class CTask;
class CEtSoundEngine;
class CClientSessionManager;
class CDnActionBase;
class CDnWorldImp;
class CEtResourceMng;

class CDnMainFrame : public CSingleton<CDnMainFrame> {
public:
	CDnMainFrame();
	~CDnMainFrame();

	// Window 생성 전에 할것들 Hacking tool 관련 체크등등..
	bool PreInitialize();

	// Engine 생성
	bool InitializeDevice();
	bool FinalizeDevice();

	bool PreloadResource();

	// 게임 생성
	bool Initialize();
	bool Finalize();

	//SetMultiCommmon
	void InitializeMultiCommon(int iCnt, std::vector <unsigned int> * vList);			//쓰레드 갯수를 세팅하고 ID를 컨테이너에 담아줘요호~
	void SetMultiThreadID(unsigned int iID);														//

	// Kernel 메인 루프

protected:
	CDnTableDB *m_pTableDB;
	CEtResourceMng *m_pResMng;
	CEtUIXML *m_pUIXML;
#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
	CEtExceptionalUIXML* m_pExceptionalUIXML;
#endif // PRE_ADD_EXCEPTIONAL_MAKESTRING

	std::map<int, std::string> m_mapCodePageString;

	std::vector<EtResourceHandle> m_hVecEngineResList;
	std::vector<CDnActionBase *> m_pVecActionList;
	std::vector<CDnWorldImp *> m_pVecWorldList;

protected:
	void InitializeLocalize();
	void PreloadSkinFile( int nTableID, char *szLable );
	void PreloadAniFile( int nTableID, char *szLable );
	void PreloadActFile( int nTableID, char *szLable );
	void FinalizePreloadFile();

	void PreloadSkinFile( const char *szFileName );
	void PreloadAniFile( const char *szFileName );
	void PreloadActFile( const char *szFileName );

protected:
};