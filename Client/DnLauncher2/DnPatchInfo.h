//
//	DnPatchInfo.h	패치 관련 정보 저장 클래스
//

#pragma once

struct stLoginServerSet
{
	CString	m_strLoginUrl;	// 로그인 서버 주소 Ip
	WORD	m_wLoginPort;	// 로그인 서버 Port
};

struct stPartitionListSet
{
	CString m_strGuidepageUrl;	// 런처 가이드 페이지 주소
	CString m_strHomepageUrl;	// 홈페이지 주소
	CString m_strPartitionName;	// 파티션 이름
	CString m_strVersionUrl;	// PatchInfoServer.cfg 파일의 주소
	CString m_strPatchUrl;		// 패치 받을 CDN의 주소
	CString m_strStateTestUrl;	// 파티션 상태 체크하기 위한 웹페이지 주소
	WORD	m_wPartitionId;		// 파티션의 Id
	WORD	m_wNew;				// 신규 파티션 표기 Flag
	WORD	m_wOpen;			// 서버점검 때 사용 할 플래그
	
	std::vector<stLoginServerSet> m_vecLoginServerList;	// 로그인서버 리스트 목록

	stPartitionListSet() : m_wPartitionId( 0 ), m_wNew( 0 ), m_wOpen( 1 ) {}
	~stPartitionListSet() { m_vecLoginServerList.clear(); }
};

struct stChannelListSet
{
	CString m_strChannelName;	// 채널 이름
	std::vector<stPartitionListSet> m_vecPartitionList;	// 파티션 정보 리스트

	~stChannelListSet() { m_vecPartitionList.clear(); }
};

//////////////////////////////////////////////////////////////////////////
// PatchConfigList.xml의 하위 구조
// Channel
//		L Partition
//				L LoginIpList
//////////////////////////////////////////////////////////////////////////

// PatchInfo Class
class CDnPatchInfo
{
public:
	CDnPatchInfo();
	virtual ~CDnPatchInfo();

	static CDnPatchInfo& CDnPatchInfo::GetInstance();
	HRESULT			Init();
	void			SetClientPath( CString strClientPath );
	CString			GetClientPath()		{ return m_strClientPath; }
	BOOL			SetPatchInfo();		// 패치정보 셋팅
	BOOL			IsSetPatchInfo()	{ return m_bSetPatchInfo; }
	void			LoadClientVersion();// 클라이언트 버전 정보 읽기
	void			LoadServerVersion();// 서버 버전 정보 읽기

	void			SetClientVersion( int nClientVersion ) { m_nClientVersion = nClientVersion; }
	void			SetServerVersion( int nServerVersion ) { m_nServerVersion = nServerVersion; }
	int				GetClientVersion() { return m_nClientVersion; }
	int				GetServerVersion() { return m_nServerVersion; }


	CString			GetGuidepageUrl()		{ return m_strGuidepageUrl; }
	CString			GetHomepageUrl()		{ return m_strHomepageUrl; }
	CString			GetPartitionName()		{ return m_strPartitionName; }
	CString			GetPatchUrl()			{ return m_strPatchUrl; }
	CString			GetTotalParameter()		{ return m_strTotalParam; }
	CString			GetOptionParameter()	{ return m_strOptionParam; }
	CString			GetVersionURL()			{ return m_strVersionUrl; }
	
	std::vector<int>&				GetSkipVersion() { return m_vecSkipVersion; }
	std::vector<stChannelListSet>&	GetChannelList() { return m_vecChannelList; }

#ifdef _USE_AUTOUPDATE
	int GetNextVersion() { return m_nNextVersion; }
#endif // _USE_AUTOUPDATE
	WORD GetPartitionOpen() { return m_wOpen; }

#ifdef _USE_PARTITION_SELECT
	int		GetPartitionId( int nChannelNum, int nPartitionNum );
	CString GetBaseGuidePageUrl();
#endif // _USE_PARTITION_SELECT

#ifdef _USE_MULTILANGUAGE
	void	SetLanguageParam( CString strLanguageParam );
	CString GetLanguageParam() { return m_strLanguageParam; }
#endif // _USE_MULTILANGUAGE
	int		GetLanguageOffset() { return m_nLanguageOffset; }

private:
	HRESULT DownLoadPatchConfigList();		// PatchConfigList.xml 다운로드
	HRESULT ParsingPatchConfigList();		// PatchConfigList.xml 파싱
	void	LoadPatchVersionInfo();					// 패치 버전정보 읽기
	void	SetPartitionList(	stChannelListSet& channellistset, WORD dwNew, WORD wPartitionId, WORD wOpen, CString strGuidepageUrl, CString strHomepageUrl, 
								CString strPartitionName, CString strVersionUrl, CString strPatchUrl, CString strStateTestUrl, std::vector<stLoginServerSet>& vecLoginServerList );

	void	ResetPatchInfo();						// 패치정보 초기화
	BOOL	SetBasePatchInfo();						// 기본 패치정보 셋팅
	BOOL	SetFirstPatchInfo();					// FIrstPatch 정보 셋팅
	BOOL	SetParameter();							// 파라미터 셋팅
	int		LoadVersion( CString& szFilePath );		// 파일에서 버전 정보 읽기
	int		LoadVersion( CString& szFilePath, CString szTypeName );
	BOOL	ReadConfig();								// 로컬에서 config파일 읽기
	void	ParseCommandLineDev( LPCTSTR szCommandLine );

public:
	bool	LoadVersionData();

	
	
#ifdef _USE_SINGLE_CLIENT
	void LoadClientVersionFromResourceFile();
#else // _USE_SINGLE_CLIENT
	void LoadClientVersionFromVersionFile();
#endif // _USE_SINGLE_CLIENT

private:
	BOOL							m_bSetPatchInfo;
	std::vector<stChannelListSet>	m_vecChannelList;
	std::vector<int>				m_vecSkipVersion; //스킵할 버젼

	// 선택된 파티션의 패치 정보
	CString m_strGuidepageUrl;	// 런처 가이드 페이지 주소
	CString m_strHomepageUrl;	// 홈페이지 주소
	CString m_strPartitionName;	// 파티션 이름
	CString m_strVersionUrl;	// PatchInfoServer.cfg 파일의 주소
	CString m_strPatchUrl;		// 패치 받을 CDN의 주소

	std::vector<stLoginServerSet> m_vecLoginServerList;	// 로그인서버 리스트 목록

	int		m_nClientVersion;			// 현재 클라이언트의 버전
	int		m_nServerVersion;			// 현재 서버의 버전


#ifdef _USE_AUTOUPDATE
	int		m_nNextVersion;		// 오토 업데이트에서 사용할 다음 패치 버전
#endif // _USE_AUTOUPDATE

	int		m_nLanguageOffset;	// Resource.h의 언어별 Offset
	WORD	m_wOpen;			// 파티션 오픈 플래그

	CString m_strClientPath;	// 클라이언트의 설치 경로
	CString m_strIPList;		// 파라미터 넘길 때 사용할 로그인 IP 묶음 (ex : 192.168.0.1;192.168.0.2;...)
	CString m_strPortList;		// 파라미터 넘길 때 사용할 로그인 Port 묶음 (ex : 4006;4007;...)
	CString m_strOptionParam;	// 옵션 파라미터
	CString m_strTotalParam;	// 전체 파라미터 (클라이언트 구동 시 전달되는 파라미터)

#ifdef _USE_MULTILANGUAGE
	CString m_strLanguageParam;	// 다국어 언어 정보 (클라이언트 구동 시 전달되는 파라미터)
#endif // _USE_MULTILANGUAGE
};

#define DNPATCHINFO CDnPatchInfo::GetInstance()