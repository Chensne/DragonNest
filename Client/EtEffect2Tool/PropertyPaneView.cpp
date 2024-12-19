// PropertyPaneView.cpp : implementation file
//

#include "stdafx.h"
#include "EtEffect2Tool.h"
#include "PropertyPaneView.h"
#include "ParticlePaneView.h"
#include "GlobalValue.h"
#include "../MFCUtility/Common.h"
#include "SequenceView.h"
#include "RenderBase.h"
#include "AxisRenderObject.h"

// CPropertyPaneView

IMPLEMENT_DYNCREATE(CPropertyPaneView, CFormView)

CPropertyPaneView::CPropertyPaneView()
	: CFormView(CPropertyPaneView::IDD)
{
	m_bActivate = false;
	m_Emitter = NULL;
	m_pLoop = NULL;
	m_pIterate = NULL;
	m_nMode = NONE;
}

CPropertyPaneView::~CPropertyPaneView()
{
	RefreshPointer();
}

void CPropertyPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPropertyPaneView, CFormView)
	ON_MESSAGE( XTPWM_PROPERTYGRID_NOTIFY, OnNotifyGrid )
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPropertyPaneView diagnostics

#ifdef _DEBUG
void CPropertyPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPropertyPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPropertyPaneView message handlers

void CPropertyPaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	if( m_bActivate == true ) return;
	m_bActivate = true;

	CRect rcRect;
	GetClientRect( &rcRect );

	m_PropertyGrid.Create( rcRect, this, 0 );
	m_PropertyGrid.SetCustomColors( RGB(200, 200, 200), 0, RGB(182, 210, 189), RGB(247, 243, 233), 0);
	// TODO: Add your specialized code here and/or call the base clas

	CGlobalValue::GetInstance().SetPropertyPaneView( this );
}

void CPropertyPaneView::SetDefaultEmitterParam(SEmitter *Param )
{
	memset( Param, 0, sizeof(SEmitter) );
	Param->nGenerateCount = 1;
	Param->nLifeDuration = 200;
	Param->nBeginningTime = 200;
	Param->fSpeedVelocityMin = 10.f;
	Param->fSpeedVelocityMax = 10.f;
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
	Param->nRandomSeed = 0;
	//	Param->nAlphaType=-1;
	Param->bUseRandomSpin = FALSE;
}

void CPropertyPaneView::Refresh()
{
	switch( m_nMode ) {
		case 	NONE:
			break;
		case PTC_PROPERTY:
			break;
		case PTC_SETTING:
			break;
		case FX_SETTING:
			if( !m_vecFXSetting.empty() ) {
				if( CGlobalValue::GetInstance().FindItem( m_fxString ) ) {
					m_vecFXSetting[0]->SetVariable( (int)CGlobalValue::GetInstance().FindItem( m_fxString )->dwStartFrame );
					m_vecFXSetting[1]->SetVariable( (int)CGlobalValue::GetInstance().FindItem( m_fxString )->dwEndFrame );
				}
				RefreshPropertyGrid( &m_vecFXSetting );
			}
			break;			
		case FX_GLOBAL_SETTING:
			if( !m_vecGlobalFXSetting.empty() ) {
				int nMin, nMax;
				CGlobalValue::GetInstance().GetSequenceView()->GetSlider()->GetRange(nMin, nMax);	

				m_vecGlobalFXSetting[0]->SetVariable( (int)nMax );
				m_vecGlobalFXSetting[1]->SetVariable( (int)CRenderBase::GetInstance().GetBackgroundColor() );
				m_vecGlobalFXSetting[2]->SetVariable( CRenderBase::GetInstance().IsShowGrid() );
				m_vecGlobalFXSetting[3]->SetVariable( (char*)m_backgroundMesh.c_str() );
			}
			break;
		case POSITION:
			if( !m_vecPositionSetting.empty() ) {
				if( CGlobalValue::GetInstance().FindItem( m_fxString ) ) {
					m_vecPositionSetting[0]->SetVariable( CGlobalValue::GetInstance().GetAxisRenderObject()->GetPosition() );
					m_vecPositionSetting[1]->SetVariable( CGlobalValue::GetInstance().FindItem( m_fxString )->posType  );					
					RefreshPropertyGrid( &m_vecPositionSetting );
				}
			}
			break;
		case ROTATION:
			if( !m_vecRotationSetting.empty() ) {
				m_vecRotationSetting[0]->SetVariable( CGlobalValue::GetInstance().GetAxisRenderObject()->GetRotation() );
				RefreshPropertyGrid( &m_vecRotationSetting );
			}
			break;
	}	
}

void CPropertyPaneView::OnPositionSelected( EtVector3 pos )
{
	m_nMode = POSITION;
	PropertyGridBaseDefine Define[] = {
		{ "Parameter", "Position", CUnionValueProperty::Vector3, "위치", TRUE },
		{ "Parameter", "Interpolation Type", CUnionValueProperty::Integer_Combo, "Linear or Spline|Linear|Spline", TRUE },
		NULL,
	};

	
	SAFE_DELETE_PVEC( m_vecPositionSetting );	
	m_vecPositionSetting.clear();	
	CUnionValueProperty *pVariable = NULL;
	for( DWORD i=0; ; i++ ) {
		if( Define[i].szCategory == NULL ) break;
		pVariable = new CUnionValueProperty( Define[i].Type );
		pVariable->SetCategory( Define[i].szCategory );
		pVariable->SetDescription( Define[i].szStr );
		pVariable->SetSubDescription( Define[i].szDescription, true );
		pVariable->SetDefaultEnable( Define[i].bEnable );
		pVariable->SetCustomDialog( Define[i].bCustomDialog );
		m_vecPositionSetting.push_back( pVariable );
	}

	m_vecPositionSetting[0]->SetVariable( pos );
	m_vecPositionSetting[1]->SetVariable( CGlobalValue::GetInstance().FindItem( m_fxString )->posType );

	RefreshPropertyGrid( &m_vecPositionSetting );
}

void CPropertyPaneView::OnRotationSelected( EtVector3 rot )
{
	m_nMode = ROTATION;
	PropertyGridBaseDefine Define[] = {
		{ "Parameter", "Rotation", CUnionValueProperty::Vector3, "회전", TRUE },
		NULL,
	};

	
	SAFE_DELETE_PVEC( m_vecRotationSetting );	
	m_vecRotationSetting.clear();
	CUnionValueProperty *pVariable = NULL;
	for( DWORD i=0; ; i++ ) {
		if( Define[i].szCategory == NULL ) break;
		pVariable = new CUnionValueProperty( Define[i].Type );
		pVariable->SetCategory( Define[i].szCategory );
		pVariable->SetDescription( Define[i].szStr );
		pVariable->SetSubDescription( Define[i].szDescription, true );
		pVariable->SetDefaultEnable( Define[i].bEnable );
		pVariable->SetCustomDialog( Define[i].bCustomDialog );
		m_vecRotationSetting.push_back( pVariable );
	}

	m_vecRotationSetting[0]->SetVariable( rot );

	RefreshPropertyGrid( &m_vecRotationSetting );
}

void CPropertyPaneView::OnGlobalFXSelected()
{
	m_nMode = FX_GLOBAL_SETTING;
	PropertyGridBaseDefine Define[] = {
		{ "Setting", "Total Frame", CUnionValueProperty::Integer, "전체 프레임", TRUE },
		{ "Background", "Background Color", CUnionValueProperty::Color, "배경 색", TRUE },		
		{ "Background", "Show Grid", CUnionValueProperty::Boolean, "Grid Show/Hide", TRUE },
		{ "Background", "Skybox", CUnionValueProperty::String_FileOpen, "Skybox|Skin File (*.skn)|*.skn", TRUE },
		NULL,
	};
	
	SAFE_DELETE_PVEC( m_vecGlobalFXSetting );	
	m_vecGlobalFXSetting.clear();
	CUnionValueProperty *pVariable = NULL;
	for( DWORD i=0; ; i++ ) {
		if( Define[i].szCategory == NULL ) break;

		pVariable = new CUnionValueProperty( Define[i].Type );
		pVariable->SetCategory( Define[i].szCategory );
		pVariable->SetDescription( Define[i].szStr );
		pVariable->SetSubDescription( Define[i].szDescription, true );
		pVariable->SetDefaultEnable( Define[i].bEnable );
		pVariable->SetCustomDialog( Define[i].bCustomDialog );

		m_vecGlobalFXSetting.push_back( pVariable );
	}
	Refresh();

	RefreshPropertyGrid( &m_vecGlobalFXSetting );
}

void CPropertyPaneView::OnFXSelected( CString str )
{
	m_nMode = FX_SETTING;
	PropertyGridBaseDefine Define[] = {
		{ "Lifetime", "Start Frame", CUnionValueProperty::Integer, "시작 프레임", TRUE },
		{ "Lifetime", "End Frame", CUnionValueProperty::Integer, "끝 프레임", TRUE },
		NULL,
	};
	m_fxString = str;

	
	SAFE_DELETE_PVEC( m_vecFXSetting );	
	m_vecFXSetting.clear();
	CUnionValueProperty *pVariable = NULL;
	for( DWORD i=0; ; i++ ) {
		if( Define[i].szCategory == NULL ) break;
		pVariable = new CUnionValueProperty( Define[i].Type );
		pVariable->SetCategory( Define[i].szCategory );
		pVariable->SetDescription( Define[i].szStr );
		pVariable->SetSubDescription( Define[i].szDescription, true );
		pVariable->SetDefaultEnable( Define[i].bEnable );
		pVariable->SetCustomDialog( Define[i].bCustomDialog );
		m_vecFXSetting.push_back( pVariable );
	}

	PropertyGridBaseDefine PtcDefine[] = {
		{ "Parameter", "Iterator", CUnionValueProperty::Boolean, "Iterator", TRUE },
		{ "Parameter", "Loop", CUnionValueProperty::Boolean, "Loop", TRUE },
		{ "Parameter", "Stop", CUnionValueProperty::Boolean, "Stop", TRUE },
		NULL,
	};
	PropertyGridBaseDefine MeshDefine[] = {
		{ "Parameter", "바라보는 축", CUnionValueProperty::Integer_Combo, "바라보는축|None|X|-X|Y|-Y|Z|-Z", TRUE },
		{ "Parameter", "고정 축", CUnionValueProperty::Integer_Combo, "고정 축|None|X|Y|Z|Billboard", TRUE },
		{ "Blending", "Blend Mode", CUnionValueProperty::Integer_Combo, "블랜드 모드|ADD|SUBTRACT|REVSUBTRACT|MIN|MAX", TRUE },
		{ "Blending", "Source Blend", CUnionValueProperty::Integer_Combo, "Source Blend|ZERO|ONE|SRCCOLOR|INVSRCCOLOR|SRCALPHA|INVSRCALPHA|DESTALPHA|INVDESTALPHA|DESTCOLOR|INVDESTCOLOR|SRCALPHASAT", TRUE },
		{ "Blending", "Dest Blend", CUnionValueProperty::Integer_Combo, "Destination Blend|ZERO|ONE|SRCCOLOR|INVSRCCOLOR|SRCALPHA|INVSRCALPHA|DESTALPHA|INVDESTALPHA|DESTCOLOR|INVDESTCOLOR|SRCALPHASAT", TRUE },
		{ "Culling", "Cull Mode", CUnionValueProperty::Integer_Combo, "컬링 모드|One Side|Two Side", TRUE },
		{ "Tiling", "Tiling Mode", CUnionValueProperty::Integer_Combo, "타일링 모드|No Tiling|Tiling", TRUE },
		{ "DepthWrite", "Depth Write Mode", CUnionValueProperty::Integer_Combo, "깊이 쓰기 모드|No Write|Write", TRUE },
		NULL,
	};
	PropertyGridBaseDefine *pBranchDefine = NULL;//
	if( strstr(str.GetBuffer(), ".ptc") != NULL ) {
		pBranchDefine	= PtcDefine;
	}
	else {
		pBranchDefine = MeshDefine;
	}

	for( DWORD i=0; ; i++ ) {
		if( pBranchDefine[i].szCategory == NULL ) break;
		pVariable = new CUnionValueProperty( pBranchDefine[i].Type );
		pVariable->SetCategory( pBranchDefine[i].szCategory );
		pVariable->SetDescription( pBranchDefine[i].szStr );
		pVariable->SetSubDescription( pBranchDefine[i].szDescription, true );
		pVariable->SetDefaultEnable( pBranchDefine[i].bEnable );
		pVariable->SetCustomDialog( pBranchDefine[i].bCustomDialog );
		m_vecFXSetting.push_back( pVariable );
	}	

	m_vecFXSetting[0]->SetVariable( (int)CGlobalValue::GetInstance().FindItem( str )->dwStartFrame );
	m_vecFXSetting[1]->SetVariable( (int)CGlobalValue::GetInstance().FindItem( str )->dwEndFrame );

	if( CGlobalValue::GetInstance().FindItem( str )->Type == PARTICLE ) {
		m_vecFXSetting[2]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->bIterator );
		m_vecFXSetting[3]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->bLoop );
		m_vecFXSetting[4]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->bStopFlag );
	}
	else {
		m_vecFXSetting[2]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->nViewAxis );
		m_vecFXSetting[3]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->nFixedAxis );

		m_vecFXSetting[4]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->blendOP-1 );
		m_vecFXSetting[5]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->srcBlend-1 );
		m_vecFXSetting[6]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->destBlend-1 );
		m_vecFXSetting[7]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->cullMode );
		m_vecFXSetting[8]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->tilingMode );
		m_vecFXSetting[9]->SetVariable( CGlobalValue::GetInstance().FindItem( str )->zWriteMode );
	}

	RefreshPropertyGrid( &m_vecFXSetting );
}

void CPropertyPaneView::OnParticleSelected( bool *pLoop, bool *pIterate )
{
	m_nMode = PTC_SETTING;
	m_pLoop = pLoop;
	m_pIterate = pIterate;

	PropertyGridBaseDefine Define[] = {
		{ "Setting", "Loop", CUnionValueProperty::Boolean, "Loop", TRUE },
		{ "Setting", "Iterator", CUnionValueProperty::Boolean, "Iterator", TRUE },
		NULL,
	};

	
	SAFE_DELETE_PVEC( m_vecParticleSetting );	
	m_vecParticleSetting.clear();
	CUnionValueProperty *pVariable = NULL;
	for( DWORD i=0; ; i++ ) {
		if( Define[i].szCategory == NULL ) break;

		pVariable = new CUnionValueProperty( Define[i].Type );
		pVariable->SetCategory( Define[i].szCategory );
		pVariable->SetDescription( Define[i].szStr );
		pVariable->SetSubDescription( Define[i].szDescription, true );
		pVariable->SetDefaultEnable( Define[i].bEnable );
		pVariable->SetCustomDialog( Define[i].bCustomDialog );

		m_vecParticleSetting.push_back( pVariable );
	}

	 m_vecParticleSetting[0]->SetVariable( *m_pLoop );
	 m_vecParticleSetting[1]->SetVariable( *m_pIterate );

	RefreshPropertyGrid( &m_vecParticleSetting );
}

void CPropertyPaneView::OnEmpty()
{
	m_nMode = NONE;
	std::vector<CUnionValueProperty*> vecEmpty;	// 
	RefreshPropertyGrid( &vecEmpty );
}

void CPropertyPaneView::ApplyParticleSetting()
{
	if( m_pLoop ) 
		*m_pLoop = m_vecParticleSetting[0]->GetVariableBool();
	if( m_pIterate )
		*m_pIterate = m_vecParticleSetting[1]->GetVariableBool();
}

void CPropertyPaneView::ApplySetting()
{
	if( m_Emitter != NULL ) {
		UpdateData( m_Emitter->GetEmitterInfo(), false);
	}
}

void CPropertyPaneView::ApplyFXSetting(int index)
{
	if( m_vecFXSetting.empty() ) {
		return;
	}
	int nStart = m_vecFXSetting[ 0 ]->GetVariableInt();
	int nEnd = m_vecFXSetting[ 1 ]->GetVariableInt();

	CEtEffectDataContainer**ppTable = CGlobalValue::GetInstance().GetFXData( m_fxString.GetBuffer() );
	int maxRange = (int)(ppTable[ERT_POSITION_TABLE]->GetMaximumKeyValue()+1);
	maxRange = __max(maxRange, (int)(ppTable[ERT_ROTATE_TABLE]->GetMaximumKeyValue()+1));
	if( nEnd-nStart < maxRange ) {
		if( index == 0 ) {
			nStart = nEnd - maxRange;
			m_vecFXSetting[ 0 ]->SetVariable( nStart );
		}
		else if( index == 1 ) {
			nEnd = nStart + maxRange;
			m_vecFXSetting[ 1 ]->SetVariable( nEnd );
		}
		RefreshPropertyGrid( &m_vecFXSetting );
	}

	if( CGlobalValue::GetInstance().FindItem( m_fxString )->Type == PARTICLE ) {
		CGlobalValue::GetInstance().FindItem( m_fxString )->bIterator = m_vecFXSetting[ 2 ]->GetVariableBool() ? TRUE : FALSE;
		CGlobalValue::GetInstance().FindItem( m_fxString )->bLoop = m_vecFXSetting[ 3 ]->GetVariableBool() ? TRUE : FALSE;
		CGlobalValue::GetInstance().FindItem( m_fxString )->bStopFlag = m_vecFXSetting[ 4 ]->GetVariableBool() ? TRUE : FALSE;
	}
	else if( CGlobalValue::GetInstance().FindItem( m_fxString )->Type == MESH ) {
		CGlobalValue::GetInstance().FindItem( m_fxString )->nViewAxis = m_vecFXSetting[ 2 ]->GetVariableInt();
		CGlobalValue::GetInstance().FindItem( m_fxString )->nFixedAxis = m_vecFXSetting[ 3 ]->GetVariableInt();

		CGlobalValue::GetInstance().FindItem( m_fxString )->blendOP = m_vecFXSetting[ 4 ]->GetVariableInt()+1;
		CGlobalValue::GetInstance().FindItem( m_fxString )->srcBlend = m_vecFXSetting[ 5 ]->GetVariableInt()+1;
		CGlobalValue::GetInstance().FindItem( m_fxString )->destBlend = m_vecFXSetting[ 6 ]->GetVariableInt()+1;
		CGlobalValue::GetInstance().FindItem( m_fxString )->cullMode = m_vecFXSetting[ 7 ]->GetVariableInt();
		CGlobalValue::GetInstance().FindItem( m_fxString )->tilingMode = m_vecFXSetting[ 8 ]->GetVariableInt();
		CGlobalValue::GetInstance().FindItem( m_fxString )->zWriteMode = m_vecFXSetting[ 9 ]->GetVariableInt();
	}

	CGlobalValue::GetInstance().FindItem( m_fxString )->dwStartFrame = nStart;
	CGlobalValue::GetInstance().FindItem( m_fxString )->dwEndFrame = nEnd;
	CGlobalValue::GetInstance().GetSequenceView()->SetFrame( m_fxString, nStart, nEnd);
}

void CPropertyPaneView::ApplyGlobalFXSetting()
{
	if( m_vecGlobalFXSetting.empty() ) {
		return;
	}
	int nRangeMax = m_vecGlobalFXSetting[0]->GetVariableInt();
	int nMin, nMax;
	CGlobalValue::GetInstance().GetSequenceView()->GetSlider()->GetRange(nMin, nMax);
	if( nMax != nRangeMax ) {
		CGlobalValue::GetInstance().GetSequenceView()->SetSliderRange(0,  nRangeMax);
		CGlobalValue::GetInstance().GetSequenceView()->RefreshSlider();
	}
	COLORREF Color = m_vecGlobalFXSetting[1]->GetVariableInt();
	CRenderBase::GetInstance().SetBackgroundColor( Color );
	CRenderBase::GetInstance().SetShowGrid( m_vecGlobalFXSetting[2]->GetVariableBool() );
}

void CPropertyPaneView::ApplyPositionSetting()
{
	if( m_vecPositionSetting.empty() ) {
		return;
	}
	EtVector3 Pos = m_vecPositionSetting[0]->GetVariableVector3();

	int nPosType = m_vecPositionSetting[1]->GetVariableInt();
	CGlobalValue::GetInstance().FindItem( m_fxString )->posType = nPosType;

	if( CGlobalValue::GetInstance().GetPositionPtr() ) {
		*CGlobalValue::GetInstance().GetPositionPtr() = Pos;
		CGlobalValue::GetInstance().GetAxisRenderObject()->SetPosition( Pos );
	}
}

void CPropertyPaneView::ApplyRotationSetting()
{
	if( m_vecRotationSetting.empty() ) {
		return;
	}
	EtVector3 Rot = m_vecRotationSetting[0]->GetVariableVector3();
	if( CGlobalValue::GetInstance().GetRotationPtr() ) {
		*CGlobalValue::GetInstance().GetRotationPtr() = Rot;
		CGlobalValue::GetInstance().GetAxisRenderObject()->SetRotation( Rot );
	}
}

void CPropertyPaneView::OnEmitterSelected( CEtBillboardEffectEmitter *pEmitter )
{
	m_nMode = PTC_PROPERTY;
	ApplySetting();	
	SEmitter param;
	PropertyGridBaseDefine Define[] = {
		{ "Emitter", "Count", CUnionValueProperty::Integer, "개수|1|500|1", TRUE },
		{ "Emitter", "Lifetime", CUnionValueProperty::Integer, "파티클 Lifetime|1|300|1", TRUE },
		{ "Emitter", "Creation Time", CUnionValueProperty::Integer, "파티클 CreationTime|1|300|1", TRUE },		
		{ "Emitter", "Start Inner Radius", CUnionValueProperty::Vector3, "파티클 시작랜덤위치 Min", TRUE },
		{ "Emitter", "Start Outer Radius", CUnionValueProperty::Vector3, "파티클 시작랜덤위치 Max", TRUE },
		{ "Emitter", "Random Size Rate", CUnionValueProperty::Integer, "파티클 크기 랜덤보정 비율(%%)|0|100|1", TRUE },
		{ "Emitter-Opt", "Mid Count", CUnionValueProperty::Integer, "중옵션 개수(0이면 안보임)|0|500|0", TRUE },
		{ "Emitter-Opt", "Low Count", CUnionValueProperty::Integer, "하옵션 개수(0이면 안보임)|0|500|0", TRUE },
		{ "Movement", "Speed Min", CUnionValueProperty::Integer, "속도 최저|0|2500|1", TRUE },
		{ "Movement", "Speed Max", CUnionValueProperty::Integer, "속도 최대|0|2500|1", TRUE },
		{ "Movement", "Shooting Angle", CUnionValueProperty::Integer, "방출 각도|0|180|1", TRUE },
		{ "Movement", "Launch Vector", CUnionValueProperty::Vector3, "방출 방향", TRUE },		
		{ "Movement", "Rotate Start", CUnionValueProperty::Integer, "회전 시작 각도|0|3600|1", TRUE },
		{ "Movement", "Rotate End", CUnionValueProperty::Integer, "회전 마지막 각도|0|3600|1", TRUE },
		{ "Movement", "Random Rotate", CUnionValueProperty::Boolean, "랜덤 회전", TRUE },
		{ "Movement", "Use Vector Speed", CUnionValueProperty::Boolean, "진행방향으로 UV회전", TRUE },
		{ "Movement", "Gravity Accel", CUnionValueProperty::Integer, "중력 세기|0|100|1", TRUE },
		{ "Movement", "Gravity Vector", CUnionValueProperty::Vector3, "중력 방향", TRUE },
		{ "Movement", "Resist Time", CUnionValueProperty::Integer, "공기 저항 시간|0|100|1", TRUE },
		{ "Movement", "Resist Scale", CUnionValueProperty::Integer, "공기 저항 크기|0|100|1", TRUE },
		{ "Blending", "Texture Repeat", CUnionValueProperty::Integer, "텍스쳐 반복|1|20|1", TRUE },
		{ "Blending", "Blend Mode", CUnionValueProperty::Integer_Combo, "블랜드 모드|ADD|SUBTRACT|REVSUBTRACT|MIN|MAX", TRUE },
		{ "Blending", "Source Blend", CUnionValueProperty::Integer_Combo, "Source Blend|ZERO|ONE|SRCCOLOR|INVSRCCOLOR|SRCALPHA|INVSRCALPHA|DESTALPHA|INVDESTALPHA|DESTCOLOR|INVDESTCOLOR|SRCALPHASAT", TRUE },
		{ "Blending", "Dest Blend", CUnionValueProperty::Integer_Combo, "Destination Blend|ZERO|ONE|SRCCOLOR|INVSRCCOLOR|SRCALPHA|INVSRCALPHA|DESTALPHA|INVDESTALPHA|DESTCOLOR|INVDESTCOLOR|SRCALPHASAT", TRUE },
		{ "Blending", "Texture", CUnionValueProperty::String_FileOpen, "텍스쳐 파일|텍스쳐 파일(*.dds)|*.dds", TRUE },
		{ "Misc", "Ignore Iterator", CUnionValueProperty::Boolean, "Iterator 무시", TRUE },
		{ "Misc", "Use OffsetMap", CUnionValueProperty::Boolean, "Offset Map 사용", TRUE },
		{ "Misc", "Use Y BillBoard", CUnionValueProperty::Boolean, "Y 빌보드 사용", TRUE },
		{ "Misc", "Use Time Random Seed", CUnionValueProperty::Boolean, "완전 랜덤 파티클", TRUE },
		NULL,
	};

	SAFE_DELETE_PVEC( m_vecPropertyList );
	m_vecPropertyList.clear();
	CUnionValueProperty *pVariable = NULL;
	for( DWORD i=0; ; i++ ) {
		if( Define[i].szCategory == NULL ) break;

		pVariable = new CUnionValueProperty( Define[i].Type );
		pVariable->SetCategory( Define[i].szCategory );
		pVariable->SetDescription( Define[i].szStr );
		pVariable->SetSubDescription( Define[i].szDescription, true );
		pVariable->SetDefaultEnable( Define[i].bEnable );
		pVariable->SetCustomDialog( Define[i].bCustomDialog );

		m_vecPropertyList.push_back( pVariable );
	}

	UpdateData( pEmitter->GetEmitterInfo(), true);
	m_Emitter = pEmitter;	

	RefreshPropertyGrid( &m_vecPropertyList );
}

void CPropertyPaneView::RefreshProperty()
{
	RefreshPropertyGrid( &m_vecPropertyList );
}

void CPropertyPaneView::UpdateData( SEmitter *Param, bool bSave )
{
	// TRUE -> Param->PropertyGrid
	// FALSE -> PropertyGrid->Param	
	if( m_vecPropertyList.empty() ) {
		return;
	}
	if( bSave ) {
		int n=0;
		m_vecPropertyList[ n++ ]->SetVariable( Param->nGenerateCount );
		m_vecPropertyList[ n++ ]->SetVariable( Param->nLifeDuration );
		m_vecPropertyList[ n++ ]->SetVariable( Param->nBeginningTime );
		m_vecPropertyList[ n++ ]->SetVariable( EtVector3(Param->vStartPosInnerRadius.x, Param->vStartPosInnerRadius.y, Param->vStartPosInnerRadius.z) );
		m_vecPropertyList[ n++ ]->SetVariable( EtVector3(Param->vStartPosOuterRadius.x, Param->vStartPosOuterRadius.y, Param->vStartPosOuterRadius.z) );
		m_vecPropertyList[ n++ ]->SetVariable( Param->nSizeAdjustRate );
		m_vecPropertyList[ n++ ]->SetVariable( Param->nOptionCountMid );
		m_vecPropertyList[ n++ ]->SetVariable( Param->nOptionCountLow );
		m_vecPropertyList[ n++ ]->SetVariable( (int)(Param->fSpeedVelocityMin));
		m_vecPropertyList[ n++ ]->SetVariable( (int)(Param->fSpeedVelocityMax));
		m_vecPropertyList[ n++ ]->SetVariable( (int)(Param->vInitiateLaunch.w * 180.f / ET_PI) );	
		m_vecPropertyList[ n++ ]->SetVariable( EtVector3(Param->vInitiateLaunch.x, Param->vInitiateLaunch.y, Param->vInitiateLaunch.z) );		
		m_vecPropertyList[ n++ ]->SetVariable( (int)Param->fZRotateStartValue );
		m_vecPropertyList[ n++ ]->SetVariable( (int)Param->fZRotateEndValue );
		m_vecPropertyList[ n++ ]->SetVariable( (int)!Param->bUseRandomSpin );		
		m_vecPropertyList[ n++ ]->SetVariable( Param->nUseRotateByDir );
		m_vecPropertyList[ n++ ]->SetVariable( (int)Param->fFallSpeed );
		m_vecPropertyList[ n++ ]->SetVariable( Param->vFallVector );
		m_vecPropertyList[ n++ ]->SetVariable( (int)(Param->fOpposeTime*100.f) );
		m_vecPropertyList[ n++ ]->SetVariable( (int)(Param->fOpposeScaleValue*100.f) );
		m_vecPropertyList[ n++ ]->SetVariable( (int)Param->fDummy );
		m_vecPropertyList[ n++ ]->SetVariable( Param->nBlendOP );
		m_vecPropertyList[ n++ ]->SetVariable( Param->nSourceBlendMode );
		m_vecPropertyList[ n++ ]->SetVariable( Param->nDestBlendMode );
		m_vecPropertyList[ n++ ]->SetVariable( Param->szTextureName );
		m_vecPropertyList[ n++ ]->SetVariable( Param->bDisregardTracePos );
		m_vecPropertyList[ n++ ]->SetVariable( Param->bUseBumpEffectTexture );
		m_vecPropertyList[ n++ ]->SetVariable( Param->bUseYBillBoard );
		m_vecPropertyList[ n++ ]->SetVariable( Param->bUseTimeRandomSeed );
	}
	else {
		int n=0;
		Param->nGenerateCount = m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->nLifeDuration = m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->nBeginningTime = m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->vStartPosInnerRadius = m_vecPropertyList[ n++ ]->GetVariableVector3();
		Param->vStartPosOuterRadius = m_vecPropertyList[ n++ ]->GetVariableVector3();
		Param->nSizeAdjustRate = m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->nOptionCountMid = m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->nOptionCountLow = m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->fSpeedVelocityMin = (float)m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->fSpeedVelocityMax = (float)m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->vInitiateLaunch.w = (float)m_vecPropertyList[ n++ ]->GetVariableInt() * ET_PI / 180.f;
		*(EtVector3*)&Param->vInitiateLaunch = m_vecPropertyList[ n++ ]->GetVariableVector3();
		EtVec3Normalize((EtVector3*)&Param->vInitiateLaunch, (EtVector3*)&Param->vInitiateLaunch);
		Param->fZRotateStartValue = (float)m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->fZRotateEndValue = (float)m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->bUseRandomSpin = !m_vecPropertyList[ n++ ]->GetVariableBool();	
		Param->nUseRotateByDir = m_vecPropertyList[ n++ ]->GetVariableBool();
		Param->fFallSpeed = (float)m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->vFallVector = m_vecPropertyList[ n++ ]->GetVariableVector3();
		EtVec3Normalize(&Param->vFallVector, &Param->vFallVector);
		Param->fOpposeTime = (float)m_vecPropertyList[ n++ ]->GetVariableInt() / 100.f;
		Param->fOpposeScaleValue = (float)m_vecPropertyList[ n++ ]->GetVariableInt() / 100.f;
		Param->fDummy = (float)m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->nBlendOP = m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->nSourceBlendMode = m_vecPropertyList[ n++ ]->GetVariableInt();
		Param->nDestBlendMode = m_vecPropertyList[ n++ ]->GetVariableInt();
		strcpy_s(Param->szTextureName, m_vecPropertyList[ n++ ]->GetVariableString());
		Param->bDisregardTracePos = m_vecPropertyList[ n++ ]->GetVariableBool();
		Param->bUseBumpEffectTexture = m_vecPropertyList[ n++ ]->GetVariableBool();
		Param->bUseYBillBoard = m_vecPropertyList[ n++ ]->GetVariableBool();
		Param->bUseTimeRandomSeed = m_vecPropertyList[ n++ ]->GetVariableBool();
	}
}

void CPropertyPaneView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( m_PropertyGrid ) {
		CRect rcRect;
		GetClientRect( &rcRect );
		m_PropertyGrid.MoveWindow( &rcRect );
	}
}

LRESULT CPropertyPaneView::OnNotifyGrid( WPARAM wParam, LPARAM lParam )
{
	return ProcessNotifyGrid( wParam, lParam );
}

void CPropertyPaneView::OnChangeValue( CUnionValueProperty *pVariable, DWORD dwIndex )
{
	switch( m_nMode ) {
		case 	NONE:
			break;
		case PTC_PROPERTY:
			{
				const int textureIndex = 24;
				if( dwIndex == textureIndex ) {	// Texture Change
					char szFileName[255];
					strcpy_s(szFileName, m_vecPropertyList[ textureIndex ]->GetVariableString());
					m_Emitter->LoadTexture( szFileName );
					char *szName = szFileName;
					char *ps = strrchr(szName, '\\');
					if( ps ) szName = ps+1;
					m_vecPropertyList[ textureIndex ]->SetVariable( szName );
					CGlobalValue::GetInstance().GetParticlePaneView()->RenameTexture( szName );
				}
			}
			ApplySetting();
			break;
		case PTC_SETTING:
			ApplyParticleSetting();
			break;
		case FX_SETTING:
			ApplyFXSetting(dwIndex);
			break;			
		case FX_GLOBAL_SETTING:
			{
				const int bgMeshIndex = 3;
				if( dwIndex == bgMeshIndex ) {
					char szFileName[255];
					strcpy_s(szFileName, m_vecGlobalFXSetting[ bgMeshIndex ]->GetVariableString());
					EtObjectHandle handle = EternityEngine::CreateStaticObject( szFileName );
					//CGlobalValue::GetInstance().SetGlobalMesh( handle->GetMyIndex() );
				}
			}
			ApplyGlobalFXSetting();
			break;
		case POSITION:
			ApplyPositionSetting();
			break;
		case ROTATION:
			ApplyRotationSetting();
			break;
	}
					
	CGlobalValue::GetInstance().GetParticlePaneView()->OnValueChange();
}

void CPropertyPaneView::RefreshPointer()
{
	m_Emitter = NULL; 
	m_pLoop = NULL; 
	m_pIterate = NULL; 

	SAFE_DELETE_PVEC(m_vecPropertyList);
	SAFE_DELETE_PVEC(m_vecParticleSetting);
	SAFE_DELETE_PVEC(m_vecFXSetting);
	SAFE_DELETE_PVEC(m_vecGlobalFXSetting);
	SAFE_DELETE_PVEC(m_vecPositionSetting);
	SAFE_DELETE_PVEC(m_vecRotationSetting);
	m_vecPropertyList.clear();
	m_vecParticleSetting.clear();
	m_vecFXSetting.clear();
	m_vecGlobalFXSetting.clear();
	m_vecPositionSetting.clear();
	m_vecRotationSetting.clear();
}
