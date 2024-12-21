// ParticlePaneView.cpp : implementation file
//

#include "stdafx.h"
#include "EtEffect2Tool.h"
#include "ParticlePaneView.h"
#include "GlobalValue.h"
#include "PropertyPaneView.h"
#include "FXPaneView.h"
#include "KeyframePropPaneView.h"
#include "MainFrm.h"
#include "EternityEngine.h"
// CParticlePaneView

IMPLEMENT_DYNCREATE(CParticlePaneView, CFormView)

CParticlePaneView::CParticlePaneView()
	: CFormView(CParticlePaneView::IDD)
{
	m_bActivate = false;
	m_pContextMenu = new CMenu;
	m_pContextMenu->LoadMenu( IDR_CONTEXT );	
	m_nClipBoared = -1;
}

CParticlePaneView::~CParticlePaneView()
{
	Reset( false );
	SAFE_DELETE( m_pContextMenu );	
}

void CParticlePaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
}

BEGIN_MESSAGE_MAP(CParticlePaneView, CFormView)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ADDPARTICLE, &CParticlePaneView::OnAddParticle)
	ON_COMMAND(ID_CREATEPARTICLE, &CParticlePaneView::OnCreateParticle)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CParticlePaneView::OnTvnSelchangedTree1)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CParticlePaneView::OnNMClickTree1)	
	ON_COMMAND(ID_SAVEPARTICLE, &CParticlePaneView::OnSaveParticle)
	ON_COMMAND(ID_ADDEMITTER, &CParticlePaneView::OnAddEmitter)
	ON_COMMAND(ID_REMOVEPARTICLE, &CParticlePaneView::OnRemoveParticle)
	ON_COMMAND(ID_EMITTER_REMOVE, &CParticlePaneView::OnEmitterRemove)
	ON_COMMAND(ID_MESH_LOADMESH, &CParticlePaneView::OnLoadMesh)
	ON_COMMAND(ID_MESH_REMOVE, &CParticlePaneView::OnMeshRemove)
	ON_COMMAND(ID_EMITTER_COPY, &CParticlePaneView::OnEmitterCopy)
	ON_COMMAND(ID_EMITTER_PASTE, &CParticlePaneView::OnEmitterPaste)
	ON_COMMAND(ID_MESH_RELOAD, &CParticlePaneView::OnMeshReload)
	ON_COMMAND(ID_MESH_RELOAD_ALL, &CParticlePaneView::OnMeshReloadAll)
	ON_COMMAND(ID_SAVEASPARTICLE, &CParticlePaneView::OnSaveasparticle)
	ON_COMMAND(ID_EMITTER_RELOAD_TEXTURE, &CParticlePaneView::OnEmitterReloadTexture)
	ON_COMMAND(ID_EMITTER_UP, &CParticlePaneView::OnEmitterUp)
	ON_COMMAND(ID_EMITTER_DOWN, &CParticlePaneView::OnEmitterDown)
END_MESSAGE_MAP()


// CParticlePaneView diagnostics

#ifdef _DEBUG
void CParticlePaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CParticlePaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CParticlePaneView message handlers

void CParticlePaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( m_bActivate == true ) return;
	m_bActivate = true;

	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_WORKSPACEICON );

	m_ImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 14, 1 );
	m_ImageList.Add( &bitmap, RGB(0,255,0) );

	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );
	m_TreeCtrl.Activate();
	m_TreeCtrl.DeleteAllItems();

	m_ptcRootItem = m_TreeCtrl.InsertItem( "Particle", 0, 1, 0 );
	m_meshRootItem = m_TreeCtrl.InsertItem( "Mesh", 0, 1, 0 );

	CGlobalValue::GetInstance().SetParticlePaneView( this );
}

HTREEITEM CParticlePaneView::GetCurrentItem()
{
	HTREEITEM hti = m_TreeCtrl.GetSelectedItem();
	return hti;
}

int CParticlePaneView::GetCurrentDepth()
{
	HTREEITEM hti = GetCurrentItem();
	int depth = -1;
	while( hti != 0 ) {
		hti = m_TreeCtrl.GetParentItem( hti );
		depth++;
	}
	return depth;
}

int CParticlePaneView::GetCurrentPeerOrder()
{
	HTREEITEM hti = GetCurrentItem();
	int depth = 0;
	while( hti != NULL ) {
		hti = m_TreeCtrl.GetPrevSiblingItem( hti );
		depth++;
	}
	return depth-1;
}

bool CParticlePaneView::IsMeshSelect()
{
	HTREEITEM hti = GetCurrentItem();
	HTREEITEM hPrev = NULL;
	while( hti != NULL ) {
		hPrev = hti;
		hti = m_TreeCtrl.GetParentItem( hti );
	}
	return (hPrev == m_meshRootItem);
}

void CParticlePaneView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int depth = GetCurrentDepth();
	CMenu *pSubMenu = NULL;
	int nMenuIndex = 0;
	if( IsMeshSelect() ) {
		HTREEITEM hti = GetCurrentItem();
		if( m_meshRootItem == hti ) nMenuIndex = 3;
		else nMenuIndex = 4;
	}
	else
		nMenuIndex = depth;
	pSubMenu = m_pContextMenu->GetSubMenu( nMenuIndex );
	pSubMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
}

void CParticlePaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	if( m_TreeCtrl ) {
		CRect rcRect;
		GetClientRect( &rcRect );
		m_TreeCtrl.MoveWindow( &rcRect );
	}
	// TODO: Add your message handler code here
}

void CParticlePaneView::Reset( bool bDeleteTreeItems )
{
	CGlobalValue::GetInstance().GetKeyframePropPaneView()->RefreshPointer();
	CGlobalValue::GetInstance().GetPropertyPaneView()->RefreshPointer();

	for(std::map< std::string, ParticleFile >::iterator it = m_ParticleFiles.begin(); it != m_ParticleFiles.end(); ++it) {
		EternityEngine::DeleteParticleData( it->second.nRenderIndex );
		if( bDeleteTreeItems ) {
			m_TreeCtrl.DeleteItem( it->second.treeItem );
		}
	}
	m_ParticleFiles.clear();

	for(std::map< std::string, MeshFile >::iterator it = m_MeshFiles.begin(); it != m_MeshFiles.end(); ++it) {
		if( bDeleteTreeItems ) {
			m_TreeCtrl.DeleteItem( it->second.treeItem );
		}
	}
	m_MeshFiles.clear();
}

void CParticlePaneView::AddParticleFile( CString str )
{
	char szFullPath[255]={0,}, szFileName[255]={0,};
	sprintf_s(szFullPath, "%s", str.GetBuffer());

	DWORD dwAttr = GetFileAttributes( szFullPath );
	if( dwAttr & FILE_ATTRIBUTE_READONLY ) {
		CString szStr;
		szStr.Format( "다음 파일들이 읽기전용 속성입니다.\n\n%s",szFullPath );
		MessageBox( szStr, "경고", MB_OK );
	}

	char *ps = strrchr(szFullPath, '\\');
	if( ps ) {
		strcpy_s(szFileName, ps+1);
	}
	else {
		strcpy(szFileName, szFullPath);
	}
	// 중복 파일 스킵
	if( m_ParticleFiles.count( szFileName ) != 0 ) {
		m_TreeCtrl.DeleteItem( m_ParticleFiles[szFileName].treeItem );
		EternityEngine::DeleteParticleData(m_ParticleFiles[szFileName].nRenderIndex);
		m_ParticleFiles.erase( szFileName );		
	}

	ParticleFile particleFile;

	particleFile.nRenderIndex = EternityEngine::LoadParticleData( szFullPath );
	assert( particleFile.nRenderIndex != -1);
	strcpy_s(particleFile.szFullPath, szFullPath );

	HTREEITEM hti = m_TreeCtrl.InsertItem( szFileName, 2, 3, m_ptcRootItem );
	particleFile.treeItem = hti;

	// Process Loading Particle
	EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( particleFile.nRenderIndex );
	for( int i = 0; i < particleHandle->GetEmitterCount(); i++) {
		CEtBillboardEffectEmitter *pEmitter = particleHandle->GetEmitter( i );
		particleFile.m_EmitterList.push_back( pEmitter );
		char szStr[255];
		sprintf_s(szStr, "Emitter - %s", pEmitter->GetEmitterInfo()->szTextureName);
		HTREEITEM hti1 = m_TreeCtrl.InsertItem( szStr, 4, 5, hti );
		m_TreeCtrl.SetItemState(hti1, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );
	}
	particleFile.bIterate = false;
	particleFile.bLoop = false;

	m_ParticleFiles.insert( std::map< std::string, ParticleFile>::value_type(szFileName, particleFile) );
	OnSelected();
	m_TreeCtrl.Expand( m_ptcRootItem, TVE_EXPAND );
}

void CParticlePaneView::AddMeshFile( CString str )
{
	char szFullPath[255]={0,}, szFileName[255]={0,};
	sprintf_s(szFullPath, "%s", str.GetBuffer());
	char *ps = strrchr(szFullPath, '\\');
	if( ps ) {
		strcpy_s(szFileName, ps+1);
	}
	else {
		strcpy(szFileName, szFullPath);
	}
	// 중복 파일 스킵
	if( m_MeshFiles.count( szFileName ) != 0 ) {
		m_TreeCtrl.DeleteItem( m_MeshFiles[szFileName].treeItem );
		m_MeshFiles.erase( szFileName );
	}

	MeshFile meshFile;
	strcpy_s(meshFile.szFullPath, szFullPath );

	meshFile.treeItem = m_TreeCtrl.InsertItem( szFileName, 2, 3, m_meshRootItem );

	m_MeshFiles.insert( std::map< std::string, MeshFile>::value_type(szFileName, meshFile) );	
	m_TreeCtrl.Expand( m_meshRootItem, TVE_EXPAND );
}

void CParticlePaneView::OnAddParticle()
{
	// TODO: Add your command handler code here
	CString szFileList;
	TCHAR szFilter[] = _T( "Particle File (*.ptc)|*.ptc|All Files (*.*)|*.*||" );
	CFileDialog dlg( TRUE, _T("ptc"), _T("*.ptc"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT| OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );
	dlg.GetOFN().lpstrFile = szFileList.GetBuffer(100000);
	dlg.GetOFN().nMaxFile = 100000;

	dlg.m_ofn.lpstrTitle = "Open Eternity Particle Engine File";

	if( IDOK != dlg.DoModal() ) {
		szFileList.ReleaseBuffer();
		return;
	}

	std::vector<CString> szVecPtcList;
	CGlobalValue::GetInstance().ParseFileList( szFileList, "ptc", szVecPtcList );	
	szFileList.ReleaseBuffer();
	
	for each( CString str in szVecPtcList ) {
		AddParticleFile( str );
	}	
}

void CParticlePaneView::OnCreateParticle()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilter[] = _T( "Particle File (*.ptc)|*.ptc|All Files (*.*)|*.*||" );
	CFileDialog Dlg( FALSE, _T("ptc"), _T("*.ptc"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, szFilter, this );
	Dlg.m_ofn.lpstrTitle = "Save Particle File";
	if( Dlg.DoModal() != IDOK ) return;

	const char *fileName = Dlg.GetOFN().lpstrFile;
	CFileStream *pStream = new CFileStream(fileName, CFileStream::OPEN_WRITE );
	SBillboardEffectHeader Header;
	char cDummy[ BILLBOARD_EFFECT_HEADER_RESERVED ];
	memset( &Header, 0, sizeof( SBillboardEffectHeader ) );
	Header.nVersion = EMITTER_VER;
	Header.nEmitterCount = 0;
	strcpy( Header.szHeader, BILLBOARD_EFFECT_HEADER );
	pStream->Write( &Header, sizeof( SBillboardEffectHeader ) );
	memset( cDummy, 0, BILLBOARD_EFFECT_HEADER_RESERVED );
	pStream->Write( cDummy, BILLBOARD_EFFECT_HEADER_RESERVED );
	delete pStream;
	AddParticleFile( fileName );	
}

void CParticlePaneView::CheckUpdateRenderView()
{
	if( IsMeshSelect() ) {
		return;
	}
	HTREEITEM hti = GetCurrentItem();
	CString str = m_TreeCtrl.GetItemText(hti);

	CGlobalValue::GetInstance().GetPropertyPaneView()->ApplySetting();
	CGlobalValue::GetInstance().GetPropertyPaneView()->ApplyParticleSetting();
	CGlobalValue::GetInstance().GetKeyframePropPaneView()->UpdateSetting();

	int depth = GetCurrentDepth();
	if( depth == 1 ) {	// particle	Render
		if( m_ParticleFiles.count( str.GetBuffer() ) != 0) {
			int nParticleIndex = m_ParticleFiles[ str.GetBuffer() ].nRenderIndex;
			MatrixEx cross;
			SAFE_RELEASE_SPTR( m_hParticle );
			EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );
			particleHandle->CreateBillboardEffectBuffer();
			m_hParticle = EternityEngine::CreateBillboardEffect( nParticleIndex, cross, false );
			m_hParticle->EnableLoop( m_ParticleFiles[ str.GetBuffer() ].bLoop );
			m_hParticle->EnableTracePos( m_ParticleFiles[ str.GetBuffer() ].bIterate );
			m_hParticle->SetParticleTick(-10);
			CGlobalValue::GetInstance().SetParticle( m_hParticle );

			HTREEITEM hChild = m_TreeCtrl.GetChildItem( m_ParticleFiles[ str.GetBuffer() ].treeItem );

			for( int i = 0; i < particleHandle->GetEmitterCount(); i++) {
				DWORD dwState = m_TreeCtrl.GetItemState( hChild, TVIS_STATEIMAGEMASK)>>12;			
				particleHandle->GetEmitter( i )->EnableDraw( dwState==2 ? true : false );
				hChild = m_TreeCtrl.GetNextSiblingItem( hChild );
			}
		}
	}
	else if( depth == 2 ) {	// emitter	Render
		hti = m_TreeCtrl.GetParentItem( hti );
		str = m_TreeCtrl.GetItemText(hti);
		if( m_ParticleFiles.count( str.GetBuffer() ) != 0) {
			int nParticleIndex = m_ParticleFiles[ str.GetBuffer() ].nRenderIndex;
			MatrixEx cross;
			SAFE_RELEASE_SPTR( m_hParticle );
			EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );
			int peerOrder = GetCurrentPeerOrder();
			for( int i = 0; i < particleHandle->GetEmitterCount(); i++) {
				particleHandle->GetEmitter( i )->EnableDraw( false );
			}
			particleHandle->GetEmitter( peerOrder )->EnableDraw( true );
			particleHandle->CreateBillboardEffectBuffer();
			m_hParticle = EternityEngine::CreateBillboardEffect( nParticleIndex, cross, false );
			m_hParticle->EnableLoop( m_ParticleFiles[ str.GetBuffer() ].bLoop );
			m_hParticle->EnableTracePos( m_ParticleFiles[ str.GetBuffer() ].bIterate );
			m_hParticle->SetParticleTick(-10);
			CGlobalValue::GetInstance().SetParticle( m_hParticle );
		}		
	}
}

void CParticlePaneView::OnSelected()
{
	int depth = GetCurrentDepth();
	if( IsMeshSelect() ) {
		if( depth == 1) {
			HTREEITEM hti = GetCurrentItem();
			CString str = m_TreeCtrl.GetItemText( hti );
			CGlobalValue::GetInstance().GetFXPaneView()->SetCurrentName( str.GetBuffer() );
		}
		((CMainFrame*)AfxGetMainWnd())->ShowPane( 3 );
		CGlobalValue::GetInstance().GetPropertyPaneView()->OnEmpty();
		return;
	}
	if( depth != 2 ) {
		CGlobalValue::GetInstance().GetKeyframePropPaneView()->OnOtherSelected();
	}
	if( depth == 1 ) {		
		HTREEITEM hti = GetCurrentItem();
		CString str = m_TreeCtrl.GetItemText( hti );
		CGlobalValue::GetInstance().GetFXPaneView()->SetCurrentName( str.GetBuffer() );
		assert( m_ParticleFiles.count( str.GetBuffer() ) != 0);
		CGlobalValue::GetInstance().GetPropertyPaneView()->OnParticleSelected(
			&m_ParticleFiles[ str.GetBuffer() ].bLoop, &m_ParticleFiles[ str.GetBuffer() ].bIterate );
	}
	else if( depth == 2 ) {
		int peerOrder = GetCurrentPeerOrder();
		HTREEITEM hti = GetCurrentItem();
		hti = m_TreeCtrl.GetParentItem( hti );
		CString str = m_TreeCtrl.GetItemText( hti );
		assert( m_ParticleFiles.count( str.GetBuffer() ) != 0);

		const ParticleFile &parFile = m_ParticleFiles[ str.GetBuffer() ];

		CGlobalValue::GetInstance().GetPropertyPaneView()->OnEmitterSelected(
			parFile.m_EmitterList[ peerOrder ]  );

		CGlobalValue::GetInstance().GetKeyframePropPaneView()->OnEmitterSelected(
			parFile.m_EmitterList[ peerOrder ] );
	}
}

void CParticlePaneView::OnValueChange()
{
	CheckUpdateRenderView();
}

void CParticlePaneView::RenameTexture( char *szName )
{
	HTREEITEM hti = GetCurrentItem();
	int peerOrder = GetCurrentPeerOrder();
	char szStr[255];
	sprintf_s(szStr, "Emitter - %s", szName);
	m_TreeCtrl.SetItemText( hti, szStr );
}

void CParticlePaneView::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CheckUpdateRenderView();
	OnSelected();
}

void CParticlePaneView::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CheckUpdateRenderView();
	OnSelected();
}

void CParticlePaneView::OnSaveParticle()
{
	int depth = GetCurrentDepth();
	assert( depth == 1);
	HTREEITEM hti = GetCurrentItem();
	CString str = m_TreeCtrl.GetItemText( hti );
	assert( m_ParticleFiles.count( str.GetBuffer() ) != 0);
	int nParticleIndex = m_ParticleFiles[ str.GetBuffer() ].nRenderIndex;
	EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );
	DWORD dwAttr = GetFileAttributes( m_ParticleFiles[ str.GetBuffer() ].szFullPath );
	if( dwAttr != 0xffffffff && (dwAttr & FILE_ATTRIBUTE_READONLY) ) {
		CString szStr;
		szStr.Format( "다음 파일들이 읽기전용 속성입니다.\n\n%s", m_ParticleFiles[ str.GetBuffer() ].szFullPath);
		MessageBox( szStr, "경고", MB_OK );
		return;
	}
	particleHandle->Save( m_ParticleFiles[ str.GetBuffer() ].szFullPath );	
}

void CParticlePaneView::SetDefaultEmitterParam( SEmitter *Param )
{
	memset( Param, 0, sizeof(SEmitter) );
	Param->nGenerateCount = 1;
	Param->nLifeDuration = 200;
	Param->nBeginningTime = 200;
	Param->fSpeedVelocityMin = 5.f;
	Param->fSpeedVelocityMax = 5.f;
	Param->vInitiateLaunch = D3DXVECTOR4( 0, 1, 0, 0 );
	Param->fFallSpeed = 0.0f;
	Param->vFallVector = D3DXVECTOR3(0, 0, 0);
	Param->fZRotateStartValue = 0;
	Param->fZRotateEndValue = 0;
	Param->fOpposeTime = 1.0f;
	Param->fOpposeScaleValue = 0.0f;
	Param->fDummy = 1.0f;
	Param->nBlendOP = 0;
	Param->nSourceBlendMode = 4;
	Param->nDestBlendMode = 1;
	Param->bUseRandomSpin = FALSE;
}

void CParticlePaneView::OnAddEmitter()
{
	SAFE_RELEASE_SPTR( m_hParticle );
	CGlobalValue::GetInstance().Refresh();
	int depth = GetCurrentDepth();
	assert( depth == 1);
	HTREEITEM hti = GetCurrentItem();
	CString str = m_TreeCtrl.GetItemText( hti );
	assert( m_ParticleFiles.count( str.GetBuffer() ) != 0);
	int nParticleIndex = m_ParticleFiles[ str.GetBuffer() ].nRenderIndex;
	EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );
	SEmitter emitParam;
	SetDefaultEmitterParam(&emitParam);
	strcpy(emitParam.szTextureName, "Glow01.dds");
	int index = particleHandle->AddEmitter(&emitParam);

	CEtBillboardEffectEmitter *pEmitter = particleHandle->GetEmitter( index );
	m_ParticleFiles[ str.GetBuffer() ].m_EmitterList.push_back( pEmitter );
	char szStr[255];
	sprintf_s(szStr, "Emitter - %s", pEmitter->GetEmitterInfo()->szTextureName);
	HTREEITEM hti1 = m_TreeCtrl.InsertItem( szStr, 4, 5, hti );
	m_TreeCtrl.SetItemState(hti1, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );
	m_TreeCtrl.Expand( hti, TVE_EXPAND );

	CGlobalValue::GetInstance().GetKeyframePropPaneView()->SetDefaultTable();
}

void CParticlePaneView::RemoveParticle( HTREEITEM hti ) 
{
	CString str = m_TreeCtrl.GetItemText( hti );
	m_TreeCtrl.DeleteItem( hti );
	EternityEngine::DeleteParticleData(m_ParticleFiles[str.GetBuffer()].nRenderIndex);
	m_ParticleFiles.erase(  str.GetBuffer() );
}

void CParticlePaneView::OnRemoveParticle()
{
	CGlobalValue::GetInstance().GetKeyframePropPaneView()->RefreshPointer();
	CGlobalValue::GetInstance().GetPropertyPaneView()->RefreshPointer();

	HTREEITEM hti = GetCurrentItem();
	RemoveParticle( hti );
}

void CParticlePaneView::OnEmitterRemove()
{
	CGlobalValue::GetInstance().GetKeyframePropPaneView()->RefreshPointer();
	CGlobalValue::GetInstance().GetPropertyPaneView()->RefreshPointer();

	assert( GetCurrentDepth() == 2);
	HTREEITEM hti = GetCurrentItem();	
	HTREEITEM hParent = m_TreeCtrl.GetParentItem( hti );
	CString str = m_TreeCtrl.GetItemText( hParent );
	if( m_ParticleFiles.count( str.GetBuffer() ) != 0) {
		ParticleFile &parFile = m_ParticleFiles[ str.GetBuffer() ];
		int nParticleIndex = parFile.nRenderIndex;
		EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );
		int peerOrder = GetCurrentPeerOrder();
		particleHandle->DeleteEmitter( peerOrder );
		parFile.m_EmitterList.erase( parFile.m_EmitterList.begin() + peerOrder );
	}	
	m_TreeCtrl.DeleteItem( hti );	
	// TODO: Add your command handler code here
}

void CParticlePaneView::OnEmitterCopy()
{
	assert( GetCurrentDepth() == 2);
	m_nClipBoared = GetCurrentPeerOrder();
	// TODO: Add your command handler code here
}

void CParticlePaneView::OnEmitterPaste()
{
	if( m_nClipBoared == -1 ) return;	
	assert( GetCurrentDepth() == 2);
	HTREEITEM hti = GetCurrentItem();	
	HTREEITEM hParent = m_TreeCtrl.GetParentItem( hti );
	CString str = m_TreeCtrl.GetItemText( hParent );
	if( m_ParticleFiles.count( str.GetBuffer() ) != 0) {
		ParticleFile &parFile = m_ParticleFiles[ str.GetBuffer() ];
		int nParticleIndex = parFile.nRenderIndex;
		EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );
		int peerOrder = GetCurrentPeerOrder();
		particleHandle->GetEmitter( peerOrder )->SetEmitterInfo( particleHandle->GetEmitter(m_nClipBoared)->GetEmitterInfo() );
		particleHandle->GetEmitter( peerOrder )->LoadTexture( particleHandle->GetEmitter(m_nClipBoared)->GetEmitterInfo()->szTextureName );
		RenameTexture(particleHandle->GetEmitter(m_nClipBoared)->GetEmitterInfo()->szTextureName);

		CGlobalValue::GetInstance().GetPropertyPaneView()->UpdateData( parFile.m_EmitterList[ peerOrder ]->GetEmitterInfo(), true);			  				
		CGlobalValue::GetInstance().GetPropertyPaneView()->RefreshProperty();
	}
	// TODO: Add your command handler code here
}


void CParticlePaneView::OnLoadMesh()
{
	CString szFileList;
	TCHAR szFilter[] = _T( "Skin File (*.skn)|*.skn|All Files (*.*)|*.*||" );
	CFileDialog dlg( TRUE, _T("skn"), _T("*.skn"), OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT| OFN_ENABLESIZING , szFilter, CGlobalValue::GetInstance().GetView() );
	dlg.GetOFN().lpstrFile = szFileList.GetBuffer(100000);
	dlg.GetOFN().nMaxFile = 100000;
	dlg.m_ofn.lpstrTitle = "Open Eternity Engine Skin File";
	if( IDOK != dlg.DoModal() ) {
		szFileList.ReleaseBuffer();
		return;
	}
	std::vector<CString> szVecPtcList;
	CGlobalValue::GetInstance().ParseFileList( szFileList, "skn", szVecPtcList );	
	szFileList.ReleaseBuffer();

	for each( CString str in szVecPtcList ) {
		AddMeshFile( str );
	}	
}

void CParticlePaneView::OnMeshRemove()
{
	// TODO: Add your command handler code here
	if( GetCurrentDepth() == 1) {
		HTREEITEM hti = GetCurrentItem();	
		m_TreeCtrl.DeleteItem( hti );
	}
}

void CParticlePaneView::OnMeshReload()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( GetCurrentDepth() == 1 ) {
		HTREEITEM hti = GetCurrentItem();	
		CString str = m_TreeCtrl.GetItemText( hti );
		str.MakeLower();

		std::vector< EtObjectHandle > vecReloadObject;
		std::vector< Item >& Items = CGlobalValue::GetInstance().GetItems();
		for( int i = 0; i < (int)Items.size(); ++i ) {
			if( Items[i].Type == MESH && Items[i].nObjectIndex != -1 ) {
				EtObjectHandle hObject;
				hObject = CEtObject::GetSmartPtr( Items[i].nObjectIndex );
				CString strSkin = hObject->GetSkinFileName();
				if( str == strSkin ) {
					vecReloadObject.push_back( hObject );
					hObject->Clear();
				}
			}
		}
		CEtResource::FlushWaitDelete();

		EtResourceHandle hSkin;
		hSkin = EternityEngine::LoadSkin( str );

		for( int i = 0; i < (int)vecReloadObject.size(); ++i )
		{
			vecReloadObject[i]->ForceSetSkin( hSkin );
			if( i != 0 ) hSkin->AddRef();
		}
	}
}

void CParticlePaneView::OnMeshReloadAll()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	std::map< CString, std::vector< EtObjectHandle > > mapReloadObjectList;
	CString str;
	std::vector< Item >& Items = CGlobalValue::GetInstance().GetItems();
	std::vector< EtObjectHandle > vecReloadObject;
	for( int i = 0; i < (int)Items.size(); ++i ) {
		if( Items[i].Type == MESH && Items[i].nObjectIndex != -1 ) {
			EtObjectHandle hObject;
			hObject = CEtObject::GetSmartPtr( Items[i].nObjectIndex );
			CString strSkin = hObject->GetSkinFileName();
			if( mapReloadObjectList.find( strSkin ) == mapReloadObjectList.end() ) {
				vecReloadObject.clear();
				vecReloadObject.push_back( hObject );
				mapReloadObjectList.insert( make_pair(strSkin, vecReloadObject) );
			}
			else {
				std::map< CString, std::vector< EtObjectHandle > >::iterator iter = mapReloadObjectList.find( strSkin );
				iter->second.push_back( hObject );
			}
		}
	}

	std::map< CString, std::vector< EtObjectHandle > >::iterator iter = mapReloadObjectList.begin();
	for( ; iter != mapReloadObjectList.end(); ++iter ) {
		std::vector< EtObjectHandle >& vecReloadObject = iter->second;
		for( int j = 0; j < (int)vecReloadObject.size(); ++j ) {
			vecReloadObject[j]->Clear();
		}
	}
	CEtResource::FlushWaitDelete();

	EtResourceHandle hSkin;
	iter = mapReloadObjectList.begin();
	for( ; iter != mapReloadObjectList.end(); ++iter ) {
		hSkin = EternityEngine::LoadSkin( iter->first );
		std::vector< EtObjectHandle >& vecReloadObject = iter->second;
		for( int j = 0; j < (int)vecReloadObject.size(); ++j ) {
			vecReloadObject[j]->ForceSetSkin( hSkin );
			if( j != 0 ) hSkin->AddRef();
		}
	}
}
void CParticlePaneView::OnSaveasparticle()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	HTREEITEM hti = GetCurrentItem();	
	CString str = m_TreeCtrl.GetItemText( hti );

	TCHAR szFilter[] = _T( "Particle File (*.ptc)|*.ptc|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("ptc"), str.GetBuffer(), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter );

	dlg.m_ofn.lpstrTitle = "Save As Particle File";

	if( IDOK != dlg.DoModal() )
		return;

	assert( m_ParticleFiles.count( str.GetBuffer() ) != 0 );
	int nParticleIndex = m_ParticleFiles[ str.GetBuffer() ].nRenderIndex;
	EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );

	DWORD dwAttr = GetFileAttributes( dlg.GetPathName().GetBuffer() );
	if( dwAttr != 0xffffffff && (dwAttr & FILE_ATTRIBUTE_READONLY) ) {
		CString szStr;
		szStr.Format( "다음 파일이 읽기전용 속성입니다.\n\n%s", dlg.GetPathName().GetBuffer() );
		MessageBox( szStr, "경고", MB_OK );
		return;
	}
	particleHandle->Save( dlg.GetPathName().GetBuffer() );	
}

void CParticlePaneView::OnEmitterReloadTexture()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.	
	assert( GetCurrentDepth() == 2 );
	HTREEITEM hti = GetCurrentItem();	
	HTREEITEM hParent = m_TreeCtrl.GetParentItem( hti );
	CString str = m_TreeCtrl.GetItemText( hParent );
	if( m_ParticleFiles.count( str.GetBuffer() ) != 0) {
		ParticleFile &parFile = m_ParticleFiles[ str.GetBuffer() ];
		int nParticleIndex = parFile.nRenderIndex;
		EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );
		int peerOrder = GetCurrentPeerOrder();
		EtTextureHandle hTexture = particleHandle->GetEmitter( peerOrder )->GetTextureHandle();

		// 현재 로딩된 Emitter들을 전부 뒤져서 같은 텍스처를 쓰는 Emitter들의 텍스처를 모두 릴리즈 하고 다시 셋팅하는건 작업량이 많아서
		// 이렇게 강제로 로딩하는거로 하겠다.
		CFileStream *pStream = new CFileStream( hTexture->GetFullName() );
		hTexture->LoadResource( pStream );
		pStream->Close();
		SAFE_DELETE( pStream );
	}
}

void CParticlePaneView::OnEmitterUp()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	assert( GetCurrentDepth() == 2 );
	HTREEITEM hti = GetCurrentItem();
	HTREEITEM hParent = m_TreeCtrl.GetParentItem( hti );
	CString str = m_TreeCtrl.GetItemText( hParent );
	if( m_ParticleFiles.count( str.GetBuffer() ) != 0 ) {
		ParticleFile &parFile = m_ParticleFiles[ str.GetBuffer() ];
		int nParticleIndex = parFile.nRenderIndex;
		EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );
		int peerOrder = GetCurrentPeerOrder();
		int nEmitterCount = particleHandle->GetEmitterCount();
		if( peerOrder != 0 ) {
			particleHandle->ChangeEmitter( peerOrder-1, peerOrder );

			HTREEITEM htiSub = m_TreeCtrl.GetPrevSiblingItem( hti );
			CString strSub = m_TreeCtrl.GetItemText( htiSub );
			m_TreeCtrl.SetItemText( htiSub, m_TreeCtrl.GetItemText( hti ) );
			m_TreeCtrl.SetItemText( hti, strSub.GetBuffer() );

			CEtBillboardEffectEmitter *pEmitter = m_ParticleFiles[ str.GetBuffer() ].m_EmitterList[peerOrder];
			m_ParticleFiles[ str.GetBuffer() ].m_EmitterList[peerOrder] = m_ParticleFiles[ str.GetBuffer() ].m_EmitterList[peerOrder-1];
			m_ParticleFiles[ str.GetBuffer() ].m_EmitterList[peerOrder-1] = pEmitter;
		}
	}
}

void CParticlePaneView::OnEmitterDown()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	assert( GetCurrentDepth() == 2 );
	HTREEITEM hti = GetCurrentItem();
	HTREEITEM hParent = m_TreeCtrl.GetParentItem( hti );
	CString str = m_TreeCtrl.GetItemText( hParent );
	if( m_ParticleFiles.count( str.GetBuffer() ) != 0 ) {
		ParticleFile &parFile = m_ParticleFiles[ str.GetBuffer() ];
		int nParticleIndex = parFile.nRenderIndex;
		EtBillboardEffectDataHandle particleHandle = CEtResource::GetResource( nParticleIndex );
		int peerOrder = GetCurrentPeerOrder();
		int nEmitterCount = particleHandle->GetEmitterCount();
		if( peerOrder != nEmitterCount-1 ) {
			particleHandle->ChangeEmitter( peerOrder, peerOrder+1 );

			HTREEITEM htiSub = m_TreeCtrl.GetNextSiblingItem( hti );
			CString strSub = m_TreeCtrl.GetItemText( htiSub );
			m_TreeCtrl.SetItemText( htiSub, m_TreeCtrl.GetItemText( hti ) );
			m_TreeCtrl.SetItemText( hti, strSub.GetBuffer() );

			CEtBillboardEffectEmitter *pEmitter = m_ParticleFiles[ str.GetBuffer() ].m_EmitterList[peerOrder];
			m_ParticleFiles[ str.GetBuffer() ].m_EmitterList[peerOrder] = m_ParticleFiles[ str.GetBuffer() ].m_EmitterList[peerOrder+1];
			m_ParticleFiles[ str.GetBuffer() ].m_EmitterList[peerOrder+1] = pEmitter;
		}
	}
}
