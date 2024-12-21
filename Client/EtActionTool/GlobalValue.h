#pragma once

#include "Singleton.h"
#include "ActionBase.h"

#define _CHECK_WALKFRONT

class CView;
class CActionObject;
class CActionBase;
class CSignalItem;
class CActionSignal;
class CSignalCustomRender;
class CGlobalValue : public CSingleton <CGlobalValue>
{
public:
	CGlobalValue();
	virtual ~CGlobalValue();

protected:
	CView *m_pActiveView;
	CActionBase *m_pRootAction;
	CActionBase *m_pRootLight;
	CActionBase *m_pGlobalOption;

	CActionBase *m_pControlObject;
	CActionBase *m_pPlayObject;
	CSignalCustomRender *m_pSignalCustonRenderObject;

	CString m_szInitLoadFile;
	CString m_szCheckOutFile;
	int m_nFPS;


	bool IsExistFile( CString szFileName );


public:
	void SetView( CView *pView );
	CView *GetView() const;

	void Finalize();

	void Reset();
	// Action 관련 함수들
	bool OpenAction( CString szName );
	bool IsOpenAction();
	bool SaveAction();
	bool CreateAction( CString szName );

#ifdef _CHECK_WALKFRONT
	bool CheckActionHasWalkFront() const;
#endif

	bool AddElement( CString szString );
	bool RemoveElement( HTREEITEM hti );

	CActionSignal *AddSignal( CString szElementName, int nSignalIndex, int nStartFrame, int nEndFrame, int nYOrder );

	CActionBase *GetRootLightObject();
	CActionBase *GetRootObject();
	CActionBase *GetObjectFromTreeID( CActionBase *pRoot, HTREEITEM hti );
	CActionBase *GetObjectFromName( CActionBase *pRoot, CString szName );

	CActionBase *FindObject( CActionBase *pObject, HTREEITEM hti );
	CActionBase *FindObject( CActionBase *pObject, CString &szDescription );

	void AddLight( CActionBase *pBase );
	void RemoveLightFromTreeID( HTREEITEM hti );
	BOOL IsSameName( CActionBase *pRoot, CString &szUniqueName );
	CString GetUniqueName( CActionBase::OBJECT_TYPE Type );

	CActionBase *GetGlobalObject() { return m_pGlobalOption; }

	bool IsCanPlayControl();

	int GetFPS() { return m_nFPS; }
	void SetFPS( int nValue ) { m_nFPS = nValue; }

	void SetControlObject( CActionBase *pBase );
	CActionBase *GetControlObject() { return m_pControlObject; }

	CActionBase *GetPlayObject() { return m_pPlayObject; }

	void SetSignalCustomRenderObject( CSignalCustomRender *pObject ) { m_pSignalCustonRenderObject = pObject; }
	CSignalCustomRender *GetSignalCustomRenderObject() { return m_pSignalCustonRenderObject; }

	CString IsCanCheckOut( const char *szFileName );
	bool CheckOut( const char *szFileName );
	bool CheckIn();
	void IgnoreCheckOut( const char *szFileName );

	void CalcSignalReport( CString &szStr );

	void ExportAction( const char *szFileName, std::vector<std::string> &szVecActionList );
	void ImportAction( const char *szFileName );


#ifdef _CHECK_WALKFRONT
private:
	void GetActFileListFromFolder(const std::string& folderName, std::vector<std::string>& fileList) const;
#endif
};

void WriteCString( CString *pStr, FILE *fp );
void ReadCString( CString *pStr, FILE *fp );
