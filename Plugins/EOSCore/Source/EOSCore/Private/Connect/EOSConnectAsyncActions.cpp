/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Connect/EOSConnectAsyncActions.h"
#include "Connect/EOSConnect.h"
#include "Core/EOSCorePluginPrivatePCH.h" 
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreConnectLogin
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreConnectLogin* UEOSCoreConnectLogin::EOSConnectLoginAsync(UObject* worldContextObject, FEOSConnectLoginOptions options)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreConnectLogin>();
		AsyncObject->Options = options;
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->WorldContextObject = worldContextObject;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreConnectLogin::Activate()
{
	Super::Activate();

	UCoreConnect::GetConnect()->EOSConnectLogin(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreConnectCreateUser
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreConnectCreateUser* UEOSCoreConnectCreateUser::EOSConnectCreateUserAsync(UObject* worldContextObject, FEOSConnectCreateUserOptions options)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreConnectCreateUser>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->WorldContextObject = worldContextObject;
		AsyncObject->Options = options;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreConnectCreateUser::Activate()
{
	Super::Activate();

	UCoreConnect::GetConnect()->EOSConnectCreateUser(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreConnectLinkAccount
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreConnectLinkAccount* UEOSCoreConnectLinkAccount::EOSConnectLinkAccountAsync(UObject* worldContextObject, FEOSConnectLinkAccountOptions options)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreConnectLinkAccount>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->WorldContextObject = worldContextObject;
		AsyncObject->Options = options;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreConnectLinkAccount::Activate()
{
	Super::Activate();

	UCoreConnect::GetConnect()->EOSConnectLinkAccount(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreConnectUnlinkAccount
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreConnectUnlinkAccount* UEOSCoreConnectUnlinkAccount::EOSConnectUnlinkAccountAsync(UObject* worldContextObject, FEOSConnectUnlinkAccountOptions options)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreConnectUnlinkAccount>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->WorldContextObject = worldContextObject;
		AsyncObject->Options = options;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreConnectUnlinkAccount::Activate()
{
	Super::Activate();

	UCoreConnect::GetConnect()->EOSConnectUnlinkAccount(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreConnectCreateDeviceId
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreConnectCreateDeviceId* UEOSCoreConnectCreateDeviceId::EOSConnectCreateDeviceIdAsync(UObject* worldContextObject, FEOSConnectCreateDeviceIdOptions options)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreConnectCreateDeviceId>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->WorldContextObject = worldContextObject;
		AsyncObject->Options = options;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreConnectCreateDeviceId::Activate()
{
	Super::Activate();

	UCoreConnect::GetConnect()->EOSConnectCreateDeviceId(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreConnectDeleteDeviceId
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreConnectDeleteDeviceId* UEOSCoreConnectDeleteDeviceId::EOSConnectDeleteDeviceIdAsync(UObject* worldContextObject, FEOSConnectDeleteDeviceIdOptions options)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreConnectDeleteDeviceId>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->WorldContextObject = worldContextObject;
		AsyncObject->Options = options;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreConnectDeleteDeviceId::Activate()
{
	Super::Activate();

	UCoreConnect::GetConnect()->EOSConnectDeleteDeviceId(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreConnectTransferDeviceIdAccount
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreConnectTransferDeviceIdAccount* UEOSCoreConnectTransferDeviceIdAccount::EOSConnectTransferDeviceIdAccountAsync(UObject* worldContextObject, FEOSConnectTransferDeviceIdAccountOptions options)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreConnectTransferDeviceIdAccount>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->WorldContextObject = worldContextObject;
		AsyncObject->Options = options;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreConnectTransferDeviceIdAccount::Activate()
{
	Super::Activate();

	UCoreConnect::GetConnect()->EOSConnectTransferDeviceIdAccount(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreConnectQueryExternalAccountMappings
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreConnectQueryExternalAccountMappings* UEOSCoreConnectQueryExternalAccountMappings::EOSConnectQueryExternalAccountMappingsAsync(UObject* worldContextObject, FEOSConnectQueryExternalAccountMappingsOptions options)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreConnectQueryExternalAccountMappings>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->WorldContextObject = worldContextObject;
		AsyncObject->Options = options;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreConnectQueryExternalAccountMappings::Activate()
{
	Super::Activate();

	UCoreConnect::GetConnect()->EOSConnectQueryExternalAccountMappings(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreConnectQueryProductUserIdMappings
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreConnectQueryProductUserIdMappings* UEOSCoreConnectQueryProductUserIdMappings::EOSConnectQueryProductUserIdMappingsAsync(UObject* worldContextObject, FEOSConnectQueryProductUserIdMappingsOptions options)
{
	if (UEOSCoreSubsystem::GetConnectHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreConnectQueryProductUserIdMappings>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->WorldContextObject = worldContextObject;
		AsyncObject->Options = options;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreConnectQueryProductUserIdMappings::Activate()
{
	Super::Activate();

	UCoreConnect::GetConnect()->EOSConnectQueryProductUserIdMappings(WorldContextObject, Options, Callback);
}
