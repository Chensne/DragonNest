
#include "Stdafx.h"
#include "DNIsolate.h"

CDNIsolate::CDNIsolate(CDNUserSendManager * pManager)
{
	m_pSendManager = pManager;
	m_IsolateList.clear();
}

CDNIsolate::~CDNIsolate()
{
	m_pSendManager = NULL;

	std::map <std::wstring, TIsolateItem*>::iterator ii;
	for (ii = m_IsolateList.begin(); ii != m_IsolateList.end(); ii++)
		SAFE_DELETE((*ii).second);
}

bool CDNIsolate::AddIsolateItem(const TIsolateItem * pItem)
{
	std::map <std::wstring, TIsolateItem*>::iterator ii = m_IsolateList.find(pItem->wszIsolateName);
	if (ii == m_IsolateList.end())
	{
		TIsolateItem * pIsolate = new TIsolateItem;

		pIsolate->biIsolateCharacterDBID = pItem->biIsolateCharacterDBID;
		_wcscpy(pIsolate->wszIsolateName, _countof(pIsolate->wszIsolateName), pItem->wszIsolateName, (int)wcslen(pItem->wszIsolateName));

		m_IsolateList[pIsolate->wszIsolateName] = pIsolate;
		return true;
	}
	return false;
}

bool CDNIsolate::DelIsolateItem(const WCHAR * pDelitem)
{
	std::map <std::wstring, TIsolateItem*>::iterator ii = m_IsolateList.find(pDelitem);
	if (ii != m_IsolateList.end())
	{
		m_IsolateList.erase(ii);
		return true;
	}
	return false;
}

bool CDNIsolate::DelIsolateItem(INT64 biCharacterDBID)
{
	std::map <std::wstring, TIsolateItem*>::iterator ii;
	for (ii = m_IsolateList.begin(); ii != m_IsolateList.end(); ii++)
	{
		if ((*ii).second->biIsolateCharacterDBID == biCharacterDBID)
		{
			m_IsolateList.erase(ii);
			return true;
		}
	}
	return false;
}

bool CDNIsolate::IsIsolateItem(const WCHAR * pName)
{
	std::map <std::wstring, TIsolateItem*>::iterator ii = m_IsolateList.find(pName);
	if (ii != m_IsolateList.end())
		return true;
	return false;
}

INT64 CDNIsolate::GetIsolateCharDBID(const WCHAR * pName)
{
	std::map <std::wstring, TIsolateItem*>::iterator ii = m_IsolateList.find(pName);
	if (ii != m_IsolateList.end())
		return (*ii).second->biIsolateCharacterDBID;
	return 0;
}

bool CDNIsolate::GetIsolateChrName(INT64 biIsolateDBID, WCHAR * pName)
{
	std::map <std::wstring, TIsolateItem*>::iterator ii;
	for (ii = m_IsolateList.begin(); ii != m_IsolateList.end(); ii++)
	{
		if ((*ii).second->biIsolateCharacterDBID == biIsolateDBID)
		{
			_wcscpy(pName, NAMELENMAX, (*ii).second->wszIsolateName, (int)wcslen((*ii).second->wszIsolateName));
			return true;
		}
	}
	return false;
}

void CDNIsolate::GetIsolateList(SCIsolateList * pPacket, int &nSize)
{
	std::map <std::wstring, TIsolateItem*>::iterator ii;
	for (ii = m_IsolateList.begin(); ii != m_IsolateList.end(); ii++)
	{
		pPacket->cNameLen[pPacket->cCount] = (BYTE)wcslen((*ii).second->wszIsolateName);
		wmemcpy_s( pPacket->wszIsolateName+nSize, _countof(pPacket->wszIsolateName)-nSize, (*ii).second->wszIsolateName, pPacket->cNameLen[pPacket->cCount]);
		nSize += pPacket->cNameLen[pPacket->cCount];// * sizeof(WCHAR);
		pPacket->cCount++;
	}
	nSize = nSize * sizeof(WCHAR);
}