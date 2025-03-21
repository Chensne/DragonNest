#pragma once

#include "DnCustomDlg.h"

class CDnSkillMovieDlg : public CDnCustomDlg
{
public:
	CDnSkillMovieDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnSkillMovieDlg() {}

	void Initialize(bool bShow);
	void Show(bool bShow);
	void Play(const std::wstring& skillName, const std::string& fileName);
	void Stop();

protected:
	void InitialUpdate();
	void InitCustomControl( CEtUIControl *pControl );
	void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */);
	void Process(float fElapsedTime);
	void Render( float fElapsedTime );

private:
	CDnMovieControl*		m_pMovieCtrl;
	CEtUIStatic*			m_pSkillName;
	CEtUIStatic*			m_pNoMovieStatic;
	CEtUIButton*			m_pClose;
};