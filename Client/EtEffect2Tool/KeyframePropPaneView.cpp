
#include "stdafx.h"
#include "EtEffect2Tool.h"
#include "EtEffectDataContainer.h"
#include "KeyframePropPaneView.h"
#include "ColorPickerDlg.h"
#include "GlobalValue.h"


// CKeyframePropPaneView
IMPLEMENT_DYNCREATE(CKeyframePropPaneView, CFormView)

CKeyframePropPaneView::CKeyframePropPaneView()
	: CFormView(CKeyframePropPaneView::IDD)
	, m_szScale(_T("30"))
{
	m_colorGrabIndex = -1;
	m_alphaGrabIndex = -1;
	m_scaleGrabIndex = -1;
	m_pEmitter = NULL;
	m_bActivate = FALSE;
}

CKeyframePropPaneView::~CKeyframePropPaneView()
{
}

void CKeyframePropPaneView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szScale);
	DDX_Control(pDX, IDC_EDIT1, m_EditCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
}

BEGIN_MESSAGE_MAP(CKeyframePropPaneView, CFormView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDBLCLK()
	ON_EN_CHANGE(IDC_EDIT1, &CKeyframePropPaneView::OnEnChangeEdit1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CKeyframePropPaneView::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CKeyframePropPaneView diagnostics

#ifdef _DEBUG
void CKeyframePropPaneView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CKeyframePropPaneView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


const static int startColorX = 40;
const static int startColorY = 40;
const static int widthColor = 200;
const static int heightColor = 30;

const static int startAlphaX = 30;
const static int startAlphaY = 110;
const static int widthAlpha = 240;
const static int heightAlpha = 60;

const static int startScaleX = 30;
const static int startScaleY = 200;
const static int widthScale = 240;
const static int heightScale = 60;

const static float COLLIDE_THRESHOLD = 0.03f;
const static float COLLIDE_THRESHOLD_Y = 0.1f;

COLORREF CKeyframePropPaneView::GetTableColor( float t) 
{
	int i, nSize;
	nSize = (int)m_colorAnchors.size();

	COLORREF result = RGB(0,0,0);

	for( i = 0;i < nSize-1; i++) {
		if( m_colorAnchors[i].fPos <= t && t < m_colorAnchors[i+1].fPos ) {
			float ratio = (t - m_colorAnchors[i].fPos ) / (m_colorAnchors[i+1].fPos - m_colorAnchors[i].fPos);
			int red = (int)(GetRValue(m_colorAnchors[i].color) * (1-ratio) + GetRValue(m_colorAnchors[i+1].color) * ratio);
			int green = (int)(GetGValue(m_colorAnchors[i].color) * (1-ratio) + GetGValue(m_colorAnchors[i+1].color) * ratio);
			int blue = (int)(GetBValue(m_colorAnchors[i].color) * (1-ratio) + GetBValue(m_colorAnchors[i+1].color) * ratio);
			result = RGB(red, green, blue);
			break;
		}
	}
	return result;
}

void CKeyframePropPaneView::DrawColorTable( CPaintDC &dc)
{
	// Draw Color Table
	for( int x = 0; x < widthColor; x++) {
		COLORREF color = GetTableColor( float(x)/widthColor);
		CPen Pen( PS_SOLID, 1, color );
		dc.SelectObject( &Pen );
		CPoint point[2];
		point[0] = CPoint( startColorX + x, startColorY + 0 );
		point[1] = CPoint( startColorX + x, startColorY + heightColor );
		dc.Polyline( point, 2 );
		Pen.DeleteObject();
	}
	CPen Pen( PS_SOLID, 1, RGB(0,0,0) );
	dc.SelectObject( &Pen );
	CPoint point[2];
	point[0] = CPoint( startColorX+widthColor-1, startColorY+heightColor );
	point[1] = CPoint( startColorX-1, startColorY+heightColor );
	dc.Polyline( point, 2 );
	Pen.DeleteObject();

	dc.SetBkMode( TRANSPARENT );
	dc.SelectStockObject( DEFAULT_GUI_FONT );
	TextOut(dc, startColorX + 10, startColorY - 20, "Color",  (UINT)strlen("Color") );	
	TextOut(dc, startColorX + 10, startColorY + 50, "Alpha",  (UINT)strlen("Alpha") );	
	TextOut(dc, startColorX + 10, startColorY + 140, "Scale",  (UINT)strlen("Scale") );	

	for each( ColorAnchor e in m_colorAnchors ) {
		CPen PenAnchor( PS_SOLID, 1, RGB(0,0,0) );
		CBrush Brush( e.color );
		dc.SelectObject( &PenAnchor );
		dc.SelectObject( &Brush );
		CPoint pt[ 3 ];
		pt[ 0 ] = CPoint( (int)(startColorX + e.fPos * widthColor), startColorY+heightColor+1 );
		pt[ 1 ] = CPoint( (int)(startColorX + e.fPos * widthColor - 6), startColorY+heightColor+8 );
		pt[ 2 ] = CPoint( (int)(startColorX + e.fPos * widthColor + 6), startColorY+heightColor+8 );
		dc.Polygon( pt, 3);
		PenAnchor.DeleteObject();
		Brush.DeleteObject();
	}
}

void CKeyframePropPaneView::DrawAlphaTable(CPaintDC &dc)
{
	// Draw Alpha Table
	{
		CPen PenAnchor( PS_SOLID, 1, RGB(140,140,140) );
		CBrush Brush( RGB(140,140,140) );
		dc.SelectObject( &PenAnchor );
		dc.SelectObject( &Brush );
		dc.Rectangle( startAlphaX-3, startAlphaY-3, startAlphaX+widthAlpha+3, startAlphaY+heightAlpha+3);
		PenAnchor.DeleteObject();
		Brush.DeleteObject();
	}
	for( int y = 0; y < 11; y++) {
		CPen Pen( (y==0||y==5||y==10)? PS_SOLID:PS_DOT, 1, RGB(75,75,75) );
		dc.SelectObject( &Pen );
		CPoint point[2];
		point[0] = CPoint( startAlphaX + 1, startAlphaY+y*6 );
		point[1] = CPoint( startAlphaX + widthAlpha + 1 , startAlphaY+y*6 );
		dc.Polyline( point, 2 );
		Pen.DeleteObject();
	}
	for( int x = 0; x < 11; x++) {
		CPen Pen( (x==0||x==5||x==10)? PS_SOLID:PS_DOT, 1, RGB(75,75,75) );
		dc.SelectObject( &Pen );
		CPoint point[2];
		point[0] = CPoint( startAlphaX + x*widthAlpha/10, startAlphaY );
		point[1] = CPoint( startAlphaX + x*widthAlpha/10, startAlphaY+heightAlpha );
		dc.Polyline( point, 2 );
		Pen.DeleteObject();
	}

	int i, nSize;
	nSize = (int)m_alphaAnchors.size();
	for( i = 0; i < nSize-1; i++) {
		CPen Pen( PS_SOLID, 1, RGB(128,255,128) );
		dc.SelectObject( &Pen );
		CPoint point[2];
		point[0] = CPoint( (int)(startAlphaX + widthAlpha * m_alphaAnchors[ i ].fPos),
			(int)(startAlphaY+ heightAlpha - heightAlpha * m_alphaAnchors[ i ].fValue) );
		point[1] = CPoint( (int)(startAlphaX + widthAlpha * m_alphaAnchors[ i + 1].fPos),
			(int)(startAlphaY+ heightAlpha - heightAlpha * m_alphaAnchors[ i + 1].fValue) );
		dc.Polyline( point, 2 );
		Pen.DeleteObject();
	}
	for( i = 0; i < nSize; i++) {
		CPen PenAnchor( PS_SOLID, 1, RGB(0,0,0) );
		CBrush Brush( RGB(220,220,0) );
		dc.SelectObject( &PenAnchor );
		dc.SelectObject( &Brush );
		int x = (int)(startAlphaX + widthAlpha * m_alphaAnchors[ i ].fPos);
		int y = (int)(startAlphaY+ heightAlpha - heightAlpha * m_alphaAnchors[ i ].fValue);
		dc.Rectangle( x-3, y-3, x+3, y+3);
		PenAnchor.DeleteObject();
		Brush.DeleteObject();
	}
}

void CKeyframePropPaneView::DrawScaleTable( CPaintDC &dc)
{
	// Draw Alpha Table
	{
		CPen PenAnchor( PS_SOLID, 1, RGB(140,140,140) );
		CBrush Brush( RGB(140,140,140) );
		dc.SelectObject( &PenAnchor );
		dc.SelectObject( &Brush );
		dc.Rectangle( startScaleX-3, startScaleY-3, startScaleX+widthScale+3, startScaleY+heightScale+3);
		PenAnchor.DeleteObject();
		Brush.DeleteObject();
	}
	for( int y = 0; y < 11; y++) {
		CPen Pen( (y==0||y==5||y==10)? PS_SOLID:PS_DOT, 1, RGB(75,75,75) );
		dc.SelectObject( &Pen );
		CPoint point[2];
		point[0] = CPoint( startScaleX + 1, startScaleY+y*6 );
		point[1] = CPoint( startScaleX + widthScale + 1 , startScaleY+y*6 );
		dc.Polyline( point, 2 );
		Pen.DeleteObject();
	}
	for( int x = 0; x < 11; x++) {
		CPen Pen( (x==0||x==5||x==10)? PS_SOLID:PS_DOT, 1, RGB(75,75,75) );
		dc.SelectObject( &Pen );
		CPoint point[2];
		point[0] = CPoint( startScaleX + x*widthScale/10, startScaleY );
		point[1] = CPoint( startScaleX + x*widthScale/10, startScaleY+heightScale );
		dc.Polyline( point, 2 );
		Pen.DeleteObject();
	}

	int i, nSize;
	nSize = (int)m_scaleAnchors.size();
	for( i = 0; i < nSize-1; i++) {		
		CPen Pen( PS_SOLID, 1, RGB(128,255,128) );
		dc.SelectObject( &Pen );
		CPoint point[2];
		point[0] = CPoint( (int)(startScaleX + widthScale * m_scaleAnchors[ i ].fPos), 
			(int)(startScaleY+ heightScale - heightScale * m_scaleAnchors[ i ].fValue) );
		point[1] = CPoint( (int)(startScaleX + widthScale * m_scaleAnchors[ i + 1].fPos), 
			(int)(startScaleY+ heightScale - heightScale * m_scaleAnchors[ i + 1].fValue) );
		dc.Polyline( point, 2 );
		Pen.DeleteObject();
	}

	for( i = 0; i < nSize; i++) {		
		CPen PenAnchor( PS_SOLID, 1, RGB(0,0,0) );
		CBrush Brush( RGB(220,220,0) );
		dc.SelectObject( &PenAnchor );
		dc.SelectObject( &Brush );
		int x = (int)(startScaleX + widthScale * m_scaleAnchors[ i ].fPos);
		int y = (int)(startScaleY+ heightScale - heightScale * m_scaleAnchors[ i ].fValue);
		dc.Rectangle( x-3, y-3, x+3, y+3);
		PenAnchor.DeleteObject();
		Brush.DeleteObject();
	}
}

// CKeyframePropPaneView message handlers
void CKeyframePropPaneView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CFormView::OnPaint() for painting messages
	if( m_bDraw ) {
		DrawColorTable(dc);
		DrawAlphaTable(dc);
		DrawScaleTable(dc);		
	}
}

void CKeyframePropPaneView::AddColorAnchor( float pos , COLORREF color, bool bFreeze)
{
	ColorAnchor anchor;
	anchor.fPos = pos;
	anchor.color = color;
	anchor.bFreeze = bFreeze;
	m_colorAnchors.push_back( anchor );
	std::sort(m_colorAnchors.begin(), m_colorAnchors.end() );
	UpdateSetting();
}

void CKeyframePropPaneView::AddAlphaAnchor( float pos , float value, bool bFreeze)
{
	AlphaAnchor anchor;
	anchor.fPos = pos;
	anchor.fValue = value;
	anchor.bFreeze = bFreeze;
	m_alphaAnchors.push_back( anchor );
	std::sort(m_alphaAnchors.begin(), m_alphaAnchors.end() );
	UpdateSetting();
}

void CKeyframePropPaneView::AddScaleAnchor( float pos , float value, bool bFreeze)
{
	ScaleAnchor anchor;
	anchor.fPos = pos;
	anchor.fValue = value;
	anchor.bFreeze = bFreeze;
	m_scaleAnchors.push_back( anchor );
	std::sort(m_scaleAnchors.begin(), m_scaleAnchors.end() );
	UpdateSetting();
}

bool CKeyframePropPaneView::IsInsideColorTable( CPoint point )
{
	const int threshold = 5;
	return (point.x > startColorX - threshold && point.x < startColorX + widthColor + threshold &&
		point.y > startColorY && point.y < startColorY + heightColor + 10);
}

bool CKeyframePropPaneView::IsInsideAlphaTable( CPoint point )
{
	const int threshold = 5;
	return (point.x > startAlphaX - threshold && point.x < startAlphaX + widthAlpha + threshold &&
		point.y > startColorY - threshold && point.y < startAlphaY + heightAlpha + threshold);
}

bool CKeyframePropPaneView::IsInsideScaleTable( CPoint point )
{
	const int threshold = 5;
	return (point.x > startScaleX - threshold && point.x < startScaleX + widthScale + threshold &&
		point.y > startScaleY - threshold && point.y < startScaleY + heightScale + threshold);
}

float CKeyframePropPaneView::ComputeColorPosition( CPoint point )
{
	float pos = (float)( point.x - startColorX ) / widthColor;
	return pos;
}

float CKeyframePropPaneView::ComputeAlphaPosition( CPoint point )
{
	float pos = (float)( point.x - startAlphaX ) / widthAlpha;
	return pos;
}

float CKeyframePropPaneView::ComputeScalePosition( CPoint point )
{
	float pos = (float)( point.x - startScaleX ) / widthScale;
	return pos;
}

float CKeyframePropPaneView::ComputeAlphaPositionY( CPoint point )
{
	float pos = 1.f - ((float)( point.y - startAlphaY ) / heightAlpha);
	return pos;
}

float CKeyframePropPaneView::ComputeScalePositionY( CPoint point )
{
	float pos = 1.f - ((float)( point.y - startScaleY ) / heightScale);
	return pos;
}

// 컨트롤에 세팅한 데이타를 실데이타쪽으로 전송
void CKeyframePropPaneView::UpdateSetting()
{
	CEtEffectDataContainer**ppTable = CGlobalValue::GetInstance().GetFXData( m_fxString );	
	if( ppTable ) {	// FX Setting
		CEtEffectDataContainer* pTable = NULL;
		int i, nSize;
		pTable = ppTable[ERT_COLOR_TABLE];
		pTable->ResetData();	
		nSize = (int)m_colorAnchors.size();
		for( i = 0; i < nSize; i++ ) {
			CEtEffectColorDataValue* pTableValue = new CEtEffectColorDataValue;
			int red = GetRValue(m_colorAnchors[i].color);
			int green = GetGValue(m_colorAnchors[i].color);
			int blue = GetBValue(m_colorAnchors[i].color);
			pTableValue->SetColor( EtVector3( red/255.f, green/255.f, blue/255.f) );
			pTableValue->SetKey( m_colorAnchors[i].fPos );
			pTable->AddData( pTableValue );
		}

		pTable = ppTable[ERT_ALPHA_TABLE];
		pTable->ResetData();
		nSize = (int)m_alphaAnchors.size();
		for( i = 0; i < nSize; i++ ) {
			CEtEffectAlphaDataValue* pTableValue = new CEtEffectAlphaDataValue;
			pTableValue->SetAlpha( m_alphaAnchors[i].fValue );
			pTableValue->SetKey( m_alphaAnchors[i].fPos );
			pTable->AddData( pTableValue );
		}

		int nCurSel = m_ComboBox.GetCurSel();
		CEtEffectDataContainer**ppScaleTable = CGlobalValue::GetInstance().GetScaleData( m_fxString );	
		switch( nCurSel ) {
			case 0:
				pTable = ppTable[ERT_SCALE_TABLE];
				break;
			case 1:
				pTable = ppScaleTable[ 0 ];
				break;
			case 2:
				pTable = ppScaleTable[ 1 ];
				break;
			case 3:
				pTable = ppScaleTable[ 2 ];
				break;
		}		
		pTable->ResetData();
		nSize = (int)m_scaleAnchors.size();
		UpdateData( TRUE );
		float fScaleVal = (float)atof( m_szScale.GetBuffer() );
		for( i = 0; i < nSize; i++ ) {
			CEtEffectScaleDataValue* pTableValue = new CEtEffectScaleDataValue;
			pTableValue->SetScale( m_scaleAnchors[i].fValue * fScaleVal );
			pTableValue->SetKey( m_scaleAnchors[i].fPos );
			pTable->AddData( pTableValue );	
		}
	}
	else if( m_pEmitter) {		// Particle Setting
		CEtEffectDataContainer* pTable = NULL;
		int i, nSize;
		pTable = m_pEmitter->GetTable( CEtBillboardEffectEmitter::COLOR_DATA_TYPE );
		pTable->ResetData();	
		nSize = (int)m_colorAnchors.size();
		for( i = 0; i < nSize; i++ ) {
			CEtEffectColorDataValue* pTableValue = new CEtEffectColorDataValue;
			int red = GetRValue(m_colorAnchors[i].color);
			int green = GetGValue(m_colorAnchors[i].color);
			int blue = GetBValue(m_colorAnchors[i].color);
			pTableValue->SetColor( EtVector3( red/255.f, green/255.f, blue/255.f) );
			pTableValue->SetKey( m_colorAnchors[i].fPos );
			pTable->AddData( pTableValue );
		}

		pTable = m_pEmitter->GetTable( CEtBillboardEffectEmitter::ALPHA_DATA_TYPE );
		pTable->ResetData();
		nSize = (int)m_alphaAnchors.size();
		for( i = 0; i < nSize; i++ ) {
			CEtEffectAlphaDataValue* pTableValue = new CEtEffectAlphaDataValue;
			pTableValue->SetAlpha( m_alphaAnchors[i].fValue );
			pTableValue->SetKey( m_alphaAnchors[i].fPos );
			pTable->AddData( pTableValue );
		}

		pTable = m_pEmitter->GetTable( CEtBillboardEffectEmitter::SCALE_DATA_TYPE );
		pTable->ResetData();
		nSize = (int)m_scaleAnchors.size();
		UpdateData( TRUE );
		float fScaleVal = (float)atof( m_szScale.GetBuffer() );
		for( i = 0; i < nSize; i++ ) {
			CEtEffectScaleDataValue* pTableValue = new CEtEffectScaleDataValue;
			pTableValue->SetScale( m_scaleAnchors[i].fValue * fScaleVal );
			pTableValue->SetKey( m_scaleAnchors[i].fPos );
			pTable->AddData( pTableValue );	
		}
	}

}

void CKeyframePropPaneView::OnOtherSelected()
{
	m_bDraw = false;
	m_EditCtrl.ShowWindow( SW_HIDE );
	m_ComboBox.ShowWindow( SW_HIDE );
	m_pEmitter = NULL;
}

// FX 데이타를 컨트롤 데이타로 전송 
void CKeyframePropPaneView::OnFXSelected(CString str)
{
	CEtEffectDataContainer**ppTable = CGlobalValue::GetInstance().GetFXData( str );
	if(!ppTable) return;
	m_fxString = str;

	m_bDraw = true;
	m_EditCtrl.ShowWindow( SW_SHOW );		
	m_ComboBox.ShowWindow( CGlobalValue::GetInstance().FindItem(str)->Type == MESH ? SW_SHOW : SW_HIDE );
	m_pEmitter = NULL;	
	
	int i, nSize;
	CEtEffectDataContainer* pTable = ppTable[ERT_COLOR_TABLE];
	if( !pTable ) return;
	nSize = pTable->GetDataCount();
	if( nSize == 0) return;
	m_colorAnchors.clear();
	for( i = 0; i < nSize; i++) {
		CEtEffectColorDataValue *pColorTable = (CEtEffectColorDataValue*)pTable->GetValueFromIndex(i);
		ColorAnchor ac;
		ac.fPos = pColorTable->GetKey();
		int red = (int)(255 * pColorTable->GetRed());
		int green = (int)(255 * pColorTable->GetGreen());
		int blue = (int)(255 * pColorTable->GetBlue());
		ac.color = RGB( red, green, blue );
		ac.bFreeze = (i == 0||i==nSize-1) ? true : false;
		m_colorAnchors.push_back( ac );
	}

	pTable = ppTable[ERT_ALPHA_TABLE];
	if( !pTable ) return;
	nSize = pTable->GetDataCount();
	if( nSize == 0) return;
	m_alphaAnchors.clear();
	for( i = 0; i < nSize; i++) {
		CEtEffectAlphaDataValue *pAlphaTable = (CEtEffectAlphaDataValue*)pTable->GetValueFromIndex(i);
		AlphaAnchor ac;
		ac.fPos = pAlphaTable->GetKey();
		ac.fValue = pAlphaTable->GetAlpha();
		ac.bFreeze = (i == 0||i==nSize-1) ? true : false;
		m_alphaAnchors.push_back( ac );
	}

	int nCurSel = m_ComboBox.GetCurSel();
	CEtEffectDataContainer**ppScaleTable = CGlobalValue::GetInstance().GetScaleData( m_fxString );	
	switch( nCurSel ) {
		case 0:
			pTable = ppTable[ERT_SCALE_TABLE];
			break;
		case 1:
			pTable = ppScaleTable[ 0 ];
			break;
		case 2:
			pTable = ppScaleTable[ 1 ];
			break;
		case 3:
			pTable = ppScaleTable[ 2 ];
			break;
	}
	if( !pTable ) return;
	nSize = pTable->GetDataCount();
	if( nSize == 0) return;
	m_scaleAnchors.clear();

	float fMaxScale = 0.f;
	for( i = 0; i < nSize; i++) {
		CEtEffectScaleDataValue *pScaleTable = (CEtEffectScaleDataValue*)pTable->GetValueFromIndex(i);
		fMaxScale = __max(fMaxScale, pScaleTable->GetScale());
	}
	assert( fMaxScale != 0.f );
	m_szScale.Format("%.2f", fMaxScale);
	UpdateData( FALSE );

	for( i = 0; i < nSize; i++) {
		CEtEffectScaleDataValue *pScaleTable = (CEtEffectScaleDataValue*)pTable->GetValueFromIndex(i);
		ScaleAnchor ac;
		ac.fPos = pScaleTable->GetKey();
		ac.fValue = pScaleTable->GetScale() / fMaxScale;
		ac.bFreeze = (i == 0||i==nSize-1) ? true : false;
		m_scaleAnchors.push_back( ac );
	}
	Invalidate();
}

// 파티클 데이타를 컨트롤 데이타로 전송 
void CKeyframePropPaneView::OnEmitterSelected(  CEtBillboardEffectEmitter *pEmitter )
{	
	m_fxString = "";
	m_bDraw = true;
	m_EditCtrl.ShowWindow( SW_SHOW );
	m_ComboBox.ShowWindow( SW_HIDE );
	m_pEmitter = pEmitter;	

	int i, nSize;
	CEtEffectDataContainer* pTable = pEmitter->GetTable( CEtBillboardEffectEmitter::COLOR_DATA_TYPE );
	if( !pTable ) return;
	nSize = pTable->GetDataCount();
	if( nSize == 0) return;
	m_colorAnchors.clear();
	for( i = 0; i < nSize; i++) {
		CEtEffectColorDataValue *pColorTable = (CEtEffectColorDataValue*)pTable->GetValueFromIndex(i);
		ColorAnchor ac;
		ac.fPos = pColorTable->GetKey();
		int red = (int)(255 * pColorTable->GetRed());
		int green = (int)(255 * pColorTable->GetGreen());
		int blue = (int)(255 * pColorTable->GetBlue());
		ac.color = RGB( red, green, blue );
		ac.bFreeze = (i == 0||i==nSize-1) ? true : false;
		m_colorAnchors.push_back( ac );
	}

	pTable = pEmitter->GetTable( CEtBillboardEffectEmitter::ALPHA_DATA_TYPE );
	if( !pTable ) return;
	nSize = pTable->GetDataCount();
	if( nSize == 0) return;
	m_alphaAnchors.clear();
	for( i = 0; i < nSize; i++) {
		CEtEffectAlphaDataValue *pAlphaTable = (CEtEffectAlphaDataValue*)pTable->GetValueFromIndex(i);
		AlphaAnchor ac;
		ac.fPos = pAlphaTable->GetKey();
		ac.fValue = pAlphaTable->GetAlpha();
		ac.bFreeze = (i == 0||i==nSize-1) ? true : false;
		m_alphaAnchors.push_back( ac );
	}

	pTable = pEmitter->GetTable( CEtBillboardEffectEmitter::SCALE_DATA_TYPE );
	if( !pTable ) return;
	nSize = pTable->GetDataCount();
	if( nSize == 0) return;
	m_scaleAnchors.clear();

	float fMaxScale = 0.f;
	for( i = 0; i < nSize; i++) {
		CEtEffectScaleDataValue *pScaleTable = (CEtEffectScaleDataValue*)pTable->GetValueFromIndex(i);
		fMaxScale = __max(fMaxScale, pScaleTable->GetScale());
	}
	assert( fMaxScale != 0.f );
	m_szScale.Format("%.2f", fMaxScale);
	UpdateData( FALSE );

	for( i = 0; i < nSize; i++) {
		CEtEffectScaleDataValue *pScaleTable = (CEtEffectScaleDataValue*)pTable->GetValueFromIndex(i);
		ScaleAnchor ac;
		ac.fPos = pScaleTable->GetKey();
		ac.fValue = pScaleTable->GetScale() / fMaxScale;
		ac.bFreeze = (i == 0||i==nSize-1) ? true : false;
		m_scaleAnchors.push_back( ac );
	}
	Invalidate();
}

void CKeyframePropPaneView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( IsInsideColorTable( point ) ) {
		float fKey = ComputeColorPosition( point );
		bool bSelect = false;
		int i, nSize;
		nSize = (int)m_colorAnchors.size();
		for( i = 0; i < nSize; i++) {	
			if( fabsf(m_colorAnchors[i].fPos - fKey) < COLLIDE_THRESHOLD ) {
				bSelect = true;
				break;
			}
		}
		if( bSelect ) {
			CColorPickerDlg dlg;
			dlg.SetColor( m_colorAnchors[i].color );
			if( dlg.DoModal() == IDOK) {
				COLORREF color = dlg.GetColor();
				m_colorAnchors[i].color = color;
				UpdateSetting();
				TRACE("color changed\n");
			}
		}
		else {
			CColorPickerDlg dlg;
			if( dlg.DoModal() == IDOK) {
				COLORREF color = dlg.GetColor();
				TRACE("RGB:%d %d %d\n", GetRValue(color), GetGValue(color),GetBValue(color));
				AddColorAnchor(fKey, color);
			}
		}
		Invalidate();
	}
	else if ( IsInsideAlphaTable(point) ) {
		float fKey = ComputeAlphaPosition( point );
		float fValue = ComputeAlphaPositionY( point );

		bool bSelect = false;
		int i, nSize;
		nSize = (int)m_alphaAnchors.size();
		for( i = 0; i < nSize; i++) {	
			if( fabsf(m_alphaAnchors[i].fPos - fKey) < COLLIDE_THRESHOLD &&
				fabsf(m_alphaAnchors[i].fValue - fValue) < COLLIDE_THRESHOLD_Y) {
				bSelect = true;
				break;
			}
		}
		if( !bSelect ) {
			AddAlphaAnchor(fKey, fValue);
		}
		Invalidate();
	}
	else if ( IsInsideScaleTable(point) ) {
		float fKey = ComputeScalePosition( point );
		float fValue = ComputeScalePositionY( point );
		bool bSelect = false;
		int i, nSize;
		nSize = (int)m_scaleAnchors.size();
		for( i = 0; i < nSize; i++) {	
			if( fabsf(m_scaleAnchors[i].fPos - fKey) < COLLIDE_THRESHOLD &&
				fabsf(m_scaleAnchors[i].fValue - fValue) < COLLIDE_THRESHOLD_Y) {
					bSelect = true;
					break;
			}
		}
		if( !bSelect ) {
			AddScaleAnchor(fKey, fValue);
		}
		Invalidate();
	}
	CFormView::OnLButtonDblClk(nFlags, point);
}

void CKeyframePropPaneView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default	
	if ( IsInsideAlphaTable( point ) ) {
		float fKey = ComputeAlphaPosition( point );
		float fValue = ComputeAlphaPositionY( point );
		int i, nSize;
		nSize = (int)m_alphaAnchors.size();
		for( i = 0; i < nSize; i++) {
			if( fabsf(m_alphaAnchors[i].fPos - fKey) < COLLIDE_THRESHOLD &&
				fabsf(m_alphaAnchors[i].fValue - fValue) < COLLIDE_THRESHOLD_Y ) {
					m_alphaGrabIndex = i;
					break;
			}
		}
	}
	if( IsInsideColorTable( point ) ) {
		int i, nSize;
		float pos = ComputeColorPosition( point );
		nSize = (int)m_colorAnchors.size();
		for( i = 0; i < nSize; i++) {
			if( fabsf(m_colorAnchors[i].fPos - pos) < COLLIDE_THRESHOLD && !m_colorAnchors[i].bFreeze ) {
				m_colorGrabIndex = i;
				break;
			}
		}
	}
	else if ( IsInsideScaleTable( point ) ) {
		float fKey = ComputeScalePosition( point );
		float fValue = ComputeScalePositionY( point );
		int i, nSize;
		nSize = (int)m_scaleAnchors.size();
		for( i = 0; i < nSize; i++) {
			if( fabsf(m_scaleAnchors[i].fPos - fKey) < COLLIDE_THRESHOLD &&// !m_scaleAnchors[i].bFreeze &&
				fabsf(m_scaleAnchors[i].fValue - fValue) < COLLIDE_THRESHOLD_Y ) {
					m_scaleGrabIndex = i;
					break;
			}
		}		
	}
	CFormView::OnLButtonDown(nFlags, point);
}

void CKeyframePropPaneView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_colorGrabIndex = -1;
	m_alphaGrabIndex = -1;
	m_scaleGrabIndex = -1;
	CFormView::OnLButtonUp(nFlags, point);
}

void CKeyframePropPaneView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	CGlobalValue::GetInstance().SetKeyframePropPaneView( this );
	if( m_bActivate == TRUE ) return;
	m_bActivate = TRUE;

	m_ComboBox.SetCurSel( 0 );

	if( m_colorAnchors.empty() ) {
		SetDefaultTable();
	}
	// TODO: Add your specialized code here and/or call the base class
}

void CKeyframePropPaneView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_colorGrabIndex != -1 ) {	
		float fMinPos = 0.f;
		float fMaxPos = 1.f;
		if( m_colorGrabIndex > 0 ) {
			fMinPos = m_colorAnchors[ m_colorGrabIndex - 1 ].fPos + COLLIDE_THRESHOLD / 2.f;
		}
		if( m_colorGrabIndex < (int)m_colorAnchors.size()-1 ) {
			fMaxPos = m_colorAnchors[ m_colorGrabIndex + 1 ].fPos - COLLIDE_THRESHOLD / 2.f; 
		}
		m_colorAnchors[m_colorGrabIndex].fPos = __min(fMaxPos, __max(fMinPos, ComputeColorPosition(point) ));
		UpdateSetting();
		Invalidate();
	}
	if( m_alphaGrabIndex != -1 ) {
		float fMinPos = 0.f;
		float fMaxPos = 1.f;
		if( m_alphaGrabIndex > 0 ) {
			fMinPos = m_alphaAnchors[ m_alphaGrabIndex - 1 ].fPos + COLLIDE_THRESHOLD / 2.f;
		}
		if( m_alphaGrabIndex < (int)m_alphaAnchors.size()-1 ) {
			fMaxPos = m_alphaAnchors[ m_alphaGrabIndex + 1 ].fPos - COLLIDE_THRESHOLD / 2.f; 
		}
		if( !m_alphaAnchors[m_alphaGrabIndex].bFreeze ) {
			m_alphaAnchors[m_alphaGrabIndex].fPos = __min(fMaxPos, __max(fMinPos, ComputeAlphaPosition(point) ));
		}
		m_alphaAnchors[m_alphaGrabIndex].fValue = __min(2.f, __max(0.f, ComputeAlphaPositionY(point)));
		UpdateSetting();
		Invalidate();
	}
	if( m_scaleGrabIndex != -1 ) {
		float fMinPos = 0.f;
		float fMaxPos = 1.f;
		if( m_scaleGrabIndex > 0 ) {
			fMinPos = m_scaleAnchors[ m_scaleGrabIndex - 1 ].fPos + COLLIDE_THRESHOLD / 2.f;
		}
		if( m_scaleGrabIndex < (int)m_scaleAnchors.size()-1 ) {
			fMaxPos = m_scaleAnchors[ m_scaleGrabIndex + 1 ].fPos - COLLIDE_THRESHOLD / 2.f; 
		}
		if( !m_scaleAnchors[m_scaleGrabIndex].bFreeze ) {
			m_scaleAnchors[m_scaleGrabIndex].fPos = __min(fMaxPos, __max(fMinPos, ComputeScalePosition(point) ));
		}
		m_scaleAnchors[m_scaleGrabIndex].fValue = __min(1.f, __max(0.f, ComputeScalePositionY(point)));
		UpdateSetting();
		Invalidate();
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CKeyframePropPaneView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( IsInsideColorTable( point ) ) {
		float fPos = ComputeColorPosition( point );
		for( std::vector< ColorAnchor >::iterator it = m_colorAnchors.begin(); it != m_colorAnchors.end(); ++it) {
			if( fabsf(it->fPos - fPos) < COLLIDE_THRESHOLD && !it->bFreeze ) {
				m_colorAnchors.erase( it );
				UpdateSetting();
				Invalidate();
				break;
			}
		}
	}
	else if( IsInsideAlphaTable( point ) ) {
		float fPos = ComputeAlphaPosition( point );
		float fValue = ComputeAlphaPositionY( point );
		for( std::vector< AlphaAnchor >::iterator it = m_alphaAnchors.begin(); it != m_alphaAnchors.end(); ++it) {
			if( fabsf(it->fPos - fPos) < COLLIDE_THRESHOLD && !it->bFreeze &&
				fabsf(it->fValue - fValue) < COLLIDE_THRESHOLD_Y ) {
				m_alphaAnchors.erase( it );
				UpdateSetting();
				Invalidate();
				break;
			}
		}
	}
	else if( IsInsideScaleTable( point ) ) {
		float fPos = ComputeScalePosition( point );
		float fValue = ComputeScalePositionY( point );
		for( std::vector< ScaleAnchor >::iterator it = m_scaleAnchors.begin(); it != m_scaleAnchors.end(); ++it) {
			if( fabsf(it->fPos - fPos) < COLLIDE_THRESHOLD && !it->bFreeze &&
				fabsf(it->fValue - fValue) < COLLIDE_THRESHOLD_Y ) {
				m_scaleAnchors.erase( it );
				UpdateSetting();
				Invalidate();
				break;
			}
		}
	}
	CFormView::OnRButtonDblClk(nFlags, point);
}

void CKeyframePropPaneView::OnEnChangeEdit1()
{
	UpdateSetting();
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CKeyframePropPaneView::SetDefaultTable()
{
	m_colorAnchors.clear();
	AddColorAnchor(0.f, RGB(255,255,255), true);
	AddColorAnchor(1.f, RGB(255,255,255), true);
	
	m_alphaAnchors.clear();
	AddAlphaAnchor(0.f, 1.f, true);
	AddAlphaAnchor(0.85f, 1.f, false);
	AddAlphaAnchor(1.f, 0.f, true);
	
	m_scaleAnchors.clear();
	AddScaleAnchor(0.f, 1.f, true);		
	AddScaleAnchor(1.f, 1.f, true);

	m_szScale.Format("30.0");
	UpdateData( FALSE );
	
	UpdateSetting();
}


void CKeyframePropPaneView::RefreshPointer()
{
	m_pEmitter = NULL;
}

void CKeyframePropPaneView::OnCbnSelchangeCombo1()
{
	OnFXSelected( m_fxString );
}
