#pragma once
#include "../MonitorsManager.h"
#include "WinMonitorManager.h"
#include "WinDisplayDevicesInfo.h"
class WinMonitorsManager :
	public MonitorsManager,
	public vmsThreadSafe4
{
public:
	WinMonitorsManager ()
	{
	}

	virtual std::shared_ptr <monitor_list_t> monitors () const override
	{
		vmsThreadSafe4Scope;
		return m_monitors;
	}

	virtual bool refresh_monitor_list () override
	{
		return refresh_monitor_list (
			std::make_shared <WinDisplayDevicesInfo> ());
	}

	bool refresh_monitor_list (
		std::shared_ptr <WinDisplayDevicesInfo> displayDevicesInfo)
	{
		assert (displayDevicesInfo);
		vmsThreadSafe4Scope;
		m_displayDevicesInfo = displayDevicesInfo;
		m_monitors = std::make_shared <monitor_list_t> ();
		return collect_monitors ();
	}

protected:
	std::shared_ptr <WinDisplayDevicesInfo> m_displayDevicesInfo;
	std::shared_ptr <monitor_list_t> m_monitors;

protected:
	virtual bool collect_monitors () = 0;
};