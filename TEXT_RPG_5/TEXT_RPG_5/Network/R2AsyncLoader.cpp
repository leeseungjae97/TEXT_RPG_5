#include "R2AsyncLoader.h"
#include "R2Connector.h"

R2AsyncLoader::R2AsyncLoader()
{
}

R2AsyncLoader::~R2AsyncLoader()
{
}

void R2AsyncLoader::StartReadLeaderboard(R2Connector& Connector)
{
	if (m_IsLoading)
	{
		cout << "R2AsyncLoader is already loading.\n";
		return;
	}

	m_IsLoading = true;
	m_HasResult = false;
	m_RequestType = ER2AsyncRequestType::ReadLeaderboard;
	m_Result = R2LoadResult();

	m_Future = async(
		launch::async,
		[&Connector]()
		{
			R2LoadResult result;
			result.ObjectKey = "GET /leaderboard";

			const string text = Connector.ReadLeaderboard();

			if (text.empty())
			{
				// 리더보드가 진짜 빈 파일일 수도 있으므로,
				// 빈 문자열을 무조건 실패로 보는 게 애매합니다.
				// 여기서는 HTTP 실패 여부를 Connector 내부에서 로그로 확인하고,
				// 결과 자체는 성공 처리합니다.
				result.Success = true;
				result.Text = "";
				return result;
			}

			result.Success = true;
			result.Text = text;

			return result;
		});
}

void R2AsyncLoader::StartWriteLeaderboard(
	R2Connector& Connector,
	const string& Name,
	float Time,
	int Level)
{
	if (m_IsLoading)
	{
		cout << "R2AsyncLoader is already loading.\n";
		return;
	}

	m_IsLoading = true;
	m_HasResult = false;
	m_RequestType = ER2AsyncRequestType::WriteLeaderboard;
	m_Result = R2LoadResult();

	m_Future = async(
		launch::async,
		[&Connector, Name, Time, Level]()
		{
			R2LoadResult result;
			result.ObjectKey = "POST /record";

			const bool success = Connector.WriteLeaderboard(Name, Time, Level);

			result.Success = success;

			if (!success)
			{
				result.ErrorMessage = "POST /record failed.";
			}

			return result;
		});
}

void R2AsyncLoader::Tick()
{
	if (!m_IsLoading)
	{
		return;
	}

	const future_status status = m_Future.wait_for(chrono::milliseconds(0));

	if (status != future_status::ready)
	{
		return;
	}

	m_Result = m_Future.get();

	m_IsLoading = false;
	m_HasResult = true;
}

void R2AsyncLoader::ClearResult()
{
	if (m_IsLoading)
	{
		return;
	}

	m_HasResult = false;
	m_RequestType = ER2AsyncRequestType::None;
	m_Result = R2LoadResult();
}
