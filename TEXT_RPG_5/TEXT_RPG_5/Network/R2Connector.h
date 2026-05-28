#pragma once
#include "../Singleton.h"
#include "../pch.h"
#include "../Struct/R2Config.h"
#include <aws/s3/S3Client.h>

class R2Connector : public Singleton<R2Connector>
{
public:
	R2Connector();
	~R2Connector();
public:
	string ReadLeaderboard();
	void WriteLeaderboard(string Name, float Time);
	
private:
	string Trim(const string& text);
	bool LoadR2ConfigFromIni(const string& iniPath, R2Config& outConfig);
	bool PutTextToR2(
	Aws::S3::S3Client& s3Client,
	const R2Config& config,
	const string& objectKey,
	const string& text);
	bool GetTextFromR2(
	Aws::S3::S3Client& s3Client,
	const R2Config& config,
	const string& objectKey,
	string& outText);
	
	string CreateNewPlayerIniText();
	string UpdatePlayerIniText(const string& oldText);
	
	bool WriteOrUpdateIniOnR2(
	Aws::S3::S3Client& s3Client,
	const R2Config& config,
	const string& objectKey);
	
public:
	
private:
	R2Config Config;

	const string ConfigPath = "../../r2Config.ini";

	// R2 Bucket 안에 저장될 파일 이름
	const string ObjectKey = "text-rpg-leaderboard.txt";
};
