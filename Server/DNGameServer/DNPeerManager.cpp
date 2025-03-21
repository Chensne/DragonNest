
#include "stdafx.h"
#include "DNPeerManager.h"

void CDNPeerManager::ConnectPeer(unsigned int iSender, unsigned int iReciever)
{
	_ASSERT(IsConnectedPeer(iSender, iReciever) == false);
	m_PeerCon.insert(std::make_pair(iSender, iReciever));
}

void CDNPeerManager::DisConnectPeer(unsigned int iSender, unsigned int iReciever)
{
	std::multimap<unsigned int, unsigned int>::iterator ii;
	for(ii = m_PeerCon.lower_bound(iSender); ii != m_PeerCon.upper_bound(iSender); ii++)
	{
		if (((*ii).second == iReciever))
		{
			m_PeerCon.erase(ii);
			break;
		}
	}
	_ASSERT(IsConnectedPeer(iSender, iReciever) == false);
}

bool CDNPeerManager::IsConnectedPeer(unsigned int iSender, unsigned int iReciever)
{
	std::multimap<unsigned int, unsigned int>::iterator ii;
	for(ii = m_PeerCon.lower_bound(iSender); ii != m_PeerCon.upper_bound(iSender); ii++)
		if (((*ii).second == iReciever))
			return true;
	return false;
}