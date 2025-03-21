#pragma once

#define CAMERA_FILE_STRING	"Eternity Engine Camera File 0.1"
#define CAMERA_FILE_VERSION	10

#define FOV_W 0	  // width-related FOV
#define FOV_H 1   // height-related FOV
#define FOV_D 2   // diagonal-related FOV

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
	Point3 vDefaultPosition;
	Quat qDefaultRotation;
	char szCameraName[ 32 ];
	char cReserved[ 1024 - 256 - sizeof( int ) * 7 - sizeof( Point3 ) - sizeof( Quat ) - 32 ];
};

struct SCameraPositionKey
{
	int nTime;
	Point3 Position;
};

struct SCameraRotationKey
{
	int nTime;
	Quat Rotation;
};

struct SCameraFOVKey
{
	int nTime;
	float fFOV;
};

class CSaveCamera
{
public:
	CSaveCamera(void);
	virtual ~CSaveCamera(void);

protected:
	INode *m_pNode;
	Interface *m_pMaxInterface;

	SCameraFileHeader m_Header;
	std::vector< SCameraPositionKey > m_vecPosition;
	std::vector< SCameraRotationKey > m_vecRotation;
	std::vector< SCameraFOVKey > m_vecFOV;

public:
	void ProcessCamera( FILE *fp, INode *pNode, Interface *pMaxInterface, bool bExportTargetCamera );
	void GetAniData();
	void DumpFOVSample();
	void DumpPosSample( INode *pNode, std::vector< SCameraPositionKey > &vecTargetPosition );
	void DumpRotSample();
	void WriteCameraInfo( FILE *fp );
};
