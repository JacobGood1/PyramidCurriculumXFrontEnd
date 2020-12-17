/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Leaderboards/EOSLeaderboards.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreLeaderboards* UCoreLeaderboards::s_LeaderboardsObject = nullptr;

UCoreLeaderboards::UCoreLeaderboards()
{
	s_LeaderboardsObject = this;
}

void UCoreLeaderboards::EOSLeaderboardsQueryLeaderboardDefinitions(UObject* worldContextObject, FEOSLeaderboardsQueryLeaderboardDefinitionsOptions options, const FOnQueryLeaderboardDefinitionsCompleteCallback& callback)
{
	LogVerbose("");

	if (GetLeaderboardsHandle(worldContextObject))
	{
		EOS_Leaderboards_QueryLeaderboardDefinitionsOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;
		LeaderboardsOptions.StartTime = FCString::Atoi64(*options.StartTime);
		LeaderboardsOptions.EndTime = FCString::Atoi64(*options.EndTime);

		FLeaderboardDefinitionsCompleteCallback* CallbackObj = new FLeaderboardDefinitionsCompleteCallback({ this, callback });
		EOS_Leaderboards_QueryLeaderboardDefinitions(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions, CallbackObj, Internal_OnLeaderboardQueryLeaderboardDefinitionsCompleteCallback);
	}
}

int32 UCoreLeaderboards::EOSLeaderboardsGetLeaderboardDefinitionCount(UObject* worldContextObject, FEOSLeaderboardsGetLeaderboardDefinitionCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetLeaderboardsHandle(worldContextObject))
	{
		EOS_Leaderboards_GetLeaderboardDefinitionCountOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;

		Result = EOS_Leaderboards_GetLeaderboardDefinitionCount(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions);
	}

	return Result;
}

EOSResult UCoreLeaderboards::EOSLeaderboardsCopyLeaderboardDefinitionByIndex(UObject* worldContextObject, FEOSLeaderboardsCopyLeaderboardDefinitionByIndexOptions options, FEOSLeaderboardsDefinition& outLeaderboardDefinition)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLeaderboardsHandle(worldContextObject))
	{
		EOS_Leaderboards_CopyLeaderboardDefinitionByIndexOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;
		LeaderboardsOptions.LeaderboardIndex = static_cast<uint32_t>(options.LeaderboardIndex);

		EOS_Leaderboards_Definition* Definition = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Leaderboards_CopyLeaderboardDefinitionByIndex(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions, &Definition));

		if (Result == EOSResult::EOS_Success)
		{
			outLeaderboardDefinition = *Definition;
		}

		EOS_Leaderboards_Definition_Release(Definition);
	}

	return Result;
}

EOSResult UCoreLeaderboards::EOSLeaderboardsCopyLeaderboardDefinitionByLeaderboardId(UObject* worldContextObject, FEOSLeaderboardsCopyLeaderboardDefinitionByLeaderboardIdOptions options, FEOSLeaderboardsDefinition& outLeaderboardDefinition)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLeaderboardsHandle(worldContextObject))
	{
		char LeaderboardIdAnsi[64];
		FCStringAnsi::Strncpy(LeaderboardIdAnsi, TCHAR_TO_UTF8(*options.LeaderboardId), (options.LeaderboardId.Len() +1));

		EOS_Leaderboards_CopyLeaderboardDefinitionByLeaderboardIdOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;
		LeaderboardsOptions.LeaderboardId = LeaderboardIdAnsi;

		EOS_Leaderboards_Definition* Definition = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Leaderboards_CopyLeaderboardDefinitionByLeaderboardId(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions, &Definition));

		if (Result == EOSResult::EOS_Success)
		{
			outLeaderboardDefinition = *Definition;
		}

		EOS_Leaderboards_Definition_Release(Definition);
	}

	return Result;
}

void UCoreLeaderboards::EOSLeaderboardsQueryLeaderboardRanks(UObject* worldContextObject, FEOSLeaderboardsQueryLeaderboardRanksOptions options, const FOnQueryLeaderboardRanksCompleteCallback& callback)
{
	LogVerbose("");

	if (GetLeaderboardsHandle(worldContextObject))
	{
		char LeaderboardIdAnsi[64];
		FCStringAnsi::Strncpy(LeaderboardIdAnsi, TCHAR_TO_UTF8(*options.LeaderboardId), (options.LeaderboardId.Len() + 1));

		EOS_Leaderboards_QueryLeaderboardRanksOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;
		LeaderboardsOptions.LeaderboardId = LeaderboardIdAnsi;

		FLeaderboardRanksCompleteCallback* CallbackObj = new FLeaderboardRanksCompleteCallback({ this, callback });
		EOS_Leaderboards_QueryLeaderboardRanks(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions, CallbackObj, Internal_OnLeaderboardQueryLeaderboardRanksCompleteCallback);
	}
}

int32 UCoreLeaderboards::EOSLeaderboardsGetLeaderboardRecordCount(UObject* worldContextObject, FEOSLeaderboardsGetLeaderboardRecordCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetLeaderboardsHandle(worldContextObject))
	{
		EOS_Leaderboards_GetLeaderboardRecordCountOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;

		Result = EOS_Leaderboards_GetLeaderboardRecordCount(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions);
	}
	return Result;
}

EOSResult UCoreLeaderboards::EOSLeaderboardsCopyLeaderboardRecordByIndex(UObject* worldContextObject, FEOSLeaderboardsCopyLeaderboardRecordByIndexOptions options, FEOSLeaderboardsLeaderboardRecord& outLeaderboardRecord)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLeaderboardsHandle(worldContextObject))
	{
		EOS_Leaderboards_CopyLeaderboardRecordByIndexOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;
		LeaderboardsOptions.LeaderboardRecordIndex = static_cast<uint32_t>(options.LeaderboardRecordIndex);

		EOS_Leaderboards_LeaderboardRecord* LeaderboardRecord = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Leaderboards_CopyLeaderboardRecordByIndex(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions, &LeaderboardRecord));

		if (Result == EOSResult::EOS_Success)
		{
			outLeaderboardRecord = *LeaderboardRecord;
		}

		EOS_Leaderboards_LeaderboardRecord_Release(LeaderboardRecord);
	}

	return Result;
}

EOSResult UCoreLeaderboards::EOSLeaderboardsCopyLeaderboardRecordByUserId(UObject* worldContextObject, FEOSLeaderboardsCopyLeaderboardRecordByUserIdOptions options, FEOSLeaderboardsLeaderboardRecord& outLeaderboardRecord)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLeaderboardsHandle(worldContextObject))
	{
		EOS_Leaderboards_CopyLeaderboardRecordByUserIdOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;
		LeaderboardsOptions.UserId = options.UserId;

		EOS_Leaderboards_LeaderboardRecord* LeaderboardRecord = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Leaderboards_CopyLeaderboardRecordByUserId(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions, &LeaderboardRecord));

		if (Result == EOSResult::EOS_Success)
		{
			outLeaderboardRecord = *LeaderboardRecord;
		}

		EOS_Leaderboards_LeaderboardRecord_Release(LeaderboardRecord);
	}
	return Result;
}

void UCoreLeaderboards::EOSLeaderboardsQueryLeaderboardUserScores(UObject* worldContextObject, FEOSLeaderboardsQueryLeaderboardUserScoresOptions options, const FOnQueryLeaderboardUserScoresCompleteCallback& callback)
{
	LogVerbose("");

	if (GetLeaderboardsHandle(worldContextObject))
	{
		FQueryLeaderboardUserScoresOperation QueryLeaderboardUserScoresOperation(options.UserIds.Num(), options.UserIds.Num(), FCString::Atoi(*options.StartTime), FCString::Atoi(*options.EndTime));

		for (int32 i = 0; i < options.UserIds.Num(); i++)
		{
			QueryLeaderboardUserScoresOperation.UserIdsArray[i] = options.UserIds[i];
		}

		for (int32 i = 0; i < options.StatInfo.Num(); i++)
		{
			FCStringAnsi::Strncpy(const_cast<char*>(QueryLeaderboardUserScoresOperation.UserScoresArray[i].StatName), TCHAR_TO_UTF8(*options.StatInfo[i].StatName), 256);
			QueryLeaderboardUserScoresOperation.UserScoresArray[i].Aggregation = static_cast<EOS_ELeaderboardAggregation>(options.StatInfo[i].Aggregation);
		}

		FLeaderboardUserScoresCompleteCallback* CallbackObj = new FLeaderboardUserScoresCompleteCallback({ this, callback });
		EOS_Leaderboards_QueryLeaderboardUserScores(GetLeaderboardsHandle(worldContextObject), &QueryLeaderboardUserScoresOperation, CallbackObj, Internal_OnLeaderboardQueryLeaderboardUserScoresCompleteCallbackInfo);
	}
}

int32 UCoreLeaderboards::EOSLeaderboardsGetLeaderboardUserScoreCount(UObject* worldContextObject, FEOSLeaderboardsGetLeaderboardUserScoreCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetLeaderboardsHandle(worldContextObject))
	{
		char StatNameAnsi[64];
		FCStringAnsi::Strncpy(StatNameAnsi, TCHAR_TO_UTF8(*options.StatName), (options.StatName.Len() + 1));

		EOS_Leaderboards_GetLeaderboardUserScoreCountOptions LeaderboardsOptions = {   };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;
		LeaderboardsOptions.StatName = StatNameAnsi;

		Result = EOS_Leaderboards_GetLeaderboardUserScoreCount(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions);
	}

	return Result;
}

EOSResult UCoreLeaderboards::EOSLeaderboardsCopyLeaderboardUserScoreByIndex(UObject* worldContextObject, FEOSLeaderboardsCopyLeaderboardUserScoreByIndexOptions options, FEOSLeaderboardsLeaderboardUserScore& outLeaderboardUserScore)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLeaderboardsHandle(worldContextObject))
	{
		char StatNameAnsi[64];
		FCStringAnsi::Strncpy(StatNameAnsi, TCHAR_TO_UTF8(*options.StatName), (options.StatName.Len() + 1));

		EOS_Leaderboards_CopyLeaderboardUserScoreByIndexOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;
		LeaderboardsOptions.LeaderboardUserScoreIndex = static_cast<uint32_t>(options.LeaderboardUserScoreIndex);
		LeaderboardsOptions.StatName = StatNameAnsi;

		EOS_Leaderboards_LeaderboardUserScore* LeaderboardUserScores = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Leaderboards_CopyLeaderboardUserScoreByIndex(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions, &LeaderboardUserScores));

		if (Result == EOSResult::EOS_Success)
		{
			outLeaderboardUserScore = *LeaderboardUserScores;
		}

		EOS_Leaderboards_LeaderboardUserScore_Release(LeaderboardUserScores);
	}
	return Result;
}

EOSResult UCoreLeaderboards::EOSLeaderboardsCopyLeaderboardUserScoreByUserId(UObject* worldContextObject, FEOSLeaderboardsCopyLeaderboardUserScoreByUserIdOptions options, FEOSLeaderboardsLeaderboardUserScore& outLeaderboardUserScore)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLeaderboardsHandle(worldContextObject))
	{
		char StatNameAnsi[64];
		FCStringAnsi::Strncpy(StatNameAnsi, TCHAR_TO_UTF8(*options.StatName), (options.StatName.Len() + 1));

		EOS_Leaderboards_CopyLeaderboardUserScoreByUserIdOptions LeaderboardsOptions = { };
		LeaderboardsOptions.ApiVersion = options.ApiVersion;
		LeaderboardsOptions.UserId = options.UserId;
		LeaderboardsOptions.StatName = StatNameAnsi;

		EOS_Leaderboards_LeaderboardUserScore* LeaderboardUserScores = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Leaderboards_CopyLeaderboardUserScoreByUserId(GetLeaderboardsHandle(worldContextObject), &LeaderboardsOptions, &LeaderboardUserScores));

		if (Result == EOSResult::EOS_Success)
		{
			outLeaderboardUserScore = *LeaderboardUserScores;
		}

		EOS_Leaderboards_LeaderboardUserScore_Release(LeaderboardUserScores);
	}

	return Result;
}

void UCoreLeaderboards::Internal_OnLeaderboardQueryLeaderboardDefinitionsCompleteCallback(const EOS_Leaderboards_OnQueryLeaderboardDefinitionsCompleteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLeaderboardDefinitionsCompleteCallback* CallbackObj = static_cast<FLeaderboardDefinitionsCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLeaderboards* Leaderboards = CallbackObj->LeaderboardsObject;
		check(Leaderboards);

		if (Leaderboards)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

	delete CallbackObj;
	}
}

void UCoreLeaderboards::Internal_OnLeaderboardQueryLeaderboardRanksCompleteCallback(const EOS_Leaderboards_OnQueryLeaderboardRanksCompleteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLeaderboardRanksCompleteCallback* CallbackObj = static_cast<FLeaderboardRanksCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLeaderboards* Leaderboards = CallbackObj->LeaderboardsObject;
		check(Leaderboards);

		if (Leaderboards)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreLeaderboards::Internal_OnLeaderboardQueryLeaderboardUserScoresCompleteCallbackInfo(const EOS_Leaderboards_OnQueryLeaderboardUserScoresCompleteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLeaderboardUserScoresCompleteCallback* CallbackObj = static_cast<FLeaderboardUserScoresCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLeaderboards* Leaderboards = CallbackObj->LeaderboardsObject;
		check(Leaderboards);

		if (Leaderboards)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}