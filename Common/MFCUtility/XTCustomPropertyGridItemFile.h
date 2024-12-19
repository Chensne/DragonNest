
#pragma once

class CXTCustomPropertyGridItemFile : public CXTPPropertyGridItem
{
public:
	CXTCustomPropertyGridItemFile(CString strCaption, LPCTSTR strValue = NULL, CString* m_pBindString = NULL);

	enum
	{
		OtherToWav	= 1,
		WavToOther	= 2,
	};
	void SetExt( const char *szStr, const char *szFileDesc );
	void SetInitDirectory( const std::string szInitDirectory );

#ifdef FILEDLG_AUTOPATH
	static char s_szDirectory[256];
	static CString s_szExt;
	static int s_nMode;
#endif

	std::string m_szInitDirectory;

protected:
	CString m_szExt;
	CString m_szFileDesc;

	virtual void OnInplaceButtonDown();
	virtual BOOL OnKeyDown (UINT nChar);
};

