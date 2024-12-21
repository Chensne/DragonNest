//
//	DnOptionData.h	�ɼ� ���� ���� ���� Ŭ����
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
	// Config.ini�� ���� �� Data
	int m_nWidth;
	int m_nHeight;
	int m_nGraphicQuality;
	int m_nTextureQuality;
	int m_bWindow;
	int m_bVSync;
	int m_bAutoGameStart;		// �ڵ� ������ ini���Ͽ� ������� ����

	// Partition.ini�� ���� �� Data
#ifdef _USE_PARTITION_SELECT
	int	m_nSelectChannelNum;
	int	m_nSelectPartitionId;
#endif // _USE_PARTITION_SELECT

	std::vector<stOptionData> m_vecOptionData;	// ��ó���� ����ϴ� �ػ�,ǰ��,â��� �ɼ������� �����ϰ� vector�� ��Ƴ��´�.

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

private:
	BOOL SetOptionValue( FILE *stream, char * szString );
};

#define DNOPTIONDATA CDnOptionData::GetInstance()