#pragma once

class CLostDeviceProcess
{
public:
	CLostDeviceProcess(void);
	virtual ~CLostDeviceProcess(void);

protected:
	static std::vector< CLostDeviceProcess * > s_vecLostDevice;
	static CSyncLock s_SyncLock;

public:
	virtual void OnLostDevice() {}
	virtual void OnResetDevice() {}

	static void OnLostDeviceList();
	static void OnResetDeviceList();
};
