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
    }
    
    R2AsyncLoader loader;

    loader.StartLoadIni(
        s3Client,
        config,
        ObjectKey);
    
    while (true)
    {
        loader.Tick();

        if (loader.IsLoading())
        {
            std::cout << "Loading R2 object";
            std::cout << ".\n";

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            continue;
        }

        if (loader.HasResult())
        {
            const R2LoadResult& result = loader.GetResult();

            // if (result.Success)
            // {
            //     std::cout << "\nR2 Load Success\n";
            //     std::cout << "ObjectKey: " << result.ObjectKey << '\n';
            //     std::cout << "\n========== Content ==========\n";
            //     std::cout << result.Text << '\n';
            //     std::cout << "=============================\n";
            // }
            // else
            // {
            //     std::cout << "\nR2 Load Failed\n";
            //     std::cout << "ObjectKey: " << result.ObjectKey << '\n';
            //     std::cout << "Error: " << result.ErrorMessage << '\n';
            // }

            break;
        }
    }
    
    return readBackText;
}

void R2Connector::WriteLeaderboard(string Name, float Time)
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
    if (!WriteOrUpdateIniOnR2(s3Client, config, ObjectKey))
    {
        Aws::ShutdownAPI(options);
        // 쓰기 실패
    }
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

string R2Connector::CreateNewPlayerIniText()
{
    ostringstream oss;

    oss << "[Player]\n";
    oss << "Name=Knight\n";
    oss << "Level=1\n";
    oss << "HP=100\n";
    oss << "Gold=0\n";
    oss << "\n";
    oss << "[LastSave]\n";
    oss << "Stage=1\n";
    oss << "PlayTime=0\n";

    return oss.str();
}

string R2Connector::UpdatePlayerIniText(const string& oldText)
{
    ostringstream oss;

    // 단순 예시:
    // 기존 내용을 보존하고 아래에 새 저장 섹션을 추가합니다.
    // 실제 게임에서는 ini 파서를 만들어 특정 key만 교체하는 방식이 더 좋습니다.
    oss << oldText;

    if (!oldText.empty() && oldText[oldText.size() - 1] != '\n')
    {
        oss << "\n";
    }

    oss << "\n";
    oss << "; Updated by C++ Client\n";
    oss << "[LastSave]\n";
    oss << "Stage=3\n";
    oss << "PlayTime=120\n";
    oss << "Gold=5000\n";

    return oss.str();
}

bool R2Connector::WriteOrUpdateIniOnR2(
    Aws::S3::S3Client& s3Client,
    const R2Config& config,
    const string& objectKey)
{
    string oldText;
    string newText;

    const bool readSuccess = GetTextFromR2(
        s3Client,
        config,
        objectKey,
        oldText);

    if (readSuccess)
    {
        newText = UpdatePlayerIniText(oldText);
    }
    else
    {
        newText = CreateNewPlayerIniText();
    }

    return PutTextToR2(
        s3Client,
        config,
        objectKey,
        newText);
}

