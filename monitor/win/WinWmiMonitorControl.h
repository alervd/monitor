#pragma once
#include "../MonitorControl.h"
#include "../../win/wmi/WbemLocator.h"
#include "WinWmiMonitorId.h"
#include "../MonitorControlGenericState.h"
class WinWmiMonitorControl :
	public MonitorControl
{
public:
	WinWmiMonitorControl (
		std::shared_ptr <WbemLocator> wbemLocator,
		std::shared_ptr <WbemServices> wbemServices,
		const std::wstring &wmi_instance_name) :
		m_wbemLocator (wbemLocator),
		m_wbemServices (wbemServices),
		m_wmi_instance_name (wmi_instance_name)
	{
	}

	bool initialize ()
	{
		{
			update_brightness_getter ();
			if (!m_brightnessGet)
				return false;
		}

		{
			auto objects = m_wbemServices->Objects (L"WmiMonitorBrightnessMethods");
			m_brightnessSet = find_object_by_instance_name (
				objects, m_wmi_instance_name);
			if (!m_brightnessSet)
				return false;
		}

		return true;
	}

	std::wstring instanceName () const
	{
		return m_wmi_instance_name;
	}

public:
	virtual xstring control_uid () const override
	{
		return L"WMI";
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
		return (cap == MonitorControl::capability::brightness) &&
			m_brightnessSet && m_brightnessGet;
	}

	virtual bool color_temperature_supported (color_temperature temp) const override
	{
		return false;
	}

	virtual bool get_brightness (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		const_cast <WinWmiMonitorControl*> (this)->update_brightness_getter ();
		assert (m_brightnessGet);
		if (!m_brightnessGet)
			return false;

		CComVariant value;
		if (!m_brightnessGet->getProperyValue (L"CurrentBrightness", &value))
			return false;

		minimum = 0;
		maximum = 100;
		current = value.uintVal;

		return true;
	}

	virtual bool set_brightness (uint32_t value) override
	{
		auto method = m_brightnessSet->methods (L"WmiSetBrightness");
		assert (method);
		if (!method)
			return false;
		auto inParams = method->inParameters ()->spawnInstance ();
		assert (inParams);
		verify (inParams->put (L"Timeout", &CComVariant (L"0"), CIM_UINT32));
		std::wstringstream wssValue; wssValue << value;
		verify (inParams->put (L"Brightness", 
			&CComVariant (wssValue.str ().c_str ()), CIM_UINT8));
		return m_brightnessSet->execMethod (L"WmiSetBrightness", inParams);
	}

	virtual bool get_contrast (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		assert (!"implemented");
		return false;
	}

	virtual bool set_contrast (uint32_t value) override
	{
		assert (!"implemented");
		return false;
	}

	virtual bool get_color_temperature (color_temperature& result) const override
	{
		assert (!"implemented");
		return false;
	}

	virtual bool set_color_temperature (color_temperature value) override
	{
		assert (!"implemented");
		return false;
	}

	virtual bool get_rgb_gain (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		assert (!"implemented");
		return false;
	}

	virtual bool set_rgb_gain (rgb_part what, uint32_t value) override
	{
		assert (!"implemented");
		return false;
	}

	virtual bool get_rgb_drive (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		assert (!"implemented");
		return false;
	}

	virtual bool set_rgb_drive (rgb_part what, uint32_t value) override
	{
		assert (!"implemented");
		return false;
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
	std::shared_ptr <WbemLocator> m_wbemLocator;
	std::shared_ptr <WbemServices> m_wbemServices;
	std::wstring m_wmi_instance_name;
	std::shared_ptr <WbemClassObject> m_brightnessGet;
	std::shared_ptr <WbemClassObject> m_brightnessSet;

protected:
	static std::shared_ptr <WbemClassObject> find_object_by_instance_name (
		const std::vector <std::shared_ptr <WbemClassObject>> &objects,
		const std::wstring &name)
	{
		for (auto &&object : objects)
		{
			CComVariant value;
			verify (object->getProperyValue (L"InstanceName", &value));
			assert (value.vt == VT_BSTR);
			if (value.vt == VT_BSTR && 
				value.bstrVal == name)
			{
				return object;
			}
		}
		return nullptr;
	}

	void update_brightness_getter ()
	{
		auto objects = m_wbemServices->Objects (L"WmiMonitorBrightness");
		m_brightnessGet = find_object_by_instance_name (
			objects, m_wmi_instance_name);
	}
};