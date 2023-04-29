#pragma once
class MonitorControlGenericState :
	public vmsSerializable
{
public:
	float m_brightness = 0, m_contrast = 0,
		m_r = 0, m_g = 0, m_b = 0;
	enum have
	{
		brightness = 1,
		contrast = 1 << 1,
		r = 1 << 2,
		g = 1 << 3,
		b = 1 << 4
	};
	uint32_t m_have = 0;

public:
	void read_from (const MonitorControl *mon)
	{
		m_have = 0;
		
		uint32_t m, mx, c;

		if (mon->capability_supported (MonitorControl::capability::brightness) &&
			mon->get_brightness (m, c, mx))
		{
			m_brightness = float ((c - m)) / (mx - m);
			m_have |= have::brightness;
		}

		if (mon->capability_supported (MonitorControl::capability::contrast) &&
			mon->get_contrast (m, c, mx))
		{
			m_contrast = float ((c - m)) / (mx - m);
			m_have |= have::contrast;
		}

		if (mon->capability_supported (MonitorControl::capability::rgb_gain))
		{
			if (mon->get_rgb_gain (MonitorControl::rgb_part::R, m, c, mx))
			{
				m_r = float ((c - m)) / (mx - m);
				m_have |= have::r;
			}
			if (mon->get_rgb_gain (MonitorControl::rgb_part::G, m, c, mx))
			{
				m_g = float ((c - m)) / (mx - m);
				m_have |= have::g;
			}
			if (mon->get_rgb_gain (MonitorControl::rgb_part::B, m, c, mx))
			{
				m_b = float ((c - m)) / (mx - m);
				m_have |= have::b;
			}
		}
	}

	bool apply_to (MonitorControl *mon)
	{
		bool result = true;
		uint32_t m, c, mx;

		if (m_have & have::brightness &&
			mon->capability_supported (MonitorControl::capability::brightness) &&
			mon->get_brightness (m, c, mx))
		{
			if (!mon->set_brightness (m + static_cast <uint32_t> ((mx-m) * m_brightness)))
				result = false;
		}

		if (m_have & have::contrast &&
			mon->capability_supported (MonitorControl::capability::contrast) &&
			mon->get_contrast (m, c, mx))
		{
			if (!mon->set_contrast (m + static_cast <uint32_t> ((mx - m) * m_contrast)))
				result = false;
		}

		if (mon->capability_supported (MonitorControl::capability::rgb_gain))
		{
			if (m_have & have::r &&
				mon->get_rgb_gain (MonitorControl::rgb_part::R, m, c, mx))
			{
				if (!mon->set_rgb_gain (MonitorControl::rgb_part::R, m + static_cast <uint32_t> ((mx - m) * m_r)))
					result = false;
			}

			if (m_have & have::g &&
				mon->get_rgb_gain (MonitorControl::rgb_part::G, m, c, mx))
			{
				if (!mon->set_rgb_gain (MonitorControl::rgb_part::G, m + static_cast <uint32_t> ((mx - m) * m_g)))
					result = false;
			}

			if (m_have & have::b &&
				mon->get_rgb_gain (MonitorControl::rgb_part::B, m, c, mx))
			{
				if (!mon->set_rgb_gain (MonitorControl::rgb_part::B, m + static_cast <uint32_t> ((mx - m) * m_b)))
					result = false;
			}
		}

		return result;
	}

public:
	virtual bool Serialize (
		vmsSerializationIoStream *pStm, uint32_t flags = 0) override
	{
		return pStm->SerializeValueS (L"have", m_have) &&
			pStm->SerializeValueS (L"brightness", m_brightness) &&
			pStm->SerializeValueS (L"contrast", m_contrast) &&
			pStm->SerializeValueS (L"r", m_r) &&
			pStm->SerializeValueS (L"g", m_g) &&
			pStm->SerializeValueS (L"b", m_b);
	}
};