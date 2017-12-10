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
	static	bool			AutoUpdateEnabled();	// Auto update enabled
	static	double			GetCpuUsage();
	static	double			GetRamUsage();
	static	long double		GetDriveUsageFreeBytesAvailable();
	static	long double		GetDriveUsageTotalNumberOfBytes();
	static	wstring			QueryLocalHostname();
	static  wstring			QueryLastSystemUpdateDateTime();
	static	wstring			QueryAntiVirusStatus();
	static  string			GetCurrentDateTime();
	static  string			GetOSArchName();
	static	string			GetOSName();

	static  vector<CertExpiryInfo> ScanCertExpiry();

};
