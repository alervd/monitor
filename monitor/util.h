#pragma once
#include "MonitorControl.h"
#include "defs.h"

inline bool PhysicalMonitorSliderValueChangable (
	const MonitorControl &monitor, monitor_slider_value value)
{
	switch (value)
	{
	case monitor_slider_value::brightness:
		return monitor.capability_supported (MonitorControl::capability::brightness);

	case monitor_slider_value::contrast:
		return monitor.capability_supported (MonitorControl::capability::contrast);

	case monitor_slider_value::red:
	case monitor_slider_value::green:
	case monitor_slider_value::blue:
		return monitor.capability_supported (MonitorControl::capability::rgb_gain);

	default:
		assert (!"implemented");
		return false;
	}
}

inline bool PhysicalMonitorSetSliderValue (
	MonitorControl &monitor, monitor_slider_value valtype, float val)
{
	uint32_t m, c, mx;

	switch (valtype)
	{
	case monitor_slider_value::brightness:
		if (!monitor.get_brightness (m, c, mx))
			return false;
		return monitor.set_brightness (m + static_cast <uint32_t> ((mx - m) * val));

	case monitor_slider_value::contrast:
		if (!monitor.get_contrast (m, c, mx))
			return false;
		return monitor.set_contrast (m + static_cast <uint32_t> ((mx - m) * val));

	case monitor_slider_value::red:
		if (!monitor.get_rgb_gain (MonitorControl::rgb_part::R, m, c, mx))
			return false;
		return monitor.set_rgb_gain (MonitorControl::rgb_part::R, m + static_cast <uint32_t> ((mx - m) * val));

	case monitor_slider_value::green:
		if (!monitor.get_rgb_gain (MonitorControl::rgb_part::G, m, c, mx))
			return false;
		return monitor.set_rgb_gain (MonitorControl::rgb_part::G, m + static_cast <uint32_t> ((mx - m) * val));

	case monitor_slider_value::blue:
		if (!monitor.get_rgb_gain (MonitorControl::rgb_part::B, m, c, mx))
			return false;
		return monitor.set_rgb_gain (MonitorControl::rgb_part::B, m + static_cast <uint32_t> ((mx - m) * val));

	default:
		assert (!"implemented");
		return false;
	}
}

inline bool PhysicalMonitorGetSliderValue (
	const MonitorControl &monitor, monitor_slider_value valtype, float &result)
{
	uint32_t m, c, mx;

	switch (valtype)
	{
	case monitor_slider_value::brightness:
		if (!monitor.get_brightness (m, c, mx))
			return false;
		break;

	case monitor_slider_value::contrast:
		if (!monitor.get_contrast (m, c, mx))
			return false;
		break;

	case monitor_slider_value::red:
		if (!monitor.get_rgb_gain (MonitorControl::rgb_part::R, m, c, mx))
			return false;
		break;

	case monitor_slider_value::green:
		if (!monitor.get_rgb_gain (MonitorControl::rgb_part::G, m, c, mx))
			return false;
		break;

	case monitor_slider_value::blue:
		if (!monitor.get_rgb_gain (MonitorControl::rgb_part::B, m, c, mx))
			return false;
		break;

	default:
		assert (!"implemented");
		return false;
	}

	result = (float) (double (c - m) / double (mx - m));
	return true;
}

inline MonitorsManager::monitor_list_t::value_type FindMonitorByInstanceName (
	const MonitorsManager::monitor_list_t &monitors, const std::wstring &name)
{
	for (const auto &monitor : monitors)
	{
		if (monitor->monitorInfo ()->instance_name () == name)
			return monitor;
	}

	assert (!"expected");
	return MonitorsManager::monitor_list_t::value_type ();
}

inline void MonitorSliderValueForEach (
	std::function <void(monitor_slider_value)> fn)
{
	static const monitor_slider_value vals [] = {
		monitor_slider_value::brightness,
		monitor_slider_value::contrast,
		monitor_slider_value::red,
		monitor_slider_value::green,
		monitor_slider_value::blue
	};

	for (auto val : vals)
		fn (val);
}