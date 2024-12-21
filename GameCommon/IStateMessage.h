#pragma once


// Note: 한기 - 특정 state 에서 Entity 에게 보내주는 메시지의 인터페이스 선언
class IStateMessage
{
public:
	enum
	{
		SCAN_ACTOR,
		MESSAGE_COUNT,
	};

protected:
	int m_iType;

public:
	IStateMessage(void);
	virtual ~IStateMessage(void);
};
