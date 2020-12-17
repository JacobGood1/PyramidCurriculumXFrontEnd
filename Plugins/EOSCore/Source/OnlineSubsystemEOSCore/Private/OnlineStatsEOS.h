/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/CoreOnline.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "OnlineSubsystemPackageEOS.h"
#include "OnlineSubsystemTypesEOS.h"
#include "eos_stats_types.h"

class FOnlineSubsystemEOS;

class FOnlineStatsEOS : public IOnlineStats
{
public:
	FOnlineStatsEOS() = delete;
	virtual ~FOnlineStatsEOS() = default;

public:
// IOnlineStats Interface
	virtual void QueryStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TSharedRef<const FUniqueNetId> StatsUser, const FOnlineStatsQueryUserStatsComplete& Delegate) override;
	virtual void QueryStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TArray<TSharedRef<const FUniqueNetId>>& StatUsers, const TArray<FString>& StatNames, const FOnlineStatsQueryUsersStatsComplete& Delegate) override;
	virtual TSharedPtr<const FOnlineStatsUserStats> GetStats(const TSharedRef<const FUniqueNetId> StatsUserId) const override;
	virtual void UpdateStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TArray<FOnlineStatsUserUpdatedStats>& UpdatedUserStats, const FOnlineStatsUpdateStatsComplete& Delegate) override;
#if !UE_BUILD_SHIPPING
	virtual void ResetStats(const TSharedRef<const FUniqueNetId> StatsUserId) override;
#endif
// ~IOnlineStats Interface

PACKAGE_SCOPE:
	FOnlineStatsEOS(FOnlineSubsystemEOS* InSubsystem)
		: m_Subsystem(InSubsystem)
	{}

private:
	void WriteStats(EOS_ProductUserId UserId, const FOnlineStatsUserUpdatedStats& PlayerStats);

private:
	FOnlineSubsystemEOS* m_Subsystem;
	TUniqueNetIdMap<TSharedRef<FOnlineStatsUserStats>> m_StatsCache;
};

typedef TSharedPtr<FOnlineStatsEOS, ESPMode::ThreadSafe> FOnlineStatsEOSPtr;
