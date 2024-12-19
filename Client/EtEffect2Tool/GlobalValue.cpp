#include "StdAfx.h"
#include "GlobalValue.h"
#include "Stream.h"
#include "SundriesFunc.h"
#include "SequenceView.h"
#include "EtEffect2Tool.h"
#include "ParticlePaneView.h"
#include "FXPaneView.h"
#include "EtEffectDataContainer.h"
#include "KeyControlPaneView.h"
#include "AxisRenderObject.h"
#include "MainFrm.h"
#include "EtResourceMng.h"
#include "RenderBase.h"
#include "PropertyPaneView.h"

CGlobalValue g_GlobalValue;

CGlobalValue::CGlobalValue()
{
	m_pActiveView = NULL;
	m_paneView = NULL;
	m_particleView = NULL;
	m_nActiveView = 0;
	m_dwCurFrame = 0;
	m_fCurFrame = 0.f;

	m_nPrevTick = 0;
	m_nCurTick = 0;
	m_fFloatTick = 0.f;
	m_playTick = 0;
	m_bPlay = false;
	m_posOrRot = -1;
	m_pAxisRenderObject = NULL;
	m_positionPtr = NULL;
	m_rotationPtr = NULL;
	m_bLoopPlay = false;
}

CGlobalValue::~CGlobalValue()
{

}

void CGlobalValue::Release()
{
	Reset();
	SAFE_DELETE( m_pAxisRenderObject );
}
void CGlobalValue::Reset()
{
	int i, nSize;
	nSize = (int)m_Items.size();
	for( i = 0; i < nSize; i++) {		
		if( m_Items[i].Type == PARTICLE ) {
			if( m_Items[i].nRenderIndex != -1 && m_Items[i].nObjectIndex != -1 ) {
				EtBillboardEffectHandle hParticle = CEtBillboardEffect::GetSmartPtr( m_Items[i].nObjectIndex );
				SAFE_RELEASE_SPTR( hParticle );
				m_Items[i].nObjectIndex = -1;
				EternityEngine::DeleteParticleData( m_Items[i].nRenderIndex );
			}
		}
		else if( m_Items[i].Type == MESH ) {
			if( m_Items[i].nObjectIndex != -1 ) {
				EtObjectHandle hObject = CEtObject::GetSmartPtr( m_Items[i].nObjectIndex );
				if( hObject ) {
					hObject->Release();
					m_Items[i].nObjectIndex = -1;
				}
			}
		}
		for( int j = 0; j < 5; j++) {
			SAFE_DELETE( m_Items[i].FxData[j] );
		}
		for( int j = 0; j < 3; j++) {
			SAFE_DELETE( m_Items[i].ScaleData[j] );
		}
	}
	m_Items.clear();
	m_fxView->Reset();
	m_gridView->Reset();
	m_particleView->Reset();
}

void CGlobalValue::CalcTick( float fElapsedTime )
{
	m_nPrevTick = m_nCurTick;
	m_fFloatTick += ( fElapsedTime * 60 );
	int nTickAdd = ( int )( m_fFloatTick );
	m_nCurTick += nTickAdd;
	m_fFloatTick -= nTickAdd;

	if( m_bPlay ) {
		int nMin, nMax;
		m_gridView->GetSlider()->GetRange(nMin, nMax);
		assert( nMin == 0 );
		int curFrame = m_gridView->GetSlider()->GetPos() + nTickAdd;
		if( curFrame >= nMax ) {
			if( m_bLoopPlay ) {
				curFrame -= nMax;
			}
			else {
				curFrame = nMax;
				m_bPlay = false;
			}
		}
		m_gridView->GetSlider()->SetPos( curFrame );
		m_keyControlView->GetSliderCtrl()->GetRange(nMin, nMax);
		assert( nMin == 0 );
		curFrame = m_keyControlView->GetSliderCtrl()->GetPos() + nTickAdd;
		if( curFrame > nMax ) {
			curFrame = nMax;
		}
		m_keyControlView->GetSliderCtrl()->SetPos( curFrame );
	}

	if( nTickAdd != 0 && m_bPlay) {
		m_gridView->Invalidate();
		m_keyControlView->Invalidate();
	}

	if ( m_hParticle ) {
		if ( m_hParticle->IsTracePos() ) {
			EtMatrix WorldMat;
			m_iteratePos += fElapsedTime * 800.f;
			EtMatrixTranslation( &WorldMat, m_iteratePos, 0.f, 0.f );	
			m_hParticle->SetWorldMat( &WorldMat );
		}
	}
}

void CGlobalValue::SetPosOrRot( int posOrRot )
{
	m_pAxisRenderObject->Enable( true );
	if( posOrRot == -1 ) {
		m_keyControlView->SetRangeMax(2);
		m_pAxisRenderObject->Enable( false );
		SetPositionPtr( NULL );
		SetRotationPtr( NULL );
	}
	m_posOrRot = posOrRot;
}

void CGlobalValue::PlayFXObject()
{
	m_bPlay = !m_bPlay;	

	if( m_bPlay ) {
		int nMin, nMax;
		m_gridView->GetSlider()->GetRange(nMin, nMax);
		assert( nMin == 0 );
		int curFrame = m_gridView->GetSlider()->GetPos();
		if( curFrame >= nMax ) {
			curFrame = 0;
		}
		m_gridView->GetSlider()->SetPos( curFrame );
		m_keyControlView->GetSliderCtrl()->GetRange(nMin, nMax);
		assert( nMin == 0 );
		curFrame = m_keyControlView->GetSliderCtrl()->GetPos();
		if( curFrame >= nMax ) {
			curFrame = 0;
		}
		m_keyControlView->GetSliderCtrl()->SetPos( curFrame );
	}

	int i, nSize;
	nSize = (int)m_Items.size();

	for( i = 0; i < nSize; i++) {
		if( m_Items[i].Type == PARTICLE && m_Items[i].nObjectIndex != -1 ) {
			EtBillboardEffectHandle hParticle = CEtBillboardEffect::GetSmartPtr( m_Items[i].nObjectIndex );
			if( hParticle ) {
				for( int j = 0; j < hParticle->GetBillboardEffectData()->GetEmitterCount(); j++) {
					hParticle->GetBillboardEffectData()->EnableDraw(j, true);
				}
			}
		}
	}
}

void CGlobalValue::PrevFXObject()
{
	int nPos = m_gridView->GetSlider()->GetPos();
	nPos--;
	int nMin, nMax;
	m_gridView->GetSlider()->GetRange(nMin, nMax);
	m_bPlay = false;
	assert( nMin == 0 );
	if( nPos < 0) {
		nPos += (nMax + 1);				
	}
	m_gridView->GetSlider()->SetPos( nPos );
	m_gridView->Invalidate();

	//////////////////////////////////////////////////////////////////////////
	nPos = m_keyControlView->GetSliderCtrl()->GetPos();
	nPos--;
	m_keyControlView->GetSliderCtrl()->GetRange(nMin, nMax);
	assert( nMin == 0 );
	if( nPos < 0) {
		nPos += (nMax + 1);				
	}
	m_keyControlView->GetSliderCtrl()->SetPos( nPos );	
	m_keyControlView->Invalidate();
}

void CGlobalValue::NextFXObject()
{
	int nPos = m_gridView->GetSlider()->GetPos();
	nPos++;
	int nMin, nMax;
	m_gridView->GetSlider()->GetRange(nMin, nMax);
	m_bPlay = false;
	assert( nMin == 0 );
	if( nPos > nMax) {
		nPos -= (nMax + 1);
	}
	m_gridView->GetSlider()->SetPos( nPos );
	m_gridView->Invalidate();
	//////////////////////////////////////////////////////////////////////////
	nPos = m_keyControlView->GetSliderCtrl()->GetPos();
	nPos++;
	m_keyControlView->GetSliderCtrl()->GetRange(nMin, nMax);
	m_bPlay = false;
	assert( nMin == 0 );
	if( nPos > nMax) {
		nPos -= (nMax + 1);
	}
	m_keyControlView->GetSliderCtrl()->SetPos( nPos );
	m_keyControlView->Invalidate();
}

void CGlobalValue::InitDefaultTable(CEtEffectDataContainer **ppTable)
{
	// 기본 테이블 등록
	CEtEffectAlphaDataValue *pAlphaValue1 = new CEtEffectAlphaDataValue( 0.f, 1.f );
	CEtEffectAlphaDataValue *pAlphaValue2 = new CEtEffectAlphaDataValue( 1.f, 1.f );
	ppTable[ERT_ALPHA_TABLE]->AddData( pAlphaValue1 );
	ppTable[ERT_ALPHA_TABLE]->AddData( pAlphaValue2 );

	CEtEffectColorDataValue *pColorValue1 = new CEtEffectColorDataValue( 0.f, D3DXVECTOR3( 1, 1, 1 ) );
	CEtEffectColorDataValue *pColorValue2 = new CEtEffectColorDataValue( 1.f, D3DXVECTOR3( 1, 1, 1 ) );
	ppTable[ERT_COLOR_TABLE]->AddData( pColorValue1 );
	ppTable[ERT_COLOR_TABLE]->AddData( pColorValue2 );

	CEtEffectScaleDataValue *pScaleValue1 = new CEtEffectScaleDataValue( 0.f, 1.f );
	CEtEffectScaleDataValue *pScaleValue2 = new CEtEffectScaleDataValue( 1.f, 1.f );
	ppTable[ERT_SCALE_TABLE]->AddData( pScaleValue1 );
	ppTable[ERT_SCALE_TABLE]->AddData( pScaleValue2 );

	CEtEffectPositionDataValue *pPositionValue = new CEtEffectPositionDataValue( 0.f, D3DXVECTOR3( 0, 0, 0 ) );
	ppTable[ERT_POSITION_TABLE]->AddData( pPositionValue );

	CEtEffectRotationDataValue *pRotationValue = new CEtEffectRotationDataValue( 0.f, D3DXVECTOR3( 0, 0, 0 ) );
	ppTable[ERT_ROTATE_TABLE]->AddData( pRotationValue );
}

void CGlobalValue::CalcColor( EtColor *pOutColor,  float fWeight, CEtEffectDataContainer **pTable )
{
	float fAlpha, *pAlpha;
	EtVector3 *pColor;

	pAlpha = pTable[ ERT_ALPHA_TABLE ]->GetInterpolationFloat( fWeight );
	if( pAlpha ) fAlpha = *pAlpha;
	else fAlpha = 1.0f;
	pColor = pTable[ ERT_COLOR_TABLE ]->GetInterpolationVector3( fWeight );
	if( pColor ) *pOutColor = EtColor( pColor->x, pColor->y, pColor->z, fAlpha );
	else *pOutColor = EtColor( 1.0f, 1.0f, 1.0f, fAlpha );
}
void CGlobalValue::CalcBillboardMatrix( EtMatrix &BillboardMat, EtVector3 *pUpVec, EtVector3 *pDir, EtVector3 *pPosition, EtVector3 *pCameraPos )
{
	float fLengthSq;
	EtVector3 XVec, UpVec, ZVec;

	fLengthSq = EtVec3LengthSq( pUpVec );
	if( fLengthSq <= 0.0f )
	{
		UpVec = *pDir;//EtVector3( 0.0f, 1.0f, 0.0f );
	}
	else
	{
		UpVec = *pUpVec;
		EtVec3Normalize( &UpVec, &UpVec );
	}

	ZVec = *pCameraPos - *pPosition;
	EtVec3Normalize( &ZVec, &ZVec );
	EtVec3Cross( &XVec, &UpVec, &ZVec );
	EtVec3Normalize( &XVec, &XVec );
	if( fLengthSq <= 0.0f )
	{
		EtVec3Cross( &UpVec, &ZVec, &XVec );
		EtVec3Normalize( &UpVec, &UpVec );
	}
	else
	{
		EtVec3Cross( &ZVec, &XVec, &UpVec );
		EtVec3Normalize( &ZVec, &ZVec );
	}

	EtMatrixIdentity( &BillboardMat );
	memcpy( &BillboardMat._11, &XVec, sizeof( EtVector3 ) );
	memcpy( &BillboardMat._21, &UpVec, sizeof( EtVector3 ) );
	memcpy( &BillboardMat._31, &ZVec, sizeof( EtVector3 ) );
	memcpy( &BillboardMat._41, pPosition, sizeof( EtVector3 ) );
}

void CGlobalValue::CalcMeshWorldMat( EtMatrix *pMat,  float fTime, float fWeight, int nViewAxis, int nFixedAxis, CEtEffectDataContainer **ppTable, CEtEffectDataContainer **ppScaleTable, int posType )
{
	float fScale;
	EtVector3 *pPosition, ScaleVec;
	EtQuat Quat;

	pPosition = ppTable[ ERT_POSITION_TABLE ]->GetInterpolationVector3( fTime, NULL, (posType == 1) );
	ASSERT( pPosition );

	fScale = *ppTable[ ERT_SCALE_TABLE ]->GetInterpolationFloat( fWeight );
	ScaleVec = EtVector3( 1.0f, 1.0f, 1.0f );	
	for(int i = 0; i < 3; i++ )
	{
		if( ppScaleTable[ i ]->GetDataCount() )
		{
			ScaleVec[ i ] = *ppScaleTable[ i ]->GetInterpolationFloat( fWeight );
		}
	}
	ScaleVec *= fScale;

	
	{
		if( ppTable[ ERT_ROTATE_TABLE ]->GetDataCount() == 1 )
		{
			EtVector3 *pTemp;

			pTemp = ( EtVector3 * )( ppTable[ ERT_ROTATE_TABLE ]->GetValueFromIndex( 0 )->GetValue() );
			EtQuaternionRotationYawPitchRoll( &Quat, EtToRadian( pTemp->x ), EtToRadian( pTemp->y ), EtToRadian( pTemp->z ) );
		}
		else
		{
			Quat = *ppTable[ ERT_ROTATE_TABLE ]->GetInterpolationQuat( fTime );
		}
		EtMatrixTransformation( pMat, NULL, NULL, &ScaleVec, NULL, &Quat, pPosition );
	}
	if( nViewAxis == 0 && nFixedAxis == 4 ) {
		EtMatrix InvViewMat, ScaleMat;
		EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
		InvViewMat = *hCamera->GetInvViewMat();
		InvViewMat._41 = InvViewMat._42 = InvViewMat._43 = 0.f;
		EtMatrixScaling( &ScaleMat, ScaleVec.x, ScaleVec.y, ScaleVec.z );
		EtMatrixMultiply(pMat, &ScaleMat, &InvViewMat);
		pMat->_41 = pPosition->x;
		pMat->_42 = pPosition->y;
		pMat->_43 = pPosition->z;
	}
	else if( nViewAxis != 0 && nFixedAxis != 0 )
	{
		EtCameraHandle hCamera;
		EtMatrix InvViewMat, ScaleMat, RotateMat, BillboardMat;
		EtVector3 vTempPos, vFixedVec;

		hCamera = CEtCamera::GetActiveCamera();
		InvViewMat = *hCamera->GetInvViewMat();

		switch( nFixedAxis )
		{
		case 1:	
			vFixedVec = EtVector3( 1.f, 0.f, 0.f );	
			break;
		case 2:	
			vFixedVec = EtVector3( 0.f, 1.f, 0.f );	
			break;
		case 3:	
			vFixedVec = EtVector3( 0.f, 0.f, 1.f );	
			break;
		case 4: 
			vFixedVec = EtVector3( 0.f, 1.f, 0.f );	
			break;
		}		

		vTempPos = *pPosition;
		CalcBillboardMatrix( BillboardMat, &vFixedVec, (EtVector3*)&pMat->_21,&vTempPos, ( EtVector3 * )&InvViewMat._41 );

		switch( nViewAxis )
		{
		case 1: 
			EtMatrixRotationY( &RotateMat, EtToRadian( -90.0f ) );
			break;
		case 2: 
			EtMatrixRotationY( &RotateMat, EtToRadian( 90.0f ) );
			break;
		case 3:	
			EtMatrixRotationX( &RotateMat, EtToRadian( 90.0f ) );
			break;
		case 4:	
			EtMatrixRotationX( &RotateMat, EtToRadian( -90.0f ) );
			break;
		case 5:
			EtMatrixIdentity( &RotateMat );
			break;
		case 6: 
			EtMatrixRotationX( &RotateMat, EtToRadian( 180.0f ) );
			break;
		}
		EtMatrixMultiply( &BillboardMat, &RotateMat, &BillboardMat );

		EtMatrixScaling( &ScaleMat, ScaleVec.x, ScaleVec.y, ScaleVec.z );
		EtMatrixMultiply( pMat, &ScaleMat, &BillboardMat );
	}
}

void CGlobalValue::CalcPtcWorldMat( EtMatrix *pMat,  float fTime, float fWeight, CEtEffectDataContainer **pTable, int posType )
{
	float fScale;
	EtVector3 *pPosition;
	EtQuat Quat;

	pPosition = pTable[ ERT_POSITION_TABLE ]->GetInterpolationVector3( fTime, NULL, (posType == 1) );
	ASSERT( pPosition );

	fScale = *pTable[ ERT_SCALE_TABLE ]->GetInterpolationFloat( fWeight );
	if( pTable[ ERT_ROTATE_TABLE ]->GetDataCount() == 1 )
	{
		EtVector3 *pTemp;

		pTemp = ( EtVector3 * )( pTable[ ERT_ROTATE_TABLE ]->GetValueFromIndex( 0 )->GetValue() );
		EtQuaternionRotationYawPitchRoll( &Quat, EtToRadian( pTemp->x ), EtToRadian( pTemp->y ), EtToRadian( pTemp->z ) );
	}
	else
	{
		Quat = *pTable[ ERT_ROTATE_TABLE ]->GetInterpolationQuat( fTime );
	}

	EtMatrixTransformation( pMat, NULL, NULL, &EtVector3( fScale, fScale, fScale ), NULL, &Quat, pPosition );
}

CEtEffectDataContainer** CGlobalValue::GetFXData( CString str )
{
	int i, nSize;
	nSize = (int)m_Items.size();
	for( i = 0; i < nSize; i++) {
		if(strcmp(str.GetBuffer(), m_Items[i].szDesc) == 0 ) {
			return m_Items[i].FxData;
		}
	}
	return NULL;
}

CEtEffectDataContainer** CGlobalValue::GetScaleData( CString str )
{
	int i, nSize;
	nSize = (int)m_Items.size();
	for( i = 0; i < nSize; i++) {
		if(strcmp(str.GetBuffer(), m_Items[i].szDesc) == 0 ) {
			return m_Items[i].ScaleData;
		}
	}
	return NULL;
}

Item* CGlobalValue::FindItem( CString str )
{
	int i, nSize;
	nSize = (int)m_Items.size();
	for( i = 0; i < nSize; i++) {
		if(strcmp(str.GetBuffer(), m_Items[i].szDesc) == 0 ) {
			return &m_Items[i];
		}
	}
	return NULL;
}

void CGlobalValue::Process()
{
	int i, nSize;
	nSize = (int)m_Items.size();
	assert( m_Items.size() == m_gridView->GetSignalList().size() );
	for( i = 0; i < nSize; i++ ) {
		
		std::vector<CSequenceView::CSequenceSignalBase *> &gridItemList = m_gridView->GetSignalList();
		int frame = m_gridView->GetSlider()->GetPos() - gridItemList[i]->nStartFrame;
		float fRatio = (float)(m_gridView->GetSlider()->GetPos() - gridItemList[i]->nStartFrame) / (gridItemList[i]->nEndFrame-gridItemList[i]->nStartFrame);
		bool bShow = ( fRatio >= 0.f && fRatio < 1.f) && m_fxView->IsVisibleItem( m_Items[i].szDesc );

		if( m_Items[i].Type == MESH && m_Items[i].nObjectIndex != -1 ) {
			EtObjectHandle hObject;
			hObject = CEtObject::GetSmartPtr( m_Items[i].nObjectIndex );
			EtMatrix MatWorld;
			EtColor Color(1, 1, 1, 1);
			EtMatrixIdentity(&MatWorld);

			CalcMeshWorldMat(&MatWorld, (float)frame, fRatio, m_Items[i].nViewAxis, m_Items[i].nFixedAxis, m_Items[i].FxData, m_Items[i].ScaleData, m_Items[i].posType );
			CalcColor(&Color, (float)fRatio, m_Items[i].FxData);

			Color.a *= 0.999f;
			hObject->SetObjectAlpha( Color.a );
			hObject->SetCustomParam( m_Items[i].AmbientParamIndex, &Color );
			Color.a = 1.f;
			hObject->SetCustomParam( m_Items[i].DiffuseParamIndex, &Color );
			float fTime = frame / 60.f;
			if( m_Items[i].FXTimeIndex != -1 ) {
				hObject->SetCustomParam( m_Items[i].FXTimeIndex, &fTime );
			}
			hObject->GetSkinInstance()->SetBlendOP( (EtBlendOP)m_Items[i].blendOP );
			hObject->GetSkinInstance()->SetSrcBlend( (EtBlendMode)m_Items[i].srcBlend  );
			hObject->GetSkinInstance()->SetDestBlend( (EtBlendMode)m_Items[i].destBlend );
			hObject->GetSkinInstance()->SetCullMode( m_Items[i].cullMode ? true : false );
			hObject->GetSkinInstance()->SetUVTiling( m_Items[i].tilingMode ? true : false );
			hObject->GetSkinInstance()->SetSkipBakeDepth( m_Items[i].zWriteMode ? false : true );
			hObject->Update( &MatWorld );
			hObject->ShowObject( bShow );
		}
		else if( m_Items[i].Type == PARTICLE ) {
			if( m_Items[i].nObjectIndex != -1 ) {
				EtBillboardEffectHandle hParticle = CEtBillboardEffect::GetSmartPtr( m_Items[i].nObjectIndex );
				if( hParticle ) {
					hParticle->Release();
					m_Items[i].nObjectIndex = -1;
				}
			}

			EtMatrix MatWorld;
			EtColor Color(1, 1, 1, 1);
			EtMatrixIdentity(&MatWorld);

			CalcPtcWorldMat(&MatWorld, (float)frame, fRatio, m_Items[i].FxData, m_Items[i].posType );
			CalcColor(&Color, (float)fRatio, m_Items[i].FxData);
			EtBillboardEffectHandle hParticle = EternityEngine::CreateBillboardEffect( m_Items[i].nRenderIndex, &MatWorld, false );
			hParticle->EnableLoop( m_Items[i].bLoop!= FALSE );
			hParticle->EnableTracePos( m_Items[i].bIterator!=FALSE );
			m_Items[i].nObjectIndex = hParticle->GetMyIndex();
			hParticle->Show( bShow );

			hParticle->SetWorldMat( &MatWorld );
			hParticle->SetColor( &Color );
			hParticle->SetParticleTick( frame );
			m_currentParticle = hParticle;
		}
	}
}

void CGlobalValue::Refresh()
{
	int i, nSize;
	nSize = (int)m_Items.size();
	for( i = 0; i < nSize; i++) {
		if( m_Items[i].Type == PARTICLE && m_Items[i].nObjectIndex != -1 ) {
			EtBillboardEffectHandle hParticle = CEtBillboardEffect::GetSmartPtr( m_Items[i].nObjectIndex );
			if( hParticle ) {
				hParticle->Release();
				m_Items[i].nObjectIndex = -1;
			}
		}
	}
}

void CGlobalValue::Save(char *szFileName)
{
	DWORD dwAttr = GetFileAttributes( szFileName );
	if( dwAttr != 0xffffffff && (dwAttr & FILE_ATTRIBUTE_READONLY) ) {
		CString szStr;
		szStr.Format( "다음 파일들이 읽기전용 속성입니다.\n\n%s", szFileName);
		MessageBox( ((CMainFrame*)AfxGetMainWnd())->m_hWnd,  szStr, "경고", MB_OK );
		return;
	}

	CFileStream Stream( szFileName, CFileStream::OPEN_WRITE );

	SEffectFileHeader EffectHeader;
	char cReserved[ 1024 ];

	memset( cReserved, 0, 1024 );
	memset( &EffectHeader, 0, sizeof( SEffectFileHeader ) );
	strcpy( EffectHeader.szHeaderString, EFFECT_FILE_STRING );
	EffectHeader.nVersion = EFFECT_FILE_VERSION;

	Stream.Write( &EffectHeader, sizeof( SEffectFileHeader ) );
	memset( cReserved, 0, 1024 );
	Stream.Write( cReserved, EFFECT_HEADER_RESERVED );
	//Stream.Write( cReserved, EFFECT_TOOL_INFO );

	DWORD dwCount = (int)m_Items.size();
	std::set< std::pair<ELEMENT_TYPE, std::string> > itemSets;
	for( DWORD i = 0; i < dwCount; i++) {
		itemSets.insert( std::make_pair(m_Items[i].Type, m_Items[i].szName ) );
	}
	dwCount = (int)itemSets.size();
	Stream.Write( &dwCount, sizeof(DWORD));
	for( std::set< std::pair<ELEMENT_TYPE, std::string> >::iterator it = itemSets.begin(); it != itemSets.end(); ++it) {
		Stream.Write(&it->first, sizeof(ELEMENT_TYPE));
		char szFile[64]={0,};
		strcpy(szFile, it->second.c_str());
		Stream.Write( szFile, 64);
		//Stream.Write( cReserved, 12);
	}
	//Stream.Write( cReserved, 32 );

	DWORD dwTotalFrame = m_gridView->GetSlider()->GetRangeMax();
	Stream.Write( &dwTotalFrame, sizeof(DWORD));

	dwCount = (int)m_Items.size();
	Stream.Write( &dwCount, sizeof(DWORD));

	for( DWORD i = 0; i < dwCount; i++) {
		Stream.Write( &m_Items[i].Type, sizeof(ELEMENT_TYPE));
		Stream.Write( &m_Items[i].szDesc, 64);
		Stream.Write( &m_Items[i].szName, 64);
		Stream.Write( &m_Items[i].dwStartFrame, sizeof(DWORD));
		Stream.Write( &m_Items[i].dwEndFrame, sizeof(DWORD));
		Stream.Write( &m_Items[i].dwYOrder, sizeof(DWORD));
		Stream.Write( &m_Items[i].posType, sizeof(int));

		for( int j=0; j<5; j++ ) {		
			m_Items[i].FxData[j]->Save(&Stream);
		}

		if( m_Items[i].Type == PARTICLE ) {
			Stream.Write( &m_Items[i].bIterator, sizeof(BOOL));
			Stream.Write( &m_Items[i].bLoop, sizeof(BOOL));
			Stream.Write( &m_Items[i].bStopFlag, sizeof(BOOL));
		}
		else if( m_Items[i].Type == MESH ) {
			Stream.Write( &m_Items[i].nViewAxis, sizeof(int));
			Stream.Write( &m_Items[i].nFixedAxis, sizeof(int));
			Stream.Write( &m_Items[i].blendOP, sizeof(int));
			Stream.Write( &m_Items[i].srcBlend, sizeof(int));
			Stream.Write( &m_Items[i].destBlend, sizeof(int));
			Stream.Write( &m_Items[i].cullMode, sizeof(int));
			Stream.Write( &m_Items[i].tilingMode, sizeof(int));
			Stream.Write( &m_Items[i].zWriteMode, sizeof(int));
			for( int j=0; j<3; j++ ) {
				m_Items[i].ScaleData[j]->Save(&Stream);
			}
		}
	}
}

void CGlobalValue::Load(char *szFileName)
{
	DWORD dwAttr = GetFileAttributes( szFileName );
	if( dwAttr & FILE_ATTRIBUTE_READONLY ) {
		CString szStr;
		szStr.Format( "다음 파일들이 읽기전용 속성입니다.\n\n%s",szFileName );
		MessageBox( ((CMainFrame*)AfxGetMainWnd())->m_hWnd, szStr, "경고", MB_OK );
	}
	m_gridView->Reset();
	char szPath[ _MAX_PATH ];
	_GetPath( szPath, _countof(szPath), szFileName );
	CEtResourceMng::GetInstance().AddResourcePath( szPath, true );

	CFileStream Stream( szFileName, CFileStream::OPEN_READ );
	if( !Stream.IsValid() ) return ;

	SEffectFileHeader EffectHeader;
	Stream.Read( &EffectHeader, sizeof( SEffectFileHeader ) );

	Stream.Seek( EFFECT_HEADER_RESERVED, SEEK_CUR);
	if( EffectHeader.nVersion < 11) {
		Stream.Seek( EFFECT_TOOL_INFO, SEEK_CUR);
	}
	DWORD dwCount;
	Stream.Read( &dwCount, sizeof(DWORD));
	m_particleView->Reset();
	for( DWORD i=0; i<dwCount; i++ ) {
		ELEMENT_TYPE Type;
		Stream.Read( &Type, sizeof(ELEMENT_TYPE));
		char szFileName[64], szDescription[64];
		if( EffectHeader.nVersion < 11) {
			Stream.Read( szFileName, 32 );
			Stream.Read( szDescription, 32 );
		}
		else {
			Stream.Read( szFileName, 64 );
		}
		if( Type == PARTICLE ) {
			m_particleView->AddParticleFile( szFileName );
		}
		else if( Type == MESH ) {
			m_particleView->AddMeshFile( szFileName );
		}

		if( EffectHeader.nVersion < 11) {
			char cInitials;
			Stream.Read( &cInitials, sizeof(char) );
			Stream.Seek( 3, SEEK_CUR);
			COLORREF TextColor, BackColor;
			Stream.Read( &TextColor, sizeof(COLORREF) );
			Stream.Read( &BackColor, sizeof(COLORREF) );
		}
	}

	if( EffectHeader.nVersion < 11) {
		char szDescription[ 32 ];
		Stream.Read( szDescription, 32 );
	}

	DWORD dwTotalFrame;
	Stream.Read( &dwTotalFrame, sizeof(DWORD));
	/////////////////////////////////////////////////////
	m_gridView->SetSliderRange(0, dwTotalFrame);
	m_gridView->Refresh();
	/////////////////////////////////////////////////////
	Stream.Read( &dwCount, sizeof(DWORD));
	for( DWORD i=0; i<dwCount; i++ ) {

		ELEMENT_TYPE Type;
		Stream.Read( &Type, sizeof(ELEMENT_TYPE) );

		char szDescription[64] = {0,};
		char szParticleDescription[64] = {0,};
		if( EffectHeader.nVersion < 11) {
			Stream.Read( szDescription, 32 );
			Stream.Read( szParticleDescription, 32 );
		}
		else {
			Stream.Read( szDescription, 64 );
			Stream.Read( szParticleDescription, 64 );
		}

		DWORD dwStartFrame=0, dwEndFrame=0, dwYOrder=0, nPositionInterpolationType=0;
		Stream.Read( &dwStartFrame, sizeof(DWORD) );
		Stream.Read( &dwEndFrame, sizeof(DWORD) );
		Stream.Read( &dwYOrder, sizeof(DWORD) );
		Stream.Read( &nPositionInterpolationType, sizeof(int));

		Item item;
		item.Type = Type;		
		strcpy(item.szName, szParticleDescription);
		strcpy(item.szDesc, szDescription);
		item.nRenderIndex = -1;
		item.nObjectIndex = -1;
		item.dwStartFrame = dwStartFrame;
		item.dwEndFrame = dwEndFrame;
		item.dwYOrder = dwYOrder;
		item.posType = nPositionInterpolationType;
		if( Type == PARTICLE ) {
			item.nRenderIndex = EternityEngine::LoadParticleData( szParticleDescription );
		}
		else if( Type == MESH ) {
			EtSkinHandle hSkin;
			EtObjectHandle hObject = EternityEngine::CreateStaticObject( szParticleDescription );
			if(!hObject) {
				AfxMessageBox("Mesh 파일이 없습니다.");
			}
			else {
				item.nObjectIndex = hObject->GetMyIndex();
				item.AmbientParamIndex = hObject->AddCustomParam( "g_MaterialAmbient" );
				item.DiffuseParamIndex = hObject->AddCustomParam( "g_MaterialDiffuse");
				item.FXTimeIndex = hObject->AddCustomParam("g_fFXElapsedTime");
			}
		}
		m_gridView->AddSignal(dwStartFrame, dwEndFrame, dwYOrder, szDescription, szParticleDescription);
		m_fxView->AddItem(szDescription);

		const int TABLE_TYPE_AMOUNT = 5;
		for( int j=0; j<TABLE_TYPE_AMOUNT; j++ ) {
			item.FxData[j] = new CEtEffectDataContainer;
			item.FxData[j]->Load(&Stream);
		}

		if( Type == PARTICLE ) {
			Stream.Read( &item.bIterator, sizeof(BOOL));
			Stream.Read( &item.bLoop, sizeof(BOOL));
			Stream.Read( &item.bStopFlag, sizeof(BOOL));
		}
		else if( Type == MESH ) {
			float fLoopCountRatio=0.f;
			int nViewAxis=0;
			int nFixedAxis=0;
			int nBlendOP=0;
			int nSourceBlend=0;
			int nDestBlend=0;
			BOOL bCheckZBuffer=FALSE;

			if( EffectHeader.nVersion < 11) {
				Stream.Read( &fLoopCountRatio, sizeof(float) );
			}
			Stream.Read( &item.nViewAxis, sizeof(int));
			Stream.Read( &item.nFixedAxis, sizeof(int));
			Stream.Read( &item.blendOP, sizeof(int));
			Stream.Read( &item.srcBlend, sizeof(int));
			Stream.Read( &item.destBlend, sizeof(int));
			if( EffectHeader.nVersion >= 13) {
				Stream.Read( &item.cullMode, sizeof(int));
			}
			else {
				item.cullMode = 0;
			}
			if( EffectHeader.nVersion >= 14) {
				Stream.Read( &item.tilingMode, sizeof(int));
			}
			else {
				item.tilingMode = 0;
			}
			if( EffectHeader.nVersion != 11 ) {
				Stream.Read( &item.zWriteMode, sizeof(int));
				for( int j=0; j<3; j++ ) {
					item.ScaleData[j] = new CEtEffectDataContainer;
					item.ScaleData[j]->Load(&Stream);
				}
				if( EffectHeader.nVersion < 11 ) {
					const int FX_MAX_CUSTOM_PARAMETER_COUNT = 4;
					for( int i=0; i< FX_MAX_CUSTOM_PARAMETER_COUNT; i++ ) 
					{
						DWORD m_CustomParamStructnParamIndex;
						Stream.Read( &m_CustomParamStructnParamIndex, sizeof(int));
						assert( m_CustomParamStructnParamIndex == -1);				
					}
				}
			}
			else {
				for( int j=0; j<3; j++ ) {		
					item.ScaleData[j] = new CEtEffectDataContainer;
					item.ScaleData[j]->AddData( new CEtEffectScaleDataValue( 0.f, 1.f ) );
					item.ScaleData[j]->AddData( new CEtEffectScaleDataValue( 1.f, 1.f ) );
				}
			}
		}
		m_Items.push_back( item );
	}
	int nTell = Stream.Tell();
	int nSize = Stream.Size();
	m_gridView->Refresh();
	GetPropertyPaneView()->Reset();
}

void CGlobalValue::AddItemOnGridView( char *pName, char *pDesc, int nYOrder)
{
	char szStr[1024] = { 0, };
	GetCurrentDirectory( 1024, szStr );
	strcat_s( szStr, "\\" );

	CEtResourceMng::GetInstance().AddResourcePath( szStr, true );
	Item item;
	item.posType = 0;
	item.nRenderIndex = -1;
	item.nObjectIndex = -1;
	item.dwYOrder = nYOrder;
	if(strstr(pName, ".ptc")) {
		item.Type = PARTICLE;
		item.nRenderIndex = EternityEngine::LoadParticleData( pName );
	}
	else if(strstr(pName, ".skn")) {
		item.Type = MESH;
		EtSkinHandle hSkin;
		EtObjectHandle hObject = EternityEngine::CreateStaticObject( pName );
		if( hObject ) {
			item.nObjectIndex = hObject->GetMyIndex();
			item.AmbientParamIndex = hObject->AddCustomParam( "g_MaterialAmbient");
			item.DiffuseParamIndex = hObject->AddCustomParam( "g_MaterialDiffuse");
			item.FXTimeIndex = hObject->AddCustomParam( "g_fFXElapsedTime");
		}
		else {
			AfxMessageBox("Mesh 파일이 없습니다.");
			item.nObjectIndex = -1;
			item.AmbientParamIndex = -1;
			item.DiffuseParamIndex = -1;
			item.FXTimeIndex = -1;
		}
	}

	strcpy(item.szName, pName);	
	strcpy(item.szDesc, pDesc);
	for( int i = 0; i < 5; i++) {
		item.FxData[i] = new CEtEffectDataContainer;		
	}
	for( int i = 0; i < 3; i++) {
		item.ScaleData[i] = new CEtEffectDataContainer;
		item.ScaleData[i]->AddData( new CEtEffectScaleDataValue( 0.f, 1.f ) );
		item.ScaleData[i]->AddData( new CEtEffectScaleDataValue( 1.f, 1.f ) );
	}
	InitDefaultTable( item.FxData );	

	if( !m_Items.empty() ) {
		item.bIterator = m_Items.back().bIterator;
		item.bLoop = m_Items.back().bLoop;
		item.bStopFlag = m_Items.back().bStopFlag;
		item.nViewAxis = m_Items.back().nViewAxis;
		item.nFixedAxis = m_Items.back().nFixedAxis;
		item.blendOP = m_Items.back().blendOP;
		item.srcBlend = m_Items.back().srcBlend;
		item.destBlend = m_Items.back().destBlend;
		item.cullMode = m_Items.back().cullMode;
		item.tilingMode = m_Items.back().tilingMode;
		item.zWriteMode = m_Items.back().zWriteMode;
		for( int i = 0; i < 3; i++) {
			*item.FxData[i] = *m_Items.back().FxData[i];
		}
		for( int i = 0; i < 3; i++) {
			if( m_Items.back().ScaleData[i] && item.ScaleData[i] ) {
				*item.ScaleData[i] = *m_Items.back().ScaleData[i];
			}
		}
	}


	m_Items.push_back( item );
}

void CGlobalValue::RemoveItemOnGridView( char *pDesc)
{
	int i, nSize;
	nSize = (int)m_Items.size();
	for( i = 0; i < nSize; i++) {
		if(strcmp(m_Items[i].szDesc, pDesc)==0) {
			if( m_Items[i].nRenderIndex != -1 ) {
				EtBillboardEffectHandle hParticle = CEtBillboardEffect::GetSmartPtr( m_Items[i].nObjectIndex );
				SAFE_RELEASE_SPTR( hParticle );
				EternityEngine::DeleteParticleData( m_Items[i].nRenderIndex );
			}
			if( m_Items[i].nObjectIndex != -1 && m_Items[i].Type == MESH ) {
				EtObjectHandle hObject = CEtObject::GetSmartPtr( m_Items[i].nObjectIndex );
				if( hObject ) {
					hObject->Release();
				}
			}
			m_Items.erase( m_Items.begin() + i );
			m_fxView->RemoveItem( pDesc);
			break;
		}
	}
}

void CGlobalValue::InitAxisRender()
{
	if( m_pAxisRenderObject == NULL ) {
		m_pAxisRenderObject = new CAxisRenderObject;
		m_pAxisRenderObject->Initialize();
		m_pAxisRenderObject->Enable( false );
		D3DXMATRIX worldMat;
		D3DXMatrixIdentity( &worldMat );
		/*
		worldMat._41 = 5.f;
		worldMat._42 = 5.f;
		worldMat._43 = 5.f;*/
		m_pAxisRenderObject->Update( &worldMat );
	}
}

void CGlobalValue::ParseFileList( CString &szFileBuffer, char *szImportExt, std::vector<CString> &szVecList )
{
	char szBuffer[1024], szExt[256];
	CString szFolder;
	std::vector<CString> szVecAniList;


	int nBeginPos = 0;
	int nEndPos = 0;
	char cPrevChar, cCurChar;
	bool bFirstFlag = false;
	bool bSecondFlag = false;

	for( DWORD i=1;; i++ ) {
		nEndPos = i;
		cCurChar = szFileBuffer.GetBuffer()[i];
		cPrevChar = szFileBuffer.GetBuffer()[i-1];
		if( cCurChar == 0 && cPrevChar != 0 ) {
			strncpy_s( szBuffer, szFileBuffer.GetBuffer() + nBeginPos, nEndPos - nBeginPos );
			nBeginPos = i+1;

			if( bFirstFlag == false ) {	// 첫번째건 폴더
				bFirstFlag = true;
				szFolder = szBuffer;
			}
			else { // 나머진 파일리스트
				bSecondFlag = true;
				_GetExt( szExt, _countof(szExt), szBuffer );
				if( _stricmp( szExt, szImportExt ) == NULL ) {
					szVecList.push_back( szFolder + "\\" + szBuffer );
				}
			}
		}
		else if( cCurChar == 0 && cPrevChar == 0 ) break;
	}

	if( bSecondFlag == false ) {
		_GetExt( szExt, _countof(szExt), szFolder );
		if( _stricmp( szExt, szImportExt ) == NULL ) {
			szVecList.push_back( szFolder );
		}
	}
}


void CGlobalValue::SetFXItemLayer( char *pDesc, bool bUp )
{
	int i, nSize;
	nSize = (int)m_Items.size();
	for( i = 0; i < nSize; i++) {
		if(strcmp(m_Items[i].szDesc, pDesc)==0) {
			if(bUp) {
				if(i != 0) {
					if( m_Items[i-1].nObjectIndex != -1 && m_Items[i-1].Type == MESH && m_Items[i].nObjectIndex != -1 && m_Items[i].Type == MESH )
						CEtObject::SwapItemIndex( m_Items[i-1].nObjectIndex, m_Items[i].nObjectIndex );
					else if( m_Items[i-1].nObjectIndex != -1 && m_Items[i-1].Type == PARTICLE && m_Items[i].nObjectIndex != -1 && m_Items[i].Type == PARTICLE )
						CEtBillboardEffect::SwapItemIndex( m_Items[i-1].nObjectIndex, m_Items[i].nObjectIndex );

					Item TempItem;
					TempItem = m_Items[i-1];
					m_Items[i-1] = m_Items[i];
					m_Items[i] = TempItem;

					CSequenceView::CSequenceSignalBase *pTempItem;
					std::vector<CSequenceView::CSequenceSignalBase *> &gridItemList = m_gridView->GetSignalList();
					pTempItem = gridItemList[i-1];
					gridItemList[i-1] = gridItemList[i];
					gridItemList[i] = pTempItem;

					m_fxView->SetLayer( pDesc, true );
				}
			}
			else {
				if( i != nSize-1 ) {
					if( m_Items[i+1].nObjectIndex != -1 && m_Items[i+1].Type == MESH && m_Items[i].nObjectIndex != -1 && m_Items[i].Type == MESH )
						CEtObject::SwapItemIndex( m_Items[i+1].nObjectIndex, m_Items[i].nObjectIndex );
					else if( m_Items[i+1].nObjectIndex != -1 && m_Items[i+1].Type == PARTICLE && m_Items[i].nObjectIndex != -1 && m_Items[i].Type == PARTICLE )
						CEtBillboardEffect::SwapItemIndex( m_Items[i+1].nObjectIndex, m_Items[i].nObjectIndex );

					Item TempItem;
					TempItem = m_Items[i+1];
					m_Items[i+1] = m_Items[i];
					m_Items[i] = TempItem;

					CSequenceView::CSequenceSignalBase *pTempItem;
					std::vector<CSequenceView::CSequenceSignalBase *> &gridItemList = m_gridView->GetSignalList();
					pTempItem = gridItemList[i+1];
					gridItemList[i+1] = gridItemList[i];
					gridItemList[i] = pTempItem;

					m_fxView->SetLayer( pDesc, false );
				}
			}
			break;
		}
	}
}

void CGlobalValue::CopyFXItem( char *pSrcItemName, char *pDestItemName, int nType )
{
	int i, nSize;
	nSize = (int)m_Items.size();
	int nSrc = -1, nDest = -1;
	for( i = 0; i < nSize; i++) {
		if(strcmp(m_Items[i].szDesc, pSrcItemName)==0) {
			nSrc = i;
			break;
		}
	}
	if( nSrc == -1 ) return;
	for( i = 0; i < nSize; i++) {
		if(strcmp(m_Items[i].szDesc, pDestItemName)==0) {
			nDest = i;
			break;
		}
	}
	if( nDest == -1 ) return;
	if( nSrc == nDest ) return;

	if( nType == 0 || nType == 1 ) {
		m_Items[nDest].bIterator = m_Items[nSrc].bIterator;
		m_Items[nDest].bLoop = m_Items[nSrc].bLoop;
		m_Items[nDest].bStopFlag = m_Items[nSrc].bStopFlag;
		m_Items[nDest].nViewAxis = m_Items[nSrc].nViewAxis;
		m_Items[nDest].nFixedAxis = m_Items[nSrc].nFixedAxis;
		m_Items[nDest].blendOP = m_Items[nSrc].blendOP;
		m_Items[nDest].srcBlend = m_Items[nSrc].srcBlend;
		m_Items[nDest].destBlend = m_Items[nSrc].destBlend;
		m_Items[nDest].cullMode = m_Items[nSrc].cullMode;
		m_Items[nDest].tilingMode = m_Items[nSrc].tilingMode;
		m_Items[nDest].zWriteMode = m_Items[nSrc].zWriteMode;
	}
	else if( nType == 0 || nType == 2 ) {
		for( int i = 0; i < 3; i++) {
			*m_Items[nDest].FxData[i] = *m_Items[nSrc].FxData[i];
		}
		for( int i = 0; i < 3; i++) {
			if( m_Items[nDest].ScaleData[i] && m_Items[nSrc].ScaleData[i] ) {
				*m_Items[nDest].ScaleData[i] = *m_Items[nSrc].ScaleData[i];
			}
		}
	}
	else if( nType == 0 || nType == 3 ) {
		*m_Items[nDest].FxData[3] = *m_Items[nSrc].FxData[3];
	}
	else if( nType == 0 || nType == 4 ) {
		*m_Items[nDest].FxData[4] = *m_Items[nSrc].FxData[4];
	}
}