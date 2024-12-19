#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

#include <string>
#include <map>

template <typename T>
inline T ConvertTrait(const std::string & value)
{
	return value;
}

template <>
inline int ConvertTrait<int>(const std::string & value)
{
	return atoi(value.c_str());
}

template <>
inline long ConvertTrait<long>(const std::string & value)
{
	return atol(value.c_str());
}

template <>
inline unsigned int ConvertTrait<unsigned int>(const std::string & value)
{
	return atoi(value.c_str());
}

template <>
inline unsigned long ConvertTrait<unsigned long>(const std::string & value)
{
	return atol(value.c_str());
}

template <>
inline float ConvertTrait<float>(const std::string & value)
{
	return (float)atof(value.c_str());
}

template <>
inline double ConvertTrait<double>(const std::string & value)
{
	return atof(value.c_str());
}

/**
* @brief 一个Key-Value的配置信息类,可用于读取Key-Value文件(.ini),
* 也可以将内存中的Key-Value信息存储到文件中并且,此类可以添加,删除,
* 读取Key-Value信息.另外,这个配置信息类,由于使用了模板,支持将文件中的内容,
* 读取为int,long,float,double等各种类型.
*
*/
class CProperties
{
public:
	CProperties();
	~CProperties();
	CProperties(const CProperties & rhs);
	CProperties & operator = (const CProperties & rhs);
	
	/**
	* @brief 将另一个配置信息类添加到本配置信息类中，如果有Key雷同，新的将覆盖旧的
	* @param pAnother : 被添加的配置信息类
	*/
	void AddProperties(const CProperties * pAnother);
	
	/**
	* @brief 导入文本配置信息
	* @param pFilePath : 导入的文件路径
	* @return 返回导入的Key-Value数量
	*/
	int ImportProperties(const char * pFilePath);
	
	/**
	* @brief 导出配置信息到文本
	* @param pFilePath : 导出的文件路径
	* @return 是否导出成功
	*/
	bool ExportProperties(const char * pFilePath);
	
	/**
	* @brief 添加一对配置信息到此配置信息类中
	* @param Key : 添加的Key
	* @param Value : 添加的Value
	*/
	void AddProperty(const std::string & Key, const std::string & Value);
	
	/**
	* @brief 删除一对配置信息
	* @param Key : 删除的Key
	* @return 是否删除成功
	*/
	bool RemoveProperty(const std::string & Key);
	
	/**
	* @brief 获取字符串类型的配置信息
	* @param Key : Key值
	* @param pValueBuffer : Value存放的Buffer
	* @param Length : Buffer的长度
	* @return 获取的Value值，此值是存放在pValueBuffer指向的空间中的，返回失败，返回NULL
	*/
	char * GetProperty(const std::string & Key, char * pValueBuffer, unsigned int Length);
	
	/**
	* @brief 获取任何种类的配置信息
	* @param Key : Key值
	* @param Def : Value的缺省值
	* @return 返回Value,Value不存在,返回Def
	*/
	template <typename T>
	inline T GetProperty(const std::string & Key, const T & Def = T())
	{
		std::string value = _GetProperty(Key);
		return value.empty() ? Def : ConvertTrait<T>(value);
	}
	
	/**
	* @brief 目前配置信息类中有多少个Key-Value配置信息对
	* @return 返回的Value
	*/
	int Size();

	/**
	* @brief 获取配置信息类中所有配置信息的string
	* @return 获取配置信息类的所有配置信息的string
	*/
	std::string toString();

private:
	std::string _GetProperty(const std::string & Key);

private:
	typedef std::map<std::string, std::string> PropertiesMap;
	typedef PropertiesMap::iterator PropertiesMapItr;
	PropertiesMap * pProperties;
};
#endif
