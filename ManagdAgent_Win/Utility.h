#pragma once

struct WriteThis {
	const char *readptr;
	size_t sizeleft;
};

class Utility
{
public:
	Utility();
	~Utility();

public:
	// String conversion
	static  string			wstring2string(wstring ws);
	static	string			double2string(double val);
	static	int				string2int(string val);

	// String Manipulation
	static  vector<wstring>	explode(wstring const& s, wchar_t delim);
	static	wstring			removeCharacter(wstring str, wchar_t ch);
	static  void			ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);
	
	// File I/O

	// Config file
	static	string			GetConfigValue(string section, string param);
	
	//Json
	static	int				PostData(string url, string data);

	// Windows System
	static  wstring			getEXEPath();
	static  wstring			getCWD();
	static	string			getCurrentDateTime();
	static _int64			TimeDelta(const SYSTEMTIME st1, const SYSTEMTIME st2);

private:
	static size_t			ReadCallback(void *dest, size_t size, size_t nmemb, void *userp);
	
};
