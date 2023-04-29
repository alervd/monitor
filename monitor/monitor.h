#pragma once

#ifdef _WIN32
#include "win/WinAllMonitorsManager.h"
#endif

inline std::shared_ptr <MonitorsManager> create_MonitorsManager (
	MonitorControl::kind preferredControlKind = MonitorControl::kind::display)
{
#ifdef _WIN32
	return std::make_shared <WinAllMonitorsManager> (
		preferredControlKind);
#endif
	assert (!"implemented on the target platform");
	return nullptr;
}