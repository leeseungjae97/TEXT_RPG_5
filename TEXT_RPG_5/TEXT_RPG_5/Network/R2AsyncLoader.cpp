// #include "R2AsyncLoader.h"
//
// #include <aws/s3/S3Client.h>
// #include <aws/s3/S3ServiceClientModel.h>
// #include <aws/s3/model/GetObjectRequest.h>
// #include <aws/s3/model/GetObjectResult.h>
// #include <aws/s3/model/PutObjectRequest.h>
//
//
// static string Trim(const string& text)
// {
//     const char* whiteSpaces = " \t\r\n";
//
//     const size_t begin = text.find_first_not_of(whiteSpaces);
//
//     if (begin == string::npos)
//     {
//         return "";
//     }
//
//     const size_t end = text.find_last_not_of(whiteSpaces);
//
//     return text.substr(begin, end - begin + 1);
// }
//
// static bool LoadR2ConfigFromIni(const string& iniPath, R2Config& outConfig)
// {
//     std::ifstream file(iniPath.c_str());
//
//     if (!file.is_open())
//     {
//         std::cerr << "Failed to open config ini: " << iniPath << '\n';
//         return false;
//     }
//
//     std::unordered_map<string, string> values;
//
//     string currentSection;
//     string line;
//
//     while (std::getline(file, line))
//     {
//         line = Trim(line);
//
//         if (line.empty())
//         {
//             continue;
//         }
//
//         if (line[0] == ';' || line[0] == '#')
//         {
//             continue;
//         }
//
//         if (line.front() == '[' && line.back() == ']')
//         {
//             currentSection = Trim(line.substr(1, line.size() - 2));
//             continue;
//         }
//
//         if (currentSection != "R2")
//         {
//             continue;
//         }
//
//         const size_t equalPos = line.find('=');
//
//         if (equalPos == string::npos)
//         {
//             continue;
//         }
//
//         const string key = Trim(line.substr(0, equalPos));
//         const string value = Trim(line.substr(equalPos + 1));
//
//         values[key] = value;
//     }
//
//     outConfig.AccountId = values["AccountId"];
//     outConfig.AccessKeyId = values["AccessKeyId"];
//     outConfig.SecretAccessKey = values["SecretAccessKey"];
//     outConfig.BucketName = values["BucketName"];
//     outConfig.Endpoint = values["Endpoint"];
//
//     if (outConfig.AccessKeyId.empty() ||
//         outConfig.SecretAccessKey.empty() ||
//         outConfig.BucketName.empty() ||
//         outConfig.Endpoint.empty())
//     {
//         std::cerr << "R2 config is incomplete.\n";
//         return false;
//     }
//
//     return true;
// }
//
// static bool PutTextToR2(
//     Aws::S3::S3Client& s3Client,
//     const R2Config& config,
//     const string& objectKey,
//     const string& text)
// {
//     Aws::S3::Model::PutObjectRequest request;
//
//     request.SetBucket(config.BucketName.c_str());
//     request.SetKey(objectKey.c_str());
//     request.SetContentType("text/plain; charset=utf-8");
//
//     std::shared_ptr<Aws::StringStream> bodyStream =
//         Aws::MakeShared<Aws::StringStream>("R2PutObjectBodyStream");
//
//     (*bodyStream) << text;
//
//     request.SetBody(bodyStream);
//
//     Aws::S3::Model::PutObjectOutcome outcome = s3Client.PutObject(request);
//
//     if (!outcome.IsSuccess())
//     {
//         std::cerr << "PutObject failed.\n";
//         std::cerr << outcome.GetError().GetExceptionName()
//                   << " - "
//                   << outcome.GetError().GetMessage()
//                   << '\n';
//
//         return false;
//     }
//
//     return true;
// }
//
// static R2LoadResult GetTextFromR2Blocking(
//     Aws::S3::S3Client& s3Client,
//     const R2Config& config,
//     const string& objectKey)
// {
//     R2LoadResult result;
//     result.ObjectKey = objectKey;
//
//     Aws::S3::Model::GetObjectRequest request;
//
//     request.SetBucket(config.BucketName.c_str());
//     request.SetKey(objectKey.c_str());
//
//     Aws::S3::Model::GetObjectOutcome outcome = s3Client.GetObject(request);
//
//     if (!outcome.IsSuccess())
//     {
//         result.Success = false;
//
//         result.ErrorMessage =
//             outcome.GetError().GetExceptionName() +
//             string(" - ") +
//             outcome.GetError().GetMessage();
//
//         return result;
//     }
//
//     Aws::S3::Model::GetObjectResult getResult = outcome.GetResultWithOwnership();
//
//     std::stringstream ss;
//     ss << getResult.GetBody().rdbuf();
//
//     result.Text = ss.str();
//     result.Success = true;
//
//     return result;
// }
//
// void R2AsyncLoader::StartLoadIni(Aws::S3::S3Client& s3Client, const R2Config& config, const string& objectKey)
// {
// 	if (m_IsLoading)
// 	{
// 		cout << "Already loading.\n";
// 		return;
// 	}
//
// 	m_IsLoading = true;
// 	m_HasResult = false;
// 	m_Result = R2LoadResult();
//
// 	// m_Future = async(
// 	// 	launch::async,
// 	// 	[&s3Client, config, objectKey]()
// 	// 	{
// 	// 		return GetTextFromR2Blocking(
// 	// 			s3Client,
// 	// 			config,
// 	// 			objectKey);
// 	// 	});
// }
//
// void R2AsyncLoader::Tick()
// {
// 	if (!m_IsLoading)
// 	{
// 		return;
// 	}
//
// 	const future_status status =
// 		m_Future.wait_for(chrono::milliseconds(0));
//
// 	if (status != future_status::ready)
// 	{
// 		return;
// 	}
//
// 	m_Result = m_Future.get();
//
// 	m_IsLoading = false;
// 	m_HasResult = true;
// }
