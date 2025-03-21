#pragma once

// XTream ToolKit 커스텀 Property base Class
// 변경사항 적용 시 체크를 위해.

class CXTCustomPropertyGridItem : public CXTPPropertyGridItem
{
public:
	CXTCustomPropertyGridItem( const CString& strCaption, LPCTSTR strValue = NULL, CString* pBindString = NULL );
	CXTCustomPropertyGridItem( UINT nID, LPCTSTR strValue = NULL, CString* pBindString = NULL );

protected:
	virtual void OnInplaceButtonDown();
};

