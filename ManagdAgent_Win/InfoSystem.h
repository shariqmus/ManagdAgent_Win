#pragma once
#include "Info.h"

struct CertExpiryInfo
{
	string certName;
	int certExpiryDays;
};

class InfoSystem :
	public Info
{
public:
	InfoSystem();
	~InfoSystem();

private:
	static ULONGLONG ft2ull(FILETIME &ft);

public:
	static	bool			RebootRequired();		// Does this machine requires a reboot
	static	bool			WindowsUpdateEnabled();	// Auto update enabled
	static	double			GetCpuUsage();
	static	int				GetRamUsage();
	static	long double		GetDriveUsageFreeBytesAvailable();
	static	long double		GetDriveUsageTotalNumberOfBytes();
	static	wstring			QueryLocalHostname();
	static  wstring			QueryLastWindowsUpdateDate();
	static	wstring			QueryAntiVirusStatus();
	static  string			GetCurrentDateTime();

	static  vector<CertExpiryInfo> ScanCertExpiry();

};
