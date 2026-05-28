# pragma once
#include <future>

#include "../pch.h"
#include "../Struct/R2Config.h"
#include "../Struct/R2Result.h"
#include <aws/s3/S3Client.h>

class R2AsyncLoader
{
public:
	void StartLoadIni(
		Aws::S3::S3Client& s3Client,
		const R2Config& config,
		const string& objectKey);

	void Tick();

	bool IsLoading() const
	{
		return m_IsLoading;
	}

	bool HasResult() const
	{
		return m_HasResult;
	}

	bool IsSuccess() const
	{
		return m_HasResult && m_Result.Success;
	}

	const R2LoadResult& GetResult() const
	{
		return m_Result;
	}

private:
	bool m_IsLoading = false;
	bool m_HasResult = false;

	R2LoadResult m_Result;
	future<R2LoadResult> m_Future;
}; 
