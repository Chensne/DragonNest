#include "StdAfx.h"

#ifdef PRE_ADD_CASHSHOP_RENEWAL

#include <wininet.h>
#include "shlobj.h"

#include "DnMainFrame.h"

#include "DnTableDB.h"

#include "DnCashShopMainBannerDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCashShopMainBanner::CDnCashShopMainBanner( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
,m_pTextureCtr(NULL)
,m_time(0.0f)
,m_crrIdx(0)
{

	//TCHAR pBuffer[MAX_PATH]={0};
	//TCHAR pImagePath[MAX_PATH]={0};

	//SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), pBuffer, CSIDL_PERSONAL, 0);
	//wsprintf( pBuffer, L"%s\\DragonNest", pBuffer );

	////이미지 경로 설정
	//wsprintf( pImagePath, L"%s\\Event_Page.png", pBuffer );
	//WideCharToMultiByte( CP_ACP, 0, pImagePath, -1, BANNERIMAGE_FILE_NAME, sizeof(BANNERIMAGE_FILE_NAME), NULL, NULL );

	
}


CDnCashShopMainBanner::~CDnCashShopMainBanner(void)
{	
	int size = (int)m_vTexture.size();
	for( int i=0; i<size; ++i )
		m_vTexture[i]->Release();
	m_vTexture.clear();

}


void CDnCashShopMainBanner::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSMainBanner.ui" ).c_str(), bShow );
}


void CDnCashShopMainBanner::InitialUpdate()
{
	// 부모윈도우 위치에 맞게 이동.
	float x, y;
	GetParentDialog()->GetPosition( x, y );
	this->SetPosition( x, y );

	m_pTextureCtr = GetControl<CEtUITextureControl>( "ID_TEXTUREL_BANNER" );
	

	TCHAR pBuffer[MAX_PATH]={0};
	TCHAR pImagePath[MAX_PATH]={0};

	SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), pBuffer, CSIDL_PERSONAL, 0);
	wsprintf( pBuffer, L"%s\\DragonNest", pBuffer );

	// URL.	
	std::string strURLs;
	DNTableFileFormat * pTable = GetDNTable(CDnTableDB::TCASHTAB);
	if( pTable )
	{
		DNTableCell * pCell = pTable->GetFieldFromLablePtr( 1, "_MainBannerURL");
		if( pCell )
			strURLs.assign( pCell->GetString() );
	}

	// Test용.
	if( strURLs.empty() )	
		strURLs.assign( "" );

	//std::string strURLs = "http://dn.image.happyoz.com/Img/2013/01/25/0c850d0e5dedcdc54908ed36774082d33.jpg;http://dn.image.happyoz.com/Img/2013/01/09/5d6836f4f73f37afecf44e07f9e9d133.jpg;http://dn.image.happyoz.com/Img/2013/01/27/19ada7f82dc5d40977282f7bdd7f80c2.jpg";
	std::vector< std::string > arrURL;
	TokenizeA( strURLs, arrURL, std::string(";") );

	HRESULT hr;
	EtTextureHandle hTexture;

	int size = (int)arrURL.size();
	for( int i=0; i<size; ++i )
	{
		//이미지 경로 설정
		char BANNERIMAGE_FILE_NAME[1024] = "";
		wsprintf( pImagePath, L"%s\\WebBanner_%d.jpg", pBuffer, i );
		WideCharToMultiByte( CP_ACP, 0, pImagePath, -1, BANNERIMAGE_FILE_NAME, sizeof(BANNERIMAGE_FILE_NAME), NULL, NULL );

		std::string & url = arrURL[ i ];
		DeleteUrlCacheEntryA( url.c_str() );

		// Download.
		hr = URLDownloadToFileA( NULL, url.c_str(), BANNERIMAGE_FILE_NAME , 0, NULL  );

		if( hr == S_OK )
		{
			CFileStream Stream( BANNERIMAGE_FILE_NAME );
			hTexture = (new CEtTexture)->GetMySmartPtr();
			hTexture->LoadResource( &Stream );			

			DeleteFileA( BANNERIMAGE_FILE_NAME );

			m_vTexture.push_back( hTexture );
		}
	}

	//// 웹주소가 잘못되었거나 다운로드받지 못하였을때에는 기본이미지적용.
	//if( m_vTexture.empty() )
	//{
	//	m_vTexture.push_back( EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName("AlteaBase.dds").c_str() ) );
	//}

	//HRESULT hr;
	//EtTextureHandle hTexture;
	//std::string url = "http://dn.image.happyoz.com/Img/2013/01/25/0c850d0e5dedcdc54908ed36774082d33.jpg";
	//DeleteUrlCacheEntryA( url.c_str() );

	//hr = URLDownloadToFileA( NULL, url.c_str(), BANNERIMAGE_FILE_NAME , 0, NULL  );

	//if( hr == S_OK )
	//{
	//	CFileStream Stream( BANNERIMAGE_FILE_NAME );		
	//	hTexture = (new CEtTexture)->GetMySmartPtr();
	//	hTexture->LoadResource( &Stream );			

	//	DeleteFileA( BANNERIMAGE_FILE_NAME );

	//	m_vTexture.push_back( hTexture );
	//}

}


void CDnCashShopMainBanner::Process( float dTime )
{
	if( !IsShow() )
		return;

	if( m_time >= 2.0f )
	{
		m_time = 0.0f;

		++m_crrIdx;
		if( m_crrIdx >= (int)m_vTexture.size() )
			m_crrIdx = 0;

		m_pTextureCtr->SetTexture( m_vTexture[ m_crrIdx ] );
	}
	m_time += dTime;


	CEtUIDialog::Process( dTime );
}


void CDnCashShopMainBanner::Show( bool bShow )
{
	

	CEtUIDialog::Show( bShow );
} 


void CDnCashShopMainBanner::OnChangeResolution()
{	
	CEtUIDialog::OnChangeResolution();

	float x, y;
	GetParentDialog()->GetPosition( x, y );
	this->SetPosition( x, y );	
}

#endif // PRE_ADD_CASHSHOP_RENEWAL