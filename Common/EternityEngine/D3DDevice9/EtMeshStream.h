#pragma once
class CStream;

struct SStreamElement
{
	MeshStreamType Type;
	EtVertexBuffer *m_pVB;
	int nStride;
	int nStreamIndex;
};

class CEtMeshStream
{
public:
	CEtMeshStream();
	virtual ~CEtMeshStream();
	void Clear();

protected:
	std::vector< SStreamElement >	m_vecElements;
	EtIndexBuffer					*m_pIB;

	bool m_bStrip;
	bool m_bGenerateTangentSpace;
	int m_nIndexCount;
	int m_nVertexCount;

	bool m_bDeleteVB;
	bool m_bDeleteIB;

	EtUsage m_Usage;
	EtPool m_Pool;

	EtBaseTexture *m_pPointSpriteTexture;
	

public:
	int GetVertexCount() { return m_nVertexCount; }
	int GetIndexCount() { return m_nIndexCount; }

	void SetUsage( EtUsage Usage ) { m_Usage = Usage; }
	EtUsage GetUsage() { return m_Usage; }

	void SetPool( EtPool Pool ) { m_Pool = Pool; }
	EtPool GetPool() { return m_Pool; }

	bool IsStrip() { return m_bStrip; }
	void SetPointSpriteTexture( EtBaseTexture *pTexture ) { m_pPointSpriteTexture = pTexture;}

	EtVertexBuffer *FindVertexStream( MeshStreamType Type, int nStreamIndex, int &nStride );
	void CreateVertexStream( MeshStreamType Type, int nStreamIndex, int nVertexCount );
	void LoadVertexStream( CStream *pStream, MeshStreamType Type, int nStreamIndex, int nVertexCount );
	void LoadIndexStream( CStream *pStream, bool bStrip, int nIndexCount );
	int GetVertexStream( MeshStreamType Type, void *pDest, int nStreamIndex = 0 );
	void SetVertexStream( MeshStreamType Type, void *pSource, int nStreamIndex = 0 );
	void GetIndexStream( WORD *pDest );
	void SetIndexStream( WORD *pSource );

	void *Lock( MeshStreamType Type, int nStreamIndex, DWORD dwFlag, int &Stride );
	void Unlock( MeshStreamType Type, int nStreamIndex );

	void SetIndexBuffer( EtIndexBuffer *pBuffer, int nIndexCount );

	void Draw( int nVertexDeclIndex, int nDrawStart = 0, int nDrawCount = 0 );
	void DrawInstancing( int nVertexDeclIndex, EtVertexBuffer *pVB, int nStartIndex, int nInstanceCount );

	void GenerateTangentSpace();

	void Attach( CEtMeshStream *pMeshStream,  std::vector<int> &BoneRemapTable, EtVector4 ScaleBias, int nMergeOrder );
	void RemapBoneIndices( std::vector<int> &BoneRemapTable );
	void Assign( CEtMeshStream *pMeshStream, EtVector4 ScaleBias );

};
