/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Sessions/EOSSessions.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSHelpers.h"
#include "Core/EOSCoreLogging.h"

UCoreSessions* UCoreSessions::s_SessionsObject = nullptr;

UCoreSessions::UCoreSessions()
{
	s_SessionsObject = this;
}

EOSResult UCoreSessions::EOSSessionsCreateSessionModification(UObject* worldContextObject, FEOSSessionsCreateSessionModificationOptions options, FEOSHSessionModification& outSessionModificationHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_CreateSessionModificationOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;
		SessionsOptions.BucketId = static_cast<const char*>(options.BucketId);
		SessionsOptions.MaxPlayers = static_cast<uint32_t>(options.MaxPlayers);
		SessionsOptions.LocalUserId = options.LocalUserId;
		SessionsOptions.bPresenceEnabled = options.bPresenceEnabled;
		
		EOS_HSessionModification Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_CreateSessionModification(GetSessionsHandle(worldContextObject), &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionModificationHandle = Handle;
		}
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionsUpdateSessionModification(UObject* worldContextObject, FEOSSessionsUpdateSessionModificationOptions options, FEOSHSessionModification& outSessionModificationHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_UpdateSessionModificationOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;

		EOS_HSessionModification Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_UpdateSessionModification(GetSessionsHandle(worldContextObject), &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionModificationHandle = Handle;
		}
	}

	return Result;
}

void UCoreSessions::EOSSessionsUpdateSession(UObject* worldContextObject, const FEOSSessionsUpdateSessionOptions& options, const FOnSessionUpdateSessionCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_UpdateSessionOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionModificationHandle = options.SessionModificationHandle;

		FUpdateSessionCallback* CallbackObj = new FUpdateSessionCallback({ this, callback });
		EOS_Sessions_UpdateSession(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionsUpdateSessionCallback);
	}
}

void UCoreSessions::EOSSessionsDestroySession(UObject* worldContextObject, FEOSSessionsDestroySessionOptions options, const FOnSessionDestroySessionCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_DestroySessionOptions SessionsOptions = { options.ApiVersion, TCHAR_TO_UTF8(*options.SessionName) };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;

		FDestroySessionCallback* CallbackObj = new FDestroySessionCallback({ this, callback });
		EOS_Sessions_DestroySession(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionsDestroySessionCallback);
	}
}

void UCoreSessions::EOSSessionsJoinSession(UObject* worldContextObject, FEOSSessionsJoinSessionOptions options, const FOnSessionJoinSessionCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_JoinSessionOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;
		SessionsOptions.SessionHandle = options.SessionHandle;
		SessionsOptions.LocalUserId = options.LocalUserId;
		SessionsOptions.bPresenceEnabled = options.bPresenceEnabled;
		
		FJoinSessionCallback* CallbackObj = new FJoinSessionCallback({ this, callback });
		EOS_Sessions_JoinSession(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionsJoinSessionCallback);
	}
}

void UCoreSessions::EOSSessionsStartSession(UObject* worldContextObject, FEOSSessionsStartSessionOptions options, const FOnSessionStartSessionCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_StartSessionOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;

		FStartSessionCallback* CallbackObj = new FStartSessionCallback({ this, callback });
		EOS_Sessions_StartSession(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionsStartSessionCallback);
	}
}

void UCoreSessions::EOSSessionsEndSession(UObject* worldContextObject, FEOSSessionsEndSessionOptions options, const FOnSessionEndSessionCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_EndSessionOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;

		FEndSessionCallback* CallbackObj = new FEndSessionCallback({ this, callback });
		EOS_Sessions_EndSession(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionsEndSessionCallback);
	}
}

void UCoreSessions::EOSSessionsRegisterPlayers(UObject* worldContextObject, FEOSSessionsRegisterPlayersOptions options, const FOnSessionRegisterPlayersCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_RegisterPlayersOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;
		SessionsOptions.PlayersToRegister = new EOS_ProductUserId[options.PlayersToRegister.Num()];
		SessionsOptions.PlayersToRegisterCount = static_cast<uint32_t>(options.PlayersToRegister.Num());

		if (options.PlayersToRegister.Num() > 0)
		{
			for (int32 i = 0; i < options.PlayersToRegister.Num(); i++)
				SessionsOptions.PlayersToRegister[i] = options.PlayersToRegister[i];
		}
		else
		{
			SessionsOptions.PlayersToRegister = NULL;
			SessionsOptions.PlayersToRegisterCount = 0;
		}

		FRegisterPlayersCallback* CallbackObj = new FRegisterPlayersCallback({ this, callback });
		EOS_Sessions_RegisterPlayers(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionRegisterPlayersCallback);

		delete[] SessionsOptions.PlayersToRegister;
	}
}

void UCoreSessions::EOSSessionsUnregisterPlayers(UObject* worldContextObject, FEOSSessionsUnregisterPlayersOptions options, const FOnSessionUnregisterPlayersCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_UnregisterPlayersOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;
		SessionsOptions.PlayersToUnregister = new EOS_ProductUserId[options.PlayersToUnregister.Num()];
		SessionsOptions.PlayersToUnregisterCount = static_cast<uint32_t>(options.PlayersToUnregister.Num());

		if (options.PlayersToUnregister.Num() > 0)
		{
			for (int32 i = 0; i < options.PlayersToUnregister.Num(); i++)
				SessionsOptions.PlayersToUnregister[i] = options.PlayersToUnregister[i];
		}
		else
		{
			SessionsOptions.PlayersToUnregister = NULL;
		}
		FUnregisterPlayersCallback* CallbackObj = new FUnregisterPlayersCallback({ this, callback });
		EOS_Sessions_UnregisterPlayers(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionsUnregisterPlayersCallback);

		delete[] SessionsOptions.PlayersToUnregister;
	}
}

void UCoreSessions::EOSSessionsSendInvite(UObject* worldContextObject, FEOSSessionsSendInviteOptions options, const FOnSessionSessionSendInviteCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_SendInviteOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;
		SessionsOptions.LocalUserId = options.LocalUserId;
		SessionsOptions.TargetUserId = options.TargetUserId;

		FSendInviteCallback* CallbackObj = new FSendInviteCallback({ this, callback });
		EOS_Sessions_SendInvite(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionsSendInviteCallback);
	}
}

void UCoreSessions::EOSSessionsRejectInvite(UObject* worldContextObject, FEOSSessionsRejectInviteOptions options, const FOnSessionRejectInviteCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_RejectInviteOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.LocalUserId = options.LocalUserId;
		SessionsOptions.InviteId = static_cast<const char*>(options.InviteId);

		FRejectInviteCallback* CallbackObj = new FRejectInviteCallback({ this, callback });
		EOS_Sessions_RejectInvite(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionsRejectInviteCallback);
	}
}

void UCoreSessions::EOSSessionsQueryInvites(UObject* worldContextObject, FEOSSessionsQueryInvitesOptions options, const FOnSessionQueryInvitesCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_QueryInvitesOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.LocalUserId = options.LocalUserId;

		FQueryInvitesCallback* CallbackObj = new FQueryInvitesCallback({ this, callback });
		EOS_Sessions_QueryInvites(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, Internal_OnSessionsQueryInvitesCallback);
	}
}

int32 UCoreSessions::EOSSessionsGetInviteCount(UObject* worldContextObject, FEOSSessionsGetInviteCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_GetInviteCountOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.LocalUserId = options.LocalUserId;

		Result = EOS_Sessions_GetInviteCount(GetSessionsHandle(worldContextObject), &SessionsOptions);
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionsGetInviteIdByIndex(UObject* worldContextObject, FEOSSessionsGetInviteIdByIndexOptions options, FString& outBuffer)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_GetInviteIdByIndexOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.LocalUserId = options.LocalUserId;
		SessionsOptions.Index = static_cast<uint32_t>(options.Index);

		char* Buffer = new char[1024];
		int32_t* Size = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_GetInviteIdByIndex(GetSessionsHandle(worldContextObject), &SessionsOptions, Buffer, Size));

		if (Result == EOSResult::EOS_Success)
		{
			outBuffer = Buffer;
		}

		delete[] Buffer;
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionsCreateSessionSearch(UObject* worldContextObject, FEOSSessionsCreateSessionSearchOptions options, FEOSHSessionSearch& outSessionSearchHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_CreateSessionSearchOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.MaxSearchResults = static_cast<uint32_t>(options.MaxSearchResults);

		EOS_HSessionSearch Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_CreateSessionSearch(GetSessionsHandle(worldContextObject), &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionSearchHandle = Handle;
		}
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionsCopyActiveSessionHandle(UObject* worldContextObject, FEOSSessionsCopyActiveSessionHandleOptions options, FEOSHActiveSession& outSessionHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_CopyActiveSessionHandleOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;

		EOS_HActiveSession Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_CopyActiveSessionHandle(GetSessionsHandle(worldContextObject), &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionHandle = Handle;
		}
	}

	return Result;
}

struct FSessionInviteReceivedCallback
{
public:
	FSessionInviteReceivedCallback(UObject* worldContextObject, const FOnSessionInviteReceivedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FSessionInviteReceivedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnSessionInviteReceivedCallback Callback;
};

static TMap<FEOSNotificationId, FSessionInviteReceivedCallback*> s_SessionInviteReceivedCallbacks;

FEOSNotificationId UCoreSessions::EOSSessionsAddNotifySessionInviteReceived(UObject* worldContextObject, const FOnSessionInviteReceivedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId NotificationId;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_AddNotifySessionInviteReceivedOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = EOS_SESSIONS_ADDNOTIFYSESSIONINVITERECEIVED_API_LATEST;

		FSessionInviteReceivedCallback* CallbackObj = new FSessionInviteReceivedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		NotificationId = EOS_Sessions_AddNotifySessionInviteReceived(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, ([](const EOS_Sessions_SessionInviteReceivedCallbackInfo* data)
		{
			FSessionInviteReceivedCallback* CallbackData = static_cast<FSessionInviteReceivedCallback*>(data->ClientData);
			CallbackData->Callback.ExecuteIfBound(*data);
		}));
		s_SessionInviteReceivedCallbacks.Add(NotificationId, CallbackObj);
	}

	return NotificationId;
}

void UCoreSessions::EOSSessionsRemoveNotifySessionInviteReceived(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_RemoveNotifySessionInviteReceived(GetSessionsHandle(worldContextObject), id);
		if (FSessionInviteReceivedCallback** CallbackObj = s_SessionInviteReceivedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_SessionInviteReceivedCallbacks.Remove(id);
	}
}

struct FSessionInviteAcceptedCallback
{
public:
	FSessionInviteAcceptedCallback(UObject* worldContextObject, const FOnSessionInviteAcceptedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FSessionInviteAcceptedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnSessionInviteAcceptedCallback Callback;
};

static TMap<FEOSNotificationId, FSessionInviteAcceptedCallback*> s_SessionInviteAcceptedCallbacks;

FEOSNotificationId UCoreSessions::EOSSessionsAddNotifySessionInviteAccepted(UObject* worldContextObject, const FOnSessionInviteAcceptedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId NotificationId;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_AddNotifySessionInviteAcceptedOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = EOS_SESSIONS_ADDNOTIFYSESSIONINVITEACCEPTED_API_LATEST;

		FSessionInviteAcceptedCallback* CallbackObj = new FSessionInviteAcceptedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		NotificationId = EOS_Sessions_AddNotifySessionInviteAccepted(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, ([](const EOS_Sessions_SessionInviteAcceptedCallbackInfo* data)
		{
			FSessionInviteAcceptedCallback* CallbackData = static_cast<FSessionInviteAcceptedCallback*>(data->ClientData);
			CallbackData->Callback.ExecuteIfBound(*data);
		}));
		s_SessionInviteAcceptedCallbacks.Add(NotificationId, CallbackObj);
	}

	return NotificationId;
}

void UCoreSessions::EOSSessionsRemoveNotifySessionInviteAccepted(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_RemoveNotifySessionInviteAccepted(GetSessionsHandle(worldContextObject), id);
		if (FSessionInviteAcceptedCallback** CallbackObj = s_SessionInviteAcceptedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_SessionInviteAcceptedCallbacks.Remove(id);
	}
}

struct FSessionJoinSessionAcceptedCallback
{
public:
	FSessionJoinSessionAcceptedCallback(UObject* worldContextObject, const FOnSessionJoinSessionAcceptedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FSessionJoinSessionAcceptedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnSessionJoinSessionAcceptedCallback Callback;
};

static TMap<FEOSNotificationId, FSessionJoinSessionAcceptedCallback*> s_SessionJoinSessionAcceptedCallbacks;

FEOSNotificationId UCoreSessions::EOSSessionsAddNotifyJoinSessionAccepted(UObject* worldContextObject, const FOnSessionJoinSessionAcceptedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId NotificationId;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_AddNotifyJoinSessionAcceptedOptions SessionsOptions = { EOS_SESSIONS_ADDNOTIFYJOINSESSIONACCEPTED_API_LATEST };
		SessionsOptions.ApiVersion = EOS_SESSIONS_ADDNOTIFYJOINSESSIONACCEPTED_API_LATEST;

		FSessionJoinSessionAcceptedCallback* CallbackObj = new FSessionJoinSessionAcceptedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		NotificationId = EOS_Sessions_AddNotifyJoinSessionAccepted(GetSessionsHandle(worldContextObject), &SessionsOptions, CallbackObj, ([](const EOS_Sessions_JoinSessionAcceptedCallbackInfo* data)
		{
			FSessionJoinSessionAcceptedCallback* CallbackData = static_cast<FSessionJoinSessionAcceptedCallback*>(data->ClientData);
			CallbackData->Callback.ExecuteIfBound(*data);
		}));
		s_SessionJoinSessionAcceptedCallbacks.Add(NotificationId, CallbackObj);
	}

	return NotificationId;
}


void UCoreSessions::EOSSessionsRemoveNotifyJoinSessionAccepted(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_RemoveNotifySessionInviteAccepted(GetSessionsHandle(worldContextObject), id);
		if (FSessionJoinSessionAcceptedCallback** CallbackObj = s_SessionJoinSessionAcceptedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_SessionJoinSessionAcceptedCallbacks.Remove(id);
	}
}

EOSResult UCoreSessions::EOSSessionsCopySessionHandleByInviteId(UObject* worldContextObject, FEOSSessionsCopySessionHandleByInviteIdOptions options, FEOSHSessionDetails& outSessionHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_CopySessionHandleByInviteIdOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.InviteId = static_cast<const char*>(options.InviteId);

		EOS_HSessionDetails Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_CopySessionHandleByInviteId(GetSessionsHandle(worldContextObject), &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionHandle = Handle;
			EOS_SessionDetails_Release(Handle);
		}
	}

	return Result;
}


EOSResult UCoreSessions::EOSSessionsCopySessionHandleByUiEventId(UObject* worldContextObject, FEOSSessionsCopySessionHandleByUiEventIdOptions options, FEOSHSessionDetails& outSessionHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_CopySessionHandleByUiEventIdOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.UiEventId = options.UiEventId;

		EOS_HSessionDetails Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_CopySessionHandleByUiEventId(GetSessionsHandle(worldContextObject), &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionHandle = Handle;
			EOS_SessionDetails_Release(Handle);
		}
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionsCopySessionHandleForPresence(UObject* worldContextObject, FEOSSessionsCopySessionHandleForPresenceOptions options, FEOSHSessionDetails& outSessionHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_Sessions_CopySessionHandleForPresenceOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.LocalUserId = options.LocalUserId;

		EOS_HSessionDetails Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_CopySessionHandleForPresence(GetSessionsHandle(worldContextObject), &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionHandle = Handle;
			EOS_SessionDetails_Release(Handle);
		}
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionsIsUserInSession(UObject* worldContextObject, FEOSSessionsIsUserInSessionOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_IsUserInSessionOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;
		SessionsOptions.TargetUserId = options.TargetUserId;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_IsUserInSession(GetSessionsHandle(worldContextObject), &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionsDumpSessionState(UObject* worldContextObject, FEOSSessionsDumpSessionStateOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionNameAnsi[64];
		FCStringAnsi::Strncpy(SessionNameAnsi, TCHAR_TO_UTF8(*options.SessionName), 64);

		EOS_Sessions_DumpSessionStateOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionName = SessionNameAnsi;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Sessions_DumpSessionState(GetSessionsHandle(worldContextObject), &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionModificationSetBucketId(UObject* worldContextObject, const FEOSHSessionModification& handle, FEOSSessionModificationSetBucketIdOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionModification_SetBucketIdOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.BucketId = static_cast<const char*>(options.BucketId);

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionModification_SetBucketId(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionModificationSetHostAddress(UObject* worldContextObject, const FEOSHSessionModification& handle, FEOSSessionModificationSetHostAddressOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char HostAddressAnsi[128];
		FCStringAnsi::Strncpy(HostAddressAnsi, TCHAR_TO_UTF8(*options.HostAddress), (options.HostAddress.Len() + 1));

		EOS_SessionModification_SetHostAddressOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.HostAddress = HostAddressAnsi;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionModification_SetHostAddress(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionModificationSetPermissionLevel(UObject* worldContextObject, const FEOSHSessionModification& handle, FEOSSessionModificationSetPermissionLevelOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionModification_SetPermissionLevelOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.PermissionLevel = static_cast<EOS_EOnlineSessionPermissionLevel>(options.PermissionLevel);

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionModification_SetPermissionLevel(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionModificationSetJoinInProgressAllowed(UObject* worldContextObject, const FEOSHSessionModification& handle, FEOSSessionModificationSetJoinInProgressAllowedOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionModification_SetJoinInProgressAllowedOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.bAllowJoinInProgress = options.bAllowJoinInProgress;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionModification_SetJoinInProgressAllowed(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionModificationSetMaxPlayers(UObject* worldContextObject, const FEOSHSessionModification& handle, FEOSSessionModificationSetMaxPlayersOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionModification_SetMaxPlayersOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.MaxPlayers = static_cast<uint32_t>(options.MaxPlayers);

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionModification_SetMaxPlayers(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionModificationSetInvitesAllowed(UObject* worldContextObject, const FEOSHSessionModification& handle, FEOSSessionModificationSetInvitesAllowedOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionModification_SetInvitesAllowedOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.bInvitesAllowed = options.bInvitesAllowed;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionModification_SetInvitesAllowed(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionModificationAddAttribute(UObject* worldContextObject, const FEOSHSessionModification& handle, const FEOSSessionModificationAddAttributeOptions& options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionModification_AddAttributeOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionAttribute = &options.Data.AttributeData;
		SessionsOptions.AdvertisementType = static_cast<EOS_ESessionAttributeAdvertisementType>(options.AdvertisementType);

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionModification_AddAttribute(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionModificationAddAttributeBool(UObject* worldContextObject, const FEOSHSessionModification& handle, EEOSESessionAttributeAdvertisementType advertisementType, const FString& key, bool bValue)
{
	LogVerbose("");

	FEOSSessionModificationAddAttributeOptions SessionsOptions;

	SessionsOptions.AdvertisementType = advertisementType;

	char KeyAnsi[64];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), (key.Len() + 1));
	EOS_Sessions_AttributeData AttributeData = { };
	AttributeData.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
	AttributeData.Key = KeyAnsi;
	AttributeData.Value.AsBool = bValue;
	AttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;

	SessionsOptions.Data.AttributeData = AttributeData;

	return EOSSessionModificationAddAttribute(worldContextObject, handle, SessionsOptions);
}

EOSResult UCoreSessions::EOSSessionModificationAddAttributeDouble(UObject* worldContextObject, const FEOSHSessionModification& handle, EEOSESessionAttributeAdvertisementType advertisementType, const FString& key, const FString& value)
{
	LogVerbose("");

	FEOSSessionModificationAddAttributeOptions SessionsOptions;

	SessionsOptions.AdvertisementType = advertisementType;

	char KeyAnsi[64];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), (key.Len() + 1));
	EOS_Sessions_AttributeData AttributeData = { };
	AttributeData.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
	AttributeData.Key = KeyAnsi;
	AttributeData.Value.AsDouble = FCString::Atod(*value);
	AttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;

	SessionsOptions.Data.AttributeData = AttributeData;

	return EOSSessionModificationAddAttribute(worldContextObject, handle, SessionsOptions);
}

EOSResult UCoreSessions::EOSSessionModificationAddAttributeInt64(UObject* worldContextObject, const FEOSHSessionModification& handle, EEOSESessionAttributeAdvertisementType advertisementType, const FString& key, const FString& value)
{
	LogVerbose("");

	FEOSSessionModificationAddAttributeOptions SessionsOptions;

	SessionsOptions.AdvertisementType = advertisementType;

	char KeyAnsi[64];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), (key.Len() + 1));
	EOS_Sessions_AttributeData AttributeData = { };
	AttributeData.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
	AttributeData.Key = KeyAnsi;
	AttributeData.Value.AsInt64 = FCString::Atoi64(*value);
	AttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;

	SessionsOptions.Data.AttributeData = AttributeData;

	return EOSSessionModificationAddAttribute(worldContextObject, handle, SessionsOptions);
}

EOSResult UCoreSessions::EOSSessionModificationAddAttributeString(UObject* worldContextObject, const FEOSHSessionModification& handle, EEOSESessionAttributeAdvertisementType advertisementType, const FString& key, const FString& value)
{
	LogVerbose("");

	FEOSSessionModificationAddAttributeOptions SessionsOptions;

	SessionsOptions.AdvertisementType = advertisementType;

	char KeyAnsi[64];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), (key.Len() + 1));
	char ValueAnsi[128];
	FCStringAnsi::Strncpy(ValueAnsi, TCHAR_TO_UTF8(*value), (value.Len() + 1));

	EOS_Sessions_AttributeData AttributeData = { };
	AttributeData.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
	AttributeData.Key = KeyAnsi;
	AttributeData.Value.AsUtf8 = ValueAnsi;
	AttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;

	SessionsOptions.Data.AttributeData = AttributeData;

	return EOSSessionModificationAddAttribute(worldContextObject, handle, SessionsOptions);
}

EOSResult UCoreSessions::EOSSessionModificationRemoveAttribute(UObject* worldContextObject, const FEOSHSessionModification& handle, const FEOSSessionModificationRemoveAttributeOptions& options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char KeyAnsi[64];
		FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*options.Key), (options.Key.Len() + 1));

		EOS_SessionModification_RemoveAttributeOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.Key = KeyAnsi;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionModification_RemoveAttribute(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSActiveSessionCopyInfo(UObject* worldContextObject, const FEOSHActiveSession& handle, FEOSActiveSessionCopyInfoOptions options, FEOSActiveSessionInfo& outActiveSessionInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_ActiveSession_CopyInfoOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;

		EOS_ActiveSession_Info* Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_ActiveSession_CopyInfo(handle, &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outActiveSessionInfo = *Handle;
			EOS_ActiveSession_Info_Release(Handle);
		}
	}

	return Result;
}

int32 UCoreSessions::EOSActiveSessionGetRegisteredPlayerCount(UObject* worldContextObject, const FEOSHActiveSession& handle, FEOSActiveSessionGetRegisteredPlayerCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_ActiveSession_GetRegisteredPlayerCountOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;

		Result = EOS_ActiveSession_GetRegisteredPlayerCount(handle, &SessionsOptions);
	}

	return Result;
}

FEOSProductUserId UCoreSessions::EOSActiveSessionGetRegisteredPlayerByIndex(UObject* worldContextObject, const FEOSHActiveSession& handle, FEOSActiveSessionGetRegisteredPlayerByIndexOptions options)
{
	LogVerbose("");

	FEOSProductUserId NotificationId;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_ActiveSession_GetRegisteredPlayerByIndexOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.PlayerIndex = static_cast<uint32_t>(options.PlayerIndex);

		NotificationId = EOS_ActiveSession_GetRegisteredPlayerByIndex(handle, &SessionsOptions);
	}

	return NotificationId;
}

EOSResult UCoreSessions::EOSSessionDetailsCopyInfo(UObject* worldContextObject, const FEOSHSessionDetails& handle, FEOSSessionDetailsCopyInfoOptions options, FEOSSessionDetailsInfo& outSessionInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionDetails_CopyInfoOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;

		EOS_SessionDetails_Info* Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionDetails_CopyInfo(handle, &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionInfo = *Handle;
			EOS_SessionDetails_Info_Release(Handle);
		}
	}

	return Result;
}

int32 UCoreSessions::EOSSessionDetailsGetSessionAttributeCount(UObject* worldContextObject, const FEOSHSessionDetails& handle, FEOSSessionDetailsGetSessionAttributeCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionDetails_GetSessionAttributeCountOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;

		Result = EOS_SessionDetails_GetSessionAttributeCount(handle, &SessionsOptions);
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionDetailsCopySessionAttributeByIndex(UObject* worldContextObject, const FEOSHSessionDetails& handle, FEOSSessionDetailsCopySessionAttributeByIndexOptions options, FEOSSessionDetailsAttribute& outSessionAttribute)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionDetails_CopySessionAttributeByIndexOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.AttrIndex = static_cast<uint32_t>(options.AttrIndex);

		EOS_SessionDetails_Attribute* Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionDetails_CopySessionAttributeByIndex(handle, &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionAttribute = *Handle;
			EOS_SessionDetails_Attribute_Release(Handle);
		}
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionDetailsCopySessionAttributeByKey(UObject* worldContextObject, const FEOSHSessionDetails& handle, FEOSSessionDetailsCopySessionAttributeByKeyOptions options, FEOSSessionDetailsAttribute& outSessionAttribute)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char KeyAnsi[64];
		FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*options.AttrKey), (options.AttrKey.Len() + 1));

		EOS_SessionDetails_CopySessionAttributeByKeyOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.AttrKey = KeyAnsi;

		EOS_SessionDetails_Attribute* Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionDetails_CopySessionAttributeByKey(handle, &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionAttribute = *Handle;
		}
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionSearchSetSessionId(UObject* worldContextObject, const FEOSHSessionSearch& handle, FEOSSessionSearchSetSessionIdOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char SessionIdAnsi[64];
		FCStringAnsi::Strncpy(SessionIdAnsi, TCHAR_TO_UTF8(*options.SessionId), (options.SessionId.Len() + 1));

		EOS_SessionSearch_SetSessionIdOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionId = SessionIdAnsi;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionSearch_SetSessionId(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionSearchSetTargetUserId(UObject* worldContextObject, const FEOSHSessionSearch& handle, FEOSSessionSearchSetTargetUserIdOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionSearch_SetTargetUserIdOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.TargetUserId = options.TargetUserId;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionSearch_SetTargetUserId(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionSearchSetParameter(UObject* worldContextObject, const FEOSHSessionSearch& handle, const FEOSSessionSearchSetParameterOptions& options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionSearch_SetParameterOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.Parameter = &options.Parameter.AttributeData;
		SessionsOptions.ComparisonOp = static_cast<EOS_EOnlineComparisonOp>(options.ComparisonOp);

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionSearch_SetParameter(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionSearchSetParameterBool(UObject* worldContextObject, const FEOSHSessionSearch& handle, FString key, bool bValue, EEOSEComparisonOp comparisonOp)
{
	LogVerbose("");

	FEOSSessionSearchSetParameterOptions SessionsOptions;

	char KeyAnsi[64];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), (key.Len() + 1));

	EOS_Sessions_AttributeData AttributeData = { };
	AttributeData.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
	AttributeData.Key = KeyAnsi;
	AttributeData.Value.AsBool = bValue;
	AttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_BOOLEAN;

	SessionsOptions.ComparisonOp = comparisonOp;
	SessionsOptions.Parameter.AttributeData = AttributeData;

	return EOSSessionSearchSetParameter(worldContextObject, handle, SessionsOptions);
}

EOSResult UCoreSessions::EOSSessionSearchSetParameterDouble(UObject* worldContextObject, const FEOSHSessionSearch& handle, FString key, FString value, EEOSEComparisonOp comparisonOp)
{
	LogVerbose("");

	FEOSSessionSearchSetParameterOptions SessionsOptions;

	char KeyAnsi[64];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), (key.Len() + 1));

	EOS_Sessions_AttributeData AttributeData = { };
	AttributeData.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
	AttributeData.Key = KeyAnsi;
	AttributeData.Value.AsDouble = FCString::Atod(*value);
	AttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_DOUBLE;

	SessionsOptions.ComparisonOp = comparisonOp;
	SessionsOptions.Parameter.AttributeData = AttributeData;

	return EOSSessionSearchSetParameter(worldContextObject, handle, SessionsOptions);
}

EOSResult UCoreSessions::EOSSessionSearchSetParameterInt64(UObject* worldContextObject, const FEOSHSessionSearch& handle, FString key, FString value, EEOSEComparisonOp comparisonOp)
{
	LogVerbose("");

	FEOSSessionSearchSetParameterOptions SessionsOptions;

	char KeyAnsi[64];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), (key.Len() + 1));

	EOS_Sessions_AttributeData AttributeData = { };
	AttributeData.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
	AttributeData.Key = KeyAnsi;
	AttributeData.Value.AsInt64 = FCString::Atoi64(*value);
	AttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_INT64;

	SessionsOptions.ComparisonOp = comparisonOp;
	SessionsOptions.Parameter.AttributeData = AttributeData;

	return EOSSessionSearchSetParameter(worldContextObject, handle, SessionsOptions);
}

EOSResult UCoreSessions::EOSSessionSearchSetParameterString(UObject* worldContextObject, const FEOSHSessionSearch& handle, FString key, FString value, EEOSEComparisonOp comparisonOp)
{
	LogVerbose("");

	FEOSSessionSearchSetParameterOptions SessionsOptions;

	char KeyAnsi[64];
	FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*key), (key.Len() + 1));
	char ValueAnsi[128];
	FCStringAnsi::Strncpy(ValueAnsi, TCHAR_TO_UTF8(*value), (value.Len() + 1));

	EOS_Sessions_AttributeData AttributeData = { };
	AttributeData.ApiVersion = EOS_SESSIONS_SESSIONATTRIBUTEDATA_API_LATEST;
	AttributeData.Key = KeyAnsi;
	AttributeData.Value.AsUtf8 = ValueAnsi;
	AttributeData.ValueType = EOS_ESessionAttributeType::EOS_AT_STRING;

	SessionsOptions.ComparisonOp = comparisonOp;
	SessionsOptions.Parameter.AttributeData = AttributeData;

	return EOSSessionSearchSetParameter(worldContextObject, handle, SessionsOptions);
}

EOSResult UCoreSessions::EOSSessionSearchRemoveParameter(UObject* worldContextObject, const FEOSHSessionSearch& handle, FEOSSessionSearchRemoveParameterOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		char KeyAnsi[64];
		FCStringAnsi::Strncpy(KeyAnsi, TCHAR_TO_UTF8(*options.Key), (options.Key.Len() + 1));

		EOS_SessionSearch_RemoveParameterOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.Key = KeyAnsi;
		SessionsOptions.ComparisonOp = static_cast<EOS_EOnlineComparisonOp>(options.ComparisonOp);

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionSearch_RemoveParameter(handle, &SessionsOptions));
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionSearchSetMaxResults(UObject* worldContextObject, const FEOSHSessionSearch& handle, FEOSSessionSearchSetMaxResultsOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionSearch_SetMaxResultsOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.MaxSearchResults = static_cast<uint32_t>(options.MaxSearchResults);

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionSearch_SetMaxResults(handle, &SessionsOptions));
	}

	return Result;
}

void UCoreSessions::EOSSessionSearchFind(UObject* worldContextObject, const FEOSHSessionSearch& handle, FEOSSessionSearchFindOptions options, const FOnSessionFindCallback& callback)
{
	LogVerbose("");

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionSearch_FindOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.LocalUserId = options.LocalUserId;

		FSessionFindCallback* CallbackObj = new FSessionFindCallback({ this, callback });
		EOS_SessionSearch_Find(handle, &SessionsOptions, CallbackObj, Internal_OnSessionsFindCallback);
	}
}

int32 UCoreSessions::EOSSessionSearchGetSearchResultCount(UObject* worldContextObject, const FEOSHSessionSearch& handle, FEOSSessionSearchGetSearchResultCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionSearch_GetSearchResultCountOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;

		Result = EOS_SessionSearch_GetSearchResultCount(handle, &SessionsOptions);
	}

	return Result;
}

EOSResult UCoreSessions::EOSSessionSearchCopySearchResultByIndex(UObject* worldContextObject, const FEOSHSessionSearch& handle, const FEOSSessionSearchCopySearchResultByIndexOptions& options, FEOSHSessionDetails& outSessionHandle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetSessionsHandle(worldContextObject))
	{
		EOS_SessionSearch_CopySearchResultByIndexOptions SessionsOptions = { };
		SessionsOptions.ApiVersion = options.ApiVersion;
		SessionsOptions.SessionIndex = static_cast<uint32_t>(options.SessionIndex);

		EOS_HSessionDetails Handle = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_SessionSearch_CopySearchResultByIndex(handle, &SessionsOptions, &Handle));

		if (Result == EOSResult::EOS_Success)
		{
			outSessionHandle = Handle;
		}
	}

	return Result;
}

void UCoreSessions::EOSSessionModificationRelease(UObject* worldContextObject, const FEOSHSessionModification& handle)
{
	LogVerbose("");

	EOS_SessionModification_Release(handle);
}

void UCoreSessions::EOSActiveSessionRelease(UObject* worldContextObject, const FEOSHActiveSession& handle)
{
	LogVerbose("");

	EOS_ActiveSession_Release(handle);
}

void UCoreSessions::EOSSessionDetailsRelease(UObject* worldContextObject, const FEOSHSessionDetails& handle)
{
	LogVerbose("");

	EOS_SessionDetails_Release(handle);
}

void UCoreSessions::EOSSessionSearchRelease(UObject* worldContextObject, const FEOSHSessionSearch& handle)
{
	LogVerbose("");

	EOS_SessionSearch_Release(handle);
}

void UCoreSessions::Internal_OnSessionsUpdateSessionCallback(const EOS_Sessions_UpdateSessionCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FUpdateSessionCallback* CallbackObj = static_cast<FUpdateSessionCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionsDestroySessionCallback(const EOS_Sessions_DestroySessionCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FDestroySessionCallback* CallbackObj = static_cast<FDestroySessionCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionsJoinSessionCallback(const EOS_Sessions_JoinSessionCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FJoinSessionCallback* CallbackObj = static_cast<FJoinSessionCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionsStartSessionCallback(const EOS_Sessions_StartSessionCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FStartSessionCallback* CallbackObj = static_cast<FStartSessionCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionsEndSessionCallback(const EOS_Sessions_EndSessionCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FEndSessionCallback* CallbackObj = static_cast<FEndSessionCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionRegisterPlayersCallback(const EOS_Sessions_RegisterPlayersCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FRegisterPlayersCallback* CallbackObj = static_cast<FRegisterPlayersCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionsUnregisterPlayersCallback(const EOS_Sessions_UnregisterPlayersCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FUnregisterPlayersCallback* CallbackObj = static_cast<FUnregisterPlayersCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionsSendInviteCallback(const EOS_Sessions_SendInviteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FSendInviteCallback* CallbackObj = static_cast<FSendInviteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionsRejectInviteCallback(const EOS_Sessions_RejectInviteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FRejectInviteCallback* CallbackObj = static_cast<FRejectInviteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionsQueryInvitesCallback(const EOS_Sessions_QueryInvitesCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryInvitesCallback* CallbackObj = static_cast<FQueryInvitesCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreSessions::Internal_OnSessionsFindCallback(const EOS_SessionSearch_FindCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FSessionFindCallback* CallbackObj = static_cast<FSessionFindCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreSessions* Sessions = CallbackObj->SessionsObject;
		check(Sessions);

		if (Sessions)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

	delete CallbackObj;
	}
}