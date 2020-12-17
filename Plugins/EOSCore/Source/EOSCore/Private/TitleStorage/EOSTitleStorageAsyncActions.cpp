/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "TitleStorage/EOSTitleStorageAsyncActions.h"
#include "Core/EOSCorePluginPrivatePCH.h" 
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreTitleStorageQueryFile
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreTitleStorageQueryFile* UEOSCoreTitleStorageQueryFile::EOSTitleStorageQueryFileAsync(UObject* worldContextObject, FEOSTitleStorageQueryFileOptions options)
{
	if (UEOSCoreSubsystem::GetTitleStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreTitleStorageQueryFile>();
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

void UEOSCoreTitleStorageQueryFile::Activate()
{
	Super::Activate();
	UCoreTitleStorage::GetTitleStorage()->EOSTitleStorageQueryFile(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreTitleStorageQueryFileList
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreTitleStorageQueryFileList* UEOSCoreTitleStorageQueryFileList::EOSTitleStorageQueryFileListAsync(UObject* worldContextObject, FEOSTitleStorageQueryFileListOptions options)
{
	if (UEOSCoreSubsystem::GetTitleStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreTitleStorageQueryFileList>();
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

void UEOSCoreTitleStorageQueryFileList::Activate()
{
	Super::Activate();
	UCoreTitleStorage::GetTitleStorage()->EOSTitleStorageQueryFileList(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreTitleStorageReadFile
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreTitleStorageReadFile* UEOSCoreTitleStorageReadFile::EOSTitleStorageReadFileAsync(UObject* worldContextObject, FEOSTitleStorageReadFileOptions options)
{
	if (UEOSCoreSubsystem::GetTitleStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreTitleStorageReadFile>();
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

void UEOSCoreTitleStorageReadFile::Activate()
{
	Super::Activate();
	UCoreTitleStorage::GetTitleStorage()->EOSTitleStorageReadFile(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreTitleStorageDeleteCache
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreTitleStorageDeleteCache* UEOSCoreTitleStorageDeleteCache::EOSTitleStorageDeleteCacheAsync(UObject* worldContextObject, FEOSTitleStorageDeleteCacheOptions options)
{
	if (UEOSCoreSubsystem::GetTitleStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreTitleStorageDeleteCache>();
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

void UEOSCoreTitleStorageDeleteCache::Activate()
{
	Super::Activate();
	UCoreTitleStorage::GetTitleStorage()->EOSTitleStorageDeleteCache(WorldContextObject, Options, Callback);
}