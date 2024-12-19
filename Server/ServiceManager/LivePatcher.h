
#pragma once

//Resource LivePatch를 하기위한 관리 클래스

struct TLivePatchItem
{
	bool bIsComplete;
	int nWatchItemID;
	bool bPatched;
	std::wstring wstrItemName;
};

struct TLiveReloadItem
{
	bool bReload;
	bool bReported;
	int nMID;
};

class CLivePatcher
{
public:
	CLivePatcher();
	~CLivePatcher();

	bool StartPatch();
	bool CancelPatch(bool bForce = true);
	bool IsStarted() { return m_bPatching; }	

	void SetReloadFlag(std::vector <int> &vList);
	bool SetReloadResult(int nMID, bool bRet, bool &bReloadRet);
	bool IsReloading();

	bool LoadPatchList(TCHAR * pFile);
	void SendLivePatch();

	bool RecvCompleteItem(int nPatchID);

private:
	bool m_bPatching;
	std::vector <TLivePatchItem> m_PatchList;
	std::vector <TLiveReloadItem> m_ReloadList;
	CSyncLock m_Sync;
};