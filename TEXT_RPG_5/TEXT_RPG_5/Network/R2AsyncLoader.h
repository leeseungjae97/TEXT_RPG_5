#pragma once

#include <future>

#include "../pch.h"
#include "../Struct/R2Result.h"

class R2Connector;

enum class ER2AsyncRequestType
{
	None,
	ReadLeaderboard,
	WriteLeaderboard
};

class R2AsyncLoader
{
public:
	R2AsyncLoader();
	~R2AsyncLoader();

public:
	void StartReadLeaderboard(R2Connector& Connector);

	void StartWriteLeaderboard(
		R2Connector& Connector,
		const string& Name,
		float Time,
		int Level = 1);

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

	ER2AsyncRequestType GetRequestType() const
	{
		return m_RequestType;
	}

	void ClearResult();

private:
	bool m_IsLoading = false;
	bool m_HasResult = false;

	ER2AsyncRequestType m_RequestType = ER2AsyncRequestType::None;

	R2LoadResult m_Result;
	future<R2LoadResult> m_Future;
};
