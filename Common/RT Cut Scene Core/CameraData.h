#pragma once

#define CAMERA_FILE_STRING	"Eternity Engine Camera File 0.1"
#define CAMERA_FILE_VERSION	10
#define CAMERA_HEADER_RESERVED	( 1024 - sizeof( SCameraFileHeader ) )

enum ExportCameraType
{
	CT_FREE_CAMERA = 0,
	CT_TARGET_CAMERA = 1,
};

struct SCameraFileHeader
{
	char szHeaderString[ 256 ];
	int nVersion;
	int nFrame;
	int nType;
	int nFOVKeyCount;
	int nPositionKeyCount;
	int nRotationKeyCount;
	float fDefaultFOV;
	EtVector3 vDefaultPosition;
	EtQuat qDefaultRotation;
	char szCameraName[ 32 ];
};

struct SCameraFOVKey
{
	int nTime;
	float fFOV;
};

struct SCameraPositionKey
{
	int nTime;
	EtVector3 vPosition;
};

struct SCameraRotationKey
{
	int nTime;
	EtQuat qRotation;
};

class CCameraData
{
public:
	CCameraData(void);
	virtual ~CCameraData(void);

public:
	SCameraFileHeader m_Header;
	std::vector< SCameraFOVKey > m_vecFOV;
	std::vector< SCameraPositionKey > m_vecPosition;
	std::vector< SCameraRotationKey > m_vecRotation;

public:
	void LoadResource( CStream *pStream );
	void Clear( void );
};
