#pragma once

#define SUBMESH_HEADER_RESERVED	( 1024 - sizeof( SSubMeshHeader ) )
struct SSubMeshHeader
{
	char szParentName[ 256 ];
	char szName[ 256 ];
	int nVertexCount;
	int nFaceBufferSize;
	int nTextureChannel;
	bool bUseStrip;
	bool bUsePhysique;
	bool bUseVertexColor;
};

class CEtSubMesh
{
public:
	CEtSubMesh();
	virtual ~CEtSubMesh();
	void Clear();

protected:
	SSubMeshHeader m_SubMeshHeader;
	CEtMeshStream m_MeshStream;
	std::vector< std::string > m_vecBoneName;
	std::vector< int > m_vecLinkIndex;
	int m_nParentBoneIndex;

	EtVector3 *m_pPosition;
	WORD *m_pIndex;

public:
	const char *GetSubMeshName() { return m_SubMeshHeader.szName; }
	int GetLinkCount() { return ( int )m_vecLinkIndex.size(); }
	const int *GetLinkIndex() { return &m_vecLinkIndex[ 0 ]; }
	int GetParentBoneIndex() { return m_nParentBoneIndex; }
	void LinkToAni( EtResourceHandle hAni );

	void LoadSubMesh( CStream *pStream );

	void Draw( int nVertexDeclIndex, int nDrawStart = 0, int nDrawCount = 0 );

	bool TestLineToTriangle( EtVector3 &vPosition, EtVector3 &vDirection, EtMatrix &TransMat, float &fDist );

	bool IsExistStream( MeshStreamType Type, int nStreamIndex );
	int GetVertexCount() { return m_SubMeshHeader.nVertexCount; }
	int GetVertexStream( MeshStreamType Type, void *pBuffer ) { return m_MeshStream.GetVertexStream( Type, pBuffer ); }
	void SetVertexStream( MeshStreamType Type, void *pBuffer ) { m_MeshStream.SetVertexStream( Type, pBuffer ); }

	void GenerateTangentSpace() { m_MeshStream.GenerateTangentSpace(); }

	CEtMeshStream *GetMeshStream() { return &m_MeshStream; }

	std::vector< std::string > & GetBoneName() {return m_vecBoneName;};
	void RemoveLinkInfo();

	void Assign( CEtSubMesh *pSubMesh, EtVector4 ScaleBias );
};
