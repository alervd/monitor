#pragma once
#include "WinMonitorsManager.h"
#include "WinWmiMonitorControl.h"
#include "wmiutil.h"
#include "WinWmiMonitorIds2DdiMap.h"
class WinWmiMonitorsManager :
	public WinMonitorsManager
{
public:
	WinWmiMonitorsManager ()
	{
	}

	std::shared_ptr <WinWmiMonitorIds2DdiMap> monitorsIdsMap () const
	{
		return m_monitorIdsMap;
	}

protected:
	std::shared_ptr <WbemLocator> m_wbemLocator;
	std::shared_ptr <WbemServices> m_wbemServices;
	std::shared_ptr <WinWmiMonitorIds2DdiMap> m_monitorIdsMap;

protected:
	virtual bool collect_monitors () override
	{
		if (!m_wbemLocator || !m_wbemServices)
		{
			m_wbemLocator = std::make_shared <WbemLocator> ();
			m_wbemServices = m_wbemLocator->ConnectServer (L"root\\wmi");
			assert (m_wbemServices);
			if (!m_wbemServices)
				return false;
		}

		auto monitors_ids = collectWmiMonitorsIds (m_wbemServices.get ());
		m_monitorIdsMap = std::make_shared <WinWmiMonitorIds2DdiMap> (
			monitors_ids, m_displayDevicesInfo);
		
		for (auto &&mon_id : monitors_ids)
		{
			auto map_e = m_monitorIdsMap->find_entry_by_wmi_instance_name (
				mon_id->m_instanceName);
			assert (map_e.adapterInfo && map_e.monitorInfo);
			if (!map_e.adapterInfo || !map_e.monitorInfo)
				continue;

			auto wmi_mon = std::make_shared <WinWmiMonitorControl> (
				m_wbemLocator, m_wbemServices, mon_id->m_instanceName);

			if (wmi_mon->initialize ())
			{
				CL_MONITOR_LOG (L"WMI monitor found" << PFL_NL);

				auto mgr = std::make_shared <WinMonitorManager> (
					map_e.adapterInfo, map_e.monitorInfo, wmi_mon);

				m_monitors->push_back (mgr);
			}
		}

		return true;
	}
};