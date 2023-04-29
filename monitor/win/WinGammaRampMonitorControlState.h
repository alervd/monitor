#pragma once
class WinGammaRampMonitorControlState :
	public vmsSerializable
{
public:
	float m_coeffRgb,
		m_coeffR,
		m_coeffG,
		m_coeffB;
	std::vector <WORD> m_ramp;

public:
	virtual bool Serialize (
		vmsSerializationIoStream *pStm, 
		unsigned flags = 0) override
	{
		if (!pStm->SerializeValueS (L"coeffRgb", m_coeffRgb) ||
			!pStm->SerializeValueS (L"coeffR", m_coeffR) ||
			!pStm->SerializeValueS (L"coeffG", m_coeffG) ||
			!pStm->SerializeValueS (L"coeffB", m_coeffB))
		{
			return false;
		}
		if (pStm->isInputStream ())
		{
			std::wstring ws;
			if (pStm->SerializeValue (L"gammaRamp", ws))
			{
				std::string s (ws.begin (), ws.end ());
				s = base64::decode (s);
				m_ramp.resize (3*256);
				memcpy (&m_ramp.front (), &s.front (), 3*256*2);
			}
		}
		else
		{
			if (!m_ramp.empty ())
			{
				std::string s;
				base64::encode (&m_ramp.front (), m_ramp.size () * 2, s);
				std::wstring ws (s.begin (), s.end ());
				if (!pStm->SerializeValue (L"gammaRamp", ws))
					return false;
			}
		}
		return true;
	}
};