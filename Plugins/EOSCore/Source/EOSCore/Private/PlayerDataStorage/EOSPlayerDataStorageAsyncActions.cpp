/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "PlayerDataStorage/EOSPlayerDataStorageAsyncActions.h"
#include "PlayerDataStorage/EOSPlayerDataStorage.h"
#include "Core/EOSCorePluginPrivatePCH.h" 
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCorePlayerDataStorageQueryFile
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCorePlayerDataStorageQueryFile* UEOSCorePlayerDataStorageQueryFile::EOSPlayerDataStorageQueryFileAsync(UObject* worldContextObject, FEOSPlayerDataStorageQueryFileOptions options)
{
	if (UEOSCoreSubsystem::GetPlayerDataStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCorePlayerDataStorageQueryFile>();
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

void UEOSCorePlayerDataStorageQueryFile::Activate()
{
	Super::Activate();
	UCorePlayerDataStorage::GetPlayerDataStorage()->EOSPlayerDataStorageQueryFile(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCorePlayerDataStorageQueryFileList
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCorePlayerDataStorageQueryFileList* UEOSCorePlayerDataStorageQueryFileList::EOSPlayerDataStorageQueryFileListAsync(UObject* worldContextObject, FEOSPlayerDataStorageQueryFileListOptions options)
{
	if (UEOSCoreSubsystem::GetPlayerDataStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCorePlayerDataStorageQueryFileList>();
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

void UEOSCorePlayerDataStorageQueryFileList::Activate()
{
	Super::Activate();
	UCorePlayerDataStorage::GetPlayerDataStorage()->EOSPlayerDataStorageQueryFileList(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCorePlayerDataStorageDuplicateFile
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCorePlayerDataStorageDuplicateFile* UEOSCorePlayerDataStorageDuplicateFile::EOSPlayerDataStorageDuplicateFileAsync(UObject* worldContextObject, FEOSPlayerDataStorageDuplicateFileOptions options)
{
	if (UEOSCoreSubsystem::GetPlayerDataStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCorePlayerDataStorageDuplicateFile>();
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

void UEOSCorePlayerDataStorageDuplicateFile::Activate()
{
	Super::Activate();
	UCorePlayerDataStorage::GetPlayerDataStorage()->EOSPlayerDataStorageDuplicateFile(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCorePlayerDataStorageDeleteFile
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCorePlayerDataStorageDeleteFile* UEOSCorePlayerDataStorageDeleteFile::EOSPlayerDataStorageDeleteFileAsync(UObject* worldContextObject, FEOSPlayerDataStorageDeleteFileOptions options)
{
	if (UEOSCoreSubsystem::GetPlayerDataStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCorePlayerDataStorageDeleteFile>();
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

void UEOSCorePlayerDataStorageDeleteFile::Activate()
{
	Super::Activate();
	UCorePlayerDataStorage::GetPlayerDataStorage()->EOSPlayerDataStorageDeleteFile(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCorePlayerDataStorageReadFile
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCorePlayerDataStorageReadFile* UEOSCorePlayerDataStorageReadFile::EOSPlayerDataStorageReadFileAsync(UObject* worldContextObject, FEOSPlayerDataStorageReadFileOptions options)
{
	if (UEOSCoreSubsystem::GetPlayerDataStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCorePlayerDataStorageReadFile>();
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

void UEOSCorePlayerDataStorageReadFile::Activate()
{
	Super::Activate();
	UCorePlayerDataStorage::GetPlayerDataStorage()->EOSPlayerDataStorageReadFile(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCorePlayerDataStorageWriteFile
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCorePlayerDataStorageWriteFile* UEOSCorePlayerDataStorageWriteFile::EOSPlayerDataStorageWriteFileAsync(UObject* worldContextObject, FEOSPlayerDataStorageWriteFileOptions options)
{
	if (UEOSCoreSubsystem::GetPlayerDataStorageHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCorePlayerDataStorageWriteFile>();
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

void UEOSCorePlayerDataStorageWriteFile::Activate()
{
	Super::Activate();
	UCorePlayerDataStorage::GetPlayerDataStorage()->EOSPlayerDataStorageWriteFile(WorldContextObject, Options, Callback);
}
