#pragma once

class CEtEngine;
extern CEtEngine g_EtEngine;
inline CEtEngine *GetEtEngine() { return &g_EtEngine; }

class CEtEngine : public CSingleton< CEtEngine > 
{
public:
	CEtEngine();
	virtual ~CEtEngine();

protected:

public:
	void Initialize( CMultiRoom *pRoom );
	void Finalize( CMultiRoom *pRoom );
};
