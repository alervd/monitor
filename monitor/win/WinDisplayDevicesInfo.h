#pragma once
#include "../DisplayDevicesInfo.h"
class WinDisplayDevicesInfo :
	public DisplayDevicesInfo
{
public:
	WinDisplayDevicesInfo ()
	{
		verify (retrieve ());
	}

	virtual const std::vector <std::shared_ptr <DisplayDeviceInfo>>&
		adapters () const override
	{
		return m_adapters;
	}

	virtual const std::vector <std::shared_ptr <DisplayDeviceInfo>>&
		monitors () const override
	{
		return m_monitors;
	}

	virtual const std::vector <std::shared_ptr <DisplayDeviceInfo>>&
		adapter_monitors (const std::shared_ptr <DisplayDeviceInfo> &adapter) const override
	{
		auto it = m_adaptersMonitors.find (adapter);
		make_sure (it != m_adaptersMonitors.end ());
		return *it->second;
	}

protected:
	std::vector <std::shared_ptr <DisplayDeviceInfo>> m_adapters;
	std::vector <std::shared_ptr <DisplayDeviceInfo>> m_monitors;
	std::unordered_map <std::shared_ptr <DisplayDeviceInfo>,
		std::unique_ptr <std::vector <std::shared_ptr <DisplayDeviceInfo>>>> m_adaptersMonitors;

protected:
	bool retrieve ()
	{
		for (DWORD adapterIndex = 0; ; ++adapterIndex)
		{
			DISPLAY_DEVICEW adapter;
			ZeroMemory (&adapter, sizeof (DISPLAY_DEVICEW));
			adapter.cb = sizeof (DISPLAY_DEVICEW);

			if (!EnumDisplayDevicesW (nullptr, adapterIndex, &adapter, 0))
				break;

			if (!(adapter.StateFlags & DISPLAY_DEVICE_ACTIVE))
				continue;

			auto adapterInfo = std::make_shared <WinDisplayDeviceInfo> ();
			adapterInfo->read_from (adapter);
			m_adapters.push_back (adapterInfo);

			auto itAdapterMonitors = m_adaptersMonitors.insert (std::make_pair (
				adapterInfo, std::make_unique <std::vector <std::shared_ptr <DisplayDeviceInfo>>> ())).first;
			//auto itAdapterMonitors = m_adaptersMonitors.find (adapterInfo);

			for (DWORD displayIndex = 0; ; ++displayIndex)
			{
				DISPLAY_DEVICEW display, display2;
				ZeroMemory (&display, sizeof (DISPLAY_DEVICEW));
				display.cb = sizeof (DISPLAY_DEVICEW);
				ZeroMemory (&display2, sizeof (DISPLAY_DEVICEW));
				display2.cb = sizeof (DISPLAY_DEVICEW);

				if (!EnumDisplayDevicesW (adapter.DeviceName, displayIndex, &display, EDD_GET_DEVICE_INTERFACE_NAME) ||
					!EnumDisplayDevicesW (adapter.DeviceName, displayIndex, &display2, 0))
				{
					break;
				}

				if (!*display.DeviceString)
					continue;

				/*HDC dc = CreateDCW (L"DISPLAY", adapter.DeviceName, nullptr, nullptr);
				DeleteDC (dc);*/

				/*if (!(display.StateFlags & DISPLAY_DEVICE_ACTIVE))
					continue;*/

				auto monitorInfo = std::make_shared <WinDisplayDeviceInfo> ();
				monitorInfo->read_from (display, EDD_GET_DEVICE_INTERFACE_NAME);
				assert (monitorInfo->m_deviceID.empty ());
				monitorInfo->m_deviceID = display2.DeviceID;

				m_monitors.push_back (monitorInfo);
				itAdapterMonitors->second->push_back (monitorInfo);
			}
		}

		return !m_adapters.empty () && !m_monitors.empty ();
	}
};