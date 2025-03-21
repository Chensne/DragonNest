#pragma once

class CDnRotateToggleButton : public CEtUIButton
{
public:
	struct SRotateToggleData
	{
		int elementIndex;
		std::wstring btnText;

		SRotateToggleData() : elementIndex(-1) {}
		void Reset() { elementIndex = -1; btnText.clear(); }
	};

	CDnRotateToggleButton(CEtUIDialog *pParent);
	virtual ~CDnRotateToggleButton(void);

	virtual void Initialize(SUIControlProperty* pProperty);

	bool RegisterRotate(int rotateStateIndex, int elementIndex, const WCHAR* pBtnText = NULL);
	void AllowRotate(bool bOK) { m_bCheckRotate = bOK; }
	void ResetState();
	void SetState(int rotateStateIndex, bool bRotate, bool bProcessCommand, bool bTriggerByUser);

protected:
	bool Advance(bool bTriggerByUser);
	virtual bool HandleMouse(UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam);
	virtual void Render(float fElapsedTime);

private:
	void RenderButtonSprite(SUIElement* pElement);

	int	m_AllElementCount;
	int m_ToggleElementCount;
	bool m_bCheckRotate;

	std::map<int, SRotateToggleData> m_ElementMap; // key : stateIndex
	int m_CurrentRotateStateIndex;
	SRotateToggleData m_CurrentRotateStateData;
};
