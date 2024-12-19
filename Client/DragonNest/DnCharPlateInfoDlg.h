#include "DnCustomDlg.h"
#include "DnGlyph.h"

using namespace std;

class CDnCharPlateInfoDlg : public CDnCustomDlg
{
public :
	enum{
		FLOAT_TYPE,
		DOUBLE_FLOAT_TYPE,
		INT_TYPE,
		DOUBLE_INT_TYPE
	};

	CDnCharPlateInfoDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCharPlateInfoDlg(void);

	virtual void SetText();
	void AddStateText( int nStringID, int eType, float fFirstData = 0.0f, float fSecondData = 0.0f );
	void SetItemStateText( CDnState *pState );

protected :
	CEtUITextBox * m_pTextBox;

	vector< pair<int, int> >					m_vIntTypeList;
	vector< pair<int, pair<int, int> > >		m_vDoubleIntTypeList;
	vector< pair<int, float> >					m_vFloatTypeList;
	vector< pair<int, pair<float, float> > >	m_vDoubleFloatTypeList;

public :
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );

protected:
	void SetEnchatInfo();
	void SetSkillInfo();
	void SetSpecialSkillInfo();

	void AddDescription( int nTableID );
};