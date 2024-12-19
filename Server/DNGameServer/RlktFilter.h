#pragma once

class RlktFilter : public CSingleton <RlktFilter>
{

private:
	//security block ddos?
	std::vector<unsigned long> m_vecBlockList;
public:
	void Block(unsigned long  *szIP);
	bool CheckBlockIP(unsigned long *szIP);
};