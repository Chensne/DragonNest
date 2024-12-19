#pragma once

class CIniHelper
{
private:
	std::string	m_fileName;
	bool m_checked;

private:
	bool CheckFile()
	{
		if ( m_checked )
			return true;

		// 파일이 존재하지 않으면 생성한다.
		if( GetFileAttributesA(m_fileName.c_str()) == INVALID_FILE_ATTRIBUTES)
		{
			FILE *fp = fopen(m_fileName.c_str(), "ab");
			if(fp)
			{
				m_checked = true;
				fclose(fp);
				return true;
			}
			else
				return false;
		}
		else
		{
			m_checked = true;
			return true;
		}

		return true;
		
	}

public:
	CIniHelper(const char* fileName = "")
	{
		SetFile(fileName);
	}

	void SetFile(const char* fileName)
	{
		m_checked = false;
		m_fileName = fileName;
	}

	// Ini 파일에서 int 값을 읽어온다.
	int Read(const char* app, const char* key, int defaultValue)
	{
		return GetPrivateProfileIntA(app, key, defaultValue, m_fileName.c_str());
	}

	// Ini 파일에서 문자열 값을 읽어온다.
	std::string Read(const char* app, const char* key, const char* defaultValue)
	{
		char buf[2048] = {0,} ;
		GetPrivateProfileStringA(app, key, defaultValue, buf, sizeof(buf), m_fileName.c_str());
		return std::string(buf);
	}

	// Ini 파일에 int 값을 쓴다.
	bool Write(const char* app, const char* key, int v)
	{

		if(!CheckFile())
			return false;

		char buf[2048] = {0,} ;
		StringCbPrintfA(buf, sizeof(buf), "%d", v);
		return WritePrivateProfileStringA(app, key, buf, m_fileName.c_str()) ? true : false;
	}

	// Ini 파일에 문자열 값을 쓴다.
	bool Write(const char* app, const char* key, const char* v)
	{
		if(!CheckFile())
			return false;

		return WritePrivateProfileStringA(app, key, v, m_fileName.c_str()) ? true : false;
	}

	// Ini 파일에 기록한 내용을 파일에 실제로 쓴다
	// 기록을 한 경우 반드시 호출해 준다.
	bool Flush()
	{
		if(!CheckFile())
			return false;

		return WritePrivateProfileStringA(NULL, NULL, NULL, m_fileName.c_str())  ? true : false ;
	}
};