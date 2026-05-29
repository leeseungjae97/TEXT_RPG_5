#pragma once

#include "../Singleton.h"
#include "../pch.h"
#include "../Struct/R2Result.h"

class R2Connector : public Singleton<R2Connector>
{
public:
	R2Connector();
	~R2Connector();

public:
	// GET /leaderboard
	string ReadLeaderboard();

	// POST /record
	bool WriteLeaderboard(string Name, float Time, int Level = 1);

	// 필요하면 외부에서 Worker 주소/토큰 교체 가능
	void SetWorkerBaseUrl(const string& InBaseUrl);
	void SetApiToken(const string& InApiToken);

private:
	struct HttpResponse
	{
		bool Success = false;
		int StatusCode = 0;
		string Body;
		string ErrorMessage;
	};

private:
	static string Trim(const string& text);
	static string EscapeJsonString(const string& text);
	static string FormatTime(float TimeSeconds);

	HttpResponse SendHttpRequest(
		const string& Method,
		const string& Url,
		const string& Body,
		const vector<pair<string, string>>& Headers) const;

	R2LoadResult RequestLeaderboard() const;
	R2LoadResult PostRecord(const string& Name, float Time, int Level) const;

private:
	// 배포된 Worker URL로 교체하세요.
	// 로컬 테스트:
	// http://localhost:8787
	string WorkerBaseUrl = "http://localhost:8787";

	// 주의:
	// public repository에는 실제 토큰을 커밋하지 마세요.
	// 배포용 exe에 넣으면 완전한 비밀은 아니지만,
	// R2 Secret Key를 넣는 것보다는 훨씬 안전합니다.
	string ApiToken = "YOUR_API_TOKEN";
};
