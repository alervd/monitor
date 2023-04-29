#pragma once
class DisplayDeviceInfo
{
public:
	virtual bool active () const = 0;
	virtual xstring name () const = 0;
	virtual xstring display_name () const = 0;
	virtual xstring instance_name () const = 0;
	virtual xstring manufacturer_name () const = 0;
	virtual xstring product_code () const = 0;
	virtual xstring serial_number () const = 0;
	virtual ~DisplayDeviceInfo () {}
};