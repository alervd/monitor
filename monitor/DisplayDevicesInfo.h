#pragma once
#include "DisplayDeviceInfo.h"
class DisplayDevicesInfo
{
public:
	virtual const std::vector <std::shared_ptr <DisplayDeviceInfo>>& 
		adapters () const = 0;

	virtual const std::vector <std::shared_ptr <DisplayDeviceInfo>>&
		monitors () const = 0;

	virtual const std::vector <std::shared_ptr <DisplayDeviceInfo>>&
		adapter_monitors (const std::shared_ptr <DisplayDeviceInfo> &adapter) const = 0;

	virtual ~DisplayDevicesInfo () {}
};