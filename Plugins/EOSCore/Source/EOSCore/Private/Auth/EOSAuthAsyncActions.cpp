/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Auth/EOSAuthAsyncActions.h"
#include "Auth/EOSAuthentication.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreAuthLogin
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreAuthLogin* UEOSCoreAuthLogin::EOSAuthLoginAsync(UObject* worldContextObject, FEOSAuthLoginOptions options)
{
	if (UEOSCoreSubsystem::GetAuthHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreAuthLogin>();
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

void UEOSCoreAuthLogin::Activate()
{
	Super::Activate();
	UCoreAuthentication::GetAuthentication()->EOSAuthLogin(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreAuthLogout
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreAuthLogout* UEOSCoreAuthLogout::EOSAuthLogoutAsync(UObject* worldContextObject, FEOSAuthLogoutOptions options)
{
	if (UEOSCoreSubsystem::GetAuthHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreAuthLogout>();
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

void UEOSCoreAuthLogout::Activate()
{
	Super::Activate();
	UCoreAuthentication::GetAuthentication()->EOSAuthLogout(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreAuthLinkAccount
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreAuthLinkAccount* UEOSCoreAuthLinkAccount::EOSAuthLinkAccount(UObject* worldContextObject, FEOSAuthLinkAccountOptions options)
{
	if (UEOSCoreSubsystem::GetAuthHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreAuthLinkAccount>();
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

void UEOSCoreAuthLinkAccount::Activate()
{
	Super::Activate();
	UCoreAuthentication::GetAuthentication()->EOSAuthLinkAccount(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreAuthDeletePersistentAuth
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreAuthDeletePersistentAuth* UEOSCoreAuthDeletePersistentAuth::EOSAuthDeletePersistentAuthAsync(UObject* worldContextObject, const FEOSAuthDeletePersistentAuthOptions& options)
{
	if (UEOSCoreSubsystem::GetAuthHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreAuthDeletePersistentAuth>();
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

void UEOSCoreAuthDeletePersistentAuth::Activate()
{
	Super::Activate();
	UCoreAuthentication::GetAuthentication()->EOSAuthDeletePersistentAuth(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreAuthVerifyUserAuth
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreAuthVerifyUserAuth* UEOSCoreAuthVerifyUserAuth::EOSAuthVerifyUserAuthAsync(UObject* worldContextObject, FEOSAuthVerifyUserAuthOptions options)
{
	if (UEOSCoreSubsystem::GetAuthHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreAuthVerifyUserAuth>();
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

void UEOSCoreAuthVerifyUserAuth::Activate()
{
	Super::Activate();
	UCoreAuthentication::GetAuthentication()->EOSAuthVerifyUserAuth(WorldContextObject, Options, Callback);
}
