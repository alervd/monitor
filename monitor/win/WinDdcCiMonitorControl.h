#pragma once
#include "../defs.h"
#include "../MonitorControl.h"
#include <LowLevelMonitorConfigurationAPI.h>
#include "WinDisplayDeviceInfo.h"
#include "../MonitorControlGenericState.h"
class WinDdcCiMonitorControl :
	public MonitorControl
{
public:
	WinDdcCiMonitorControl (HANDLE monitor) :
		m_monitor (monitor)
	{
		query_capabilities ();
	}
	
	~WinDdcCiMonitorControl ()
	{
		verify (DestroyPhysicalMonitor (m_monitor));
	}

	virtual xstring control_uid () const override
	{
		return L"DDC/CI";
	}

	virtual xstring control_uid (capability cap) const override
	{
		return control_uid ();
	}

	virtual kind control_kind () const override
	{
		return kind::display;
	}

	virtual kind control_kind (capability cap) const override
	{
		return kind::display;
	}

	virtual void preferred_control_kind (kind value) override
	{
	}

	virtual bool capability_supported (capability cap) const override
	{
		return m_capabilities [(int)cap];
	}

	virtual bool color_temperature_supported (color_temperature temp) const override
	{
		return m_color_temperatures [(int)temp];
	}

	virtual bool get_brightness (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		assert (capability_supported (MonitorControl::capability::brightness) || m_noCompatibilityCheckAssert);
		DWORD m = 0, c = 0, mx = 0;
		if (!GetMonitorBrightness (m_monitor, &m, &c, &mx))
			return false;
		minimum = m;
		current = c;
		maximum = mx;
		return true;
	}

	virtual bool set_brightness (uint32_t value) override
	{
		assert (capability_supported (MonitorControl::capability::brightness));
		return SetMonitorBrightness (m_monitor, value) != FALSE;
	}

	virtual bool get_contrast (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		assert (capability_supported (MonitorControl::capability::contrast) || m_noCompatibilityCheckAssert);
		DWORD m = 0, c = 0, mx = 0;
		if (!GetMonitorContrast (m_monitor, &m, &c, &mx))
			return false;
		minimum = m;
		current = c;
		maximum = mx;
		return true;
	}

	virtual bool set_contrast (uint32_t value) override
	{
		assert (capability_supported (MonitorControl::capability::contrast));
		return SetMonitorContrast (m_monitor, value) != FALSE;
	}

	virtual bool get_color_temperature (color_temperature& result) const override
	{
		assert (capability_supported (MonitorControl::capability::color_temperature) || m_noCompatibilityCheckAssert);
		MC_COLOR_TEMPERATURE ct;
		if (!GetMonitorColorTemperature (m_monitor, &ct))
			return false;
		auto it = color_temperature_map ().find (ct);
		assert (!"implemented" || it != color_temperature_map ().end ());
		if (it == color_temperature_map ().end ())
			return false;
		result = it->second;
		return true;
	}

	virtual bool set_color_temperature (color_temperature value) override
	{
		assert (capability_supported (MonitorControl::capability::color_temperature));
		const auto& map = color_temperature_map ();
		for (const auto& temp : map)
		{
			if (temp.second == value)
				return SetMonitorColorTemperature (m_monitor, temp.first) != FALSE;
		}
		assert (!"implemented");
		return false;
	}

	virtual bool get_rgb_gain (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		assert (capability_supported (MonitorControl::capability::rgb_gain) || m_noCompatibilityCheckAssert);
		DWORD m = 0, c = 0, mx = 0;
		if (!GetMonitorRedGreenOrBlueGain (m_monitor, (MC_GAIN_TYPE)what, &m, &c, &mx))
			return false;
		minimum = m;
		current = c;
		maximum = mx;
		return true;
	}

	virtual bool set_rgb_gain (rgb_part what, uint32_t value) override
	{
		assert (capability_supported (MonitorControl::capability::rgb_gain));

		uint32_t m = 0, c = 0, mm = 0;
		if (!get_rgb_gain(what, m, c, mm))
			return false;
		if (value == c)
			return true;

		return SetMonitorRedGreenOrBlueGain (m_monitor, (MC_GAIN_TYPE)what, value) != FALSE;
	}

	virtual bool get_rgb_drive (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		assert (capability_supported (MonitorControl::capability::rgb_drive) || m_noCompatibilityCheckAssert);
		DWORD m = 0, c = 0, mx = 0;
		if (!GetMonitorRedGreenOrBlueDrive (m_monitor, (MC_DRIVE_TYPE)what, &m, &c, &mx))
			return false;
		minimum = m;
		current = c;
		maximum = mx;
		return true;
	}

	virtual bool set_rgb_drive (rgb_part what, uint32_t value) override
	{
		assert (capability_supported (MonitorControl::capability::rgb_drive));

		uint32_t m = 0, c = 0, mm = 0;
		if (!get_rgb_drive(what, m, c, mm))
			return false;
		if (value == c)
			return true;

		return SetMonitorRedGreenOrBlueDrive (m_monitor, (MC_DRIVE_TYPE)what, value) != FALSE;
	}

	virtual std::string get_current_state (state_type st) override
	{
		switch (st)
		{
		case MonitorControl::control_and_monitor:
		{
			MonitorControlGenericState s;
			s.read_from (this);
			return vmsJsonSerializeObject (s);
		}

		default:
			return "";
		}
	}

	virtual bool set_current_state (state_type st, const std::string& state) override
	{
		switch (st)
		{
		case MonitorControl::control_and_monitor:
		{
			MonitorControlGenericState s;
			if (!vmsJsonSerializeObject (s, state))
				return false;
			return s.apply_to (this);
		}

		default:
			return true;
		}
	}

protected:
	HANDLE m_monitor;
	std::vector <bool> m_capabilities;
	std::vector <bool> m_color_temperatures;
#ifdef _DEBUG
	bool m_noCompatibilityCheckAssert = false;
#endif

protected:
	void query_capabilities ()
	{
		CL_MONITOR_LOG (L"query capabilities of monitor " << description () << PFL_NL);

		/*{
			std::wstringstream wss;
			wss << L"capabilities:" << std::endl;
			DWORD c = 0, m = 0;
			if (GetVCPFeatureAndVCPFeatureReply (m_monitor, 0x72, nullptr, &c, &m))
				wss << L"got 0x72, current = " << c << L", max = " << m << std::endl;
			if (GetVCPFeatureAndVCPFeatureReply (m_monitor, 0x92, nullptr, &c, &m))
				wss << L"got 0x92, current = " << c << L", max = " << m << std::endl;
			if (GetVCPFeatureAndVCPFeatureReply (m_monitor, 0x8E, nullptr, &c, &m))
				wss << L"got 0x8E, current = " << c << L", max = " << m << std::endl;
			MessageBox (0, wss.str().c_str (), 0, MB_SETFOREGROUND);
		}*/

		m_capabilities.resize (64);
		m_color_temperatures.resize (64);

		DWORD caps = 0, temps = 0;
		if (!GetMonitorCapabilities (m_monitor, &caps, &temps))
		{
			CL_MONITOR_LOG (L" failed  (error: " << GetLastError () << L")" << PFL_NL);
			query_capabilities__workaround ();
			return;
		}

		CL_MONITOR_LOG (L"caps = " << caps << L" temps = " << temps << PFL_NL);

		static const std::pair <DWORD, capability> known_caps [] = {
			{MC_CAPS_BRIGHTNESS, capability::brightness},
			{MC_CAPS_COLOR_TEMPERATURE, capability::color_temperature},
			{MC_CAPS_CONTRAST, capability::contrast},
			{MC_CAPS_RED_GREEN_BLUE_GAIN, capability::rgb_gain},
			{MC_CAPS_RED_GREEN_BLUE_DRIVE, capability::rgb_drive}
		};

		for (const auto& cap : known_caps)
		{
			if (caps & cap.first)
				m_capabilities [(int)cap.second] = true;
		}

		static const auto known_temps = {
			std::make_pair (MC_SUPPORTED_COLOR_TEMPERATURE_4000K, color_temperature::temperature_4000K),
			std::make_pair (MC_SUPPORTED_COLOR_TEMPERATURE_5000K, color_temperature::temperature_5000K),
			std::make_pair (MC_SUPPORTED_COLOR_TEMPERATURE_6500K, color_temperature::temperature_6500K),
			std::make_pair (MC_SUPPORTED_COLOR_TEMPERATURE_7500K, color_temperature::temperature_7500K),
			std::make_pair (MC_SUPPORTED_COLOR_TEMPERATURE_8200K, color_temperature::temperature_8200K),
			std::make_pair (MC_SUPPORTED_COLOR_TEMPERATURE_9300K, color_temperature::temperature_9300K),
			std::make_pair (MC_SUPPORTED_COLOR_TEMPERATURE_10000K, color_temperature::temperature_10000K),
			std::make_pair (MC_SUPPORTED_COLOR_TEMPERATURE_11500K, color_temperature::temperature_11500K),
		};

		for (const auto& temp : known_temps)
		{
			if (temps & temp.first)
				m_color_temperatures [(int)temp.second] = true;
		}
	}

	void query_capabilities__workaround ()
	{
		// some monitors do not support GetMonitorCapabilities due to some unknown reason
		// thus this API fails.
		// we need to check capabilities by direct calling of the appropriate functions

#ifdef _DEBUG
		m_noCompatibilityCheckAssert = true;
#endif

		CL_MONITOR_LOG (L"query capabilities (WA) of monitor " << description () << PFL_NL);

		uint32_t m, mx, c;

		if (get_brightness (m, c, mx))
		{
			m_capabilities [(int)capability::brightness] = true;
			CL_MONITOR_LOG (L"brightness is supported" << PFL_NL);
		}

		if (get_contrast (m, c, mx))
		{
			m_capabilities [(int)capability::contrast] = true;
			CL_MONITOR_LOG (L"contrast is supported" << PFL_NL);
		}

		if (get_rgb_gain (MonitorControl::rgb_part::R, m, c, mx))
		{
			m_capabilities [(int)capability::rgb_gain] = true;
			CL_MONITOR_LOG (L"rgb gain is supported" << PFL_NL);
		}

		if (get_rgb_drive (MonitorControl::rgb_part::R, m, c, mx))
		{
			m_capabilities [(int)capability::rgb_drive] = true;
			CL_MONITOR_LOG (L"rgb drive is supported" << PFL_NL);
		}

		color_temperature ct;
		if (get_color_temperature (ct))
		{
			m_capabilities [(int)capability::color_temperature] = true;
			CL_MONITOR_LOG (L"color temperature is supported" << PFL_NL);
		}

#ifdef _DEBUG
		m_noCompatibilityCheckAssert = false;
#endif
	}

protected:
	static const std::map <MC_COLOR_TEMPERATURE, color_temperature>& color_temperature_map ()
	{
		static std::map <MC_COLOR_TEMPERATURE, color_temperature> map = {
			{MC_COLOR_TEMPERATURE_4000K, color_temperature::temperature_4000K},
			{MC_COLOR_TEMPERATURE_5000K, color_temperature::temperature_5000K},
			{MC_COLOR_TEMPERATURE_6500K, color_temperature::temperature_6500K},
			{MC_COLOR_TEMPERATURE_7500K, color_temperature::temperature_7500K},
			{MC_COLOR_TEMPERATURE_8200K, color_temperature::temperature_8200K},
			{MC_COLOR_TEMPERATURE_9300K, color_temperature::temperature_9300K},
			{MC_COLOR_TEMPERATURE_10000K, color_temperature::temperature_10000K},
			{MC_COLOR_TEMPERATURE_11500K, color_temperature::temperature_11500K}
		};
		return map;
	}

};