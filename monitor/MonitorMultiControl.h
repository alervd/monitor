#pragma once
#include "MonitorControl.h"
#include "MonitorMultiControlState.h"
class MonitorMultiControl :
	public MonitorControl
{
public:
	MonitorMultiControl (
		const std::vector <std::shared_ptr <MonitorControl>> &controls,
		kind preferredControlKind = kind::display) :
		m_capabilityControls (size_t (capability::max_index) + 1),
		m_controls (controls),
		m_preferredKind (preferredControlKind)
	{
		assign_controls ();
	}

public:
	virtual xstring control_uid () const override
	{
		return L"multi-control";
	}

	virtual xstring control_uid (capability cap) const override
	{
		auto ctrl = m_capabilityControls [(int)cap];
		return ctrl ? ctrl->control_uid (cap) : L"";
	}

	virtual kind control_kind () const override
	{
		return m_kind;
	}

	virtual kind control_kind (capability cap) const override
	{
		auto ctrl = m_capabilityControls [(int)cap];
		return ctrl ? ctrl->control_kind (cap) : kind::unknown;
	}

	virtual void preferred_control_kind (kind value) override
	{
		m_preferredKind = value;
		if (!m_controls.empty ())
			assign_controls ();
	}

	virtual bool capability_supported (capability cap) const override
	{
		return m_capabilityControls [(int)cap] != nullptr;
	}

	virtual bool color_temperature_supported (color_temperature temp) const override
	{
		auto ctrl = m_capabilityControls [(int)capability::color_temperature];
		return ctrl ? ctrl->color_temperature_supported (temp) : false;
	}

	virtual bool get_brightness (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		auto ctrl = m_capabilityControls [(int)capability::brightness];
		return ctrl ? ctrl->get_brightness (minimum, current, maximum) : false;
	}

	virtual bool set_brightness (uint32_t value) override
	{
		auto ctrl = m_capabilityControls [(int)capability::brightness];
		return ctrl ? ctrl->set_brightness (value) : false;
	}

	virtual bool get_contrast (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		auto ctrl = m_capabilityControls [(int)capability::contrast];
		return ctrl ? ctrl->get_contrast (minimum, current, maximum) : false;
	}

	virtual bool set_contrast (uint32_t value) override
	{
		auto ctrl = m_capabilityControls [(int)capability::contrast];
		return ctrl ? ctrl->set_contrast (value) : false;
	}

	virtual bool get_color_temperature (color_temperature& result) const override
	{
		auto ctrl = m_capabilityControls [(int)capability::color_temperature];
		return ctrl ? ctrl->get_color_temperature (result) : false;
	}

	virtual bool set_color_temperature (color_temperature value) override
	{
		auto ctrl = m_capabilityControls [(int)capability::color_temperature];
		return ctrl ? ctrl->set_color_temperature (value) : false;
	}

	virtual bool get_rgb_gain (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		auto ctrl = m_capabilityControls [(int)capability::rgb_gain];
		return ctrl ? ctrl->get_rgb_gain (what, minimum, current, maximum) : false;
	}

	virtual bool set_rgb_gain (rgb_part what, uint32_t value) override
	{
		auto ctrl = m_capabilityControls [(int)capability::rgb_gain];
		return ctrl ? ctrl->set_rgb_gain (what, value) : false;
	}

	virtual bool get_rgb_drive (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		auto ctrl = m_capabilityControls [(int)capability::rgb_drive];
		return ctrl ? ctrl->get_rgb_drive (what, minimum, current, maximum) : false;
	}

	virtual bool set_rgb_drive (rgb_part what, uint32_t value) override
	{
		auto ctrl = m_capabilityControls [(int)capability::rgb_drive];
		return ctrl ? ctrl->set_rgb_drive (what, value) : false;
	}

	virtual std::string get_current_state (state_type st) override
	{
		MonitorMultiControlState s;
		s.read_from (st, m_controls);
		return vmsJsonSerializeObject (s);
	}

	virtual bool set_current_state (state_type st, const std::string& state) override
	{
		MonitorMultiControlState s;
		if (!vmsJsonSerializeObject (s, state))
			return false;
		return s.apply_to (st, m_controls);
	}

protected:
	kind m_preferredKind;
	kind m_kind = kind::unknown;
	std::vector <std::shared_ptr <MonitorControl>> m_controls;
	std::vector <std::shared_ptr <MonitorControl>> m_capabilityControls;

protected:
	void assign_controls ()
	{
		kind kindAll = kind::unknown;
		bool kindAllPassed1st = false;

		for (size_t i = 0; i <= (size_t)capability::max_index; ++i)
		{
			std::shared_ptr <MonitorControl> mc;

			for (auto &&ctrl : m_controls)
			{
				if (!ctrl->capability_supported ((capability)i))
					continue;

				if (m_preferredKind == kind::unknown ||
					ctrl->control_kind ((capability)i) == m_preferredKind)
				{
					mc = ctrl;
					break;
				}

				if (!mc)
					mc = ctrl;
			}

			m_capabilityControls [i] = mc;

			if (mc)
			{
				const auto kindCap = mc->control_kind ((capability)i);
				if (!kindAllPassed1st)
				{
					kindAll = kindCap;
					kindAllPassed1st = true;
				}
				else if (kindAll != kindCap)
				{
					kindAll = kind::unknown;
				}
			}
		}

		m_kind = kindAll;
	}
};