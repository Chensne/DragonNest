#pragma once
#include "BackgroundLoader.h"
#include "LostDeviceProcess.h"
#include "StringUtil.h"

class CEtResource;
typedef CSmartPtr< CEtResource > EtResourceHandle;

class CEtResource : public CSmartPtrBase< CEtResource >, public CBackgroundLoader
{
public:
	CEtResource();
	virtual ~CEtResource();

protected:
	int m_nRefCount;
	int m_ResourceType;
	CFileNameString m_szFullName;
	CFileNameString m_szFileName;
	bool m_bDeleteImmediate;
	static std::vector< CEtResource * > s_vecWaitDelete;
	static std::map< INT64, EtResourceHandle > s_mapResourceSearch;

public:	
	void Delete();
	virtual int AddRef();
	virtual int Release();
	int GetRefCount() { return m_nRefCount; }
	static void FlushWaitDelete();
	static void FlushWaitDelete( int nResourceType );
	static void FlushWaitDeleteByCount(int nCount);
	static bool IsEmptyWaitDelete() { return s_vecWaitDelete.empty(); }
	const char *GetFullName() { return m_szFullName.c_str(); }
	const char *GetFileName() { return m_szFileName.c_str(); }
	void SetFileName( const char *pFileName ) { m_szFileName = pFileName; }

	void AddResourceSearchMap();
	void RemoveResourceSearchMap();

	void SetResourceType( int Type ) { m_ResourceType = Type; }
	int GetResourceType() { return m_ResourceType; }
	void SetDeleteImmediate( bool bSet ) { m_bDeleteImmediate = bSet; }

	static bool CheckOutOfMemory( float fRemainRatio, DWORD dwTermTick = 0 );
	void CheckResourceMemory();
	int Load( const char *pFileName );
	int Load( CStream *pStream );

	static CSmartPtr< CEtResource > GetResource( const char *pFileName, bool bAddRef = false );
	static CSmartPtr< CEtResource > GetResource( int nIndex ) { return GetSmartPtr( nIndex ); }

	void AddToListByLock();
	void DeleteFromListByLock();
	void RemoveFromMap();
};