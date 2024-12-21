#pragma once
#include <wx/PropGrid/PropGrid.h>

class ICommand;


// 프로퍼티 대리자 인터페이스
class IPropertyDelegate
{
protected:
	wxPropertyGrid*			m_pPropGrid;

	IPropertyDelegate( void );

public:
	IPropertyDelegate( wxWindow* pParent, int id );
	virtual ~IPropertyDelegate(void);

	wxPropertyGrid* GetPropertyGrid( void ) { return m_pPropGrid; };
	void Show( bool bShow );

	virtual void Initialize( void ) = 0;
	virtual void OnShow( void ) = 0;
	virtual void OnPropertyChanged( wxPropertyGridEvent& PGEvent ) = 0;
	virtual void ClearTempData( void ) = 0;
	virtual void CommandPerformed( ICommand* pCommand ) = 0;
};
