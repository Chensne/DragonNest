#pragma once


class DnFirstPatchInfo
{
public:
	DnFirstPatchInfo(void);
	virtual ~DnFirstPatchInfo(void);
	static DnFirstPatchInfo& DnFirstPatchInfo::GetInstance();

	enum 
	{
		EM_FIRSTPATCH_OK = 0,
		EM_FIRSTPATCH_VERSION_FAIL ,
		EM_FIRSTPATCH_ING ,
		EM_FIRSTPATCH_NOT
	};

private:
	CString						m_strSavePostPathPath;
	CString						m_strPostPatchList;			// PostPatch.txt ���� ���.
	std::vector<std::string>	m_vecCopyList;				// ī�� ���� ����Ʈ
	std::vector<std::string>	m_vecDeleteList;			// ���� ���� ����Ʈ

	int							m_nLocalModuleVersion;		// ���� ��� ����.
	int							m_nServerModuleVersion;		// ������ ��� ����.

	bool						m_bIsChangedLauncherFile;	// ��ó ������ ����Ǿ��°�?

	int							m_nFirstPatchStatus;		// FirstPatch Status
	int							m_nIsFirstPatchProcess;		// ��ġ���ΰ�?

public:
	void			SetLocalModuleVersion( int nVersion )		{ m_nLocalModuleVersion  = nVersion;  }
	void			SetServerModuleVersion( int nVersion )		{ m_nServerModuleVersion = nVersion; }
	int				GetLocalModuleVersion()						{ return m_nLocalModuleVersion;  }
	int				GetServerModuleVersion()					{ return m_nServerModuleVersion; }

	void			SetIsChangedLauncherFile(bool bFlag)		{ m_bIsChangedLauncherFile = bFlag; }
	bool			GetIsChangedLauncherFile()					{ return m_bIsChangedLauncherFile;	}

	int				GetFirstPatchStatus()						{ return m_nFirstPatchStatus;	 }
	void			SetFirstPatchStatus(int nStatus)			{ m_nFirstPatchStatus = nStatus; }

	int				IsFirstPatchProcess()						{ return m_nIsFirstPatchProcess;	 }
	void			SetFirstPatchProcess(int nStatus)			{ m_nIsFirstPatchProcess = nStatus; }
	
	int				SetPatchInfo(); // Ŭ����ġ ������ ��ġ�� �Ѱ�. ( ��ó & ��� )
};
#define DNFIRSTPATCHINFO DnFirstPatchInfo::GetInstance()