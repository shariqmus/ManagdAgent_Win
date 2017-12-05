#include "stdafx.h"
#include "InfoSystem.h"

#include "Utility.h"
#include "PowerShellProcess.h"

#pragma warning (disable : 4996)

InfoSystem::InfoSystem()
{
}


InfoSystem::~InfoSystem()
{
}

// Does this machine requires a reboot
bool InfoSystem::RebootRequired()
{
	bool bRet = false;

	const wstring testSubKey = L"SYSTEM\\CurrentControlSet\\Control\\Session Manager";
	RegKey key;

	try
	{
		key.Open(HKEY_LOCAL_MACHINE, testSubKey);

		vector<wstring> MultiSz1 = key.GetMultiStringValue(L"PendingFileRenameOperations");

		key.Close();

		if (MultiSz1.size() > 0)
			bRet = true;

		return bRet;
	}
	catch (RegException e)
	{
		return false;
	}
}


bool InfoSystem::WindowsUpdateEnabled()
{
	bool bRet = false;

	const wstring testSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WindowsUpdate\\Auto Update";
	RegKey key;

	try
	{
		key.Open(HKEY_LOCAL_MACHINE, testSubKey);

		DWORD Dw1 = key.GetDwordValue(L"AUOptions");

		key.Close();

		if (std::to_wstring(Dw1) == L"4")
			bRet = true;

		return bRet;
	}
	catch (RegException e)
	{
		return false;
	}
}

ULONGLONG InfoSystem::ft2ull(FILETIME &ft) {
	ULARGE_INTEGER ul;
	ul.HighPart = ft.dwHighDateTime;
	ul.LowPart = ft.dwLowDateTime;

	return ul.QuadPart;
}

double InfoSystem::GetCpuUsage()
{
/*
	wstring lastWindowsUpdateDate = L"";

	// This PS Script should be smart enough to tell us the status of antivirus on this host
	wstring scriptOutput = PowerShellProcess::ExecPowerShellScript(L"psQueryCPULoad.ps1");

	return _wtof(scriptOutput.c_str());
*/

    FILETIME idle, prev_idle;
	FILETIME kernel, prev_kernel;
	FILETIME user, prev_user;
	GetSystemTimes(&prev_idle, &prev_kernel, &prev_user);
	Sleep(1000);
	GetSystemTimes(&idle, &kernel, &user);
	ULONGLONG sys = (ft2ull(user) - ft2ull(prev_user)) +
		(ft2ull(kernel) - ft2ull(prev_kernel));
	int cpu = int((sys - ft2ull(idle) + ft2ull(prev_idle)) * 100.0 / sys);
	prev_idle = idle;
	prev_kernel = kernel;
	prev_user = user;

	return cpu;


	/*
	static PDH_HQUERY cpuQuery;
	static PDH_HCOUNTER cpuTotal;

	PdhOpenQuery(NULL, NULL, &cpuQuery);
	// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
	PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
	PdhCollectQueryData(cpuQuery);

	PDH_FMT_COUNTERVALUE counterVal;

	PdhCollectQueryData(cpuQuery);
	PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);

	return counterVal.doubleValue;
	*/
}

int InfoSystem::GetRamUsage(){

	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof(statex);

	GlobalMemoryStatusEx(&statex);

	return statex.dwMemoryLoad;
}

long double InfoSystem::GetDriveUsageFreeBytesAvailable(){
	_int64 FreeBytesAvailable;
	_int64 TotalNumberOfBytes;
	_int64 TotalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(0,
		(PULARGE_INTEGER)&FreeBytesAvailable,
		(PULARGE_INTEGER)&TotalNumberOfBytes,
		(PULARGE_INTEGER)&TotalNumberOfFreeBytes
		);

	return (long double)FreeBytesAvailable / 1024.0 / 1024.0 / 1024.0;
}

long double InfoSystem::GetDriveUsageTotalNumberOfBytes()
{
	_int64 FreeBytesAvailable;
	_int64 TotalNumberOfBytes;
	_int64 TotalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(0,
		(PULARGE_INTEGER)&FreeBytesAvailable,
		(PULARGE_INTEGER)&TotalNumberOfBytes,
		(PULARGE_INTEGER)&TotalNumberOfFreeBytes
		);

	return (long double)TotalNumberOfBytes / 1024.0 / 1024.0 / 1024.0;
}

wstring InfoSystem::QueryLocalHostname()
{
	TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(computerName) / sizeof(computerName[0]);
	GetComputerName(computerName, &size);

	wstring hostName(computerName);

	return (hostName);
}

wstring InfoSystem::QueryLastWindowsUpdateDate()
{
	wstring lastWindowsUpdateDate = L"";
	wstring scriptOutput = PowerShellProcess::ExecPowerShellScript(L"psQueryWindowsUpdates.ps1");
	wistringstream iss(scriptOutput);

	for (wstring line; std::getline(iss, line); )
	{
		vector<wstring> v = Utility::explode(line, ',');

		for (std::vector<wstring>::iterator it = v.begin(); it != v.end(); ++it) 
		{
			lastWindowsUpdateDate = *it;
			break; // only interested in first record
		}
	}

	return lastWindowsUpdateDate;
}

wstring InfoSystem::QueryAntiVirusStatus()
{
	wstring antiVirusStatus = L"";
	
	// This PS Script should be smart enough to tell us the status of antivirus on this host
	wstring scriptOutput = PowerShellProcess::ExecPowerShellScript(L"psQueryAntiVirusStatus.ps1");
	wistringstream iss(scriptOutput);

	for (wstring line; std::getline(iss, line); )
	{
		vector<wstring> v = Utility::explode(line, ';');

		for (std::vector<wstring>::iterator it = v.begin(); it != v.end(); ++it)
		{
			antiVirusStatus = *it;
			break; // only interested in first record
		}
	}

	return antiVirusStatus;
}

string InfoSystem::GetCurrentDateTime()
{
	return Utility::getCurrentDateTime();
}

vector<CertExpiryInfo> InfoSystem::ScanCertExpiry()
{
	vector<CertExpiryInfo> certExpiryInfo;
	
	HCERTSTORE       hSysStore;
	PCCERT_CONTEXT   pCertContext = NULL;
	wchar_t			 pszNameString[256];
	DWORD            dwPropId = 0;

	hSysStore = CertOpenStore(
		CERT_STORE_PROV_SYSTEM,   // the store provider type
		0,                        // the encoding type is not needed
		NULL,                     // use the default HCRYPTPROV
		CERT_SYSTEM_STORE_LOCAL_MACHINE,
		L"My"    // the store name as a Unicode string, "My", "root", "trust", or "CA".
		);

	if (hSysStore)
	{
		FILE_LOG(logINFO) << "Certificate store opened successfully.";
	}
	else
	{
		FILE_LOG(logINFO) << "Error opening system store.";
	}

	while (pCertContext = CertEnumCertificatesInStore(hSysStore, pCertContext))
	{
		if (CertGetNameString(pCertContext,	CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, pszNameString, 128))
		{
			FILE_LOG(logINFO) << "Inquiry for Certificate: "  << Utility::wstring2string(wstring(pszNameString)).c_str();
		}
		else
		{
			FILE_LOG(logERROR) << "CertGetName failed.";
		}

		// Check Expiry
		
		FILE_LOG(logINFO) << "Calculating cert expiry time difference...";
		
		FILETIME expirytime;
		SYSTEMTIME systemTime;
		expirytime = pCertContext->pCertInfo->NotAfter;
		FileTimeToSystemTime(&expirytime, &systemTime);

		/*printf("System Time: Year:%d\nMonth:%d\nDate:%d\nHour:%d\nMin:%d\nSecond:%d\n",
			systemTime.wYear, systemTime.wMonth, systemTime.wDay,
			systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
        */

		FILE_LOG(logINFO) << "Expiry date: " << systemTime.wMonth << "/" << systemTime.wDay << "/" << systemTime.wYear;

		// Current date/time
		SYSTEMTIME currentTime;
		GetSystemTime(&currentTime);
		
		/*printf("Current Time: Year:%d\nMonth:%d\nDate:%d\nHour:%d\nMin:%d\nSecond:%d\n", 
			currentTime.wYear, currentTime.wMonth, currentTime.wDay, 
			currentTime.wHour, currentTime.wMinute, currentTime.wSecond);
        */

		_int64 i = Utility::TimeDelta(currentTime, systemTime);
		FILE_LOG(logINFO) << "times are " << (i / 10000000) / 86400 << " days apart";

		int dayApart = (int) (i / 10000000) / 86400;
		
		CertExpiryInfo expInfo;

		expInfo.certName = Utility::wstring2string(wstring(pszNameString));
		expInfo.certExpiryDays = dayApart;

		certExpiryInfo.push_back(expInfo);
	}

	/* TEST */
	/*
	CertExpiryInfo expInfo2;
	expInfo2.certName = "test cert";
	expInfo2.certExpiryDays = 99;
	certExpiryInfo.push_back(expInfo2);
	*/
	/* TEST */

	CertFreeCertificateContext(pCertContext);
	CertCloseStore(hSysStore, 0);

	return certExpiryInfo;
}
