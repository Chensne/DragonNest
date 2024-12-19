#pragma once

// XTPPropertyGridItemNumber ����. XTCustomPropertyGridItem ó�� InplaceButtonDown ���� Ŀ�������� �߰�.

class CXTCustomPropertyGridItemNumber : public CXTPPropertyGridItemNumber
{
public:
	CXTCustomPropertyGridItemNumber(const CString& strCaption, long nValue = 0, long* pBindNumber = NULL );
	CXTCustomPropertyGridItemNumber(UINT nID, long nValue = 0, long* pBindNumber = NULL );

protected:
	virtual void OnInplaceButtonDown();

	DECLARE_DYNAMIC(CXTCustomPropertyGridItemNumber)
};
