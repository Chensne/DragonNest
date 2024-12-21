#pragma once


template<class T>
class CUnknownRepository
{
public:
	CUnknownRepository()
		: m_Unknown(NULL)
		, m_bModify(false)
	{
	}

	void SetModify( bool bModify ) { m_bModify = bModify; }

	void SetValue( T UnknownValue ) { m_Unknown = UnknownValue; }
	T GetValue() { return m_Unknown; }

protected:
	T m_Unknown;
	bool m_bModify;
};