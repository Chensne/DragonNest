#pragma once

#ifdef PRE_MOD_INTEG_SYSTEM_STATE

template <typename T>
class CDnAcceptRequestDataManager
{
public:
	CDnAcceptRequestDataManager() : INVALID_TIME_VALUE(-1.f), m_bLockProcess(false) {}
	virtual ~CDnAcceptRequestDataManager() {}

	void AddAcceptRequestInfo(const T& data);
	void PopAcceptRequestInfo();
	bool Process(float fElapsedTime);
	void ClearList();
	void TerminateList();

	float GetShortestAcceptTime() const;
	const T& GetCurrentInfo() const;

	bool IsListEmpty() const { return m_RequestInfoList.empty(); }
	void LockProcess(bool bLock) { m_bLockProcess = bLock; }

	const float INVALID_TIME_VALUE;
private:
	std::list<T> m_RequestInfoList;
	T m_NullObj;
	bool m_bLockProcess;
};

template <typename T>
void CDnAcceptRequestDataManager<T>::AddAcceptRequestInfo(const T& data)
{
	m_RequestInfoList.push_back(data);
}
template <typename T>
void CDnAcceptRequestDataManager<T>::PopAcceptRequestInfo()
{
	if (m_RequestInfoList.empty() == false)
		m_RequestInfoList.pop_front();
}

template <typename T>
bool CDnAcceptRequestDataManager<T>::Process(float fElapsedTime)
{
	if (m_bLockProcess)
		return true;

	std::list<T>::iterator askIter = m_RequestInfoList.begin();
	for (; askIter != m_RequestInfoList.end();)
	{
#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
		T& info = *askIter;
		++askIter;

		info.OnProcess(fElapsedTime);

		if (m_RequestInfoList.empty() || askIter == m_RequestInfoList.end())
			break;
#else
		T& info = *askIter;
		info.OnProcess(fElapsedTime);

		if (m_RequestInfoList.empty())
			break;

		++askIter;
#endif
	}

	return true;
}

template <typename T>
void CDnAcceptRequestDataManager<T>::ClearList()
{
	std::list<T>::iterator askIter = m_RequestInfoList.begin();
	for(; askIter != m_RequestInfoList.end(); ++askIter)
	{
		T& info = *askIter;
		info.OnClear();
	}

	m_RequestInfoList.clear();
}

template <typename T>
void CDnAcceptRequestDataManager<T>::TerminateList()
{
	std::list<T>::iterator askIter = m_RequestInfoList.begin();
	for(; askIter != m_RequestInfoList.end(); ++askIter)
	{
		T& info = *askIter;
		info.OnTerminate();
	}
}


template <typename T>
float CDnAcceptRequestDataManager<T>::GetShortestAcceptTime() const
{
	if (m_RequestInfoList.empty() == false)
	{
		std::list<T>::const_iterator iter = m_RequestInfoList.begin();
		const T& info = *iter;
		return info.m_fAcceptTime;
	}

	return INVALID_TIME_VALUE;
}

template <typename T>
const T& CDnAcceptRequestDataManager<T>::GetCurrentInfo() const
{
	if (m_RequestInfoList.empty() == false)
	{
		std::list<T>::const_iterator iter = m_RequestInfoList.begin();
		return (*iter);
	}

	return m_NullObj;
}

#endif // PRE_MOD_INTEG_SYSTEM_STATE