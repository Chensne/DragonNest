#pragma once

class RLKTAuth : public CSingleton<RLKTAuth>
{
public:
	RLKTAuth();
	~RLKTAuth();
	void Main();
	 bool isValidated();
};

