#pragma once

class CEtUINameLinkInterface
{
public:
	virtual bool OnParseTextColor(DWORD color)	{ return false; }
	virtual bool OnParseTextFailColor()			{ return false; }
	virtual bool OnParseTextRestoreColor()		{ return false; }
	virtual bool OnParseTextItemInfo(const std::wstring& argString)		{ return false; }
	virtual bool OnParseTextUserName(const std::wstring& name)		{ return false; }
	virtual bool OnParseTextSkillInfo(const std::wstring& argString)		{ return false; }
	virtual void OnOpenHyperLink(const std::wstring& argString)	{}
};