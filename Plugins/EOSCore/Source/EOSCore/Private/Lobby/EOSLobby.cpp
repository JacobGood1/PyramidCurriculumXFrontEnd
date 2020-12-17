/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Lobby/EOSLobby.h"
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreLobby* UCoreLobby::s_LobbyObject = nullptr;

UCoreLobby::UCoreLobby()
{
	s_LobbyObject = this;
}

void UCoreLobby::EOSLobbyCreateLobby(UObject* worldContextObject, FEOSLobbyCreateLobbyOptions options, const FOnLobbyCreateLobbyCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_CreateLobbyOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;
		LobbyOptions.MaxLobbyMembers = static_cast<uint32_t>(options.MaxLobbyMembers);
		LobbyOptions.PermissionLevel = static_cast<EOS_ELobbyPermissionLevel>(options.PermissionLevel);

		FCreateLobbyCallback* CallbackObj = new FCreateLobbyCallback({ this, callback });
		EOS_Lobby_CreateLobby(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbyCreateLobbyCallback);
	}
}

void UCoreLobby::EOSLobbyDestroyLobby(UObject* worldContextObject, FEOSLobbyDestroyLobbyOptions options, const FOnLobbyDestroyLobbyCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_DestroyLobbyOptions LobbyOptions = { };

		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;
		LobbyOptions.LobbyId = static_cast<const char*>(options.LobbyId);

		FDestroyLobbyCallback* CallbackObj = new FDestroyLobbyCallback({ this, callback });
		EOS_Lobby_DestroyLobby(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbyDestroyLobbyCallback);
	}
}

void UCoreLobby::EOSLobbyJoinLobby(UObject* worldContextObject, FEOSLobbyJoinLobbyOptions options, const FOnLobbyJoinLobbyCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_JoinLobbyOptions LobbyOptions = {  };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LobbyDetailsHandle = options.LobbyDetailsHandle;
		LobbyOptions.LocalUserId = options.LocalUserId;

		FJoinLobbyCallback* CallbackObj = new FJoinLobbyCallback({ this, callback });
		EOS_Lobby_JoinLobby(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbyJoinLobbyCallback);
	}
}

void UCoreLobby::EOSLobbyLeaveLobby(UObject* worldContextObject, FEOSLobbyLeaveLobbyOptions options, const FOnLobbyLeaveLobbyCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_LeaveLobbyOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;
		LobbyOptions.LobbyId = static_cast<const char*>(options.LobbyId);

		FLeaveLobbyCallback* CallbackObj = new FLeaveLobbyCallback({ this, callback });
		EOS_Lobby_LeaveLobby(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbyLeaveLobbyCallback);
	}
}

EOSResult UCoreLobby::EOSLobbyUpdateLobbyModification(UObject* worldContextObject, FEOSLobbyUpdateLobbyModificationOptions options, FEOSHLobbyModification& outLobbyModificationHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_UpdateLobbyModificationOptions LobbyOptions = {  };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;
		LobbyOptions.LobbyId = static_cast<const char*>(options.LobbyId);

		EOS_HLobbyModification LobbyHandle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Lobby_UpdateLobbyModification(GetLobbyHandle(worldContextObject), &LobbyOptions, &LobbyHandle));
		if (Result == EOSResult::EOS_Success)
		{
			outLobbyModificationHandle = LobbyHandle;
		}
	}

	return Result;
}

void UCoreLobby::EOSLobbyUpdateLobby(UObject* worldContextObject, FEOSLobbyUpdateLobbyOptions options, const FOnLobbyUpdateLobbyCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_UpdateLobbyOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LobbyModificationHandle = options.LobbyModificationHandle;

		FUpdateLobbyCallback* CallbackObj = new FUpdateLobbyCallback({ this, callback });
		EOS_Lobby_UpdateLobby(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbyUpdateLobbyCallback);
	}
}

void UCoreLobby::EOSLobbyPromoteMember(UObject* worldContextObject, FEOSLobbyPromoteMemberOptions options, const FOnLobbyPromoteMemberCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_PromoteMemberOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;
		LobbyOptions.TargetUserId = options.TargetUserId;
		LobbyOptions.LobbyId = static_cast<const char*>(options.LobbyId);

		FLobbyPromoteMemberCallback* CallbackObj = new FLobbyPromoteMemberCallback({ this, callback });
		EOS_Lobby_PromoteMember(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbyPromoteMemberCallback);
	}
}

void UCoreLobby::EOSLobbyKickMember(UObject* worldContextObject, FEOSLobbyKickMemberOptions options, const FOnLobbyKickMemberCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_KickMemberOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;
		LobbyOptions.LobbyId = static_cast<const char*>(options.LobbyId);
		LobbyOptions.TargetUserId = options.TargetUserId;

		FLobbyKickMemberCallback* CallbackObj = new FLobbyKickMemberCallback({ this, callback });
		EOS_Lobby_KickMember(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbyKickMemberCallback);
	}
}

struct FLobbyUpdateReceivedCallback
{
public:
	FLobbyUpdateReceivedCallback(UObject* worldContextObject, const FOnLobbyUpdateReceivedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FLobbyUpdateReceivedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnLobbyUpdateReceivedCallback Callback;
};

static TMap<FEOSNotificationId, FLobbyUpdateReceivedCallback*> s_LobbyUpdateReceivedCallbacks;

FEOSNotificationId UCoreLobby::EOSLobbyAddNotifyLobbyUpdateReceived(UObject* worldContextObject, const FOnLobbyUpdateReceivedCallback& callback)
{
	LogVerbose("");

	static FEOSNotificationId Id;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_AddNotifyLobbyUpdateReceivedOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYUPDATERECEIVED_API_LATEST;

		FLobbyUpdateReceivedCallback* CallbackObj = new FLobbyUpdateReceivedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		Id = EOS_Lobby_AddNotifyLobbyUpdateReceived(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, ([](const EOS_Lobby_LobbyUpdateReceivedCallbackInfo* data)
		{
			FLobbyUpdateReceivedCallback* CallbackObj = static_cast<FLobbyUpdateReceivedCallback*>(data->ClientData);
			CallbackObj->Callback.ExecuteIfBound(*data);
		}));
		s_LobbyUpdateReceivedCallbacks.Add(Id, CallbackObj);
	}

	return Id;
}

void UCoreLobby::EOSLobbyRemoveNotifyLobbyUpdateReceived(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_RemoveNotifyLobbyUpdateReceived(GetLobbyHandle(worldContextObject), id);
		if (FLobbyUpdateReceivedCallback** CallbackObj = s_LobbyUpdateReceivedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_LobbyUpdateReceivedCallbacks.Remove(id);
	}
}

struct FLobbyMemberUpdateReceivedCallback
{
public:
	FLobbyMemberUpdateReceivedCallback(UObject* worldContextObject, const FOnLobbyMemberUpdateReceivedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FLobbyMemberUpdateReceivedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnLobbyMemberUpdateReceivedCallback Callback;
};

static TMap<FEOSNotificationId, FLobbyMemberUpdateReceivedCallback*> s_LobbyMemberUpdateReceivedCallbacks;

FEOSNotificationId UCoreLobby::EOSLobbyAddNotifyLobbyMemberUpdateReceived(UObject* worldContextObject, const FOnLobbyMemberUpdateReceivedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId Id;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_AddNotifyLobbyMemberUpdateReceivedOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERUPDATERECEIVED_API_LATEST;

		FLobbyMemberUpdateReceivedCallback* CallbackObj = new FLobbyMemberUpdateReceivedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		Id = EOS_Lobby_AddNotifyLobbyMemberUpdateReceived(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, ([](const EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo* data)
		{
			FLobbyMemberUpdateReceivedCallback* CallbackObj = static_cast<FLobbyMemberUpdateReceivedCallback*>(data->ClientData);
			CallbackObj->Callback.ExecuteIfBound(*data);
		}));

		s_LobbyMemberUpdateReceivedCallbacks.Add(Id, CallbackObj);
	}

	return Id;
}

void UCoreLobby::EOSLobbyRemoveNotifyLobbyMemberUpdateReceived(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_RemoveNotifyLobbyMemberUpdateReceived(GetLobbyHandle(worldContextObject), id);
		if (FLobbyMemberUpdateReceivedCallback** CallbackObj = s_LobbyMemberUpdateReceivedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_LobbyMemberUpdateReceivedCallbacks.Remove(id);
	}
}

struct FLobbyMemberStatusReceivedCallback
{
public:
	FLobbyMemberStatusReceivedCallback(UObject* worldContextObject, const FOnLobbyMemberStatusReceivedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FLobbyMemberStatusReceivedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnLobbyMemberStatusReceivedCallback Callback;
};

static TMap<FEOSNotificationId, FLobbyMemberStatusReceivedCallback*> s_LobbyMemberStatusReceivedCallbacks;

FEOSNotificationId UCoreLobby::EOSLobbyAddNotifyLobbyMemberStatusReceived(UObject* worldContextObject, const FOnLobbyMemberStatusReceivedCallback& callback)
{
	LogVerbose("");

	static FEOSNotificationId Id;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_AddNotifyLobbyMemberStatusReceivedOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERSTATUSRECEIVED_API_LATEST;

		FLobbyMemberStatusReceivedCallback* CallbackObj = new FLobbyMemberStatusReceivedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		Id = EOS_Lobby_AddNotifyLobbyMemberStatusReceived(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, ([](const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* data)
		{
			FLobbyMemberStatusReceivedCallback* CallbackObj = static_cast<FLobbyMemberStatusReceivedCallback*>(data->ClientData);
			CallbackObj->Callback.ExecuteIfBound(*data);
		}));

		s_LobbyMemberStatusReceivedCallbacks.Add(Id, CallbackObj);
	}

	return Id;
}

void UCoreLobby::EOSLobbyRemoveNotifyLobbyMemberStatusReceived(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_RemoveNotifyLobbyMemberStatusReceived(GetLobbyHandle(worldContextObject), id);
		if (FLobbyMemberStatusReceivedCallback** CallbackObj = s_LobbyMemberStatusReceivedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_LobbyMemberStatusReceivedCallbacks.Remove(id);
	}
}

void UCoreLobby::EOSLobbySendInvite(UObject* worldContextObject, FEOSLobbySendInviteOptions options, const FOnLobbySendInviteCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_SendInviteOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;
		LobbyOptions.LobbyId = static_cast<const char*>(options.LobbyId);
		LobbyOptions.TargetUserId = options.TargetUserId;

		FLobbySendInviteCallback* CallbackObj = new FLobbySendInviteCallback({ this, callback });
		EOS_Lobby_SendInvite(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbySendInviteCallback);
	}
}

void UCoreLobby::EOSLobbyRejectInvite(UObject* worldContextObject, FEOSLobbyRejectInviteOptions options, const FOnLobbyRejectInviteCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_RejectInviteOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.InviteId = static_cast<const char*>(options.InviteId);
		LobbyOptions.LocalUserId = options.LocalUserId;

		FLobbyRejectInviteCallback* CallbackObj = new FLobbyRejectInviteCallback({ this, callback });
		EOS_Lobby_RejectInvite(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbyRejectInviteCallback);
	}
}

void UCoreLobby::EOSLobbyQueryInvites(UObject* worldContextObject, FEOSLobbyQueryInvitesOptions options, const FOnLobbyQueryInvitesCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_QueryInvitesOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;

		FLobbyQueryInvitesCallback* CallbackObj = new FLobbyQueryInvitesCallback({ this, callback });
		EOS_Lobby_QueryInvites(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, Internal_OnLobbyQueryInvitesCallback);
	}
}

int32 UCoreLobby::EOSLobbyGetInviteCount(UObject* worldContextObject, FEOSLobbyGetInviteCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_GetInviteCountOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;

		Result = EOS_Lobby_GetInviteCount(GetLobbyHandle(worldContextObject), &LobbyOptions);
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyGetInviteIdByIndex(UObject* worldContextObject, FEOSLobbyGetInviteIdByIndexOptions options, FLobbyInviteId& string)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_GetInviteIdByIndexOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;
		LobbyOptions.Index = static_cast<uint32_t>(options.Index);

		char Buffer[512];
		int32 Size = 512;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Lobby_GetInviteIdByIndex(GetLobbyHandle(worldContextObject), &LobbyOptions, Buffer, &Size));

		if (Result == EOSResult::EOS_Success)
		{
			string = FString(UTF8_TO_TCHAR(Buffer));
		}
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyCreateLobbySearch(UObject* worldContextObject, FEOSLobbyCreateLobbySearchOptions options, FEOSHLobbySearch& outLobbySearchHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_CreateLobbySearchOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.MaxResults = static_cast<uint32_t>(options.MaxResults);

		EOS_HLobbySearch LobbyHandle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Lobby_CreateLobbySearch(GetLobbyHandle(worldContextObject), &LobbyOptions, &LobbyHandle));

		if (Result == EOSResult::EOS_Success)
		{
			outLobbySearchHandle = LobbyHandle;
		}
	}

	return Result;
}

struct FLobbyInviteReceivedCallback
{
public:
	FLobbyInviteReceivedCallback(UObject* worldContextObject, const FOnLobbyInviteReceivedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FLobbyInviteReceivedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnLobbyInviteReceivedCallback Callback;
};

static TMap<FEOSNotificationId, FLobbyInviteReceivedCallback*> s_LobbyInviteReceivedCallbacks;

FEOSNotificationId UCoreLobby::EOSLobbyAddNotifyLobbyInviteReceived(UObject* worldContextObject, const FOnLobbyInviteReceivedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId Id;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_AddNotifyLobbyInviteReceivedOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYINVITERECEIVED_API_LATEST;

		FLobbyInviteReceivedCallback* CallbackObj = new FLobbyInviteReceivedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		Id = EOS_Lobby_AddNotifyLobbyInviteReceived(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, ([](const EOS_Lobby_LobbyInviteReceivedCallbackInfo* data)
		{
			FLobbyInviteReceivedCallback* CallbackObj = static_cast<FLobbyInviteReceivedCallback*>(data->ClientData);
			CallbackObj->Callback.ExecuteIfBound(*data);
		}));

		s_LobbyInviteReceivedCallbacks.Add(Id, CallbackObj);
	}

	return Id;
}

void UCoreLobby::EOSLobbyRemoveNotifyLobbyInviteReceived(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_RemoveNotifyLobbyInviteReceived(GetLobbyHandle(worldContextObject), id);
		if (FLobbyInviteReceivedCallback** CallbackObj = s_LobbyInviteReceivedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_LobbyInviteReceivedCallbacks.Remove(id);
	}
}

struct FLobbyInviteAcceptedCallback
{
public:
	FLobbyInviteAcceptedCallback(UObject* worldContextObject, const FOnLobbyInviteAcceptedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FLobbyInviteAcceptedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnLobbyInviteAcceptedCallback Callback;
};

static TMap<FEOSNotificationId, FLobbyInviteAcceptedCallback*> s_LobbyInviteAcceptedCallbacks;

FEOSNotificationId UCoreLobby::EOSLobbyAddNotifyLobbyInviteAccepted(UObject* worldContextObject, FEOSLobbyAddNotifyLobbyInviteAcceptedOptions options, const FOnLobbyInviteAcceptedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId Id;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_AddNotifyLobbyInviteAcceptedOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;

		FLobbyInviteAcceptedCallback* CallbackObj = new FLobbyInviteAcceptedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		Id = EOS_Lobby_AddNotifyLobbyInviteAccepted(GetLobbyHandle(worldContextObject), &LobbyOptions, CallbackObj, ([](const EOS_Lobby_LobbyInviteAcceptedCallbackInfo* data)
		{
			FLobbyInviteAcceptedCallback* CallbackObj = static_cast<FLobbyInviteAcceptedCallback*>(data->ClientData);
			CallbackObj->Callback.ExecuteIfBound(*data);
		}));

		s_LobbyInviteAcceptedCallbacks.Add(Id, CallbackObj);
	}

	return Id;
}

void UCoreLobby::EOSLobbyRemoveNotifyLobbyInviteAccepted(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_RemoveNotifyLobbyInviteReceived(GetLobbyHandle(worldContextObject), id);
		if (FLobbyInviteAcceptedCallback** CallbackObj = s_LobbyInviteAcceptedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_LobbyInviteAcceptedCallbacks.Remove(id);
	}
}

EOSResult UCoreLobby::EOSLobbyCopyLobbyDetailsHandleByInviteId(UObject* worldContextObject, FEOSLobbyCopyLobbyDetailsHandleByInviteIdOptions options, FEOSHLobbyDetails& outLobbyDetailsHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_CopyLobbyDetailsHandleByInviteIdOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.InviteId = static_cast<const char*>(options.InviteId);

		EOS_HLobbyDetails LobbyHandle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Lobby_CopyLobbyDetailsHandleByInviteId(GetLobbyHandle(worldContextObject), &LobbyOptions, &LobbyHandle));

		if (Result == EOSResult::EOS_Success)
		{
			outLobbyDetailsHandle = LobbyHandle;
		}
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyCopyLobbyDetailsHandle(UObject* worldContextObject, FEOSLobbyCopyLobbyDetailsHandleOptions options, FEOSHLobbyDetails& outLobbyDetailsHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_Lobby_CopyLobbyDetailsHandleOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LobbyId = static_cast<const char*>(options.LobbyId);
		LobbyOptions.LocalUserId = options.LocalUserId;

		EOS_HLobbyDetails LobbyHandle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Lobby_CopyLobbyDetailsHandle(GetLobbyHandle(worldContextObject), &LobbyOptions, &LobbyHandle));

		if (Result == EOSResult::EOS_Success)
		{
			outLobbyDetailsHandle = LobbyHandle;
		}
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyModificationSetPermissionLevel(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationSetPermissionLevelOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyModification_SetPermissionLevelOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.PermissionLevel = static_cast<EOS_ELobbyPermissionLevel>(options.PermissionLevel);

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyModification_SetPermissionLevel(handle, &LobbyOptions));
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyModificationSetMaxMembers(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationSetMaxMembersOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyModification_SetMaxMembersOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.MaxMembers = static_cast<uint32_t>(options.MaxMembers);

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyModification_SetMaxMembers(handle, &LobbyOptions));
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyModificationAddAttribute(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationAddAttributeOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyModification_AddAttributeOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.Attribute = &options.Attribute.AttributeData;
		LobbyOptions.Visibility = static_cast<EOS_ELobbyAttributeVisibility>(options.Visibility);

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyModification_AddAttribute(handle, &LobbyOptions));
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyModificationAddAttributeBool(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, bool bValue, EEOSELobbyAttributeVisibility visibility)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbyModificationAddAttributeOptions Options;
	EOS_Lobby_AttributeData LobbyAttributeData = { };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;
	LobbyAttributeData.Value.AsBool = bValue;
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;

	Options.Attribute = LobbyAttributeData;
	Options.Visibility = visibility;

	return EOSLobbyModificationAddAttribute(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbyModificationAddAttributeDouble(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbyModificationAddAttributeOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = { };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Value.AsDouble = FCString::Atod(*value);
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;

	Options.Attribute = LobbyAttributeData;
	Options.Visibility = visibility;

	return EOSLobbyModificationAddAttribute(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbyModificationAddAttributeInt64(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbyModificationAddAttributeOptions Options;
	EOS_Lobby_AttributeData LobbyAttributeData = { };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Value.AsInt64 = FCString::Atoi64(*value);
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;

	Options.Attribute = LobbyAttributeData;
	Options.Visibility = visibility;

	return EOSLobbyModificationAddAttribute(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbyModificationAddAttributeString(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);
	char ValueAnsi[512];
	FCStringAnsi::Strncpy(ValueAnsi, TCHAR_TO_UTF8(*value), 512);

	FEOSLobbyModificationAddAttributeOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = { };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;

	LobbyAttributeData.Value.AsUtf8 = ValueAnsi;
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;

	Options.Attribute = LobbyAttributeData;
	Options.Visibility = visibility;

	return EOSLobbyModificationAddAttribute(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbyModificationRemoveAttribute(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationRemoveAttributeOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		char KeyAnsi[512];
		FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*options.Key), 512);

		EOS_LobbyModification_RemoveAttributeOptions LobbyOptions = {  };
		LobbyOptions.ApiVersion = options.ApiVersion;

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyModification_RemoveAttribute(handle, &LobbyOptions));
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyModificationAddMemberAttribute(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationAddMemberAttributeOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyModification_AddMemberAttributeOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.Attribute = &options.Attribute.AttributeData;
		LobbyOptions.Visibility = static_cast<EOS_ELobbyAttributeVisibility>(options.Visibility);

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyModification_AddMemberAttribute(handle, &LobbyOptions));
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyModificationAddMemberAttributeBool(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, bool bValue, EEOSELobbyAttributeVisibility visibility)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbyModificationAddMemberAttributeOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = {  };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;
	LobbyAttributeData.Value.AsBool = bValue;
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;

	Options.Attribute = LobbyAttributeData;
	Options.Visibility = visibility;

	return EOSLobbyModificationAddMemberAttribute(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbyModificationAddMemberAttributeDouble(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbyModificationAddMemberAttributeOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = {  };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;
	LobbyAttributeData.Value.AsDouble = FCString::Atod(*value);
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;

	Options.Attribute = LobbyAttributeData;
	Options.Visibility = visibility;

	return EOSLobbyModificationAddMemberAttribute(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbyModificationAddMemberAttributeInt64(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbyModificationAddMemberAttributeOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = {  };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;
	LobbyAttributeData.Value.AsInt64 = FCString::Atoi64(*value);
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;

	Options.Attribute = LobbyAttributeData;
	Options.Visibility = visibility;

	return EOSLobbyModificationAddMemberAttribute(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbyModificationAddMemberAttributeString(UObject* worldContextObject, const FEOSHLobbyModification& handle, FString key, FString value, EEOSELobbyAttributeVisibility visibility)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);
	char ValueAnsi[512];
	FCStringAnsi::Strncpy(ValueAnsi, TCHAR_TO_UTF8(*value), 512);

	FEOSLobbyModificationAddMemberAttributeOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = {  };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;
	LobbyAttributeData.Value.AsUtf8 = ValueAnsi;
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;

	Options.Attribute = LobbyAttributeData;
	Options.Visibility = visibility;

	return EOSLobbyModificationAddMemberAttribute(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbyModificationRemoveMemberAttribute(UObject* worldContextObject, const FEOSHLobbyModification& handle, FEOSLobbyModificationRemoveMemberAttributeOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		char KeyAnsi[512];
		FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*options.Key), 512);

		EOS_LobbyModification_RemoveMemberAttributeOptions LobbyOptions = {  };
		LobbyOptions.ApiVersion = options.ApiVersion;

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyModification_RemoveMemberAttribute(handle, &LobbyOptions));
	}

	return Result;
}

FEOSProductUserId UCoreLobby::EOSLobbyDetailsGetLobbyOwner(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetLobbyOwnerOptions options)
{
	LogVerbose("");

	FEOSProductUserId ProductUserId;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyDetails_GetLobbyOwnerOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;

		ProductUserId = EOS_LobbyDetails_GetLobbyOwner(handle, &LobbyOptions);
	}

	return ProductUserId;
}

EOSResult UCoreLobby::EOSLobbyDetailsCopyInfo(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyInfoOptions options, FEOSLobbyDetailsInfo& outLobbyDetailsInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyDetails_CopyInfoOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		EOS_LobbyDetails_Info* LobbyDetails = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyDetails_CopyInfo(handle, &LobbyOptions, &LobbyDetails));

		if (Result == EOSResult::EOS_Success)
		{
			outLobbyDetailsInfo = *LobbyDetails;
		}
	}

	return Result;
}

int32 UCoreLobby::EOSLobbyDetailsGetAttributeCount(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetAttributeCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyDetails_GetAttributeCountOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;

		Result = EOS_LobbyDetails_GetAttributeCount(handle, &LobbyOptions);
	}
	return Result;
}

EOSResult UCoreLobby::EOSLobbyDetailsCopyAttributeByIndex(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyAttributeByIndexOptions options, FEOSLobbyAttribute& outAttribute)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyDetails_CopyAttributeByIndexOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.AttrIndex = static_cast<uint32_t>(options.AttrIndex);

		EOS_Lobby_Attribute* LobbyDetails = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyDetails_CopyAttributeByIndex(handle, &LobbyOptions, &LobbyDetails));

		if (Result == EOSResult::EOS_Success)
		{
			outAttribute = *LobbyDetails;
		}
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyDetailsCopyAttributeByKey(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyAttributeByKeyOptions options, FEOSLobbyAttribute& outAttribute)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		char KeyAnsi[512];
		FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*options.AttrKey), 512);

		EOS_LobbyDetails_CopyAttributeByKeyOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.AttrKey = KeyAnsi;

		EOS_Lobby_Attribute* LobbyDetails = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyDetails_CopyAttributeByKey(handle, &LobbyOptions, &LobbyDetails));

		if (Result == EOSResult::EOS_Success)
		{
			outAttribute = *LobbyDetails;
		}
	}

	return Result;
}

int32 UCoreLobby::EOSLobbyDetailsGetMemberCount(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetMemberCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyDetails_GetMemberCountOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;

		Result = EOS_LobbyDetails_GetMemberCount(handle, &LobbyOptions);
	}

	return Result;
}

FEOSProductUserId UCoreLobby::EOSLobbyDetailsGetMemberByIndex(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetMemberByIndexOptions options)
{
	LogVerbose("");

	FEOSProductUserId ProductUserId;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyDetails_GetMemberByIndexOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.MemberIndex = static_cast<uint32_t>(options.MemberIndex);

		ProductUserId = EOS_LobbyDetails_GetMemberByIndex(handle, &LobbyOptions);
	}

	return ProductUserId;
}

int32 UCoreLobby::EOSLobbyDetailsGetMemberAttributeCount(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsGetMemberAttributeCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyDetails_GetMemberAttributeCountOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.TargetUserId = options.TargetUserId;

		Result = EOS_LobbyDetails_GetMemberAttributeCount(handle, &LobbyOptions);
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyDetailsCopyMemberAttributeByIndex(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyMemberAttributeByIndexOptions options, FEOSLobbyAttribute& outAttribute)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbyDetails_CopyMemberAttributeByIndexOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.TargetUserId = options.TargetUserId;
		LobbyOptions.AttrIndex = static_cast<uint32_t>(options.AttrIndex);

		EOS_Lobby_Attribute* LobbyDetails = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyDetails_CopyMemberAttributeByIndex(handle, &LobbyOptions, &LobbyDetails));

		if (Result == EOSResult::EOS_Success)
		{
			outAttribute = *LobbyDetails;
		}
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbyDetailsCopyMemberAttributeByKey(UObject* worldContextObject, const FEOSHLobbyDetails& handle, FEOSLobbyDetailsCopyMemberAttributeByKeyOptions options, FEOSLobbyAttribute& outAttribute)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		char KeyAnsi[512];
		FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*options.AttrKey), 512);

		EOS_LobbyDetails_CopyMemberAttributeByKeyOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.TargetUserId = options.TargetUserId;
		LobbyOptions.AttrKey = KeyAnsi;

		EOS_Lobby_Attribute* LobbyDetails = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbyDetails_CopyMemberAttributeByKey(handle, &LobbyOptions, &LobbyDetails));

		if (Result == EOSResult::EOS_Success)
		{
			outAttribute = *LobbyDetails;
		}
	}

	return Result;
}

void UCoreLobby::EOSLobbySearchFind(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchFindOptions options, const FOnLobbyFindCallback& callback)
{
	LogVerbose("");

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbySearch_FindOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LocalUserId = options.LocalUserId;

		FLobbyFindCallback* CallbackObj = new FLobbyFindCallback({ this, callback });
		EOS_LobbySearch_Find(handle, &LobbyOptions, CallbackObj, Internal_OnLobbyFindCallback);
	}
}

EOSResult UCoreLobby::EOSLobbySearchSetLobbyId(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchSetLobbyIdOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbySearch_SetLobbyIdOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LobbyId = static_cast<const char*>(options.LobbyId);

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbySearch_SetLobbyId(handle, &LobbyOptions));
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbySearchSetTargetUserId(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchSetTargetUserIdOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbySearch_SetTargetUserIdOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.TargetUserId = options.TargetUserId;

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbySearch_SetTargetUserId(handle, &LobbyOptions));
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbySearchSetParameter(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchSetParameterOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbySearch_SetParameterOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.Parameter = &options.Parameter.AttributeData;
		LobbyOptions.ComparisonOp = static_cast<EOS_EComparisonOp>(options.ComparisonOp);

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbySearch_SetParameter(handle, &LobbyOptions));
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbySearchSetParameterBool(UObject* worldContextObject, const FEOSHLobbySearch& handle, FString key, bool bValue, EEOSEComparisonOp comparisonOp)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbySearchSetParameterOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = { };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;
	LobbyAttributeData.Value.AsBool = bValue;
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;

	Options.Parameter = LobbyAttributeData;
	Options.ComparisonOp = comparisonOp;

	return EOSLobbySearchSetParameter(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbySearchSetParameterDouble(UObject* worldContextObject, const FEOSHLobbySearch& handle, FString key, FString value, EEOSEComparisonOp comparisonOp)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbySearchSetParameterOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = {  };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;
	LobbyAttributeData.Value.AsDouble = FCString::Atod(*value);
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;

	Options.Parameter = LobbyAttributeData;
	Options.ComparisonOp = comparisonOp;

	return EOSLobbySearchSetParameter(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbySearchSetParameterInt64(UObject* worldContextObject, const FEOSHLobbySearch& handle, FString key, FString value, EEOSEComparisonOp comparisonOp)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbySearchSetParameterOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = {  };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;
	LobbyAttributeData.Value.AsInt64 = FCString::Atoi64(*value);
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;

	Options.Parameter = LobbyAttributeData;
	Options.ComparisonOp = comparisonOp;

	return EOSLobbySearchSetParameter(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbySearchSetParameterString(UObject* worldContextObject, const FEOSHLobbySearch& handle, FString key, FString value, EEOSEComparisonOp comparisonOp)
{
	LogVerbose("");

	char KeyAnsi[512];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), 512);
	char ValueAnsi[512];
	FCStringAnsi::Strncpy(ValueAnsi, TCHAR_TO_UTF8(*key), 512);

	FEOSLobbySearchSetParameterOptions Options;

	EOS_Lobby_AttributeData LobbyAttributeData = {  };
	LobbyAttributeData.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	LobbyAttributeData.Key = KeyAnsi;
	LobbyAttributeData.Value.AsUtf8 = ValueAnsi;
	LobbyAttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;

	Options.Parameter = LobbyAttributeData;
	Options.ComparisonOp = comparisonOp;

	return EOSLobbySearchSetParameter(worldContextObject, handle, Options);
}

EOSResult UCoreLobby::EOSLobbySearchRemoveParameter(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchRemoveParameterOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		char KeyAnsi[512];
		FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*options.Key), 512);

		EOS_LobbySearch_RemoveParameterOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.Key = KeyAnsi;
		LobbyOptions.ComparisonOp = static_cast<EOS_EComparisonOp>(options.ComparisonOp);

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbySearch_RemoveParameter(handle, &LobbyOptions));
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbySearchSetMaxResults(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchSetMaxResultsOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbySearch_SetMaxResultsOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.MaxResults = static_cast<uint32_t>(options.MaxResults);

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbySearch_SetMaxResults(handle, &LobbyOptions));
	}

	return Result;
}

int32 UCoreLobby::EOSLobbySearchGetSearchResultCount(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchGetSearchResultCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbySearch_GetSearchResultCountOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;

		Result = EOS_LobbySearch_GetSearchResultCount(handle, &LobbyOptions);
	}

	return Result;
}

EOSResult UCoreLobby::EOSLobbySearchCopySearchResultByIndex(UObject* worldContextObject, const FEOSHLobbySearch& handle, FEOSLobbySearchCopySearchResultByIndexOptions options, FEOSHLobbyDetails& outLobbyDetailsHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetLobbyHandle(worldContextObject))
	{
		EOS_LobbySearch_CopySearchResultByIndexOptions LobbyOptions = { };
		LobbyOptions.ApiVersion = options.ApiVersion;
		LobbyOptions.LobbyIndex = static_cast<uint32_t>(options.LobbyIndex);

		EOS_HLobbyDetails LobbyHandle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_LobbySearch_CopySearchResultByIndex(handle, &LobbyOptions, &LobbyHandle));

		if (Result == EOSResult::EOS_Success)
		{
			outLobbyDetailsHandle = LobbyHandle;
		}
	}

	return Result;
}

void UCoreLobby::Internal_OnLobbyCreateLobbyCallback(const EOS_Lobby_CreateLobbyCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FCreateLobbyCallback* CallbackObj = static_cast<FCreateLobbyCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbyDestroyLobbyCallback(const EOS_Lobby_DestroyLobbyCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FDestroyLobbyCallback* CallbackObj = static_cast<FDestroyLobbyCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbyJoinLobbyCallback(const EOS_Lobby_JoinLobbyCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FJoinLobbyCallback* CallbackObj = static_cast<FJoinLobbyCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbyLeaveLobbyCallback(const EOS_Lobby_LeaveLobbyCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLeaveLobbyCallback* CallbackObj = static_cast<FLeaveLobbyCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbyUpdateLobbyCallback(const EOS_Lobby_UpdateLobbyCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FUpdateLobbyCallback* CallbackObj = static_cast<FUpdateLobbyCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbyPromoteMemberCallback(const EOS_Lobby_PromoteMemberCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLobbyPromoteMemberCallback* CallbackObj = static_cast<FLobbyPromoteMemberCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}
	
		delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbyKickMemberCallback(const EOS_Lobby_KickMemberCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLobbyKickMemberCallback* CallbackObj = static_cast<FLobbyKickMemberCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

	delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbySendInviteCallback(const EOS_Lobby_SendInviteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLobbySendInviteCallback* CallbackObj = static_cast<FLobbySendInviteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbyRejectInviteCallback(const EOS_Lobby_RejectInviteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLobbyRejectInviteCallback* CallbackObj = static_cast<FLobbyRejectInviteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

	delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbyQueryInvitesCallback(const EOS_Lobby_QueryInvitesCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLobbyQueryInvitesCallback* CallbackObj = static_cast<FLobbyQueryInvitesCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}
	delete CallbackObj;
	}
}

void UCoreLobby::Internal_OnLobbyFindCallback(const EOS_LobbySearch_FindCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FLobbyFindCallback* CallbackObj = static_cast<FLobbyFindCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreLobby* Lobby = CallbackObj->LobbyObject;
		check(Lobby);

		if (Lobby)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

	delete CallbackObj;
	}


}