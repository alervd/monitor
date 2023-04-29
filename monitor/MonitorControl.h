#pragma once
class MonitorControl
{
public:
	// a way in which monitor control performs operations
	enum class kind 
	{
		unknown,
		display,		// perform via commands to display (e.g. via DDC/CI)
		video_card		// manipulating by video memory
	};

	enum class capability
	{
		brightness, 
		contrast, 
		color_temperature,
		rgb_gain,
		rgb_drive,
		max_index = rgb_drive
	};

	enum class color_temperature
	{
		temperature_4000K, 
		temperature_5000K, 
		temperature_6500K,
		temperature_7500K,
		temperature_8200K,
		temperature_9300K,
		temperature_10000K,
		temperature_11500K
	};

	enum class rgb_part	{R, G, B};

public:
	virtual xstring control_uid () const = 0;
	virtual xstring control_uid (capability cap) const = 0;

	virtual kind control_kind () const = 0;
	virtual kind control_kind (capability cap) const = 0;
	virtual void preferred_control_kind (kind value) = 0;

	virtual bool capability_supported (capability cap) const = 0;
	virtual bool color_temperature_supported (color_temperature temp) const = 0;

	virtual bool get_brightness (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const = 0;
	virtual bool set_brightness (uint32_t value) = 0;

	virtual bool get_contrast (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const = 0;
	virtual bool set_contrast (uint32_t value) = 0;

	virtual bool get_color_temperature (color_temperature& result) const = 0;
	virtual bool set_color_temperature (color_temperature value) = 0;

	virtual bool get_rgb_gain (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const = 0;
	virtual bool set_rgb_gain (rgb_part what, uint32_t value) = 0;

	virtual bool get_rgb_drive (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const = 0;
	virtual bool set_rgb_drive (rgb_part what, uint32_t value) = 0;

	enum state_type 
	{
		control_only, 
		control_and_monitor
	};

	virtual std::string get_current_state (state_type) = 0;
	virtual bool set_current_state (state_type, const std::string&) = 0;

	virtual ~MonitorControl () {}
};