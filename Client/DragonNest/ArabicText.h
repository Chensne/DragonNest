#pragma once
class ArabicText : public CSingleton<ArabicText>
{
private:
	bool containsArabic;
	bool needFixing;
public:
	ArabicText(void);
	~ArabicText(void);
	bool isArabic(std::wstring &text);
	void FixForm2(std::wstring &text);
	void Fix(std::wstring &text);
};

