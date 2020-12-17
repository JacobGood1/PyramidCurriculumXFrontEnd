/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Achievements/EOSAchievementsAsyncActions.h"
#include "Achievements/EOSAchievements.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreAchievementsQueryDefinitions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreAchievementsQueryDefinitions* UEOSCoreAchievementsQueryDefinitions::EOSAchievementsQueryDefinitionsAsync(UObject* worldContextObject, FEOSAchievementsQueryDefinitionsOptions options)
{
	if (UEOSCoreSubsystem::GetAchievementsHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreAchievementsQueryDefinitions>();
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

void UEOSCoreAchievementsQueryDefinitions::Activate()
{
	Super::Activate();

	UCoreAchievements::GetAchievements()->EOSAchievementsQueryDefinitions(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreAchievementsQueryPlayerAchievements
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreAchievementsQueryPlayerAchievements* UEOSCoreAchievementsQueryPlayerAchievements::EOSAchievementsQueryPlayerAchievementsAsync(UObject* worldContextObject, FEOSAchievementsQueryPlayerAchievementsOptions options)
{
	if (UEOSCoreSubsystem::GetAchievementsHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreAchievementsQueryPlayerAchievements>();

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

void UEOSCoreAchievementsQueryPlayerAchievements::Activate()
{
	Super::Activate();

	UCoreAchievements::GetAchievements()->EOSAchievementsQueryPlayerAchievements(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreAchievementsUnlockAchievements
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreAchievementsUnlockAchievements* UEOSCoreAchievementsUnlockAchievements::EOSAchievementsUnlockAchievements(UObject* worldContextObject, FEOSAchievementsUnlockAchievementsOptions options)
{
	if (UEOSCoreSubsystem::GetAchievementsHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreAchievementsUnlockAchievements>();
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

void UEOSCoreAchievementsUnlockAchievements::Activate()
{
	Super::Activate();

	UCoreAchievements::GetAchievements()->EOSAchievementsUnlockAchievements(WorldContextObject, Options, Callback);
}
