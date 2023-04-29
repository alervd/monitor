#pragma once
#include "WinDisplayDeviceInfo.h"
#include "WinWmiMonitorId.h"
class WinDisplayDeviceMultiInfo :
	public DisplayDeviceInfo
{
public:
	WinDisplayDeviceMultiInfo (
		std::shared_ptr <const DisplayDeviceInfo> info,
		std::shared_ptr <WinWmiMonitorId> wmiInfo) :
		m_info (info),
		m_wmiInfo (wmiInfo)
	{
	}

	std::shared_ptr <const DisplayDeviceInfo> rawInfo () const
	{
		return m_info;
	}

public:
	virtual bool active () const override
	{
		return m_info->active ();
	}

	virtual xstring name () const override
	{
		return m_info->name ();
	}

	virtual xstring display_name () const override
	{
		auto s = m_wmiInfo ? m_wmiInfo->m_userFriendlyName : L"";
		return !s.empty () ? s : m_info->display_name ();
	}

	virtual xstring instance_name () const override
	{
		return m_info->instance_name ();
	}

	virtual xstring manufacturer_name () const override
	{
		return m_wmiInfo ? 
			m_wmiInfo->m_manufacturerName : L"";
	}

	virtual xstring product_code () const override
	{
		return m_wmiInfo ? 
			m_wmiInfo->m_productCode : L"";
	}

	virtual xstring serial_number () const override
	{
		return m_wmiInfo ? 
			m_wmiInfo->m_serialNumber : L"";
	}

protected:
	std::shared_ptr <const DisplayDeviceInfo> m_info;
	std::shared_ptr <WinWmiMonitorId> m_wmiInfo;
};