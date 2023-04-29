#pragma once
class MonitorMultiControlState :
	public vmsSerializable
{
public:
	std::unordered_map <xstring, std::string> m_controlsStates;

public:
	void read_from (
		MonitorControl::state_type st,
		const std::vector <std::shared_ptr <MonitorControl>> controls)
	{
		m_controlsStates.clear ();
		for (auto &&ctrl : controls)
		{
			auto s = ctrl->get_current_state (st);
			if (!s.empty ())
				m_controlsStates [ctrl->control_uid ()] = s;
		}
	}

	bool apply_to (
		MonitorControl::state_type st,
		const std::vector <std::shared_ptr <MonitorControl>> controls)
	{
		bool result = true;
		for (auto &&ctrl : controls)
		{
			auto it = m_controlsStates.find (ctrl->control_uid ());
			if (it == m_controlsStates.end ())
				continue;
			if (!ctrl->set_current_state (st, it->second))
				result = false;
		}
		return result;
	}

public:
	virtual bool Serialize (vmsSerializationIoStream *pStm, 
		unsigned flags = 0) override
	{
		if (pStm->isInputStream ())
		{
			m_controlsStates.clear ();
			auto nodes = pStm->SelectNodes (L"control");
			for (auto &&node : nodes)
			{
				std::wstring uid, state;
				if (!node->SerializeValue (L"uid", uid) ||
					!node->SerializeValue (L"state", state))
				{
					return false;
				}
				m_controlsStates [uid] = std::string (state.begin (), state.end ());
			}
		}
		else
		{
			for (auto &&item : m_controlsStates)
			{
				xstring uid = item.first;
				xstring state (item.second.begin (), item.second.end ());
				auto node = pStm->SelectOrCreateNode (L"control");
				if (!node->SerializeValue (L"uid", uid) ||
					!node->SerializeValue (L"state", state))
				{
					return false;
				}
			}
		}
		return true;
	}
};