//
//	DnOptionData.h	옵션 관련 정보 저장 클래스
//

#pragma once

struct stOptionData
{
	std::string m_strOptionField;
	std::string m_strOptionValue;
};

class CDnOptionData
{
public:
	// Config.ini에 저장 될 Data
	int m_nWidth;
	int m_nHeight;
	int m_nGraphicQuality;
	int m_nTextureQuality;
	int m_bWindow;
	int m_bVSync;
	int m_bAutoGameStart;		// 자동 실행은 ini파일에 저장되지 않음
	int m_bNewUI;
	char username[32];
	char password[32];
	// Partition.ini에 저장 될 Data
#ifdef _USE_PARTITION_SELECT
	int	m_nSelectChannelNum;
	int	m_nSelectPartitionId;
#endif // _USE_PARTITION_SELECT

#ifdef _USE_MULTILANGUAGE
	int m_nLanguageID;
#endif // _USE_MULTILANGUAGE

	std::vector<stOptionData> m_vecOptionData;	// 런처에서 사용하는 해상도,품질,창모드 옵션정보를 제외하고 vector에 모아놓는다.

public:
	CDnOptionData();
	virtual ~CDnOptionData();

	static CDnOptionData& CDnOptionData::GetInstance();
	BOOL SaveConfigOption( HWND hWnd );
	BOOL LoadConfigOption( HWND hWnd );
#ifdef _USE_PARTITION_SELECT
	BOOL SavePartitionOption( HWND hWnd );
	BOOL LoadPartitionOption( HWND hWnd );
#endif // _USE_PARTITION_SELECT

#ifdef _USE_MULTILANGUAGE
	void SetLanguageID( int nLanguageID ) { m_nLanguageID = nLanguageID; }
#endif // _USE_MULTILANGUAGE

private:
	BOOL SetOptionValue( FILE *stream, char * szString );
};

#define DNOPTIONDATA CDnOptionData::GetInstance()