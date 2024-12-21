// SignalListViewDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EtActionTool.h"
#include "SignalListViewDlg.h"
#include "GlobalValue.h"
#include "ActionElement.h"
#include "ActionObject.h"



// SignalListViewDlg 대화 상자입니다.

#ifdef _EXPORT_EXCEL_ACTION_SIGNAL

IMPLEMENT_DYNAMIC(SignalListViewDlg, CDialog)

SignalListViewDlg::SignalListViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SignalListViewDlg::IDD, pParent)
	, m_EditCtrl_SignalCount(_T(""))
{

}

SignalListViewDlg::~SignalListViewDlg()
{
	int c = 10;
}

void SignalListViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_SignalListComboBox);
	DDX_Text(pDX, IDC_EDIT1, m_EditCtrl_SignalCount);
}


BEGIN_MESSAGE_MAP(SignalListViewDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &SignalListViewDlg::OnSignalList)
END_MESSAGE_MAP()


// SignalListViewDlg 메시지 처리기입니다.

void SignalListViewDlg::OnSignalList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nSelectedSel = m_SignalListComboBox.GetCurSel();
		
	if( nSelectedSel < 0 ) {
		::MessageBox(NULL, "시그널을 선택해주세요", "경고", MB_OK);
		return;
	}

	CActionObject* pActionObject =  NULL;
	CActionElement* pActionElement = NULL;

	pActionObject = dynamic_cast<CActionObject*> ( CGlobalValue::GetInstance().GetControlObject() );

	DWORD i, j, n, nCount;
	i = j = n = nCount = 0;
	char buf[_MAX_PATH] = {0,};
	char szSelecteditem[_MAX_PATH] = {0,};

	m_EditCtrl_SignalCount.Empty();

	m_SignalListComboBox.GetLBText( nSelectedSel, szSelecteditem ) ;
	m_strSignalTypeName = szSelecteditem;
	
	// 액션 폴더를 선택 했을 때 
	if( pActionObject ) {
		for( i = 0; i < pActionObject->GetChildCount(); i++ )
		{
			pActionElement= dynamic_cast<CActionElement*>( pActionObject->GetChild( i ) );

			if( ! pActionElement )
				continue;

			for( j = 0; j < pActionElement->GetChildCount(); j++ ) {
				if( m_strSignalTypeName.Compare( pActionElement->GetChild( j )->GetProperty( 0 )->GetCategory() ) == 0 )
					nCount++;
			}
		}
	}	
	// 액션 파일을 선택했을 때 
	else {
		pActionElement= dynamic_cast<CActionElement*>( CGlobalValue::GetInstance().GetControlObject() );

		if( pActionElement == NULL )
			return;
		
		for( i = 0; i < pActionElement->GetChildCount(); i++ ) {
			if( m_strSignalTypeName.Compare( pActionElement->GetChild( i )->GetProperty( 0 )->GetCategory() ) == 0 )
				nCount++;
		}
	}


	m_EditCtrl_SignalCount.Format("%d", nCount );

	UpdateData( FALSE );	
	
}


BOOL SignalListViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CActionObject* pActionObject =  NULL;
	CActionElement* pActionElement = NULL;
	CActionSignal*  pSignal = NULL;

	pActionObject = dynamic_cast<CActionObject*> ( CGlobalValue::GetInstance().GetControlObject() );

	DWORD i, j, n;
	i = j = n = 0;
	char	sComboboxlist[_MAX_PATH] = {0,};
	bool	bFlag = TRUE;

	if( pActionObject ) {
		for( i = 0; i < pActionObject->GetChildCount(); i++ )
		{
			pActionElement= dynamic_cast<CActionElement*>( pActionObject->GetChild( i ) );
			
			if( ! pActionElement )
				continue;

			for( j = 0; j < pActionElement->GetChildCount(); j ++ ) {	
				
				pSignal = dynamic_cast<CActionSignal*>( pActionElement->GetChild( j ) );
		
				if( ! pSignal ) continue;
			
				for( n = 0; n < m_SignalListComboBox.GetCount(); n++ ) {
					m_SignalListComboBox.GetLBText( n, sComboboxlist );
					if( strcmp( pSignal->GetProperty( 0 )->GetCategory(),  sComboboxlist ) == 0) {
						bFlag = FALSE;
						break;
					}
				}
				if( bFlag ) { 
					m_SignalListComboBox.AddString(  pSignal->GetProperty( 0 )->GetCategory() );
				}
				bFlag = TRUE;
			}
		}
	}	
	else {
		pActionElement= dynamic_cast<CActionElement*>( CGlobalValue::GetInstance().GetControlObject() );	

		if( pActionElement == NULL )
			return FALSE;

		for( j = 0; j < pActionElement->GetChildCount(); j ++ ) {	

			pSignal = dynamic_cast<CActionSignal*>( pActionElement->GetChild( j ) );

			if( ! pSignal ) continue;

			for( n = 0; n < m_SignalListComboBox.GetCount(); n++ ) {
				m_SignalListComboBox.GetLBText( n, sComboboxlist );
				if( strcmp( pSignal->GetProperty( 0 )->GetCategory(),  sComboboxlist ) == 0 ) {
					bFlag = FALSE;
					break;
				}
			}
			if( bFlag ) { 
				m_SignalListComboBox.AddString(  pSignal->GetProperty( 0 )->GetCategory()  );
			}
			bFlag = TRUE;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

#endif