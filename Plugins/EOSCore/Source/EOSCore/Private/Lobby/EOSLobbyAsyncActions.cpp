/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Lobby/EOSLobbyAsyncActions.h"
#include "Lobby/EOSLobby.h"
#include "Core/EOSCorePluginPrivatePCH.h" 
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCoreLogging.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbyCreateLobby
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbyCreateLobby* UEOSCoreLobbyCreateLobby::EOSLobbyCreateLobbyAsync(UObject* worldContextObject, FEOSLobbyCreateLobbyOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbyCreateLobby>();
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

void UEOSCoreLobbyCreateLobby::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbyCreateLobby(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbyDestroyLobby
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbyDestroyLobby* UEOSCoreLobbyDestroyLobby::EOSLobbyDestroyLobbyAsync(UObject* worldContextObject, FEOSLobbyDestroyLobbyOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbyDestroyLobby>();
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

void UEOSCoreLobbyDestroyLobby::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbyDestroyLobby(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbyDestroyLobby
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbyJoinLobby* UEOSCoreLobbyJoinLobby::EOSLobbyJoinLobbyAsync(UObject* worldContextObject, FEOSLobbyJoinLobbyOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbyJoinLobby>();
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

void UEOSCoreLobbyJoinLobby::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbyJoinLobby(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbyLeaveLobby
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbyLeaveLobby* UEOSCoreLobbyLeaveLobby::EOSLobbyLeaveLobbyAsync(UObject* worldContextObject, FEOSLobbyLeaveLobbyOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbyLeaveLobby>();
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

void UEOSCoreLobbyLeaveLobby::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbyLeaveLobby(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbyUpdateLobby
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbyUpdateLobby* UEOSCoreLobbyUpdateLobby::EOSLobbyUpdateLobbyAsync(UObject* worldContextObject, FEOSLobbyUpdateLobbyOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbyUpdateLobby>();
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

void UEOSCoreLobbyUpdateLobby::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbyUpdateLobby(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbyPromoteMember
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbyPromoteMember* UEOSCoreLobbyPromoteMember::EOSLobbyPromoteMemberAsync(UObject* worldContextObject, FEOSLobbyPromoteMemberOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbyPromoteMember>();
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

void UEOSCoreLobbyPromoteMember::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbyPromoteMember(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbyKickMember
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbyKickMember* UEOSCoreLobbyKickMember::EOSLobbyKickMemberAsync(UObject* worldContextObject, FEOSLobbyKickMemberOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbyKickMember>();
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

void UEOSCoreLobbyKickMember::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbyKickMember(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbySendInvite
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbySendInvite* UEOSCoreLobbySendInvite::EOSLobbySendInviteAsync(UObject* worldContextObject, FEOSLobbySendInviteOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbySendInvite>();
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

void UEOSCoreLobbySendInvite::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbySendInvite(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbyRejectInvite
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbyRejectInvite* UEOSCoreLobbyRejectInvite::EOSLobbyRejectInviteAsync(UObject* worldContextObject, FEOSLobbyRejectInviteOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbyRejectInvite>();
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

void UEOSCoreLobbyRejectInvite::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbyRejectInvite(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbyQueryInvites
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbyQueryInvites* UEOSCoreLobbyQueryInvites::EOSLobbyQueryInvitesAsync(UObject* worldContextObject, FEOSLobbyQueryInvitesOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbyQueryInvites>();
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

void UEOSCoreLobbyQueryInvites::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbyQueryInvites(WorldContextObject, Options, Callback);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreLobbySearchFind
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UEOSCoreLobbySearchFind* UEOSCoreLobbySearchFind::EOSLobbySearchFindAsync(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchFindOptions options)
{
	if (UEOSCoreSubsystem::GetLobbyHandle(worldContextObject))
	{
		auto* GameInstance = worldContextObject->GetWorld()->GetGameInstance();
		auto* AsyncObject = NewObject<UEOSCoreLobbySearchFind>();
		AsyncObject->RegisterWithGameInstance(GameInstance);
		AsyncObject->Options = options;
		AsyncObject->Handle = handle;
		AsyncObject->WorldContextObject = worldContextObject;

		return AsyncObject;
	}
	else
	{
		LogError("Plugin not initialized!");
	}

	return nullptr;
}

void UEOSCoreLobbySearchFind::Activate()
{
	Super::Activate();
	UCoreLobby::GetLobby()->EOSLobbySearchFind(WorldContextObject, Handle, Options, Callback);
}
