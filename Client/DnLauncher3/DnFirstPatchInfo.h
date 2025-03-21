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
	CString						m_strPostPatchList;			// PostPatch.txt 파일 경로.
	std::vector<std::string>	m_vecCopyList;				// 카피 파일 리스트
	std::vector<std::string>	m_vecDeleteList;			// 삭제 파일 리스트

	int							m_nLocalModuleVersion;		// 현재 모듈 버전.
	int							m_nServerModuleVersion;		// 서버의 모듈 버전.

	bool						m_bIsChangedLauncherFile;	// 런처 파일이 변경되었는가?

	int							m_nFirstPatchStatus;		// FirstPatch Status
	int							m_nIsFirstPatchProcess;		// 패치중인가?

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
	
	int				SetPatchInfo(); // 클라패치 이전의 패치를 총괄. ( 런처 & 모듈 )
};
#define DNFIRSTPATCHINFO DnFirstPatchInfo::GetInstance()