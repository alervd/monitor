#pragma once
class StaticDisplayDeviceInfo :
	public vmsSerializable
{
public:
	uint32_t m_id = 0;
	xstring m_displayName;
	xstring m_instanceName;
	xstring m_manufacturerName;
	xstring m_productCode;
	xstring m_serialNumber;

public:
	void copy_from (const DisplayDeviceInfo &info)
	{
		m_instanceName = info.instance_name ();
		m_displayName = info.display_name ();
		m_manufacturerName = info.manufacturer_name ();
		m_productCode = info.product_code ();
		m_serialNumber = info.serial_number ();
	}

public:
	virtual bool Serialize (
		vmsSerializationIoStream *pStm, 
		unsigned flags = 0) override
	{
		return pStm->SerializeValue (L"displayName", m_displayName) &&
			pStm->SerializeValue (L"instanceName", m_instanceName) &&
			pStm->SerializeValueS (L"id", m_id) &&
			pStm->SerializeValue (L"manufacturerName", m_manufacturerName) &&
			pStm->SerializeValue (L"productCode", m_productCode) &&
			pStm->SerializeValue (L"serialNumber", m_serialNumber);
	}
};