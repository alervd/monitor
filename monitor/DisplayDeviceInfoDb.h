#pragma once
#include "StaticDisplayDeviceInfo.h"
class DisplayDeviceInfoDb : 
	public vmsSerializable
{
public:
	using device_list_t = std::vector <std::shared_ptr <StaticDisplayDeviceInfo>>;

	void add_device (std::shared_ptr <StaticDisplayDeviceInfo> dev)
	{
		dev->m_id = m_nextDeviceId++;
		m_devices.push_back (dev);
		setDirty ();
	}

	const device_list_t& devices () const
	{
		return m_devices;
	}

protected:
	device_list_t m_devices;
	uint32_t m_nextDeviceId = 1;

public:
	virtual bool Serialize (
		vmsSerializationIoStream *pStm, 
		unsigned flags = 0) override
	{
		if (pStm->isInputStream ())
		{
			m_devices.clear ();
			auto nodes = pStm->SelectNodes (L"device");
			for (auto &&node : nodes)
			{
				auto dev = std::make_shared <StaticDisplayDeviceInfo> ();
				if (dev->Serialize (node.get (), flags))
				{
					m_devices.push_back (dev);
					m_nextDeviceId = max (m_nextDeviceId, dev->m_id+1);
				}
			}
		}
		else
		{
			for (auto &&dev : m_devices)
			{
				auto node = pStm->SelectOrCreateNode (L"device");
				assert (node);
				if (!node)
					return false;
				if (!dev->Serialize (node.get (), flags))
					return false;
			}
		}
		return true;
	}
};