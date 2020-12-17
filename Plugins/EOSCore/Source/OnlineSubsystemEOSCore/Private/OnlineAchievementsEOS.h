/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/CoreOnline.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "OnlineSubsystemPackageEOS.h"
#include "OnlineSubsystemTypesEOS.h"
#include "eos_achievements_types.h"

class FOnlineSubsystemEOS;

class FOnlineAchievementsEOS :
	public IOnlineAchievements
{
public:
	FOnlineAchievementsEOS() = delete;
	virtual ~FOnlineAchievementsEOS() = default;

// IOnlineAchievements Interface
	virtual void WriteAchievements(const FUniqueNetId& PlayerId, FOnlineAchievementsWriteRef& WriteObject, const FOnAchievementsWrittenDelegate& Delegate = FOnAchievementsWrittenDelegate())  override;
	virtual void QueryAchievements(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate = FOnQueryAchievementsCompleteDelegate())  override;
	virtual void QueryAchievementDescriptions(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate = FOnQueryAchievementsCompleteDelegate())  override;
	virtual EOnlineCachedResult::Type GetCachedAchievement(const FUniqueNetId& PlayerId, const FString& AchievementId, FOnlineAchievement& OutAchievement)  override;
	virtual EOnlineCachedResult::Type GetCachedAchievements(const FUniqueNetId& PlayerId, TArray<FOnlineAchievement>& OutAchievements)  override;
	virtual EOnlineCachedResult::Type GetCachedAchievementDescription(const FString& AchievementId, FOnlineAchievementDesc& OutAchievementDesc)  override;
#if !UE_BUILD_SHIPPING
	virtual bool ResetAchievements(const FUniqueNetId& PlayerId) override;
#endif
// ~IOnlineAchievements Interface

PACKAGE_SCOPE:
	FOnlineAchievementsEOS(FOnlineSubsystemEOS* InSubsystem)
		: EOSSubsystem(InSubsystem)
	{}
private:
	FOnlineSubsystemEOS* EOSSubsystem;
	TMap<FString, TSharedPtr<TArray<FOnlineAchievement>>> CachedAchievementsMap;
	TArray<FOnlineAchievementDesc> CachedAchievementDefinitions;
	TMap<FString, FOnlineAchievementDesc*> CachedAchievementDefinitionsMap;
};

typedef TSharedPtr<FOnlineAchievementsEOS, ESPMode::ThreadSafe> FOnlineAchievementsEOSPtr;