#pragma once

class DnScriptManager;
class CDNTalk
{
public:

	CDNTalk();
	virtual ~CDNTalk();

	static void SetScriptManager(DnScriptManager* pScriptManager);
	DnScriptManager* GetScriptManager() { return m_pScriptManager; }

	bool Create(const WCHAR* wszTalkFileName, const WCHAR* szScriptFileName);
	void Destroy();

	virtual bool CheckTalk(UINT nUserObjectID, IN std::wstring& wszIndex, IN std::wstring& wszTarget);
	virtual bool OnTalk(UINT nUserObjectID, UINT nNpcUniqueID, IN std::wstring& wszIndex, IN std::wstring& wszTarget);
	const std::wstring& GetTalkFileName() { return m_wszTalkFileName; }

protected:

	static DnScriptManager*	m_pScriptManager;
	TALK_PARAGRAPH_MAP	m_TalkParagraphMap;
	std::wstring		m_wszTalkFileName;
	std::wstring		m_wszScriptFilename;
};
