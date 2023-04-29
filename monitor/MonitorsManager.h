#pragma once
#include "MonitorManager.h"
class MonitorsManager
{
public:
	using monitor_list_t = std::vector <std::shared_ptr <MonitorManager>>;
	virtual std::shared_ptr <monitor_list_t> monitors () const = 0;
	virtual bool refresh_monitor_list () = 0;
	virtual void on_monitors_state_changed () {}
	virtual bool resore_monitors_original_state () {return false;}
	virtual ~MonitorsManager () {}
};