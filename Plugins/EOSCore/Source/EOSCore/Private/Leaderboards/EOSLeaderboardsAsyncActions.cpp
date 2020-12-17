/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Leaderboards/EOSLeaderboardsAsyncActions.h"
#include "Leaderboards/EOSLeaderboards.h"
#include "Core/EOSCorePluginPrivatePCH.h" 
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLeaderboardsQueryLeaderboardDefinitions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLeaderboardsQueryLeaderboardDefinitions* UEOSCoreLeaderboardsQueryLeaderboardDefinitions::EOSLeaderboardsQueryLeaderboardDefinitionsAsync(UObject* worldContextObject, FEOSLeaderboardsQueryLeaderboardDefinitionsOptions options)
{
	if (UEOSCoreSubsystem::GetLeaderboardsHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLeaderboardsQueryLeaderboardDefinitions>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->Options = options;
		AsyncObject->WorldContextObject = worldContextObject;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreLeaderboardsQueryLeaderboardDefinitions::Activate()
{
	Super::Activate();
	UCoreLeaderboards::GetLeaderboards()->EOSLeaderboardsQueryLeaderboardDefinitions(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLeaderboardsQueryLeaderboardRanks
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLeaderboardsQueryLeaderboardRanks* UEOSCoreLeaderboardsQueryLeaderboardRanks::EOSLeaderboardsQueryLeaderboardRanksAsync(UObject* worldContextObject, FEOSLeaderboardsQueryLeaderboardRanksOptions options)
{
	if (UEOSCoreSubsystem::GetLeaderboardsHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLeaderboardsQueryLeaderboardRanks>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->Options = options;
		AsyncObject->WorldContextObject = worldContextObject;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreLeaderboardsQueryLeaderboardRanks::Activate()
{
	Super::Activate();
	UCoreLeaderboards::GetLeaderboards()->EOSLeaderboardsQueryLeaderboardRanks(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLeaderboardsQueryLeaderboardUserScores
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLeaderboardsQueryLeaderboardUserScores* UEOSCoreLeaderboardsQueryLeaderboardUserScores::EOSLeaderboardsQueryLeaderboardUserScoresAsync(UObject* worldContextObject, FEOSLeaderboardsQueryLeaderboardUserScoresOptions options)
{
	if (UEOSCoreSubsystem::GetLeaderboardsHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLeaderboardsQueryLeaderboardUserScores>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->Options = options;
		AsyncObject->WorldContextObject = worldContextObject;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreLeaderboardsQueryLeaderboardUserScores::Activate()
{
	Super::Activate();
	UCoreLeaderboards::GetLeaderboards()->EOSLeaderboardsQueryLeaderboardUserScores(WorldContextObject, Options, Callback);
}
