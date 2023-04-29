#pragma once
#include "WinWmiMonitorId.h"

inline std::vector <std::shared_ptr <WinWmiMonitorId>> collectWmiMonitorsIds (
	WbemServices *services)
{
	std::vector <std::shared_ptr <WinWmiMonitorId>> result;
	auto objects = services->Objects (L"WmiMonitorID");
	for (auto &&object : objects)
	{
		auto id = std::make_shared <WinWmiMonitorId> (
			object.get ());
		if (!id->m_instanceName.empty ())
			result.push_back (id);
	}
	return result;
}