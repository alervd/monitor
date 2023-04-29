#pragma once
#include "WinMonitorsManager.h"
#include "WinGammaRampMonitorControl.h"
class WinGammaRampMonitorsManager :
	public WinMonitorsManager
{
protected:
	virtual bool collect_monitors () override
	{
		auto &&adapters = m_displayDevicesInfo->adapters ();

		for (auto &&adapter : adapters)
		{
			auto &&monitors = m_displayDevicesInfo->
				adapter_monitors (adapter);

			for (auto &&monitor : monitors)
				collect_monitor (adapter, monitor);
		}

		return true;
	}

	bool collect_monitor (
		std::shared_ptr <const DisplayDeviceInfo> adapter,
		std::shared_ptr <const DisplayDeviceInfo> monitor)
	{
		auto control = std::make_shared <WinGammaRampMonitorControl> (
			adapter->name ());
		if (!control->initialize ())
			return false;

		auto mgr = std::make_shared <WinMonitorManager> (
			adapter, monitor, control);

		m_monitors->push_back (mgr);

		return true;
	}
};