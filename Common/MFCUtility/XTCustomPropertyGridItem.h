#pragma once

// XTream ToolKit Ŀ���� Property base Class
// ������� ���� �� üũ�� ����.

class CXTCustomPropertyGridItem : public CXTPPropertyGridItem
{
public:
	CXTCustomPropertyGridItem( const CString& strCaption, LPCTSTR strValue = NULL, CString* pBindString = NULL );
	CXTCustomPropertyGridItem( UINT nID, LPCTSTR strValue = NULL, CString* pBindString = NULL );

protected:
	virtual void OnInplaceButtonDown();
};

