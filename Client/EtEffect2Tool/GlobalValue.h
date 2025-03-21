#pragma once

#include "Singleton.h"
#include "EtBillboardEffect.h"
class CView;
class CPropertyPaneView;
class CParticlePaneView;
class CKeyframePropPaneView;
class CSequenceView;
class CFXPaneView;
class CKeyControlPaneView;

enum ELEMENT_TYPE {
	UNKNOWN = -1,
	MESH,
	PARTICLE,
	POINT_LIGHT,

	ELEMENT_TYPE_FORCE_DWORD	= 0xFFFFFFFF,
};
struct Item
{
	ELEMENT_TYPE Type;
	char szName[64];
	char szDesc[64];
	int nRenderIndex;
	int nObjectIndex;	
	int AmbientParamIndex;
	int DiffuseParamIndex;
	int FXTimeIndex;
	DWORD dwStartFrame;
	DWORD dwEndFrame;
	DWORD dwYOrder;
	int posType;

	BOOL bIterator;
	BOOL bLoop;
	BOOL bStopFlag;

	int nViewAxis;
	int nFixedAxis;

	int	blendOP;
	int	srcBlend;
	int destBlend;

	int cullMode;
	int tilingMode;
	int zWriteMode;

	CEtEffectDataContainer	*FxData[5];
	CEtEffectDataContainer	*ScaleData[3];	

	Item() {
		bIterator = FALSE;
		bLoop = TRUE;
		bStopFlag = TRUE;
		nViewAxis = 0;
		nFixedAxis = 0;
		blendOP = BLENDOP_ADD;
		srcBlend = BLEND_SRCALPHA;
		destBlend = BLEND_INVSRCALPHA;
		cullMode = 0;
		tilingMode = 0;
		zWriteMode = 0;
		for( int i = 0; i < 5; i++) {
			FxData[i] = NULL;
		}
		for( int i = 0; i < 3; i++) {
			ScaleData[i] = NULL;
		}
	};

	void operator = ( const Item &e )
	{
		memcpy(this, &e, sizeof(Item));
		for( int j=0; j<5; j++ ) {		
			FxData[j] = new CEtEffectDataContainer;				
			*FxData[j] = *e.FxData[j];
		}
		for( int j=0; j<3; j++ ) {
			ScaleData[j] = NULL;
			if( e.ScaleData[j] ) {
				ScaleData[j] = new CEtEffectDataContainer;
				*ScaleData[j] = *e.ScaleData[j];
			}
		}
	}
};

class CAxisRenderObject;
class CGlobalValue : public CSingleton <CGlobalValue>
{
public:
public:
	CGlobalValue();
	virtual ~CGlobalValue();

protected:
	CView							*m_pActiveView;
	int									m_nParticleIndex;
	CPropertyPaneView			*m_paneView;
	CParticlePaneView			*m_particleView;
	CKeyframePropPaneView	*m_keyframeView;
	CSequenceView				*m_gridView;
	CFXPaneView					*m_fxView;
	CKeyControlPaneView		*m_keyControlView;
	int									m_nActiveView;

	DWORD							m_dwCurFrame;
	float								m_fCurFrame;

	int									m_nPrevTick;
	int									m_nCurTick;
	float								m_fFloatTick;

	int									m_playTick;
	bool								m_bPlay;
	int									m_nEffectIndex;
	EtEffectObjectHandle		m_hEffectObject;
	int									m_posOrRot;
	std::vector< Item >			m_Items;
	CAxisRenderObject			*m_pAxisRenderObject;
	EtVector3						*m_positionPtr;
	EtVector3						*m_rotationPtr;
	EtBillboardEffectHandle				m_hParticle;
	float								m_iteratePos;

	EtBillboardEffectHandle				 m_currentParticle;
	bool								m_bLoopPlay;
	//int									m_globalMesh;
	// Load Info	
	/*std::vector< std::pair<ELEMENT_TYPE, std::string> > m_resourceFile;*/

public:
	void SetView( CView *pView ) {m_pActiveView = pView;}
	CView *GetView(){ return m_pActiveView; }
	void SetPropertyPaneView( CPropertyPaneView *pView ) { m_paneView = pView;}
	CPropertyPaneView* GetPropertyPaneView() { return m_paneView;}
	void SetParticlePaneView( CParticlePaneView *pView ) {m_particleView = pView;}
	CParticlePaneView* GetParticlePaneView() {return m_particleView;}
	void SetKeyframePropPaneView( CKeyframePropPaneView *pView ){ m_keyframeView = pView;}
	CKeyframePropPaneView* GetKeyframePropPaneView() { return m_keyframeView; }
	void SetSequenceView(CSequenceView *pView ) { m_gridView = pView;}
	CSequenceView* GetSequenceView() { return m_gridView;}
	void SetFXPaneView( CFXPaneView *pView ) { m_fxView = pView;}
	CFXPaneView* GetFXPaneView() { return m_fxView;}
	void SetKeyControlView( CKeyControlPaneView *pView ) {m_keyControlView = pView;}
	CKeyControlPaneView *GetKeyControlPaneView() { return m_keyControlView;}

	CEtEffectDataContainer **GetFXData( CString str );
	CEtEffectDataContainer **GetScaleData( CString str );
	Item* FindItem( CString str );
	std::vector< Item >& GetItems() {return m_Items;}

	void AddItemOnGridView( char *pName, char *pDescName, int nYOrder );
	void RemoveItemOnGridView( char *pName);
	void SetPosOrRot( int posOrRot );
	int GetPosOrRot() {return m_posOrRot;}

	void SetFXItemLayer( char *pName, bool bUp );
	void CopyFXItem( char *pSrcItemName, char *pDestItemName, int nType = 0 );

	void InitDefaultTable(CEtEffectDataContainer **ppTable);
	void CalcColor( EtColor *pColor,  float fTime, CEtEffectDataContainer **ppTable );
	void CalcBillboardMatrix( EtMatrix &BillboardMat, EtVector3 *pUpVec, EtVector3 *pDir, EtVector3 *pPosition, EtVector3 *pCameraPos );
	void CalcMeshWorldMat( EtMatrix *pMat,  float fTime, float fWeight, int nViewAxis, int nFixedAxis, CEtEffectDataContainer **ppTable, CEtEffectDataContainer **ppScaleTable, int posType );
	void CalcPtcWorldMat( EtMatrix *pMat,  float fTime, float fWeight, CEtEffectDataContainer **ppTable, int posType );
	void Process();
	void CalcTick( float fElapsedTime );

	void Save(char *szFileName);
	void Load(char *szFileName);

	void Release();
	void Reset();
	void SetParticleIndex(int index) {m_nParticleIndex=index;}
	void ParseFileList( CString &szFileBuffer, char *szImportExt, std::vector<CString> &szVecList );

	void PlayFXObject();
	void PrevFXObject();
	void NextFXObject();
	
	void Refresh();

	void InitAxisRender();
	CAxisRenderObject* GetAxisRenderObject() {return m_pAxisRenderObject;}
	void SetPositionPtr( EtVector3 *pPos ) {m_positionPtr = pPos;}
	EtVector3* GetPositionPtr() { return m_positionPtr; }
	void SetRotationPtr( EtVector3 *pPos ) {m_rotationPtr = pPos;}
	EtVector3* GetRotationPtr() { return m_rotationPtr; }
	void SetParticle( EtBillboardEffectHandle hParticle ) { m_hParticle = hParticle;  m_iteratePos = 0.f;}

	void SetLoopPlay(bool bLoop ) {m_bLoopPlay = bLoop;}
};
