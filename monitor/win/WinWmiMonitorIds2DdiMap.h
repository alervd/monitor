#pragma once
#include "wmiutil.h"
class WinWmiMonitorIds2DdiMap
{
public:
	WinWmiMonitorIds2DdiMap (
		const std::vector <std::shared_ptr <WinWmiMonitorId>> &monitorIds,
		const std::shared_ptr <WinDisplayDevicesInfo> &ddis) :
		m_displayDevicesInfo (ddis)
	{
		build_map (monitorIds);
	}

public:
	struct map_entry
	{
		std::shared_ptr <WinDisplayDeviceInfo> adapterInfo;
		std::shared_ptr <WinDisplayDeviceInfo> monitorInfo;
		std::shared_ptr <WinWmiMonitorId> wmiInfo;
	};

	map_entry find_entry_by_wmi_instance_name (
		const std::wstring &wmi_instance_name) const
	{
		auto it = m_map.find (wmi_instance_name);
		if (it == m_map.end ())
			return {};
		return it->second;
	}

	map_entry find_entry_by_monitor_uid (
		const std::wstring &id) const
	{
		auto it = m_monitorUID2WmiInstanceName.find (id);
		if (it == m_monitorUID2WmiInstanceName.end ())
			return {};
		return it->second;
	}

protected:
	std::shared_ptr <WinDisplayDevicesInfo> m_displayDevicesInfo;
	// WMI instance name --> entry
	std::unordered_map <std::wstring, map_entry> m_map;
	// monitor info UID --> entry
	std::unordered_map <std::wstring, map_entry> m_monitorUID2WmiInstanceName;

protected:
	void build_map (
		const std::vector <std::shared_ptr <WinWmiMonitorId>> &monitorIds)
	{
		for (auto &&id : monitorIds)
		{
			auto e = build_entry (id);
			m_map [id->m_instanceName] = e;
			if (e.monitorInfo && !e.monitorInfo->m_deviceUID.empty ())
				m_monitorUID2WmiInstanceName [e.monitorInfo->m_deviceUID] = e;
		}
	}

	map_entry build_entry (
		const std::shared_ptr <WinWmiMonitorId> &id)
	{
		assert (!id->m_manufacturerName.empty () &&
			!id->m_productCode.empty ());

		if (id->m_manufacturerName.empty () ||
			id->m_productCode.empty ())
		{
			return {};
		}

		std::wstring str_mm = id->m_manufacturerName + 
			id->m_productCode;

		std::wstring str_uid;
		auto pos = id->m_instanceName.find (L"&UID");
		assert (pos != std::wstring::npos);
		if (pos == std::wstring::npos)
			return {};
		str_uid.assign (id->m_instanceName.begin () + pos, 
			id->m_instanceName.begin () + pos + 4);
		pos += 4;
		while (pos < id->m_instanceName.length () &&
			iswalnum (id->m_instanceName [pos]))
		{
			str_uid += id->m_instanceName [pos++];
		}

		auto &&adapters = m_displayDevicesInfo->adapters ();

		for (auto &&adapter : adapters)
		{
			auto &&monitors = m_displayDevicesInfo->adapter_monitors (adapter);

			for (auto &&monitor : monitors)
			{
				auto winmon = std::dynamic_pointer_cast <WinDisplayDeviceInfo> (monitor);
				assert (winmon);
				if (!winmon)
					continue;
				assert (!winmon->m_deviceUID.empty ());
				if (winmon->m_deviceUID.empty ())
					continue;
				auto pos = winmon->m_deviceUID.find (str_uid.c_str ());
				if (pos == std::wstring::npos)
					continue;
				if (iswalnum (winmon->m_deviceUID [pos])) // make sure it's not a different UID
					continue;
				pos = winmon->m_deviceUID.find (str_mm.c_str ());
				if (pos == std::wstring::npos)
					continue;
				// make sure it's not a different product id
				if ((pos && iswalnum (winmon->m_deviceUID [pos-1])) ||
					iswalnum (winmon->m_deviceUID [pos + str_mm.length ()]))
				{
					continue;
				}
				//  it seems we found it
				map_entry e;
				e.adapterInfo = std::dynamic_pointer_cast <WinDisplayDeviceInfo> (adapter);
				e.monitorInfo = std::dynamic_pointer_cast <WinDisplayDeviceInfo> (monitor);
				e.wmiInfo = id;
				return e;
			}
		}

		return {};
	}
};