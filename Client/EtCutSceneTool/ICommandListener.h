#pragma once


class ICommand;

class ICommandListener
{
private:


public:
	ICommandListener(void);
	virtual ~ICommandListener(void);

	virtual void CommandPerformed( ICommand* pCommand ) = 0;
};
