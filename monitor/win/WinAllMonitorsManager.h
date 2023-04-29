#pragma once
#include "WinDdcCiMonitorsManager.h"
#include "WinWmiMonitorsManager.h"
#include "../MonitorMultiControl.h"
#include "WinDisplayDeviceMultiInfo.h"
#include "WinGammaRampMonitorsManager.h"
class WinAllMonitorsManager :
	public WinMonitorsManager
{
public:
	WinAllMonitorsManager (
		MonitorControl::kind preferredControlKind = MonitorControl::kind::display) :
		m_preferredControlKind (preferredControlKind)
	{
	}

protected:
	MonitorControl::kind m_preferredControlKind;
	std::vector <std::shared_ptr <WinMonitorsManager>> m_managers;
	std::weak_ptr <WinWmiMonitorsManager> m_wmiMonitorsManager;

protected:
	virtual bool collect_monitors () override
	{
		create_managers ();

		for (auto &&manager : m_managers)
			verify (manager->refresh_monitor_list (m_displayDevicesInfo));

		auto wmiMonitorsManager = m_wmiMonitorsManager.lock ();
		auto monitorsIdsMap = wmiMonitorsManager->monitorsIdsMap ();

		struct monitor_data
		{
			std::shared_ptr <const WinDisplayDeviceInfo> adapterInfo;
			std::vector <std::shared_ptr <MonitorControl>> controls;
		};

		std::unordered_map <
			std::shared_ptr <const WinDisplayDeviceInfo>,
			monitor_data> monitors_data;

		for (auto &&manager : m_managers)
		{
			auto &&monitors = manager->monitors ();
			assert (monitors);

			for (auto &&monitor : *monitors)
			{
				auto monInfo = std::dynamic_pointer_cast <const WinDisplayDeviceInfo> (
					monitor->monitorInfo ());
				if (!monInfo)
					continue;

				auto adapterInfo = std::dynamic_pointer_cast <const WinDisplayDeviceInfo> (
					monitor->adapterInfo ());

				auto monControl = monitor->monitorControl ();
				assert (monControl);
				if (!monControl)
					continue;

				auto itData = monitors_data.find (monInfo);
				if (itData == monitors_data.end ())
				{
					monitor_data d;
					d.adapterInfo = adapterInfo;
					d.controls.push_back (monControl);
					monitors_data.insert (std::make_pair (monInfo, d));
				}
				else
				{
					itData->second.controls.push_back (monControl);
					if (!itData->second.adapterInfo)
						itData->second.adapterInfo = adapterInfo;
				}
			}
		}

		for (auto &&mon_item : monitors_data)
		{
			auto monInfo = mon_item.first;

			auto eWmi = monitorsIdsMap->find_entry_by_monitor_uid (
				monInfo->m_deviceUID);

			auto monMultiInfo = std::make_shared <WinDisplayDeviceMultiInfo> (
				monInfo, eWmi.wmiInfo);

			auto monControl = std::make_shared <MonitorMultiControl> (
				mon_item.second.controls, m_preferredControlKind);

			auto monitor = std::make_shared <WinMonitorManager> (
				mon_item.second.adapterInfo, 
				monMultiInfo, 
				monControl);

			m_monitors->push_back (monitor);
		}

		return true;
	}

	void create_managers ()
	{
		m_managers.clear ();

		m_managers.push_back (
			std::make_shared <WinDdcCiMonitorsManager> ());

		auto mgr = std::make_shared <WinWmiMonitorsManager> ();
		m_wmiMonitorsManager = mgr;
		m_managers.push_back (mgr);

		m_managers.push_back (
			std::make_shared <WinGammaRampMonitorsManager> ());
	}
};