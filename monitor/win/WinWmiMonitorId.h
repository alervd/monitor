#pragma once
#include <atlsafe.h>
class WinWmiMonitorId
{
public:
	WinWmiMonitorId (WbemClassObject *monitorIdGetter)
	{
		verify (retrieve (monitorIdGetter));
	}

public:
	std::wstring m_instanceName;
	std::wstring m_manufacturerName;
	std::wstring m_productCode;
	std::wstring m_serialNumber;
	std::wstring m_userFriendlyName;

protected:
	bool retrieve (WbemClassObject *monitorIdGetter)
	{
		CComVariant value;

		if (monitorIdGetter->getProperyValue (L"InstanceName", &value) &&
			value.vt == VT_BSTR)
		{
			m_instanceName = value.bstrVal;
		}

		assert (!m_instanceName.empty ());
		if (m_instanceName.empty ())
			return false;

		if (monitorIdGetter->getProperyValue (L"ManufacturerName", &value))
			m_manufacturerName = SafeArrayToString (value);
		
		if (monitorIdGetter->getProperyValue (L"ProductCodeID", &value))
			m_productCode = SafeArrayToString (value);

		if (monitorIdGetter->getProperyValue (L"SerialNumberID", &value))
			m_serialNumber = SafeArrayToString (value);

		if (monitorIdGetter->getProperyValue (L"UserFriendlyName", &value))
			m_userFriendlyName = SafeArrayToString (value);

		return true;
	}

protected:
	static std::wstring SafeArrayToString (const CComVariant &value,
		bool remove_right_zeroes = true)
	{
		assert ((value.vt & VT_ARRAY) || (value.vt == VT_NULL));
		if (!(value.vt & VT_ARRAY))
			return {};
		const auto valtype = value.vt & VT_TYPEMASK;
		const bool integer = 
			valtype == VT_I1 || valtype == VT_I2 || valtype == VT_I4 ||
			valtype == VT_UI1 || valtype == VT_UI2 || valtype == VT_UI4 ||
			valtype == VT_INT || valtype == VT_UINT;
		assert (integer);
		if (!integer)
			return {};
		if (FAILED (SafeArrayLock (value.parray)))
			return {};
		std::wstring result;
		LONG lb = 0, ub = 0;
		verify (SUCCEEDED (SafeArrayGetLBound (value.parray, 1, &lb)));
		verify (SUCCEEDED (SafeArrayGetUBound (value.parray, 1, &ub)));
		for (auto index = lb; index != ub; ++index)
		{
			uint32_t val = 0;
			verify (SUCCEEDED (SafeArrayGetElement (value.parray, &index, &val)));
			result.push_back ((wchar_t)val);
		}
		verify (SUCCEEDED (SafeArrayUnlock (value.parray)));
		if (remove_right_zeroes && !result.empty ())
		{
			auto it = result.end () - 1;
			while (!*it)
				--it;
			result.erase (it+1, result.end ());
		}
		return result;
	}
};