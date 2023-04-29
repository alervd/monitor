#pragma once
#include "../defs.h"
#include "../MonitorControl.h"
#include "WinDisplayDeviceInfo.h"
#include "WinGammaRampMonitorControlState.h"
class WinGammaRampMonitorControl :
	public MonitorControl
{
public:
	WinGammaRampMonitorControl (
		const std::wstring &adapterName) :
		m_adapterName (adapterName)
	{
	}

	~WinGammaRampMonitorControl ()
	{
		if (m_hdcMonitor)		
			DeleteDC (m_hdcMonitor);
	}

public:
	virtual xstring control_uid () const override
	{
		return L"GammaRamp";
	}

	virtual xstring control_uid (capability cap) const override
	{
		return control_uid ();
	}

	virtual kind control_kind () const override
	{
		return kind::video_card;
	}

	virtual kind control_kind (capability cap) const override
	{
		return kind::video_card;
	}

	virtual void preferred_control_kind (kind value) override
	{
	}

	virtual bool capability_supported (capability cap) const override
	{
		if (!m_gammaRampSupported)
			return false;
		return cap == capability::brightness ||
			cap == capability::rgb_gain;
	}

	virtual bool color_temperature_supported (color_temperature temp) const override
	{
		return false; // not implemented
	}

	virtual bool get_brightness (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		minimum = 0;
		maximum = 100;
		current = (uint32_t)(m_coeffRgb * 100);
		return true;
	}

	virtual bool set_brightness (uint32_t value) override
	{
		auto newval = value / 100.0f;
		if (newval != m_coeffRgb)
		{
			apply_coefficients_to_ramp_array (
				&m_ramp.front (), newval, m_coeffR, m_coeffG, m_coeffB);
			if (!SetDeviceGammaRamp (m_hdcMonitor, &m_ramp.front ()))
				return false;
			m_coeffRgb = newval;
		}
		return true;
	}

	virtual bool get_contrast (uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		return false;
	}

	virtual bool set_contrast (uint32_t value) override
	{
		return false;
	}

	virtual bool get_color_temperature (color_temperature& result) const override
	{
		return false;
	}

	virtual bool set_color_temperature (color_temperature value) override
	{
		return false;
	}

	virtual bool get_rgb_gain (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		minimum = 0;
		maximum = 256;
		current = (uint32_t)(256*rgb_coefficient (what));
		return true;
	}

	virtual bool set_rgb_gain (rgb_part what, uint32_t value) override
	{
		auto newval = value/256.0f;
		if (abs(newval - rgb_coefficient (what)) > 0.0001f)
		{
			apply_color_coefficient (rgb_ramp (what), newval*m_coeffRgb);
			if (!SetDeviceGammaRamp (m_hdcMonitor, &m_ramp.front ()))
				return false;
			rgb_coefficient (what) = newval;
		}
		return true;
	}

	virtual bool get_rgb_drive (rgb_part what, uint32_t& minimum, uint32_t& current, uint32_t& maximum) const override
	{
		return false;
	}

	virtual bool set_rgb_drive (rgb_part what, uint32_t value) override
	{
		return false;
	}

	virtual std::string get_current_state (state_type st) override
	{
		WinGammaRampMonitorControlState s;
		switch (st)
		{
		case MonitorControl::control_and_monitor:
			s.m_ramp = m_ramp;
			// no break here
		case MonitorControl::control_only:
			s.m_coeffRgb = m_coeffRgb;
			s.m_coeffR = m_coeffR;
			s.m_coeffG = m_coeffG;
			s.m_coeffB = m_coeffB;
			break;
		}
		return vmsJsonSerializeObject (s);
	}

	virtual bool set_current_state (state_type st, 
		const std::string& state) override
	{
		WinGammaRampMonitorControlState s;
		if (!vmsJsonSerializeObject (s, state))
			return false;
		switch (st)
		{
		case MonitorControl::control_and_monitor:
			assert (!s.m_ramp.empty ());
			if (s.m_ramp.empty ())
				return false;
			if (!SetDeviceGammaRamp (m_hdcMonitor, &s.m_ramp.front ()))
				return false;
			m_ramp = s.m_ramp;
			m_coeffRgb = s.m_coeffRgb;
			m_coeffR = s.m_coeffR;
			m_coeffG = s.m_coeffG;
			m_coeffB = s.m_coeffB;
			return true;

		case MonitorControl::control_only:
		{
			std::vector <WORD> ramp (3*GAMMA_RAMP_SIZE);
			apply_coefficients_to_ramp_array (
				&ramp.front (), s.m_coeffRgb, s.m_coeffR, s.m_coeffG, s.m_coeffB);
			if (!memcmp (&m_ramp.front (), &ramp.front (), 3*GAMMA_RAMP_SIZE*2))
			{
				m_coeffRgb = s.m_coeffRgb;
				m_coeffR = s.m_coeffR;
				m_coeffG = s.m_coeffG;
				m_coeffB = s.m_coeffB;
			}
			return true;
		}

		default:
			assert (!"implemented");
			return false;
		}
	}

protected:
	std::wstring m_adapterName;
	bool m_gammaRampSupported = false;
	HDC m_hdcMonitor = nullptr;
	const size_t GAMMA_RAMP_SIZE = 256;
	std::vector <WORD> m_ramp;

protected:
	float m_coeffRgb = 1.0f, 
		m_coeffR = 1.0f, 
		m_coeffG = 1.0f, 
		m_coeffB = 1.0f;

public:
	bool initialize ()
	{
		m_hdcMonitor = CreateDC (L"DISPLAY", 
			!m_adapterName.empty () ? m_adapterName.c_str () : nullptr,
			nullptr, nullptr);
		assert (m_hdcMonitor);
		if (!m_hdcMonitor)
			return false;

		m_ramp.resize (3*GAMMA_RAMP_SIZE);
		if (!GetDeviceGammaRamp (m_hdcMonitor, &m_ramp.front ()))
			return false;

		auto caps = GetDeviceCaps (m_hdcMonitor, COLORMGMTCAPS);
		if (caps != CM_GAMMA_RAMP &&
			!SetDeviceGammaRamp (m_hdcMonitor, &m_ramp.front ()))
		{
			return false;
		}

		m_gammaRampSupported = true;

		read_coefficients ();

		return true;
	}

protected:
	void read_coefficients ()
	{
		assert (m_ramp.size () == 3*GAMMA_RAMP_SIZE);
		float coeffR = read_color_coefficient (
			rgb_ramp (MonitorControl::rgb_part::R));
		float coeffG = read_color_coefficient (
			rgb_ramp (MonitorControl::rgb_part::G));
		float coeffB = read_color_coefficient (
			rgb_ramp (MonitorControl::rgb_part::B));
		float coeffMax = max (coeffR, coeffG);
		coeffMax = max (coeffMax, coeffB);
		if (coeffMax > 1.0f)
		{
			coeffR /= coeffMax;
			coeffG /= coeffMax;
			coeffB /= coeffMax;
		}
		m_coeffR = coeffR;
		m_coeffG = coeffG;
		m_coeffB = coeffB;
		m_coeffRgb = coeffMax > 1.0f ? coeffMax : 1.0f;
	}

	float read_color_coefficient (const WORD *ramp)
	{
		float result = 0;
		for (size_t i = 1; i < GAMMA_RAMP_SIZE; ++i)
		{
			float val = (ramp [i] / i - 128) / 256.0f;
			result = (result*(i-1) + val) / i;
		}
		return result;
	}

	void apply_color_coefficient (WORD *ramp, float coeff)
	{
		size_t tmp = (size_t)(256*coeff + 128);
		for (size_t i = 0; i < GAMMA_RAMP_SIZE; ++i)
		{
			auto val = tmp*i;
			if (val > 65535)
				val = 65535;
			ramp [i] = (WORD)val;
		}
	}
	
	float& rgb_coefficient (rgb_part what)
	{
		switch (what)
		{
		case MonitorControl::rgb_part::R: return m_coeffR;
		case MonitorControl::rgb_part::G: return m_coeffG;
		case MonitorControl::rgb_part::B: return m_coeffB;
		default: make_sure (!"expected"); return m_coeffB;
		}
	}

	const float& rgb_coefficient (rgb_part what) const
	{
		return const_cast <WinGammaRampMonitorControl*> (this)->
			rgb_coefficient (what);
	}

	WORD* rgb_ramp (rgb_part what)
	{
		switch (what)
		{
		case MonitorControl::rgb_part::R: return &m_ramp [0];
		case MonitorControl::rgb_part::G: return &m_ramp [GAMMA_RAMP_SIZE];
		case MonitorControl::rgb_part::B: return &m_ramp [2*GAMMA_RAMP_SIZE];
		default: make_sure (!"expected"); return nullptr;
		}
	}

	void apply_coefficients_to_ramp_array (
		WORD *gamma_ramp, 
		float rgb, float r, float g, float b)
	{
		apply_color_coefficient (gamma_ramp, rgb*r);
		apply_color_coefficient (gamma_ramp+GAMMA_RAMP_SIZE, rgb*g);
		apply_color_coefficient (gamma_ramp+2*GAMMA_RAMP_SIZE, rgb*b);
	}
};