#pragma once
#include "../defs.h"
#include "WinMonitorsManager.h"
#include "WinDdcCiMonitorControl.h"
class WinDdcCiMonitorsManager :
	public WinMonitorsManager
{
public:
	WinDdcCiMonitorsManager ()
	{
	}

protected:
	virtual bool collect_monitors () override
	{
		return FALSE != EnumDisplayMonitors (
			nullptr, nullptr, MonitorEnumProc, (LPARAM)this);
	}

	static BOOL CALLBACK MonitorEnumProc (
		HMONITOR hMonitor, HDC hdcMonitor,
		LPRECT lprcMonitor, LPARAM dwData)
	{
		auto *pthis = reinterpret_cast <WinDdcCiMonitorsManager*> (dwData);

		DWORD cMons = 0;
		if (!GetNumberOfPhysicalMonitorsFromHMONITOR (hMonitor, &cMons))
			return TRUE;

		std::vector <PHYSICAL_MONITOR> ph_monitors (cMons);

		if (GetPhysicalMonitorsFromHMONITOR (hMonitor, cMons, &ph_monitors.front ()))
		{
			MONITORINFOEX miAdapter;
			miAdapter.cbSize = sizeof (miAdapter);
			verify (GetMonitorInfo (hMonitor, &miAdapter));

			std::shared_ptr <WinDisplayDeviceInfo> adapterInfo;

			for (const auto& ph_monitor : ph_monitors)
			{
				CL_MONITOR_LOG (L"Found monitor: " << ph_monitor.szPhysicalMonitorDescription <<
					L", handle = " << ph_monitor.hPhysicalMonitor << PFL_NL);

				std::shared_ptr <WinDisplayDeviceInfo> monitorInfo;

				if (!pthis->find_monitor_info (ph_monitor, 
					miAdapter, adapterInfo, monitorInfo))
				{
					assert (!"expected");
					continue;
				}

				auto controller = std::make_shared <WinDdcCiMonitorControl> (
					ph_monitor.hPhysicalMonitor);

				auto mm = std::make_shared <WinMonitorManager> (
					adapterInfo, monitorInfo, controller);

				pthis->m_monitors->push_back (mm);
			}
		}

		return TRUE;
	}

	bool find_monitor_info (
		const PHYSICAL_MONITOR &ph_monitor,
		const MONITORINFOEX &miAdapter,
		std::shared_ptr <WinDisplayDeviceInfo> &adapterInfo,
		std::shared_ptr <WinDisplayDeviceInfo> &monitorInfo)
	{
		if (!adapterInfo)
		{
			auto &&adapters = m_displayDevicesInfo->adapters ();
			for (auto &&adapter : adapters)
			{
				if (adapter->name () != miAdapter.szDevice)
					continue;
				auto &&monitors = m_displayDevicesInfo->adapter_monitors (adapter);
				for (auto &&monitor : monitors)
				{
					if (monitor->display_name () ==
						ph_monitor.szPhysicalMonitorDescription)
					{
						monitorInfo = std::dynamic_pointer_cast <WinDisplayDeviceInfo> (monitor);
						adapterInfo = std::dynamic_pointer_cast <WinDisplayDeviceInfo> (adapter);
						return true;
					}
				}
			}
		}
		else
		{
			auto &&monitors = m_displayDevicesInfo->adapter_monitors (adapterInfo);
			for (auto &&monitor : monitors)
			{
				if (monitor->display_name () ==
					ph_monitor.szPhysicalMonitorDescription)
				{
					monitorInfo = std::dynamic_pointer_cast <WinDisplayDeviceInfo> (monitor);
					return true;
				}
			}

			assert (!"expected");
			// try to search among all adapters
			adapterInfo.reset ();
			return find_monitor_info (
				ph_monitor, miAdapter, adapterInfo, monitorInfo);
		}

		monitorInfo.reset ();
		return false;
	}
};