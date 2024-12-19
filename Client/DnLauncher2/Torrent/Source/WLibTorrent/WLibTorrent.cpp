#include "libtorrent/pch.hpp"
#include "WLibTorrent.h"


WLibTorrent* WLibTorrent::m_pInstance = NULL;


WLibTorrent::WLibTorrent()
: m_pSession( NULL )
, m_nMakeTorrentProgressCount( 0 )
, m_nMakeTorrentTotalNum( 0 )
, m_bMakeSaveFile( false )
{
}

WLibTorrent::~WLibTorrent()
{
	using namespace libtorrent;

	if( m_pSession )
	{
		int nNumResumeData = 0;
		m_pSession->pause();
		for( handles_t::iterator i=m_handles.begin(); i!=m_handles.end(); ++i )
		{
			torrent_handle& h = i->second;
			if( !h.is_valid() ) continue;
			if( h.is_paused() ) continue;
			if( !h.has_metadata() ) continue;
			h.save_resume_data();
			++nNumResumeData;
		}

		while( nNumResumeData > 0 )
		{
			alert const* pAlert = m_pSession->wait_for_alert( seconds( 30 ) );
			if( pAlert == 0 )
				break;

			std::auto_ptr<alert> holder = m_pSession->pop_alert();
			if( alert_cast<save_resume_data_failed_alert>( pAlert ) )
			{
				--nNumResumeData;
				continue;
			}

			save_resume_data_alert const* rd = alert_cast<save_resume_data_alert>( pAlert );
			if( !rd ) continue;
			--nNumResumeData;

			if( !rd->resume_data ) continue;

			if( m_bMakeSaveFile )
			{
				torrent_handle TorrentHandle = rd->handle;
				std::vector<char> out;
				bencode( std::back_inserter( out ), *rd->resume_data );

				fs::path savePath = TorrentHandle.save_path();
				std::string strPath = savePath.directory_string();
				SaveFile( strPath + TorrentHandle.name() + ".resume", out );
			}
		}
#ifndef TORRENT_DISABLE_DHT
		m_pSession->stop_dht();
#endif
		if( m_bMakeSaveFile )
		{
			entry session_state;
			m_pSession->save_state( session_state );

			std::vector<char> out;
			bencode( std::back_inserter( out ), session_state );
			SaveFile( ".SessionState", out );
		}

		m_handles.clear();
		SAFE_DELETE( m_pSession );
	}
}

WLibTorrent* WLibTorrent::GetInstance()
{
	if( m_pInstance == NULL )
	{
		m_pInstance = new WLibTorrent;
	}

	return m_pInstance;
}

void WLibTorrent::DestroyInstance()
{
	SAFE_DELETE( m_pInstance );
}

bool WLibTorrent::InitSession( LPCWSTR wstrIdentification )
{
	std::string strIdentification;
	W2AString( wstrIdentification, strIdentification );

	return InitSession( strIdentification );
}

bool WLibTorrent::InitSession( std::string const& strIdentification )
{
	using namespace libtorrent;

	if( m_pSession )
		SAFE_DELETE( m_pSession );

	m_pSession = new session( fingerprint( "LT", LIBTORRENT_VERSION_MAJOR, LIBTORRENT_VERSION_MINOR, 0, 0 ), session::start_default_features | session::add_default_plugins,
								alert::all_categories & ~(alert::dht_notification + alert::progress_notification + alert::debug_notification + alert::stats_notification) );

	if( m_pSession == NULL )
		return false;

	std::vector<char> in;
	boost::system::error_code ec;
	if( load_file( ".SessionState", in, ec ) == 0 )
	{
		lazy_entry e;
		if( lazy_bdecode( &in[0], &in[0] + in.size(), e ) == 0 )
			m_pSession->load_state( e );
	}

#ifndef TORRENT_DISABLE_DHT
	m_pSession->add_dht_router( std::make_pair( std::string( "router.bittorrent.com" ), 6881 ) );
	m_pSession->add_dht_router( std::make_pair( std::string( "router.utorrent.com" ), 6881 ) );
	m_pSession->add_dht_router( std::make_pair( std::string( "router.bitcomet.com" ), 6881 ) );
	m_pSession->start_dht();
#endif

	m_pSession->set_max_half_open_connections( m_stSessionSetting.m_nMaxHalfOpenConnections );
	m_pSession->set_download_rate_limit( m_stSessionSetting.m_nDownloadRateLimit * 1000 );
	m_pSession->set_upload_rate_limit( m_stSessionSetting.m_nUploadRateLimit * 1000 );
	m_pSession->set_max_uploads( m_stSessionSetting.m_nMaxUploads );
	m_pSession->set_max_connections( m_stSessionSetting.m_nMaxConnectinos );

	proxy_settings ps;
	m_pSession->set_proxy( ps );
	if( !m_pSession->listen_on( std::make_pair( m_stSessionSetting.m_nListenPort, m_stSessionSetting.m_nListenPort + 10 ) ) )
		return false;

	session_settings settings;
	settings.allow_multiple_connections_per_ip = m_stSessionSetting.m_bAllowMultipleConnectionsPerIp;
	settings.announce_to_all_tiers = m_stSessionSetting.m_bAnnounceToAllTiers;
	settings.user_agent = strIdentification + LIBTORRENT_VERSION;
	settings.auto_upload_slots_rate_based = true;
	settings.optimize_hashing_for_speed = false;
	settings.disk_cache_algorithm = session_settings::largest_contiguous;
#ifndef TORRENT_DISABLE_DHT
	settings.use_dht_as_fallback = false;
#endif // TORRENT_DISABLE_DHT
	m_pSession->set_settings( settings );

	return true;
}

bool WLibTorrent::AddTorrent( LPCWSTR wstrTorrentName, LPCWSTR wstrSavePath, bool bRemoveRootDirForMultiplTorrent )
{
	std::string strTorrentName;
	W2AString( wstrTorrentName, strTorrentName );
	std::string strSavePath;
	W2AString( wstrSavePath, strSavePath );

	return AddTorrent( strTorrentName, strSavePath, bRemoveRootDirForMultiplTorrent );
}

bool WLibTorrent::AddTorrent( std::string const& strTorrentName, std::string const& strSavePath, bool bRemoveRootDirForMultiplTorrent )
{
	using namespace libtorrent;

	if( m_pSession == NULL || strTorrentName.length() == 0 )
	{
		m_stTorrentSetting.Clear();
		return false;
	}

	if( strSavePath.length() > 0 )
		m_stTorrentSetting.m_strSavePath = strSavePath;

	boost::system::error_code ec;
	if( std::strstr( strTorrentName.c_str(), "magnet:" ) == strTorrentName.c_str() )
	{
		add_torrent_params p;
		p.seed_mode = m_stTorrentSetting.m_bSeedMode;
		p.save_path = m_stTorrentSetting.m_strSavePath;
		p.storage_mode = (storage_mode_t)m_stTorrentSetting.m_nAllocationMode;
		
		torrent_handle h = add_magnet_uri( *m_pSession, strTorrentName.c_str(), p, ec );
		if( ec )
		{
			m_stTorrentSetting.Clear();
			return false;
		}

		m_handles.insert( std::pair<const std::string, torrent_handle>( std::string(), h ) );

		h.set_max_connections( m_stTorrentSetting.m_nMaxConnectionsPerTorrent );
		h.set_max_uploads( -1 );
		h.set_ratio( m_stTorrentSetting.m_fPreferredRatio );
		h.set_upload_limit( m_stTorrentSetting.m_nTorrentUploadLimit );
		h.set_download_limit( m_stTorrentSetting.m_nTorrentDownloadLimit );
	}

	std::string strUTF8 = GetStringtoUTF8( strTorrentName );
	boost::intrusive_ptr<torrent_info> pTorrentInfo;
	pTorrentInfo = new torrent_info( strUTF8.c_str(), ec );
	if( ec )
	{
		m_stTorrentSetting.Clear();
		return false;
	}

	if( bRemoveRootDirForMultiplTorrent )
	{
		int nNum = pTorrentInfo->num_files();
		if( nNum > 0 )
		{
			int i = 0;
			torrent_info::file_iterator iter = pTorrentInfo->begin_files();
			for( ; iter != pTorrentInfo->end_files(); iter++, i++ )
			{
				std::string strChangeFileName;
				strChangeFileName = convert_to_native( (*iter).path.external_file_string() );
				int nSlashIndex = strChangeFileName.rfind( "\\" );
				strChangeFileName.erase( 0, nSlashIndex + 1 );
				pTorrentInfo->rename_file( i, strChangeFileName );
			}
		}
	}

	add_torrent_params TorrentParam;
	TorrentParam.seed_mode = m_stTorrentSetting.m_bSeedMode;

	std::vector<char> buf;
	if( load_file( m_stTorrentSetting.m_strSavePath + pTorrentInfo->name() + ".resume", buf, ec ) == 0 )
		TorrentParam.resume_data = &buf;

	TorrentParam.ti = pTorrentInfo;
	TorrentParam.save_path = m_stTorrentSetting.m_strSavePath;
	TorrentParam.storage_mode = (storage_mode_t)m_stTorrentSetting.m_nAllocationMode;
	TorrentParam.paused = true;
	TorrentParam.duplicate_is_error = false;
	TorrentParam.auto_managed = true;
	torrent_handle TorrentHandle = m_pSession->add_torrent( TorrentParam, ec );
	if( ec )
	{
		m_stTorrentSetting.Clear();
		return false;
	}

	m_handles.insert( std::pair<const std::string, torrent_handle>( std::string( strTorrentName ), TorrentHandle ) );

	TorrentHandle.set_max_connections( m_stTorrentSetting.m_nMaxConnectionsPerTorrent );
	TorrentHandle.set_max_uploads( -1 );
	TorrentHandle.set_ratio( m_stTorrentSetting.m_fPreferredRatio );
	TorrentHandle.set_upload_limit( m_stTorrentSetting.m_nTorrentUploadLimit );
	TorrentHandle.set_download_limit( m_stTorrentSetting.m_nTorrentDownloadLimit );
#ifndef TORRENT_DISABLE_RESOLVE_COUNTRIES
	TorrentHandle.resolve_countries( true );
#endif // TORRENT_DISABLE_RESOLVE_COUNTRIES

	m_stTorrentSetting.Clear();
	return true;
}

torrent_handle*	WLibTorrent::GetTorrentHandle( LPCWSTR wstrTorrentName )
{
	std::string strTorrentName;
	W2AString( wstrTorrentName, strTorrentName );

	return GetTorrentHandle( strTorrentName );
}

torrent_handle*	WLibTorrent::GetTorrentHandle( std::string const& strTorrentName )
{
	using namespace libtorrent;

	if( strTorrentName.length() == 0 )
		return NULL;

	torrent_handle* pTorrentHandle = NULL;
	handles_t::iterator iter = m_handles.find( strTorrentName );
	if( iter != m_handles.end() )
		pTorrentHandle = &iter->second;

	return pTorrentHandle;
}

bool WLibTorrent::MakeTorrent( LPCWSTR wstrFileName, std::vector<std::wstring>& vecWstrWebSeeds, std::vector<std::wstring>& vecWstrTrackers,
								LPCWSTR wstrOuputName, LPCWSTR wstrCreator, int flags, int nPieceSize, int nPadFileLimit )
{
	std::string strFileName;
	W2AString( wstrFileName, strFileName );
	std::string strOutputName;
	W2AString( wstrOuputName, strOutputName );
	std::string strCreator;
	W2AString( wstrCreator, strCreator );

	std::vector<std::string> vecStrWebSeeds;
	std::vector<std::string> vecStrTrackers;

	for( int i=0; i<static_cast<int>( vecWstrWebSeeds.size() ); i++ )
	{
		std::string strWebSeed;
		W2AString( vecWstrWebSeeds[i].c_str(), strWebSeed );
		vecStrWebSeeds.push_back( strWebSeed );
	}

	for( int i=0; i<static_cast<int>( vecWstrTrackers.size() ); i++ )
	{
		std::string strTracker;
		W2AString( vecWstrTrackers[i].c_str(), strTracker );
		vecStrTrackers.push_back( strTracker );
	}

	bool bReturn = MakeTorrent( strFileName, vecStrWebSeeds, vecStrTrackers, strOutputName, strCreator, flags, nPieceSize, nPadFileLimit );

	vecStrWebSeeds.clear();
	vecStrTrackers.clear();

	return bReturn;
}

bool WLibTorrent::MakeTorrent( std::string const& strFileName, std::vector<std::string>& vecStrWebSeeds, std::vector<std::string>& vecStrTrackers,
								std::string const& strOuputName, std::string const& strCreator, int flags, int nPieceSize, int nPadFileLimit )
{
	using namespace libtorrent;
	using namespace boost::filesystem;

	if( strFileName.length() == 0 || strOuputName.length() == 0 )
		return false;

#ifndef BOOST_NO_EXCEPTIONS
	try
	{
#endif
		m_nMakeTorrentProgressCount = 0;
		m_nMakeTorrentTotalNum = 0;

		file_storage fileStorage;
		path fullPath = complete( path( strFileName ) );

		add_files( fileStorage, fullPath, libtorrent::detail::default_pred, flags );
		if( fileStorage.num_files() == 0 )
			return false;

		create_torrent cTorrent( fileStorage, nPieceSize, nPadFileLimit, flags );
		for (std::vector<std::string>::iterator i = vecStrTrackers.begin(), end(vecStrTrackers.end()); i != end; ++i )
			cTorrent.add_tracker( *i );

		for( std::vector<std::string>::iterator i = vecStrWebSeeds.begin(), end(vecStrWebSeeds.end()); i != end; ++i )
			cTorrent.add_url_seed( *i );
		
		error_code ec;
		set_piece_hashes( cTorrent, fullPath.branch_path(), ec );
		if( ec )
			return false;

		cTorrent.set_creator( strCreator.c_str() );

		// create the torrent and print it to stdout
		std::vector<char> torrent;
		bencode( back_inserter(torrent), cTorrent.generate() );
		FILE* output = stdout;
		if( !strOuputName.empty() )
			output = fopen( strOuputName.c_str(), "wb+" );

		fwrite( &torrent[0], 1, torrent.size(), output );

		if( output != stdout )
			fclose( output );

/*		if (!merklefile.empty())
		{
			output = fopen(merklefile.c_str(), "wb+");
			int ret = fwrite(&t.merkle_tree()[0], 20, t.merkle_tree().size(), output);
			if (ret != t.merkle_tree().size() * 20)
			{
				fprintf(stderr, "failed to write %s: (%d) %s\n"
					, merklefile.c_str(), errno, strerror(errno));
			}
			fclose(output);
		}
*/
#ifndef BOOST_NO_EXCEPTIONS
	}
	catch( std::exception& e )
	{
		return false;
	}
#endif

	return true;
}

int SaveFile( boost::filesystem::path const& filename, std::vector<char>& v )
{
	using namespace libtorrent;

	file f;
	boost::system::error_code ec;
	if( !f.open( filename, file::write_only, ec ) ) return -1;
	if( ec ) return -1;
	file::iovec_t b = {&v[0], v.size()};
	size_type written = f.writev( 0, &b, 1, ec );
	if( written != v.size() ) return -3;
	if( ec ) return -3;
	return 0;
}

const int STR_BUFFER_LENGTH	= 16384;

std::string GetUTF8toString( std::string const& strUTF8 )
{
	using namespace libtorrent;

	std::wstring wString;
	utf8_wchar( strUTF8, wString );
	char szBuff[STR_BUFFER_LENGTH] = "";
	ZeroMemory( szBuff, sizeof(char) * STR_BUFFER_LENGTH );
	WideCharToMultiByte( CP_ACP, 0, wString.c_str(), -1, szBuff, STR_BUFFER_LENGTH, NULL, NULL );
	std::string strString = szBuff;

	return strString;
}

std::string GetStringtoUTF8( std::string const& strString )
{
	using namespace libtorrent;

	WCHAR wszBuff[STR_BUFFER_LENGTH] = L"";
	ZeroMemory( wszBuff, sizeof(char) * STR_BUFFER_LENGTH );
	MultiByteToWideChar( CP_ACP, 0, strString.c_str(), -1, wszBuff, STR_BUFFER_LENGTH );
	std::wstring wString = wszBuff;
	std::string strUTF8;
	wchar_utf8( wString, strUTF8 );

	return strUTF8;
}

void W2AString( LPCWSTR wStr, std::string& aStr )
{
	int nStrLen = wcslen( wStr );
	if( nStrLen > 0 )
	{
		int cSize = WideCharToMultiByte( CP_ACP, 0, wStr, nStrLen, NULL, 0, NULL, NULL );
		aStr.resize( static_cast<size_t>(cSize), '\0' );
		WideCharToMultiByte( CP_ACP, 0, wStr, nStrLen, reinterpret_cast<char*>(&aStr[0]), cSize, NULL, NULL );
	}
}