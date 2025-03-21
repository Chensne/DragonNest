#pragma once

class CEtBackBufferMng
{
public:
	CEtBackBufferMng(void);
	virtual ~CEtBackBufferMng(void);
	void Clear();

protected:
	EtTextureHandle m_hBackBuffer;
	std::vector< EtTextureHandle > m_UserBackBuffers;

public:
	void DumpBackBuffer();
	void DumpUserBackBuffer( int nIndex );

	void CreateBackBuffer();
	int CreateUserBackBuffer();
	void ReleaseUserBackBuffer( int nIndex );

	int GetBackBufferIndex();
	EtTextureHandle GetBackBuffer();
	EtTextureHandle SetBackBuffer( EtTextureHandle hNewBackBuffer );
	EtTextureHandle GetUserBackBuffer( int nIndex	);
};

extern CEtBackBufferMng g_EtBackBufferMng;
inline CEtBackBufferMng *GetEtBackBufferMng() { return &g_EtBackBufferMng; }