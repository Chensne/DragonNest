#pragma once

#include "EtUITemplate.h"
#include "UIControlInfo.h"

#define UIT_FILE_STRING	"Eternity Engine UI Template File 0.2"
#define UIT_HEADER_RESERVED	( 1024 - sizeof( SUITemplateFileHeader ) )
#define UIT_FILE_VERSION_01		0x0101
#define UIT_FILE_VERSION_02		0x0102
#define UIT_FILE_VERSION		0x0103

struct SUITemplateFileHeader
{
	char szHeaderString[ 256 ];
	int nVersion;

	SUITemplateFileHeader()
	{
		SecureZeroMemory( this, sizeof(SUITemplateFileHeader) );
		strcpy_s( szHeaderString, sizeof(szHeaderString), UIT_FILE_STRING );
		nVersion = UIT_FILE_VERSION;
	}
};


class CUIToolTemplate
{
public:
	CUIToolTemplate(void);
	virtual ~CUIToolTemplate(void);

public:
	UI_CONTROL_TYPE m_UIType;
	std::vector< std::string > m_vecTextureName;
	std::vector< int > m_vecFontSetIndex;
	std::vector< std::string > m_vecElementName;

	// 템플릿의 크기는 특별하게 사용되는게 아니라,
	// 레이아웃뷰에서 컨트롤을 추가한다거나 할때 기본 템플릿 크기로 생성되게 해준다는 정도의 일만 한다.
	// 즉, 렌더링할때 늘어나보이지 말라고, 영역을 분할한다는 등의 작업과는 관련없다.
	float m_fDefaultWidth;
	float m_fDefaultHeight;

	SUIControlInfo m_ControlInfo;
	CEtUITemplate m_Template;

	CUIToolTemplate *m_pExternControlTemplate;
	std::string m_szExternalControlName;

	bool m_bChanged;

public:
	UI_CONTROL_TYPE UIType() { return m_UIType; }
	void SetUIType( UI_CONTROL_TYPE Type ) { m_UIType = Type; }

	void SetTemplateName( const char *pszName ) { m_Template.m_szTemplateName = pszName; }
	const char *GetTemplateName() { return m_Template.m_szTemplateName.c_str(); }

	void AddUIElement( const char *pTextureName, const int nFontSetIndex, const char *pElementName, SUIElement &Element );
	void DelUIElement( CString &strElementName );

	bool Load_01( CStream &Stream );
	bool Load_02( CStream &Stream );

	bool Load( CStream &Stream );
	bool Save( CStream &Stream );
};
