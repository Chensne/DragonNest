
#pragma once


#include "libtorrent/session.hpp"
#include "libtorrent/magnet_uri.hpp"
#include "libtorrent/utf8.hpp"
#include "libtorrent/alert_types.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/create_torrent.hpp"


using namespace libtorrent;

typedef std::multimap<std::string, torrent_handle> handles_t;

struct TorrentSetting
{
	int				m_nTorrentUploadLimit;		// Limit KByte;
	int				m_nTorrentDownloadLimit;	// Limit KByte;
	int				m_nMaxConnectionsPerTorrent;
	int				m_nAllocationMode;
	bool			m_bSeedMode;
	float			m_fPreferredRatio;
	std::string		m_strSavePath;

	TorrentSetting() : m_nTorrentUploadLimit( 0 ), m_nTorrentDownloadLimit( 0 ), m_nMaxConnectionsPerTorrent( 50 ), 
					m_nAllocationMode( libtorrent::storage_mode_sparse ), m_bSeedMode( false ), m_fPreferredRatio( 0.0f ), m_strSavePath( "." ) {}

	void Clear() { m_nTorrentUploadLimit = 0; m_nTorrentDownloadLimit = 0; m_nMaxConnectionsPerTorrent = 50;
				m_nAllocationMode = libtorrent::storage_mode_sparse; m_bSeedMode = false; m_fPreferredRatio = 0.0f; m_strSavePath = "."; }
};

struct SessionSetting
{
	int	m_nListenPort;
	int m_nMaxHalfOpenConnections;
	int m_nDownloadRateLimit;	// Limit KByte;
	int m_nUploadRateLimit;		// Limit KByte;
	int m_nMaxUploads;
	int m_nMaxConnectinos;
	bool m_bAllowMultipleConnectionsPerIp;
	bool m_bAnnounceToAllTiers;

	SessionSetting() : m_nListenPort( 6881 ), m_nMaxHalfOpenConnections( 8 ), m_nDownloadRateLimit( 0 ), m_nUploadRateLimit( 0 ),
					m_nMaxUploads( 8 ), m_nMaxConnectinos( 200 ) , m_bAllowMultipleConnectionsPerIp( false ), m_bAnnounceToAllTiers( false ) {}

	void Clear() { m_nListenPort = 6881; m_nMaxHalfOpenConnections = 8; m_nDownloadRateLimit = 0; m_nUploadRateLimit = 0;
				m_nMaxUploads = 8, m_nMaxConnectinos = 200; m_bAllowMultipleConnectionsPerIp = false; m_bAnnounceToAllTiers = false; }
};

class WLibTorrent
{
private:
	WLibTorrent();
public:
	virtual ~WLibTorrent();

public:
	static WLibTorrent*	GetInstance();
	static void			DestroyInstance();

	void	MakeSaveFile( bool bMakeSaveFile ) { m_bMakeSaveFile = bMakeSaveFile; }
	void	SetSessionSetting( SessionSetting& stSessionSettings ) { m_stSessionSetting = stSessionSettings; }
	bool	InitSession( LPCWSTR wstrIdentification );
	bool	InitSession( std::string const& strIdentification );

	void	SetTorrentSetting( TorrentSetting& stTorrentSetting ) { m_stTorrentSetting = stTorrentSetting; }
	bool	AddTorrent( LPCWSTR wstrTorrentName, LPCWSTR wstrSavePath, bool bRemoveRootDirForMultiplTorrent = false );
	bool	AddTorrent( std::string const& strTorrentName, std::string const& strSavePath, bool bRemoveRootDirForMultiplTorrent = false );

	torrent_handle*	GetTorrentHandle( LPCWSTR wstrTorrentName );
	torrent_handle*	GetTorrentHandle( std::string const& strTorrentName );

	bool	MakeTorrent( LPCWSTR wstrFileName, std::vector<std::wstring>& vecWstrWebSeeds, std::vector<std::wstring>& vecWstrTrackers,
						LPCWSTR wstrOuputName, LPCWSTR wstrCreator, int flags = 0, int nPieceSize = 0, int nPadFileLimit = -1 );
	bool	MakeTorrent( std::string const& strFileName, std::vector<std::string>& vecStrWebSeeds, std::vector<std::string>& vecStrTrackers,
						std::string const& strOuputName, std::string const& strCreator, int flags = 0, int nPieceSize = 0, int nPadFileLimit = -1 );

	int		GetMakeTorrentProgressCount() { return m_nMakeTorrentProgressCount; }
	int		GetMakeTorrentTotalNum() { return m_nMakeTorrentTotalNum; }

protected:
	SessionSetting	m_stSessionSetting;		// ººº« º≥¡§ ¿”Ω√ ∞¥√º
	TorrentSetting	m_stTorrentSetting;		// ≈‰∑ª∆Æ º≥¡§ ¿”Ω√ ∞¥√º

	session*		m_pSession;				// ººº«
	handles_t		m_handles;				// ≈‰∑ª∆Æ «ÓµÈ ∏ÆΩ∫∆Æ

	static WLibTorrent*		m_pInstance;	// ΩÃ±€≈Ê ∞¥√º

	int		m_nMakeTorrentProgressCount;
	int		m_nMakeTorrentTotalNum;
	bool	m_bMakeSaveFile;
};

int			SaveFile( boost::filesystem::path const& filename, std::vector<char>& v );
std::string GetUTF8toString( std::string const& strUTF8 );
std::string GetStringtoUTF8( std::string const& strString );
void		W2AString( LPCWSTR wStr, std::string& aStr );

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x) { delete x; x = NULL; }
#endif // SAFE_DELETE