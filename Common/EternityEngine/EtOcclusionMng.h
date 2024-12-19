#pragma once

class CEtOcclusionMng
{
public:
	CEtOcclusionMng(void);
	virtual ~CEtOcclusionMng(void);

protected:
	std::vector< CEtOcclusionQuery * > m_vecOcclusionQuery;
	int m_nAllocCount;

public:
	int AllocOcclusion();
	void ClearOcclusion() { m_nAllocCount = 0; }

	void BeginOcclusion( int nIndex );
	void EndOcclusion( int nIndex );
	DWORD ResultOcclusion( int nIndex );

	void FlushBuffer();
};
