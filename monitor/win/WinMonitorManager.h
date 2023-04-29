#pragma once
#include "../MonitorManager.h"
#include "WinDisplayDeviceInfo.h"
class WinMonitorManager :
	public MonitorManager
{
public:
	WinMonitorManager (
		std::shared_ptr <const DisplayDeviceInfo> adapterInfo,
		std::shared_ptr <const DisplayDeviceInfo> monitorInfo,
		std::shared_ptr <MonitorControl> monitorControl) :
		m_adapterInfo (adapterInfo),
		m_monitorInfo (monitorInfo),
		m_monitorControl (monitorControl)
	{
	}

	virtual std::shared_ptr <const DisplayDeviceInfo> monitorInfo () const override
	{
		return m_monitorInfo;
	}

	virtual std::shared_ptr <const DisplayDeviceInfo> adapterInfo () const override
	{
		return m_adapterInfo;
	}

	virtual std::shared_ptr <MonitorControl> monitorControl () const override
	{
		return m_monitorControl;
	}

public:
	std::shared_ptr <const DisplayDeviceInfo> m_adapterInfo;
	std::shared_ptr <const DisplayDeviceInfo> m_monitorInfo;
	std::shared_ptr <MonitorControl> m_monitorControl;
};