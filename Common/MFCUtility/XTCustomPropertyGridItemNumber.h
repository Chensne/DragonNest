#pragma once

// XTPPropertyGridItemNumber 변형. XTCustomPropertyGridItem 처럼 InplaceButtonDown 위해 커스텀으로 추가.

class CXTCustomPropertyGridItemNumber : public CXTPPropertyGridItemNumber
{
public:
	CXTCustomPropertyGridItemNumber(const CString& strCaption, long nValue = 0, long* pBindNumber = NULL );
	CXTCustomPropertyGridItemNumber(UINT nID, long nValue = 0, long* pBindNumber = NULL );

protected:
	virtual void OnInplaceButtonDown();

	DECLARE_DYNAMIC(CXTCustomPropertyGridItemNumber)
};
