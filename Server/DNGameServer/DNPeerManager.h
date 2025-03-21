#pragma once

#include <map>

class CDNPeerManager
{
public:
	void ConnectPeer(unsigned int iSender, unsigned int iReciever);
	void DisConnectPeer(unsigned int iSender, unsigned int iReciever);
	bool IsConnectedPeer(unsigned int iSender, unsigned int iReciever);
	
private:
	std::multimap<unsigned int, unsigned int> m_PeerCon;
};