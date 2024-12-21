#pragma once

struct SAttachFileHeader
{
	char FileName[_MAX_FNAME];
	int  FileSize; 
		
	SAttachFileHeader()
	{
		ZeroMemory(FileName , sizeof(FileName) );
		FileSize = 0 ; 
	}
};

class CDnAttachFile
{
public:
	CDnAttachFile(void);
	~CDnAttachFile(void);

	inline FILE*	GetFP() { return m_FP;}
	inline int		GetExeModuleSize()	{	return m_ExeModuleSize;	}
	inline int		GetCurDetachSize()	{	return m_CurDetachSize;	}

	void Initialize();
	bool Open(const char * FileName);
	bool AttachFile( char* DestDir, char* FileNameOnly );
	bool Create( const char* ModuleFileName, const char* NewFileName );
	void Close();
	bool Detach(const char* DestDir);
	bool Detach( const char* DestDir, const SAttachFileHeader* pHeader );
	bool ReadHeader(SAttachFileHeader * pHeader);
	void WriteVersion(int PrevVersion , int LastVersion);
protected:
	FILE* m_FP; 
	char  m_Token[17];
	int m_CurDetachSize;
	int m_ExeModuleSize;

};
