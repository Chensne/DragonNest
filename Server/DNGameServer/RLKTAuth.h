#pragma once

extern bool isConnected;
extern bool isLicenseValid;

class RLKTAuth : public CSingleton<RLKTAuth>
{
public:
	RLKTAuth();
	~RLKTAuth();
	void Main();
	//bool isValidated();
};


inline bool isValidated()
{
	

	return true;
}