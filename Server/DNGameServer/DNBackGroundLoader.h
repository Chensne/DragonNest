#pragma once

#include <vector>
#include <queue>
#include "MultiCommon.h"
#include "thread.h"
#include "DNGameRoom.h"
#include "CriticalSection.h"

class CDNBackGroundLoader : public Thread
{
public:
	CDNBackGroundLoader( bool bLog=true );
	~CDNBackGroundLoader();

	bool PushToLoadProcess(CDNGameRoom * pRoom, bool bContinue = false);
#if defined( PRE_THREAD_ROOMDESTROY )
	bool PushToDestroyProcess( CDNGameRoom* pRoom );
	void DestroyConfirm( int idx );
#endif // #if defined( PRE_THREAD_ROOMDESTROY )
	bool IsLoaded(int idx);
	bool IsLoading( int idx );
	bool LoadConfirm(int idx);
	bool LoadCancel(int idx);

private:

	class CScopeNowLoading
	{
		public:

			CScopeNowLoading( int* p, int iIdx ):m_p(p)
			{
				(*m_p) = iIdx;
			}
			~CScopeNowLoading()
			{
				(*m_p) = -1;
			}
		private:
			int* m_p;
	};

	void Run();
	
	bool m_bLog;
	int m_inowLoading;
	std::queue <int> m_CallList;
	CSyncLock m_CallSync;
	static HANDLE m_hCallSignal;

	struct _STANDBY_ROOM
	{
		bool volatile bIsLoaded;
		bool bIsContinue;
		bool bIsInitFail;
		CDNGameRoom * pRoom;
#if defined( PRE_THREAD_ROOMDESTROY )
		bool bIsDestroyProcess;
#endif // #if defined( PRE_THREAD_ROOMDESTROY )

		_STANDBY_ROOM()
		{
			bIsLoaded = false;
			bIsContinue = false;
			bIsInitFail = false;
			pRoom = NULL;
#if defined( PRE_THREAD_ROOMDESTROY )
			bIsDestroyProcess = false;
#endif // #if defined( PRE_THREAD_ROOMDESTROY )
		}
	};
	_STANDBY_ROOM * m_pStandbyList;
};

extern CDNBackGroundLoader * g_pBackLoader;