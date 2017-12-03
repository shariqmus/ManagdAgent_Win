// ManagdAgent_Win.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Utility.h"
#include "InfoSystem.h"

#define APP_CODE_VERSION "0.1.0"

int worker()
{
	std::cout << "Starting process run...";

	// Instantiate cxxtimer::Timer object
	cxxtimer::Timer timer;

	// Start the timer
	timer.start();

	// My Initializations
	FILELog::ReportingLevel() = logDEBUG3;
	FILE* log_fd = fopen("agent.log", "a+");
	Output2FILE::Stream() = log_fd;

	// json preparation
	Object jsonRoot, jsonSystem, jsonAlert, jsonMetrics;

	// System	
	jsonSystem << "Hostname" << Utility::wstring2string(InfoSystem::QueryLocalHostname());
	jsonSystem << "SystemDateTime" << InfoSystem::GetCurrentDateTime();

	// Metrics
	double val = 0.0;
	string str = "";

	val = InfoSystem::GetCpuUsage();
	str = Utility::double2string(val);

	jsonMetrics << "CpuUsage" << str;
	jsonMetrics << "CpuUsageUnit" << "Percent";

	jsonMetrics << "RamUsage" << InfoSystem::GetRamUsage();
	jsonMetrics << "RamUsageUnit" << "Percent";

	jsonMetrics << "DriveUsageFreeBytesAvailable" << (int)InfoSystem::GetDriveUsageFreeBytesAvailable();
	jsonMetrics << "DriveUsageFreeBytesAvailableUnit" << "GB";

	jsonMetrics << "DriveUsageTotalNumberOfBytes" << (int)InfoSystem::GetDriveUsageTotalNumberOfBytes();
	jsonMetrics << "DriveUsageTotalNumberOfBytesUnit" << "GB";

	// Alert
	jsonAlert << "RebootRequired" << InfoSystem::RebootRequired();
	jsonAlert << "WindowsUpdateEnabled" << InfoSystem::WindowsUpdateEnabled();
	jsonAlert << "LastWindowsUpdateDateTime" << Utility::wstring2string(InfoSystem::QueryLastWindowsUpdateDate());
	jsonAlert << "AntiVirusStatus" << Utility::wstring2string(InfoSystem::QueryAntiVirusStatus());

	// Root
	jsonRoot << "CodeVersion" << APP_CODE_VERSION;
	jsonRoot << "ConfigVersion" << Utility::GetConfigValue("agent", "version");
	jsonRoot << "AccountNumber" << Utility::GetConfigValue("agent", "account");

	jsonRoot << "System" << jsonSystem;
	jsonRoot << "Metrics" << jsonMetrics;


	vector<CertExpiryInfo> vecCertExpiry = InfoSystem::ScanCertExpiry();

	Array ar;

	for (vector<CertExpiryInfo>::iterator it = vecCertExpiry.begin(); it != vecCertExpiry.end(); it++)
	{
		CertExpiryInfo ce = *it;
		Object o;

		o << "Name" << ce.certName;
		o << "ExpiryDays" << ce.certExpiryDays;

		ar << o;
	}

	jsonAlert << "CertExpiry" << ar;
	jsonRoot << "Alerts" << jsonAlert;

	string data = jsonRoot.json().c_str();
	Utility::ReplaceStringInPlace(data, "\n", "");
	Utility::ReplaceStringInPlace(data, "\t", "");

	// Post json
	string url = Utility::GetConfigValue("agent", "endpoint");
	FILE_LOG(logINFO) << "Sending json: " << data;
	//	Utility::PostData(url, data);

	// Print on screen
	string s = jsonRoot.json().c_str();
	Utility::ReplaceStringInPlace(s, "\n", "\r\n");
	Utility::ReplaceStringInPlace(s, "\t", "  ");
	cout << s;

	//system("pause");

	// Stop/pause the timer
	timer.stop();

	// Get the elapsed time
	FILE_LOG(logINFO) << "Process took " << timer.count<std::chrono::seconds>() << " seconds to run." << std::endl;

	return 0;
}

int main()
{
	int waitSeconds = Utility::string2int(Utility::GetConfigValue("agent", "waitperiod"));
	
	for (;;)
	{
		worker();
		Sleep(waitSeconds * 1000);		// Sleep for 10*1000ms
	}
}
