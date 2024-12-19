
#include "Stdafx.h"
#include "Log.h"
#include "LivePatcher.h"
#include "ServiceServer.h"

extern TServiceManagerConfig g_Config;

CLivePatcher::CLivePatcher()
{
	ScopeLock <CSyncLock> sync(m_Sync);
	m_bPatching = false;
	m_PatchList.clear();
}

CLivePatcher::~CLivePatcher()
{
}

bool CLivePatcher::StartPatch()
{
	ScopeLock <CSyncLock> sync(m_Sync);
	if (m_bPatching || !m_PatchList.empty())
		return false;

	m_PatchList.clear();
	m_bPatching = true;
	return true;
}

bool CLivePatcher::CancelPatch(bool bForce)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	
	if (bForce == false)
	{
		if (IsReloading())
		{
			g_Log.Log(LogType::_ERROR, L"Now Reload Ext Cancel Failed\n");
			return false;
		}
	}

	m_bPatching = false;
	m_PatchList.clear();
	m_ReloadList.clear();
	return true;
}

void CLivePatcher::SetReloadFlag(std::vector <int> &vList)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	TLiveReloadItem item;
	for (int i = 0; i < (int)vList.size(); i++)
	{
		memset(&item, 0, sizeof(TLiveReloadItem));

		item.nMID = vList[i];

		m_ReloadList.push_back(item);
	}
}

bool CLivePatcher::SetReloadResult(int nMID, bool bRet, bool &bReloadRet)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector <TLiveReloadItem>::iterator ii;
	for (ii = m_ReloadList.begin(); ii != m_ReloadList.end(); ii++)
	{
		if ((*ii).nMID == nMID)
		{
			(*ii).bReload = bRet;
			(*ii).bReported = true;
		}
	}

	bool bWholeReported = true;
	bReloadRet = true;
	for (ii = m_ReloadList.begin(); ii != m_ReloadList.end(); ii++)
	{
		if ((*ii).bReported == false)
			bWholeReported = false;
		if ((*ii).bReload == false)
			bReloadRet = false;
	}

	if (bWholeReported)
		CancelPatch();

	return bWholeReported;
}

bool CLivePatcher::IsReloading()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (m_ReloadList.empty())
		return false;
	return true;
}

bool CLivePatcher::LoadPatchList(TCHAR * pFile)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	if (pFile)			//파일명 변경해야함
	{
		TLivePatchItem Item;
		memset(&Item, 0, sizeof(TLivePatchItem));

		std::vector<std::wstring> tokens;
		TokenizeW(pFile, tokens, L",");

		std::vector<std::wstring>::iterator ii;
		for (ii = tokens.begin(); ii != tokens.end(); ii++)
		{
			memset(&Item, 0, sizeof(TLivePatchItem));

			Item.nWatchItemID = -1;
			Item.wstrItemName = g_Config.wszPatchBaseURL;
			Item.wstrItemName += (*ii);

			m_PatchList.push_back(Item);
		}
		return true;
	}	
	return false;
}

void CLivePatcher::SendLivePatch()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector<TLivePatchItem>::iterator ii;
	for (ii = m_PatchList.begin(); ii != m_PatchList.end(); ii++)
	{
		int nPatchID = g_pServiceManager->SendPatchEachItem(0, (*ii).wstrItemName.c_str(), L"ext");
		if (nPatchID >= 0)
			(*ii).nWatchItemID = nPatchID;
		else
			g_Log.Log(LogType::_ERROR, L"LivePatch PatchItem Send Error\n");
	}
}

bool CLivePatcher::RecvCompleteItem(int nPatchID)
{
	if (m_PatchList.empty() || m_bPatching == false) return false;

	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector<TLivePatchItem>::iterator ii;
	for (ii = m_PatchList.begin(); ii != m_PatchList.end(); ii++)
	{
		if ((*ii).nWatchItemID == nPatchID)
			(*ii).bPatched = true;
	}

	int nPatchedCount = 0;
	for (ii = m_PatchList.begin(); ii != m_PatchList.end(); ii++)
	{
		if ((*ii).bPatched)
			nPatchedCount++;
	}

	return m_PatchList.size() == nPatchedCount ? true : false;
}