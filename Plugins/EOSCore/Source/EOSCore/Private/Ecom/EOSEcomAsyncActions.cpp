/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Ecom/EOSEcomAsyncActions.h"
#include "Ecom/EOSEcom.h"
#include "Core/EOSCorePluginPrivatePCH.h" 
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreEcomQueryOwnership
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreEcomQueryOwnership* UEOSCoreEcomQueryOwnership::EOSEcomQueryOwnershipAsync(UObject* worldContextObject, FEOSEcomQueryOwnershipOptions options)
{
	if (UEOSCoreSubsystem::GetEcomHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreEcomQueryOwnership>();
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

void UEOSCoreEcomQueryOwnership::Activate()
{
	Super::Activate();
	UCoreEcom::GetEcom()->EOSEcomQueryOwnership(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreEcomQueryOwnershipToken
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreEcomQueryOwnershipToken* UEOSCoreEcomQueryOwnershipToken::EOSEcomQueryOwnershipTokenAsync(UObject* worldContextObject, FEOSEcomQueryOwnershipTokenOptions options)
{
	if (UEOSCoreSubsystem::GetEcomHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreEcomQueryOwnershipToken>();
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

void UEOSCoreEcomQueryOwnershipToken::Activate()
{
	Super::Activate();
	UCoreEcom::GetEcom()->EOSEcomQueryOwnershipToken(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreEcomQueryEntitlements
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreEcomQueryEntitlements* UEOSCoreEcomQueryEntitlements::EOSEcomQueryEntitlementsAsync(UObject* worldContextObject, FEOSEcomQueryEntitlementsOptions options)
{
	if (UEOSCoreSubsystem::GetEcomHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreEcomQueryEntitlements>();
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

void UEOSCoreEcomQueryEntitlements::Activate()
{
	Super::Activate();
	UCoreEcom::GetEcom()->EOSEcomQueryEntitlements(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreEcomQueryOffers
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreEcomQueryOffers* UEOSCoreEcomQueryOffers::EOSEcomQueryOffersAsync(UObject* worldContextObject, FEOSEcomQueryOffersOptions options)
{
	if (UEOSCoreSubsystem::GetEcomHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreEcomQueryOffers>();
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

void UEOSCoreEcomQueryOffers::Activate()
{
	Super::Activate();
	UCoreEcom::GetEcom()->EOSEcomQueryOffers(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreEcomCheckout
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreEcomCheckout* UEOSCoreEcomCheckout::EOSEcomCheckoutAsync(UObject* worldContextObject, FEOSEcomCheckoutOptions options)
{
	if (UEOSCoreSubsystem::GetEcomHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreEcomCheckout>();
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

void UEOSCoreEcomCheckout::Activate()
{
	Super::Activate();
	UCoreEcom::GetEcom()->EOSEcomCheckout(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreEcomRedeemEntitlements
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreEcomRedeemEntitlements* UEOSCoreEcomRedeemEntitlements::EOSEcomRedeemEntitlementsAsync(UObject* worldContextObject, FEOSEcomRedeemEntitlementsOptions options)
{
	if (UEOSCoreSubsystem::GetEcomHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreEcomRedeemEntitlements>();
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

void UEOSCoreEcomRedeemEntitlements::Activate()
{
	Super::Activate();
	UCoreEcom::GetEcom()->EOSEcomRedeemEntitlements(WorldContextObject, Options, Callback);
}