/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Stats/EOSStats.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreStats* UCoreStats::s_StatsObject = nullptr;

UCoreStats::UCoreStats()
{
	s_StatsObject = this;
}

void UCoreStats::EOSStatsIngestStat(UObject* worldContextObject, FEOSStatsIngestStatOptions options, const FOnStatsIngestStatCompleteCallback& callback)
{
	LogVerbose("");

	if (GetStatsHandle(worldContextObject))
	{
		FIngestStatOperation IngestStatsOperation(options.Stats.Num(), options.LocalUserId, options.TargetUserId);

		for (int32 i = 0; i < options.Stats.Num(); i++)
		{
			FCStringAnsi::Strncpy(const_cast<char*>(IngestStatsOperation.PointerArray[i].StatName), TCHAR_TO_UTF8(*options.Stats[i].StatName), (options.Stats[i].StatName.Len() + 1));
			IngestStatsOperation.PointerArray[i].IngestAmount = options.Stats[i].IngestAmount;
		}

		FIngestStatCompleteCallback* CallbackObj = new FIngestStatCompleteCallback({ this, callback });
		EOS_Stats_IngestStat(GetStatsHandle(worldContextObject), &IngestStatsOperation, CallbackObj, Internal_OnStatsIngestStatCompleteCallback);
	}
}

void UCoreStats::EOSStatsQueryStats(UObject* worldContextObject, FEOSStatsQueryStatsOptions options, const FOnStatsQueryStatsCompleteCallback& callback)
{
	LogVerbose("");

	if (GetStatsHandle(worldContextObject))
	{
		FQueryStatsOperation QueryStatsOperation(options.StatNames.Num(), options.LocalUserId, options.TargetUserId, (options.StartTime == "-1" || options.StartTime.IsEmpty()) ? -1 : FCString::Atoi64(*options.StartTime), (options.EndTime == "-1" || options.EndTime.IsEmpty()) ? -1 : FCString::Atoi64(*options.EndTime));

		for (int32 i = 0; i < options.StatNames.Num(); i++)
		{
			FCStringAnsi::Strncpy(QueryStatsOperation.PointerArray[i], TCHAR_TO_UTF8(*options.StatNames[i]), (options.StatNames[i].Len() + 1));
		}

		FQueryStatsCompleteCallback* CallbackObj = new FQueryStatsCompleteCallback({ this, callback });
		EOS_Stats_QueryStats(GetStatsHandle(worldContextObject), &QueryStatsOperation, CallbackObj, Internal_OnStatsQueryStatsCompleteCallback);
	}
}

int32 UCoreStats::EOSStatsGetStatsCount(UObject* worldContextObject, FEOSStatsGetStatCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetStatsHandle(worldContextObject))
	{
		EOS_Stats_GetStatCountOptions StatsOptions = { };
		StatsOptions.ApiVersion = options.ApiVersion;
		StatsOptions.TargetUserId = options.TargetUserId;

		Result = EOS_Stats_GetStatsCount(GetStatsHandle(worldContextObject), &StatsOptions);
	}

	return Result;
}

EOSResult UCoreStats::EOSStatsCopyStatByIndex(UObject* worldContextObject, FEOSStatsCopyStatByIndexOptions options, FEOSStatsStat& outStat)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetStatsHandle(worldContextObject))
	{
		EOS_Stats_CopyStatByIndexOptions StatsOptions = { };
		StatsOptions.ApiVersion = options.ApiVersion;
		StatsOptions.TargetUserId = options.TargetUserId;
		StatsOptions.StatIndex = static_cast<uint32_t>(options.StatIndex);

		EOS_Stats_Stat* Stat = nullptr;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Stats_CopyStatByIndex(GetStatsHandle(worldContextObject), &StatsOptions, &Stat));

		if (Result == EOSResult::EOS_Success)
		{
			outStat = *Stat;
		}

		EOS_Stats_Stat_Release(Stat);
	}

	return Result;
}

EOSResult UCoreStats::EOSStatsCopyStatByName(UObject* worldContextObject, FEOSStatsCopyStatByNameOptions options, FEOSStatsStat& outStat)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;
	
	if (GetStatsHandle(worldContextObject))
	{
		char StatNameAnsi[64];
		FCStringAnsi::Strncpy(StatNameAnsi, TCHAR_TO_UTF8(*options.Name), (options.Name.Len() +1));

		EOS_Stats_CopyStatByNameOptions StatsOptions = { };
		StatsOptions.ApiVersion = options.ApiVersion;
		StatsOptions.TargetUserId = options.TargetUserId;
		StatsOptions.Name = StatNameAnsi;

		EOS_Stats_Stat* Stat = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Stats_CopyStatByName(GetStatsHandle(worldContextObject), &StatsOptions, &Stat));

		if (Result == EOSResult::EOS_Success)
		{
			outStat = * Stat;
		}

		EOS_Stats_Stat_Release(Stat);
	}

	return Result;
}

void UCoreStats::Internal_OnStatsIngestStatCompleteCallback(const EOS_Stats_IngestStatCompleteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FIngestStatCompleteCallback* CallbackObj = static_cast<FIngestStatCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	UCoreStats* Stats = CallbackObj->StatsObject;
	check(Stats);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}

void UCoreStats::Internal_OnStatsQueryStatsCompleteCallback(const EOS_Stats_OnQueryStatsCompleteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));
	
	FQueryStatsCompleteCallback* CallbackObj = static_cast<FQueryStatsCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	UCoreStats* Stats = CallbackObj->StatsObject;
	check(Stats);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}