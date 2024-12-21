#pragma once

class CEtCustomMeshStream
{
public:
	CEtCustomMeshStream(void);
	virtual ~CEtCustomMeshStream(void);

protected:
	EtVertexBuffer	*m_pVB;
	char *m_pLockedVB;
	EtIndexBuffer	*m_pIB;
	WORD *m_pwLockedIB;

	int m_nIndexCount;
	int m_nVertexCount;
	int m_nStride;
	int m_nVertexBufferSize;
	int m_nIndexBufferSize;
	PrimitiveType m_Type;

public:
	void Clear();
	void MakeEmpty() { m_nVertexCount = 0; m_nIndexCount = 0; }
	int GetVertexCount() { return m_nVertexCount; }
	int GetIndexCount() { return m_nIndexCount; }
	void SetPrimitiveType( PrimitiveType Type ) { m_Type = Type; }
	PrimitiveType GetPrimitiveType() { return m_Type; }

	char *LockVB();
	void UnlockVB();
	bool IsLockedVB() { return m_pLockedVB != NULL; }
	void ReleaseVB();

	WORD *LockIB();
	void UnlockIB();
	bool IsLockedIB() { return m_pwLockedIB != NULL; }
	void ReleaseIB();

	void LoadVertexStream( CStream *pStream, int nVertexCount, int nStride);
	void AddVertexStream( CStream *pStream, int nVertexCount, int nStride);
	void LoadIndexStream( CStream *pStream, int nIndexCount );
	void AddIndexStream( CStream *pStream, int nIndexCount );

	void Draw( int nVertexDeclIndex, int nDrawStart = 0, int nDrawCount = 0 );
};
