#pragma once

#include "Singleton.h"
#include "ObjectBase.h"

class CView;
class CGlobalValue : public CSingleton<CGlobalValue> {
public:
	CGlobalValue();
	virtual ~CGlobalValue();

	CString m_szVecInitLoadFile;
protected:
	CView *m_pActiveView;
	CObjectBase *m_pObjectRoot;
	CObjectBase *m_pControlObject;
	bool m_bFullScreen;

	struct SceneHeader {
		char szStr[16];
		int nVersion;
		int nObjectCount;
		SceneHeader() {
			sprintf_s( szStr, "TdScene" );
			nVersion = 1;
			nObjectCount = 0;
		};
	};

public:
	void Finalize();
	void SetView( CView *pView ) { m_pActiveView = pView; }
	CView *GetView() { return m_pActiveView; }

	// Object  ����
	CString GetUniqueString( CObjectBase::OBJECT_TYPE Type );

	CObjectBase *GetObjectFromDescription( CString &szDescription );
	CObjectBase::OBJECT_TYPE GetObjectTypeFromDescription( CString &szDescription );
	CObjectBase *GetObjectFromTreeID( HTREEITEM hti );
	CObjectBase *FindObject( CObjectBase *pObject, CString &szDescription );
	CObjectBase *FindObject( CObjectBase *pObject, HTREEITEM hti );
	CObjectBase *FindObject( CObjectBase *pObject, CObjectBase::OBJECT_TYPE Type );

	BOOL AddObject( CString &szParentName, CObjectBase *pObject );
	BOOL AddObject( CObjectBase *pParent, CObjectBase *pObject );
	BOOL AddObject( HTREEITEM hParent, CObjectBase *pObject );
	BOOL IsSameName( CObjectBase::OBJECT_TYPE Type, CString &szUniqueName );
	void RemoveObject( CString &szDescription );
	void RemoveObjectFromTreeID( HTREEITEM hti );
	CObjectBase *GetRootObject() { return m_pObjectRoot; }
	void SaveSkinObject( CObjectBase *pObject, const char *szNewFileName = NULL );

	void ResetObject( bool bCreateDefaultObject = true );
	void SaveLightSetting( CString szFileName );
	void LoadLightSetting( CString szFileName );

	bool ExportScene( CString szFileName );
	bool ImportScene( CString szFileName );

	void ParseFileList( CString &szBuffer, char *szExt, std::vector<CString> &szVecList );

	int GetObjectCount( CObjectBase::OBJECT_TYPE Type );
	void GetObjectName( CObjectBase::OBJECT_TYPE Type, CString &szStr, CObjectBase *pBase = NULL );
	CObjectBase *GetOneObject( CObjectBase::OBJECT_TYPE Type );
	void CountObject( CObjectBase::OBJECT_TYPE Type, CObjectBase *pObject, int &nCount );

	void UpdateProgress( int nPer );
	void SetControlObject( CObjectBase *pBase ) { m_pControlObject = pBase; }
	CObjectBase *GetControlObject() { return m_pControlObject; }

	void SetFullScreen( bool bFullScreen ) { m_bFullScreen = bFullScreen; }
	bool IsFullScreen() { return m_bFullScreen; }
};

void WriteCString( CString *pStr, FILE *fp );
void ReadCString( CString *pStr, FILE *fp );
