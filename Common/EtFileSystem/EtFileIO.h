#pragma once

class CEtPackingFile;

class CEtFileIO
{
public:
	enum FileIOType
	{
		// 일반 파일 입출력 
		ET_STD_FILE = 0,
		ET_PACK_FILE = 1
	};

	enum AccessType
	{
		ET_READ = 0,
		ET_WRITE,
		ET_ALL
	};
	enum SeekType
	{
		// winbase.h 에 FILE_BEGIN = 0 이거랑 같은 값인데 이름만 다르다, 숫자 바꾸면 안됨!
		ET_SEEK_BEGIN = 0,		// 파일 처음 기준
		ET_SEEK_CURRENT = 1,	// 파일 현재 기준
		ET_SEEK_END = 2			// 파일 끝 기준 
	};

public:
	
	CEtFileIO() { }
	virtual ~CEtFileIO() { }

	virtual bool	Open(const TCHAR* szFileName, AccessType nAccessMode) = 0;
	virtual bool	Close() = 0;

	virtual int		Read(void* pBuffer, int nLen) = 0;
	virtual int		Write(const void* pBuffer, int nLen ) = 0;
	virtual int		Seek(int nOffset, SeekType nPosition) = 0;
	virtual int		Tell() = 0;

public:
	
	// 타입에 맞는 파일입출력 객체를 생성해서 돌려준다 ( 해제는 알아서.. )
	static	CEtFileIO*	CreateFileIO(FileIOType opt);

};



