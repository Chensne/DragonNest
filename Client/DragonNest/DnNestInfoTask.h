#pragma once

#include "Task.h"
#include "MessageListener.h"
#include "DnWorld.h"

class CDnNestInfoTask : public CTask, public CTaskListener, public CSingleton<CDnNestInfoTask> {
public:
	CDnNestInfoTask();
	virtual ~CDnNestInfoTask();

	struct NestDungeonInfoStruct 
	{
		CDnWorld::MapSubTypeEnum SubType;
		int		nMapTableID;
		tstring	szMapName;
		int		nMaxTryCount;
		int		nTryCount;
		bool	bExpandable;
		int		nEnterTableID;
		bool	bPCBangAdd;				// PC방 추가 네스트 인가?
		int		nPCBangAddCount;		// PC방 추가 네스트 횟 수
		int		nPCBangClearCount;		// PC방 추가 네스트 클리어 횟 수
	};

protected:
	std::vector<NestDungeonInfoStruct *> m_pVecNestList;
	int m_nExpandTryCount;
	bool m_bInitPCBangNestInfo;

protected:
	void Refresh();

	virtual void OnRecvCharMessage( int nSubCmd, char *pData, int nSize );
	int CheckPCBangAddNestCount( int nMapTableID );

public:
	bool Initialize();
	void Finalize();

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	virtual void OnRecvCharNestClear( SCNestClear *pPacket );
	virtual void OnRecvCharUpdateNestClear( SCUpdateNestClear *pPacket );
	

	DWORD GetNestDungeonCount();
	NestDungeonInfoStruct *GetNestDungeonInfo( DWORD dwIndex );
	NestDungeonInfoStruct *FindNestDungeonInfoFromMapIndex( int nMapIndex );
	void ResetAllTryCount();

	void RefreshExpandTryCount();
	int GetExpandTryCount() { return m_nExpandTryCount; }

	void RefreshNestDungeonList();
	void SetPCBangNestCountInfo();

};
