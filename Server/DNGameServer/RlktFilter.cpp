#include "stdafx.h"
#include "Singleton.h"
#include "RlktFilter.h"

void RlktFilter::Block(unsigned long *nIP)
{
	printf("BLOCK\n");
	if (nIP == NULL) return;
	if (m_vecBlockList.empty()) return;
	std::vector<unsigned long>::iterator it;
	it = find(m_vecBlockList.begin(), m_vecBlockList.end(), *nIP);
	if (it != m_vecBlockList.end()) return;

	m_vecBlockList.push_back(*nIP);

	struct in_addr ip_addr;
	ip_addr.s_addr = (u_long)nIP;
	printf("[][][]  BANIP: %s invalid shit\n", inet_ntoa(ip_addr));
}

bool RlktFilter::CheckBlockIP(unsigned long *nIP)
{
	if (nIP == NULL) return false;
	if (m_vecBlockList.empty()) return false;
	std::vector<unsigned long>::iterator it;
	it = find(m_vecBlockList.begin(), m_vecBlockList.end(), *nIP);
	if (it != m_vecBlockList.end()) return true;

	return false;
}