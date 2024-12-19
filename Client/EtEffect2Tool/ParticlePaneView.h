#pragma once
#include "afxcmn.h"

// CParticlePaneView form view
#include "TreeCtrlEx.h"

class CParticlePaneView : public CFormView
{
	DECLARE_DYNCREATE(CParticlePaneView)

protected:
	CParticlePaneView();           // protected constructor used by dynamic creation
	virtual ~CParticlePaneView();

public:
	enum { IDD = IDD_PARTICLEEDIT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:

public:
	CTreeCtrlEx m_TreeCtrl;
	CImageList m_ImageList;
	bool	m_bActivate;
	HTREEITEM	m_ptcRootItem;
	HTREEITEM	m_meshRootItem;
	CMenu *m_pContextMenu;	
	struct ParticleFile {
		int			nRenderIndex;
		char		szFullPath[ MAX_PATH ];
		std::vector< CEtBillboardEffectEmitter* >	m_EmitterList;
		HTREEITEM	treeItem;
		bool		bIterate;
		bool		bLoop;
	};

	struct MeshFile {
		HTREEITEM treeItem;
		char szFullPath[MAX_PATH];
		int nTemp;
	};
	
	std::map< std::string, ParticleFile > m_ParticleFiles; // 중복파일 체크
	std::map< std::string, MeshFile > m_MeshFiles;
	EtBillboardEffectHandle m_hParticle;

	int		m_nClipBoared;
	
public:
	virtual void OnInitialUpdate();
	HTREEITEM GetCurrentItem();
	int GetCurrentDepth();
	int GetCurrentPeerOrder();
	bool IsMeshSelect();
	void CheckUpdateRenderView();
	void OnSelected();
	void OnValueChange();
	void RenameTexture( char *szName );
	void Reset(bool bDeleteTreeItems=true);
	void AddParticleFile( CString str );
	void AddMeshFile( CString str );
	void RemoveParticle( HTREEITEM hti);
	void SetDefaultEmitterParam( SEmitter *Param );
	//char *GetCurrentParticleName() {return m_szSelectParticleName;}
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAddParticle();
	afx_msg void OnCreateParticle();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnSaveParticle();
	afx_msg void OnAddEmitter();
	afx_msg void OnRemoveParticle();
	afx_msg void OnEmitterRemove();
	afx_msg void OnLoadMesh();
	afx_msg void OnMeshRemove();
	afx_msg void OnEmitterCopy();
	afx_msg void OnEmitterPaste();
	afx_msg void OnMeshReload();
	afx_msg void OnMeshReloadAll();
	afx_msg void OnSaveasparticle();
	afx_msg void OnEmitterReloadTexture();
	afx_msg void OnEmitterUp();
	afx_msg void OnEmitterDown();
};


