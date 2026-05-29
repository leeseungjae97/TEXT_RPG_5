#include "R2Connector.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

#include <iomanip>
#include <sstream>

R2Connector::R2Connector()
{
	SetWorkerBaseUrl("https://text-rpg-r2-worker.dltmdwo2323.workers.dev");
	SetApiToken("text-rpg-api-key-9898");
}

R2Connector::~R2Connector()
{
}

void R2Connector::SetWorkerBaseUrl(const string& InBaseUrl)
{
	WorkerBaseUrl = InBaseUrl;
}

void R2Connector::SetApiToken(const string& InApiToken)
{
	ApiToken = InApiToken;
}

string R2Connector::ReadLeaderboard()
{
	R2LoadResult result = RequestLeaderboard();

	if (!result.Success)
	{
		cout << "ReadLeaderboard failed: " << result.ErrorMessage << '\n';
		return "";
	}

	return result.Text;
}

bool R2Connector::WriteLeaderboard(string Name, float Time, int Level)
{
	R2LoadResult result = PostRecord(Name, Time, Level);

	if (!result.Success)
	{
		cout << "WriteLeaderboard failed: " << result.ErrorMessage << '\n';
		return false;
	}

	return true;
}

string R2Connector::Trim(const string& text)
{
	const char* whiteSpaces = " \t\r\n";

	const size_t begin = text.find_first_not_of(whiteSpaces);

	if (begin == string::npos)
	{
		return "";
	}

	const size_t end = text.find_last_not_of(whiteSpaces);

	return text.substr(begin, end - begin + 1);
}

string R2Connector::EscapeJsonString(const string& text)
{
	ostringstream oss;

	for (size_t i = 0; i < text.size(); ++i)
	{
		const unsigned char c = static_cast<unsigned char>(text[i]);

		switch (c)
		{
		case '\"':
			oss << "\\\"";
			break;

		case '\\':
			oss << "\\\\";
			break;

		case '\b':
			oss << "\\b";
			break;

		case '\f':
			oss << "\\f";
			break;

		case '\n':
			oss << "\\n";
			break;

		case '\r':
			oss << "\\r";
			break;

		case '\t':
			oss << "\\t";
			break;

		default:
			if (c < 0x20)
			{
				oss << "\\u"
					<< hex
					<< setw(4)
					<< setfill('0')
					<< static_cast<int>(c)
					<< dec
					<< setfill(' ');
			}
			else
			{
				oss << text[i];
			}

			break;
		}
	}

	return oss.str();
}

string R2Connector::FormatTime(float TimeSeconds)
{
	if (TimeSeconds < 0.0f)
	{
		TimeSeconds = 0.0f;
	}

	const int totalMilliseconds = static_cast<int>(TimeSeconds * 1000.0f + 0.5f);

	const int milliseconds = totalMilliseconds % 1000;
	const int totalSeconds = totalMilliseconds / 1000;

	const int seconds = totalSeconds % 60;
	const int totalMinutes = totalSeconds / 60;

	const int minutes = totalMinutes % 60;
	const int hours = totalMinutes / 60;

	ostringstream oss;

	oss << setw(2) << setfill('0') << hours << ":"
		<< setw(2) << setfill('0') << minutes << ":"
		<< setw(2) << setfill('0') << seconds << "."
		<< setw(3) << setfill('0') << milliseconds;

	return oss.str();
}

static wstring Utf8ToWide(const string& text)
{
	if (text.empty())
	{
		return L"";
	}

	const int requiredSize = MultiByteToWideChar(
		CP_UTF8,
		0,
		text.c_str(),
		static_cast<int>(text.size()),
		nullptr,
		0);

	if (requiredSize <= 0)
	{
		return L"";
	}

	wstring result;
	result.resize(requiredSize);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		text.c_str(),
		static_cast<int>(text.size()),
		&result[0],
		requiredSize);

	return result;
}

static string WideToUtf8(const wstring& text)
{
	if (text.empty())
	{
		return "";
	}

	const int requiredSize = WideCharToMultiByte(
		CP_UTF8,
		0,
		text.c_str(),
		static_cast<int>(text.size()),
		nullptr,
		0,
		nullptr,
		nullptr);

	if (requiredSize <= 0)
	{
		return "";
	}

	string result;
	result.resize(requiredSize);

	WideCharToMultiByte(
		CP_UTF8,
		0,
		text.c_str(),
		static_cast<int>(text.size()),
		&result[0],
		requiredSize,
		nullptr,
		nullptr);

	return result;
}

R2Connector::HttpResponse R2Connector::SendHttpRequest(
	const string& Method,
	const string& Url,
	const string& Body,
	const vector<pair<string, string>>& Headers) const
{
	HttpResponse response;

	const wstring wideUrl = Utf8ToWide(Url);

	URL_COMPONENTS urlComponents;
	ZeroMemory(&urlComponents, sizeof(urlComponents));

	wchar_t hostName[256];
	wchar_t urlPath[2048];

	ZeroMemory(hostName, sizeof(hostName));
	ZeroMemory(urlPath, sizeof(urlPath));

	urlComponents.dwStructSize = sizeof(urlComponents);
	urlComponents.lpszHostName = hostName;
	urlComponents.dwHostNameLength = _countof(hostName);
	urlComponents.lpszUrlPath = urlPath;
	urlComponents.dwUrlPathLength = _countof(urlPath);

	if (!WinHttpCrackUrl(
		wideUrl.c_str(),
		static_cast<DWORD>(wideUrl.length()),
		0,
		&urlComponents))
	{
		response.ErrorMessage = "WinHttpCrackUrl failed.";
		return response;
	}

	const bool isHttps = urlComponents.nScheme == INTERNET_SCHEME_HTTPS;

	HINTERNET session = WinHttpOpen(
		L"TextRPG-Worker-Client/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0);

	if (!session)
	{
		response.ErrorMessage = "WinHttpOpen failed.";
		return response;
	}

	HINTERNET connection = WinHttpConnect(
		session,
		urlComponents.lpszHostName,
		urlComponents.nPort,
		0);

	if (!connection)
	{
		response.ErrorMessage = "WinHttpConnect failed.";
		WinHttpCloseHandle(session);
		return response;
	}

	const DWORD requestFlags = isHttps ? WINHTTP_FLAG_SECURE : 0;

	HINTERNET request = WinHttpOpenRequest(
		connection,
		Utf8ToWide(Method).c_str(),
		urlComponents.lpszUrlPath,
		nullptr,
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		requestFlags);

	if (!request)
	{
		response.ErrorMessage = "WinHttpOpenRequest failed.";
		WinHttpCloseHandle(connection);
		WinHttpCloseHandle(session);
		return response;
	}

	wstring headerText;

	for (size_t i = 0; i < Headers.size(); ++i)
	{
		headerText += Utf8ToWide(Headers[i].first);
		headerText += L": ";
		headerText += Utf8ToWide(Headers[i].second);
		headerText += L"\r\n";
	}

	const void* bodyData = Body.empty() ? WINHTTP_NO_REQUEST_DATA : Body.data();
	const DWORD bodySize = static_cast<DWORD>(Body.size());

	const BOOL sendResult = WinHttpSendRequest(
		request,
		headerText.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : headerText.c_str(),
		headerText.empty() ? 0 : static_cast<DWORD>(headerText.length()),
		const_cast<void*>(bodyData),
		bodySize,
		bodySize,
		0);

	if (!sendResult)
	{
		response.ErrorMessage = "WinHttpSendRequest failed.";
		WinHttpCloseHandle(request);
		WinHttpCloseHandle(connection);
		WinHttpCloseHandle(session);
		return response;
	}

	if (!WinHttpReceiveResponse(request, nullptr))
	{
		response.ErrorMessage = "WinHttpReceiveResponse failed.";
		WinHttpCloseHandle(request);
		WinHttpCloseHandle(connection);
		WinHttpCloseHandle(session);
		return response;
	}

	DWORD statusCode = 0;
	DWORD statusCodeSize = sizeof(statusCode);

	if (WinHttpQueryHeaders(
		request,
		WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&statusCode,
		&statusCodeSize,
		WINHTTP_NO_HEADER_INDEX))
	{
		response.StatusCode = static_cast<int>(statusCode);
	}

	string responseBody;

	while (true)
	{
		DWORD bytesAvailable = 0;

		if (!WinHttpQueryDataAvailable(request, &bytesAvailable))
		{
			response.ErrorMessage = "WinHttpQueryDataAvailable failed.";
			break;
		}

		if (bytesAvailable == 0)
		{
			break;
		}

		string buffer;
		buffer.resize(bytesAvailable);

		DWORD bytesRead = 0;

		if (!WinHttpReadData(
			request,
			&buffer[0],
			bytesAvailable,
			&bytesRead))
		{
			response.ErrorMessage = "WinHttpReadData failed.";
			break;
		}

		buffer.resize(bytesRead);
		responseBody += buffer;
	}

	WinHttpCloseHandle(request);
	WinHttpCloseHandle(connection);
	WinHttpCloseHandle(session);

	response.Body = responseBody;
	response.Success = response.StatusCode >= 200 && response.StatusCode < 300;

	if (!response.Success && response.ErrorMessage.empty())
	{
		ostringstream oss;
		oss << "HTTP request failed. StatusCode: " << response.StatusCode
			<< ", Body: " << response.Body;

		response.ErrorMessage = oss.str();
	}

	return response;
}

R2LoadResult R2Connector::RequestLeaderboard() const
{
	R2LoadResult result;
	result.ObjectKey = "GET /leaderboard";

	const string url = WorkerBaseUrl + "/leaderboard";

	vector<pair<string, string>> headers;
	headers.push_back(make_pair("Accept", "text/plain"));

	const HttpResponse response = SendHttpRequest(
		"GET",
		url,
		"",
		headers);

	if (!response.Success)
	{
		result.Success = false;
		result.ErrorMessage = response.ErrorMessage;
		return result;
	}

	result.Success = true;
	result.Text = response.Body;

	return result;
}

R2LoadResult R2Connector::PostRecord(const string& Name, float Time, int Level) const
{
	R2LoadResult result;
	result.ObjectKey = "POST /record";

	const string timeText = FormatTime(Time);

	ostringstream body;

	body << "{";
	body << "\"time\":\"" << timeText << "\",";
	body << "\"level\":" << Level << ",";
	body << "\"name\":\"" << EscapeJsonString(Name) << "\"";
	body << "}";

	const string url = WorkerBaseUrl + "/record";

	vector<pair<string, string>> headers;
	headers.push_back(make_pair("Content-Type", "application/json"));
	headers.push_back(make_pair("Accept", "application/json"));
	headers.push_back(make_pair("Authorization", "Bearer " + ApiToken));

	const HttpResponse response = SendHttpRequest(
		"POST",
		url,
		body.str(),
		headers);

	if (!response.Success)
	{
		result.Success = false;
		result.ErrorMessage = response.ErrorMessage;
		return result;
	}

	result.Success = true;
	result.Text = response.Body;

	return result;
}
