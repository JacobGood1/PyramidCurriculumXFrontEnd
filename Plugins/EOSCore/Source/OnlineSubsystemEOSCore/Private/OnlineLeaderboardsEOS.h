/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/CoreOnline.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "OnlineSubsystemPackageEOS.h"
#include "OnlineSubsystemTypesEOS.h"
#include "eos_leaderboards_types.h"

class FOnlineSubsystemEOS;

#define EOS_MAX_NUM_RANKINGS 1000

class FOnlineLeaderboardsEOS : public IOnlineLeaderboards
{
public:
	FOnlineLeaderboardsEOS() = delete;
	virtual ~FOnlineLeaderboardsEOS() = default;
public:
// IOnlineLeaderboards Interface
	virtual bool ReadLeaderboards(const TArray< TSharedRef<const FUniqueNetId> >& Players, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual bool ReadLeaderboardsForFriends(int32 LocalUserNum, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual bool ReadLeaderboardsAroundRank(int32 Rank, uint32 Range, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual bool ReadLeaderboardsAroundUser(TSharedRef<const FUniqueNetId> Player, uint32 Range, FOnlineLeaderboardReadRef& ReadObject) override;
	virtual void FreeStats(FOnlineLeaderboardRead& ReadObject) override;
	virtual bool WriteLeaderboards(const FName& SessionName, const FUniqueNetId& Player, FOnlineLeaderboardWrite& WriteObject) override;
	virtual bool FlushLeaderboards(const FName& SessionName) override;
	virtual bool WriteOnlinePlayerRatings(const FName& SessionName, int32 LeaderboardId, const TArray<FOnlinePlayerScore>& PlayerScores) override;
// ~IOnlineLeaderboards Interface

PACKAGE_SCOPE:
	FOnlineLeaderboardsEOS(FOnlineSubsystemEOS* InSubsystem)
		: m_Subsystem(InSubsystem)
	{}
private:
	FOnlineSubsystemEOS* m_Subsystem;
};

typedef TSharedPtr<FOnlineLeaderboardsEOS, ESPMode::ThreadSafe> FOnlineLeaderboardsEOSPtr;