#pragma once
#include "../DisplayDeviceInfo.h"
class WinDisplayDeviceInfo :
	public DisplayDeviceInfo
{
public:
	virtual bool active () const override
	{
		return m_active;
	}

	virtual xstring name () const override
	{
		return m_name;
	}

	virtual xstring display_name () const override
	{
		return m_displayName;
	}

	virtual xstring instance_name () const override
	{
		return !m_deviceUID.empty () ? 
			m_deviceUID : m_deviceID;
	}

	virtual xstring manufacturer_name () const override
	{
		return L"";
	}

	virtual xstring product_code () const override
	{
		return L"";
	}

	virtual xstring serial_number () const override
	{
		return L"";
	}

public:
	// flags can be EDD_GET_DEVICE_INTERFACE_NAME
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd162609%28v=vs.85%29.aspx
	void read_from (const DISPLAY_DEVICE &dd, DWORD flags = 0)
	{
		m_name = dd.DeviceName;
		m_displayName = dd.DeviceString;
		if (flags & EDD_GET_DEVICE_INTERFACE_NAME)
			m_deviceUID = dd.DeviceID;
		else
			m_deviceID = dd.DeviceID;
		m_active = (dd.StateFlags & DISPLAY_DEVICE_ACTIVE) != 0;
	}

public:
	bool m_active = false;
	std::wstring m_name;
	std::wstring m_displayName;
	std::wstring m_deviceID;
	std::wstring m_deviceUID;
};