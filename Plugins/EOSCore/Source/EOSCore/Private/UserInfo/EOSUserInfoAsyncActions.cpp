/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "UserInfo/EOSUserInfoAsyncActions.h"
#include "UserInfo/EOSUserInfo.h"
#include "Core/EOSCorePluginPrivatePCH.h" 
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreUserInfoQueryUserInfo
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreUserInfoQueryUserInfo* UEOSCoreUserInfoQueryUserInfo::EOSUserInfoQueryUserInfoAsync(UObject* worldContextObject, FEOSUserInfoQueryUserInfoOptions options)
{
	if (UEOSCoreSubsystem::GetUserInfoHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreUserInfoQueryUserInfo>();
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

void UEOSCoreUserInfoQueryUserInfo::Activate()
{
	Super::Activate();
	UCoreUserInfo::GetUserInfo()->EOSUserInfoQueryUserInfo(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreUserInfoQueryUserInfoByDisplayName
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreUserInfoQueryUserInfoByDisplayName* UEOSCoreUserInfoQueryUserInfoByDisplayName::EOSUserInfoQueryUserInfoByDisplayNameAsync(UObject* worldContextObject, FEOSUserInfoQueryUserInfoByDisplayNameOptions options)
{
	if (UEOSCoreSubsystem::GetUserInfoHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreUserInfoQueryUserInfoByDisplayName>();
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

void UEOSCoreUserInfoQueryUserInfoByDisplayName::Activate()
{
	Super::Activate();
	UCoreUserInfo::GetUserInfo()->EOSUserInfoQueryUserInfoByDisplayName(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreUserInfoQueryUserInfoByExternalAccount
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreUserInfoByExternalAccount* UEOSCoreUserInfoByExternalAccount::EOSUserInfoQueryUserInfoByExternalAccountAsync(UObject* worldContextObject, FEOSUserInfoQueryUserInfoByExternalAccountOptions options)
{
	if (UEOSCoreSubsystem::GetUserInfoHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreUserInfoByExternalAccount>();
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

void UEOSCoreUserInfoByExternalAccount::Activate()
{
	Super::Activate();
	UCoreUserInfo::GetUserInfo()->EOSUserInfoQueryUserInfoByExternalAccount(WorldContextObject, Options, Callback);
}