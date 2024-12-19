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

protected:

public:
	void LoadSubMesh( CStream *pStream );
};
