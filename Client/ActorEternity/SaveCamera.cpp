#include "Stdafx.h"
#include "SaveCamera.h"
#include "DumpUtil.h"

CSaveCamera::CSaveCamera(void)
{
	m_pNode = NULL;
	m_pMaxInterface = NULL;
}

CSaveCamera::~CSaveCamera(void)
{
}

void CSaveCamera::ProcessCamera( FILE *fp, INode *pNode, Interface *pMaxInterface, bool bExportTargetCamera )
{
	m_pNode = pNode;
	m_pMaxInterface = pMaxInterface;

	Matrix3 LocalMat;
	TimeValue StartTime;
	AffineParts Affine;

	StartTime = m_pMaxInterface->GetAnimRange().Start();
	LocalMat = m_pNode->GetNodeTM( StartTime );
	LocalMat.PreRotateX( -90.0f * ( 3.141592654f / 180.0f ) );
	decomp_affine( LocalMat, &Affine );

	memset( &m_Header, 0, sizeof( SCameraFileHeader ) );

	m_Header.nType = CT_FREE_CAMERA;
	if( ( m_pNode->GetTarget() ) && ( bExportTargetCamera ) )
	{
		m_Header.nType = CT_TARGET_CAMERA;
	}
	strcpy( m_Header.szHeaderString, CAMERA_FILE_STRING );
	m_Header.nVersion = CAMERA_FILE_VERSION;
	m_Header.nFrame = m_pMaxInterface->GetAnimRange().End() / GetTicksPerFrame() - m_pMaxInterface->GetAnimRange().Start() / GetTicksPerFrame() + 1;

	int nStrlen;

	nStrlen = strlen( pNode->GetName() );
	if( nStrlen >= 32 )
	{
		memcpy( m_Header.szCameraName, pNode->GetName(), 31 );
		m_Header.szCameraName[ 31 ] = 0;
	}
	else
	{
		strcpy( m_Header.szCameraName, pNode->GetName() );
	}

	m_Header.vDefaultPosition.x = Affine.t.x;
	m_Header.vDefaultPosition.y = Affine.t.z;
	m_Header.vDefaultPosition.z = Affine.t.y;

	m_Header.qDefaultRotation.x = Affine.q.x;
	m_Header.qDefaultRotation.y = Affine.q.z;
	m_Header.qDefaultRotation.z = Affine.q.y;
	m_Header.qDefaultRotation.w = Affine.q.w;

	if( m_Header.nType == CT_TARGET_CAMERA )
	{
		LocalMat = m_pNode->GetTarget()->GetNodeTM( StartTime );
		LocalMat.PreRotateX( -90.0f * ( 3.141592654f / 180.0f ) );
		decomp_affine( LocalMat, &Affine );

		m_Header.vDefaultPosition.x = m_Header.vDefaultPosition.x - Affine.t.x;
		m_Header.vDefaultPosition.y = m_Header.vDefaultPosition.y - Affine.t.z;
		m_Header.vDefaultPosition.z = m_Header.vDefaultPosition.z - Affine.t.y;
	}

	GetAniData();
	WriteCameraInfo( fp );
}

void CSaveCamera::GetAniData()
{
	DumpFOVSample();
	DumpPosSample( m_pNode, m_vecPosition );
	DumpRotSample();
}

void CSaveCamera::DumpFOVSample() 
{
	int i, j;
	int nStart, nEnd;
	GenCamera *pObject;
	SCameraFOVKey FOVKey;
	float fFOVRatio;
	std::vector< SCameraFOVKey > vecFOV;

	pObject = ( GenCamera * )m_pNode->EvalWorldState( 0 ).obj;
	if( pObject->GetFOVType() == FOV_W )
	{
		fFOVRatio = m_pMaxInterface->GetRendImageAspect();
	}
	else
	{
		fFOVRatio = 1.0f;
	}

	nStart = m_pMaxInterface->GetAnimRange().Start() / GetTicksPerFrame();
	nEnd = m_pMaxInterface->GetAnimRange().End() / GetTicksPerFrame();
	nEnd++;

	m_Header.fDefaultFOV = pObject->GetFOV( m_pMaxInterface->GetAnimRange().Start() ) / fFOVRatio;
	for( i = nStart; i < nEnd; i++ )
	{
		FOVKey.nTime = i - nStart;
		FOVKey.fFOV = pObject->GetFOV( i * GetTicksPerFrame() ) / fFOVRatio;
		vecFOV.push_back( FOVKey );
	}

	m_vecFOV.push_back( vecFOV[ 0 ] );
	for( i = 0; i < ( int )vecFOV.size() - 2; i++ )
	{
		bool bIsInterpolate = true;
		for( j = i + 2; j < ( int )vecFOV.size(); j++ )
		{
			float fCalcFOV, fWeight;

			fWeight = ( j - i - 1 ) / ( float )( j - i );
			fCalcFOV = vecFOV[ i ].fFOV * ( 1.0f - fWeight ) + vecFOV[ j ].fFOV * fWeight;
			if( fabs( vecFOV[ j ].fFOV - fCalcFOV ) > 0.0001f )
			{
				bIsInterpolate = false;
				break;
			}
		}
		if( bIsInterpolate )
		{
			m_vecFOV.push_back( vecFOV[ vecFOV.size() - 1 ] );
			break;
		}
		else
		{
			m_vecFOV.push_back( vecFOV[ j - 1 ] );
			i += j - i - 2;
		}
	}
	/*if( m_vecFOV.size() == 2 )
	{
		if( m_vecFOV[ 0 ].fFOV == m_vecFOV[ 1 ].fFOV )
		{
			if( m_vecFOV[ 0 ].fFOV == m_Header.fDefaultFOV )
			{
				m_vecFOV.clear();
			}
		}
	}*/
}

void CSaveCamera::DumpPosSample( INode *pNode, std::vector< SCameraPositionKey > &vecTargetPosition ) 
{
	int i, j;
	int nStart, nEnd;
	GenCamera *pObject;
	SCameraPositionKey PosKey;
	std::vector< SCameraPositionKey > vecPos;

	pObject = ( GenCamera * )pNode->EvalWorldState( 0 ).obj;

	nStart = m_pMaxInterface->GetAnimRange().Start() / GetTicksPerFrame();
	nEnd = m_pMaxInterface->GetAnimRange().End() / GetTicksPerFrame();
	nEnd++;

	for( i = nStart; i < nEnd; i++ )
	{
		Matrix3 LocalMat;
		AffineParts Affine;

		LocalMat = pNode->GetNodeTM( i * GetTicksPerFrame() );
		LocalMat.PreRotateX( -90.0f * ( 3.141592654f / 180.0f ) );

		decomp_affine( LocalMat, &Affine );

		PosKey.nTime = i - nStart;
		PosKey.Position = Affine.t;
		if( m_Header.nType == CT_TARGET_CAMERA )
		{
			LocalMat = pNode->GetTarget()->GetNodeTM( i * GetTicksPerFrame() );
			LocalMat.PreRotateX( -90.0f * ( 3.141592654f / 180.0f ) );
			decomp_affine( LocalMat, &Affine );
			PosKey.Position = PosKey.Position - Affine.t;
		}
		vecPos.push_back( PosKey );
	}

	vecTargetPosition.push_back( vecPos[ 0 ] );
	for( i = 0; i < ( int )vecPos.size() - 2; i++ )
	{
		bool bIsInterpolate = true;
		for( j = i + 2; j < ( int )vecPos.size(); j++ )
		{
			Point3 CalcPos, PosDiff;
			float fWeight;

			fWeight = ( j - i - 1 ) / ( float )( j - i );
			CalcPos = vecPos[ i ].Position * ( 1.0f - fWeight ) + vecPos[ j ].Position * fWeight;
			PosDiff = vecPos[ j ].Position - CalcPos;
			if( PosDiff.Length() > 0.01f )
			{
				bIsInterpolate = false;
				break;
			}
		}
		if( bIsInterpolate )
		{
			vecTargetPosition.push_back( vecPos[ vecPos.size() - 1 ] );
			break;
		}
		else
		{
			vecTargetPosition.push_back( vecPos[ j - 1 ] );
			i += j - i - 2;
		}
	}
	if( vecTargetPosition.size() == 2 )
	{
		if( vecTargetPosition[ 0 ].Position == vecTargetPosition[ 1 ].Position )
		{
			if( vecTargetPosition[ 0 ].Position == m_Header.vDefaultPosition )
			{
				vecTargetPosition.clear();
			}
		}
	}
}

void CSaveCamera::DumpRotSample() 
{
	int i;
	int nStart, nEnd;
	GenCamera *pObject;
	SCameraRotationKey RotKey;
	std::vector< SCameraRotationKey > vecRot;

	pObject = ( GenCamera * )m_pNode->EvalWorldState( 0 ).obj;

	nStart = m_pMaxInterface->GetAnimRange().Start() / GetTicksPerFrame();
	nEnd = m_pMaxInterface->GetAnimRange().End() / GetTicksPerFrame();
	nEnd++;

	for( i = nStart; i < nEnd; i++ )
	{
		Matrix3 LocalMat;
		AffineParts Affine;

		LocalMat = m_pNode->GetNodeTM( i * GetTicksPerFrame() );
		LocalMat.PreRotateX( -90.0f * ( 3.141592654f / 180.0f ) );
		decomp_affine( LocalMat, &Affine );

		RotKey.nTime = i - nStart;
		RotKey.Rotation = Affine.q;
		m_vecRotation.push_back( RotKey );
	}
}

void CSaveCamera::WriteCameraInfo( FILE *fp )
{
	int i;

	m_Header.nFOVKeyCount = ( int )m_vecFOV.size();
	m_Header.nPositionKeyCount = ( int )m_vecPosition.size();
	m_Header.nRotationKeyCount = ( int )m_vecRotation.size();

	fwrite( &m_Header, sizeof( SCameraFileHeader ), 1, fp );
	for( i = 0; i < m_Header.nFOVKeyCount; i++ )
	{
		fwrite( &m_vecFOV[ i ], sizeof( SCameraFOVKey ), 1, fp );
	}
	for( i = 0; i < m_Header.nPositionKeyCount; i++ )
	{
		fwrite( &m_vecPosition[ i ].nTime, sizeof( int ), 1, fp );
		DumpPoint3( fp, m_vecPosition[ i ].Position );
	}
	for( i = 0; i < m_Header.nRotationKeyCount; i++ )
	{
		fwrite( &m_vecRotation[ i ].nTime, sizeof( int ), 1, fp );
		DumpQuat( fp, m_vecRotation[ i ].Rotation );
	}
}
