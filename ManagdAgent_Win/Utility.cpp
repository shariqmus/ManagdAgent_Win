#include "stdafx.h"
#include "Utility.h"

Utility::Utility()
{
}

Utility::~Utility()
{
}

string Utility::wstring2string(wstring ws)
{
	string s(ws.begin(), ws.end());
	s.assign(ws.begin(), ws.end());

	return s;
}

/*
CString Utility::readFile(CString sFilePath)
{
	CString sRet = L"";

	std::ifstream infile(sFilePath);
	std::string line;

	while (std::getline(infile, line)) 
	{
		sRet += line.c_str();
		
		std::istringstream iss(line);
		int a, b;
		if (!(iss >> a >> b)) { break; }
	}

	return sRet;
}


CString Utility::writeFile(CString sFilePath)
{
	ofstream Finput;
	Finput.open("file.txt");

	char str[100];

	cout << "Enter String : ";
	cin.getline(str, sizeof(str));

	Finput << str;
	Finput.close();
}
*/

wstring Utility::getEXEPath()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	wstring::size_type pos = wstring(buffer).find_last_of(L"\\/");
	return wstring(buffer).substr(0, pos);
}

wstring Utility::getCWD()
{
	wchar_t buff[FILENAME_MAX];
	GetCurrentDirectory(FILENAME_MAX, buff);
	std::wstring current_working_dir(buff);
	return current_working_dir;
}

vector<wstring> Utility::explode(wstring const& s, wchar_t delim)
{
	vector<wstring> result;
	wistringstream iss(s);

	for (wstring token; getline(iss, token, delim); )
	{
		result.push_back(move(token));
	}

	return result;
}

wstring Utility::removeCharacter(wstring str, wchar_t ch)
{
	wstring result = str;
	result.erase(remove(result.begin(), result.end(), ch), result.end());

	return result;
}

string	Utility::getCurrentDateTime()
{
	// Get current time
	std::chrono::time_point<std::chrono::system_clock> time_now = std::chrono::system_clock::now();

	// Print time using ctime
	std::time_t time_now_t = std::chrono::system_clock::to_time_t(time_now);
	//std::cout << std::ctime(&time_now_t) << std::endl;

	// Format time and print using strftime
	std::tm now_tm = *std::localtime(&time_now_t);
	
	/*char buf[512];
	std::strftime(buf, 512, "%m/%d/%Y %H:%M:%S", &now_tm);
	std::cout << buf << std::endl;
	*/

	// Format time and print using put_time
	std::stringstream ss;
	ss << std::put_time(&now_tm, "%m/%d/%Y %I:%M:%S %p");
	//std::cout << ss.str() << std::endl;

	return ss.str();
}

void Utility::ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

string Utility::double2string(double val)
{
	std::ostringstream strs;
	strs << val;
	std::string str = strs.str();

	return str;
}

int Utility::string2int(string val)
{
	return atoi(val.c_str());
}

string Utility::GetConfigValue(string section, string param)
{
	INIReader reader("agent.conf");

	if (reader.ParseError() < 0) {
		FILE_LOG(logERROR) << L"Can't load agent.conf";
		return "";
	}
	
	FILE_LOG(logINFO) << "Config loaded from 'agent.conf': version=" << reader.Get("agent", "version", "");

	return reader.Get(section, param, "");
}

size_t Utility::ReadCallback(void *dest, size_t size, size_t nmemb, void *userp)
{
	struct WriteThis *wt = (struct WriteThis *)userp;
	size_t buffer_size = size*nmemb;

	if (wt->sizeleft) 
	{
		/* copy as much as possible from the source to the destination */
		size_t copy_this_much = wt->sizeleft;
		if (copy_this_much > buffer_size)
			copy_this_much = buffer_size;
		memcpy(dest, wt->readptr, copy_this_much);

		wt->readptr += copy_this_much;
		wt->sizeleft -= copy_this_much;

		return copy_this_much; /* we copied this many bytes */
	}

	return 0; /* no more data left to deliver */
}

#define USE_CHUNKED    0
#define DISABLE_EXPECT 1

int Utility::PostData(string url, string data)
{
	CURL *curl;
	CURLcode res;

	struct WriteThis wt;
	
	wt.readptr = data.c_str();
	wt.sizeleft = strlen(data.c_str());

	struct curl_slist *list = NULL;
	list = curl_slist_append(list, "Accept: application/json");
	list = curl_slist_append(list, "Content-Type: application/json");

	/* In windows, this will init the winsock stuff */
	res = curl_global_init(CURL_GLOBAL_DEFAULT);

	/* Check for errors */
	if (res != CURLE_OK)
	{
		FILE_LOG(logERROR) << "curl_global_init() failed with error: " << curl_easy_strerror(res);
		return 1;
	}
	
	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());		
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); /* if url is redirected, so we tell LibCurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_POST, 1L); /* Now specify we want to POST data */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback); /* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READDATA, &wt); /* pointer to pass to our read function */
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); /* get verbose debug output please */
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

		/* SSL Options */
//		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
//		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);

		/* Provide CA Certs from http://curl.haxx.se/docs/caextract.html */
//		curl_easy_setopt(curl, CURLOPT_CAINFO, "ca-bundle.crt");

		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)wt.sizeleft);

		res = curl_easy_perform(curl); /* Perform the request, res will get the return code */

		/* Check for errors */
		if (res != CURLE_OK)
		{
			FILE_LOG(logERROR) << "curl_easy_perform() failed with error: " << curl_easy_strerror(res);
		}

		/* Always cleanup */
		curl_slist_free_all(list);
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	return 0;
}

// Return time difference in units of 100 us.
_int64 Utility::TimeDelta(const SYSTEMTIME st1, const SYSTEMTIME st2)
{
	union timeunion {
		FILETIME fileTime;
		ULARGE_INTEGER ul;
	};

	timeunion ft1;
	timeunion ft2;

	SystemTimeToFileTime(&st1, &ft1.fileTime);
	SystemTimeToFileTime(&st2, &ft2.fileTime);

	return ft2.ul.QuadPart - ft1.ul.QuadPart;
}