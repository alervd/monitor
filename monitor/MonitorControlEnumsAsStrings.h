#pragma once

#include "MonitorControl.h"

typedef MonitorControl::rgb_part MonitorControl_rgb_part_t;
ENUM_STREAM_SUPPORT_BEGIN (MonitorControl_rgb_part_t)
	ENUM_STREAM_SUPPORT_ENTRY (MonitorControl_rgb_part_t::R, L"R")
	ENUM_STREAM_SUPPORT_ENTRY (MonitorControl_rgb_part_t::G, L"G")
	ENUM_STREAM_SUPPORT_ENTRY (MonitorControl_rgb_part_t::B, L"B")
ENUM_STREAM_SUPPORT_END (MonitorControl_rgb_part_t)
