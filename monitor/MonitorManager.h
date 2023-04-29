#pragma once
#include "DisplayDeviceInfo.h"
#include "MonitorControl.h"
class MonitorManager
{
public:
	virtual std::shared_ptr <const DisplayDeviceInfo> monitorInfo () const = 0;
	virtual std::shared_ptr <const DisplayDeviceInfo> adapterInfo () const = 0;
	virtual std::shared_ptr <MonitorControl> monitorControl () const = 0;
	virtual ~MonitorManager () {}
};