#pragma once

class CEtWorldSector;
class CDNUserSession;

class CEtWorldProp
{
public:
	CEtWorldProp();
	virtual ~CEtWorldProp();

	struct PropStruct {
		char szPropName[64];
		EtVector3 vPosition;
		EtVector3 vRotation;
		EtVector3 vScale;
		bool bCastShadow;
		bool bReceiveShadow;
		int nUniqueID;
		bool bIgnoreBuildNavMesh;
		bool bControlLock;
		bool bIgnoreBuildColMesh;
		int nNavType;
		char cDummy[497];
	};

protected:
	int m_nClassID;
	std::string m_szPropName;
	EtAniObjectHandle m_Handle;

	EtVector3 m_vPosition;
	EtVector3 m_vRotation;
	EtVector3 m_vScale;
	EtVector3 m_vPrePosition;		// 수정
	EtVector3 m_vPreRotation;		// 수정

	bool m_bCastShadow;
	bool m_bReceiveShadow;
	bool m_bCastLightmap;
	bool m_bLightmapInfluence;
	int m_nCreateUniqueID;
	bool m_bPostCreateCustomParam;
	bool m_bIgnoreBuildNavMesh;
	bool m_bIgnoreBuildColMesh;
	bool m_bCastWater;
	bool m_bControlLock;
	int m_nNavType;

	void *m_pData;
	std::vector<int> m_nVec2Index;
	std::vector<int> m_nVec3Index;
	std::vector<int> m_nVec4Index;
	std::vector<int> m_nVecStrIndex;
#ifdef WIN64
	std::vector<int> m_nVec2Index64;
	std::vector<int> m_nVec3Index64;
	std::vector<int> m_nVec4Index64;
	std::vector<int> m_nVecStrIndex64;
#endif

	CEtWorldSector *m_pParentSector;
	CEtOctreeNode<CEtWorldProp *> *m_pCurOctreeNode;

protected:
	void ReleaseCustomInfo();
#ifdef WIN64
	char IsPointerTable( int nIndex );
	void IncreasePointerTableIndex( int nValue );
#endif
	virtual void* AllocPropData(int& usingCount);

public:
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

	const char *GetPropName() { return m_szPropName.c_str(); }
	CEtWorldSector *GetSector() { return m_pParentSector; }
	int GetClassID() { return m_nClassID; }
	
	EtVector3 *GetPrePosition()	{ return &m_vPrePosition; }		// 수정
	EtVector3 *GetPreRotation()	{ return &m_vPreRotation; }		// 수정
	EtVector3 *GetPosition() { return &m_vPosition; }
	EtVector3 *GetRotation() { return &m_vRotation; }
	EtVector3 *GetScale() { return &m_vScale; }
	bool IsCastShadow() { return m_bCastShadow; }
	bool IsReceiveShadow() { return m_bReceiveShadow; }
	bool IsCastLightMap() { return m_bCastLightmap; }
	bool IsLightmapInfluence() { return m_bLightmapInfluence; }
	bool IsIgnoreBuildNavMesh() { return m_bIgnoreBuildNavMesh; }
	bool IsIgnoreBuildColMesh() { return m_bIgnoreBuildColMesh; }
	bool IsControlLock() { return m_bControlLock; }

	void SetPropName( const char *szName ) { m_szPropName = szName; }
	void SetClassID( int nValue ) { m_nClassID = nValue; }
	void SetPosition( EtVector3 &vValue ) { m_vPosition = vValue; }
	void SetRotation( EtVector3 &vValue ) { m_vRotation = vValue; }
	void SetScale( EtVector3 &vValue ) { m_vScale = vValue; }

	virtual void EnableCastShadow( bool bEnable ) { m_bCastShadow = bEnable; }
	virtual void EnableReceiveShadow( bool bEnable ) { m_bReceiveShadow = bEnable; }
	virtual void EnableCastLightmap( bool bEnable ) { m_bCastLightmap = bEnable; }
	virtual void EnableCastWater( bool bEnable ) { m_bCastWater = bEnable; }
	virtual void EnableLightmapInfluence( bool bEnable ) { m_bLightmapInfluence = bEnable; }
	virtual void EnableIgnoreBuildNavMesh( bool bEnable ) { m_bIgnoreBuildNavMesh = bEnable; }
	virtual void EnableIgnoreBuildColMesh( bool bEnable ) { m_bIgnoreBuildColMesh = bEnable; }
	virtual void EnableControlLock( bool bEnable ) { m_bControlLock = bEnable; }

	void GetBoundingSphere( SSphere &Sphere, bool bActorSize = false );
	void GetBoundingBox( SAABox &Box );
	void GetBoundingBox( SOBB &Box );

	void SetNavType( int nNavType ) { m_nNavType = nNavType; }
	int GetNavType() { return m_nNavType; }

	virtual EtAniObjectHandle GetObjectHandle() { return m_Handle; }
	
	void SetCurOctreeNode( CEtOctreeNode<CEtWorldProp *> *pNode );
	CEtOctreeNode<CEtWorldProp *> *GetCurOctreeNode() { return m_pCurOctreeNode; }

	int GetCreateUniqueID() { return m_nCreateUniqueID; }
	void SetCreateUniqueID( int nValue ) { m_nCreateUniqueID = nValue; }

	void SetCustomParam( void *pCustomParam );
	void *GetData() { return m_pData; }
	bool LoadCustomInfo( CStream *pStream );

	virtual bool bIsCanBroken(){ return false; }
	virtual void OnSyncComplete( CDNUserSession* pBreakIntoGameSession=NULL ){};
};
