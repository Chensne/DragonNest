#pragma once

class CEtSaveMat;
extern CEtSaveMat g_EtSaveMat;
inline CEtSaveMat *GetEtSaveMat() { return &g_EtSaveMat; }

class CEtSaveMat
{
public:
	CEtSaveMat() { m_nCurrentStartIndex = 0; m_bSaveTransMat = true; }
	virtual ~CEtSaveMat() {}

protected:
	int m_nCurrentStartIndex;
	std::vector< EtMatrix > m_vecSaveMat;
	std::vector< EtMatrix > m_vecTransMat;
	std::vector< int > m_vecTransCount;

	bool m_bSaveTransMat;

	static CSyncLock s_SaveMatLock;

public:
	void Clear();
	int LockMatrix( int nCount, EtMatrix **pLockedMatrix );
	void PushBack( EtMatrix &SaveMatrix ) { m_vecSaveMat.push_back( SaveMatrix ); }
	EtMatrix *GetMatrix( int nIndex ) { return &m_vecSaveMat[ nIndex - m_nCurrentStartIndex ]; }
	void Transform( int nIndex, EtMatrix &Mat );
	EtMatrix *GetTransMatrix( int nIndex ) { return &m_vecTransMat[ nIndex - m_nCurrentStartIndex ]; }
	bool IsValidIndex( int nIndex );
	void EnableSaveTransMat( bool bSave ) { m_bSaveTransMat = bSave; }

	void FlushBuffer();
};
