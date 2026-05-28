#include "R2Connector.h"

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>

#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>

#include "R2AsyncLoader.h"

// Windows.h를 다른 곳에서 include하고 있다면 GetObject 매크로 충돌 방지
#ifdef GetObject
#undef GetObject
#endif


R2Connector::R2Connector()
{
	
}

R2Connector::~R2Connector()
{
	
}

string R2Connector::ReadLeaderboard() 
{
    R2Config config;

    if (!LoadR2ConfigFromIni(ConfigPath, config))
    {
        // 설정 실패
    }

    Aws::SDKOptions options;
    Aws::InitAPI(options);

    Aws::Client::ClientConfiguration clientConfig;

    clientConfig.endpointOverride = config.Endpoint;
    clientConfig.region = "auto";
    clientConfig.scheme = Aws::Http::Scheme::HTTPS;

    Aws::Auth::AWSCredentials credentials(
        config.AccessKeyId.c_str(),
        config.SecretAccessKey.c_str());

    Aws::S3::S3Client s3Client(
        credentials,
        clientConfig,
        Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never,
        false);

    string readBackText;
    
    // 읽어오기
    if (!GetTextFromR2(s3Client, config, ObjectKey, readBackText))
    {
        Aws::ShutdownAPI(options);
        return "";
    }
    Aws::ShutdownAPI(options);
    return readBackText;
}

void R2Connector::WriteLeaderboard(string Name, float Time, int Level)
{
    R2Config config;

    if (!LoadR2ConfigFromIni(ConfigPath, config))
    {
        // 설정 실패
    }

    Aws::SDKOptions options;
    Aws::InitAPI(options);

    Aws::Client::ClientConfiguration clientConfig;

    clientConfig.endpointOverride = config.Endpoint;
    clientConfig.region = "auto";
    clientConfig.scheme = Aws::Http::Scheme::HTTPS;

    Aws::Auth::AWSCredentials credentials(
        config.AccessKeyId.c_str(),
        config.SecretAccessKey.c_str());

    Aws::S3::S3Client s3Client(
        credentials,
        clientConfig,
        Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never,
        false);

    // 1. R2의 text-rpg-leaderboard.txt를 읽어본다.
    // 2. 있으면 기존 내용 기반으로 수정한다.
    // 3. 없으면 새 ini 내용을 만든다.
    // 4. PutObject로 R2에 저장한다.
    if (!WriteOrUpdateIniOnR2(s3Client, config, ObjectKey, Name, Time, Level))
    {
        Aws::ShutdownAPI(options);
        // 쓰기 실패
        return;
    }

    Aws::ShutdownAPI(options);
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

bool R2Connector::LoadR2ConfigFromIni(const string& iniPath, R2Config& outConfig)
{
    ifstream file(iniPath.c_str());

    if (!file.is_open())
    {
        cerr << "Failed to open config ini: " << iniPath << '\n';
        return false;
    }

    unordered_map<string, string> values;

    string currentSection;
    string line;

    while (getline(file, line))
    {
        line = Trim(line);

        if (line.empty())
        {
            continue;
        }

        if (line[0] == ';' || line[0] == '#')
        {
            continue;
        }

        if (line.front() == '[' && line.back() == ']')
        {
            currentSection = Trim(line.substr(1, line.size() - 2));
            continue;
        }

        if (currentSection != "R2")
        {
            continue;
        }

        const size_t equalPos = line.find('=');

        if (equalPos == string::npos)
        {
            continue;
        }

        const string key = Trim(line.substr(0, equalPos));
        const string value = Trim(line.substr(equalPos + 1));

        values[key] = value;
    }

    outConfig.AccountId = values["AccountId"];
    outConfig.AccessKeyId = values["AccessKeyId"];
    outConfig.SecretAccessKey = values["SecretAccessKey"];
    outConfig.BucketName = values["BucketName"];
    outConfig.Endpoint = values["Endpoint"];

    if (outConfig.AccessKeyId.empty() ||
        outConfig.SecretAccessKey.empty() ||
        outConfig.BucketName.empty() ||
        outConfig.Endpoint.empty())
    {
        cerr << "R2 config is incomplete.\n";
        cerr << "Required: AccessKeyId, SecretAccessKey, BucketName, Endpoint\n";
        return false;
    }

    return true;
}

bool R2Connector::PutTextToR2(
    Aws::S3::S3Client& s3Client,
    const R2Config& config,
    const string& objectKey,
    const string& text)
{
    Aws::S3::Model::PutObjectRequest request;

    request.SetBucket(config.BucketName.c_str());
    request.SetKey(objectKey.c_str());
    request.SetContentType("text/plain; charset=utf-8");

    shared_ptr<Aws::StringStream> bodyStream =
        Aws::MakeShared<Aws::StringStream>("R2PutObjectBodyStream");

    (*bodyStream) << text;

    request.SetBody(bodyStream);

    Aws::S3::Model::PutObjectOutcome outcome = s3Client.PutObject(request);

    if (!outcome.IsSuccess())
    {
        cerr << "PutObject failed.\n";
        cerr << "Error: "
                  << outcome.GetError().GetExceptionName()
                  << " - "
                  << outcome.GetError().GetMessage()
                  << '\n';

        return false;
    }

    cout << "R2 write success: " << objectKey << '\n';

    return true;
}

bool R2Connector::GetTextFromR2(
    Aws::S3::S3Client& s3Client,
    const R2Config& config,
    const string& objectKey,
    string& outText)
{
    Aws::S3::Model::GetObjectRequest request;

    request.SetBucket(config.BucketName.c_str());
    request.SetKey(objectKey.c_str());

    Aws::S3::Model::GetObjectOutcome outcome = s3Client.GetObject(request);

    if (!outcome.IsSuccess())
    {
        cerr << "GetObject failed.\n";
        cerr << "ObjectKey: " << objectKey << '\n';
        cerr << "Error: "
                  << outcome.GetError().GetExceptionName()
                  << " - "
                  << outcome.GetError().GetMessage()
                  << '\n';

        return false;
    }

    Aws::S3::Model::GetObjectResult result = outcome.GetResultWithOwnership();

    stringstream ss;
    ss << result.GetBody().rdbuf();

    outText = ss.str();

    cout << "R2 read success: " << objectKey << '\n';

    return true;
}

string R2Connector::CreateLeaderboardText(const string& oldText, const string& Name, float Time, int Level)
{
    ostringstream oss;
    oss << oldText;
    if (!oldText.empty() && oldText.back() != '\n')
    {
        oss << '\n';
    }

    int nextRank = 1;
    stringstream stream(oldText);
    string line;
    while (getline(stream, line))
    {
        if (!Trim(line).empty())
        {
            ++nextRank;
        }
    }

    oss << nextRank << ". " << fixed << setprecision(3) << Time << ", " << Level << ", " << Name << '\n';

    return oss.str();
}

bool R2Connector::WriteOrUpdateIniOnR2(
    Aws::S3::S3Client& s3Client,
    const R2Config& config,
    const string& objectKey,
    const string& Name,
    float Time,
    int Level)
{
    string oldText;
    string newText;

    const bool readSuccess = GetTextFromR2(
        s3Client,
        config,
        objectKey,
        oldText);

    newText = CreateLeaderboardText(readSuccess ? oldText : "", Name, Time, Level);

    return PutTextToR2(
        s3Client,
        config,
        objectKey,
        newText);
}
