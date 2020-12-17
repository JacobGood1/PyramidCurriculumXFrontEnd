/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Achievements/EOSAchievements.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreAchievements* UCoreAchievements::s_AchievementsObject = nullptr;

UCoreAchievements::UCoreAchievements()
{
	s_AchievementsObject = this;
}
	
void UCoreAchievements::EOSAchievementsQueryDefinitions(UObject* worldContextObject, FEOSAchievementsQueryDefinitionsOptions options, const FOnAchievementsQueryDefinitionsCallback& callback)
{
	LogVerbose("");

	if (GetAchievementsHandle(worldContextObject))
	{
		EOS_Achievements_QueryDefinitionsOptions AchievementsOptions = { };
		AchievementsOptions.ApiVersion = options.ApiVersion;

		FQueryDefinitionsCallback* CallbackObj = new FQueryDefinitionsCallback({ this, callback });
		EOS_Achievements_QueryDefinitions(GetAchievementsHandle(worldContextObject), &AchievementsOptions, CallbackObj, Internal_OnAchievementsQueryDefinitionsCompleteCallback);
	}
}

int32 UCoreAchievements::EOSAchievementsGetAchievementDefinitionCount(UObject* worldContextObject, FEOSAchievementsGetAchievementDefinitionCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetAchievementsHandle(worldContextObject))
	{
		EOS_Achievements_GetAchievementDefinitionCountOptions AchievementsOptions = {  };
		AchievementsOptions.ApiVersion = options.ApiVersion;

		Result = EOS_Achievements_GetAchievementDefinitionCount(GetAchievementsHandle(worldContextObject), &AchievementsOptions);
	}

	return Result;
}

EOSResult UCoreAchievements::EOSAchievementsCopyAchievementDefinitionV2ByIndex(UObject* worldContextObject, FEOSAchievementsCopyAchievementDefinitionV2ByIndexOptions options, FEOSAchievementsDefinitionV2& outDefinition)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_AccessDenied;

	if (GetAchievementsHandle(worldContextObject))
	{
		EOS_Achievements_CopyAchievementDefinitionV2ByIndexOptions AchievementsOptions = { };
		AchievementsOptions.ApiVersion = options.ApiVersion;
		AchievementsOptions.AchievementIndex = static_cast<uint32_t>(options.AchievementIndex);

		EOS_Achievements_DefinitionV2* Definition = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Achievements_CopyAchievementDefinitionV2ByIndex(GetAchievementsHandle(worldContextObject), &AchievementsOptions, &Definition));

		if (Result == EOSResult::EOS_Success)
		{
			outDefinition = *Definition;
		}

		EOS_Achievements_DefinitionV2_Release(Definition);
	}

	return Result;
}

EOSResult UCoreAchievements::EOSAchievementsCopyAchievementDefinitionV2ByAchievementId(UObject* worldContextObject, FEOSAchievementsCopyAchievementDefinitionV2ByAchievementIdOptions options, FEOSAchievementsDefinitionV2& outDefinition)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_AccessDenied;

	if (GetAchievementsHandle(worldContextObject))
	{
		char AchievementIdAnsi[128];
		FCStringAnsi::Strncpy(AchievementIdAnsi, TCHAR_TO_UTF8(*options.AchievementId), 128);

		EOS_Achievements_CopyAchievementDefinitionV2ByAchievementIdOptions AchievementsOptions = { };
		AchievementsOptions.ApiVersion = options.ApiVersion;
		AchievementsOptions.AchievementId = AchievementIdAnsi;

		EOS_Achievements_DefinitionV2* Definition = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Achievements_CopyAchievementDefinitionV2ByAchievementId(GetAchievementsHandle(worldContextObject), &AchievementsOptions, &Definition));

		if (Result == EOSResult::EOS_Success)
		{
			outDefinition = *Definition;
		}

		EOS_Achievements_DefinitionV2_Release(Definition);
	}

	return Result;
}

void UCoreAchievements::EOSAchievementsQueryPlayerAchievements(UObject* worldContextObject, FEOSAchievementsQueryPlayerAchievementsOptions options, const FOnAchievementsQueryPlayerAchievementsCallback& callback)
{
	LogVerbose("");

	if (GetAchievementsHandle(worldContextObject))
	{
		EOS_Achievements_QueryPlayerAchievementsOptions AchievementsOptions = { };
		AchievementsOptions.ApiVersion = options.ApiVersion;
		AchievementsOptions.UserId = options.UserId;

		FQueryPlayerAchievementsCallback* CallbackObj = new FQueryPlayerAchievementsCallback({ this, callback });
		EOS_Achievements_QueryPlayerAchievements(GetAchievementsHandle(worldContextObject), &AchievementsOptions, CallbackObj, Internal_OnAchievementsQueryPlayerAchievementsCompleteCallback);
	}
}

int32 UCoreAchievements::EOSAchievementsGetPlayerAchievementCount(UObject* worldContextObject, FEOSAchievementsGetPlayerAchievementCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetAchievementsHandle(worldContextObject))
	{
		EOS_Achievements_GetPlayerAchievementCountOptions AchievementsOptions = { };
		AchievementsOptions.ApiVersion = options.ApiVersion;
		AchievementsOptions.UserId = options.UserId;

		Result = EOS_Achievements_GetPlayerAchievementCount(GetAchievementsHandle(worldContextObject), &AchievementsOptions);
	}

	return Result;
}

EOSResult UCoreAchievements::EOSAchievementsCopyPlayerAchievementByIndex(UObject* worldContextObject, FEOSAchievementsCopyPlayerAchievementByIndexOptions options, FEOSAchievementsPlayerAchievement& outAchievement)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_AccessDenied;

	if (GetAchievementsHandle(worldContextObject))
	{
		EOS_Achievements_CopyPlayerAchievementByIndexOptions AchievementsOptions = { };
		AchievementsOptions.ApiVersion = options.ApiVersion;
		AchievementsOptions.UserId = options.UserId;
		AchievementsOptions.AchievementIndex = static_cast<uint32_t>(options.AchievementIndex);

		EOS_Achievements_PlayerAchievement* CallbackObj = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Achievements_CopyPlayerAchievementByIndex(GetAchievementsHandle(worldContextObject), &AchievementsOptions, &CallbackObj));

		if (Result == EOSResult::EOS_Success)
		{
			outAchievement = *CallbackObj;
		}

		EOS_Achievements_PlayerAchievement_Release(CallbackObj);
	}

	return Result;
}

void UCoreAchievements::EOSAchievementsUnlockAchievements(UObject* worldContextObject, FEOSAchievementsUnlockAchievementsOptions options, const FOnAchievementsUnlockAchievementsCallback& callback)
{
	LogVerbose("");

	if (GetAchievementsHandle(worldContextObject))
	{
		FUnlockAchievementsOperation UnlockAchievementsOperation(options.AchievementIds.Num(), options.UserId);

		for (int32 i = 0; i < options.AchievementIds.Num(); i++)
		{
			FCStringAnsi::Strncpy(const_cast<char*>(UnlockAchievementsOperation.PointerArray[i]), TCHAR_TO_UTF8(*options.AchievementIds[i]), (options.AchievementIds[i].Len() +1));
		}

		FUnlockAchievementsCallback* CallbackObj = new FUnlockAchievementsCallback({ this, callback });
		EOS_Achievements_UnlockAchievements(GetAchievementsHandle(worldContextObject), &UnlockAchievementsOperation, CallbackObj, Internal_OnAchievementsUnlockAchievementsCompleteCallback);
	}
}

struct FAchievementsUnlockedV2Callback
{
public:
	FAchievementsUnlockedV2Callback(UObject* worldContextObject, const FOnAchievementsUnlockedV2Callback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FAchievementsUnlockedV2Callback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnAchievementsUnlockedV2Callback Callback;
};

static TMap<FEOSNotificationId, FAchievementsUnlockedV2Callback*> s_AchievementsUnlockedV2Callbacks;

FEOSNotificationId UCoreAchievements::EOSAchievementsAddNotifyAchievementsUnlockedV2(UObject* worldContextObject, const FOnAchievementsUnlockedV2Callback& callback)
{
	LogVerbose("");

	FEOSNotificationId NotificationId;

	if (GetAchievementsHandle(worldContextObject))
	{
		EOS_Achievements_AddNotifyAchievementsUnlockedV2Options AchievementsOptions = { };
		AchievementsOptions.ApiVersion = EOS_ACHIEVEMENTS_ADDNOTIFYACHIEVEMENTSUNLOCKEDV2_API_LATEST;

		FAchievementsUnlockedV2Callback* CallbackObj = new FAchievementsUnlockedV2Callback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		NotificationId = EOS_Achievements_AddNotifyAchievementsUnlockedV2(GetAchievementsHandle(worldContextObject), &AchievementsOptions, CallbackObj, ([](const EOS_Achievements_OnAchievementsUnlockedCallbackV2Info* data)
		{
			FAchievementsUnlockedV2Callback* CallbackData = static_cast<FAchievementsUnlockedV2Callback*>(data->ClientData);
			CallbackData->Callback.ExecuteIfBound(*data);
		}));
		s_AchievementsUnlockedV2Callbacks.Add(NotificationId, CallbackObj);
	}

	return NotificationId;
}

void UCoreAchievements::EOSAchievementsRemoveNotifyAchievementsUnlocked(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetAchievementsHandle(worldContextObject))
	{
		EOS_Achievements_RemoveNotifyAchievementsUnlocked(GetAchievementsHandle(worldContextObject), id);
		if (FAchievementsUnlockedV2Callback** CallbackObj = s_AchievementsUnlockedV2Callbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_AchievementsUnlockedV2Callbacks.Remove(id);
	}
}

void UCoreAchievements::Internal_OnAchievementsQueryDefinitionsCompleteCallback(const EOS_Achievements_OnQueryDefinitionsCompleteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryDefinitionsCallback* CallbackObj = static_cast<FQueryDefinitionsCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreAchievements* Achievements = CallbackObj->AchievementsObject;
		check(Achievements);

		if (Achievements)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

	delete CallbackObj;
	}
}

void UCoreAchievements::Internal_OnAchievementsQueryPlayerAchievementsCompleteCallback(const EOS_Achievements_OnQueryPlayerAchievementsCompleteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryPlayerAchievementsCallback* CallbackObj = static_cast<FQueryPlayerAchievementsCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreAchievements* Achievements = CallbackObj->AchievementsObject;
		check(Achievements);

		if (Achievements)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreAchievements::Internal_OnAchievementsUnlockAchievementsCompleteCallback(const EOS_Achievements_OnUnlockAchievementsCompleteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FUnlockAchievementsCallback* CallbackObj = static_cast<FUnlockAchievementsCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreAchievements* Achievements = CallbackObj->AchievementsObject;
		check(Achievements);

		if (Achievements)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}